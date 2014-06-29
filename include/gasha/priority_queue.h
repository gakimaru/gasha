#pragma once
#ifndef __PRIORITY_QUEUE_H_
#define __PRIORITY_QUEUE_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// priority_queue.h
// 優先度付きキューコンテナ【宣言部】※データのメモリ管理を行わない擬似コンテナ
//
// ※コンテナをインスタンス化する際は、別途下記のファイルをインクルードする必要あり
//
//   ・priority_queue.inl   ... 【インライン関数／テンプレート関数定義部】
//                            コンテナクラスの操作が必要な場所でインクルード。
//   ・priority_queue.cpp.h ... 【関数定義部】
//                            コンテナクラスの実体化が必要な場所でインクルード。
//
// ※面倒なら三つまとめてインクルードして使用しても良いが、分けた方が、
// 　コンパイル・リンク時間の短縮、および、クラス修正時の影響範囲の抑制になる。
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dummy_lock.h>//ダミーロック
#include <gasha/lock_guard.h>//スコープロック
#include <gasha/unique_lock.h>//単一ロック

#include <cstdint>//std::int32_t, std::uint32_t

#include <gasha/binary_heap.h>//二分ヒープコンテナ【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//データ構造とアルゴリズム
//--------------------------------------------------------------------------------
//【特徴】
//・ヒープ系コンテナを活用し、エンキュー時の順序性を保証する優先度付きキュー。
//--------------------------------------------------------------------------------
//【利点】
//・優先度が高いデータの探索がほぼO(1)で行える。
//・優先度が同じキューは、デキュー時にエンキュー時の順序性を保証する。
//--------------------------------------------------------------------------------
//【欠点】
//・エンキュー／デキュー以外の操作ができない。
//・各要素にはシーケンス番頭と優先度を保持する必要がある。（メモリオーバーヘッドがある）
//--------------------------------------------------------------------------------
//【本プログラムにおける実装要件】
//・ヒープ系コンテナを内包したコンテナアダプタとする。
//・ノードの優先度情報とシーケンス番号をユーザー定義可能とする。
//　コンテナアダプタ操作用のユーザー定義構造体に、優先度情報とシーケンス番号への
//　アクセスメソッドを実装することを必要とする。
//・STLの std::priority_qeueu と異なり、優先度付きキューに特化した独自の
//　インターフェースを実装。
//・デキュー時にキューを取り出さず、優先度を変更することに対応。
//・（他のコンテナと同様に）コンテナ操作対象・方法を設定した
//　構造体をユーザー定義して用いる。
//・エンキュー時に値（キュー）のコピーをせずに、直接コンテナのコンストラクタを
//　呼び出して初期化可能とする。（処理時間を効率化する）
//--------------------------------------------------------------------------------
//【想定する具的的な用途】
//・エンキュー時の順序性を保証する優先度付きキュー。
//--------------------------------------------------------------------------------

namespace priority_queue
{
	//--------------------
	//優先度付きキュー操作用テンプレート構造体
	//※CRTPを活用し、下記のような派生構造体を作成して使用する
	//  //struct 派生構造体名 : public priority_queue::baseOpe<派生構造体名, 要素の型, 優先度の型, シーケンス番号の型>
	//	struct ope : public priority_queue::baseOpe<ope, data_t, int, unsigned int>
	//	{
	//		//優先度を取得
	//		inline static priority_type getPriority(const node_type& node){ return node.m_priority; }
	//		//優先度を更新
	//		inline static void setPriority(node_type& node, const priority_type priority){ node.m_priority = priority; }
	//		
	//		//シーケンス番号を取得
	//		inline static seq_no_type getSeqNo(const node_type& node){ return node.m_seqNo; }
	//		//シーケンス番号を更新
	//		inline static void setSeqNo(node_type& node, const seq_no_type seq_no){ node.m_seqNo = seq_no; }
	//		
	//		//優先度を比較 ※必要に応じて定義
	//		inline static int compareProior(const priority_type lhs, const priority_type rhs){ return ???; }
	//		
	//		//ロック型 ※必要に応じて定義
	//		//※ロックでコンテナ操作をスレッドセーフにしたい場合は、
	//		//　有効なロック型（spin_lockなど）を lock_type 型として定義する。
	//		typedef spin_lock lock_type;//ロックオブジェクト型
	//	};
	template<class OPE_TYPE, typename NODE_TYPE, typename PRIORITY_TYPE = std::int32_t, typename SEQ_NO_TYPE = std::uint32_t>
	struct baseOpe
	{
		//型
		typedef OPE_TYPE ope_type;//ノード操作型
		typedef NODE_TYPE node_type;//ノード型
		typedef PRIORITY_TYPE priority_type;//優先度型
		typedef SEQ_NO_TYPE seq_no_type;//シーケンス番号型
		
		//ロック型
		typedef dummyLock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※ロックでコンテナ操作をスレッドセーフにしたい場合は、
		//　baseOpeの派生クラスにて、有効なロック型（spinLock など）を
		//　lock_type 型として再定義する。
		//【補足】コンテナには、あらかじめロック制御のための仕組みがソースコードレベルで
		//　　　　仕込んであるが、有効な型を与えない限りは、実行時のオーバーヘッドは一切ない。

		//優先度を比較
		//※デフォルト
		//Return value:
		//  0     ... lhs == rhs
		//  1以上 ... lhs > rhs ※正順
		// -1以下 ... lhs < rhs ※逆順
		inline static int comparePriority(const priority_type lhs, const priority_type rhs)
		{
			return static_cast<int>(lhs) - static_cast<int>(rhs);
		}

		//シーケンス番号を比較
		//※デフォルト
		//※lhsの方が小さいければ true を返す
		inline static bool lessSeqNo(const seq_no_type lhs, const seq_no_type rhs)
		{
			return lhs < rhs;
		}

		//優先度とシーケンス番号を比較する
		//※lhsの方が優先度が高ければ true を返す
		//※優先度が同じなら、シー件番号が小さければ true を返す
		inline static bool lessPriorityAndSeqNo(const int compare_priority, const seq_no_type lhs, const seq_no_type rhs)
		{
			return compare_priority > 0 ? true : compare_priority == 0 ? ope_type::lessSeqNo(lhs, rhs) : false;
		}

		//ノード比較用プレディケート関数オブジェクト
		//※trueで並び順が正しい
		struct less{
			inline bool operator()(const node_type& lhs, const node_type& rhs) const
			{
				return lessPriorityAndSeqNo(ope_type::comparePriority(ope_type::getPriority(lhs), ope_type::getPriority(rhs)), ope_type::getSeqNo(lhs), ope_type::getSeqNo(rhs));
			}
		};

		//シーケンス番号をリセット
		//※リセット後の値を返す
		//※デフォルト
		//※必要に応じて派生クラスで処理を変更する
		inline static SEQ_NO_TYPE resetSeqNo(SEQ_NO_TYPE& seq_no)
		{
			seq_no = 0;
			return seq_no;
		}

		//シーケンス番号をカウントアップ
		//※カウントアップ前の値を返す
		//※デフォルト
		//※必要に応じて派生クラスで処理を変更する
		inline static SEQ_NO_TYPE countupSeqNo(SEQ_NO_TYPE& seq_no)
		{
			return seq_no++;
		}

		//デストラクタ呼び出し
		static void callDestructor(node_type* obj){ obj->~NODE_TYPE(); }

		//コンテナ操作型
		struct container_ope_type
		{
			typedef OPE_TYPE adapter_ope_type;//コンテナアダプタのノード操作型
			typedef container_ope_type ope_type;//ノード操作型
			typedef NODE_TYPE node_type;//ノード型

			//ノード比較用プレディケート関数オブジェクト
			//※trueでlhsの方が小さい（並び順が正しい）
			struct less{
				inline bool operator()(const node_type& lhs, const node_type& rhs) const
				{
					return adapter_ope_type::less()(lhs, rhs);
				}
			};
			
			//デストラクタ呼び出し
			static void callDestructor(node_type* obj){ obj->~NODE_TYPE(); }
			
			//ロック型
			typedef dummyLock lock_type;//ロックオブジェクト型
			//※コンテナ側ではロック制御しない
		};
	};
	
	//--------------------
	//基本型定義マクロ
	#define DECLARE_OPE_TYPES(OPE_TYPE) \
		typedef OPE_TYPE ope_type; \
		typedef typename ope_type::node_type node_type; \
		typedef node_type value_type; \
		typedef value_type& reference; \
		typedef const value_type& const_reference; \
		typedef value_type* pointer; \
		typedef const value_type* const_pointer; \
		typedef std::size_t size_type; \
		typedef typename ope_type::priority_type priority_type; \
		typedef typename ope_type::seq_no_type seq_no_type; \
		typedef typename ope_type::lock_type lock_type;
	
	//----------------------------------------
	//プライオリティキューコンテナアダプタ
	//※コンテナのデフォルトは二分ヒープ（binary_heap::container）。
	//　同じインターフェースを持ったクラスなら、置き換えて使用可能。
	//※std::priority_queueとはあまり互換性がなく、イテレータにも対応しない
	//※イテレータが必要なら、containerAdapter::container_type にキャストして
	//　コンテナを取り出せば操作可能。
	//※データのコピーを避けて処理効率を向上させるために、
	//　enqueueBegin()～enqueueEnd()、dequeueBegin()～dequeueEnd()
	//　というメソッドを用意している。内部のバッファを直接参照するので高速。
	//　なお、begin～end の間はロックが行われる点に注意。
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, class CONTAINER_TYPE = binary_heap::container<typename OPE_TYPE::container_ope_type, _TABLE_SIZE> >
	class containerAdapter
	{
	public:
		//型
		DECLARE_OPE_TYPES(OPE_TYPE);
		typedef CONTAINER_TYPE container_type;//コンテナ型
		typedef typename container_type::status_t status_t;//ステータス型
	public:
		//アクセッサ
		inline const container_type& getContainer() const { return m_container; }//コンテナ取得
		inline container_type& getContainer(){ return m_container; }//コンテナ取得
	public:
		//キャストオペレータ
		inline operator const container_type&() const{ return m_container; }//コンテナを返す
		inline operator container_type&(){ return m_container; }//コンテナを返す
		inline operator lock_type&(){ return m_lock; }//ロックオブジェクト
		inline operator lock_type&() const { return m_lock; }//ロックオブジェクト ※mutable
	public:
		//メソッド：ロック取得系
		//単一ロック取得
		inline GASHA_ unique_lock<lock_type> lockUnique(){ GASHA_ unique_lock<lock_type> lock(*this); return lock; }
		inline GASHA_ unique_lock<lock_type> lockUnique(const GASHA_ with_lock_t){ GASHA_ unique_lock<lock_type> lock(*this, GASHA_ with_lock); return lock; }
		inline GASHA_ unique_lock<lock_type> lockUnique(const GASHA_ try_lock_t){ GASHA_ unique_lock<lock_type> lock(*this, GASHA_ try_lock); return lock; }
		inline GASHA_ unique_lock<lock_type> lockUnique(const GASHA_ adopt_lock_t){ GASHA_ unique_lock<lock_type> lock(*this, GASHA_ adopt_lock); return lock; }
		inline GASHA_ unique_lock<lock_type> lockUnique(const GASHA_ defer_lock_t){ GASHA_ unique_lock<lock_type> lock(*this, GASHA_ defer_lock); return lock; }
		//スコープロック取得
		inline GASHA_ lock_guard<lock_type> lockScoped(){ GASHA_ lock_guard<lock_type> lock(*this); return lock; }
	public:
		//メソッド：基本情報系
		inline size_type max_size() const { return m_container.max_aize(); }//最大要素数を取得
		inline size_type capacity() const { return m_container.capacity(); }//最大要素数を取得
		inline size_type size() const { return m_container.size(); }//使用中の要素数を取得
		inline size_type remain() const { return m_container.remain(); }//残りの要素数を取得
		inline bool empty() const { return m_container.empty(); }//空か？
		inline bool full() const { return m_container.full(); }//満杯か？
	private:
		//シーケンス番号発行
		inline seq_no_type getNextSeqNo();
		
		//可能ならシーケンス番号をリセット
		void checkAndResetSeqNo();
	private:
		//エンキュー（本体）：ムーブ
		node_type* _enqueueCopying(const node_type&& obj);
		
		//エンキュー（本体）：コピー
		node_type* _enqueueCopying(const node_type& obj);
	public:
		//エンキュー
		//※オブジェクト渡し
		//※オブジェクトには、あらかじめ優先度を設定しておく必要がある
		//※オブジェクトのコピーが２回発生する点に注意（少し遅くなる）
		//　（シーケンス番号をセットするために1回テンポラリにコピーし、プッシュ時にさらにコピーする。）
		//※ムーブコンストラクタとムーブオペレータを使用してコピーする点に注意
		//※処理中、ロックを取得する
		inline node_type* enqueueCopying(const node_type&& obj);
		inline node_type* enqueueCopying(const node_type& obj);
	private:
		//エンキュー（本体）
		template<typename... Tx>
		node_type* _enqueue(const priority_type priority, Tx... args);
	public:
		//エンキュー
		//※パラメータ渡し
		//※オブジェクトのコンストラクタが呼び出される
		//※オブジェクトには、シーケンス番号が書き込まれる
		//※処理中、ロックを取得する
		template<typename... Tx>
		node_type* enqueue(const priority_type priority, Tx... args);
	private:
		//エンキュー開始（本体）
		template<typename... Tx>
		node_type* _enqueueBegin(const priority_type priority, Tx... args);
	public:
		//エンキュー開始
		//※空きキュー取得
		//※エンキュー完了時に enqueueEnd を呼び出す必要あり
		//※この時点で、優先度とシーケンス番号が書き込まれる
		//※処理が成功すると、ロックを取得した状態になる（enqueueEndで解放する）
		template<typename... Tx>
		node_type* enqueueBegin(const priority_type priority, Tx... args);
	private:
		//エンキュー終了（本体）
		node_type* _enqueueEnd();
	public:
		//エンキュー終了
		//※enqueueBeginで取得したロックを解放する
		node_type* enqueueEnd();
	private:
		//エンキュー取り消し（本体）
		bool _enqueueCancel();
	public:
		//エンキュー取り消し
		//※enqueueBeginで取得したロックを解放する
		bool enqueueCancel();
	private:
		//デキュー（本体）
		bool _dequeueCopying(node_type& dst);
	public:
		//デキュー
		//※オブジェクトのコピーを受け取る領域を渡す
		//※オブジェクトのデストラクタが呼び出される ※コピー後に実行
		//※処理中、ロックを取得する
		inline bool dequeueCopying(node_type& dst);
	private:
		//デキュー開始（本体）
		//※デキュー完了時に dequeueEnd を呼び出す必要あり
		node_type* _dequeueBegin();
	public:
		//デキュー開始
		//※デキュー完了時に dequeueEnd を呼び出す必要あり
		//※処理が成功すると、ロックを取得した状態になる（dequeueEndで解放する）
		node_type* dequeueBegin();
	private:
		//デキュー終了（本体）
		bool _dequeueEnd();
	public:
		//デキュー終了
		//※オブジェクトのデストラクタが呼び出される
		//※dequeueBeginで取得したロックを解放する
		bool dequeueEnd();
	private:
		//デキュー取り消し（本体）
		bool _dequeueCancel();
	public:
		//デキュー取り消し
		//※dequeueBeginで取得したロックを解放する
		bool dequeueCancel();
		//先頭（根）キューを参照
		//※デキューしない
		const node_type* top() const;
	private:
		//先頭（根）キューのプライオリティ変更（本体）
		node_type* _changePriorityOnTop(const priority_type priority);
	public:
		//先頭（根）キューのプライオリティ変更
		//※プライオリティを変更した時点でキューの位置が入れ替わる
		//※シーケンス番号を再更新する
		//※同じプライオリティに変更した場合、同じプライオリティのキューの一番最後に回される
		//※処理中、ロックを取得する
		inline node_type* changePriorityOnTop(const priority_type priority);
		//※ロックなし版（top()参照～プライオリティ変更までを任意にロックするならこちらを使用する）
		inline node_type* changePriorityOnTopWithoutLock(const priority_type priority);
	private:
		//クリア（本体）
		void _clear();
	public:
		//クリア
		//※処理中、ロックを取得する
		inline void clear();
	public:
		//コンストラクタ
		inline containerAdapter();
		//デストラクタ
		~containerAdapter();
	private:
		//フィールド
		container_type m_container;//コンテナ
		int m_dummy;
		seq_no_type m_seqNo;//シーケンス番号 ※mutable修飾子
		mutable lock_type m_lock;//ロックオブジェクト
	};
	//----------------------------------------
	//シンプル優先度付きキューコンテナアダプタ
	//※操作用構造体の定義を省略してコンテナを使用するためのクラス。
	//※最も基本的な操作用構造体とそれに基づくコンテナ型を自動定義する。
	//プロトタイプ：
	//  int& REF_PRIORITY_FUNC(const value_type&)
	//  int& REF_SEQ_NO_FUNC(const std::uint32&)
	template<typename NODE_TYPE, std::size_t _TABLE_SIZE, class REF_PRIORTIY_FUNC, class REF_SEQ_NO_FUNC>
	struct simpleContainerAdapter
	{
		//優先度付きキュー操作用構造体
		struct ope : public baseOpe<ope, NODE_TYPE>
		{
			typedef typename baseOpe<ope, NODE_TYPE>::node_type node_type;
			typedef typename baseOpe<ope, NODE_TYPE>::priority_type priority_type;
			typedef typename baseOpe<ope, NODE_TYPE>::seq_no_type seq_no_type;
			
			//優先度を取得
			inline static priority_type getPriority(const node_type& node){ priority_type& ref_priority = REF_PRIORTIY_FUNC(const_cast<node_type*>(node)); return ref_priority; }
			//優先度を更新
			inline static void setPriority(node_type& node, const priority_type priority){ priority_type& ref_priority = REF_PRIORTIY_FUNC(const_cast<node_type*>(node)); ref_priority = priority; }
			
			//シーケンス番号を取得
			inline static seq_no_type getSeqNo(const node_type& node){ seq_no_type& ref_seq_no = REF_SEQ_NO_FUNC(const_cast<node_type*>(node)); return ref_seq_no; }
			//シーケンス番号を更新
			inline static void setSeqNo(node_type& node, const seq_no_type seq_no){ seq_no_type& ref_seq_no = REF_SEQ_NO_FUNC(const_cast<node_type*>(node)); ref_seq_no = seq_no; }
		};

		//基本型定義
		DECLARE_OPE_TYPES(ope);

		//優先度付きキューコンテナアダプタ
		class con : public containerAdapter<ope_type, _TABLE_SIZE>
		{
		public:
		#ifdef GASHA_HAS_INHERITING_CONSTRUCTORS
			using containerAdapter<ope_type, _TABLE_SIZE>::containerAdapter;//継承コンストラクタ
		#else//GASHA_HAS_INHERITING_CONSTRUCTORS
			//デフォルトコンスタラクタ
			inline con() :
				containerAdapter<ope_type, _TABLE_SIZE>()
			{}
		#endif//GASHA_HAS_INHERITING_CONSTRUCTORS
			//デストラクタ
			inline ~con()
			{}
		};
	};
	
	//--------------------
	//安全なエンキュー／デキュー操作クラス
	//※操作状態を記憶し、デストラクタで必ず完了させる
	template<class CON>
	class operation_guard
	{
	public:
		//型
		typedef CON container_adapter_type;//コンテナアダプター型
		typedef typename CON::node_type node_type;//ノード型
		typedef typename CON::status_t status_t;//ステータス型
		typedef typename CON::priority_type priority_type;//優先度型
		//typedef typename CON::seq_no_type seq_no_type;//シーケンス番号型
	public:
		//アクセッサ
		status_t status() const { return m_status; }//ステータスを取得
	public:
		//エンキュー開始
		template<typename... Tx>
		node_type* enqueueBegin(const priority_type priority, Tx... args);
		
		//エンキュー終了
		node_type* enqueueEnd();
		
		//エンキュー取り消し
		bool enqueueCancel();
		
		//デキュー開始
		node_type* dequeueBegin();
		
		//デキュー終了
		bool dequeueEnd();
		
		//デキュー取り消し
		bool dequeueCancel();
	public:
		//コンストラクタ
		inline operation_guard(container_adapter_type& containerAdapter);
		
		//デフォルトコンストラクタ
		operation_guard() = delete;

		//デストラクタ
		~operation_guard();
	private:
		//フィールド
		container_adapter_type& m_containerAdapter;//コンテナアダプタ
		status_t m_status;//ステータス
	};

	//--------------------
	//基本型定義マクロ消去
	#undef DECLARE_OPE_TYPES
}//namespace priority_queue

//--------------------
//クラスの別名
//※ネームスペースの指定を省略してクラスを使用するための別名

//優先度付きキュー操作用テンプレート構造体
template<class OPE_TYPE, typename NODE_TYPE, typename PRIORITY_TYPE, typename SEQ_NO_TYPE>
using priorityQueue_baseOpe = priority_queue::baseOpe<OPE_TYPE, NODE_TYPE, PRIORITY_TYPE, SEQ_NO_TYPE>;

//優先度付きキューコンテナアダプタ
template<class OPE_TYPE, std::size_t _TABLE_SIZE>
using priorityQueue = priority_queue::containerAdapter<OPE_TYPE, _TABLE_SIZE>;

//シンプル優先度付きキューコンテナアダプタ
template<typename NODE_TYPE, std::size_t _TABLE_SIZE, class REF_PRIORTIY_FUNC, class REF_SEQ_NO_FUNC>
using simplePriorityQueue = priority_queue::simpleContainerAdapter<NODE_TYPE, _TABLE_SIZE, REF_PRIORTIY_FUNC, REF_SEQ_NO_FUNC>;

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルードする場合
#ifdef GASHA_PRIORITY_QUEUE_ALLWAYS_TOGETHER_INL
#include <gasha/dynamic_array.inl>
#endif//GASHA_PRIORITY_QUEUE_ALLWAYS_TOGETHER_INL

//.hファイルのインクルードに伴い、常に.cp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_PRIORITY_QUEUE_ALLWAYS_TOGETHER_CPP_H
#include <gasha/dynamic_array.cpp.h>
#endif//GASHA_PRIORITY_QUEUE_ALLWAYS_TOGETHER_CPP_H

#endif//__PRIORITY_QUEUE_H_

// End of file

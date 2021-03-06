﻿#pragma once
#ifndef GASHA_INCLUDED_PRIORITY_QUEUE_H
#define GASHA_INCLUDED_PRIORITY_QUEUE_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// priority_queue.h
// 優先度付きキューコンテナ【宣言部】
// ※データのメモリ管理を行わない擬似コンテナ
//
// ※クラスをインスタンス化する際は、別途 .cpp.h ファイルをインクルードする必要あり。
// ※明示的なインスタンス化を避けたい場合は、ヘッダーファイルと共にインクルード。
// 　（この場合、実際に使用するメンバー関数しかインスタンス化されないので、対象クラスに不要なインターフェースを実装しなくても良い）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dummy_lock.h>//ダミーロック
#include <gasha/lock_guard.h>//スコープロック
#include <gasha/unique_lock.h>//単一ロック
#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し

#include <cstdint>//C++11 std::int32_t, std::uint32_t

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
	//  //struct 派生構造体名 : public priority_queue::baseOpe<派生構造体名, テーブルサイズ, 要素の型, 優先度の型, シーケンス番号の型>
	//	struct ope : public priority_queue::baseOpe<ope, TABLE_SIZE, data_t, int, unsigned int>
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
	//		inline static int comparePriority(const priority_type lhs, const priority_type rhs){ return ???; }
	//		
	//		//ロックポリシー ※必要に応じて定義
	//		//※ロックでコンテナ操作をスレッドセーフにしたい場合は、
	//		//　有効なロック型（spinLockなど）を lock_type 型として定義する。
	//		typedef spinLock lock_type;//ロックオブジェクト型
	//	};
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, typename NODE_TYPE, typename PRIORITY_TYPE = std::int32_t, typename SEQ_NO_TYPE = std::uint32_t>
	struct baseOpe
	{
		//型
		typedef OPE_TYPE ope_type;//ノード操作型
		typedef NODE_TYPE node_type;//ノード型
		typedef PRIORITY_TYPE priority_type;//優先度型
		typedef SEQ_NO_TYPE seq_no_type;//シーケンス番号型

		//定数
		static const std::size_t TABLE_SIZE = _TABLE_SIZE;//テーブルサイズ ※扱えるデータの要素数

		//ロックポリシー
		typedef GASHA_ dummyLock lock_type;//ロックオブジェクト型
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
		//※rhsの方が小さいければ true を返す（優先度の逆順判定）
		inline static bool lessSeqNo(const seq_no_type lhs, const seq_no_type rhs)
		{
			return lhs > rhs;
		}

		//優先度とシーケンス番号を比較する
		//※lhsの方が優先度が低ければ true を返す（優先度の逆順判定）
		//※優先度が同じなら、rhsの方のシーケンス番号が小さければ true を返す
		inline static bool lessPriorityAndSeqNo(const int compare_priority, const seq_no_type lhs, const seq_no_type rhs)
		{
			return compare_priority < 0 ? true : compare_priority == 0 ? ope_type::lessSeqNo(lhs, rhs) : false;
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
		static void callDestructor(node_type* obj){ GASHA_ callDestructor(obj); }

		//コンテナ操作型
		//※コンテナを変更する場合、派生クラスで改めて container_ope_type を実装する。
		struct container_ope_type : public GASHA_ binary_heap::baseOpe<container_ope_type, TABLE_SIZE, node_type>
		{
			typedef OPE_TYPE adapter_ope_type;//コンテナアダプタのノード操作型

			//ノード比較用プレディケート関数オブジェクト
			//※trueでlhsの方が小さい（並び順が正しい）
			struct less{
				inline bool operator()(const node_type& lhs, const node_type& rhs) const
				{
					return typename adapter_ope_type::less()(lhs, rhs);
				}
			};
			
			//デストラクタ呼び出し
			static void callDestructor(node_type* obj){ GASHA_ callDestructor(obj); }
			
			//ロック型
			typedef dummyLock lock_type;//ロックオブジェクト型
			//※コンテナ側ではロック制御しない
		};

		//コンテナ型
		//※コンテナを変更する場合、派生クラスで改めて container_type を実装する。
		typedef GASHA_ binary_heap::container<container_ope_type> container_type;
	};
	
	//--------------------
	//基本型定義マクロ
	#define GASHA_DECLARE_OPE_TYPES(OPE_TYPE) \
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
	//※イテレータが必要なら、container::container_type にキャストして
	//　コンテナを取り出せば操作可能。
	//※データのコピーを避けて処理効率を向上させるために、
	//　enqueueBegin()～enqueueEnd()、dequeueBegin()～dequeueEnd()
	//　というメソッドを用意している。内部のバッファを直接参照するので高速。
	//　なお、begin～end の間はロックが行われる点に注意。
	template<class OPE_TYPE>
	class container
	{
	public:
		//型
		GASHA_DECLARE_OPE_TYPES(OPE_TYPE);
		typedef typename ope_type::container_type container_type;//コンテナ型
		typedef typename container_type::status_t status_t;//ステータス型
	public:
		//定数
		static const size_type TABLE_SIZE = ope_type::TABLE_SIZE;//配列要素数
	public:
		//--------------------
		//単一操作オブジェクト（安全なエンキュー／デキュー操作クラス）
		//※操作状態を記憶し、デストラクタで必ず完了させる
		class uniqueOperation
		{
		public:
			//型
			typedef container container_adapter_type;//コンテナアダプタ型
			typedef typename container_adapter_type::node_type node_type;//ノード型
			typedef typename container_adapter_type::status_t status_t;//ステータス型
			typedef typename container_adapter_type::priority_type priority_type;//優先度型
			//typedef typename container_adapter_type::seq_no_type seq_no_type;//シーケンス番号型
		public:
			//アクセッサ
			status_t status() const { return m_status; }//ステータスを取得
		public:
			//エンキュー開始
			template<typename... Tx>
			node_type* enqueueBegin(const priority_type priority, Tx&&... args);
			
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
			//ムーブコンストラクタ
			inline uniqueOperation(uniqueOperation&& obj);
			//コピーコンストラクタ
			uniqueOperation(const uniqueOperation& obj) = delete;
			//コンストラクタ
			inline uniqueOperation(container_adapter_type& container_adapter);
			//デフォルトコンストラクタ
			uniqueOperation() = delete;
			//デストラクタ
			~uniqueOperation();
		private:
			//フィールド
			container_adapter_type& m_container;//コンテナアダプタ
			status_t m_status;//ステータス
		};
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
		inline GASHA_ unique_lock<lock_type> lockUnique() const { GASHA_ unique_lock<lock_type> lock(*this); return lock; }
		inline GASHA_ unique_lock<lock_type> lockUnique(const GASHA_ with_lock_t&) const { GASHA_ unique_lock<lock_type> lock(*this, GASHA_ with_lock); return lock; }
		inline GASHA_ unique_lock<lock_type> lockUnique(const GASHA_ try_to_lock_t&) const { GASHA_ unique_lock<lock_type> lock(*this, GASHA_ try_to_lock); return lock; }
		inline GASHA_ unique_lock<lock_type> lockUnique(const GASHA_ adopt_lock_t&) const { GASHA_ unique_lock<lock_type> lock(*this, GASHA_ adopt_lock); return lock; }
		inline GASHA_ unique_lock<lock_type> lockUnique(const GASHA_ defer_lock_t&) const { GASHA_ unique_lock<lock_type> lock(*this, GASHA_ defer_lock); return lock; }
		//スコープロック取得
		inline GASHA_ lock_guard<lock_type> lockScoped() const { GASHA_ lock_guard<lock_type> lock(*this); return lock; }
	public:
		//単一操作オブジェクト
		inline uniqueOperation operationUnique(){ uniqueOperation operation(*this); return operation; }
	public:
		//メソッド：基本情報系
		inline size_type max_size() const { return m_container.max_size(); }//最大要素数を取得
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
		node_type* _enqueueCopying(node_type&& obj);
		
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
		inline node_type* enqueueCopying(node_type&& obj);
		inline node_type* enqueueCopying(const node_type& obj);
	private:
		//エンキュー（本体）
		template<typename... Tx>
		node_type* _enqueue(const priority_type priority, Tx&&... args);
	public:
		//エンキュー
		//※パラメータ渡し
		//※オブジェクトのコンストラクタが呼び出される
		//※オブジェクトには、シーケンス番号が書き込まれる
		//※処理中、ロックを取得する
		template<typename... Tx>
		node_type* enqueue(const priority_type priority, Tx&&... args);
	private:
		//エンキュー開始（本体）
		template<typename... Tx>
		node_type* _enqueueBegin(const priority_type priority, Tx&&... args);
	public:
		//エンキュー開始
		//※空きキュー取得
		//※エンキュー完了時に enqueueEnd を呼び出す必要あり
		//※この時点で、優先度とシーケンス番号が書き込まれる
		//※処理が成功すると、ロックを取得した状態になる（enqueueEndで解放する）
		template<typename... Tx>
		node_type* enqueueBegin(const priority_type priority, Tx&&... args);
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
	public:
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
		inline container();
		//デストラクタ
		~container();
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
	template<typename VALUE_TYPE, std::size_t _TABLE_SIZE>
	class simpleContainer
	{
	public:
		typedef VALUE_TYPE core_value_type;//値型
		typedef std::int32_t core_priority_type;//優先度型
		typedef std::uint32_t core_seq_no_type;//シーケンス番号型

		//データノード型
		//※元の値型に対するプロキシーとして振る舞う
		struct node
		{
			core_value_type m_value;//値
			core_priority_type m_priority;//優先度
			core_seq_no_type m_seqNo;//シーケンス番号

			//キャストオペレータ
			inline operator const core_value_type&() const { return m_value; }
			inline operator core_value_type&(){ return m_value; }
			inline operator const core_value_type*() const { return &m_value; }
			inline operator core_value_type*(){ return &m_value; }
			//inline operator core_priority_type() const { return m_priority; }
			//inline operator core_seq_no_type() const { return m_seqNo; }
			//アクセッサ
			inline const core_value_type& value() const { return m_value; }
			inline core_value_type& value(){ return m_value; }
			inline const core_value_type* pointer() const { return &m_value; }
			inline core_value_type* pointer(){ return &m_value; }
			inline const core_priority_type& priority() const { return m_priority; }
			inline const core_seq_no_type& seqNo() const { return m_seqNo; }
			//基本オペレータ
			inline const core_value_type& operator*() const { return m_value; }
			inline core_value_type& operator*(){ return m_value; }
			inline const core_value_type* operator->() const { return &m_value; }
			inline core_value_type* operator->(){ return &m_value; }
		#if 1//※とくに必要なインターフェースではないが、あっても問題ない
			//比較オペレータ（自身の型との比較）
			inline bool operator==(const node& rhs) const { return m_priority == rhs.m_priority && m_seqNo == m_seqNo; }
			inline bool operator!=(const node& rhs) const { return m_priority != rhs.m_priority || m_seqNo != m_seqNo; }
			inline bool operator<(const node& rhs) const { return m_priority < rhs.m_priority || (m_priority == rhs.m_priority && m_seqNo < m_seqNo); }
			inline bool operator>(const node& rhs) const { return m_priority > rhs.m_priority || (m_priority == rhs.m_priority && m_seqNo > m_seqNo); }
			inline bool operator<=(const node& rhs) const { return m_priority <= rhs.m_priority || (m_priority == rhs.m_priority && m_seqNo <= m_seqNo); }
			inline bool operator>=(const node& rhs) const { return m_priority >= rhs.m_priority || (m_priority == rhs.m_priority && m_seqNo >= m_seqNo); }
		#endif
			//ムーブオペレータ
			inline node& operator=(core_value_type&& value);
			//コピーオペレータ
			inline node& operator=(const core_value_type& value);
			//ムーブコンストラクタ
			inline node(const core_priority_type priority, core_value_type&& value);
			//コピーコンストラクタ
			inline node(const core_priority_type priority, const core_value_type& value);
			//コンストラクタ
			template<typename... Tx>
			inline node(const core_priority_type priority, Tx&&... args);
			//デフォルトコンストラクタ
			inline node();
			//デストラクタ
			inline ~node();
		};

		//優先度付きキュー操作用構造体
		struct ope : public baseOpe<ope, _TABLE_SIZE, node, core_priority_type, core_seq_no_type>
		{
			typedef typename baseOpe<ope, _TABLE_SIZE, node, core_priority_type, core_seq_no_type>::node_type node_type;
			typedef typename baseOpe<ope, _TABLE_SIZE, node, core_priority_type, core_seq_no_type>::priority_type priority_type;
			typedef typename baseOpe<ope, _TABLE_SIZE, node, core_priority_type, core_seq_no_type>::seq_no_type seq_no_type;
			
			//優先度を取得
			inline static priority_type getPriority(const node_type& node){ return node.m_priority; }
			//優先度を更新
			inline static void setPriority(node_type& node, const priority_type priority){ node.m_priority = priority; }
			
			//シーケンス番号を取得
			inline static seq_no_type getSeqNo(const node_type& node){ return node.m_seqNo; }
			//シーケンス番号を更新
			inline static void setSeqNo(node_type& node, const seq_no_type seq_no){ node.m_seqNo = seq_no; }
		};

		//基本型定義
		GASHA_DECLARE_OPE_TYPES(ope);

		//優先度付きキューコンテナアダプタ
		class con : public container<ope_type>
		{
		public:
		#ifdef GASHA_HAS_INHERITING_CONSTRUCTORS
			using container<ope_type>::container;//継承コンストラクタ
		#else//GASHA_HAS_INHERITING_CONSTRUCTORS
			//デフォルトコンスタラクタ
			inline con() :
				container<ope_type>()
			{}
		#endif//GASHA_HAS_INHERITING_CONSTRUCTORS
			//デストラクタ
			inline ~con()
			{}
		};
	};

	//--------------------
	//基本型定義マクロ消去
	#undef GASHA_DECLARE_OPE_TYPES
}//namespace priority_queue

//--------------------
//クラスの別名
//※ネームスペースの指定を省略してクラスを使用するための別名

//優先度付きキュー操作用テンプレート構造体
template<class OPE_TYPE, std::size_t _TABLE_SIZE, typename NODE_TYPE, typename PRIORITY_TYPE = std::int32_t, typename SEQ_NO_TYPE = std::uint32_t>
using pQueue_baseOpe = priority_queue::baseOpe<OPE_TYPE, _TABLE_SIZE, NODE_TYPE, PRIORITY_TYPE, SEQ_NO_TYPE>;

//優先度付きキューコンテナアダプタ
template<class OPE_TYPE>
using pQueue = priority_queue::container<OPE_TYPE>;

//シンプル優先度付きキューコンテナアダプタ
template<typename NODE_TYPE, std::size_t _TABLE_SIZE>
using simplePQueue = priority_queue::simpleContainer<NODE_TYPE, _TABLE_SIZE>;

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/priority_queue.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_PRIORITY_QUEUE_ALLWAYS_TOGETHER_CPP_H
#include <gasha/priority_queue.cpp.h>
#endif//GASHA_PRIORITY_QUEUE_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_PRIORITY_QUEUE_H

// End of file

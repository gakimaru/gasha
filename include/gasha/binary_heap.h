#pragma once
#ifndef GASHA_INCLUDED_BINARY_HEAP_H
#define GASHA_INCLUDED_BINARY_HEAP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// binary_heap.h
// 二分ヒープコンテナ【宣言部】
// ※データのメモリ管理を行わない擬似コンテナ
//
// ※コンテナをインスタンス化する際は、別途下記のファイルをインクルードする必要あり
//
//   ・binary_heap.inl   ... 【インライン関数／テンプレート関数定義部】
//                            コンテナクラスの操作が必要な場所でインクルード。
//   ・binary_heap.cpp.h ... 【関数定義部】
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

#include <gasha/sort_basic.h>//ソート処理基本（大小比較用）

#include <cstddef>//std::size_t, std::ptrdiff_t用

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <iterator> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <iterator>//std::iterator用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//二分ヒープ
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//データ構造とアルゴリズム
//--------------------------------------------------------------------------------
//【特徴】
//・二分ヒープの木構造とアルゴリズムにより、常に根が最大となることを保証する。
//  これにより、最大値の取得が常に O(1) で行える。
//　＜二分ヒープのデータ構造＞
//	  - 平衡二分木。
//	  - 配列を二分木に見立てて扱う。先頭要素が根となり、順番に子ノードして扱う。
//      配列の順列で木の配置が決定するため、連結情報が不要。
//	  - ノードの子（左右両方）には、自身よりキーが小さいノードを連結。
//	  - 兄弟ノード間のキーの大小は不定。
//--------------------------------------------------------------------------------
//【利点】
//・最大値の探索がほぼO(1)で行える。
//・各要素のメモリオーバーヘッドがない。※要素間の連結情報などを持つ必要がない
//--------------------------------------------------------------------------------
//【欠点】
//・ランダムアクセスができない。
//・要素の昇順・降順アクセスができない。
//・同一キーのデータの取得順序が不定。（登録順に取得できることを保証しない）
//--------------------------------------------------------------------------------
//【本プログラムにおける実装要件】
//・固定配列のコンテナとし、最大要素数の自動拡張を行わない。
//・ノードの型をユーザー定義可能とする。
//・コンテナを用いずとも、配列のヒープ操作アルゴリズム（関数）のみの利用も可能とする。
//・コンテナは、STLの std::priority_qeueu をモデルとしたインターフェースを実装する。
//・STL（std::priority_qeueu）との主な違いは下記のとおり。
//    - 固定長のため、自動拡張を行わない。
//    - 例外を扱わない。そのため、イテレータへの isExist() メソッドの追加や、
//      at()メソッドが返す情報がポインターなど、インターフェースの相違点がある。
//    - （他のコンテナと同様に）コンテナ操作対象・方法を設定した
//      構造体をユーザー定義して用いる。
//・プッシュ時に値（キュー）のコピーをせずに、直接要素のコンストラクタを呼び出して
//　初期化可能とする。（処理時間を効率化する）
//--------------------------------------------------------------------------------
//【想定する具的的な用途】
//・優先度付きキュー。ただし、キューイングの順序性を保証しない。
//--------------------------------------------------------------------------------

namespace binary_heap
{
	//--------------------
	//二分ヒープ操作用テンプレート構造体
	//※CRTPを活用し、下記のような派生構造体を作成して使用する
	//  //template<class OPE_TYPE, typename NODE_TYPE>
	//  //struct 派生構造体名 : public binary_heap::baseOpe<派生構造体名, ノードの型>
	//	struct ope : public binary_heap::baseOpe<ope, data_t>
	//	{
	//		//ノード比較用プレディケート関数オブジェクト
	//		//※必要に応じて実装する
	//		struct predicateForSort{
	//			inline bool operator()(const node_type& lhs, const node_type& rhs) const { return lhs.??? < rhs.???; }
	//		};
	//		
	//		//ロック型 ※必要に応じて定義
	//		//※ロックでコンテナ操作をスレッドセーフにしたい場合は、
	//		//　有効なロック型（spin_lockなど）を lock_type 型として定義する。
	//		typedef spin_lock lock_type;//ロックオブジェクト型
	//	};
	template<class OPE_TYPE, typename NODE_TYPE>
	struct baseOpe
	{
		//型
		typedef OPE_TYPE ope_type;//ノード操作型
		typedef NODE_TYPE node_type;//ノード型

		//ロック型
		typedef dummyLock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※ロックでコンテナ操作をスレッドセーフにしたい場合は、
		//　baseOpeの派生クラスにて、有効なロック型（spinLock など）を
		//　lock_type 型として再定義する。
		//【補足】コンテナには、あらかじめロック制御のための仕組みがソースコードレベルで
		//　　　　仕込んであるが、有効な型を与えない限りは、実行時のオーバーヘッドは一切ない。

		//ノード比較用プレディケート関数オブジェクト
		//※trueでlhsの方が小さい（並び順が正しい）
		struct less{
			inline bool operator()(const node_type& lhs, const node_type& rhs) const { return GASHA_ less<node_type>()(lhs, rhs); }
		};

		//デストラクタ呼び出し
		inline static void callDestructor(node_type* obj){ obj->~NODE_TYPE(); }
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
		typedef int difference_type; \
		typedef std::size_t size_type; \
		typedef std::size_t index_type; \
		typedef typename ope_type::lock_type lock_type;
		//typedef std::ptrdiff_t difference_type;//※difference_typeは、std::ptrdiff_t を使用するとイテレータのオペレータのオーバーロードで問題を起こすので、int 型で扱う
	
	//--------------------
	//二分ヒープ操作関数：親のインデックス計算
	inline std::size_t calcParent(const std::size_t index);
	//--------------------
	//二分ヒープ操作関数：子のインデックス計算
	inline std::size_t calcChildL(const std::size_t index);
	inline std::size_t calcChildR(const std::size_t index);
	//--------------------
	//二分ヒープ操作関数：アップヒープ
	//※ノードを上方に移動
	template<class OPE_TYPE, class PREDICATE>
	typename OPE_TYPE::node_type* upHeap(typename OPE_TYPE::node_type* top, const std::size_t size, typename OPE_TYPE::node_type* now, PREDICATE less);
	//--------------------
	//二分ヒープ操作関数：ダウンヒープ
	//※ノードを下方に移動
	template<class OPE_TYPE, class PREDICATE>
	typename OPE_TYPE::node_type* downHeap(typename OPE_TYPE::node_type* top, const std::size_t size, typename OPE_TYPE::node_type* now, PREDICATE less);
	
	//----------------------------------------
	//二分ヒープコンテナ
	//※固定配列と使用中の要素数を持つ
	//※std::vectorとstd::priority_queueを一つにしたようなコンテナ。
	//　なお、emplace()メソッドには非対応。
	//※データのコピーを避けて処理効率を向上させるために、
	//　pushBegin()～pushEnd()、popBegin()～popEnd()
	//　というメソッドを用意している。内部のバッファを直接参照するので高速。
	//　なお、begin～end の間はロックが行われる点に注意。
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	class container
	{
	public:
		//型
		DECLARE_OPE_TYPES(OPE_TYPE);
	public:
		//定数
		static const size_type TABLE_SIZE = _TABLE_SIZE;//配列要素数
		static const index_type INVALID_INDEX = 0xffffffffu;//無効なインデックス
		enum status_t//ステータス
		{
			IDLE = 0,//アイドル
			PUSH_BEGINNING,//プッシュ開始中
			PUSH_ENDED,//プッシュ終了した
			PUSH_CANCELLED,//プッシュ取り消した
			POP_BEGINNING,//ポップ開始中
			POP_ENDED,//ポップ終了した
			POP_CANCELLED,//ポップ取り消した
		};
	public:
		//--------------------
		//イテレータ宣言
		typedef std::input_iterator_tag iterator_category;
		class iterator;
		class reverse_iterator;
		typedef const iterator const_iterator;
	#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
		typedef const reverse_iterator const_reverse_iterator;
	#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
		//--------------------
		//イテレータ
		class iterator : public std::iterator<iterator_category, value_type>
		{
			friend class container;
		#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			friend class reverse_iterator;
		#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
		public:
			//※コンパイラによって優先して参照する型があいまいになることを避けるための定義
			typedef typename container::value_type value_type;
			typedef typename container::difference_type difference_type;
			typedef typename container::size_type size_type;
			typedef typename container::index_type indextype;
		#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			typedef typename container::reverse_iterator reverse_iterator;
		#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const_reference() const { return *getValue(); }
			//inline operator reference(){ return *getValue(); }//std::input_iterator_tag には不要
			inline operator const_pointer() const { return getValue(); }
			//inline operator poiner*(){ return getValue(); }//std::input_iterator_tag には不要
		public:
			//基本オペレータ
			inline const_reference operator*() const { return *getValue(); }
			//inline reference operator*(){ return *getValue(); }//std::input_iterator_tag には不要
			inline const_pointer operator->() const { return getValue(); }
			//inline pointer operator->(){ return getValue(); }//std::input_iterator_tag には不要
		#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE//std::input_iterator_tag には本来必要ではない
			inline const iterator operator[](const int index) const;
			inline iterator operator[](const int index);
		#endif//GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//比較オペレータ
			inline bool operator==(const iterator& rhs) const;
			inline bool operator!=(const iterator& rhs) const;
		#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE//std::input_iterator_tag には本来必要ではない
			inline bool operator>(const iterator& rhs) const;
			inline bool operator>=(const iterator& rhs) const;
			inline bool operator<(const iterator& rhs) const;
			inline bool operator<=(const iterator& rhs) const;
		#endif//GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//演算オペレータ
			inline iterator& operator++();
		#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			inline iterator& operator--();
		#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
			inline iterator operator++(int);
		#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			inline iterator operator--(int);
		#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
			inline const iterator& operator++() const { return const_cast<iterator*>(this)->operator++(); }
		#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			inline const iterator& operator--() const { return const_cast<iterator*>(this)->operator--(); }
		#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
			inline const iterator operator++(int) const { return const_cast<iterator*>(this)->operator++(0); }
		#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			inline const iterator operator--(int) const { return const_cast<iterator*>(this)->operator--(0); }
		#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
		#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE//std::input_iterator_tag には本来必要ではない
			inline iterator& operator+=(const difference_type rhs);
			inline iterator& operator+=(const size_type rhs) { return operator+=(static_cast<difference_type>(rhs)); }
		#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			inline iterator& operator-=(const difference_type rhs);
			inline iterator& operator-=(const size_type rhs) { return operator-=(static_cast<difference_type>(rhs)); }
		#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			inline const iterator& operator+=(const difference_type rhs) const { return const_cast<iterator*>(this)->operator+=(rhs); }
			inline const iterator& operator+=(const size_type rhs) const { return const_cast<iterator*>(this)->operator+=(rhs); }
		#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			inline const iterator& operator-=(const difference_type rhs) const { return const_cast<iterator*>(this)->operator-=(rhs); }
			inline const iterator& operator-=(const size_type rhs) const  { return const_cast<iterator*>(this)->operator-=(rhs); }
		#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			inline iterator operator+(const difference_type rhs) const;
			inline iterator operator+(const size_type rhs) const { return operator+(static_cast<difference_type>(rhs)); }
		#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			inline iterator operator-(const difference_type rhs) const;
			inline iterator operator-(const size_type rhs) const { return operator-(static_cast<difference_type>(rhs)); }
		#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			inline difference_type operator-(const iterator& rhs) const;
		#endif//GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//アクセッサ
			inline bool isExist() const;
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const;
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const;//終端か？
			inline index_type getIndex() const;//インデックス
			inline const value_type* getValue() const;//現在の値
			inline value_type* getValue();//現在の値
		private:
			//メソッド
			void update(const index_type index) const;
			void addIndexAndUpdate(const difference_type add) const;
		public:
			//ムーブオペレータ
			iterator& operator=(iterator&& rhs);
		#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			iterator& operator=(reverse_iterator&& rhs);
		#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
			//コピーオペレータ
			iterator& operator=(const iterator& rhs);
		#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			iterator& operator=(const reverse_iterator& rhs);
		#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
		public:
			//ムーブコンストラクタ
			iterator(iterator&& obj);
		#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			iterator(reverse_iterator&& obj);
		#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
			//コピーコンストラクタ
			iterator(const iterator& obj);
		#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
			iterator(const reverse_iterator& obj);
		#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
			//コンストラクタ
			iterator(const container& con, const bool is_end);
			//デフォルトコンストラクタ
			inline iterator() :
				m_con(nullptr),
				m_index(INVALID_INDEX),
				m_value(nullptr)
			{}
			//デストラクタ
			inline ~iterator()
			{}
		protected:
			//フィールド
			const container* m_con;//コンテナ
			mutable index_type m_index;//現在のインデックス
			mutable value_type* m_value;//現在の値
		};
	#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
		//--------------------
		//リバースイテレータ
		//class reverse_iterator : public std::reverse_iterator<iterator>
		class reverse_iterator : public std::iterator<iterator_category, value_type>
		{
			friend class container;
			friend class iterator;
		public:
			//※コンパイラによって優先して参照する型があいまいになることを避けるための定義
			typedef typename container::value_type value_type;
			typedef typename container::difference_type difference_type;
			typedef typename container::size_type size_type;
			typedef typename container::index_type indextype;
			typedef typename container::iterator iterator;
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const value_type&() const { return *getValue(); }
			//inline operator value_type&(){ return *getValue(); }//std::input_iterator_tag には不要
			inline operator const value_type*() const { return getValue(); }
			//inline operator value_type*(){ return getValue(); }//std::input_iterator_tag には不要
		public:
			//基本オペレータ
			inline const value_type& operator*() const { return *getValue(); }
			//inline value_type& operator*(){ return *getValue(); }//std::input_iterator_tag には不要
			inline const_pointer operator->() const { return getValue(); }
			//inline pointer operator->(){ return getValue(); }//std::input_iterator_tag には不要
		#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE//std::input_iterator_tag には本来必要ではない
			inline const reverse_iterator operator[](const int index) const;
			inline reverse_iterator operator[](const int index);
		#endif//GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//比較オペレータ
			inline bool operator==(const reverse_iterator& rhs) const;
			inline bool operator!=(const reverse_iterator& rhs) const;
		#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE//std::input_iterator_tag には本来必要ではない
			inline bool operator>(const reverse_iterator& rhs) const;
			inline bool operator>=(const reverse_iterator& rhs) const;
			inline bool operator<(const reverse_iterator& rhs) const;
			inline bool operator<=(const reverse_iterator& rhs) const;
		#endif//GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//演算オペレータ
			inline reverse_iterator& operator++();
			inline reverse_iterator& operator--();
			inline reverse_iterator operator++(int);
			inline reverse_iterator operator--(int);
			inline const reverse_iterator& operator++() const { return const_cast<reverse_iterator*>(this)->operator++(); }
			inline const reverse_iterator& operator--() const { return const_cast<reverse_iterator*>(this)->operator--(); }
			inline const reverse_iterator operator++(int) const { return const_cast<reverse_iterator*>(this)->operator++(0); }
			inline const reverse_iterator operator--(int) const { return const_cast<reverse_iterator*>(this)->operator--(0); }
		#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE//std::input_iterator_tag には本来必要ではない
			inline reverse_iterator& operator+=(const difference_type rhs);
			inline reverse_iterator& operator+=(const size_type rhs) { return operator+=(static_cast<difference_type>(rhs)); }
			inline reverse_iterator& operator-=(const difference_type rhs);
			inline reverse_iterator& operator-=(const size_type rhs) { return operator-=(static_cast<difference_type>(rhs)); }
			inline const reverse_iterator& operator+=(const difference_type rhs) const { return const_cast<reverse_iterator*>(this)->operator+=(rhs); }
			inline const reverse_iterator& operator+=(const size_type rhs) const { return const_cast<reverse_iterator*>(this)->operator+=(rhs); }
			inline const reverse_iterator& operator-=(const difference_type rhs) const { return const_cast<reverse_iterator*>(this)->operator-=(rhs); }
			inline const reverse_iterator& operator-=(const size_type rhs) const  { return const_cast<reverse_iterator*>(this)->operator-=(rhs); }
			inline reverse_iterator operator+(const difference_type rhs) const;
			inline reverse_iterator operator+(const size_type rhs) const { return operator+(static_cast<difference_type>(rhs)); }
			inline reverse_iterator operator-(const difference_type rhs) const;
			inline reverse_iterator operator-(const size_type rhs) const { return operator-(static_cast<difference_type>(rhs)); }
			inline difference_type operator-(const reverse_iterator& rhs) const;
		#endif//GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//アクセッサ
			inline bool isExist() const;
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const;
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const;//終端か？
			inline index_type getIndex() const;//インデックス
			inline const value_type* getValue() const;//現在の値
			inline value_type* getValue();//現在の値
		private:
			//参照を更新
			void update(const index_type index) const;
			void addIndexAndUpdate(const difference_type add) const;
		public:
			//ベースを取得
			inline const iterator base() const;
			inline iterator base();
		public:
			//ムーブオペレータ
			reverse_iterator& operator=(reverse_iterator&& rhs);
			reverse_iterator& operator=(iterator&& rhs);
			//コピーオペレータ
			reverse_iterator& operator=(const reverse_iterator& rhs);
			reverse_iterator& operator=(const iterator& rhs);
		public:
			//ムーブコンストラクタ
			reverse_iterator(reverse_iterator&& obj);
			reverse_iterator(iterator&& obj);
			//コピーコンストラクタ
			reverse_iterator(const reverse_iterator& obj);
			reverse_iterator(const iterator& obj);
			//コンストラクタ
			reverse_iterator(const container& con, const bool is_end);
			//デフォルトコンストラクタ
			inline reverse_iterator() :
				m_con(nullptr),
				m_index(INVALID_INDEX),
				m_value(nullptr)
			{}
			//デストラクタ
			inline ~reverse_iterator()
			{}
		protected:
			//フィールド
			const container* m_con;//コンテナ
			mutable index_type m_index;//現在のインデックス
			mutable value_type* m_value;//現在の値
		};
	#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
	public:
		//--------------------
		//単一操作オブジェクト（安全なプッシュ／ポップ操作クラス）
		//※操作状態を記憶し、デストラクタで必ず完了させる
		class uniqueOperation
		{
		public:
			//型
			typedef container container_type;//コンテナ型
			typedef typename container::node_type node_type;//ノード型
			typedef typename container::status_t status_t;//ステータス型
		public:
			//アクセッサ
			status_t status() const { return m_status; }//ステータスを取得
		public:
			//プッシュ開始
			template<typename... Tx>
			node_type* pushBegin(Tx&&... args);
			
			//プッシュ終了
			node_type* pushEnd();
			
			//プッシュ取り消し
			bool pushCancel();
			
			//ポップ開始
			node_type* popBegin();
			
			//ポップ終了
			bool popEnd();
			
			//ポップ取り消し
			bool popCancel();
		public:
			//ムーブコンストラクタ
			inline uniqueOperation(uniqueOperation&& obj);
			//コピーコンストラクタ
			uniqueOperation(const uniqueOperation& obj) = delete;
			//コンストラクタ
			inline uniqueOperation(container_type& container);
			//デフォルトコンストラクタ
			uniqueOperation() = delete;
			//デストラクタ
			~uniqueOperation();
		private:
			//フィールド
			container_type& m_container;//コンテナ
			status_t m_status;//ステータス
		};
	public:
		//アクセッサ
		//※at(), []()は、値のポインタを返し、例外を発生させない点に注意
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後でロックの取得と解放を行う必要がある
		inline const node_type* at(const int index) const { return refNode(index); }
		//inline node_type* at(const int index){ return refNode(index); }//直接変更禁止
		inline const node_type* operator[](const int index) const { return refNode(index); }
		//inline node_type* operator[](const int index){ return refNode(index); }//直接変更禁止
		inline status_t status() const { return m_status; }
	public:
		//キャストオペレータ
		inline operator lock_type&(){ return m_lock; }//ロックオブジェクト
		inline operator lock_type&() const { return m_lock; }//ロックオブジェクト ※mutable
	public:
		//メソッド：ロック取得系
		//単一ロック取得
		inline GASHA_ unique_lock<lock_type> lockUnique() const { GASHA_ unique_lock<lock_type> lock(*this); return lock; }
		inline GASHA_ unique_lock<lock_type> lockUnique(const GASHA_ with_lock_t) const { GASHA_ unique_lock<lock_type> lock(*this, GASHA_ with_lock); return lock; }
		inline GASHA_ unique_lock<lock_type> lockUnique(const GASHA_ try_to_lock_t) const { GASHA_ unique_lock<lock_type> lock(*this, GASHA_ try_to_lock); return lock; }
		inline GASHA_ unique_lock<lock_type> lockUnique(const GASHA_ adopt_lock_t) const { GASHA_ unique_lock<lock_type> lock(*this, GASHA_ adopt_lock); return lock; }
		inline GASHA_ unique_lock<lock_type> lockUnique(const GASHA_ defer_lock_t) const { GASHA_ unique_lock<lock_type> lock(*this, GASHA_ defer_lock); return lock; }
		//スコープロック取得
		inline GASHA_ lock_guard<lock_type> lockScoped() const { GASHA_ lock_guard<lock_type> lock(*this); return lock; }
	public:
		//単一操作オブジェクト
		inline uniqueOperation operationUnique(){ uniqueOperation operation(*this); return operation; }
	public:
		//メソッド：イテレータ取得系
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後でロックの取得と解放を行う必要がある
		//イテレータ取得
		inline const iterator cbegin() const { iterator ite(*this, false); return ite; }
		inline const iterator cend() const { iterator ite(*this, true); return ite; }
		inline const iterator begin() const { iterator ite(*this, false); return ite; }
		inline const iterator end() const { iterator ite(*this, true); return ite; }
		inline iterator begin() { iterator ite(*this, false); return ite; }
		inline iterator end() { iterator ite(*this, true); return ite; }
	#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
		//リバースイテレータを取得
		inline const reverse_iterator crbegin() const { reverse_iterator ite(*this, false); return ite; }
		inline const reverse_iterator crend() const { reverse_iterator ite(*this, true); return ite; }
		inline const reverse_iterator rbegin() const { reverse_iterator ite(*this, false); return ite; }
		inline const reverse_iterator rend() const { reverse_iterator ite(*this, true); return ite; }
		inline reverse_iterator rbegin() { reverse_iterator ite(*this, false); return ite; }
		inline reverse_iterator rend() { reverse_iterator ite(*this, true); return ite; }
	#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
	private:
		//メソッド：要素アクセス系（独自拡張版）
		//※範囲チェックなし（非公開）
		inline const node_type* _refNode(const index_type index) const { return reinterpret_cast<const node_type*>(&m_table[index]); }//ノード参照
		inline const node_type* _refTop() const { return _refNode(0); }//先頭ノード参照
		inline const node_type* _refBottom() const { return _refNode(m_used - 1); }//終端ノード参照
		inline const node_type* _refNew() const { return _refNode(m_used); }//新規ノード参照
		inline node_type* _refNode(const index_type index){ return reinterpret_cast<node_type*>(&m_table[index]); }//ノード参照
		inline node_type* _refTop(){ return _refNode(0); }//先頭ノード参照
		inline node_type* _refBottom(){ return _refNode(m_used - 1); }//終端ノード参照
		inline node_type* _refNew(){ return _refNode(m_used); }//新規ノード参照
		//inline index_type _adjIndex(const index_type index) const { return index >= 0 && index < TABLE_SIZE ? index : INVALID_INDEX; }//インデックスを範囲内に補正
		inline index_type _adjIndex(const index_type index) const { return index < TABLE_SIZE ? index : INVALID_INDEX; }//インデックスを範囲内に補正
		inline index_type _refIndex(const node_type* node) const{ return node - _refTop(); }//ノードをインデックスに変換 ※範囲チェックなし
		inline index_type _calcParent(const index_type index) const;//親インデックス計算 ※範囲チェックなし
		inline index_type _calcChildL(const index_type index) const;//左側の子インデックス計算 ※範囲チェックなし
		inline index_type _calcChildR(const index_type index) const;//右側の子インデックス計算 ※範囲チェックなし
	public:
		//メソッド：要素アクセス系（独自拡張版）
		//※範囲チェックあり（公開）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）または
		//　排他ロック（ライトロック）の取得と解放を行う必要がある
		//inline const node_type* refNode(const index_type index) const { return index >= 0 && index < m_used ? _refNode(index) : nullptr; }//ノード参照
		inline const node_type* refNode(const index_type index) const { return index < m_used ? _refNode(index) : nullptr; }//ノード参照
		inline const node_type* refTop() const { return m_used == 0 ? nullptr : _refTop(); }//先頭ノード参照
		inline const node_type* refBottom() const { return m_used == 0 ? nullptr : _refBottom(); }//終端ノード参照
		inline const node_type* refNew() const { return m_used == TABLE_SIZE ? nullptr : _refNew(); }//新規ノード参照
		inline node_type* refNode(const index_type index){ return  const_cast<node_type*>(const_cast<const container*>(this)->refNode(index)); }//ノード参照
		inline node_type* refTop(){ return const_cast<node_type*>(const_cast<const container*>(this)->refTop()); }//先頭ノード参照
		inline node_type* refBottom(){ return const_cast<node_type*>(const_cast<const container*>(this)->refBottom()); }//終端ノード参照
		inline node_type* refNew(){ return const_cast<node_type*>(const_cast<const container*>(this)->refNew()); }//新規ノード参照
		inline index_type refIndex(const node_type* node) const{ return _adjIndex(_refIndex(node)); }//ノードをインデックスに変換
		inline index_type calcParent(const index_type index) const { return _adjIndex(_calcParent(index)); }//親インデックス計算
		inline index_type calcChildL(const index_type index) const { return _adjIndex(_calcChildL(index)); }//左側の子インデックス計算
		inline index_type calcChildR(const index_type index) const { return _adjIndex(_calcChildR(index)); }//右側の子インデックス計算
		inline index_type calc_child(const int index, const bool is_right) const { return is_right ? calcChildR(index) : calcChildL(index); }//子インデックス計算
		inline const node_type* refParent(const node_type* node) const { return refNode(_calcParent(_refIndex(node))); }//親ノード参照
		inline const node_type* refChildL(const node_type* node) const { return refNode(_calcChildL(_refIndex(node))); }//左側の子ノード参照
		inline const node_type* refChildR(const node_type* node) const { return refNode(_calcChildR(_refIndex(node))); }//右側の子ノード参照
		inline const node_type* refChild(const node_type* node, const bool is_right) const { return is_right ? refChildR(node) : refChildL(node); }//子ノード参照
		inline node_type* refParent(const node_type* node){ return const_cast<node_type*>(const_cast<const container*>(this)->refParent(node)); }//親ノード参照
		inline node_type* refChildL(const node_type* node){ return const_cast<node_type*>(const_cast<const container*>(this)->refChildL(node)); }//左側の子ノード参照
		inline node_type* refChildR(const node_type* node){ return const_cast<node_type*>(const_cast<const container*>(this)->refChildR(node)); }//左側の子ノード参照
		inline node_type* refChild(const node_type* node, const bool is_right){ return const_cast<node_type*>(const_cast<const container*>(this)->refChild(node, is_right)); }//子ノード参照
	public:
		//メソッド：基本情報系
		inline size_type max_size() const { return TABLE_SIZE; }//最大要素数を取得
		inline size_type capacity() const { return TABLE_SIZE; }//最大要素数を取得
		inline size_type size() const { return m_used; }//使用中の要素数を取得
		inline size_type remain() const { return TABLE_SIZE - m_used; }//残りの要素数を取得
		inline bool empty() const { return m_used == 0; }//空か？
		inline bool full() const { return m_used == TABLE_SIZE; }//満杯か？
	public:
		//メソッド：要素アクセス系
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）または
		//　排他ロック（ライトロック）の取得と解放を行う必要がある
		inline const node_type* top() const { return refTop(); }//先頭ノード参照
		inline node_type* top(){ return refTop(); }//先頭ノード参照
	public:
		//最大の深さを取得
		int maxDepth() const;
	private:
		//プッシュ（本体）：ムーブ
		node_type* _pushCopying(node_type&& src);
		
		//プッシュ（本体）：コピー
		node_type* _pushCopying(const node_type& src);
	public:
		//プッシュ
		//※オブジェクト渡し
		//※オブジェクトのコピーが発生する点に注意（少し遅くなる）
		//※ムーブオペレータを使用してコピーする点に注意
		//※処理中、ロックを取得する
		inline node_type* pushCopying(node_type&& src);
		inline node_type* pushCopying(const node_type& src);
	private:
		//プッシュ（本体）
		template<typename... Tx>
		node_type* _push(Tx&&... args);
	public:
		//プッシュ
		//※パラメータ渡し
		//※オブジェクトのコンストラクタが呼び出される
		//※処理中、ロックを取得する
		template<typename... Tx>
		inline node_type* push(Tx&&... args);
	private:
		//プッシュ開始（本体）
		template<typename... Tx>
		node_type* _pushBegin(Tx&&... args);
	public:
		//プッシュ開始
		//※空きノードを取得し、コンストラクタが呼び出される
		//※処理が成功すると、ロックを取得した状態になる（pushEndで解放する）
		template<typename... Tx>
		node_type* pushBegin(Tx&&... args);
	private:
		//プッシュ終了（本体）
		node_type* _pushEnd();
	public:
		//プッシュ終了
		//※追加した新規ノードを上に移動
		//※pushBeginで取得したロックを解放する
		node_type* pushEnd();
	private:
		//プッシュ取り消し（本体）
		bool _pushCancel();
	public:
		//プッシュ取り消し
		//※pushBeginで取得したロックを解放する
		bool pushCancel();
	private:
		//ポップ（本体）
		bool _popCopying(node_type& dst);
	public:
		//ポップ
		//※オブジェクトのコピーを受け取る領域を渡す
		//※オブジェクトのデストラクタが呼び出される ※コピー後に実行
		//※処理中、ロックを取得する
		inline bool popCopying(node_type& dst);
	private:
		//ポップ開始（本体）
		node_type* _popBegin();
	public:
		//ポップ開始
		//※根ノード取得
		//※処理が成功すると、ロックを取得した状態になる（popEndで解放する）
		node_type* popBegin();
	private:
		//ポップ終了（本体）
		bool _popEnd();
	public:
		//ポップ終了
		//※オブジェクトのデストラクタが呼び出される
		//※削除した根ノードの隙間を埋めるために、以降のノードを上に移動
		//※popBeginで取得したロックを解放する
		bool popEnd();
	private:
		//ポップ取り消し（本体）
		bool _popCancel();
	public:
		//ポップ取り消し
		//※popBeginで取得したロックを解放する
		bool popCancel();
	public:
		//ノードを上方に移動
		//※ロックを取得しないで処理するので注意！
		//　（局所的なロックで済む処理ではないため）
		//　必ず呼び出し元でロックを取得すること！
		inline node_type* upHeap(node_type* obj);
		
		//ノードを下方に移動
		//※ロックを取得しないで処理するので注意！
		//　（局所的なロックで済む処理ではないため）
		//　必ず呼び出し元でロックを取得すること！
		inline node_type* downHeap(node_type* obj);
	private:
		//クリア（本体）
		void _clear();
	public:
		//クリア
		//※処理中、ロックを取得する
		void clear();
	public:
		//デフォルトコンストラクタ
		container();
		//デストラクタ
		~container();
	private:
		//フィールド
		unsigned char m_table[TABLE_SIZE][sizeof(value_type)];//データテーブル
		size_type m_used;//使用数
		status_t m_status;//ステータス
		mutable lock_type m_lock;//ロックオブジェクト
	};
	//----------------------------------------
	//シンプル二分ヒープコンテナ
	//※操作用構造体の定義を省略してコンテナを使用するためのクラス。
	//※最も基本的な操作用構造体とそれに基づくコンテナ型を自動定義する。
	template<typename NODE_TYPE, std::size_t _TABLE_SIZE>
	class simpleContainer
	{
	public:
		//二分ヒープ操作用構造体
		struct ope : public baseOpe<ope, NODE_TYPE>{};

		//基本型定義
		DECLARE_OPE_TYPES(ope);

		//二分ヒープコンテナ
		class con : public container<ope_type, _TABLE_SIZE>
		{
		public:
		#ifdef GASHA_HAS_INHERITING_CONSTRUCTORS
			using container<ope_type, _TABLE_SIZE>::container;//継承コンストラクタ
		#else//GASHA_HAS_INHERITING_CONSTRUCTORS
			//デフォルトコンスタラクタ
			inline con() :
				container<ope_type, _TABLE_SIZE>()
			{}
		#endif//GASHA_HAS_INHERITING_CONSTRUCTORS
			//デストラクタ
			inline ~con()
			{}
		};
	};
	
	//--------------------
	//基本型定義マクロ消去
	#undef DECLARE_OPE_TYPES
}//namespace binary_heap

//--------------------
//クラスの別名
//※ネームスペースの指定を省略してクラスを使用するための別名

//※略称が bHeap では、「二項ヒープ」（Binomial Heap）と区別が付かないが、良しとする

//二分ヒープ操作用テンプレート構造体
template<class OPE_TYPE, typename NODE_TYPE>
using bHeap_baseOpe = binary_heap::baseOpe<OPE_TYPE, NODE_TYPE>;

//二分ヒープコンテナ
template<class NODE_TYPE, std::size_t _TABLE_SIZE>
using bHeap = binary_heap::container<NODE_TYPE, _TABLE_SIZE>;

//シンプル二分ヒープコンテナ
template<typename NODE_TYPE, std::size_t _TABLE_SIZE>
using simpleBHeap = binary_heap::simpleContainer<NODE_TYPE, _TABLE_SIZE>;

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/dynamic_array.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_BINARY_HEAP_ALLWAYS_TOGETHER_CPP_H
#include <gasha/dynamic_array.cpp.h>
#endif//GASHA_BINARY_HEAP_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_BINARY_HEAP_H

// End of file

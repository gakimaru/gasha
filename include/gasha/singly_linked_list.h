#pragma once
#ifndef __SINGLY_LINKED_LIST_H_
#define __SINGLY_LINKED_LIST_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// singly_linked_list.h
// 片方向連結リストコンテナ【宣言部】※データのメモリ管理を行わない擬似コンテナ
//
// ※コンテナをインスタンス化する際は、別途下記のファイルをインクルードする必要あり
//
//   ・singly_linked_list.inl   ... 【インライン関数／テンプレート関数定義部】
//                            コンテナクラスの操作が必要な場所でインクルード。
//   ・singly_linked_list.cpp.h ... 【関数定義部】
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

#include <gasha/dummy_shared_lock.h>//ダミー共有ロック
#include <gasha/lock_guard.h>//スコープロック
#include <gasha/shared_lock_guard.h>//スコープ共有ロック
#include <gasha/unique_shared_lock.h>//単一共有ロック

#include <cstddef>//std::size_t, std::ptrdiff_t用
#include <gasha/sort_basic.h>//ソート処理基本
#include <gasha/search_basic.h>//探索処理基本

//【VC++】例外を無効化した状態で <iterator> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <iterator>//std::iterator用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//片方向連結リスト（singly-linked list）
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//データ構造とアルゴリズム
//--------------------------------------------------------------------------------
//【特徴】
//・要素（ノード）ごとに独立したデータ（メモリ）を連結して管理する。
//・後部への連結を扱うため、リストの昇順アクセスが行える。
//--------------------------------------------------------------------------------
//【利点】
//・リストの昇順アクセスができる。
//・リストの先頭・末端への要素の挿入がほぼO(1)で行える。（制約：末端の処理が速いのは、末端を管理している場合のみ）
//・リストの先頭要素の削除がほぼO(1)で行える。（末端の削除は遅い）
//・リストの途中への要素の挿入がほぼO(1)で行える。（制約：指定要素の後ろへの挿入に限る）
//・リストの途中要素の削除がほぼO(1)で行える。（制約：指定要素の後ろの要素の削除に限る）
//・リストの要素数の上限を決めずに扱える。
//・双方向連結リストよりもメモリを節約できる。
//・双方向連結リストよりも若干速い。
//--------------------------------------------------------------------------------
//【欠点】
//・ランダムアクセスができない。
//・リストの降順アクセスができない。
//・探索が遅い。
//・リストの末端の削除ができない。（先頭から末端までリストを辿れば可能）
//・各要素に一つの連結情報を追加する必要がある。（メモリオーバーヘッドがある）
//・要素の追加・削除に対して、双方向連結リストよりも制約がある。
//--------------------------------------------------------------------------------
//【本プログラムにおける実装要件】
//・アルゴリズムとデータを分離した擬似コンテナとする。
//・ノードの連結情報をユーザー定義可能とする。
//　コンテナ操作用のユーザー定義構造体に、連結情報へのアクセスメソッドを実装することを必要とする。
//・コンテナを用いずとも、ノードの連結操作アルゴリズム（関数）のみの利用も可能とする。
//・コンテナ自体はデータの実体（ノード）を持たず、メモリ確保／解放を行わない。
//・データの実体（ノード）はコンテナの外部から受け取り、コンテナは先頭要素と末尾要素を管理する。
//・コンテナは、STLの std::forward_list (C++11)をモデルとしたインターフェースを実装する。
//・STL（std::forward_list）との主な違いは下記のとおり。
//    - ノードの生成／破棄を行わない。
//    - 例外を扱わない。そのため、イテレータへの isExist() メソッドの追加など、
//      インターフェースの相違点がある。
//    - （他のコンテナと同様に）コンテナ操作対象・方法を設定した
//      構造体をユーザー定義して用いる。
//    - メモリ操作を伴うメソッドの実装を見送り。具体的には、unique(),
//      emplace_front(), emplace_after() など。
//    - リスト全体を操作するメソッドの実装を（ひとまず）見送り。具体的には、
//      merge(), splice_after(), reverse() など。
//    - 利便性を考慮し、探索処理をメソッドとして実装。
//    - 利便性を考慮し、非効率なメソッドも実装する。具体的には、前ノードが
//      不明な要素の追加・削除、リバースイテレータなど。
//      基本的に非推奨メソッドであり、やむを得ない場合やデバッグ目的でのみ
//      使用可能とする。
//--------------------------------------------------------------------------------
//【想定する具的的な用途】
//・途中挿入・削除を多用するリスト。
//・複数種のキュー／スタックを同一ノードが渡り歩く必要がある場合。
//　　例：処理待ちリスト（キュー）に連結→処理中リストに再連結→削除待ちリストに再連結といった流れを扱う。
//    ※リストの逆順走査要件がなければ、双方向連結リストよりも片方向連結リストの方が良い。
//・片方向連結リストをスタックとして使用する際は、先頭へのプッシュと
//  先頭のポップで処理しなければならない点に注意。（末尾へのプッシュは不可）
//--------------------------------------------------------------------------------

namespace singly_linked_list
{
	//--------------------
	//片方向連結リスト操作用テンプレート構造体
	//※CRTPを活用し、下記のような派生構造体を作成して使用する
	//  //struct 派生構造体名 : public singly_linked_list::baseOpe_t<派生構造体名, ノードの型>
	//	struct ope_t : public singly_linked_list::baseOpe_t<ope_t, data_t>
	//	{
	//		//次ノードを取得
	//		inline static const node_type* getNext(const node_type& node){ return ???; }
	//		//次ノードを変更
	//		inline static void setNext(node_type& node, const node_type* next){ ??? = next; }
	//		
	//		//ソート用プレディケート関数オブジェクト
	//		//※必要に応じて実装する
	//		struct predicateForSort{
	//			inline bool operator()(const node_type& lhs, const node_type& rhs) const { return lhs.??? < rhs.???; }
	//		};
	//
	//		//線形探索用プレディケート関数オブジェクト
	//		//※必要に応じて実装する
	//		struct predicateForFind{
	//			inline bool operator()(const node_type& lhs, const ???& rhs) const { return lhs.??? == rhs; }
	//		};
	//		
	//		//ロック型 ※必要に応じて定義
	//		//※共有ロック（リード・ライトロック）でコンテナ操作をスレッドセーフにしたい場合は、
	//		//　有効な共有ロック型（shared_spin_lockなど）を lock_type 型として定義する。
	//		typedef shared_spin_lock lock_type;//ロックオブジェクト型
	//	};
	template<class OPE_TYPE, typename NODE_TYPE>
	struct baseOpe_t
	{
		//型
		typedef OPE_TYPE ope_type;//ノード操作型
		typedef NODE_TYPE node_type;//ノード型

		//ロック型
		typedef dummySharedLock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※共有ロック（リード・ライトロック）でコンテナ操作をスレッドセーフにしたい場合は、
		//　baseOpe_tの派生クラスにて、有効な共有ロック型（sharedSpinLock など）を
		//　lock_type 型として再定義する。

		//次ノードを取得 ※const外し(remove_const)
		inline static node_type* getNext_rc(node_type& node){ return const_cast<node_type*>(ope_type::getNext(const_cast<const node_type&>(node))); }

		//ソート用プレディケート関数オブジェクト
		//※trueでlhsの方が小さい（並び順が正しい）
		struct predicateForSort{
			inline bool operator()(const node_type& lhs, const node_type& rhs) const { return GASHA_ less<node_type>()(lhs, rhs); }
		};

		//探索用プレディケート関数オブジェクト
		//※trueで一致（探索成功）
		struct predicateForFind{
			template<typename V>
			inline bool operator()(const node_type& lhs, const V& rhs) const { return GASHA_ equal_to<node_type>()(lhs, rhs); }
		};

	#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_BINARY_SEARCH
		//探索用比較関数オブジェクト
		//※0で一致（探索成功）、1以上でlhsの方が大きい、-1以下でrhsの方が大きい
		struct comparisonForSearch{
			template<typename V>
			inline int operator()(const node_type& lhs, const V& rhs) const { return GASHA_ compare_to<node_type>()(lhs, rhs); }
		};
	#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_BINARY_SEARCH
	};
	
	//--------------------
	//基本型定義マクロ
	#define DECLARE_OPE_TYPES(OPE_TYPE) \
		typedef OPE_TYPE ope_type; \
		typedef typename ope_type::node_type node_type; \
		typedef node_type value_type; \
		typedef node_type& reference; \
		typedef const node_type& const_reference; \
		typedef node_type* pointer; \
		typedef const node_type* const_pointer; \
		typedef std::size_t size_type; \
		typedef std::size_t index_type; \
		typedef typename ope_type::lock_type lock_type;
	
	//--------------------
	//片方向連結リスト操作関数：指定ノードの次ノードを取得
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getNextNode(const typename OPE_TYPE::node_type& target);
	//--------------------
	//片方向連結リスト操作関数：指定ノードの後方のノードを取得
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getForwardNode(const typename OPE_TYPE::node_type& target, std::size_t& step);
	//--------------------
	//片方向連結リスト操作関数：指定ノードの末尾ノードを取得
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getLastNode(const typename OPE_TYPE::node_type& target);
	//--------------------
	//片方向連結リスト操作関数：指定ノードの数を数える
	template<class OPE_TYPE>
	std::size_t countNodes(const typename OPE_TYPE::node_type& target);
	//--------------------
	//片方向連結リスト操作関数：指定ノードの前ノードを取得
	//【注意】低速処理（backwardを指定することで高速化可能）
	//※もし、targetの直前もしくはそれより前のノードが分かっているなら、
	//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
	//　nullptrを指定した場合、firstからtargetまで辿る。
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getPrevNode(const typename OPE_TYPE::node_type& target, const typename OPE_TYPE::node_type* first, const typename OPE_TYPE::node_type* backward = nullptr);
	//--------------------
	//片方向連結リスト操作関数：指定ノードの前方のノードを取得
	//【注意】低速処理（backwardを指定することで高速化可能）
	//※もし、targetの直前もしくはそれより前のノードが分かっているなら、
	//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
	//　nullptrを指定した場合、firstからtargetまで辿る。
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getBackwardNode(const typename OPE_TYPE::node_type& target, std::size_t& step, const typename OPE_TYPE::node_type* first, const typename OPE_TYPE::node_type* backward = nullptr);
	//--------------------
	//片方向連結リスト操作関数：指定ノードの次に連結
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeAfter(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type* target, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last);
	//--------------------
	//片方向連結リスト操作関数：指定ノードの前に連結
	//【注意】低速処理（backwardを指定することで高速化可能）
	//※もし、targetの直前もしくはそれより前のノードが分かっているなら、
	//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
	//　nullptrを指定した場合、firstからtargetまで辿る。
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeBefore(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type& target, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last, const typename OPE_TYPE::node_type* backward = nullptr);
	//--------------------
	//片方向連結リスト操作関数：先頭に連結
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeBeginning(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last);
	//--------------------
	//片方向連結リスト操作関数：末尾に連結
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeEnd(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last);
	//--------------------
	//片方向連結リスト操作関数：指定ノードの次のノードを連結から外す
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* removeNodeAftere(typename OPE_TYPE::node_type* prev_target, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last);
	//--------------------
	//片方向連結リスト操作関数：指定ノードを連結から外す
	//【注意】低速処理（backwardを指定することで高速化可能）
	//※もし、nodeの直前もしくはそれより前のノードが分かっているなら、
	//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
	//　nullptrを指定した場合、firstからnodeまで辿る。
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* removeNode(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last, typename OPE_TYPE::node_type* backward = nullptr);
	//--------------------
	//片方向連結リスト操作関数：指定ノードの範囲を連結から外す
	//【注意】低速処理（backwardを指定することで高速化可能）
	//※もし、startの直前もしくはそれより前のノードが分かっているなら、
	//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
	//　nullptrを指定した場合、firstからstartまで辿る。
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* removeNodes(typename OPE_TYPE::node_type& start, typename OPE_TYPE::node_type& end, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last, typename OPE_TYPE::node_type* backward = nullptr);
	//----------------------------------------
	//双方向連結リスト操作関数：線形探索
	template<class OPE_TYPE, typename V>
	const typename OPE_TYPE::node_type* linearSearchValue(const typename OPE_TYPE::node_type* first, const V& value);
	template<class OPE_TYPE, typename V, class PREDICATE>
	const typename OPE_TYPE::node_type* linearSearchValue(const typename OPE_TYPE::node_type* first, const V& value, PREDICATE predicate);
	template<class OPE_TYPE, class PREDICATE>
	const typename OPE_TYPE::node_type* linearSearch(const typename OPE_TYPE::node_type* first, PREDICATE predicate);
	//----------------------------------------
	//双方向連結リスト操作関数：二分探索
	template<class OPE_TYPE, typename V>
	const typename OPE_TYPE::node_type* binarySearchValue(const typename OPE_TYPE::node_type* first, const V& value);
	template<class OPE_TYPE, typename V, class COMPARISON>
	const typename OPE_TYPE::node_type* binarySearchValue(const typename OPE_TYPE::node_type* first, const V& value, COMPARISON comparison);
	template<class OPE_TYPE, class COMPARISON>
	const typename OPE_TYPE::node_type* binarySearch(const typename OPE_TYPE::node_type* first, COMPARISON comparison);
	//----------------------------------------
	//双方向連結リスト操作関数：非整列状態確認
	template<class OPE_TYPE, class PREDICATE>
	std::size_t isUnordered(const typename OPE_TYPE::node_type* first);
	template<class OPE_TYPE, class PREDICATE>
	std::size_t isUnordered(const typename OPE_TYPE::node_type* first, PREDICATE predicate);
	//----------------------------------------
	//双方向連結リスト操作関数：整列状態確認
	template<class OPE_TYPE, class PREDICATE>
	std::size_t isOrdered(const typename OPE_TYPE::node_type* first);
	template<class OPE_TYPE, class PREDICATE>
	std::size_t isOrdered(const typename OPE_TYPE::node_type* first, PREDICATE predicate);
	//----------------------------------------
	//双方向連結リスト操作関数：非整列要素数計上
	template<class OPE_TYPE, class PREDICATE>
	std::size_t sumupUnordered(const typename OPE_TYPE::node_type* first);
	template<class OPE_TYPE, class PREDICATE>
	std::size_t sumupUnordered(const typename OPE_TYPE::node_type* first, PREDICATE predicate);
	//----------------------------------------
	//双方向連結リスト操作関数：挿入ソート
	template<class OPE_TYPE, class PREDICATE>
	std::size_t insertionSort(typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last);
	template<class OPE_TYPE, class PREDICATE>
	std::size_t insertionSort(typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last, PREDICATE predicate);

	//----------------------------------------
	//片方向連結リストコンテナ
	//※先頭ノードと末尾ノードを持つのみ
	//※std::forward_listを模した構造
	template<class OPE_TYPE>
	class container
	{
	public:
		//型
		DECLARE_OPE_TYPES(OPE_TYPE);
	public:
		//定数
		static const std::size_t BEFORE_BEGIN = 0xffffffff;//before_begin識別子
	public:
		//--------------------
		//イテレータ宣言
		class iterator;
		typedef const iterator const_iterator;
	#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
		class reverse_iterator;
		typedef const reverse_iterator const_reverse_iterator;
	#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
		//--------------------
		//イテレータ
		class iterator : public std::iterator<std::forward_iterator_tag, node_type>
		{
			friend class container;
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			friend class reverse_iterator;
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
		public:
			//※コンパイラによって優先して参照する型があいまいになることを避けるための定義
			typedef typename container::value_type value_type;
			typedef typename container::reverse_iterator reverse_iterator;
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const_reference() const { return *getValue(); }
			inline operator reference(){ return *getValue(); }
			inline operator const_pointer() const { return getValue(); }
			inline operator pointer(){ return getValue(); }
		public:
			//基本オペレータ
			inline const_reference operator*() const { return *getValue(); }
			inline reference operator*(){ return *getValue(); }
			inline const_pointer operator->() const { return getValue(); }
			inline pointer operator->(){ return getValue(); }
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE//std::forard_iterator_tag には本来必要ではない
			inline const iterator operator[](const int index) const;
			inline iterator operator[](const int index);
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//比較オペレータ
			inline bool operator==(const iterator& rhs) const;
			inline bool operator!=(const iterator& rhs) const;
		public:
			//演算オペレータ
			inline const iterator& operator++() const;
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			inline const iterator& operator--() const;
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			inline iterator& operator++();
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			inline iterator& operator--();
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			inline const iterator operator++(int) const;
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			inline const iterator operator--(int) const;
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			inline iterator operator++(int);
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			inline iterator operator--(int);
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
			inline const iterator& operator+=(const int rhs) const;
			inline const iterator& operator+=(const std::size_t rhs) const { return operator+=(static_cast<int>(rhs)); }
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			inline const iterator& operator-=(const int rhs) const;
			inline const iterator& operator-=(const std::size_t rhs) const { return operator-=(static_cast<int>(rhs)); }
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			inline iterator& operator+=(const int rhs);
			inline iterator& operator+=(const std::size_t rhs) { return operator+=(static_cast<int>(rhs)); }
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			inline iterator& operator-=(const int rhs);
			inline iterator& operator-=(const std::size_t rhs) { return operator-=(static_cast<int>(rhs)); }
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			inline const iterator operator+(const int rhs) const;
			inline const iterator operator+(const std::size_t rhs) const { return operator+(static_cast<int>(rhs)); }
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			inline const iterator operator-(const int rhs) const;
			inline const iterator operator-(const std::size_t rhs) const { return operator-(static_cast<int>(rhs)); }
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			inline iterator operator+(const int rhs);
			inline iterator operator+(const std::size_t rhs) { return operator+(static_cast<int>(rhs)); }
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			inline iterator operator-(const int rhs);
			inline iterator operator-(const std::size_t rhs) { return operator-(static_cast<int>(rhs)); }
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			//inline int operator-(const iterator& rhs) const;
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//アクセッサ
			inline bool isExist() const;
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const;
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isBeforeBegin() const;
			inline bool isEnd() const;//終端か？
			inline const value_type* getValue() const;//現在の値（ノード）
			inline value_type* getValue();//現在値の（ノード）
		private:
			//メソッド
			//参照を更新
			void updateNext() const;
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			void updatePrev() const;
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			void updateForward(const std::size_t step) const;
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			void updateBackward(const std::size_t step) const;
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			void updateBeforeBegin() const;
		public:
			//ムーブオペレータ
			iterator& operator=(const iterator&& rhs);
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			iterator& operator=(const reverse_iterator&& rhs);
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			//コピーオペレータ
			iterator& operator=(const iterator& rhs);
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			iterator& operator=(const reverse_iterator& rhs);
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
		public:
			//ムーブコンストラクタ
			iterator(const iterator&& obj);
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			iterator(const reverse_iterator&& obj);
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			//コピーコンストラクタ
			iterator(const iterator& obj);
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
			iterator(const reverse_iterator& obj);
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
		public:
			//コンストラクタ
			iterator(const container& con, const bool is_end);
			iterator(const container& con, value_type* value, const bool is_end);
			//デフォルトコンストラクタ
			inline iterator() :
				m_con(nullptr),
				m_value(nullptr),
				m_isEnd(false)
			{}
			//デストラクタ
			inline ~iterator()
			{}
		protected:
			//フィールド
			const container* m_con;//コンテナ
			mutable value_type* m_value;//現在のノード
			mutable bool m_isEnd;//終端か？
		};
	#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
		//--------------------
		//リバースイテレータ
		//class reverse_iterator : public std::reverse_iterator<iterator>
		class reverse_iterator : public std::iterator<std::forward_iterator_tag, node_type>
		{
			friend class container;
			friend class iterator;
		public:
			//※コンパイラによって優先して参照する型があいまいになることを避けるための定義
			typedef typename container::value_type value_type;
			typedef typename container::iterator iterator;
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const_reference() const { return *getValue(); }
			inline operator reference(){ return *getValue(); }
			inline operator const_pointer() const { return getValue(); }
			inline operator pointer(){ return getValue(); }
		public:
			//基本オペレータ
			inline const_reference operator*() const { return *getValue(); }
			inline reference operator*(){ return *getValue(); }
			inline const_pointer operator->() const { return getValue(); }
			inline pointer operator->(){ return getValue(); }
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
			inline const reverse_iterator operator[](const int index) const;
			inline reverse_iterator operator[](const int index);
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//比較オペレータ
			inline bool operator==(const reverse_iterator& rhs) const;
			inline bool operator!=(const reverse_iterator& rhs) const;
		public:
			//演算オペレータ
			inline const reverse_iterator& operator++() const;
			inline const reverse_iterator& operator--() const;
			inline reverse_iterator& operator++();
			inline reverse_iterator& operator--();
			inline const reverse_iterator operator++(int) const;
			inline const reverse_iterator operator--(int) const;
			inline reverse_iterator operator++(int);
			inline reverse_iterator operator--(int);
		#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
			inline const reverse_iterator& operator+=(const int rhs) const;
			inline const reverse_iterator& operator+=(const std::size_t rhs) const { return operator+=(static_cast<int>(rhs)); }
			inline const reverse_iterator& operator-=(const int rhs) const;
			inline const reverse_iterator& operator-=(const std::size_t rhs) const { return operator-=(static_cast<int>(rhs)); }
			inline reverse_iterator& operator+=(const int rhs);
			inline reverse_iterator& operator+=(const std::size_t rhs) { return operator+=(static_cast<int>(rhs)); }
			inline reverse_iterator& operator-=(const int rhs);
			inline reverse_iterator& operator-=(const std::size_t rhs) { return operator-=(static_cast<int>(rhs)); }
			inline const reverse_iterator operator+(const int rhs) const;
			inline const reverse_iterator operator+(const std::size_t rhs) const { return operator+(static_cast<int>(rhs)); }
			inline const reverse_iterator operator-(const int rhs) const;
			inline const reverse_iterator operator-(const std::size_t rhs) const { return operator-(static_cast<int>(rhs)); }
			inline reverse_iterator operator+(const int rhs);
			inline reverse_iterator operator+(const std::size_t rhs) { return operator+(static_cast<int>(rhs)); }
			inline reverse_iterator operator-(const int rhs);
			inline reverse_iterator operator-(const std::size_t rhs) { return operator-(static_cast<int>(rhs)); }
			//inline int operator-(const reverse_iterator& rhs);
		#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//アクセッサ
			inline bool isExist() const;
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const;
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const;//終端か？
			inline const value_type* getValue() const;//現在の値（ノード）
			inline value_type* getValue();//現在の値（ノード）
		public:
			//ベースを取得
			inline const iterator base() const;
			inline iterator base();
		public:
			//メソッド
			//参照を更新
			void updateNext() const;
			void updatePrev() const;
			void updateForward(const std::size_t step) const;
			void updateBackward(const std::size_t step) const;
		public:
			//ムーブオペレータ
			reverse_iterator& operator=(const reverse_iterator&& rhs);
			reverse_iterator& operator=(const iterator&& rhs);
			//コピーオペレータ
			reverse_iterator& operator=(const reverse_iterator& rhs);
			reverse_iterator& operator=(const iterator& rhs);
		public:
			//ムーブコンストラクタ
			reverse_iterator(const reverse_iterator&& obj);
			reverse_iterator(const iterator&& obj);
			//コピーコンストラクタ
			reverse_iterator(const reverse_iterator& obj);
			reverse_iterator(const iterator& obj);
		public:
			//コンストラクタ
			reverse_iterator(const container& con, const bool is_end);
			reverse_iterator(const container& con, value_type* value, const bool is_end);
			//デフォルトコンストラクタ
			inline reverse_iterator() :
				m_con(nullptr),
				m_value(nullptr),
				m_isEnd(false)
			{}
			//デストラクタ
			inline ~reverse_iterator()
			{}
		protected:
			//フィールド
			const container* m_con;//コンテナ
			mutable value_type* m_value;//現在の値（ノード）
			mutable bool m_isEnd;//終端か？
		};
	#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	public:
		//アクセッサ
	#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
		//※at(), []()は、ノードのポインタを返し、例外を発生させない点に注意
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）または
		//　排他ロック（ライトロック）の取得と解放を行う必要がある
		const node_type* at(const index_type index) const
		{
			return getForwardNode<ope_type>(m_first, index);
		}
		inline node_type* at(const index_type index){ return const_cast<node_type*>(const_cast<const container*>(this)->at(index)); }
		inline const node_type* operator[](const index_type index) const { return at(index); }
		inline node_type* operator[](const index_type index){ return at(index); }
	#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
	public:
		//メソッド：ロック取得系
		//単一ロック取得
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(){ GASHA_ unique_shared_lock<lock_type> lock(*this); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ with_lock_t){ GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ with_lock); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ with_lock_shared_t){ GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ with_lock_shared); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ try_lock_t){ GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ try_lock); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ try_lock_shared_t){ GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ try_lock_shared); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ adopt_lock_t){ GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ adopt_lock); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ adopt_shared_lock_t){ GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ adopt_shared_lock); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ defer_lock_t){ GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ defer_lock); return lock; }
		//スコープロック取得
		inline GASHA_ lock_guard<lock_type> lockScoped(){ GASHA_ lock_guard<lock_type> lock(*this); return lock; }
		inline GASHA_ shared_lock_guard<lock_type> lockSharedScoped(){ GASHA_ shared_lock_guard<lock_type> lock(*this); return lock; }
	public:
		//メソッド：イテレータ取得系
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）または
		//　排他ロック（ライトロック）の取得と解放を行う必要がある
		//イテレータ取得
		inline const iterator cbefore_begin() const { iterator ite(*this, false); ite.updateBeforeBegin(); return std::move(ite); }
		inline const iterator cbegin() const { iterator ite(*this, false); return ite; }
		inline const iterator cend() const { iterator ite(*this, true); return ite; }
		inline const_iterator before_begin() const { iterator ite(*this, false); ite.updateBeforeBegin(); return std::move(ite); }
		inline const iterator begin() const { iterator ite(*this, false); return ite; }
		inline const iterator end() const { iterator ite(*this, true); return ite; }
		inline iterator before_begin() { iterator ite(*this, false); ite.updateBeforeBegin(); return std::move(ite); }
		inline iterator begin() { iterator ite(*this, false); return ite; }
		inline iterator end() { iterator ite(*this, true); return ite; }
	#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
		//リバースイテレータを取得
		inline const reverse_iterator crbegin() const { reverse_iterator ite(*this, false); return ite; }
		inline const reverse_iterator crend() const { reverse_iterator ite(*this, true); return ite; }
		inline const reverse_iterator rbegin() const { reverse_iterator ite(*this, false); return ite; }
		inline const reverse_iterator rend() const { reverse_iterator ite(*this, true); return ite; }
		inline reverse_iterator rbegin() { reverse_iterator ite(*this, false); return ite; }
		inline reverse_iterator rend() { reverse_iterator ite(*this, true); return ite; }
	#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	public:
		//メソッド：基本情報系
		//inline std::size_t max_size() const { return (不定); }
		//inline std::size_t capacity() const { return (不定); }
		inline std::size_t size() const { return m_first ? countNodes<ope_type>(*m_first) : 0; }//ノード数を取得
		inline bool empty() const { return m_first == nullptr; }//空か？
	public:
		//メソッド：要素アクセス系
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）または
		//　排他ロック（ライトロック）の取得と解放を行う必要がある
		inline node_type* front(){ return m_first; }//先頭ノードを参照
		inline node_type* back(){ return m_last; }//末尾ノードを参照
		inline const node_type* front() const { return m_first; }//先頭ノードを参照
		inline const node_type* back() const { return m_last; }//末尾ノードを参照
		inline node_type*& firstRef(){ return m_first; }//先頭ノードの参照を取得
		inline node_type*& lastRef(){ return m_last; }//末尾ノードの参照を取得
	public:
		//追加／削除系メソッド
		//※std::forward_listと異なり、追加／削除対象のノードを直接指定し、結果をポインタで受け取る（成功したら、追加／削除したポインタを返す）
		//※要素のメモリ確保／解放を行わない点に注意
		//※assign(), remove_if(), emplace_after(), emprace_front()には非対応
		//※insert_before()を追加
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		//先頭にノードを挿入（連結に追加）
		inline node_type* push_front(const node_type& node)
		{
			return insertNodeBeginning<ope_type>(*const_cast<node_type*>(&node), m_first, m_last);
		}
		//末尾にノードを挿入（連結に追加）
		//※std::forward_listにないメソッド
		inline node_type* push_back(const node_type& node)
		{
			return insertNodeEnd<ope_type>(*const_cast<node_type*>(&node), m_first, m_last);
		}
		//先頭ノードを削除（連結解除）
		inline node_type* pop_front()
		{
			if (!m_first)
				return nullptr;
			return removeNode<ope_type>(*m_first, m_first, m_last, nullptr);
		}
		//末尾ノードを削除（連結解除）
		//【注意】低速処理（backwardを指定することで高速化可能）
		//※もし、lastの直前もしくはそれより前のノードが分かっているなら、
		//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
		//　nullptrを指定した場合、firstからlastまで辿る。
		//※std::forward_listにないメソッド
		inline node_type* pop_back(node_type* backward = nullptr)
		{
			if (!m_first)
				return nullptr;
			return removeNode<ope_type>(*m_last, m_first, m_last, backward);
		}
		//指定の位置の後ろにノードを挿入（連結に追加）
		//※位置はイテレータで指定
		inline node_type* insert_after(iterator pos, const node_type& node)
		{
			if (pos.isNotExist() && !pos.isBeforeBegin())
				return nullptr;
			return insertNodeAfter<ope_type>(*const_cast<node_type*>(&node), *pos, m_first, m_last);
		}
		//※insert_after()とinsert()は同じ
		//※std::forward_listにないメソッド
		inline node_type* insert(iterator pos, const node_type& node)
		{
			if (pos.isNotExist() && !pos.isBeforeBegin())
				return nullptr;
			return insertNodeAfter<ope_type>(*const_cast<node_type*>(&node), *pos, m_first, m_last);
		}
		//指定の位置の前にノードを挿入（連結に追加）
		//※位置はイテレータで指定
		//【注意】低速処理（backwardを指定することで高速化可能）
		//※もし、posの直前もしくはそれより前のノードが分かっているなら、
		//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
		//　nullptrを指定した場合、firstからposまで辿る。
		//※std::forward_listにないメソッド
		inline node_type* insert_before(iterator pos, const node_type& node, node_type* backward = nullptr)
		{
			if (pos.isNotExist())
				return nullptr;
			return insertNodeBefore<ope_type>(*const_cast<node_type*>(&node), *pos, m_first, m_last, backward);
		}
		//指定ノードを削除（連結解除）
		//【注意】低速処理（backwardを指定することで高速化可能）
		//※もし、nodeの直前もしくはそれより前のノードが分かっているなら、
		//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
		//　nullptrを指定した場合、firstからnodeまで辿る。
		inline node_type* remove(node_type& node, node_type* backward = nullptr)
		{
			if (!m_first)
				return nullptr;
			return removeNode<ope_type>(node, m_first, m_last, backward);
		}
		//指定位置の次のノードを削除（連結解除）
		//※位置はイテレータで指定
		inline node_type* erase_after(iterator pos)
		{
			if (!m_first || pos.isNotExist())
				return nullptr;
			node_type* before = &*pos;
			node_type& _pos = *const_cast<node_type*>(ope_type::getNext(*pos));
			return removeNode<ope_type>(_pos, m_first, m_last, before);
		}
		//指定位置の次のノードからの範囲のノードを削除（連結解除）
		//※範囲はイテレータで指定
		inline node_type* erase_after(iterator start, iterator end)
		{
			if (!m_first || start.isNotExist() || end.isNotEnabled())
				return nullptr;
			node_type* before = &*start;
			node_type& _start = *const_cast<node_type*>(ope_type::getNext(*start));
			return removeNodes<ope_type>(_start, *end, m_first, m_last, before);
		}
		//指定位置のノードを削除（連結解除）
		//※位置はイテレータで指定
		//【注意】低速処理（backwardを指定することで高速化可能）
		//※もし、posの直前もしくはそれより前のノードが分かっているなら、
		//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
		//　nullptrを指定した場合、firstからposまで辿る。
		//※std::forward_listにないメソッド
		inline node_type* erase(iterator pos, node_type* backward = nullptr)
		{
			if (!m_first || pos.isNotExist())
				return nullptr;
			return removeNode<ope_type>(*pos, m_first, m_last, backward);
		}
		//指定範囲のノードを削除（連結解除）
		//※範囲はイテレータで指定
		//【注意】低速処理（backwardを指定することで高速化可能）
		//※もし、startの直前もしくはそれより前のノードが分かっているなら、
		//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
		//　nullptrを指定した場合、firstからstartまで辿る。
		//※std::forward_listにないメソッド
		inline node_type* erase(iterator start, iterator end, node_type* backward = nullptr)
		{
			if (!m_first || start.isNotExist() || end.isNotEnabled())
				return nullptr;
			return removeNodes<ope_type>(*start, *end, m_first, m_last, backward);
		}
		//全ノードをクリア
		//※先頭ノードを返す
		inline node_type* clear()
		{ 
			node_type* first = m_first;
			m_first = nullptr;
			m_last = nullptr;
			return first;
		}
	public:
		//ソート
		//※挿入ソートを使用（シェルソートではあまり速度が上がらないため）
		//※ope_type::predicateForSort() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		void sort()
		{
			insertionSort<OPE_TYPE>(m_first, m_last, typename ope_type::predicateForSort());
		}
		//※プレディケート関数指定版
		template<class PREDICATE>
		void sort(PREDICATE predicate)
		{
			insertionSort<OPE_TYPE>(m_first, m_last, predicate);
		}
	#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_STABLE_SORT
		//安定ソート
		//※挿入ソートを使用
		//※ope_type::predicateForSort() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		void stableSort()
		{
			insertionSort<OPE_TYPE>(m_first, m_last, typename ope_type::predicateForSort());
		}
		//※プレディケート関数指定版
		template<class PREDICATE>
		void stableSort(PREDICATE predicate)
		{
			insertionSort<OPE_TYPE>(m_first, m_last, predicate);
		}
	#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_STABLE_SORT
		//ソート済み状態チェック
		//※ope_type::predicateForSort() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		bool isOrdered() const
		{
			return isOrdered<OPE_TYPE>(m_first, typename ope_type::predicateForSort());
		}
		//※プレディケート関数指定版
		template<class PREDICATE>
		bool isOrdered(PREDICATE predicate) const
		{
			return isOrdered<OPE_TYPE>(m_first, predicate);
		}
	public:
		//線形探索
		//※探索値指定版
		//※ope_type::predicateForFind() を使用して探索（標準では、データ型の operator==() に従って探索）
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		template<typename V>
		iterator findValue(const V& value)
		{
			const node_type* found_node = linearSearchValue(m_first, value, typename ope_type::predicateForFind());
			iterator found(*this, found_node, found_node == nullptr);
			return found;
		}
		//※比較関数＋値指定版
		template<typename V, class PREDICATE>
		iterator findValue(const V& value, PREDICATE predicate)
		{
			const node_type* found_node = linearSearchValue(m_first, value, predicate);
			iterator found(*this, found_node, found_node == nullptr);
			return found;
		}
		//※比較関数指定版
		//※値の指定は関数に含んでおく（クロ―ジャを用いるなどする）
		template<class PREDICATE>
		iterator find(PREDICATE predicate)
		{
			const node_type* found_node = linearSearch(m_first, predicate);
			iterator found(*this, found_node, found_node == nullptr);
			return found;
		}
	#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_BINARY_SEARCH
		//二分探索
		//※探索値指定版
		//※ope_type::comparisonForSearch() を使用して探索（標準では、データ型の operator==() と operator<() に従って探索）
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		//【注意】低速処理
		template<typename V>
		iterator binarySearchValue(const V& value)
		{
			const node_type* found_node = binarySearchValue(m_first, value, typename ope_type::comparisonForSearch());
			iterator found(*this, found_node, found_node == nullptr);
			return found;
		}
		//※比較関数＋値指定版
		template<typename V, class COMPARISON>
		iterator binarySearchValue(const V& value, COMPARISON comparison)
		{
			const node_type* found_node = binarySearchValue(m_first, value, comparison);
			iterator found(*this, found_node, found_node == nullptr);
			return found;
		}
		//※比較関数指定版
		//※値の指定は関数に含んでおく（クロ―ジャを用いるなどする）
		template<class COMPARISON>
		iterator binary_search(COMPARISON comparison)
		{
			const node_type* found_node = binarySearch(m_first, comparison);
			iterator found(*this, found_node, found_node == nullptr);
			return found;
		}
	#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_BINARY_SEARCH

		//リスト操作系メソッド
		//※merge(), splice_after(), reverse(), unique(),  emplace_front(), emplace_after() には非対応

	public:
		//ムーブコンストラクタ
		container(const container&& con) :
			m_first(con.m_first),
			m_last(con.m_last)
		{}
		//コピーコンストラクタ
		container(const container& con) :
			m_first(con.m_first),
			m_last(con.m_last)
		{}
		//コンストラクタ
		container() :
			m_first(nullptr),
			m_last(nullptr)
		{}
		//デストラクタ
		~container()
		{}
	private:
		//フィールド
		node_type* m_first;//先頭ノード
		node_type* m_last;//末尾ノード
		mutable lock_type m_lock;//ロックオブジェクト
	};

	//--------------------
	//基本型定義マクロ消去
	#undef DECLARE_OPE_TYPES
}//namespace singly_linked_list

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルードする場合
#ifdef GASHA_SINGLY_LINKED_LIST_ALLWAYS_TOGETHER_INL
#include <gasha/singly_linked_list.inl>
#endif//GASHA_SINGLY_LINKED_LIST_ALLWAYS_TOGETHER_INL

//.hファイルのインクルードに伴い、常に.cp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_SINGLY_LINKED_LIST_ALLWAYS_TOGETHER_CPP_H
#include <gasha/singly_linked_list.cpp.h>
#endif//GASHA_SINGLY_LINKED_LIST_ALLWAYS_TOGETHER_CPP_H

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__SINGLY_LINKED_LIST_H_

// End of file

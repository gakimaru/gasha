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
//#include <cstdint>//std::intptr_t用
#include <gasha/sort_basic.h>//ソート処理基本
#include <gasha/search_basic.h>//探索処理基本

//【VC++】例外を無効化した状態で <iterator> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <iterator>//std::iterator用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//片方向連結リスト（double-linked list）
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//【本プログラムにおける実装要件】
//・テンプレートにより、アルゴリズムを汎用化。
//・リンクの方法に柔軟性を持たせ、かつ、virtualを必須としないように、
//　データ構造は自由とする。
//　（リンクの変数をポインタにしようがインデックスにしようが自由）
//・代わりに、データ操作専用のクラスをユーザー定義することで、
//　処理に汎用性を持たせる。
//・一切メモリ操作は行わず、ノードのオブジェクトを受け取って操作する
//　アルゴリズムのみを提供する。
//・std::forward_listを模したコンテナとイテレータを利用可能とする。
//　ただし、メモリ管理を行わないため、複製を作るようなメソッドには対応しない。
//・コンテナは、先頭ノードと末尾ノードのみを保持し、一切メモリ操作しないものとする。
//--------------------------------------------------------------------------------

//#include <cstddef>//srd::size_t用
//#include <iterator>//std::iterator用
//#include <algorithm>//C++11 std::move用

namespace singly_linked_list
{
#if 0
	//--------------------
	//片方向連結リストノード操作用基底テンプレートクラス
	//※下記のような派生クラス（CRTP）を定義して使用する
	//  //struct クラス名 : public singly_linked_list::baseOpe_t<クラス名, ノード型>
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
		typedef dummy_shared_lock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※共有ロック（リード・ライトロック）でコンテナ操作をスレッドセーフにしたい場合は、
		//　baseOpe_tの派生クラスにて、有効な共有ロック型（shared_spin_lock など）を
		//　lock_type 型として再定義する。

		//次ノードを取得 ※const外し(remove_const)
		inline static node_type* getNext_rc(node_type& node){ return const_cast<node_type*>(ope_type::getNext(const_cast<const node_type&>(node))); }

		//ソート用プレディケート関数オブジェクト
		//※trueでlhsの方が小さい（並び順が正しい）
		struct predicateForSort{
			inline bool operator()(const node_type& lhs, const node_type& rhs) const { return less<node_type>()(lhs, rhs); }
		};

		//探索用プレディケート関数オブジェクト
		//※trueで一致（探索成功）
		struct predicateForFind{
			template<typename V>
			inline bool operator()(const node_type& lhs, const V& rhs) const { return equal_to<node_type>()(lhs, rhs); }
		};

	#ifdef ENABLE_BINARY_SEARCH
		//探索用比較関数オブジェクト
		//※0で一致（探索成功）、1以上でlhsの方が大きい、-1以下でrhsの方が大きい
		struct comparisonForSearch{
			template<typename V>
			inline int operator()(const node_type& lhs, const V& rhs) const { return compare_to<node_type>()(lhs, rhs); }
		};
	#endif//ENABLE_BINARY_SEARCH
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
	const typename OPE_TYPE::node_type* getNextNode(const typename OPE_TYPE::node_type& target)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		const typename ope_type::node_type* node = &target;
		node = ope_type::getNext(*node);
		return node;
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードの後方のノードを取得
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getForwardNode(const typename OPE_TYPE::node_type& target, std::size_t& step)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		const typename ope_type::node_type* node = &target;
		while (step > 0 && node)
		{
			node = ope_type::getNext(*node);
			--step;
		}
		return node;
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードの末尾ノードを取得
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getLastNode(const typename OPE_TYPE::node_type& target)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		const typename ope_type::node_type* node = &target;
		while (true)
		{
			const typename ope_type::node_type* next = ope_type::getNext(*node);
			if (!next)
				return node;
			node = next;
		}
		return nullptr;//ダミー
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードの数を数える
	template<class OPE_TYPE>
	std::size_t countNodes(const typename OPE_TYPE::node_type& target)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		std::size_t num = 1;
		const typename ope_type::node_type* node = &target;
		while (true)
		{
			const typename ope_type::node_type* next = ope_type::getNext(*node);
			if (!next)
				return num;
			++num;
			node = next;
		}
		return 0;//ダミー
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードの前ノードを取得
	//【注意】低速処理（backwardを指定することで高速化可能）
	//※もし、targetの直前もしくはそれより前のノードが分かっているなら、
	//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
	//　nullptrを指定した場合、firstからtargetまで辿る。
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getPrevNode(const typename OPE_TYPE::node_type& target, const typename OPE_TYPE::node_type* first, const typename OPE_TYPE::node_type* backward = nullptr)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		const typename ope_type::node_type* prev = nullptr;
		const typename ope_type::node_type* node = backward;
		if (!node)
			node = first;
		while (node != &target)
		{
			prev = node;
			node = ope_type::getNext(*node);
		}
		return prev;
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードの前方のノードを取得
	//【注意】低速処理（backwardを指定することで高速化可能）
	//※もし、targetの直前もしくはそれより前のノードが分かっているなら、
	//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
	//　nullptrを指定した場合、firstからtargetまで辿る。
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getBackwardNode(const typename OPE_TYPE::node_type& target, std::size_t& step, const typename OPE_TYPE::node_type* first, const typename OPE_TYPE::node_type* backward = nullptr)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		const typename ope_type::node_type* node = &target;
		while (step > 0 && node)
		{
			node = getPrevNode<OPE_TYPE>(*node, first, backward);
			--step;
		}
		return node;
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードの次に連結
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeAfter(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type& target, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		if (&target == nullptr)
		{
			ope_type::setNext(node, first);
			first = &node;
			return &node;
		}
		typename ope_type::node_type* next = const_cast<typename ope_type::node_type*>(ope_type::getNext(target));
		if (!next)
			last = &node;
		ope_type::setNext(target, &node);
		ope_type::setNext(node, next);
		return &node;
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードの前に連結
	//【注意】低速処理（backwardを指定することで高速化可能）
	//※もし、targetの直前もしくはそれより前のノードが分かっているなら、
	//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
	//　nullptrを指定した場合、firstからtargetまで辿る。
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeBefore(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type& target, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last, const typename OPE_TYPE::node_type* backward = nullptr)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		typename ope_type::node_type* prev = const_cast<typename ope_type::node_type*>(getPrevNode<OPE_TYPE>(target, first, backward));
		if (!prev)
			first = &node;
		else
			ope_type::setNext(*prev, &node);
		ope_type::setNext(node, &target);
		return &node;
	}
	//--------------------
	//片方向連結リスト操作関数：先頭に連結
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeBeginning(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		if (!first)
		{
			first = &node;
			last = &node;
			ope_type::setNext(node, nullptr);
			return &node;
		}
		ope_type::setNext(node, first);
		first = &node;
		return &node;
	}
	//--------------------
	//片方向連結リスト操作関数：末尾に連結
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeEnd(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		if (!last)
		{
			first = &node;
			last = &node;
			ope_type::setNext(node, nullptr);
			return &node;
		}
		ope_type::setNext(*last, &node);
		ope_type::setNext(node, nullptr);
		last = &node;
		return &node;
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードを連結から外す
	//【注意】低速処理（backwardを指定することで高速化可能）
	//※もし、nodeの直前もしくはそれより前のノードが分かっているなら、
	//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
	//　nullptrを指定した場合、firstからnodeまで辿る。
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* removeNode(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last, typename OPE_TYPE::node_type* backward = nullptr)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		typename ope_type::node_type* prev = const_cast<typename ope_type::node_type*>(getPrevNode<OPE_TYPE>(node, first, backward));
		typename ope_type::node_type* next = const_cast<typename ope_type::node_type*>(ope_type::getNext(node));
		if (prev)
			ope_type::setNext(*prev, next);
		else
			first = next;
		if (!next)
			last = prev;
		ope_type::setNext(node, nullptr);
		return &node;
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードの範囲を連結から外す
	//【注意】低速処理（backwardを指定することで高速化可能）
	//※もし、startの直前もしくはそれより前のノードが分かっているなら、
	//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
	//　nullptrを指定した場合、firstからstartまで辿る。
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* removeNodes(typename OPE_TYPE::node_type& start, typename OPE_TYPE::node_type& end, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last, typename OPE_TYPE::node_type* backward = nullptr)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		typename ope_type::node_type* prev = const_cast<typename ope_type::node_type*>(getPrevNode<OPE_TYPE>(start, first, backward));
		typename ope_type::node_type* next = &end;
		typename ope_type::node_type* _end = next ? const_cast<typename ope_type::node_type*>(getPrevNode<OPE_TYPE>(end, first, &start)) : last;
		if (prev)
			ope_type::setNext(*prev, next);
		else
			first = next;
		if (!next)
			last = prev;
		ope_type::setNext(*_end, nullptr);
		return &start;
	}

	//========================================
	//ソートアルゴリズム
	//========================================

	//----------------------------------------
	//比較ソート処理オーバーロード関数用マクロ
	//※連結リスト対応版
	//※標準プレディケート関数使用版
	#define liskedListSortFuncSetByDefaultPredicate(func_name) \
		template<class OPE_TYPE> \
		inline std::size_t func_name(typename OPE_TYPE::node_type* first) \
		{ \
			typedef typename OPE_TYPE::node_type node_type; \
			return func_name<OPE_TYPE>(first, less<node_type>()); \
		}
	#define linkedListSortFuncSet(func_name) \
		liskedListSortFuncSetByDefaultPredicate(func_name)

	//----------------------------------------
	//整列状態確認 ※連結リスト対応版
	template<class OPE_TYPE, class PREDICATE>
	inline std::size_t linkedListCalcUnordered(const typename OPE_TYPE::node_type* start, PREDICATE predicate)
	{
		typedef typename OPE_TYPE::node_type node_type;
		std::size_t unordered = 0;
		const node_type* prev = start;
		const node_type* now = OPE_TYPE::getNext(*prev);
		while(now)
		{
			if (predicate(*now, *prev))
				++unordered;
			now = OPE_TYPE::getNext(*now);
		}
		return unordered;
	}
	linkedListSortFuncSet(linkedListCalcUnordered);

	//========================================
	//ソートアルゴリズム分類：挿入ソート
	//========================================

	//----------------------------------------
	//アルゴリズム：変形挿入ソート ※片方向連結リスト対応版
	//----------------------------------------
	//・最良計算時間：O(n)
	//・平均計算時間：O(n^2)
	//・最悪計算時間：O(n^2)
	//・メモリ使用量：O(1)
	//・安定性：　　　○
	//----------------------------------------
	//挿入先の探索を先頭から行う方式
	//----------------------------------------
	template<class OPE_TYPE, class PREDICATE>
	std::size_t singlyLinkedListInsertionSort(typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last, PREDICATE predicate)
	{
		typedef typename OPE_TYPE::node_type node_type;
		if (!first || !OPE_TYPE::getNext(*first))
			return 0;
		std::size_t swapped_count = 0;
		node_type* now = first;
		node_type* next = const_cast<node_type*>(OPE_TYPE::getNext(*now));
		while (next)
		{
			if (predicate(*next, *now))
			{
				node_type* min_before = nullptr;
				node_type* min = first;
				while (min != now)
				{
					if (predicate(*next, *min))
						break;
					min_before = min;
					min = const_cast<node_type*>(OPE_TYPE::getNext(*min));
				}
				removeNode<OPE_TYPE>(*next, first, last, now);
				insertNodeBefore<OPE_TYPE>(*next, *min, first, last, min_before);
				++swapped_count;
				next = const_cast<node_type*>(OPE_TYPE::getNext(*now));
			}
			else
			{
				now = next;
				next = const_cast<node_type*>(OPE_TYPE::getNext(*next));
			}
		}
		return swapped_count;
	}
	linkedListSortFuncSet(singlyLinkedListInsertionSort);

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
	#ifdef ENABLE_REVERSE_ITERATOR
		class reverse_iterator;
		typedef const reverse_iterator const_reverse_iterator;
	#endif//ENABLE_REVERSE_ITERATOR
		//--------------------
		//イテレータ
		class iterator : public std::iterator<std::forward_iterator_tag, node_type>
		{
			friend class container;
		#ifdef ENABLE_REVERSE_ITERATOR
			friend class reverse_iterator;
		#endif//ENABLE_REVERSE_ITERATOR
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const node_type() const { return *getNode(); }
			inline operator node_type&(){ return *getNode(); }
		public:
			//オペレータ
			inline const_reference operator*() const { return *getNode(); }
			inline reference operator*(){ return *getNode(); }
			inline const_pointer operator->() const { return getNode(); }
			inline pointer operator->(){ return getNode(); }
		#if 1//std::forard_iterator_tag には本来必要ではない
			inline const_iterator operator[](const int index) const
			{
				iterator ite(*m_con, false);
				ite += index;
				return std::move(ite);
			}
			inline iterator operator[](const int index)
			{
				iterator ite(*m_con, false);
				ite += index;
				return std::move(ite);
			}
		#endif
			//比較オペレータ
			inline bool operator==(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_isEnd && rhs.m_isEnd ? true :
				       m_isEnd || rhs.m_isEnd ? false :
				       m_node == rhs.m_node;
			}
			inline bool operator!=(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_isEnd && rhs.m_isEnd ? false :
				       m_isEnd || rhs.m_isEnd ? true :
					   m_node != rhs.m_node;
			}
			//演算オペレータ
			inline const_iterator& operator++() const
			{
				updateNext();
				return *this;
			}
		#ifdef ENABLE_REVERSE_ITERATOR
			inline const_iterator& operator--() const
			{
				updatePrev();
				return *this;
			}
		#endif//ENABLE_REVERSE_ITERATOR
			inline iterator& operator++()
			{
				updateNext();
				return *this;
			}
		#ifdef ENABLE_REVERSE_ITERATOR
			inline iterator& operator--()
			{
				updatePrev();
				return *this;
			}
		#endif//ENABLE_REVERSE_ITERATOR
			inline const_iterator operator++(int) const
			{
				iterator ite(*this);
				++(*this);
				return std::move(ite);
			}
		#ifdef ENABLE_REVERSE_ITERATOR
			inline const_iterator operator--(int) const
			{
				iterator ite(*this);
				--(*this);
				return  std::move(ite);
			}
		#endif//ENABLE_REVERSE_ITERATOR
			inline iterator operator++(int)
			{
				iterator ite(*this);
				++(*this);
				return  std::move(ite);
			}
		#ifdef ENABLE_REVERSE_ITERATOR
			inline iterator operator--(int)
			{
				iterator ite(*this);
				--(*this);
				return  std::move(ite);
			}
		#endif//ENABLE_REVERSE_ITERATOR
		#if 1//std::forward_iterator_tag には本来必要ではない
			inline const_iterator& operator+=(const typename iterator::difference_type rhs) const
			{
				updateForward(rhs);
				return *this;
			}
			inline const_iterator& operator+=(const std::size_t rhs) const
			{
				return operator+=(static_cast<typename iterator::difference_type>(rhs));
			}
		#ifdef ENABLE_REVERSE_ITERATOR
			inline const_iterator& operator-=(const typename iterator::difference_type rhs) const
			{
				updateBackward(rhs);
				return *this;
			}
			inline const_iterator& operator-=(const std::size_t rhs) const
			{
				return operator-=(static_cast<typename iterator::difference_type>(rhs));
			}
		#endif//ENABLE_REVERSE_ITERATOR
			inline iterator& operator+=(const typename iterator::difference_type rhs)
			{
				updateForward(rhs);
				return *this;
			}
			inline iterator& operator+=(const std::size_t rhs)
			{
				return operator+=(static_cast<typename iterator::difference_type>(rhs));
			}
		#ifdef ENABLE_REVERSE_ITERATOR
			inline iterator& operator-=(const typename iterator::difference_type rhs)
			{
				updateBackward(rhs);
				return *this;
			}
			inline iterator& operator-=(const std::size_t rhs)
			{
				return operator-=(static_cast<typename iterator::difference_type>(rhs));
			}
		#endif//ENABLE_REVERSE_ITERATOR
			inline const_iterator operator+(const typename iterator::difference_type rhs) const
			{
				iterator ite(*this);
				ite += rhs;
				return  std::move(ite);
			}
			inline const_iterator operator+(const std::size_t rhs) const
			{
				return std::move(operator+(static_cast<typename iterator::difference_type>(rhs)));
			}
		#ifdef ENABLE_REVERSE_ITERATOR
			inline const_iterator operator-(const typename iterator::difference_type rhs) const
			{
				iterator ite(*this);
				ite -= rhs;
				return  std::move(ite);
			}
			inline const_iterator operator-(const std::size_t rhs) const
			{
				return std::move(operator-(static_cast<typename iterator::difference_type>(rhs)));
			}
		#endif//ENABLE_REVERSE_ITERATOR
			inline iterator operator+(const typename iterator::difference_type rhs)
			{
				iterator ite(*this);
				ite += rhs;
				return  std::move(ite);
			}
			inline iterator operator+(const std::size_t rhs)
			{
				return std::move(operator+(static_cast<typename iterator::difference_type>(rhs)));
			}
		#ifdef ENABLE_REVERSE_ITERATOR
			inline iterator operator-(const typename iterator::difference_type rhs)
			{
				iterator ite(*this);
				ite -= rhs;
				return  std::move(ite);
			}
			inline iterator operator-(const std::size_t rhs)
			{
				return std::move(operator-(static_cast<typename iterator::difference_type>(rhs)));
			}
			//inline typename iterator::difference_type operator-(const iterator rhs)
			//{
			//	return ???;
			//}
		#endif//ENABLE_REVERSE_ITERATOR
		#endif
		public:
			//ムーブオペレータ
			inline iterator& operator=(const_iterator&& rhs)
			{
				m_con = rhs.m_con;
				m_node = rhs.m_node;
				m_isEnd = rhs.m_isEnd;
				return *this;
			}
		#ifdef ENABLE_REVERSE_ITERATOR
			iterator& operator=(const_reverse_iterator&& rhs);
		#endif//ENABLE_REVERSE_ITERATOR
			//コピーオペレータ
			inline iterator& operator=(const_iterator& rhs)
			{
				m_con = rhs.m_con;
				m_node = rhs.m_node;
				m_isEnd = rhs.m_isEnd;
				return *this;
			}
		#ifdef ENABLE_REVERSE_ITERATOR
			iterator& operator=(const_reverse_iterator& rhs);
		#endif//ENABLE_REVERSE_ITERATOR
		public:
			//アクセッサ
			inline bool isExist() const { return m_node != nullptr && m_node != reinterpret_cast<node_type*>(BEFORE_BEGIN); }
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const { return m_node != nullptr || m_isEnd; }
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isBeforeBegin() const { m_node != reinterpret_cast<node_type*>(BEFORE_BEGIN); }
			inline bool isEnd() const { return m_isEnd; }//終端か？
			inline const node_type* getNode() const { return m_node == reinterpret_cast<node_type*>(BEFORE_BEGIN) ? nullptr : m_node; }//現在のノード
			inline node_type* getNode(){ return m_node == reinterpret_cast<node_type*>(BEFORE_BEGIN) ? nullptr : m_node; }//現在のノード
		private:
			//メソッド
			inline void updateNext() const
			{
				if (m_node == reinterpret_cast<node_type*>(BEFORE_BEGIN))
				{
					m_node = m_con->m_first;
					m_isEnd = (m_con->m_first == nullptr);
					return;
				}
				node_type* prev = m_node;
				if (m_node)
					m_node = const_cast<node_type*>(getNextNode<ope_type>(*m_node));
				m_isEnd = (prev && !m_node);
			}
		#ifdef ENABLE_REVERSE_ITERATOR
			inline void updatePrev() const
			{
				if (m_node == reinterpret_cast<node_type*>(BEFORE_BEGIN))
				{
					m_node = nullptr;
					m_isEnd = false;
					return;
				}
				if (m_isEnd)
				{
					m_node = const_cast<node_type*>(m_con->m_last);
					m_isEnd = false;
					return;
				}
				if (m_node)
					m_node = const_cast<node_type*>(getPrevNode<ope_type>(*m_node, m_con->m_first, nullptr));
				m_isEnd = false;
			}
		#endif//ENABLE_REVERSE_ITERATOR
			void updateForward(const std::size_t step) const
			{
				if (step == 0)
					return;
				std::size_t _step = step;
				node_type* prev = m_node;
				if (m_node == reinterpret_cast<node_type*>(BEFORE_BEGIN))
				{
					m_node = m_con->m_first;
					--_step;
				}
				if (m_node)
					m_node = const_cast<node_type*>(getForwardNode<ope_type>(*m_node, _step));
				m_isEnd = (prev && !m_node && _step == 0);
			}
		#ifdef ENABLE_REVERSE_ITERATOR
			void updateBackward(const std::size_t step) const
			{
				if (step == 0)
					return;
				if (m_node == reinterpret_cast<node_type*>(BEFORE_BEGIN))
				{
					m_node = nullptr;
					m_isEnd = false;
					return;
				}
				std::size_t _step = step;
				if (_step > 0 && m_isEnd)
				{
					m_node = const_cast<node_type*>(m_con->m_last);
					--_step;
				}
				if (m_node)
					m_node = const_cast<node_type*>(getBackwardNode<ope_type>(*m_node, _step, m_con->m_first, nullptr));
				m_isEnd = false;
			}
		#endif//ENABLE_REVERSE_ITERATOR
		public:
			inline void updateBeforeBegin()
			{
				m_node = reinterpret_cast<node_type*>(BEFORE_BEGIN);
				m_isEnd = false;
			}
			inline bool isBeforeBegin()
			{
				return m_node == reinterpret_cast<node_type*>(BEFORE_BEGIN);
			}
		public:
			//ムーブコンストラクタ
			inline iterator(const_iterator&& obj) :
				m_con(obj.m_con),
				m_node(obj.m_node),
				m_isEnd(obj.m_isEnd)
			{}
		#ifdef ENABLE_REVERSE_ITERATOR
			iterator(const_reverse_iterator&& obj);
		#endif//ENABLE_REVERSE_ITERATOR
			//コピーコンストラクタ
			inline iterator(const_iterator& obj) :
				m_con(obj.m_con),
				m_node(obj.m_node),
				m_isEnd(obj.m_isEnd)
			{}
		#ifdef ENABLE_REVERSE_ITERATOR
			iterator(const_reverse_iterator& obj);
		#endif//ENABLE_REVERSE_ITERATOR
			//コンストラクタ
			inline iterator(const container& con, const bool is_end) :
				m_con(&con),
				m_node(nullptr),
				m_isEnd(is_end)
			{
				if (!is_end)
				{
					m_node = const_cast<node_type*>(con.m_first);
					if (!m_node)
						m_isEnd = true;
				}
			}
			inline iterator(const container& con, node_type* node, const bool is_end) :
				m_con(&con),
				m_node(node),
				m_isEnd(is_end)
			{}
			inline iterator() :
				m_con(nullptr),
				m_node(nullptr),
				m_isEnd(false)
			{}
			//デストラクタ
			inline ~iterator()
			{}
		protected:
			//フィールド
			const container* m_con;//コンテナ
			mutable node_type* m_node;//現在のノード
			mutable bool m_isEnd;//終端か？
		};
	#ifdef ENABLE_REVERSE_ITERATOR
		//--------------------
		//リバースイテレータ
		//class reverse_iterator : public std::reverse_iterator<iterator>
		class reverse_iterator : public std::iterator<std::forward_iterator_tag, node_type>
		{
			friend class container;
			friend class iterator;
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const node_type() const { return *getNode(); }
			inline operator node_type&(){ return *getNode(); }
		public:
			//オペレータ
			inline const_reference operator*() const { return *getNode(); }
			inline reference operator*(){ return *getNode(); }
			inline const_pointer operator->() const { return getNode(); }
			inline pointer operator->(){ return getNode(); }
		#if 1//std::bidirectional_iterator_tag には本来必要ではない
			inline const_reverse_iterator operator[](const int index) const
			{
				reverse_iterator ite(*m_con, false);
				ite += index;
				return std::move(ite);
			}
			inline reverse_iterator operator[](const int index)
			{
				reverse_iterator ite(*m_con, false);
				ite += index;
				return std::move(ite);
			}
		#endif
		public:
			//比較オペレータ
			inline bool operator==(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_isEnd && m_isEnd ? true :
				       rhs.m_isEnd || m_isEnd ? false :
					   m_node == rhs.m_node;
			}
			inline bool operator!=(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_isEnd && m_isEnd ? false :
				       rhs.m_isEnd || m_isEnd ? true :
					   m_node != rhs.m_node;
			}
			//演算オペレータ
			inline const_reverse_iterator& operator++() const
			{
				updateNext();
				return *this;
			}
			inline const_reverse_iterator& operator--() const
			{
				updatePrev();
				return *this;
			}
			inline reverse_iterator& operator++()
			{
				updateNext();
				return *this;
			}
			inline reverse_iterator& operator--()
			{
				updatePrev();
				return *this;
			}
			inline const_reverse_iterator operator++(int) const
			{
				reverse_iterator ite(*this);
				++(*this);
				return  std::move(ite);
			}
			inline const_reverse_iterator operator--(int) const
			{
				reverse_iterator ite(*this);
				--(*this);
				return  std::move(ite);
			}
			inline reverse_iterator operator++(int)
			{
				reverse_iterator ite(*this);
				++(*this);
				return  std::move(ite);
			}
			inline reverse_iterator operator--(int)
			{
				reverse_iterator ite(*this);
				--(*this);
				return  std::move(ite);
			}
		#if 1//std::bidirectional_iterator_tag には本来必要ではない
			inline const_reverse_iterator& operator+=(const typename reverse_iterator::difference_type rhs) const
			{
				updateForward(rhs);
				return *this;
			}
			inline const_reverse_iterator& operator+=(const std::size_t rhs) const
			{
				return operator+=(static_cast<typename reverse_iterator::difference_type>(rhs));
			}
			inline const_reverse_iterator& operator-=(const typename reverse_iterator::difference_type rhs) const
			{
				updateBackward(rhs);
				return *this;
			}
			inline const_reverse_iterator& operator-=(const std::size_t rhs) const
			{
				return operator-=(static_cast<typename reverse_iterator::difference_type>(rhs));
			}
			inline reverse_iterator& operator+=(const typename reverse_iterator::difference_type rhs)
			{
				updateForward(rhs);
				return *this;
			}
			inline reverse_iterator& operator+=(const std::size_t rhs)
			{
				return operator+=(static_cast<typename reverse_iterator::difference_type>(rhs));
			}
			inline reverse_iterator& operator-=(const typename reverse_iterator::difference_type rhs)
			{
				updateBackward(rhs);
				return *this;
			}
			inline reverse_iterator& operator-=(const std::size_t rhs)
			{
				return operator-=(static_cast<typename reverse_iterator::difference_type>(rhs));
			}
			inline const_reverse_iterator operator+(const typename reverse_iterator::difference_type rhs) const
			{
				reverse_iterator ite(*this);
				ite += rhs;
				return  std::move(ite);
			}
			inline const_reverse_iterator operator+(const std::size_t rhs) const
			{
				return std::move(operator+(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			inline const_reverse_iterator operator-(const typename reverse_iterator::difference_type rhs) const
			{
				reverse_iterator ite(*this);
				ite -= rhs;
				return  std::move(ite);
			}
			inline const_reverse_iterator operator-(const std::size_t rhs) const
			{
				return std::move(operator-(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			inline reverse_iterator operator+(const typename reverse_iterator::difference_type rhs)
			{
				reverse_iterator ite(*this);
				ite += rhs;
				return  std::move(ite);
			}
			inline reverse_iterator operator+(const std::size_t rhs)
			{
				return std::move(operator+(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			inline reverse_iterator operator-(const typename reverse_iterator::difference_type rhs)
			{
				reverse_iterator ite(*this);
				ite -= rhs;
				return  std::move(ite);
			}
			inline reverse_iterator operator-(const std::size_t rhs)
			{
				return std::move(operator-(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			//inline typename reverse_iterator::difference_type operator-(const reverse_iterator rhs)
			//{
			//	return ???;
			//}
		#endif
		public:
			//ムーブオペレータ
			inline reverse_iterator& operator=(const_reverse_iterator&& rhs)
			{
				m_con = rhs.m_con;
				m_node = rhs.m_node;
				m_isEnd = rhs.m_isEnd;
				return *this;
			}
			inline reverse_iterator& operator=(const_iterator&& rhs)
			{
				m_con = rhs.m_con;
				m_node = rhs.m_node;
				m_isEnd = false;
				if (m_node)
				{
					if (m_node == reinterpret_cast<node_type*>(BEFORE_BEGIN))
					{
						m_node = m_con->m_first;
						if (!m_node)
							m_isEnd = true;
					}
					else
						++(*this);
				}
				else
				{
					if (rhs.m_isEnd)
						m_node = const_cast<node_type*>(m_con->m_last);
				}
				return *this;
			}
			//コピーオペレータ
			inline reverse_iterator& operator=(const_reverse_iterator& rhs)
			{
				m_con = rhs.m_con;
				m_node = rhs.m_node;
				m_isEnd = rhs.m_isEnd;
				return *this;
			}
			inline reverse_iterator& operator=(const_iterator& rhs)
			{
				m_con = rhs.m_con;
				m_node = rhs.m_node;
				m_isEnd = false;
				if (m_node)
				{
					if (m_node == reinterpret_cast<node_type*>(BEFORE_BEGIN))
					{
						m_node = m_con->m_first;
						if (!m_node)
							m_isEnd = true;
					}
					else
						++(*this);
				}
				else
				{
					if (rhs.m_isEnd)
						m_node = const_cast<node_type*>(m_con->m_last);
				}
				return *this;
			}
		public:
			//アクセッサ
			inline bool isExist() const { return m_node != nullptr; }
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const { return m_node != nullptr || m_isEnd; }
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const { return m_isEnd; }//終端か？
			inline const node_type* getNode() const { return m_node; }//現在のノード
			inline node_type* getNode(){ return m_node; }//現在のノード
		public:
			//メソッド
			inline void updateNext() const
			{
				node_type* prev = m_node;
				if (m_node)
					m_node = const_cast<node_type*>(getPrevNode<ope_type>(*m_node, m_con->m_first, nullptr));
				m_isEnd = (prev && !m_node);
			}
			inline void updatePrev() const
			{
				if (m_isEnd)
				{
					m_node = const_cast<node_type*>(m_con->m_first);
					m_isEnd = false;
					return;
				}
				if (m_node)
					m_node = const_cast<node_type*>(getNextNode<ope_type>(*m_node));
				m_isEnd = false;
			}
			void updateForward(const std::size_t step) const
			{
				std::size_t _step = step;
				node_type* prev = m_node;
				if (m_node)
					m_node = const_cast<node_type*>(getBackwardNode<ope_type>(*m_node, _step, m_con->m_first, nullptr));
				m_isEnd = (prev && !m_node && _step == 0);
			}
			void updateBackward(const std::size_t step) const
			{
				std::size_t _step = step;
				if (_step > 0 && m_isEnd)
				{
					m_node = const_cast<node_type*>(m_con->m_first);
					--_step;
				}
				if (m_node)
					m_node = const_cast<node_type*>(getForwardNode<ope_type>(*m_node, _step));
				m_isEnd = false;
			}
		public:
			//ベースを取得
			inline const_iterator base() const
			{
				iterator ite(*this);
				return std::move(ite);
			}
			inline iterator base()
			{
				iterator ite(*this);
				return std::move(ite);
			}
		public:
			//ムーブコンストラクタ
			inline reverse_iterator(const_reverse_iterator&& obj) :
				m_con(obj.m_con),
				m_node(obj.m_node),
				m_isEnd(obj.m_isEnd)
			{}
			inline reverse_iterator(const_iterator&& obj) :
				m_con(obj.m_con),
				m_node(obj.m_node),
				m_isEnd(false)
			{
				if (m_node)
				{
					if (m_node == reinterpret_cast<node_type*>(BEFORE_BEGIN))
					{
						m_node = m_con->m_first;
						if (!m_node)
							m_isEnd = true;
					}
					else
						++(*this);
				}
				else
				{
					if (obj.m_isEnd)
						m_node = const_cast<node_type*>(m_con->m_last);
				}
			}
			//コピーコンストラクタ
			inline reverse_iterator(const_reverse_iterator& obj) :
				m_con(obj.m_con),
				m_node(obj.m_node),
				m_isEnd(obj.m_isEnd)
			{}
			inline reverse_iterator(const_iterator& obj) :
				m_con(obj.m_con),
				m_node(obj.m_node),
				m_isEnd(obj.m_isEnd)
			{
				if (m_node)
				{
					if (m_node == reinterpret_cast<node_type*>(BEFORE_BEGIN))
					{
						m_node = m_con->m_first;
						if (!m_node)
							m_isEnd = true;
					}
					else
						++(*this);
				}
				else
				{
					if (obj.m_isEnd)
						m_node = const_cast<node_type*>(m_con->m_last);
				}
			}
			//コンストラクタ
			inline reverse_iterator(const container& con, const bool is_end) :
				m_con(&con),
				m_node(nullptr),
				m_isEnd(is_end)
			{
				if (!is_end)
				{
					m_node = const_cast<node_type*>(m_con->m_last);
					if (!m_node)
						m_isEnd = true;
				}
			}
			inline reverse_iterator(const container& con, node_type* node, const bool is_end) :
				m_con(con),
				m_node(node),
				m_isEnd(is_end)
			{}
			inline reverse_iterator() :
				m_con(nullptr),
				m_node(nullptr),
				m_isEnd(false)
			{}
			//デストラクタ
			inline ~reverse_iterator()
			{}
		protected:
			//フィールド
			const container* m_con;//コンテナ
			mutable node_type* m_node;//現在のノード
			mutable bool m_isEnd;//終端か？
		};
	#endif//ENABLE_REVERSE_ITERATOR
	public:
		//アクセッサ
		//※std::forward_listと異なり、ノードのポインタを返す
		const node_type* at(const index_type index) const
		{
			return getForwardNode<ope_type>(m_first, index);
		}
		inline node_type* at(const index_type index){ return const_cast<node_type*>(const_cast<const container*>(this)->at(index)); }
		inline const node_type* operator[](const index_type index) const { return at(index); }
		inline node_type* operator[](const index_type index){ return at(index); }
	public:
		//キャストオペレータ
		inline operator lock_type&(){ return m_lock; }//ロックオブジェクト
		inline operator lock_type&() const { return m_lock; }//ロックオブジェクト ※mutable
	public:
		//メソッド
		//inline std::size_t max_size() const { return (不定); }
		//inline std::size_t capacity() const { return (不定); }
		inline std::size_t size() const { return m_first ? countNodes<ope_type>(*m_first) : 0; }//ノード数を取得
		inline bool empty() const { return m_first == nullptr; }//空か？
		inline node_type* front(){ return m_first; }//先頭ノードを参照
		inline const node_type* front() const { return m_first; }//先頭ノードを参照
		inline node_type*& first_ref(){ return m_first; }//先頭ノードの参照を取得
		inline node_type* back(){ return m_last; }//末尾ノードを参照
		inline const node_type* back() const { return m_last; }//末尾ノードを参照
		inline node_type*& last_ref(){ return m_last; }//末尾ノードの参照を取得
		//イテレータを取得
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロック全体の前後で共有ロック（リードロック）の取得と解放を行う必要がある
		inline const_iterator cbefore_begin() const
		{
			iterator ite(*this, false);
			ite.updateBeforeBegin();
			return std::move(ite);
		}
		inline const_iterator cbegin() const
		{
			iterator ite(*this, false);
			return std::move(ite);
		}
		inline const_iterator cend() const
		{
			iterator ite(*this, true);
			return std::move(ite);
		}
		inline const_iterator before_begin() const
		{
			iterator ite(*this, false);
			ite.updateBeforeBegin();
			return std::move(ite);
		}
		inline const_iterator begin() const
		{
			iterator ite(*this, false);
			return std::move(ite);
		}
		inline const_iterator end() const
		{
			iterator ite(*this, true);
			return std::move(ite);
		}
		inline iterator before_begin()
		{
			iterator ite(*this, false);
			ite.updateBeforeBegin();
			return std::move(ite);
		}
		inline iterator begin()
		{
			iterator ite(*this, false);
			return std::move(ite);
		}
		inline iterator end()
		{
			iterator ite(*this, true);
			return std::move(ite);
		}
	#ifdef ENABLE_REVERSE_ITERATOR
		//リバースイテレータを取得
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロック全体の前後で共有ロック（リードロック）の取得と解放を行う必要がある
		//【注意】低速処理（イテレータを移動する度に先頭から辿り直すのでかなり低速）
		//※std::forward_listにないメソッド
		inline const_reverse_iterator crbegin() const
		{
			reverse_iterator ite(*this, false);
			return std::move(ite);
		}
		inline const_reverse_iterator crend() const
		{
			reverse_iterator ite(*this, true);
			return std::move(ite);
		}
		inline const_reverse_iterator rbegin() const
		{
			reverse_iterator ite(*this, false);
			return std::move(ite);
		}
		inline const_reverse_iterator rend() const
		{
			reverse_iterator ite(*this, true);
			return std::move(ite);
		}
		inline reverse_iterator rbegin()
		{
			reverse_iterator ite(*this, false);
			return std::move(ite);
		}
		inline reverse_iterator rend()
		{
			reverse_iterator ite(*this, true);
			return std::move(ite);
		}
	#endif//ENABLE_REVERSE_ITERATOR

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
			singlyLinkedListInsertionSort<OPE_TYPE>(m_first, m_last, typename ope_type::predicateForSort());
		}
		//※プレディケート関数指定版
		template<class PREDICATE>
		void sort(PREDICATE predicate)
		{
			singlyLinkedListInsertionSort<OPE_TYPE>(m_first, m_last, predicate);
		}
	#ifdef ENABLE_STABLE_SORT
		//安定ソート
		//※挿入ソートを使用
		//※ope_type::predicateForSort() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		void stableSort()
		{
			singlyLinkedListInsertionSort<OPE_TYPE>(m_first, m_last, typename ope_type::predicateForSort());
		}
		//※プレディケート関数指定版
		template<class PREDICATE>
		void stableSort(PREDICATE predicate)
		{
			singlyLinkedListInsertionSort<OPE_TYPE>(m_first, m_last, predicate);
		}
	#endif//ENABLE_STABLE_SORT
		//ソート済み状態チェック
		//※ope_type::predicateForSort() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		bool isOrdered() const
		{
			return linkedListCalcUnordered<OPE_TYPE>(m_first, typename ope_type::predicateForSort()) == 0;
		}
		//※プレディケート関数指定版
		template<class PREDICATE>
		bool isOrdered(PREDICATE predicate) const
		{
			return linkedListCalcUnordered<OPE_TYPE>(m_first, predicate) == 0;
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
			iterator found = iteratorLinearSearchValue(begin(), end(), value, typename ope_type::predicateForFind());
			return std::move(found);
		}
		//※比較関数＋値指定版
		template<typename V, class PREDICATE>
		iterator findValue(const V& value, PREDICATE predicate)
		{
			iterator found = iteratorLinearSearchValue(begin(), end(), value, predicate);
			return std::move(found);
		}
		//※比較関数指定版
		//※値の指定は関数に含んでおく（クロ―ジャを用いるなどする）
		template<class PREDICATE>
		iterator find(PREDICATE predicate)
		{
			iterator found = iteratorLinearSearch(begin(), end(), predicate);
			return std::move(found);
		}
	#ifdef ENABLE_BINARY_SEARCH
		//二分探索
		//※探索値指定版
		//※ope_type::comparisonForSearch() を使用して探索（標準では、データ型の operator==() と operator<() に従って探索）
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		//【注意】低速処理
		template<typename V>
		iterator binarySearchValue(const V& value)
		{
			iterator found = iteratorBinarySearchValue(begin(), end(), value, typename ope_type::comparisonForSearch());
			return std::move(found);
		}
		//※比較関数＋値指定版
		template<typename V, class COMPARISON>
		iterator binarySearchValue(const V& value, COMPARISON comparison)
		{
			iterator found = iteratorBinarySearchValue(begin(), end(), value, comparison);
			return std::move(found);
		}
		//※比較関数指定版
		//※値の指定は関数に含んでおく（クロ―ジャを用いるなどする）
		template<class COMPARISON>
		iterator binary_search(COMPARISON comparison)
		{
			iterator found = iteratorBinarySearch(begin(), end(), comparison);
			return std::move(found);
		}
	#endif//ENABLE_BINARY_SEARCH

		//リスト操作系メソッド
		//※merge(), splice_after(), reverse(), unique()には非対応

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
#ifdef ENABLE_REVERSE_ITERATOR
	//イテレータのムーブオペレータ
	template<class OPE_TYPE>
	//typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(typename container<OPE_TYPE>::const_reverse_iterator&& rhs)//GCCはOK, VC++はNG
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator&& rhs)//VC++もOK
	{
		m_con = rhs.m_con;
		m_node = rhs.m_node;
		m_isEnd = false;
		if (m_node)
		{
			++(*this);
		}
		else
		{
			if (rhs.m_isEnd)
				m_node = const_cast<node_type*>(m_con->m_first);
		}
		return *this;
	}
	//イテレータのコピーオペレータ
	template<class OPE_TYPE>
	//typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(typename container<OPE_TYPE>::const_reverse_iterator& rhs)//GCCはOK, VC++はNG
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)//VC++もOK
	{
		m_con = rhs.m_con;
		m_node = rhs.m_node;
		m_isEnd = false;
		if (m_node)
		{
			++(*this);
		}
		else
		{
			if (rhs.m_isEnd)
				m_node = const_cast<node_type*>(m_con->m_first);
		}
		return *this;
	}
	//イテレータのムーブコンストラクタ
	template<class OPE_TYPE>
	//container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::const_reverse_iterator&& obj) ://GCCはOK, VC++はNG
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator&& obj) ://VC++もOK
		m_con(obj.m_con),
		m_node(obj.m_node),
		m_isEnd(false)
	{
		if (m_node)
		{
			++(*this);
		}
		else
		{
			if (obj.m_isEnd)
				m_node = const_cast<node_type*>(m_con->m_first);
		}
	}
	//イテレータのコピーコンストラクタ
	template<class OPE_TYPE>
	//container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::const_reverse_iterator& obj) ://GCCはOK, VC++はNG
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) ://VC++もOK
		m_con(obj.m_con),
		m_node(obj.m_node),
		m_isEnd(false)
	{
		if (m_node)
		{
			++(*this);
		}
		else
		{
			if (obj.m_isEnd)
				m_node = const_cast<node_type*>(m_con->m_first);
		}
	}
#endif//ENABLE_REVERSE_ITERATOR
	//--------------------
	//基本型定義マクロ消去
	#undef DECLARE_OPE_TYPES
#endif
}//namespace singly_linked_list

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__SINGLY_LINKED_LIST_H_

// End of file

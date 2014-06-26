﻿#pragma once
#ifndef __SINGLY_LINKED_LIST_INL_
#define __SINGLY_LINKED_LIST_INL_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// singly_linked_list.inl
// 片方向連結リストコンテナ【インライン関数／テンプレート関数定義部】
//
// ※コンテナクラスの操作が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/singly_linked_list.h>//片方向連結リストコンテナ【宣言部】

#include <gasha/intro_sort.h>//イントロソート
#include <gasha/insertion_sort.h>//挿入ソート

#include <gasha/linear_search.h>//線形探索
#include <gasha/binary_search.h>//二分探索

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace singly_linked_list
{
	//--------------------
	//片方向連結リスト操作関数：指定ノードの次ノードを取得
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getNextNode(const typename OPE_TYPE::node_type& target)
	{
		const typename OPE_TYPE::node_type* node = &target;
		node = OPE_TYPE::getNext(*node);
		return node;
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードの後方のノードを取得
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getForwardNode(const typename OPE_TYPE::node_type& target, std::size_t& step)
	{
		const typename OPE_TYPE::node_type* node = &target;
		while (step > 0 && node)
		{
			node = OPE_TYPE::getNext(*node);
			--step;
		}
		return node;
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードの末尾ノードを取得
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getLastNode(const typename OPE_TYPE::node_type& target)
	{
		const typename OPE_TYPE::node_type* node = &target;
		while (true)
		{
			const typename OPE_TYPE::node_type* next = OPE_TYPE::getNext(*node);
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
		std::size_t num = 1;
		const typename OPE_TYPE::node_type* node = &target;
		while (true)
		{
			const typename OPE_TYPE::node_type* next = OPE_TYPE::getNext(*node);
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
	const typename OPE_TYPE::node_type* getPrevNode(const typename OPE_TYPE::node_type& target, const typename OPE_TYPE::node_type* first, const typename OPE_TYPE::node_type* backward)
	{
		const typename OPE_TYPE::node_type* prev = nullptr;
		const typename OPE_TYPE::node_type* node = backward;
		if (!node)
			node = first;
		while (node != &target)
		{
			prev = node;
			node = OPE_TYPE::getNext(*node);
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
	const typename OPE_TYPE::node_type* getBackwardNode(const typename OPE_TYPE::node_type& target, std::size_t& step, const typename OPE_TYPE::node_type* first, const typename OPE_TYPE::node_type* backward)
	{
		const typename OPE_TYPE::node_type* node = &target;
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
	typename OPE_TYPE::node_type* insertNodeAfter(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type* target, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		if (target == nullptr)
		{
			OPE_TYPE::setNext(node, first);
			first = &node;
			return &node;
		}
		typename OPE_TYPE::node_type* next = const_cast<typename OPE_TYPE::node_type*>(OPE_TYPE::getNext(*target));
		if (!next)
			last = &node;
		OPE_TYPE::setNext(*target, &node);
		OPE_TYPE::setNext(node, next);
		return &node;
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードの前に連結
	//【注意】低速処理（backwardを指定することで高速化可能）
	//※もし、targetの直前もしくはそれより前のノードが分かっているなら、
	//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
	//　nullptrを指定した場合、firstからtargetまで辿る。
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeBefore(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type& target, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last, const typename OPE_TYPE::node_type* backward)
	{
		typename OPE_TYPE::node_type* prev = const_cast<typename OPE_TYPE::node_type*>(getPrevNode<OPE_TYPE>(target, first, backward));
		if (!prev)
			first = &node;
		else
			OPE_TYPE::setNext(*prev, &node);
		OPE_TYPE::setNext(node, &target);
		return &node;
	}
	//--------------------
	//片方向連結リスト操作関数：先頭に連結
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeBeginning(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		if (!first)
		{
			first = &node;
			last = &node;
			OPE_TYPE::setNext(node, nullptr);
			return &node;
		}
		OPE_TYPE::setNext(node, first);
		first = &node;
		return &node;
	}
	//--------------------
	//片方向連結リスト操作関数：末尾に連結
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeEnd(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		if (!last)
		{
			first = &node;
			last = &node;
			OPE_TYPE::setNext(node, nullptr);
			return &node;
		}
		OPE_TYPE::setNext(*last, &node);
		OPE_TYPE::setNext(node, nullptr);
		last = &node;
		return &node;
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードの次のノードを連結から外す
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* removeNodeAfter(typename OPE_TYPE::node_type* prev_target, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		if (!prev_target && !first)
			return nullptr;
		typename OPE_TYPE::node_type* prev = prev_target;
		typename OPE_TYPE::node_type& node = *prev ? *OPE_TYPE::getNext(*prev) : *first;
		typename OPE_TYPE::node_type* next = OPE_TYPE::getNext(*node);
		if (prev)
			OPE_TYPE::setNext(*prev, next);
		else
			first = next;
		if (!next)
			last = prev;
		OPE_TYPE::setNext(node, nullptr);
		return &node;
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードを連結から外す
	//【注意】低速処理（backwardを指定することで高速化可能）
	//※もし、nodeの直前もしくはそれより前のノードが分かっているなら、
	//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
	//　nullptrを指定した場合、firstからnodeまで辿る。
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* removeNode(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last, typename OPE_TYPE::node_type* backward)
	{
		typename OPE_TYPE::node_type* prev = const_cast<typename OPE_TYPE::node_type*>(getPrevNode<OPE_TYPE>(node, first, backward));
		typename OPE_TYPE::node_type* next = const_cast<typename OPE_TYPE::node_type*>(OPE_TYPE::getNext(node));
		if (prev)
			OPE_TYPE::setNext(*prev, next);
		else
			first = next;
		if (!next)
			last = prev;
		OPE_TYPE::setNext(node, nullptr);
		return &node;
	}
	//--------------------
	//片方向連結リスト操作関数：指定ノードの範囲を連結から外す
	//【注意】低速処理（backwardを指定することで高速化可能）
	//※もし、startの直前もしくはそれより前のノードが分かっているなら、
	//　それを backward に指定するこで検索範囲が短縮され、処理効率が向上する。
	//　nullptrを指定した場合、firstからstartまで辿る。
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* removeNodes(typename OPE_TYPE::node_type& start, typename OPE_TYPE::node_type& end, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last, typename OPE_TYPE::node_type* backward)
	{
		typename OPE_TYPE::node_type* prev = const_cast<typename OPE_TYPE::node_type*>(getPrevNode<OPE_TYPE>(start, first, backward));
		typename OPE_TYPE::node_type* next = &end;
		typename OPE_TYPE::node_type* _end = next ? const_cast<typename OPE_TYPE::node_type*>(getPrevNode<OPE_TYPE>(end, first, &start)) : last;
		if (prev)
			OPE_TYPE::setNext(*prev, next);
		else
			first = next;
		if (!next)
			last = prev;
		OPE_TYPE::setNext(*_end, nullptr);
		return &start;
	}

	//----------------------------------------
	//片方向連結リスト操作関数：線形探索
	template<class OPE_TYPE, typename V>
	const typename OPE_TYPE::node_type* linearSearchValue(const typename OPE_TYPE::node_type* first, const V& value)
	{
		return GASHA_ singlyLinkedListLinearSearchValue(first, OPE_TYPE::getNext, value);
	}
	template<class OPE_TYPE, typename V, class PREDICATE>
	const typename OPE_TYPE::node_type* linearSearchValue(const typename OPE_TYPE::node_type* first, const V& value, PREDICATE predicate)
	{
		return GASHA_ singlyLinkedListLinearSearchValue(first, OPE_TYPE::getNext, value, predicate);
	}
	template<class OPE_TYPE, class PREDICATE>
	const typename OPE_TYPE::node_type* linearSearch(const typename OPE_TYPE::node_type* first, PREDICATE predicate)
	{
		return GASHA_ singlyLinkedListLinearSearch(first, OPE_TYPE::getNext, predicate);
	}

	//----------------------------------------
	//片方向連結リスト操作関数：二分探索
	template<class OPE_TYPE, typename V>
	const typename OPE_TYPE::node_type* binarySearchValue(const typename OPE_TYPE::node_type* first, const V& value)
	{
		return GASHA_ singlyLinkedListBinarySearchValue(first, OPE_TYPE::getNext, value);
	}
	template<class OPE_TYPE, typename V, class COMPARISON>
	const typename OPE_TYPE::node_type* binarySearchValue(const typename OPE_TYPE::node_type* first, const V& value, COMPARISON comparison)
	{
		return GASHA_ singlyLinkedListBinarySearchValue(first, OPE_TYPE::getNext, value, comparison);
	}
	template<class OPE_TYPE, class COMPARISON>
	const typename OPE_TYPE::node_type* binarySearch(const typename OPE_TYPE::node_type* first, COMPARISON comparison)
	{
		return GASHA_ singlyLinkedListBinarySearch(first, OPE_TYPE::getNext, comparison);
	}

	//----------------------------------------
	//片方向連結リスト操作関数：非整列状態確認
	template<class OPE_TYPE, class PREDICATE>
	std::size_t isUnordered(const typename OPE_TYPE::node_type* first)
	{
		return GASHA_ singlyLinkedListIsUnordered(first, OPE_TYPE::getNext);
	}
	template<class OPE_TYPE, class PREDICATE>
	std::size_t isUnordered(const typename OPE_TYPE::node_type* first, PREDICATE predicate)
	{
		return GASHA_ singlyLinkedListIsUnordered(first, OPE_TYPE::getNext, predicate);
	}

	//----------------------------------------
	//片方向連結リスト操作関数：整列状態確認
	template<class OPE_TYPE, class PREDICATE>
	std::size_t isOrdered(const typename OPE_TYPE::node_type* first)
	{
		return GASHA_ singlyLinkedListIsOrdered(first, OPE_TYPE::getNext);
	}
	template<class OPE_TYPE, class PREDICATE>
	std::size_t isOrdered(const typename OPE_TYPE::node_type* first, PREDICATE predicate)
	{
		return GASHA_ singlyLinkedListIsOrdered(first, OPE_TYPE::getNext, predicate);
	}

	//----------------------------------------
	//片方向連結リスト操作関数：非整列要素数計上
	template<class OPE_TYPE, class PREDICATE>
	std::size_t sumupUnordered(const typename OPE_TYPE::node_type* first)
	{
		return GASHA_ singlyLinkedListSumupUnordered(first, OPE_TYPE::getNext);
	}
	template<class OPE_TYPE, class PREDICATE>
	std::size_t sumupUnordered(const typename OPE_TYPE::node_type* first, PREDICATE predicate)
	{
		return GASHA_ singlyLinkedListSumupUnordered(first, OPE_TYPE::getNext, predicate);
	}

	//----------------------------------------
	//片方向連結リスト操作関数：挿入ソート
	template<class OPE_TYPE, class PREDICATE>
	std::size_t insertionSort(typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		return GASHA_ singlyLinkedListInsertionSort(first, last, OPE_TYPE::getNext, insertNodeAfter<OPE_TYPE>, removeNodeAfter<OPE_TYPE>);
	}
	template<class OPE_TYPE, class PREDICATE>
	std::size_t insertionSort(typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last, PREDICATE predicate)
	{
		return GASHA_ singlyLinkedListInsertionSort(first, last, OPE_TYPE::getNext, insertNodeAfter<OPE_TYPE>, removeNodeAfter<OPE_TYPE>, predicate);
	}

	//--------------------
	//イテレータのインライン関数
	
	//ムーブオペレータ
	//基本オペレータ
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE//std::forard_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator[](const int index) const
	{
		iterator ite(*m_con, false);
		ite += index;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator[](const int index)
	{
		iterator ite(*m_con, false);
		ite += index;
		return ite;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
	//比較オペレータ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator==(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			m_value == rhs.m_value;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator!=(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? false :
			m_isEnd || rhs.m_isEnd ? true :
			m_value != rhs.m_value;
	}
	//演算オペレータ
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator++() const
	{
		updateNext();
		return *this;
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator--() const
	{
		updatePrev();
		return *this;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator++()
	{
		updateNext();
		return *this;
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator--()
	{
		updatePrev();
		return *this;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator++(int) const
	{
		iterator ite(*this);
		++(*this);
		return ite;
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator--(int) const
	{
		iterator ite(*this);
		--(*this);
		return ite;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator++(int)
	{
		iterator ite(*this);
		++(*this);
		return ite;
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator--(int)
	{
		iterator ite(*this);
		--(*this);
		return ite;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator+=(const int rhs) const
	{
		updateForward(rhs);
		return *this;
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator-=(const int rhs) const
	{
		updateBackward(rhs);
		return *this;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator+=(const int rhs)
	{
		updateForward(rhs);
		return *this;
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator-=(const int rhs)
	{
		updateBackward(rhs);
		return *this;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator+(const int rhs) const
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator-(const int rhs) const
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator+(const int rhs)
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator-(const int rhs)
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
	//template<class OPE_TYPE>
	//inline int container<OPE_TYPE>::iterator::operator-(const typename container<OPE_TYPE>::iterator& rhs) const
	//{
	//	return ...;
	//}
	//ムーブオペレータ
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = false;
		if (m_value)
		{
			++(*this);
		}
		else
		{
			if (rhs.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_first);
		}
		return *this;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	//コピーオペレータ
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const iterator& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = false;
		if (m_value)
		{
			++(*this);
		}
		else
		{
			if (rhs.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_first);
		}
		return *this;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	//アクセッサ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isExist() const
	{
		return m_value != nullptr && m_value != reinterpret_cast<value_type*>(BEFORE_BEGIN);
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isEnabled() const
	{
		return m_value != nullptr || m_isEnd;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isBeforeBegin() const
	{
		return m_value == reinterpret_cast<value_type*>(BEFORE_BEGIN);
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isEnd() const//終端か？
	{
		return m_isEnd;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::iterator::getValue() const//現在の値（ノード）
	{
		return m_value == reinterpret_cast<value_type*>(BEFORE_BEGIN) ? nullptr : m_value;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::iterator::getValue()//現在の値（ノード）
	{
		return m_value == reinterpret_cast<value_type*>(BEFORE_BEGIN) ? nullptr : m_value;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(false)
	{
		if (m_value)
		{
			++(*this);
		}
		else
		{
			if (obj.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_first);
		}
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	//コピーコンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(false)
	{
		if (m_value)
		{
			++(*this);
		}
		else
		{
			if (obj.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_first);
		}
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	//コンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::iterator::iterator(const container& con, const bool is_end) :
		m_con(&con),
		m_value(nullptr),
		m_isEnd(is_end)
	{
		if (!is_end)
		{
			m_value = const_cast<value_type*>(con.m_first);
			if (!m_value)
				m_isEnd = true;
		}
	}
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::iterator::iterator(const container& con, typename container<OPE_TYPE>::value_type* value, const bool is_end) :
		m_con(&con),
		m_value(value),
		m_isEnd(is_end)
	{}

	//--------------------
	//リバースイテレータのインライン関数

	//基本オペレータ
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator[](const int index) const
	{
		reverse_iterator ite(*m_con, false);
		ite += index;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator[](const int index)
	{
		reverse_iterator ite(*m_con, false);
		ite += index;
		return ite;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
	//比較オペレータ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator==(const reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			m_value == rhs.m_value;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator!=(const reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? false :
			rhs.m_isEnd || m_isEnd ? true :
			m_value != rhs.m_value;
	}
	//演算オペレータ
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator++() const
	{
		updateNext();
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator--() const
	{
		updatePrev();
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator++()
	{
		updateNext();
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator--()
	{
		updatePrev();
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator++(int) const
	{
		reverse_iterator ite(*this);
		++(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator--(int) const
	{
		reverse_iterator ite(*this);
		--(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator++(int)
	{
		reverse_iterator ite(*this);
		++(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator--(int)
	{
		reverse_iterator ite(*this);
		--(*this);
		return ite;
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator+=(const int rhs) const
	{
		updateForward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator-=(const int rhs) const
	{
		updateBackward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator+=(const int rhs)
	{
		updateForward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator-=(const int rhs)
	{
		updateBackward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator+(const int rhs) const
	{
		reverse_iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator-(const int rhs) const
	{
		reverse_iterator ite(*this);
		ite -= rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator+(const int rhs)
	{
		reverse_iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator-(const int rhs)
	{
		reverse_iterator ite(*this);
		ite -= rhs;
		return ite;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
	//template<class OPE_TYPE>
	//inline int container<OPE_TYPE>::reverse_iterator::operator-(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	//{
	//	return ???;
	//}
	//アクセッサ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isExist() const
	{
		return m_value != nullptr;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isEnabled() const
	{
		return m_value != nullptr || m_isEnd;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isEnd() const//終端か？
	{
		return m_isEnd;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::reverse_iterator::getValue() const//現在の値（ノード）
	{
		return m_value;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::reverse_iterator::getValue()//現在の値（ノード）
	{
		return m_value;
	}
	//ベースを取得
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::reverse_iterator::base() const
	{
		iterator ite(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::reverse_iterator::base()
	{
		iterator ite(*this);
		return ite;
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = false;
		if (m_value)
		{
			if (m_value == reinterpret_cast<value_type*>(BEFORE_BEGIN))
			{
				m_value = m_con->m_first;
				if (!m_value)
					m_isEnd = true;
			}
			else
				++(*this);
		}
		else
		{
			if (rhs.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_last);
		}
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = false;
		if (m_value)
		{
			if (m_value == reinterpret_cast<value_type*>(BEFORE_BEGIN))
			{
				m_value = m_con->m_first;
				if (!m_value)
					m_isEnd = true;
			}
			else
				++(*this);
		}
		else
		{
			if (rhs.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_last);
		}
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(false)
	{
		if (m_value)
		{
			if (m_value == reinterpret_cast<value_type*>(BEFORE_BEGIN))
			{
				m_value = m_con->m_first;
				if (!m_value)
					m_isEnd = true;
			}
			else
				++(*this);
		}
		else
		{
			if (obj.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_last);
		}
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{
		if (m_value)
		{
			if (m_value == reinterpret_cast<value_type*>(BEFORE_BEGIN))
			{
				m_value = m_con->m_first;
				if (!m_value)
					m_isEnd = true;
			}
			else
				++(*this);
		}
		else
		{
			if (obj.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_last);
		}
	}
	//コンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::reverse_iterator::reverse_iterator(const container& con, const bool is_end) :
		m_con(&con),
		m_value(nullptr),
		m_isEnd(is_end)
	{
		if (!is_end)
		{
			m_value = const_cast<value_type*>(m_con->m_last);
			if (!m_value)
				m_isEnd = true;
		}
	}
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::reverse_iterator::reverse_iterator(const container& con, typename container<OPE_TYPE>::value_type* value, const bool is_end) :
		m_con(con),
		m_value(value),
		m_isEnd(is_end)
	{}

	//----------------------------------------
	//コンテナ本体のメソッド

	//配列の再割り当て
//	template<class OPE_TYPE>
//	template<std::size_t N>
//	inline void container<OPE_TYPE>::assignArray(typename container<OPE_TYPE>::value_type(&array)[N], const int size)
//	{
//	}

}//namespace singly_linked_list

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__SINGLY_LINKED_LIST_INL_

// End of file

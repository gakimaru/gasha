#pragma once
#ifndef __LINKED_LIST_INL_
#define __LINKED_LIST_INL_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// linked_list.inl
// 双方向連結リストコンテナ【インライン関数／テンプレート関数定義部】
//
// ※コンテナクラスの操作が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/linked_list.h>//双方向連結リストコンテナ【宣言部】

#include <gasha/is_ordered.h>//整列状態確認

#include <gasha/intro_sort.h>//イントロソート
#include <gasha/insertion_sort.h>//挿入ソート

#include <gasha/linear_search.h>//線形探索
#include <gasha/binary_search.h>//二分探索

#include <utility>//C++11 std::move

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で new 演算子を使用すると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace linked_list
{
	//--------------------
	//双方向連結リスト操作関数：指定ノードの次ノードを取得
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getNextNode(const typename OPE_TYPE::node_type& target)
	{
		const typename OPE_TYPE::node_type* node = &target;
		node = OPE_TYPE::getNext(*node);
		return node;
	}
	//--------------------
	//双方向連結リスト操作関数：指定ノードの後方のノードを取得
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
	//双方向連結リスト操作関数：指定ノードの前ノードを取得
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getPrevNode(const typename OPE_TYPE::node_type& target)
	{
		const typename OPE_TYPE::node_type* node = &target;
		node = OPE_TYPE::getPrev(*node);
		return node;
	}
	//--------------------
	//双方向連結リスト操作関数：指定ノードの前方のノードを取得
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getBackwardNode(const typename OPE_TYPE::node_type& target, std::size_t& step)
	{
		const typename OPE_TYPE::node_type* node = &target;
		while (step > 0 && node)
		{
			node = OPE_TYPE::getPrev(*node);
			--step;
		}
		return node;
	}
	//--------------------
	//双方向連結リスト操作関数：指定ノードの先頭ノードを取得
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getFirstNode(const typename OPE_TYPE::node_type& target)
	{
		const typename OPE_TYPE::node_type* node = &target;
		while (true)
		{
			const typename OPE_TYPE::node_type* prev = OPE_TYPE::getPrev(*node);
			if (!prev)
				return node;
			node = prev;
		}
		return nullptr;//ダミー
	}
	//--------------------
	//双方向連結リスト操作関数：指定ノードの末尾ノードを取得
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
	//双方向連結リスト操作関数：指定ノードの数を数える
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
	//双方向連結リスト操作関数：指定ノードの次に連結
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeAfter(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type& target, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		typename OPE_TYPE::node_type* next = const_cast<typename OPE_TYPE::node_type*>(OPE_TYPE::getNext(target));
		if (!next)
			last = &node;
		else
			OPE_TYPE::setPrev(*next, &node);
		OPE_TYPE::setPrev(node, &target);
		OPE_TYPE::setNext(target, &node);
		OPE_TYPE::setNext(node, next);
		return &node;
	}
	//--------------------
	//双方向連結リスト操作関数：指定ノードの前に連結
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeBefore(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type& target, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		typename OPE_TYPE::node_type* prev = const_cast<typename OPE_TYPE::node_type*>(OPE_TYPE::getPrev(target));
		if (!prev)
			first = &node;
		else
			OPE_TYPE::setNext(*prev, &node);
		OPE_TYPE::setNext(node, &target);
		OPE_TYPE::setPrev(target, &node);
		OPE_TYPE::setPrev(node, prev);
		return &node;
	}
	//--------------------
	//双方向連結リスト操作関数：先頭に連結
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeBeginning(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		if (!first)
		{
			first = &node;
			last = &node;
			OPE_TYPE::setPrev(node, nullptr);
			OPE_TYPE::setNext(node, nullptr);
			return &node;
		}
		OPE_TYPE::setNext(node, first);
		OPE_TYPE::setPrev(*first, &node);
		OPE_TYPE::setPrev(node, nullptr);
		first = &node;
		return &node;
	}
	//--------------------
	//双方向連結リスト操作関数：末尾に連結
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* insertNodeEnd(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		if (!last)
		{
			first = &node;
			last = &node;
			OPE_TYPE::setPrev(node, nullptr);
			OPE_TYPE::setNext(node, nullptr);
			return &node;
		}
		OPE_TYPE::setPrev(node, last);
		OPE_TYPE::setNext(*last, &node);
		OPE_TYPE::setNext(node, nullptr);
		last = &node;
		return &node;
	}
	//--------------------
	//双方向連結リスト操作関数：指定ノードを連結から外す
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* removeNode(typename OPE_TYPE::node_type& node, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		typename OPE_TYPE::node_type* prev = const_cast<typename OPE_TYPE::node_type*>(OPE_TYPE::getPrev(node));
		typename OPE_TYPE::node_type* next = const_cast<typename OPE_TYPE::node_type*>(OPE_TYPE::getNext(node));
		if (prev)
			OPE_TYPE::setNext(*prev, next);
		else
			first = next;
		if (next)
			OPE_TYPE::setPrev(*next, prev);
		else
			last = prev;
		OPE_TYPE::setPrev(node, nullptr);
		OPE_TYPE::setNext(node, nullptr);
		return &node;
	}
	//--------------------
	//双方向連結リスト操作関数：指定ノードの範囲を連結から外す
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* removeNodes(typename OPE_TYPE::node_type& start, typename OPE_TYPE::node_type& end, typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		typename OPE_TYPE::node_type* prev = const_cast<typename OPE_TYPE::node_type*>(OPE_TYPE::getPrev(start));
		typename OPE_TYPE::node_type* next = &end;
		typename OPE_TYPE::node_type* _end = next ? const_cast<typename OPE_TYPE::node_type*>(OPE_TYPE::getPrev(end)) : last;
		if (prev)
			OPE_TYPE::setNext(*prev, next);
		else
			first = next;
		if (next)
			OPE_TYPE::setPrev(*next, prev);
		else
			last = prev;
		OPE_TYPE::setPrev(start, nullptr);
		OPE_TYPE::setNext(*_end, nullptr);
		return &start;
	}

	//----------------------------------------
	//双方向連結リスト操作関数：線形探索
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
	//双方向連結リスト操作関数：二分探索
	template<class OPE_TYPE, typename V>
	const typename OPE_TYPE::node_type* binarySearchValue(const typename OPE_TYPE::node_type* first, const V& value)
	{
		return GASHA_ linkedListBinarySearchValue(first, OPE_TYPE::getNext, OPE_TYPE::getPrev, value);
	}
	template<class OPE_TYPE, typename V, class COMPARISON>
	const typename OPE_TYPE::node_type* binarySearchValue(const typename OPE_TYPE::node_type* first, const V& value, COMPARISON comparison)
	{
		return GASHA_ linkedListBinarySearchValue(first, OPE_TYPE::getNext, OPE_TYPE::getPrev, value, comparison);
	}
	template<class OPE_TYPE, class COMPARISON>
	const typename OPE_TYPE::node_type* binarySearch(const typename OPE_TYPE::node_type* first, COMPARISON comparison)
	{
		return GASHA_ linkedListBinarySearch(first, OPE_TYPE::getNext, OPE_TYPE::getPrev, comparison);
	}

	//----------------------------------------
	//双方向連結リスト操作関数：非整列状態確認
	template<class OPE_TYPE, class PREDICATE>
	bool isUnordered(const typename OPE_TYPE::node_type* first)
	{
		return GASHA_ singlyLinkedListIsUnordered(first, OPE_TYPE::getNext);
	}
	template<class OPE_TYPE, class PREDICATE>
	bool isUnordered(const typename OPE_TYPE::node_type* first, PREDICATE predicate)
	{
		return GASHA_ singlyLinkedListIsUnordered(first, OPE_TYPE::getNext, predicate);
	}

	//----------------------------------------
	//双方向連結リスト操作関数：整列状態確認
	template<class OPE_TYPE, class PREDICATE>
	bool isOrdered(const typename OPE_TYPE::node_type* first)
	{
		return GASHA_ singlyLinkedListIsOrdered(first, OPE_TYPE::getNext);
	}
	template<class OPE_TYPE, class PREDICATE>
	bool isOrdered(const typename OPE_TYPE::node_type* first, PREDICATE predicate)
	{
		return GASHA_ singlyLinkedListIsOrdered(first, OPE_TYPE::getNext, predicate);
	}

	//----------------------------------------
	//双方向連結リスト操作関数：非整列要素数計上
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
	//双方向連結リスト操作関数：挿入ソート
	template<class OPE_TYPE, class PREDICATE>
	std::size_t insertionSort(typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		return GASHA_ linkedListInsertionSort(first, last, OPE_TYPE::getNext, OPE_TYPE::getPrev, insertNodeBefore<OPE_TYPE>, removeNode<OPE_TYPE>);
	}
	template<class OPE_TYPE, class PREDICATE>
	std::size_t insertionSort(typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last, PREDICATE predicate)
	{
		return GASHA_ linkedListInsertionSort(first, last, OPE_TYPE::getNext, OPE_TYPE::getPrev, insertNodeBefore<OPE_TYPE>, removeNode<OPE_TYPE>, predicate);
	}

	//----------------------------------------
	//双方向連結リスト操作関数：シェルソート
	template<class OPE_TYPE, class PREDICATE>
	std::size_t shellSort(typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last)
	{
		return GASHA_ linkedListShellSort(first, last, OPE_TYPE::getNext, OPE_TYPE::getPrev, insertNodeBefore<OPE_TYPE>, removeNode<OPE_TYPE>);
	}
	template<class OPE_TYPE, class PREDICATE>
	std::size_t shellSort(typename OPE_TYPE::node_type*& first, typename OPE_TYPE::node_type*& last, PREDICATE predicate)
	{
		return GASHA_ linkedListShellSort(first, last, OPE_TYPE::getNext, OPE_TYPE::getPrev, insertNodeBefore<OPE_TYPE>, removeNode<OPE_TYPE>, predicate);
	}

	//--------------------
	//イテレータのインライン関数
	
	//基本オペレータ
#ifdef GASHA_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
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
#endif//GASHA_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
	//比較オペレータ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator==(const typename container<OPE_TYPE>::iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			m_value == rhs.m_value;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator!=(const typename container<OPE_TYPE>::iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? false :
			m_isEnd || rhs.m_isEnd ? true :
			m_value != rhs.m_value;
	}
	//演算オペレータ
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator++()
	{
		updateNext();
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator--()
	{
		updatePrev();
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator++(int)
	{
		iterator ite(*this);
		++(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator--(int)
	{
		iterator ite(*this);
		--(*this);
		return ite;
	}
#ifdef GASHA_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator+=(const typename container<OPE_TYPE>::difference_type rhs)
	{
		updateForward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator-=(const typename container<OPE_TYPE>::difference_type rhs)
	{
		updateBackward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator+(const typename container<OPE_TYPE>::difference_type rhs) const
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator-(const typename container<OPE_TYPE>::difference_type rhs) const
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
#endif//GASHA_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
	//アクセッサ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isExist() const
	{
		return m_value != nullptr;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isEnabled() const
	{
		return m_value != nullptr || m_isEnd;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isEnd() const//終端か？
	{
		return m_isEnd;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::iterator::getValue() const//現在のノード
	{
		return m_value;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type*  container<OPE_TYPE>::iterator::getValue()//現在のノード
	{
		return m_value;
	}

	//--------------------
	//リバースイテレータのインライン関数
	//基本オペレータ
#ifdef GASHA_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
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
#endif//GASHA_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
	//比較オペレータ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator==(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			m_value == rhs.m_value;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator!=(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? false :
			rhs.m_isEnd || m_isEnd ? true :
			m_value != rhs.m_value;
	}
	//演算オペレータ
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
#ifdef GASHA_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator+=(const typename container<OPE_TYPE>::difference_type rhs)
	{
		updateForward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator-=(const typename container<OPE_TYPE>::difference_type rhs)
	{
		updateBackward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator+(const typename container<OPE_TYPE>::difference_type rhs) const
	{
		reverse_iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator-(const typename container<OPE_TYPE>::difference_type rhs) const
	{
		reverse_iterator ite(*this);
		ite -= rhs;
		return ite;
	}
#endif//GASHA_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
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
	inline typename container<OPE_TYPE>::value_type*  container<OPE_TYPE>::reverse_iterator::getValue()//現在の値（ノード）
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

	//----------------------------------------
	//コンテナ本体のメソッド

	//先頭にノードを挿入（連結に追加）
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::push_front(const typename container<OPE_TYPE>::node_type& node)
	{
		return insertNodeBeginning<ope_type>(*const_cast<node_type*>(&node), m_first, m_last);
	}

	//末尾にノードを挿入（連結に追加）
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::push_back(const typename container<OPE_TYPE>::node_type& node)
	{
		return insertNodeEnd<ope_type>(*const_cast<node_type*>(&node), m_first, m_last);
	}

	//先頭ノードを削除（連結解除）
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::pop_front()
	{
		if (!m_first)
			return nullptr;
		return removeNode<ope_type>(*m_first, m_first, m_last);
	}

	//末尾ノードを削除（連結解除）
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::pop_back()
	{
		if (!m_first)
			return nullptr;
		return removeNode<ope_type>(*m_last, m_first, m_last);
	}

	//指定の位置の後ろにノードを挿入（連結に追加）
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::insert(typename container<OPE_TYPE>::iterator pos, const typename container<OPE_TYPE>::node_type& node)
	{
		if (pos.isNotExist())
			return nullptr;
		return insertNodeAfter<ope_type>(*const_cast<node_type*>(&node), *pos, m_first, m_last);
	}

	//指定の位置の前にノードを挿入（連結に追加）
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::insert_before(typename container<OPE_TYPE>::iterator pos, const typename container<OPE_TYPE>::node_type& node)
	{
		if (pos.isNotExist())
			return nullptr;
		return insertNodeBefore<ope_type>(*const_cast<node_type*>(&node), *pos, m_first, m_last);
	}

	//指定ノードを削除（連結解除）
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::remove(typename container<OPE_TYPE>::node_type& node)
	{
		if (!m_first)
			return nullptr;
		return removeNode<ope_type>(node, m_first, m_last);
	}

	//指定位置のノードを削除（連結解除）
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::erase(typename container<OPE_TYPE>::iterator pos)
	{
		if (!m_first || pos.isNotExist())
			return nullptr;
		return removeNode<ope_type>(*pos, m_first, m_last);
	}

	//指定範囲のノードを削除（連結解除）
	//※範囲はイテレータで指定
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::erase(typename container<OPE_TYPE>::iterator start, typename container<OPE_TYPE>::iterator end)
	{
		if (!m_first || start.isNotExist() || end.isNotEnabled())
			return nullptr;
		return removeNodes<ope_type>(*start, *end, m_first, m_last);
	}

	//ソート
	template<class OPE_TYPE>
	inline void container<OPE_TYPE>::sort()
	{
	#ifdef GASHA_LINKED_LIST_USE_SHELL_SORT
		shellSort<OPE_TYPE>(m_first, m_last, typename ope_type::predicateForSort());
	#else//GASHA_LINKED_LIST_USE_SHELL_SORT
		insertionSort<OPE_TYPE>(m_first, m_last, typename ope_type::predicateForSort());
	#endif//GASHA_LINKED_LIST_USE_SHELL_SORT
	}
	//※プレディケート関数指定版
	template<class OPE_TYPE>
	template<class PREDICATE>
	inline void container<OPE_TYPE>::sort(PREDICATE predicate)
	{
	#ifdef GASHA_LINKED_LIST_USE_SHELL_SORT
		shellSort<OPE_TYPE>(m_first, m_last, predicate);
	#else//GASHA_LINKED_LIST_USE_SHELL_SORT
		insertionSort<OPE_TYPE>(m_first, m_last, predicate);
	#endif//GASHA_LINKED_LIST_USE_SHELL_SORT
	}

#ifdef GASHA_LINKED_LIST_ENABLE_STABLE_SORT
	//安定ソート
	template<class OPE_TYPE>
	inline void container<OPE_TYPE>::stableSort()
	{
		insertionSort<OPE_TYPE>(m_first, m_last, typename ope_type::predicateForSort());
	}
	//※プレディケート関数指定版
	template<class OPE_TYPE>
	template<class PREDICATE>
	inline void container<OPE_TYPE>::stableSort(PREDICATE predicate)
	{
		insertionSort<OPE_TYPE>(m_first, m_last, predicate);
	}
#endif//GASHA_LINKED_LIST_ENABLE_STABLE_SORT

	//ソート済み状態チェック
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::isOrdered() const
	{
		return linked_list::isOrdered<OPE_TYPE>(m_first, typename ope_type::predicateForSort());
	}
	//※プレディケート関数指定版
	template<class OPE_TYPE>
	template<class PREDICATE>
	inline bool container<OPE_TYPE>::isOrdered(PREDICATE predicate) const
	{
		return linked_list::isOrdered<OPE_TYPE>(m_first, predicate);
	}

	//線形探索
	//※探索値指定版
	template<class OPE_TYPE>
	template<typename V>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::findValue(const V& value)
	{
		const node_type* found_node = linearSearchValue<ope_type>(m_first, value, typename ope_type::predicateForFind());
		iterator found(*this, const_cast<node_type*>(found_node), found_node == nullptr);
		return found;
	}
	//※比較関数＋値指定版
	template<class OPE_TYPE>
	template<typename V, class PREDICATE>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::findValue(const V& value, PREDICATE predicate)
	{
		const node_type* found_node = linearSearchValue<ope_type>(m_first, value, predicate);
		iterator found(*this, const_cast<node_type*>(found_node), found_node == nullptr);
		return found;
	}
	//※比較関数指定版
	template<class OPE_TYPE>
	template<class PREDICATE>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::find(PREDICATE predicate)
	{
		const node_type* found_node = linearSearch<ope_type>(m_first, predicate);
		iterator found(*this, const_cast<node_type*>(found_node), found_node == nullptr);
		return found;
	}

#ifdef GASHA_LINKED_LIST_ENABLE_BINARY_SEARCH
	//二分探索
	//※探索値指定版
	template<class OPE_TYPE>
	template<typename V>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::binarySearchValue(const V& value)
	{
		const node_type* found_node = linked_list::binarySearchValue<ope_type>(m_first, value, typename ope_type::comparisonForSearch());
		iterator found(*this, const_cast<node_type*>(found_node), found_node == nullptr);
		return found;
	}
	//※比較関数＋値指定版
	template<class OPE_TYPE>
	template<typename V, class COMPARISON>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::binarySearchValue(const V& value, COMPARISON comparison)
	{
		const node_type* found_node = linked_list::binarySearchValue<ope_type>(m_first, value, comparison);
		iterator found(*this, const_cast<node_type*>(found_node), found_node == nullptr);
		return found;
	}
	//※比較関数指定版
	template<class OPE_TYPE>
	template<class COMPARISON>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::binarySearch(COMPARISON comparison)
	{
		const node_type* found_node = linked_list::binarySearch<ope_type>(m_first, comparison);
		iterator found(*this, const_cast<node_type*>(found_node), found_node == nullptr);
		return found;
	}

	//デフォルトコンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::container() :
		m_first(nullptr),
		m_last(nullptr)
	{}
#endif//GASHA_LINKED_LIST_ENABLE_BINARY_SEARCH
	
	//----------------------------------------
	//シンプル双方向連結リストコンテナ
	
	//明示的なコンストラクタ呼び出し
	template<typename VALUE_TYPE>
	template<typename... Tx>
	inline void simpleContainer<VALUE_TYPE>::node::constructor(const Tx&... args)
	{
		new(&m_value)core_value_type(args...);
	}
	
	//明示的なデストラクタ呼び出し
	template<typename VALUE_TYPE>
	inline void simpleContainer<VALUE_TYPE>::node::destructor()
	{
		m_value.~core_value_type();//デストラクタ呼び出し
		operator delete(&m_value, &m_value);//（作法として）deleteオペレータ呼び出し
	}

	//ムーブオペレータ
	template<typename VALUE_TYPE>
	inline typename simpleContainer<VALUE_TYPE>::node& simpleContainer<VALUE_TYPE>::node::operator=(typename simpleContainer<VALUE_TYPE>::core_value_type&& value)
	{
		m_value = std::move(value);
		return *this;
	}
	
	//コピーオペレータ
	template<typename VALUE_TYPE>
	inline typename simpleContainer<VALUE_TYPE>::node& simpleContainer<VALUE_TYPE>::node::operator=(const typename simpleContainer<VALUE_TYPE>::core_value_type& value)
	{
		m_value = value;
		return *this;
	}
	
	//ムーブコンストラクタ
	template<typename VALUE_TYPE>
	inline simpleContainer<VALUE_TYPE>::node::node(typename simpleContainer<VALUE_TYPE>::core_value_type&& value) :
		m_value(std::move(value)),
		m_next(nullptr),
		m_prev(nullptr)
	{}
	
	//コピーコンストラクタ
	template<typename VALUE_TYPE>
	inline simpleContainer<VALUE_TYPE>::node::node(const typename simpleContainer<VALUE_TYPE>::core_value_type& value) :
		m_value(value),
		m_next(nullptr),
		m_prev(nullptr)
	{}
	
	//デフォルトコンストラクタ
	template<typename VALUE_TYPE>
	inline simpleContainer<VALUE_TYPE>::node::node() :
		m_value(),
		m_next(nullptr),
		m_prev(nullptr)
	{}
	
	//デストラクタ
	template<typename VALUE_TYPE>
	inline simpleContainer<VALUE_TYPE>::node::~node()
	{}

}//namespace linked_list

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//__LINKED_LIST_INL_

// End of file

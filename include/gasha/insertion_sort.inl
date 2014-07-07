#pragma once
#ifndef GASHA_INCLUDED_INSERTION_SORT_INL
#define GASHA_INCLUDED_INSERTION_SORT_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// insertion_sort.inl
// 挿入ソート【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/insertion_sort.h>//挿入ソート【宣言部】

#include <gasha/utility.h>//汎用ユーティリティ（値交換用）

#include <gasha/iterator.h>//イテレータ用アルゴリズム

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//ソートアルゴリズム分類：挿入ソート
//========================================

//----------------------------------------
//アルゴリズム：挿入ソート
template<class T, class PREDICATE>
std::size_t insertionSort(T* array, const std::size_t size, PREDICATE predicate)
{
	if (!array || size <= 1)
		return 0;
	std::size_t swapped_count = 0;
	const T* end = array + size;
	T* now = array;
	T* next = now + 1;
	while (next < end)
	{
		if (predicate(*next, *now))
		{
			T* min = now;
			T* prev = now - 1;
			while (prev >= array)
			{
				if (predicate(*next, *prev))
					min = prev;
				else
					break;
				--prev;
			}
			GASHA_ rotateValues(min, next, 1);
			++swapped_count;
		}
		++now;
		++next;
	}
	return swapped_count;
}

//----------------------------------------
//アルゴリズム：挿入ソート
//※イテレータ対応版
template<class ITERATOR, class PREDICATE>
std::size_t iteratorInsertionSort(ITERATOR begin, ITERATOR end, PREDICATE predicate)
{
	if (begin == end)
		return 0;
	std::size_t swapped_count = 0;
	ITERATOR now = begin;
	ITERATOR next = begin;
	++next;
	while (next < end)
	{
		if (predicate(*next, *now))
		{
			ITERATOR min = now;
			ITERATOR prev = now;
			--prev;
			while (prev >= begin)
			{
				if (predicate(*next, *prev))
					min = prev;
				else
					break;
				--prev;
			}
			GASHA_ iteratorRotateValues(min, next, 1);
			++swapped_count;
		}
		++now;
		++next;
	}
	return swapped_count;
}

//----------------------------------------
//アルゴリズム：挿入ソート
//※双方向連結リスト対応版
template<class T, class GET_NEXT_FUNC, class GET_PREV_FUNC, class INSERT_NODE_BEFORE_FUNC, class REMOVE_NODE_FUNC, class PREDICATE>
std::size_t linkedListInsertionSort(T*& first, T*& last, GET_NEXT_FUNC get_next_func, GET_PREV_FUNC get_prev_func, INSERT_NODE_BEFORE_FUNC insert_node_before_func, REMOVE_NODE_FUNC remove_node_func, PREDICATE predicate)
{
	if (!first || !get_next_func(*first))
		return 0;
	std::size_t swapped_count = 0;
	T* now = first;
	T* next = const_cast<T*>(get_next_func(*now));
	while (next)
	{
		if (predicate(*next, *now))
		{
			T* min = now;
			T* prev = const_cast<T*>(get_prev_func(*now));
			while (prev)
			{
				if (predicate(*next, *prev))
					min = prev;
				else
					break;
				prev = const_cast<T*>(get_prev_func(*prev));
			}
			remove_node_func(*next, first, last);
			insert_node_before_func(*next, min, first, last);
			++swapped_count;
			next = const_cast<T*>(get_next_func(*now));
		}
		else
		{
			now = next;
			next = const_cast<T*>(get_next_func(*next));
		}
	}
	return swapped_count;
}

//----------------------------------------
//アルゴリズム：変形挿入ソート
//※片方向連結リスト対応版
template<class T, class GET_NEXT_FUNC, class INSERT_NODE_AFTER_FUNC, class REMOVE_NODE_AFTER_FUNC, class PREDICATE>
std::size_t singlyLinkedListInsertionSort(T*& first, T*& last, GET_NEXT_FUNC get_next_func, INSERT_NODE_AFTER_FUNC insert_node_after_func, REMOVE_NODE_AFTER_FUNC remove_node_after_func, PREDICATE predicate)
{
	if (!first || !get_next_func(*first))
		return 0;
	std::size_t swapped_count = 0;
	T* now = first;
	T* next = const_cast<T*>(get_next_func(*now));
	while (next)
	{
		if (predicate(*next, *now))
		{
			T* min_before = nullptr;
			T* min = first;
			while (min != now)
			{
				if (predicate(*next, *min))
					break;
				min_before = min;
				min = const_cast<T*>(get_next_func(*min));
			}
			remove_node_after_func(now, first, last);//nextを連結から外す
			insert_node_after_func(*next, min_before, first, last);
			++swapped_count;
			next = const_cast<T*>(get_next_func(*now));
		}
		else
		{
			now = next;
			next = const_cast<T*>(get_next_func(*next));
		}
	}
	return swapped_count;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_INSERTION_SORT_INL

// End of file

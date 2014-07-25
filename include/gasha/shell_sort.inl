#pragma once
#ifndef GASHA_INCLUDED_SHELL_SORT_INL
#define GASHA_INCLUDED_SHELL_SORT_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shell_sort.inl
// シェルソート【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shell_sort.h>//シェルソート【宣言部】

#include <gasha/utility.h>//汎用ユーティリティ（値交換用）

#include <gasha/iterator.h>//イテレータ用アルゴリズム

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//ソートアルゴリズム分類：挿入ソート
//========================================

//----------------------------------------
//アルゴリズム：シェルソート
template<class T, class PREDICATE>
std::size_t shellSort(T* array, const std::size_t size, PREDICATE predicate)
{
	if (!array || size <= 1)
		return 0;
	std::size_t swapped_count = 0;
	const T* end = array + size;
	const int h_max = static_cast<int>(size) / 3;
	int h = 1;
	while (h <= h_max)
		h = 3 * h + 1;
	while (h > 0)
	{
		T* now = array;
		T* next = now + h;
		while (next < end)
		{
			if (predicate(*next, *now))
			{
				T* min = now;
				T* prev = now - h;
				while (prev >= array)
				{
					if (predicate(*next, *prev))
						min = prev;
					else
						break;
					prev -= h;
				}
				GASHA_ rotateValues(min, next, h);
				++swapped_count;
			}
			++now;
			++next;
		}
		h = (h - 1) / 3;
	}
	return swapped_count;
}

//----------------------------------------
//アルゴリズム：シェルソート
//※イテレータ対応版
template<class ITERATOR, class PREDICATE>
std::size_t iteratorShellSort(ITERATOR begin, ITERATOR end, PREDICATE predicate)
{
	if (begin == end)
		return 0;
	std::size_t swapped_count = 0;
	const typename ITERATOR::difference_type size = iteratorDifference(begin, end);
	const typename ITERATOR::difference_type h_max = size / 3;
	typename ITERATOR::difference_type h = 1;
	while (h <= h_max)
		h = 3 * h + 1;
	while (h > 0)
	{
		ITERATOR now = begin;
		ITERATOR next = begin;
		next += h;
		while (next < end)
		{
			if (predicate(*next, *now))
			{
				ITERATOR min = now;
				ITERATOR prev = now;
				prev -= h;
				while (prev >= begin)
				{
					if (predicate(*next, *prev))
						min = prev;
					else
						break;
					prev -= h;
				}
				GASHA_ iteratorRotateValues(min, next, h);
				++swapped_count;
			}
			++now;
			++next;
		}
		h = (h - 1) / 3;
	}
	return swapped_count;
}

//----------------------------------------
//アルゴリズム：シェルソート ※双方向連結リスト対応版
template<class T, class GET_NEXT_FUNC, class GET_PREV_FUNC, class INSERT_NODE_BEFORE_FUNC, class REMOVE_NODE_FUNC, class PREDICATE>
std::size_t linkedListShellSort(T*& first, T*& last, GET_NEXT_FUNC get_next_func, GET_PREV_FUNC get_prev_func, INSERT_NODE_BEFORE_FUNC insert_node_before_func, REMOVE_NODE_FUNC remove_node_func, PREDICATE predicate)
{
	if (!first || !get_next_func(*first))
		return 0;
	std::size_t swapped_count = 0;
	std::size_t size = 0;
	{
		const T* node = first;
		while (node)
		{
			++size;
			node = get_next_func(*node);
		}
	}
	const int h_max = static_cast<int>(size) / 3;
	int h = 1;
	while (h <= h_max)
		h = 3 * h + 1;
	while (h > 0)
	{
		T* now = first;
		T* next = now;
		for (int i = 0; i < h && next; ++i)
			next = const_cast<T*>(get_next_func(*next));
		while (next)
		{
			if (predicate(*next, *now))
			{
				T* min = now;
				T* prev = now;
				for (int i = 0; i < h && prev; ++i)
					prev = const_cast<T*>(get_prev_func(*prev));
				while (prev)
				{
					if (predicate(*next, *prev))
						min = prev;
					else
						break;
					for (int i = 0; i < h && prev; ++i)
						prev = const_cast<T*>(get_prev_func(*prev));
				}
				remove_node_func(*next, first, last);
				insert_node_before_func(*next, min, first, last);
				++swapped_count;
				next = now;
				for (int i = 0; i < h && next; ++i)
					next = const_cast<T*>(get_next_func(*next));
			}
			else
			{
				now = const_cast<T*>(get_next_func(*now));
				next = const_cast<T*>(get_next_func(*next));
			}
		}
		h = (h - 1) / 3;
	}
	return swapped_count;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SHELL_SORT_INL

// End of file

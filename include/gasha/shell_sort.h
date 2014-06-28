#pragma once
#ifndef __SHELL_SORT_H_
#define __SHELL_SORT_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shell_sort.h
// シェルソート
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/sort_basic.h>//ソート処理基本
#include <gasha/utility.h>//汎用ユーティリティ（値交換用）

#include <gasha/iterator.h>//イテレータ用アルゴリズム

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//ソートアルゴリズムの説明
//========================================

//・計算時間：
//    - O(n)       ... データ件数分の時間
//    - O(n ^ 2)   ... データ件数の２乗分の時間
//    - O(log n)   ... log2(データ件数)分の時間（4→2, 16→4, 1024→10,1048576→20）
//    - O(n log n) ... n×log n 分の時間
//・メモリ使用量：
//    - O(1)       ... １件分のメモリが必要
//    - O(n)       ... データ件数分のメモリが必要
//    - O(log n)   ... log2(データ件数)分のメモリが必要
//・安定性：
//    - ○         ... キーが同じデータの順序性が維持されることを保証する
//                     例：{ 3-a, 5-b, 4-c, 5-d, 9-e, 3-f, 4-g, 3-h, 5-i } → { 3-a, 3-f, 3-h, 4-c, 4-g, 5-b, 5-d, 5-i, 9-e }
//    - ×         ... 例：(同上)                        

//========================================
//ソートアルゴリズム分類：挿入ソート
//========================================

//----------------------------------------
//アルゴリズム：シェルソート
//----------------------------------------
//・最良計算時間：O(n)
//・平均計算時間：O(n log^2 n) or O(n^3/2)
//・最悪計算時間：O(n log^2 n)
//・メモリ使用量：O(1)
//・安定性：　　　×
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
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
sortingFuncSet(shellSort);

//----------------------------------------
//アルゴリズム：シェルソート
//※イテレータ対応版
//----------------------------------------
//・最良計算時間：O(n)
//・平均計算時間：O(n log^2 n) or O(n^3/2)
//・最悪計算時間：O(n log^2 n)
//・メモリ使用量：O(1)
//・安定性：　　　×
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const typename ITERATOR::value_type& value1, const typename ITERATOR::value_type& value2)//value1 == value2 ならtrueを返す
template<class ITERATOR, class PREDICATE>
std::size_t iteratorShellSort(ITERATOR begin, ITERATOR end, PREDICATE predicate)
{
	if (begin == end)
		return 0;
	std::size_t swapped_count = 0;
	const typename ITERATOR::difference_type size = iteratorDifference(begin, end);
	const int h_max = size / 3;
	int h = 1;
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
iteratorSortingFuncSet(iteratorShellSort);

//----------------------------------------
//アルゴリズム：シェルソート ※双方向連結リスト対応版
//----------------------------------------
//・最良計算時間：O(n)
//・平均計算時間：O(n log^2 n) or O(n^3/2)
//・最悪計算時間：O(n log^2 n)
//・メモリ使用量：O(1)
//・安定性：　　　×
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
//・T* GET_NEXT_FUNC(T& node)//次のノードを返す
//・T* GET_PREV_FUNC(T& node)//前のノードを返す
//・void INSERT_NODE_BEFORE_FUNC(T& new_node, T& target_node, T*& first_ref, T*& last_ref)//target_nodeの前にnew_nodeを連結する
//・void REMOVE_NODE_FUNC(T& target_node, T*& first_ref, T*& last_ref)//target_nodeを連結から解除する
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
	const int h_max = size / 3;
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
				insert_node_before_func(*next, *min, first, last);
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
linkedListSortingFuncSet(linkedListShellSort);

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__SHELL_SORT_H_

// End of file

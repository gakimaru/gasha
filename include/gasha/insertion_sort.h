﻿#pragma once
#ifndef __INSERTION_SORT_H_
#define __INSERTION_SORT_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// insertion_sort.h
// 挿入ソート
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
//アルゴリズム：挿入ソート
//----------------------------------------
//・最良計算時間：O(n)
//・平均計算時間：O(n^2)
//・最悪計算時間：O(n^2)
//・メモリ使用量：O(1)
//・安定性：　　　○
//----------------------------------------
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
			rotateValues(min, next, 1);
			++swapped_count;
		}
		++now;
		++next;
	}
	return swapped_count;
}
sortingFuncSet(insertionSort);

//----------------------------------------
//アルゴリズム：挿入ソート
//※イテレータ対応版
//----------------------------------------
//・最良計算時間：O(n)
//・平均計算時間：O(n^2)
//・最悪計算時間：O(n^2)
//・メモリ使用量：O(1)
//・安定性：　　　○
//----------------------------------------
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
			iteratorRotateValues(min, next, 1);
			++swapped_count;
		}
		++now;
		++next;
	}
	return swapped_count;
}
iteratorSortingFuncSet(iteratorInsertionSort);

//----------------------------------------
//アルゴリズム：挿入ソート
//※双方向連結リスト対応版
//----------------------------------------
//・最良計算時間：O(n)
//・平均計算時間：O(n^2)
//・最悪計算時間：O(n^2)
//・メモリ使用量：O(1)
//・安定性：　　　○
//----------------------------------------
template<class NODE_TYPE, class GET_NEXT_FUNC, class GET_PREV_FUNC, class INSERT_NODE_BEFORE_FUNC, class REMOVE_NODE_FUNC, class PREDICATE>
std::size_t linkedListInsertionSort(NODE_TYPE*& first, NODE_TYPE*& last, GET_NEXT_FUNC get_next_func, GET_PREV_FUNC get_prev_func, INSERT_NODE_BEFORE_FUNC insert_node_before_func, REMOVE_NODE_FUNC remove_node_func, PREDICATE predicate)
{
	typedef NODE_TYPE node_type;
	if (!first || !get_next_func(*first))
		return 0;
	std::size_t swapped_count = 0;
	node_type* now = first;
	node_type* next = const_cast<node_type*>(get_next_func(*now));
	while (next)
	{
		if (predicate(*next, *now))
		{
			node_type* min = now;
			node_type* prev = const_cast<node_type*>(get_prev_func(*now));
			while (prev)
			{
				if (predicate(*next, *prev))
					min = prev;
				else
					break;
				prev = const_cast<node_type*>(get_prev_func(*prev));
			}
			remove_node_func(*next, first, last);
			insert_node_before_func(*next, *min, first, last);
			++swapped_count;
			next = const_cast<node_type*>(get_next_func(*now));
		}
		else
		{
			now = next;
			next = const_cast<node_type*>(get_next_func(*next));
		}
	}
	return swapped_count;
}
linkedListSortingFuncSet(linkedListInsertionSort);

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__INSERTION_SORT_H_

// End of file

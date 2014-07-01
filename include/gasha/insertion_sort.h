#pragma once
#ifndef GASHA_INCLUDED_INSERTION_SORT_H
#define GASHA_INCLUDED_INSERTION_SORT_H

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
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
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
//プロトタイプ：
//・bool PREDICATE(const typename ITERATOR::value_type& value1, const typename ITERATOR::value_type& value2)//value1 == value2 ならtrueを返す
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
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
//・T* GET_NEXT_FUNC(T& node)//次のノードを返す
//・T* GET_PREV_FUNC(T& node)//前のノードを返す
//・void INSERT_NODE_BEFORE_FUNC(T& new_node, T* target_node, T*& first_ref, T*& last_ref)//target_nodeの前にnew_nodeを連結する(target_nodeがnullptrだと末尾に連結)
//・void REMOVE_NODE_FUNC(T& target_node, T*& first_ref, T*& last_ref)//target_nodeを連結から解除する
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
linkedListSortingFuncSet(linkedListInsertionSort);

//----------------------------------------
//アルゴリズム：変形挿入ソート
//※片方向連結リスト対応版
//----------------------------------------
//・最良計算時間：O(n)
//・平均計算時間：O(n^2)
//・最悪計算時間：O(n^2)
//・メモリ使用量：O(1)
//・安定性：　　　○
//----------------------------------------
//挿入先の探索を先頭から行う方式
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
//・T* GET_NEXT_FUNC(T& node)//次のノードを返す
//・void INSERT_NODE_AFTER_FUNC(T& new_node, T* target_node, T*& first_ref, T*& last_ref)//target_nodeの後にnew_nodeを連結する(target_nodeがnullptrだと先頭に連結)
//・void REMOVE_NODE_AFTER_FUNC(T& prev_target_node, T*& first_ref, T*& last_ref)//prev_target_nodeの次のノードを連結から解除する
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
singlyLinkedListSortingFuncSet(singlyLinkedListInsertionSort);

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_INSERTION_SORT_H

// End of file

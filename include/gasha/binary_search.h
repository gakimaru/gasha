#pragma once
#ifndef _BINARY_SEARCH_H_
#define _BINARY_SEARCH_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// binary_search.h
// 二分探索
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/search_basic.h>//探索処理基本

#include <gasha/iterator.h>//イテレータ用アルゴリズム

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//探索アルゴリズムの説明
//========================================

//・計算時間：
//    - O(n)       ... データ件数分の時間
//    - O(n ^ 2)   ... データ件数の２乗分の時間
//    - O(log n)   ... log2(データ件数)分の時間（4→2, 16→4, 1024→10,1048576→20）
//    - O(n log n) ... n×log n 分の時間

//----------------------------------------
//アルゴリズム：二分探索
//----------------------------------------
//・最良計算時間：O(1)
//・平均計算時間：O(log n)
//・最悪計算時間：O(log n)
//・探索失敗時：  O(log n)
//----------------------------------------
//プロトタイプ：
//・bool COMPARISON(const T& value)//value == 探索値なら0を、value < 探索値なら-1以下を、value > 探索値なら1以上を返す
template<class T, class COMPARISON>
T* binarySearch(T* array, const std::size_t size, COMPARISON comparison)
{
	if (!array || size == 0)
		return nullptr;
	std::size_t range = size;
	T* begin = array;
	T* found = nullptr;
	while (true)
	{
		const std::size_t range_half = range / 2;//探索範囲の半分の範囲
		T* mid = begin + range_half;//探索範囲の中心要素
		const int comp = comparison(*mid);//中心要素を探索値と比較
		if (comp == 0)//中心要素が探索値と一致
			found = mid;//発見した場所を記憶 ※見つかった位置の先頭を発見するため、探索を続行する
		if (range_half == 0)//探索範囲が残っていなければ探索終了
			break;
		if (comp <= 0)//探索値が中心要素より小さいか同じだった場合、次に中心より前の範囲に絞って探索する
			range = range_half;
		else//if (comp > 0)//探索値が中心要素より大きかった場合、次に中心より後の範囲に絞って探索する
		{
			begin = mid + 1;
			range -= (range_half + 1);
		}
	}
	if (found && found != begin)//見つかった地点が先頭でなければ、一つ前を調べる
	{
		T* found_prev = found - 1;
		if (comparison(*found_prev) == 0)//一つ前が一致するならそれを返す
			found = found_prev;
	}
	return found;
}
searchFuncSetByComparison(binarySearch);

//----------------------------------------
//アルゴリズム：二分探索
//※イテレータ対応版
//----------------------------------------
//・最良計算時間：O(1)
//・平均計算時間：O(log n)
//・最悪計算時間：O(log n)
//・探索失敗時：  O(log n)
//----------------------------------------
//プロトタイプ：
//・bool COMPARISON(const typename ITERATOR::value_type& value)//value == 探索値なら0を、value < 探索値なら-1以下を、value > 探索値なら1以上を返す
template<class ITERATOR, class COMPARISON>
ITERATOR iteratorBinarySearch(ITERATOR begin, ITERATOR end, COMPARISON comparison)
{
	if (begin == end)
		return end;
	const typename ITERATOR::difference_type size = iteratorDifference(begin, end);
	typename ITERATOR::difference_type range = size;
	ITERATOR found = end;
	while (true)
	{
		const typename ITERATOR::difference_type range_half = range / 2;//探索範囲の半分の範囲
		ITERATOR mid = begin + range_half;//探索範囲の中心要素
		const int comp = comparison(*mid);//中心要素を探索値と比較
		if (comp == 0)//中心要素が探索値と一致
			found = mid;//発見した場所を記憶 ※見つかった位置の先頭を発見するため、探索を続行する
		if (range_half == 0)//探索範囲が残っていなければ探索終了
			break;
		if (comp <= 0)//探索値が中心要素より小さいか同じだった場合、次に中心より前の範囲に絞って探索する
			range = range_half;
		else//if (comp > 0)//探索値が中心要素より大きかった場合、次に中心より後の範囲に絞って探索する
		{
			begin = mid + static_cast<typename ITERATOR::difference_type>(1);
			range -= (range_half + 1);
		}
	}
	if (found != end && found != begin)//見つかった地点が先頭でなければ、一つ前を調べる
	{
		ITERATOR found_prev = found;
		--found_prev;
		if (comparison(*found_prev) == 0)//一つ前が一致するならそれを返す
			found = found_prev;
	}
	return found;
}
iteratorSearchFuncSetByComparison(iteratorBinarySearch);

//----------------------------------------
//アルゴリズム：二分探索
//※双方向連結リスト対応版
//----------------------------------------
//・最良計算時間：O(1)
//・平均計算時間：O(log n)
//・最悪計算時間：O(log n)
//・探索失敗時：  O(log n)
//----------------------------------------
//プロトタイプ：
//・bool COMPARISON(const T& value)//value == 探索値なら0を、value < 探索値なら-1以下を、value > 探索値なら1以上を返す
//・T* GET_NEXT_FUNC(T& node)//次のノードを返す
//・T* GET_PREV_FUNC(T& node)//前のノードを返す
template<class T, class GET_NEXT_FUNC, class GET_PREV_FUNC, class COMPARISON>
const T* linkedListBinarySearch(const T* first, GET_NEXT_FUNC get_next_func, GET_PREV_FUNC get_prev_func, COMPARISON comparison)
{
	if (!first)
		return nullptr;
	int size = 0;
	for (const T* node = first; node; ++size, node = get_next_func(*node));
	int range = size;
	const T* found = nullptr;
	while (true)
	{
		const int range_half = range / 2;//探索範囲の半分の範囲
		const T* mid = first;//探索範囲の中心要素
		for (int forward = range_half; --forward >= 0; mid = get_next_func(*mid));
		const int comp = comparison(*mid);//中心要素を探索値と比較
		if (comp == 0)//中心要素が探索値と一致
			found = mid;//発見した場所を記憶 ※見つかった位置の先頭を発見するため、探索を続行する
		if (range_half == 0)//探索範囲が残っていなければ探索終了
			break;
		if (comp <= 0)//探索値が中心要素より小さいか同じだった場合、次に中心より前の範囲に絞って探索する
			range = range_half;
		else//if (comp > 0)//探索値が中心要素より大きかった場合、次に中心より後の範囲に絞って探索する
		{
			first = get_next_func(*mid);
			range -= (range_half + 1);
		}
	}
	if (found && found != first)//見つかった地点が先頭でなければ、一つ前を調べる
	{
		const T* found_prev = get_prev_func(*found);
		if (comparison(*found_prev) == 0)//一つ前が一致するならそれを返す
			found = found_prev;
	}
	return found;
}
linkedListSearchFuncSetByComparison(linkedListBinarySearch);

//----------------------------------------
//アルゴリズム：二分探索
//※片方向連結リスト対応版
//----------------------------------------
//・最良計算時間：O(1)
//・平均計算時間：O(log n)
//・最悪計算時間：O(log n)
//・探索失敗時：  O(log n)
//----------------------------------------
//プロトタイプ：
//・bool COMPARISON(const T& value)//value == 探索値なら0を、value < 探索値なら-1以下を、value > 探索値なら1以上を返す
//・T* GET_NEXT_FUNC(T& node)//次のノードを返す
template<class T, class GET_NEXT_FUNC, class COMPARISON>
const T* singlyLinkedListBinarySearch(const T* first, GET_NEXT_FUNC get_next_func, COMPARISON comparison)
{
	if (!first)
		return nullptr;
	int size = 0;
	for (const T* node = first; node; ++size, node = get_next_func(node));
	int range = size;
	const T* found = nullptr;
	const T* mid_prev = nullptr;
	while (true)
	{
		const int range_half = range / 2;//探索範囲の半分の範囲
		const T* mid = first;//探索範囲の中心要素
		mid_prev = nullptr;
		for (int forward = range_half; --forward >= 0; mid_prev = mid, mid = get_next_func(mid));
		const int comp = comparison(*mid);//中心要素を探索値と比較
		if (comp == 0)//中心要素が探索値と一致
			found = mid;//発見した場所を記憶 ※見つかった位置の先頭を発見するため、探索を続行する
		if (range_half == 0)//探索範囲が残っていなければ探索終了
			break;
		if (comp <= 0)//探索値が中心要素より小さいか同じだった場合、次に中心より前の範囲に絞って探索する
			range = range_half;
		else//if (comp > 0)//探索値が中心要素より大きかった場合、次に中心より後の範囲に絞って探索する
		{
			first = get_next_func(mid);
			range -= (range_half + 1);
		}
	}
	if (found && mid_prev)//見つかった地点が先頭でなければ、一つ前を調べる
	{
		if (comparison(*mid_prev) == 0)//一つ前が一致するならそれを返す
			found = mid_prev;
	}
	return found;
}
singlyLinkedListSearchFuncSetByComparison(singlyLinkedListBinarySearch);

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//_BINARY_SEARCH_H_

// End of file

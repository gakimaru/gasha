#pragma once
#ifndef GASHA_INCLUDED_BINARY_SEARCH_INL
#define GASHA_INCLUDED_BINARY_SEARCH_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// binary_search.inl
// 二分探索【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/binary_search.h>//二分探索【宣言部】

#include <gasha/iterator.h>//イテレータ用アルゴリズム

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//アルゴリズム：二分探索
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

//----------------------------------------
//アルゴリズム：二分探索
//※イテレータ対応版
template<class ITERATOR, class COMPARISON>
ITERATOR iteratorBinarySearch(ITERATOR begin, ITERATOR end, COMPARISON comparison)
{
	if (begin == end)
		return end;
	const typename ITERATOR::difference_type size = GASHA_ iteratorDifference(begin, end);
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
			if (begin == end)
				break;
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

//----------------------------------------
//アルゴリズム：二分探索
//※双方向連結リスト対応版
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
			if (!first)
				break;
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

//----------------------------------------
//アルゴリズム：二分探索
//※片方向連結リスト対応版
template<class T, class GET_NEXT_FUNC, class COMPARISON>
const T* singlyLinkedListBinarySearch(const T* first, GET_NEXT_FUNC get_next_func, COMPARISON comparison)
{
	if (!first)
		return nullptr;
	int size = 0;
	for (const T* node = first; node; ++size, node = get_next_func(*node));
	int range = size;
	const T* found = nullptr;
	const T* mid_prev = nullptr;
	while (true)
	{
		const int range_half = range / 2;//探索範囲の半分の範囲
		const T* mid = first;//探索範囲の中心要素
		mid_prev = nullptr;
		for (int forward = range_half; --forward >= 0; mid_prev = mid, mid = get_next_func(*mid));
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
			if (!first)
				break;
		}
	}
	if (found && mid_prev)//見つかった地点が先頭でなければ、一つ前を調べる
	{
		if (comparison(*mid_prev) == 0)//一つ前が一致するならそれを返す
			found = mid_prev;
	}
	return found;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_BINARY_SEARCH_INL

// End of file

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

#include <gasha/iterator.h>//イテレータ

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

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
		const int comp = comparison(*mid);//中心要素を探索キーと比較
		if (comp == 0)//中心要素が探索キーと一致
			found = mid;//発見した場所を記憶 ※見つかった位置の先頭を発見するため、探索を続行する
		if (range_half == 0)//探索範囲が残っていなければ探索終了
			break;
		if (comp <= 0)//探索キーが中心要素より小さいか同じだった場合、次に中心より前の範囲に絞って探索する
			range = range_half;
		else//if (comp > 0)//探索キーが中心要素より大きかった場合、次に中心より後の範囲に絞って探索する
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
template<class  ITERATOR, class COMPARISON>
ITERATOR iteratorBinarySearch(ITERATOR begin, ITERATOR end, COMPARISON comparison)
{
	if (begin == end)
		return end;
	const int size = iteratorDifference(begin, end);
	int range = size;
	ITERATOR found = end;
	while (true)
	{
		const int range_half = range / 2;//探索範囲の半分の範囲
		ITERATOR mid = begin + range_half;//探索範囲の中心要素
		const int comp = comparison(*mid);//中心要素を探索キーと比較
		if (comp == 0)//中心要素が探索キーと一致
			found = mid;//発見した場所を記憶 ※見つかった位置の先頭を発見するため、探索を続行する
		if (range_half == 0)//探索範囲が残っていなければ探索終了
			break;
		if (comp <= 0)//探索キーが中心要素より小さいか同じだった場合、次に中心より前の範囲に絞って探索する
			range = range_half;
		else//if (comp > 0)//探索キーが中心要素より大きかった場合、次に中心より後の範囲に絞って探索する
		{
			begin = mid + 1;
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

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//_BINARY_SEARCH_H_

// End of file

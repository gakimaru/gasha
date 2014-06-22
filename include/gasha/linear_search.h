#pragma once
#ifndef __LINERAR_SEARCH_H_
#define __LINERAR_SEARCH_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// linear_search.h
// 線形探索
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/build_settings.h>//ビルド設定

#include <gasha/search_basic.h>//探索処理基本

//#include <gasha/iterator.h>//イテレータ

NAMESPACE_GASHA_BEGIN//ネームスペース：開始

//========================================
//探索アルゴリズムの説明
//========================================

//・計算時間：
//    - O(n)       ... データ件数分の時間
//    - O(n ^ 2)   ... データ件数の２乗分の時間
//    - O(log n)   ... log2(データ件数)分の時間（4→2, 16→4, 1024→10,1048576→20）
//    - O(n log n) ... n×log n 分の時間

//----------------------------------------
//アルゴリズム：線形探索
//----------------------------------------
//・最良計算時間：O(1)
//・平均計算時間：O(n/2)
//・最悪計算時間：O(n)
//・探索失敗時：  O(n)
//----------------------------------------
template<class T, class PREDICATE>
T* linearSearch(T* array, const std::size_t size, PREDICATE predicate)
{
	if (!array || size == 0)
		return nullptr;
	T* now = array;
	for (std::size_t i = 0; i < size; ++i, ++now)//順次探索
	{
		if (predicate(*now))//探索キーと一致したら終了
			return now;
	}
	return nullptr;//探索失敗
}
searchFuncSetByPredicate(linearSearch);

//----------------------------------------
//アルゴリズム：線形探索
//※イテレータ対応版
//----------------------------------------
//・最良計算時間：O(1)
//・平均計算時間：O(n/2)
//・最悪計算時間：O(n)
//・探索失敗時：  O(n)
//----------------------------------------
template<class  ITERATOR, class PREDICATE>
ITERATOR iteratorLinearSearch(ITERATOR begin, ITERATOR end, PREDICATE predicate)
{
	if (begin == end)
		return end;
	ITERATOR now = begin;
	for (; now != end; ++now)//順次探索
	{
		if (predicate(*now))//探索キーと一致したら終了
			return now;
	}
	return end;//探索失敗
}
iteratorSearchFuncSetByPredicate(iteratorLinearSearch);

NAMESPACE_GASHA_END//ネームスペース：終了

#endif//__LINERAR_SEARCH_H_

// End of file

#pragma once
#ifndef __ODD_EVENT_SORT_H_
#define __ODD_EVENT_SORT_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// odd_even_sort.h
// 奇遇転置ソート
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/sort_basic.h>//ソート処理基本
#include <gasha/utility.h>//汎用ユーティリティ（値交換用）

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
//ソートアルゴリズム分類：交換ソート
//========================================

//----------------------------------------
//アルゴリズム：奇遇転置ソート
//----------------------------------------
//・最良計算時間：O(2n)
//・平均計算時間：O(n^2)
//・最悪計算時間：O(n^2)
//・メモリ使用量：O(1)×並列処理分
//・安定性：　　　○
//----------------------------------------
//※OpenMPを使用し、並列化で最適化する。
//----------------------------------------
template<class T, class PREDICATE>
std::size_t oddEvenSort(T* array, const std::size_t size, PREDICATE predicate)
{
	if (!array || size <= 1)
		return 0;
	std::size_t swapped_count = 0;
	std::size_t end = size - 1;
	bool is_swapped = true;
	while (is_swapped)
	{
		is_swapped = false;
		for (std::size_t odd_even = 0; odd_even < 2; ++odd_even)
		{
			int i;//本来は std::size_t
			T* now;
			T* next;
	#ifdef GASHA_ODD_EVEN_SORT_USE_OPENMP
		#pragma omp parallel for reduction(+:swapped_count) reduction(||:is_swapped) private(now, next)
	#endif//GASHA_ODD_EVEN_SORT_USE_OPENMP
			for (i = static_cast<int>(odd_even); i < static_cast<int>(end); i += 2)
			{
				now = array + i;
				next = now + 1;
				if (predicate(*next, *now))
				{
					swapValues(*next, *now);
					is_swapped = true;
					++swapped_count;
				}
			}
		}
	}
	return swapped_count;
}
sortingFuncSet(oddEvenSort);

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__ODD_EVENT_SORT_H_

// End of file

#pragma once
#ifndef __COMB_SORT_H_
#define __COMB_SORT_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// comb_sort.h
// コムソート
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/sort_basic.h>//ソート処理基本
#include <gasha/swap_values.h>//値交換

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

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
//アルゴリズム：コムソート
//----------------------------------------
//・最良計算時間：O(n)
//・平均計算時間：O(n log n)
//・最悪計算時間：O(n^2)
//・メモリ使用量：O(1)
//・安定性：　　　×
//----------------------------------------
template<class T, class PREDICATE>
std::size_t combSort(T* array, const std::size_t size, PREDICATE predicate)
{
	if (!array || size <= 1)
		return 0;
	std::size_t swapped_count = 0;
	std::size_t h = size;
	const T* end = array + size;
	while (true)
	{
		h = h == 1 ? 1 : h * 10 / 13;
		bool is_swapped = false;
		T* now = array;
		T* next = now + h;
		while (next < end)
		{
			if (predicate(*next, *now))
			{
				swapValues(*next, *now);
				is_swapped = true;
				++swapped_count;
			}
			++now;
			++next;
		}
		if (h == 1 && !is_swapped)
			break;
	}
	return swapped_count;
}
sortFuncSet(combSort);

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__COMB_SORT_H_

// End of file

#pragma once
#ifndef GASHA_INCLUDED_ODD_EVENT_SORT_INL
#define GASHA_INCLUDED_ODD_EVENT_SORT_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// odd_even_sort.inl
// 奇遇転置ソート【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/odd_even_sort.h>//奇遇転置ソート【宣言部】

#include <gasha/utility.h>//汎用ユーティリティ（値交換用）

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//ソートアルゴリズム分類：交換ソート
//========================================

//----------------------------------------
//アルゴリズム：奇遇転置ソート
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
					GASHA_ swapValues(*next, *now);
					is_swapped = true;
					++swapped_count;
				}
			}
		}
	}
	return swapped_count;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ODD_EVENT_SORT_INL

// End of file

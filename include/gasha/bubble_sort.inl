#pragma once
#ifndef GASHA_INCLUDED_BUBBLE_SORT_INL
#define GASHA_INCLUDED_BUBBLE_SORT_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// bubble_sort.inl
// バブルソート【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/bubble_sort.h>//バブルソート【宣言部】

#include <gasha/utility.h>//汎用ユーティリティ（値交換用）

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//ソートアルゴリズム分類：交換ソート
//========================================

//----------------------------------------
//アルゴリズム：バブルソート
template<class T, class PREDICATE>
std::size_t bubbleSort(T* array, const std::size_t size, PREDICATE predicate)
{
	if (!array || size <= 1)
		return 0;
	std::size_t swapped_count = 0;
	std::size_t end = size - 1;
	//for (std::size_t i = 0; i < end - 1; ++i)
	while (true)
	{
		bool is_swapped = false;
		T* now = array;
		T* next = now + 1;
		for (std::size_t ii = 0; ii < end; ++ii, ++now, ++next)
		{
			if (predicate(*next, *now))
			{
				GASHA_ swapValues(*next, *now);
				is_swapped = true;
				++swapped_count;
			}
		}
		--end;
		if (end == 0 || !is_swapped)
			break;
	}
	return swapped_count;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_BUBBLE_SORT_INL

// End of file

#pragma once
#ifndef GASHA_INCLUDED_SHAKER_SORT_INL
#define GASHA_INCLUDED_SHAKER_SORT_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shaker_sort.inl
// シェーカーソート【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shaker_sort.h>//シェーカーソート【宣言部】

#include <gasha/utility.h>//汎用ユーティリティ（値交換用）

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//ソートアルゴリズム分類：交換ソート
//========================================

//----------------------------------------
//アルゴリズム：シェーカーソート
template<class T, class PREDICATE>
std::size_t shakerSort(T* array, const std::size_t size, PREDICATE predicate)
{
	if (!array || size <= 1)
		return 0;
	std::size_t swapped_count = 0;
	std::size_t begin = 0;
	std::size_t end = size - 1;
	while(true)
	{
		bool is_swapped = false;
		T* now = array + begin;
		T* next = now + 1;
		for (std::size_t i = begin; i < end; ++i, ++now, ++next)
		{
			if (predicate(*next, *now))
			{
				GASHA_ swapValues(*next, *now);
				is_swapped = true;
				++swapped_count;
			}
		}
		--end;
		if (begin == end || !is_swapped)
			break;
		is_swapped = false;
		now = array + end;
		T* prev = now - 1;
		for (std::size_t i = end; i > begin; --i, --now, --prev)
		{
			if (predicate(*now, *prev))
			{
				GASHA_ swapValues(*now, *prev);
				is_swapped = true;
				++swapped_count;
			}
		}
		++begin;
		if (begin == end || !is_swapped)
			break;
	}
	return swapped_count;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SHAKER_SORT_INL

// End of file

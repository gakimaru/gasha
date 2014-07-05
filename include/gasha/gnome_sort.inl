#pragma once
#ifndef GASHA_INCLUDED_GNOME_SORT_INL
#define GASHA_INCLUDED_GNOME_SORT_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// gnome_sort.inl
// ノームソート【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/gnome_sort.h>//ノームソート【宣言部】

#include <gasha/utility.h>//汎用ユーティリティ（値交換用）

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始


//========================================
//ソートアルゴリズム分類：交換ソート
//========================================

//----------------------------------------
//アルゴリズム：ノームソート
template<class T, class PREDICATE>
std::size_t gnomeSort(T* array, const std::size_t size, PREDICATE predicate)
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
			T* prev = now;
			now = next;
			while (prev >= array)
			{
				if (predicate(*now, *prev))
				{
					GASHA_ swapValues(*now, *prev);
					++swapped_count;
				}
				else
					break;
				--prev;
				--now;
			}
		}
		now = next;
		++next;
	}
	return swapped_count;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_GNOME_SORT_INL

// End of file

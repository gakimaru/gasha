#pragma once
#ifndef GASHA_INCLUDED_SELECTION_SORT_INL
#define GASHA_INCLUDED_SELECTION_SORT_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// selection_sort.inl
// 選択ソート【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/selection_sort.h>//選択ソート【宣言部】

#include <gasha/utility.h>//汎用ユーティリティ（値交換用）

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//ソートアルゴリズム分類：選択ソート
//========================================

//----------------------------------------
//アルゴリズム：選択ソート
template<class T, class PREDICATE>
std::size_t selectionSort(T* array, const std::size_t size, PREDICATE predicate)
{
	if (!array || size <= 1)
		return 0;
	std::size_t swapped_count = 0;
	T* now = array;
	const std::size_t size_1 = size - 1;
	for (std::size_t i = 0; i < size_1; ++i, ++now)
	{
		T* min = now;
		T* check = now;
		for (std::size_t ii = i + 1; ii < size; ++ii)
		{
			++check;
			if (predicate(*check, *min))
				min = check;
		}
		if (now != min)
		{
			GASHA_ swapValues(*min, *now);
			++swapped_count;
		}
	}
	return swapped_count;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SELECTION_SORT_INL

// End of file

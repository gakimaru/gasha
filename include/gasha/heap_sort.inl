#pragma once
#ifndef GASHA_INCLUDED_HEAP_SORT_INL
#define GASHA_INCLUDED_HEAP_SORT_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// heap_sort.inl
// ヒープソート【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/heap_sort.h>//ヒープソート【宣言部】

#include <gasha/utility.h>//汎用ユーティリティ（値交換用）

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//ソートアルゴリズム分類：選択ソート
//========================================

//----------------------------------------
//アルゴリズム：ヒープソート
template<class T, class PREDICATE>
std::size_t heapSort(T* array, const std::size_t size, PREDICATE predicate)
{
	if (!array || size <= 1)
		return 0;
	std::size_t swapped_count = 0;
	//二分ヒープ登録
	for (std::size_t heap_size = 1; heap_size < size; ++heap_size)
	{
		T* now = array + heap_size;
		//アップヒープ
		std::size_t parent_i = heap_size;
		while (parent_i > 0)
		{
			parent_i = (parent_i - 1) >> 1;
			T* parent = array + parent_i;
			if (predicate(*parent, *now))
			{
				GASHA_ swapValues(*parent, *now);
				++swapped_count;
			}
			else
				break;
			now = parent;
		}
	}
	//二分ヒープ取り出し
	for (std::size_t heap_size = size - 1; heap_size > 0; --heap_size)
	{
		T* now = array + heap_size;
		//ダウンヒープ
		{
			GASHA_ swapValues(*array, *now);
			++swapped_count;
		}
		now = array;
		std::size_t child_i = 0;
		while (true)
		{
			child_i = (child_i << 1) + 1;
			if (child_i >= heap_size)
				break;
			T* child = array + child_i;
			T* child_r = child_i + 1 < heap_size ? child + 1 : nullptr;
			if (child_r && predicate(*child, *child_r))
			{
				child = child_r;
				++child_i;
			}
			if(predicate(*now, *child))
			{
				GASHA_ swapValues(*now, *child);
				++swapped_count;
			}
			else
				break;
			now = child;
		}
	}
	return swapped_count;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_HEAP_SORT_INL

// End of file

#pragma once
#ifndef GASHA_INCLUDED_INPLACE_MERGE_SORT_INL
#define GASHA_INCLUDED_INPLACE_MERGE_SORT_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// inplace_merge_sort.inl
// インプレースマージソート【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/inplace_merge_sort.h>//インプレースマージソート【宣言部】

#include <gasha/utility.h>//汎用ユーティリティ（値交換用）

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//ソートアルゴリズム分類：マージソート
//========================================

//----------------------------------------
//アルゴリズム：インプレースマージソート
template<class T, class PREDICATE>
std::size_t inplaceMergeSort(T* array, const std::size_t size, PREDICATE predicate)
{
#ifndef GASHA_INPLACE_MERGE_SORT_USE_OPENMP
	//通常版
	if (!array || size <= 1)
		return 0;
	std::size_t swapped_count = 0;
	for (std::size_t block_size = 1; block_size < size; block_size <<= 1)
	{
		const std::size_t merge_size = block_size << 1;
		const std::size_t left_block_size = block_size;
		for (std::size_t merge_pos = 0; merge_pos + block_size <= size; merge_pos += merge_size)
		{
			const std::size_t right_block_size = merge_pos + merge_size <= size ? block_size : size % block_size;
			T* left_begin = array + merge_pos;
			T* left_end = left_begin + left_block_size;
			T* right_begin = left_end;
			T* right_end = right_begin + right_block_size;
			T* left = left_end - 1;
			T* right = right_begin;
			T* left_ins_prev = nullptr;
			while (right < right_end)
			{
				if (predicate(*right, *left))//左ブロックの右端と右ブロックの左端をチェック
				{
				#if 0
					//挿入位置検索 ※線形検索
					T* left_ins = left;
					while (left_ins >= left_begin && predicate(*right, *left_ins))
						--left_ins;
					++left_ins;
				#else
					//挿入位置検索 ※二分検索
					T* left_ins;
					{
						T* search_begin = left_ins_prev ? left_ins_prev + 1 : left_begin;
						std::size_t search_range_half = (right - search_begin) >> 1;
						std::size_t search_pos = search_range_half;
						left_ins = search_begin + search_pos;
						while (search_range_half > 1)
						{
							search_range_half = search_range_half >> 1;
							left_ins = search_begin + search_pos;
							if (predicate(*right, *left_ins))
								search_pos -= search_range_half;
							else
								search_pos += search_range_half;
						}
						while (left_ins < right && !predicate(*right, *left_ins))
							++left_ins;
						while (left_ins >= search_begin && predicate(*right, *left_ins))
							--left_ins;
						++left_ins;
						left_ins_prev = left_ins;
					}
				#endif
					//挿入
					GASHA_ rotateValues(left_ins, right, 1);
					++swapped_count;
					++left;
					++right;
				}
				else
					break;
			}
		}
	}
	return swapped_count;
#else//GASHA_INPLACE_MERGE_SORT_USE_OPENMP
	//OpenMP版
	if (!array || size <= 1)
		return 0;
	std::size_t swapped_count = 0;
	std::size_t merge_pos;
	std::size_t right_block_size;
	T* left_begin;
	T* left_end;
	T* left;
	T* left_ins_prev;
	T* left_ins;
	T* right_begin;
	T* right_end;
	T* right;
	T* search_begin;
	std::size_t search_range_half;
	std::size_t search_pos;
	for (std::size_t block_size = 1; block_size < size; block_size <<= 1)
	{
		const std::size_t merge_size = block_size << 1;
		const std::size_t left_block_size = block_size;
		const int loop_count = static_cast<int>((size / merge_size) + (size % merge_size > block_size));
	#pragma omp parallel for reduction(+:swapped_count) private(merge_pos, right_block_size, left_begin, left_end, left, left_ins_prev, left_ins, right_begin, right_end, right, search_begin, search_range_half, search_pos)
		for (int i = 0; i< loop_count; ++i)
		{
			merge_pos = i * merge_size;
			right_block_size = merge_pos + merge_size <= size ? block_size : size % block_size;
			left_begin = array + merge_pos;
			left_end = left_begin + left_block_size;
			right_begin = left_end;
			right_end = right_begin + right_block_size;
			left = left_end - 1;
			right = right_begin;
			left_ins_prev = nullptr;
			while (right < right_end)
			{
				if (predicate(*right, *left))//左ブロックの右端と右ブロックの左端をチェック
				{
				#if 0
					//挿入位置検索 ※線形検索
					left_ins = left;
					while (left_ins >= left_begin && predicate(*right, *left_ins))
						--left_ins;
					++left_ins;
				#else
					//挿入位置検索 ※二分検索
					{
						search_begin = left_ins_prev ? left_ins_prev + 1 : left_begin;
						search_range_half = (right - search_begin) >> 1;
						search_pos = search_range_half;
						left_ins = search_begin + search_pos;
						while (search_range_half > 1)
						{
							search_range_half = search_range_half >> 1;
							left_ins = search_begin + search_pos;
							if (predicate(*right, *left_ins))
								search_pos -= search_range_half;
							else
								search_pos += search_range_half;
						}
						while (left_ins < right && !predicate(*right, *left_ins))
							++left_ins;
						while (left_ins >= search_begin && predicate(*right, *left_ins))
							--left_ins;
						++left_ins;
						left_ins_prev = left_ins;
					}
				#endif
					//挿入
					GASHA_ rotateValues(left_ins, right, 1);
					++swapped_count;
					++left;
					++right;
				}
				else
					break;
			}
		}
	}
	return swapped_count;
#endif//GASHA_INPLACE_MERGE_SORT_USE_OPENMP
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_INPLACE_MERGE_SORT_INL

// End of file

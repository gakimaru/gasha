#pragma once
#ifndef GASHA_INCLUDED_SHEAR_SORT_INL
#define GASHA_INCLUDED_SHEAR_SORT_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shear_sort.inl
// シェアソート【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shear_sort.h>//シェアソート【宣言部】

#include <gasha/utility.h>//汎用ユーティリティ（値交換用）
#include <gasha/fast_math.h>//高速算術

#ifdef GASHA_SHEAR_SORT_USE_OPENMP_NEST
#include <omp.h>//omp_set_nested()
#endif//GASHA_SHEAR_SORT_USE_OPENMP_NEST

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//ソートアルゴリズム分類：交換ソート
//========================================

//----------------------------------------
//アルゴリズム：シェアソート
template<class T, class PREDICATE>
std::size_t shearSort(T* array, const std::size_t size, PREDICATE predicate)
{
	if (!array || size <= 1)
		return 0;
#ifdef GASHA_SHEAR_SORT_USE_OPENMP_NEST
	const int omp_nested_before = omp_get_nested();
	omp_set_nested(1);//並列化のネストを許可
#endif//GASHA_SHEAR_SORT_USE_OPENMP_NEST
	std::size_t swapped_count = 0;
	int row;//本来は std::size_t
	int col;//本来は std::size_t
	std::size_t rows_col;
	std::size_t cols_row;
	std::size_t rows_1_col;
	std::size_t cols_1_row;
	std::size_t col_odd_even;
	std::size_t row_odd_even;
	bool is_swapped;
	T* now;
	T* next;
	bool is_odd;
	std::size_t rows = static_cast<std::size_t>(GASHA_ sqr(fastestA_f(static_cast<float>(size))));//正方形のデータとして扱うための行数算出
	std::size_t over = size % rows;//余り算出
	if (over > 0 && (rows & 0x1) == 0x1)//余りが出た場合、（余りを除く）行数が偶数になるように調整
	{
		--rows;
		over = size % rows;
	}
	const std::size_t cols = size / rows;//1行当たりの列数算出
	auto log2 = [](std::size_t val) -> std::size_t
	{
		std::size_t result = 0;
		while (val >>= 1)
			++result;
		return result;
	};
	const int sorting_count = static_cast<int>(log2(rows) + 1);//ソート実行回数を算出
	for (int i = 0; i < sorting_count; ++i)
	{
		//各行ごとに、列方向にソート
		//※偶数行は小さい順、奇数行は大きい順にソート
#if defined(GASHA_SHEAR_SORT_USE_OPENMP_NEST)
	#pragma omp parallel for reduction(+:swapped_count) private(cols_row, cols_1_row, is_swapped, col_odd_even, is_odd)
#elif defined(GASHA_SHEAR_SORT_USE_OPENMP)
	#pragma omp parallel for reduction(+:swapped_count) private(cols_row, cols_1_row, is_swapped, col_odd_even, is_odd, col, now, next)
#endif//GASHA_SHEAR_SORT_USE_OPENMP
		for (row = 0; row <= static_cast<int>(rows); ++row)
		{
			cols_row = row == static_cast<int>(rows) ? over : cols;
			if (cols_row > 1)
			{
				is_odd = ((row & 0x1) == 0x1);
				//奇遇転置ソート
				cols_1_row = cols_row - 1;
				is_swapped = true;
				while (is_swapped)
				{
					is_swapped = false;
					for (col_odd_even = 0; col_odd_even < 2; ++col_odd_even)
					{
				#ifdef GASHA_SHEAR_SORT_USE_OPENMP_NEST
					#pragma omp parallel for reduction(+:swapped_count) reduction(||:is_swapped) private(now, next)
				#endif//GASHA_SHEAR_SORT_USE_OPENMP_NEST
						for (col = static_cast<int>(col_odd_even); col < static_cast<int>(cols_1_row); col += 2)
						{
							now = array + row * cols + col;
							next = now + 1;
							if ((!is_odd && predicate(*next, *now)) ||//偶数行は小さい順
								( is_odd && predicate(*now, *next)))  //奇数行は大きい順
							{
								GASHA_ swapValues(*next, *now);
								is_swapped = true;
								++swapped_count;
							}
						}
					}
				}
			}
		}
		//各列ごとに、行方向にソート
		//※小さい順にソート
#if defined(GASHA_SHEAR_SORT_USE_OPENMP_NEST)
	#pragma omp parallel for reduction(+:swapped_count) private(rows_col, rows_1_col, row_odd_even, is_swapped)
#elif defined(GASHA_SHEAR_SORT_USE_OPENMP)
	#pragma omp parallel for reduction(+:swapped_count) private(rows_col, rows_1_col, row_odd_even, is_swapped, row, now, next)
#endif//GASHA_SHEAR_SORT_USE_OPENMP
		for (col = 0; col < static_cast<int>(cols); ++col)
		{
			rows_col = rows + (col < static_cast<int>(over) ? 1 : 0);
			//奇遇転置ソート
			rows_1_col = rows_col - 1;
			is_swapped = true;
			while (is_swapped)
			{
				is_swapped = false;
				for (row_odd_even = 0; row_odd_even < 2; ++row_odd_even)
				{
			#ifdef GASHA_SHEAR_SORT_USE_OPENMP_NEST
				#pragma omp parallel for reduction(+:swapped_count) reduction(||:is_swapped) private(now, next)
			#endif//GASHA_SHEAR_SORT_USE_OPENMP_NEST
					for (row = static_cast<int>(row_odd_even); row < static_cast<int>(rows_1_col); row += 2)
					{
						now = array + row * cols + col;
						next = now + cols;
						if (predicate(*next, *now))
						{
							GASHA_ swapValues(*next, *now);
							is_swapped = true;
							++swapped_count;
						}
					}
				}
			}
		}
	}
	{
		//各行ごとに、列方向にソート
		//※小さい順にソート
#if defined(GASHA_SHEAR_SORT_USE_OPENMP_NEST)
	#pragma omp parallel for reduction(+:swapped_count) private(cols_row, cols_1_row, col_odd_even, is_swapped)
#elif defined(GASHA_SHEAR_SORT_USE_OPENMP)
	#pragma omp parallel for reduction(+:swapped_count) private(cols_row, cols_1_row, col_odd_even, is_swapped, col, now, next)
#endif//GASHA_SHEAR_SORT_USE_OPENMP
		for (row = 0; row <= static_cast<int>(rows); ++row)
		{
			cols_row = row == static_cast<int>(rows) ? over : cols;
			if (cols_row > 1)
			{
				//奇遇転置ソート
				cols_1_row = cols_row - 1;
				is_swapped = true;
				while (is_swapped)
				{
					is_swapped = false;
					for (col_odd_even = 0; col_odd_even < 2; ++col_odd_even)
					{
					#ifdef GASHA_SHEAR_SORT_USE_OPENMP_NEST
						#pragma omp parallel for reduction(+:swapped_count) reduction(||:is_swapped) private(now, next)
					#endif//GASHA_SHEAR_SORT_USE_OPENMP_NEST
						for (col = static_cast<int>(col_odd_even); col < static_cast<int>(cols_1_row); col += 2)
						{
							now = array + row * cols + col;
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
			}
		}
	}
#ifdef GASHA_SHEAR_SORT_USE_OPENMP_NEST
	omp_set_nested(omp_nested_before);
#endif//GASHA_SHEAR_SORT_USE_OPENMP_NEST
	return swapped_count;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SHEAR_SORT_INL

// End of file

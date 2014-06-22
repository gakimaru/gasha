#pragma once
#ifndef __QUICK_SORT_H_
#define __QUICK_SORT_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// quick_sort.h
// クイックソート
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/build_settings.h>//ビルド設定

#include <gasha/sort_basic.h>//ソート処理基本
#include <gasha/swap_values.h>//値交換

#include <assert.h>//assert用

NAMESPACE_GASHA_BEGIN//ネームスペース：開始

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
//ソートアルゴリズム分類：分割交換ソート
//========================================

//----------------------------------------
//アルゴリズム：クイックソート
//----------------------------------------
//・最良計算時間：O(n log n)
//・平均計算時間：O(n log n)
//・最悪計算時間：O(n^2)
//・メモリ使用量：O(log n)～O(n) ※ループ処理版は O(32*2)
//・安定性：　　　×
//----------------------------------------
//※再帰処理を使用せず、スタックを使用したループ処理にして最適化する。
//　（最大件数を log2(4294967296) = 32 とする）
//----------------------------------------
template<class T, class PREDICATE>
std::size_t _quickSort(T* array, const std::size_t size, PREDICATE predicate)
{
#ifndef QUICK_SORT_NO_USE_RECURSIVE_CALL
	//--------------------
	//再帰処理版
	if (size <= 1)
		return 0;
	std::size_t swapped_count = 0;
	//配列の範囲情報
	const T* term = array + size;
	T* begin = array;
	T* end = array + size - 1;
	//軸を決定
	const T* mid = array + (size >> 1);
	const T* pivot =
		predicate(*begin, *mid) ?
			predicate(*mid, *end) ?
				mid :
				predicate(*end, *begin) ?
					begin :
					end :
			predicate(*end, *mid) ?
				mid :
				predicate(*begin, *end) ?
					begin :
					end;
	//軸未満の配列と軸以上の配列に二分
	while (true)
	{
		while (predicate(*begin, *pivot))
			++begin;
		while (predicate(*pivot, *end))
			--end;
		if (begin >= end)
			break;
		swapValues(*begin, *end);
		pivot = pivot == begin ? end : pivot == end ? begin : pivot;//軸の位置調整（軸の位置も入れ替わるため）
		++swapped_count;
		++begin;
		--end;
	}
	//再帰処理
	swapped_count += _quickSort(array, begin - array, predicate);//軸未満の配列
	swapped_count += _quickSort(end + 1, term - end - 1, predicate);//軸以上の配列
	return swapped_count;
#else//QUICK_SORT_NO_USE_RECURSIVE_CALL
#ifndef QUICK_SORT_USE_OPENMP
	//--------------------
	//スタック処理版
	std::size_t swapped_count = 0;
	struct stack_t
	{
		T* array;
		std::size_t size;
	};
	static const int STACK_DEPTH_MAX = 32 * 2;
	stack_t stack[STACK_DEPTH_MAX];
	//最初の配列をスタックにプッシュ
	stack_t* stack_p = &stack[0];
	stack_p->array = array;
	stack_p->size = size;
	int stack_curr = 1;//スタック位置
	while (stack_curr > 0)
	{
		//配列をスタックから取得
		stack_p = &stack[--stack_curr];
		T* _array = stack_p->array;
		const size_t _size = stack_p->size;
		//配列の範囲情報
		const T* term = _array + _size;
		T* begin = _array;
		T* end = _array + _size - 1;
		//軸を決定
		const T* mid = _array + (_size >> 1);
		const T* pivot =
			predicate(*begin, *mid) ?
				predicate(*mid, *end) ?
					mid :
					predicate(*end, *begin) ?
						begin :
						end :
				predicate(*end, *mid) ?
					mid :
					predicate(*begin, *end) ?
						begin :
						end;
		//軸未満の配列と軸以上の配列に二分
		while (true)
		{
			while (predicate(*begin, *pivot))
				++begin;
			while (predicate(*pivot, *end))
				--end;
			if (begin >= end)
				break;
			swapValues(*begin, *end);
			pivot = pivot == begin ? end : pivot == end ? begin : pivot;//軸の位置調整（軸の位置も入れ替わるため）
			++swapped_count;
			++begin;
			--end;
		}
		//recursive = 0 : 軸未満の配列をプッシュ
		//            1 : 軸以上の配列をプッシュ
		for (int recursive = 0; recursive < 2; ++recursive)
		{
			T* new_array = recursive == 0 ? _array : end + 1;
			const std::size_t new_size = recursive == 0 ? begin - _array : term - end - 1;
			if (new_size >= 1)
			{
				stack_p = &stack[stack_curr++];
				assert(stack_curr <= STACK_DEPTH_MAX);
				stack_p->array = new_array;
				stack_p->size = new_size;
			}
		}
	}
	return swapped_count;
#else//QUICK_SORT_USE_OPENMP
	//--------------------
	//OpenMP＆キュー処理版
	//※並列化のためにスタックではなくキューを使用。
	//　そのため、全件分のキュー（メモリ）が必要。
	//　パフォーマンスも今ひとつ。
	std::size_t swapped_count = 0;
	struct queue_t
	{
		T* array;
		std::size_t size;
	};
	queue_t* queue = new queue_t[size];
	//最初の配列をキューにプッシュ
	queue_t* queue_p = &queue[0];
	queue_p->array = array;
	queue_p->size = size;
	unsigned int queue_write = 1;//キュー書き込み位置
	unsigned int queue_read = 0;//キュー読み込み位置
	unsigned int queue_read_tmp;
	unsigned int queue_write_tmp;
	T* _array;
	size_t _size;
	const T* term;
	T* begin;
	T* end;
	const T* mid;
	const T* pivot;
	int recursive;
	T* new_array;
	std::size_t new_size;
	while (queue_read != queue_write)
	{
		queue_read_tmp = queue_read;
		queue_write_tmp = queue_write;
		const int loop_count = queue_write_tmp > queue_read_tmp ? queue_write_tmp - queue_read_tmp : size - queue_write_tmp + queue_read_tmp;
	#pragma omp parallel for reduction(+:swapped_count) private(queue_p, _array, _size, term, begin, end, mid, pivot, recursive, new_array, new_size, queue_read_tmp, queue_write_tmp)
		for (int loop = 0; loop < loop_count; ++loop)
		{
		#pragma omp critical
			{ queue_read_tmp = queue_read++; }
			//if (true)
			{
				//配列をキューから取得
				queue_p = &queue[queue_read_tmp % size];
				_array = queue_p->array;
				_size = queue_p->size;
				//配列の範囲情報
				term = _array + _size;
				begin = _array;
				end = _array + _size - 1;
				//軸を決定
				mid = _array + (_size >> 1);
				pivot =
					predicate(*begin, *mid) ?
						predicate(*mid, *end) ?
							mid :
							predicate(*end, *begin) ?
								begin :
								end :
						predicate(*end, *mid) ?
							mid :
							predicate(*begin, *end) ?
								begin :
								end;
				//軸未満の配列と軸以上の配列に二分
				while (true)
				{
					while (predicate(*begin, *pivot))
						++begin;
					while (predicate(*pivot, *end))
						--end;
					if (begin >= end)
						break;
					swapValues(*begin, *end);
					pivot = pivot == begin ? end : pivot == end ? begin : pivot;//軸の位置調整（軸の位置も入れ替わるため）
					++swapped_count;
					++begin;
					--end;
				}
				//recursive = 0 : 軸未満の配列をプッシュ
				//            1 : 軸以上の配列をプッシュ
				for (recursive = 0; recursive < 2; ++recursive)
				{
					new_array = recursive == 0 ? _array : end + 1;
					new_size = recursive == 0 ? begin - _array : term - end - 1;
					if (new_size >= 1)
					{
					#pragma omp critical
						{ queue_write_tmp = queue_write++; }
						assert(queue_write_tmp >= queue_read ? queue_write_tmp - queue_read < size : size - queue_write_tmp + queue_read < size);
						queue_p = &queue[queue_write_tmp % size];
						queue_p->array = new_array;
						queue_p->size = new_size;
					}
				}
			}
		}
	}
	delete[] queue;
	return swapped_count;
#endif//QUICK_SORT_USE_OPENMP
#endif//QUICK_SORT_NO_USE_RECURSIVE_CALL
}
template<class T, class PREDICATE>
inline std::size_t quickSort(T* array, const std::size_t size, PREDICATE predicate)
{
	if (!array || size <= 1)
		return 0;
	return _quickSort(array, size, predicate);
}
sortFuncSet(quickSort);

NAMESPACE_GASHA_END//ネームスペース：終了

#endif//__QUICK_SORT_H_

// End of file

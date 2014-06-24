#pragma once
#ifndef __INTRO_SORT_H_
#define __INTRO_SORT_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// intro_sort.h
// イントロソート
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/sort_basic.h>//ソート処理基本
#include <gasha/utility.h>//汎用ユーティリティ（値交換用）

#include <gasha/insertion_sort.h>//挿入ソート
#include <gasha/shell_sort.h>//シェルソート
#include <gasha/heap_sort.h>//ヒープソート

#include <gasha/iterator.h>//イテレータ用アルゴリズム

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

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
//ソートアルゴリズム分類：混成ソート
//========================================

//----------------------------------------
//アルゴリズム：イントロソート
//----------------------------------------
//・最良計算時間：O(n log n)
//・平均計算時間：O(n log n)
//・最悪計算時間：O(n log n)
//・メモリ使用量：O(n log n) ※ループ処理版は O(32*2)
//・安定性：　　　×
//----------------------------------------
//※クイックソートの再帰レベルが log n に達したら、
//　ヒープソートに切り替える。
//　また、再起の末、ソートの対象件数が一定数（32など）未満に
//　なったら、挿入ソートに切り替える。
//※STLのstd::sort()と同様の手法。
//----------------------------------------
//※再帰処理を使用せず、ループ処理にして最適化する。
//　（最大件数を log2(4294967296) = 32 とする）
//※挿入ソート切り替えタイミングを16に設定する。
//　（VC++2013のSTL std::sortでは32）
//※（再帰の末）対象件数が一定以下になったときに切り替えるアルゴリズムは、
//　本来の挿入ソートではなく、シェルソートを使用するスタイルに改良。
//※再帰が一定以上深くなったときに切り替えるアルゴリズムは、
//　本来のヒープソートではなく、シェルソートを使用するスタイルに改良。
//※整列済み判定を最初に一度行うことで最適化する。
//----------------------------------------
template<class T, class PREDICATE>
std::size_t _introSort(T* array, const std::size_t size, PREDICATE predicate)
{
	int depth_max = 0;//ヒープソートに切り替える再帰（スタック）の深さ
	for (std::size_t size_tmp = size; size_tmp > 1; size_tmp >>= 1, ++depth_max);
	//--------------------
	//クイックソート：スタック処理版
	//※再帰処理版は省略
	static const std::size_t SIZE_THRESHOLD = 16;//32;//挿入ソートに切り替える件数
	std::size_t swapped_count = 0;
	struct stack_t
	{
		T* array;
		std::size_t size;
		int depth;
	};
	static const int STACK_DEPTH_MAX = 32 * 2;
	stack_t stack[STACK_DEPTH_MAX];
	//最初の配列をスタックにプッシュ
	stack_t* stack_p = &stack[0];
	stack_p->array = array;
	stack_p->size = size;
	stack_p->depth = depth_max;
	int stack_curr = 1;//スタック位置
	while (stack_curr > 0)
	{
		//配列をスタックから取得
		stack_p = &stack[--stack_curr];
		T* _array = stack_p->array;
		const size_t _size = stack_p->size;
		const int _depth = stack_p->depth;
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
			const int new_depth = _depth - 1;
			if (new_size >= 1)
			{
				if (new_size < SIZE_THRESHOLD)
				{
					//swapped_count += insertionSort(new_array, new_size, predicate);//【本来の処理】挿入ソートに切り替え
					swapped_count += shellSort(new_array, new_size, predicate);//【改良】シェルソートに切り替え
				}
				else if (new_depth == 0)
				{
					//swapped_count += heapSort(new_array, new_size, predicate);//【本来の処理】ヒープソートに切り替え
					swapped_count += shellSort(new_array, new_size, predicate);//【改良】シェルソートに切り替え
				}
				else
				{
					//再帰
					stack_p = &stack[stack_curr++];
					stack_p->array = new_array;
					stack_p->size = new_size;
					stack_p->depth = new_depth;
				}
			}
		}
	}
	return swapped_count;
}
template<class T, class PREDICATE>
inline std::size_t introSort(T* array, const std::size_t size, PREDICATE predicate)
{
	if (!array || size <= 1)
		return 0;
	if (calcUnordered(array, size, predicate) == 0)
		return 0;
	return _introSort(array, size, predicate);
}
sortFuncSet(introSort);

//========================================
//ソートアルゴリズム分類：混成ソート
//========================================

//----------------------------------------
//アルゴリズム：イントロソート
//※イテレータ対応版
//----------------------------------------
//・最良計算時間：O(n log n)
//・平均計算時間：O(n log n)
//・最悪計算時間：O(n log n)
//・メモリ使用量：O(n log n) ※ループ処理版は O(32*2)
//・安定性：　　　×
//----------------------------------------
template<class ITERATOR, class PREDICATE>
std::size_t _iteratorIntroSort(ITERATOR begin, ITERATOR end, PREDICATE predicate)
{
	int depth_max = 0;//ヒープソートに切り替える再帰（スタック）の深さ
	typename ITERATOR::difference_type size = iteratorDifference(begin, end);
	for (std::size_t size_tmp = size; size_tmp > 1; size_tmp >>= 1, ++depth_max);
	//--------------------
	//クイックソート：スタック処理版
	//※再帰処理版は省略
	static const std::size_t SIZE_THRESHOLD = 16;//32;//挿入ソートに切り替える件数
	std::size_t swapped_count = 0;
	struct stack_t
	{
		ITERATOR begin;
		ITERATOR end;
		int depth;
	};
	static const int STACK_DEPTH_MAX = 32 * 2;
	stack_t stack[STACK_DEPTH_MAX];
	//最初の配列をスタックにプッシュ
	stack_t* stack_p = &stack[0];
	stack_p->begin = begin;
	stack_p->end = end;
	stack_p->depth = depth_max;
	int stack_curr = 1;//スタック位置
	while (stack_curr > 0)
	{
		//配列をスタックから取得
		stack_p = &stack[--stack_curr];
		ITERATOR _begin_range = std::move(stack_p->begin);
		ITERATOR _end_range = std::move(stack_p->end);
		const typename ITERATOR::difference_type _size = iteratorDifference(_begin_range, _end_range);
		const int _depth = stack_p->depth;
		//配列の範囲情報
		ITERATOR _begin = _begin_range;
		ITERATOR _end = _end_range - 1;
		//軸を決定
		ITERATOR _mid = _begin + (_size >> 1);
		ITERATOR _pivot =
			predicate(_begin, _mid) ?
				predicate(_mid, _end) ?
					_mid :
					predicate(_end, _begin) ?
						_begin :
						_end :
				predicate(_end, _mid) ?
					_mid :
					predicate(_begin, _end) ?
						_begin :
						_end;
		//軸未満の配列と軸以上の配列に二分
		while (true)
		{
			while (predicate(_begin, _pivot))
				++_begin;
			while (predicate(_pivot, _end))
				--_end;
			if (_begin >= _end)
				break;
			iteratorSwapValues(_begin, _end);
			_pivot = _pivot == _begin ? _end : _pivot == _end ? _begin : _pivot;//軸の位置調整（軸の位置も入れ替わるため）
			++swapped_count;
			++_begin;
			--_end;
		}
		//recursive = 0 : 軸未満の配列をプッシュ
		//            1 : 軸以上の配列をプッシュ
		for (int recursive = 0; recursive < 2; ++recursive)
		{
			ITERATOR _new_begin = recursive == 0 ? _begin_range : _end + 1;
			ITERATOR _new_end = recursive == 0 ? _begin : _end_range;
			const typename ITERATOR::difference_type _new_size = _new_end - _new_begin;
			const int _new_depth = _depth - 1;
			if (_new_size >= 1)
			{
				if (_new_size < SIZE_THRESHOLD)
				{
					//swapped_count += iteratorInsertionSort(_new_begin, _new_end, predicate);//【本来の処理】挿入ソートに切り替え
					//swapped_count += iteratorCombSort(_new_begin, _new_end, predicate);//【改良】コムソートに切り替え
					swapped_count += iteratorShellSort(_new_begin, _new_end, predicate);//【改良】シェルソートに切り替え
				}
				else if (_new_depth == 0)
				{
					//swapped_count += iteratorHeapSort(_new_begin, _new_end, predicate);//【本来の処理】ヒープソートに切り替え
					//swapped_count += iteratorCombSort(_new_begin, _new_end, predicate);//【改良】コムソートに切り替え
					swapped_count += iteratorShellSort(_new_begin, _new_end, predicate);//【改良】シェルソートに切り替え
				}
				else
				{
					//再帰
					stack_p = &stack[stack_curr++];
					stack_p->begin = _new_begin;
					stack_p->end = _new_end;
					stack_p->depth = _new_depth;
				}
			}
		}
	}
	return swapped_count;
}
template<class ITERATOR, class PREDICATE>
inline std::size_t iteratorIntroSort(ITERATOR begin, ITERATOR end, PREDICATE predicate)
{
	if (begin == end)
		return 0;
	if (iteratorCalcUnordered(begin, end, predicate) == 0)
		return 0;
	return _iteratorIntroSort(begin, end, predicate);
}
iteratorSortFuncSet(iteratorIntroSort);

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__INTRO_SORT_H_

// End of file

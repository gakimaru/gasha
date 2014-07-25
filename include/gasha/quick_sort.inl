#pragma once
#ifndef GASHA_INCLUDED_QUICK_SORT_INL
#define GASHA_INCLUDED_QUICK_SORT_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// quick_sort.inl
// クイックソート【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/quick_sort.h>//クイックソート【宣言部】

#include <gasha/utility.h>//汎用ユーティリティ（値交換用）
#include <gasha/simple_assert.h>//シンプルアサーション

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//ソートアルゴリズム分類：分割交換ソート
//========================================

//----------------------------------------
//アルゴリズム：クイックソート
namespace _private
{
	template<class T, class PREDICATE>
	std::size_t quickSort(T* array, const std::size_t size, PREDICATE predicate)
	{
	#ifdef GASHA_QUICK_SORT_USE_RECURSIVE_CALL
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
			GASHA_ swapValues(*begin, *end);
			pivot = pivot == begin ? end : pivot == end ? begin : pivot;//軸の位置調整（軸の位置も入れ替わるため）
			++swapped_count;
			++begin;
			--end;
		}
		//再帰処理
		swapped_count += _quickSort(array, begin - array, predicate);//軸未満の配列
		swapped_count += _quickSort(end + 1, term - end - 1, predicate);//軸以上の配列
		return swapped_count;
	#else//GASHA_QUICK_SORT_USE_RECURSIVE_CALL
	#ifndef GASHA_QUICK_SORT_USE_OPENMP
		//--------------------
		//スタック処理版
		//※再帰処理より速いが、並びの状態によってはスタックオーバーフローの危険性がある。
		//※イントロソートなら、一定の再帰レベルでクイックソートを打ち切るので、スタックオーバーフローの危険性がない。
		std::size_t swapped_count = 0;
		struct stack_t
		{
			T* array;
			std::size_t size;
		};
		static const int STACK_DEPTH_MAX = 64 * 2;//最大再帰レベル※多めに設定
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
				GASHA_ swapValues(*begin, *end);
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
					GASHA_SIMPLE_ASSERT(stack_curr < STACK_DEPTH_MAX, "Stack overflow.");
					if (stack_curr < STACK_DEPTH_MAX - 1)
					{
						stack_p = &stack[stack_curr++];
						stack_p->array = new_array;
						stack_p->size = new_size;
					}
				}
			}
		}
		return swapped_count;
	#else//GASHA_QUICK_SORT_USE_OPENMP
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
						GASHA_ swapValues(*begin, *end);
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
							GASHA_SIMPLE_ASSERT(queue_write_tmp >= queue_read ? queue_write_tmp - queue_read < size : size - queue_write_tmp + queue_read < size, "Stack overflow.");
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
	#endif//GASHA_QUICK_SORT_USE_OPENMP
	#endif//GASHA_QUICK_SORT_USE_RECURSIVE_CALL
	}
}//namespace _private
template<class T, class PREDICATE>
inline std::size_t quickSort(T* array, const std::size_t size, PREDICATE predicate)
{
	if (!array || size <= 1)
		return 0;
	return _private::quickSort(array, size, predicate);
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_QUICK_SORT_INL

// End of file

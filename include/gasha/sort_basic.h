#pragma once
#ifndef __SORT_BASIC_H_
#define __SORT_BASIC_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// sort_basic.h
// ソート処理基本
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

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
//ソート用基本関数オブジェクト
//========================================

//----------------------------------------
//ソート用標準プレディケート関数オブジェクト：less
//※対象オブジェクトが operator<に対応している必要あり
template<typename T>
struct less{
	inline bool operator()(const T& val1, const T& val2) const
	{
		return val1 < val2;
	}
};

//========================================
//ソート関数補助マクロ
//========================================

//----------------------------------------
//比較ソート処理オーバーロード関数用マクロ
//※プレディケート関数指定版
#define sortFuncSetByUserPredicate(func_name) \
	template<class T, std::size_t N, class PREDICATE> \
	inline std::size_t func_name(T(&array)[N], PREDICATE predicate) \
	{ \
		return func_name(array, N, predicate); \
	} \
	template<class T, class PREDICATE> \
	inline std::size_t func_name(T* begin, T* end, PREDICATE predicate) \
	{ \
		const std::size_t size = end - begin; \
		return size == 0 ? 0 : func_name(begin, size, predicate); \
	} \
	template<class ITERATOR, class PREDICATE> \
	inline std::size_t func_name(ITERATOR& begin, ITERATOR& end, PREDICATE predicate) \
	{ \
		const std::size_t size = end - begin; \
		return size == 0 ? 0 : func_name(&begin[0], size, predicate); \
	} \
	template<class CONTAINER, class PREDICATE> \
	inline std::size_t func_name(CONTAINER& con, PREDICATE predicate) \
	{ \
		std::size_t size = con.size(); \
		return size == 0 ? 0 : func_name(&(con.at(0)), size, predicate); \
	}
//※標準プレディケート関数使用版
#define sortFuncSetByDefaultPredicate(func_name) \
	template<class T> \
	inline std::size_t func_name(T* array, const std::size_t size) \
	{ \
		return func_name(array, size, less<T>()); \
	} \
	template<class T, std::size_t N> \
	inline std::size_t func_name(T(&array)[N]) \
	{ \
		return func_name(array, less<T>()); \
	} \
	template<class T> \
	inline std::size_t func_name(T* begin, T* end) \
	{ \
		return func_name(begin, end, less<T>()); \
	} \
	template<class ITERATOR> \
	inline std::size_t func_name(ITERATOR& begin, ITERATOR& end) \
	{ \
		return func_name(begin, end, less<typename ITERATOR::value_type>()); \
	} \
	template<class CONTAINER> \
	inline std::size_t func_name(CONTAINER& con) \
	{ \
		return func_name(con, less<typename CONTAINER::value_type>()); \
	}
#define sortFuncSet(func_name) \
	sortFuncSetByUserPredicate(func_name) \
	sortFuncSetByDefaultPredicate(func_name)

//----------------------------------------
//比較ソート処理オーバーロード関数用マクロ
//※イテレータ対応版
//※標準プレディケート関数使用版
#define iteratorSortFuncSetByDefaultPredicate(func_name) \
	template<class ITERATOR> \
	inline std::size_t func_name(ITERATOR begin, ITERATOR end) \
{ \
	typedef typename ITERATOR::value_type value_type; \
	return func_name(begin, end, less<value_type>()); \
}
#define iteratorSortFuncSet(func_name) \
	iteratorSortFuncSetByDefaultPredicate(func_name)

//----------------------------------------
//非比較ソート処理オーバーロード関数用マクロ
//※キー取得用関数オブジェクト指定版
#define distributedSortFuncSetByUserFunctor(func_name) \
	template<class T, std::size_t N, class GET_KEY_FUNCTOR> \
	inline std::size_t func_name(T(&array)[N], GET_KEY_FUNCTOR get_key_functor) \
	{ \
		return func_name(array, N, get_key_functor); \
	} \
	template<class T, class GET_KEY_FUNCTOR> \
	inline std::size_t func_name(T* begin, T* end, GET_KEY_FUNCTOR get_key_functor) \
	{ \
		const std::size_t size = end - begin; \
		return size == 0 ? 0 : func_name(begin, size, get_key_functor); \
	} \
	template<class ITERATOR, class GET_KEY_FUNCTOR> \
	inline std::size_t func_name(ITERATOR& begin, ITERATOR& end, GET_KEY_FUNCTOR get_key_functor) \
	{ \
		const std::size_t size = end - begin; \
		return size == 0 ? 0 : func_name(&begin[0], size, get_key_functor); \
	} \
	template<class CONTAINER, class GET_KEY_FUNCTOR> \
	inline std::size_t func_name(CONTAINER& con, GET_KEY_FUNCTOR get_key_functor) \
	{ \
		std::size_t size = con.size(); \
		return size == 0 ? 0 : func_name(&(con.at(0)), size, get_key_functor); \
	}
#define distributedSortFuncSet(func_name) \
	distributedSortFuncSetByUserFunctor(func_name)

//========================================
//整列状態確認
//========================================

//----------------------------------------
//整列状態確認
template<class T, class PREDICATE>
inline std::size_t calcUnordered(const T* array, const std::size_t size, PREDICATE predicate)
{
	std::size_t unordered = 0;
	const T* prev = array;
	const T* now = prev + 1;
	for (std::size_t i = 1; i < size; ++i, ++now, ++prev)
	{
		if (predicate(*now, *prev))
			++unordered;
	}
	return unordered;
}
sortFuncSet(calcUnordered);

//----------------------------------------
//整列状態確認
//※イテレータ対応版
template<class ITERATOR, class PREDICATE>
inline std::size_t iteratorCalcUnordered(ITERATOR begin, ITERATOR end, PREDICATE predicate)
{
	std::size_t unordered = 0;
	ITERATOR prev = begin;
	ITERATOR now = begin;
	++now;
	for (;now != end; ++now, ++prev)
	{
		if (predicate(*now, *prev))
			++unordered;
	}
	return unordered;
}
iteratorSortFuncSet(iteratorCalcUnordered);

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__SORT_BASIC_H_

// End of file

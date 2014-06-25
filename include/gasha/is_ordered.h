#pragma once
#ifndef __IS_ORDERED_H_
#define __IS_ORDERED_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// is_ordered.h
// 整列状態（ソート済み状態）確認
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/sort_basic.h>//ソート処理基本

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//整列状態確認補助マクロ
//========================================

//----------------------------------------
//整列状態確認処理オーバーロード関数用マクロ
//※プレディケート関数指定版
#define isOrderedFuncSetByUserPredicate(func_name) \
	template<class T, std::size_t N, class PREDICATE> \
	inline bool func_name(T(&array)[N], PREDICATE predicate) \
	{ \
		return func_name(array, N, predicate); \
	} \
	template<class T, class PREDICATE> \
	inline bool func_name(T* begin, T* end, PREDICATE predicate) \
	{ \
		const std::size_t size = end - begin; \
		return func_name(begin, size, predicate); \
	} \
	template<class ITERATOR, class PREDICATE> \
	inline bool func_name(ITERATOR& begin, ITERATOR& end, PREDICATE predicate) \
	{ \
		const std::size_t size = end - begin; \
		return func_name(&begin[0], size, predicate); \
	} \
	template<class CONTAINER, class PREDICATE> \
	inline bool func_name(CONTAINER& con, PREDICATE predicate) \
	{ \
		const std::size_t size = con.size(); \
		return func_name(&(con.at(0)), size, predicate); \
	}
//※標準プレディケート関数使用版
#define isOrderedFuncSetByDefaultPredicate(func_name) \
	template<class T> \
	inline bool func_name(T* array, const std::size_t size) \
	{ \
		return func_name(array, size, less<T>()); \
	} \
	template<class T, std::size_t N> \
	inline bool func_name(T(&array)[N]) \
	{ \
		return func_name(array, less<T>()); \
	} \
	template<class T> \
	inline bool func_name(T* begin, T* end) \
	{ \
		return func_name(begin, end, less<T>()); \
	} \
	template<class ITERATOR> \
	inline bool func_name(ITERATOR& begin, ITERATOR& end) \
	{ \
		return func_name(begin, end, less<typename ITERATOR::value_type>()); \
	} \
	template<class CONTAINER> \
	inline bool func_name(CONTAINER& con) \
	{ \
		return func_name(con, less<typename CONTAINER::value_type>()); \
	}
#define isOrderedFuncSet(func_name) \
	isOrderedFuncSetByUserPredicate(func_name) \
	isOrderedFuncSetByDefaultPredicate(func_name)

//----------------------------------------
//整列状態確認処理オーバーロード関数用マクロ
//※イテレータ対応版
//※標準プレディケート関数使用版
#define iteratorIsOrderedFuncSetByDefaultPredicate(func_name) \
	template<class ITERATOR> \
	inline bool func_name(ITERATOR begin, ITERATOR end) \
{ \
	typedef typename ITERATOR::value_type value_type; \
	return func_name(begin, end, less<value_type>()); \
}
#define iteratorIsOrderedFuncSet(func_name) \
	iteratorIsOrderedFuncSetByDefaultPredicate(func_name)

//========================================
//整列状態確認
//========================================

//----------------------------------------
//非整列状態確認
template<class T, class PREDICATE>
bool isUnordered(const T* array, const std::size_t size, PREDICATE predicate)
{
	const T* prev = array;
	const T* now = prev + 1;
	for (std::size_t i = 1; i < size; ++i, ++now, ++prev)
	{
		if (predicate(*now, *prev))
			return true;
	}
	return false;
}
isOrderedFuncSet(isUnordered);

//----------------------------------------
//非整列状態確認
//※イテレータ対応版
template<class ITERATOR, class PREDICATE>
bool iteratorIsUnordered(ITERATOR begin, ITERATOR end, PREDICATE predicate)
{
	ITERATOR prev = begin;
	ITERATOR now = begin;
	++now;
	for (;now != end; ++now, ++prev)
	{
		if (predicate(*now, *prev))
			return true;
	}
	return false;
}
iteratorIsOrderedFuncSet(iteratorIsUnordered);

//----------------------------------------
//整列状態確認
template<class T, class PREDICATE>
inline bool isOrdered(const T* array, const std::size_t size, PREDICATE predicate)
{
	return !isUnordered(array, size, predicate);
}
isOrderedFuncSet(isOrdered);

//----------------------------------------
//整列状態確認
//※イテレータ対応版
template<class ITERATOR, class PREDICATE>
bool iteratorIsOrdered(ITERATOR begin, ITERATOR end, PREDICATE predicate)
{
	return !iteratorIsUnordered(begin, end, predicate);
}
iteratorIsOrderedFuncSet(iteratorIsOrdered);

//----------------------------------------
//非整列要素数計上
template<class T, class PREDICATE>
std::size_t sumupUnordered(const T* array, const std::size_t size, PREDICATE predicate)
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
sortFuncSet(sumupUnordered);

//----------------------------------------
//非整列要素数計上
//※イテレータ対応版
template<class ITERATOR, class PREDICATE>
std::size_t iteratorSumupUnordered(ITERATOR begin, ITERATOR end, PREDICATE predicate)
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
iteratorSortFuncSet(iteratorSumupUnordered);

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__IS_ORDERED_H_

// End of file

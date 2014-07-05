#pragma once
#ifndef GASHA_INCLUDED_IS_ORDERED_INL
#define GASHA_INCLUDED_IS_ORDERED_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// is_ordered.inl
// 整列状態（ソート済み状態）確認【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/is_ordered.h>//整列状態（ソート済み状態）確認【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

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

//----------------------------------------
//非整列状態確認
//※片方向連結リスト対応版
template<class T, class GET_NEXT_FUNC, class PREDICATE>
bool singlyLinkedListIsUnordered(const T* first, GET_NEXT_FUNC get_next_func, PREDICATE predicate)
{
	const T* prev = first;
	const T* now = get_next_func(*prev);
	while (now)
	{
		if (predicate(*now, *prev))
			return true;
		now = get_next_func(*now);
	}
	return false;
}

//----------------------------------------
//整列状態確認
template<class T, class PREDICATE>
inline bool isOrdered(const T* array, const std::size_t size, PREDICATE predicate)
{
	return !isUnordered(array, size, predicate);
}

//----------------------------------------
//整列状態確認
template<class ITERATOR, class PREDICATE>
bool iteratorIsOrdered(ITERATOR begin, ITERATOR end, PREDICATE predicate)
{
	return !iteratorIsUnordered(begin, end, predicate);
}

//----------------------------------------
//整列状態確認
//※片方向連結リスト対応版
template<class T, class GET_NEXT_FUNC, class PREDICATE>
bool singlyLinkedListIsOrdered(const T* first, GET_NEXT_FUNC get_next_func, PREDICATE predicate)
{
	return !singlyLinkedListIsUnordered(first, get_next_func, predicate);
}

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

//----------------------------------------
//非整列要素数計上
//※片方向連結リスト対応版
template<class T, class GET_NEXT_FUNC, class PREDICATE>
std::size_t singlyLinkedListSumupUnordered(const T* first, GET_NEXT_FUNC get_next_func, PREDICATE predicate)
{
	std::size_t unordered = 0;
	const T* prev = first;
	const T* now = get_next_func(*prev);
	while (now)
	{
		if (predicate(*now, *prev))
			++unordered;
		now = get_next_func(*now);
	}
	return unordered;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_IS_ORDERED_INL

// End of file

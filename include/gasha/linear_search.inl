#pragma once
#ifndef GASHA_INCLUDED_LINERAR_SEARCH_INL
#define GASHA_INCLUDED_LINERAR_SEARCH_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// linear_search.inl
// 線形探索【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/linear_search.h>//線形探索【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//アルゴリズム：線形探索
template<class T, class PREDICATE>
T* linearSearch(T* array, const std::size_t size, PREDICATE predicate)
{
	if (!array || size == 0)
		return nullptr;
	T* now = array;
	for (std::size_t i = 0; i < size; ++i, ++now)//順次探索
	{
		if (predicate(*now))//探索値と一致したら終了
			return now;
	}
	return nullptr;//探索失敗
}

//----------------------------------------
//アルゴリズム：線形探索
//※イテレータ対応版
template<class ITERATOR, class PREDICATE>
ITERATOR iteratorLinearSearch(ITERATOR begin, ITERATOR end, PREDICATE predicate)
{
	if (begin == end)
		return end;
	ITERATOR now = begin;
	for (; now != end; ++now)//順次探索
	{
		if (predicate(*now))//探索値と一致したら終了
			return now;
	}
	return end;//探索失敗
}

//----------------------------------------
//アルゴリズム：線形探索
//※片方向連結リスト対応版
template<class T, class GET_NEXT_FUNC, class PREDICATE>
const T* singlyLinkedListLinearSearch(const T* first, GET_NEXT_FUNC get_next_func, PREDICATE predicate)
{
	for (const T* now = first; now; now = get_next_func(*now))//順次探索
	{
		if (predicate(*now))//探索値と一致したら終了
			return now;
	}
	return nullptr;//探索失敗
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LINERAR_SEARCH_INL

// End of file

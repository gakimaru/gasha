#pragma once
#ifndef GASHA_INCLUDED_IS_ORDERED_H
#define GASHA_INCLUDED_IS_ORDERED_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// is_ordered.h
// 整列状態（ソート済み状態）確認【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/sort_basic.h>//ソート処理基本

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//整列状態確認補助マクロ
//※比較ソート用の比較関数を使用
//========================================

//----------------------------------------
//整列状態確認処理オーバーロード関数用マクロ
//※プレディケート関数指定版
#define GASHA_OVERLOAD_SET_FOR_IS_ORDERED_WITH_PREDICATE(func_name) \
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
#define GASHA_OVERLOAD_SET_FOR_IS_ORDERED_WITH_DEFAULT_PREDICATE(func_name) \
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
//※全種
#define GASHA_OVERLOAD_SET_FOR_IS_ORDERED(func_name) \
	GASHA_OVERLOAD_SET_FOR_IS_ORDERED_WITH_PREDICATE(func_name) \
	GASHA_OVERLOAD_SET_FOR_IS_ORDERED_WITH_DEFAULT_PREDICATE(func_name)

//----------------------------------------
//整列状態確認処理オーバーロード関数用マクロ
//※イテレータ対応版
//※標準プレディケート関数使用版
#define GASHA_OVERLOAD_SET_FOR_ITERATOR_IS_ORDERED_WITH_DEFAULT_PREDICATE(func_name) \
	template<class ITERATOR> \
	inline bool func_name(ITERATOR begin, ITERATOR end) \
{ \
	typedef typename ITERATOR::value_type value_type; \
	return func_name(begin, end, less<value_type>()); \
}
//※全種
#define GASHA_OVERLOAD_SET_FOR_ITERATOR_IS_ORDERED(func_name) \
	GASHA_OVERLOAD_SET_FOR_ITERATOR_IS_ORDERED_WITH_DEFAULT_PREDICATE(func_name)

//----------------------------------------
//整列状態確認処理オーバーロード関数用マクロ
//※片方向連結リスト対応版
//※標準プレディケート関数使用版
#define GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_IS_ORDERED_WITH_DEFAULT_PREDICATE(func_name) \
	template<class T, class GET_NEXT_FUNC> \
	inline bool func_name(T* first, GET_NEXT_FUNC get_next_func) \
	{ \
		return func_name<T, GET_NEXT_FUNC>(first, get_next_func, less<T>()); \
	}
//※全種
#define GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_IS_ORDERED(func_name) \
	GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_IS_ORDERED_WITH_DEFAULT_PREDICATE(func_name)

//----------------------------------------
//非整列要素数計上処理オーバーロード関数用マクロ
//※全種
#define GASHA_OVERLOAD_SET_FOR_SUMUP_UNORDERED(func_name) \
	GASHA_OVERLOAD_SET_FOR_SORT(func_name);

//----------------------------------------
//非整列要素数計上処理オーバーロード関数用マクロ
//※イテレータ対応版
//※全種
#define GASHA_OVERLOAD_SET_FOR_ITERATOR_SUMUP_UNORDERED(func_name) \
	GASHA_OVERLOAD_SET_FOR_ITERATOR_SORT(func_name);

//----------------------------------------
//非整列要素数計上処理オーバーロード関数用マクロ
//※片方向連結リスト対応版
//※標準プレディケート関数使用版
#define GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SUMUP_UNORDERED_WITH_DEFAULT_PREDICATE(func_name) \
	template<class T, class GET_NEXT_FUNC> \
	inline std::size_t func_name(T* first, GET_NEXT_FUNC get_next_func) \
	{ \
		return func_name<T, GET_NEXT_FUNC>(first, get_next_func, less<T>()); \
	}
#define GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SUMUP_UNORDERED(func_name) \
	GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SUMUP_UNORDERED_WITH_DEFAULT_PREDICATE(func_name)

//========================================
//整列状態確認
//========================================

//----------------------------------------
//非整列状態確認
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
template<class T, class PREDICATE>
bool isUnordered(const T* array, const std::size_t size, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_IS_ORDERED(isUnordered);

//----------------------------------------
//非整列状態確認
//※イテレータ対応版
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const typename ITERATOR::value_type& value1, const typename ITERATOR::value_type& value2)//value1 == value2 ならtrueを返す
template<class ITERATOR, class PREDICATE>
bool iteratorIsUnordered(ITERATOR begin, ITERATOR end, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_ITERATOR_IS_ORDERED(iteratorIsUnordered);

//----------------------------------------
//非整列状態確認
//※片方向連結リスト対応版
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
template<class T, class GET_NEXT_FUNC, class PREDICATE>
bool singlyLinkedListIsUnordered(const T* first, GET_NEXT_FUNC get_next_func, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_IS_ORDERED(singlyLinkedListIsUnordered);

//----------------------------------------
//整列状態確認
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
template<class T, class PREDICATE>
inline bool isOrdered(const T* array, const std::size_t size, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_IS_ORDERED(isOrdered);

//----------------------------------------
//整列状態確認
//※イテレータ対応版
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const typename ITERATOR::value_type& value1, const typename ITERATOR::value_type& value2)//value1 == value2 ならtrueを返す
template<class ITERATOR, class PREDICATE>
bool iteratorIsOrdered(ITERATOR begin, ITERATOR end, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_ITERATOR_IS_ORDERED(iteratorIsOrdered);

//----------------------------------------
//整列状態確認
//※片方向連結リスト対応版
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
template<class T, class GET_NEXT_FUNC, class PREDICATE>
bool singlyLinkedListIsOrdered(const T* first, GET_NEXT_FUNC get_next_func, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_IS_ORDERED(singlyLinkedListIsOrdered);

//----------------------------------------
//非整列要素数計上
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
template<class T, class PREDICATE>
std::size_t sumupUnordered(const T* array, const std::size_t size, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_SUMUP_UNORDERED(sumupUnordered);

//----------------------------------------
//非整列要素数計上
//※イテレータ対応版
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const typename ITERATOR::value_type& value1, const typename ITERATOR::value_type& value2)//value1 == value2 ならtrueを返す
template<class ITERATOR, class PREDICATE>
std::size_t iteratorSumupUnordered(ITERATOR begin, ITERATOR end, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_ITERATOR_SUMUP_UNORDERED(iteratorSumupUnordered);

//----------------------------------------
//非整列要素数計上
//※片方向連結リスト対応版
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
template<class T, class GET_NEXT_FUNC, class PREDICATE>
std::size_t singlyLinkedListSumupUnordered(const T* first, GET_NEXT_FUNC get_next_func, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SUMUP_UNORDERED(singlyLinkedListSumupUnordered);

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/is_ordered.inl>

#endif//GASHA_INCLUDED_IS_ORDERED_H

// End of file

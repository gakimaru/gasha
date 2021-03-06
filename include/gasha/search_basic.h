﻿#pragma once
#ifndef GASHA_INCLUDED_SEARCH_BASIC_H
#define GASHA_INCLUDED_SEARCH_BASIC_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// search_basic.h
// 探索処理基本【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//探索用基本関数オブジェクト
//========================================

//----------------------------------------
//線形探索用標準プレディケート関数オブジェクト：eqal_to
//※対象オブジェクトが operator==に対応している必要あり
template<typename T>
struct equal_to{
	template<typename V>
	inline bool operator()(const T& val1, const V& val2) const
	{
		return val1 == val2;
	}
};

//----------------------------------------
//二分探索用標準比較関数オブジェクト：compare_to
//※対象オブジェクトが operator==, operator<に対応している必要あり
template<typename T>
struct compare_to{
	template<typename V>
	inline int operator()(const T& val1, const V& val2) const
	{
		if (val1 == val2)
			return 0;
		else if (val1 < val2)
			return 1;
		else//if (val1 > val2)
			return -1;
	}
};

//========================================
//探索関数補助マクロ
//========================================

//----------------------------------------
//探索処理オーバーロード関数用マクロ
#define GASHA_OVERLOAD_SET_FOR_SEARCH_WITH_PREDICATE_OR_COMPARISON(func_name) \
	template<class T, class PREDICATE_OR_COMPARISON> \
	inline const T* func_name(const T* array, const std::size_t size, PREDICATE_OR_COMPARISON predicate_or_comparison) \
	{ \
		return func_name(const_cast<T*>(array), size, predicate_or_comparison); \
	} \
	template<class T, std::size_t N, class PREDICATE_OR_COMPARISON> \
	inline T* func_name(T(&array)[N], PREDICATE_OR_COMPARISON predicate_or_comparison) \
	{ \
		return func_name(array, N, predicate_or_comparison); \
	} \
	template<class T, std::size_t N, class PREDICATE_OR_COMPARISON> \
	inline const T* func_name(const T(&array)[N], PREDICATE_OR_COMPARISON predicate_or_comparison) \
	{ \
		return func_name(const_cast<T*>(array), N, predicate_or_comparison); \
	} \
	template<class T, class PREDICATE_OR_COMPARISON> \
	inline T* func_name(T* begin, T* end, PREDICATE_OR_COMPARISON predicate_or_comparison) \
	{ \
		const std::size_t size = end - begin; \
		return size == 0 ? nullptr : func_name(begin, size, predicate_or_comparison); \
	} \
	template<class T, class PREDICATE_OR_COMPARISON> \
	inline const T* func_name(const T* begin, const T* end, PREDICATE_OR_COMPARISON predicate_or_comparison) \
	{ \
		const std::size_t size = end - begin; \
		return size == 0 ? nullptr : func_name(const_cast<T*>(begin), size, predicate_or_comparison); \
	} \
	template<class ITERATOR, class PREDICATE_OR_COMPARISON> \
	inline typename ITERATOR::value_type* func_name(ITERATOR& begin, ITERATOR& end, PREDICATE_OR_COMPARISON predicate_or_comparison) \
	{ \
		const std::size_t size = end - begin; \
		return size == 0 ? nullptr : func_name(&begin[0], size, predicate_or_comparison); \
	} \
	template<class ITERATOR, class PREDICATE_OR_COMPARISON> \
	inline const typename ITERATOR::value_type* func_name(const ITERATOR& begin, const ITERATOR& end, PREDICATE_OR_COMPARISON predicate_or_comparison) \
	{ \
		const std::size_t size = end - begin; \
		return size == 0 ? nullptr : func_name(const_cast<typename ITERATOR::value_type*>(&begin[0]), size, predicate_or_comparison); \
	} \
	template<class CONTAINER, class PREDICATE_OR_COMPARISON> \
	inline typename CONTAINER::value_type* func_name(CONTAINER& con, PREDICATE_OR_COMPARISON predicate_or_comparison) \
	{ \
		const std::size_t size = con.size(); \
		return size == 0 ? nullptr : func_name(&(con.at(0)), size, predicate_or_comparison); \
	} \
	template<class CONTAINER, class PREDICATE_OR_COMPARISON> \
	inline const typename CONTAINER::value_type* func_name(const CONTAINER& con, PREDICATE_OR_COMPARISON predicate_or_comparison) \
	{ \
		const std::size_t size = con.size(); \
		return size == 0 ? nullptr : func_name(&(const_cast<CONTAINER*>(&con)->at(0)), size, predicate_or_comparison); \
	}
//※探索値指定版：プレディケート関数と値で比較
#define GASHA_OVERLOAD_SET_FOR_SEARCH_WITH_PREDICATE_ADN_VALUE(func_name) \
	template<class T, typename V, class PREDICATE> \
	inline T* func_name##Value(T* array, const std::size_t size, const V& value, PREDICATE predicate) \
	{ \
		auto _equal = [&value, &predicate](T& val1) -> bool { return predicate(val1, value); }; \
		return func_name(array, size, _equal); \
	} \
	template<class T, typename V, class PREDICATE> \
	inline const T* func_name##Value(const T* array, const std::size_t size, const V& value, PREDICATE predicate) \
	{ \
		auto _equal = [&value, &predicate](const T& val1) -> bool { return predicate(val1, value); }; \
		return func_name(const_cast<T*>(array), size, _equal); \
	} \
	template<class T, std::size_t N, typename V, class PREDICATE> \
	inline T* func_name##Value(T(&array)[N], const V& value, PREDICATE predicate) \
	{ \
		auto _equal = [&value, &predicate](const T& val1) -> bool { return predicate(val1, value); }; \
		return func_name(array, N, _equal); \
	} \
	template<class T, std::size_t N, typename V, class PREDICATE> \
	inline const T* func_name##Value(const T(&array)[N], const V& value, PREDICATE predicate) \
	{ \
		auto _equal = [&value, &predicate](const T& val1) -> bool { return predicate(val1, value); }; \
		return func_name(const_cast<T*>(array), N, _equal); \
	} \
	template<class T, typename V, class PREDICATE> \
	inline T* func_name##Value(T* begin, T* end, const V& value, PREDICATE predicate) \
	{ \
		const std::size_t size = end - begin; \
		auto _equal = [&value, &predicate](const T& val1) -> bool { return predicate(val1, value); }; \
		return size == 0 ? nullptr : func_name(begin, size, _equal); \
	} \
	template<class T, typename V, class PREDICATE> \
	inline const T* func_name##Value(const T* begin, const T* end, const V& value, PREDICATE predicate) \
	{ \
		const std::size_t size = end - begin; \
		auto _equal = [&value, &predicate](const T& val1) -> bool { return predicate(val1, value); }; \
		return size == 0 ? nullptr : func_name(const_cast<T*>(begin), size, _equal); \
	} \
	template<class ITERATOR, typename V, class PREDICATE> \
	inline typename ITERATOR::value_type* func_name##Value(ITERATOR& begin, ITERATOR& end, const V& value, PREDICATE predicate) \
	{ \
		const std::size_t size = end - begin; \
		auto _equal = [&value, &predicate](const typename ITERATOR::value_type& val1) -> bool { return predicate(val1, value); }; \
		return size == 0 ? nullptr : func_name(&begin[0], size, _equal); \
	} \
	template<class ITERATOR, typename V, class PREDICATE> \
	inline const typename ITERATOR::value_type* func_name##Value(const ITERATOR& begin, const ITERATOR& end, const V& value, PREDICATE predicate) \
	{ \
		const std::size_t size = end - begin; \
		auto _equal = [&value, &predicate](const typename ITERATOR::value_type& val1) -> bool { return predicate(val1, value); }; \
		return size == 0 ? nullptr : func_name(const_cast<typename ITERATOR::value_type*>(&begin[0]), size, _equal); \
	} \
	template<class CONTAINER, typename V, class PREDICATE> \
	inline typename CONTAINER::value_type* func_name##Value(CONTAINER& con, const V& value, PREDICATE predicate) \
	{ \
		const std::size_t size = con.size(); \
		auto _equal = [&value, &predicate](const typename CONTAINER::value_type& val1) -> bool { return predicate(val1, value); }; \
		return size == 0 ? nullptr : func_name(&(con.at(0)), size, _equal); \
	} \
	template<class CONTAINER, typename V, class PREDICATE> \
	inline const typename CONTAINER::value_type* func_name##Value(const CONTAINER& con, const V& value, PREDICATE predicate) \
	{ \
		const std::size_t size = con.size(); \
		auto _equal = [&value, &predicate](const typename CONTAINER::value_type& val1) -> bool { return predicate(val1, value); }; \
		return size == 0 ? nullptr : func_name(&(const_cast<CONTAINER*>(&con)->at(0)), size, _equal); \
	}
//※探索値指定版：標準のプレディケート関数と値で比較
#define GASHA_OVERLOAD_SET_FOR_SEARCH_WITH_DEFAULT_PREDICATE_AND_VALUE(func_name) \
	template<class T, typename V> \
	inline T* func_name##Value(T* array, const std::size_t size, const V& value) \
	{ \
		auto _equal = [&value](T& val1) -> bool { return equal_to<T>()(val1, value); }; \
		return func_name(array, size, _equal); \
	} \
	template<class T, typename V> \
	inline const T* func_name##Value(const T* array, const std::size_t size, const V& value) \
	{ \
		auto _equal = [&value](const T& val1) -> bool { return equal_to<T>()(val1, value); }; \
		return func_name(const_cast<T*>(array), size, _equal); \
	} \
	template<class T, std::size_t N, typename V> \
	inline T* func_name##Value(T(&array)[N], const V& value) \
	{ \
		auto _equal = [&value](const T& val1) -> bool { return equal_to<T>()(val1, value); }; \
		return func_name(array, N, _equal); \
	} \
	template<class T, std::size_t N, typename V> \
	inline const T* func_name##Value(const T(&array)[N], const V& value) \
	{ \
		auto _equal = [&value](const T& val1) -> bool { return equal_to<T>()(val1, value); }; \
		return func_name(const_cast<T*>(array), N, _equal); \
	} \
	template<class T, typename V> \
	inline T* func_name##Value(T* begin, T* end, const V& value) \
	{ \
		const std::size_t size = end - begin; \
		auto _equal = [&value](const T& val1) -> bool { return equal_to<T>()(val1, value); }; \
		return size == 0 ? nullptr : func_name(begin, size, _equal); \
	} \
	template<class T, typename V> \
	inline const T* func_name##Value(const T* begin, const T* end, const V& value) \
	{ \
		const std::size_t size = end - begin; \
		auto _equal = [&value](const T& val1) -> bool { return equal_to<T>()(val1, value); }; \
		return size == 0 ? nullptr : func_name(const_cast<T*>(begin), size, _equal); \
	} \
	template<class ITERATOR, typename V> \
	inline typename ITERATOR::value_type* func_name##Value(ITERATOR& begin, ITERATOR& end, const V& value) \
	{ \
		const std::size_t size = end - begin; \
		auto _equal = [&value](const typename ITERATOR::value_type& val1) -> bool { return equal_to<typename ITERATOR::value_type>()(val1, value); }; \
		return size == 0 ? nullptr : func_name(&begin[0], size, _equal); \
	} \
	template<class ITERATOR, typename V> \
	inline const typename ITERATOR::value_type* func_name##Value(const ITERATOR& begin, const ITERATOR& end, const V& value) \
	{ \
		const std::size_t size = end - begin; \
		auto _equal = [&value](const typename ITERATOR::value_type& val1) -> bool { return equal_to<typename ITERATOR::value_type>()(val1, value); }; \
		return size == 0 ? nullptr : func_name(const_cast<typename ITERATOR::value_type*>(&begin[0]), size, _equal); \
	} \
	template<class CONTAINER, typename V> \
	inline typename CONTAINER::value_type* func_name##Value(CONTAINER& con, const V& value) \
	{ \
		const std::size_t size = con.size(); \
		auto _equal = [&value](const typename CONTAINER::value_type& val1) -> bool { return equal_to<typename CONTAINER::value_type>()(val1, value); }; \
		return size == 0 ? nullptr : func_name(&(con.at(0)), size, _equal); \
	} \
	template<class CONTAINER, typename V> \
	inline const typename CONTAINER::value_type* func_name##Value(const CONTAINER& con, const V& value) \
	{ \
		const std::size_t size = con.size(); \
		auto _equal = [&value](const typename CONTAINER::value_type& val1) -> bool { return equal_to<typename CONTAINER::value_type>()(val1, value); }; \
		return size == 0 ? nullptr : func_name(&(const_cast<CONTAINER*>(&con)->at(0)), size, _equal); \
	}
#define GASHA_OVERLOAD_SET_FOR_SEARCH_WITH_PREDICATE(func_name) \
	GASHA_OVERLOAD_SET_FOR_SEARCH_WITH_PREDICATE_OR_COMPARISON(func_name) \
	GASHA_OVERLOAD_SET_FOR_SEARCH_WITH_PREDICATE_ADN_VALUE(func_name) \
	GASHA_OVERLOAD_SET_FOR_SEARCH_WITH_DEFAULT_PREDICATE_AND_VALUE(func_name)
//※探索値指定版：比較関数と値で比較
#define GASHA_OVERLOAD_SET_FOR_SEARCH_WITH_COMPARISON_AND_VALUE(func_name) \
	template<class T, typename V, class COMPARISON> \
	inline T* func_name##Value(T* array, const std::size_t size, const V& value, COMPARISON comparison) \
	{ \
		auto _comparison = [&value, &comparison](T& val1) -> int { return comparison(val1, value); }; \
		return func_name(array, size, _comparison); \
	} \
	template<class T, typename V, class COMPARISON> \
	inline const T* func_name##Value(const T* array, const std::size_t size, const V& value, COMPARISON comparison) \
	{ \
		auto _comparison = [&value, &comparison](const T& val1) -> int { return comparison(val1, value); }; \
		return func_name(const_cast<T*>(array), size, _comparison); \
	} \
	template<class T, std::size_t N, typename V, class COMPARISON> \
	inline T* func_name##Value(T(&array)[N], const V& value, COMPARISON comparison) \
	{ \
		auto _comparison = [&value, &comparison](const T& val1) -> int { return comparison(val1, value); }; \
		return func_name(array, N, _comparison); \
	} \
	template<class T, std::size_t N, typename V, class COMPARISON> \
	inline const T* func_name##Value(const T(&array)[N], const V &value, COMPARISON comparison) \
	{ \
		auto _comparison = [&value, &comparison](const T& val1) -> int { return comparison(val1, value); }; \
		return func_name(const_cast<T*>(array), N, _comparison); \
	} \
	template<class T, typename V, class COMPARISON> \
	inline T* func_name##Value(T* begin, T* end, const V& value, COMPARISON comparison) \
	{ \
		const std::size_t size = end - begin; \
		auto _comparison = [&value, &comparison](const T& val1) -> int { return comparison(val1, value); }; \
		return size == 0 ? nullptr : func_name(begin, size, _comparison); \
	} \
	template<class T, typename V, class COMPARISON> \
	inline const T* func_name##Value(const T* begin, const T* end, const V& value, COMPARISON comparison) \
	{ \
		const std::size_t size = end - begin; \
		auto _comparison = [&value, &comparison](const T& val1) -> int { return comparison(val1, value); }; \
		return size == 0 ? nullptr : func_name(const_cast<T*>(begin), size, _comparison); \
	} \
	template<class ITERATOR, typename V, class COMPARISON> \
	inline typename ITERATOR::value_type* func_name##Value(ITERATOR& begin, ITERATOR& end, const V& value, COMPARISON comparison) \
	{ \
		const std::size_t size = end - begin; \
		auto _comparison = [&value, &comparison](const typename ITERATOR::value_type& val1) -> int { return comparison(val1, value); }; \
		return size == 0 ? nullptr : func_name(&begin[0], size, _comparison); \
	} \
	template<class ITERATOR, typename V, class COMPARISON> \
	inline const typename ITERATOR::value_type* func_name##Value(const ITERATOR& begin, const ITERATOR& end, const V& value, COMPARISON comparison) \
	{ \
		const std::size_t size = end - begin; \
		auto _comparison = [&value, &comparison](const typename ITERATOR::value_type& val1) -> int { return comparison(val1, value); }; \
		return size == 0 ? nullptr : func_name(const_cast<typename ITERATOR::value_type*>(&begin[0]), size, _comparison); \
	} \
	template<class CONTAINER, typename V, class COMPARISON> \
	inline typename CONTAINER::value_type* func_name##Value(CONTAINER& con, const V& value, COMPARISON comparison) \
	{ \
		const std::size_t size = con.size(); \
		auto _comparison = [&value, &comparison](const typename CONTAINER::value_type& val1) -> int { return comparison(val1, value); }; \
		return size == 0 ? nullptr : func_name(&(con.at(0)), size, _comparison); \
	} \
	template<class CONTAINER, typename V, class COMPARISON> \
	inline const typename CONTAINER::value_type* func_name##Value(const CONTAINER& con, const V& value, COMPARISON comparison) \
	{ \
		const std::size_t size = con.size(); \
		auto _comparison = [&value, &comparison](const typename CONTAINER::value_type& val1) -> int { return comparison(val1, value); }; \
		return size == 0 ? nullptr : func_name(&(const_cast<CONTAINER*>(&con)->at(0)), size, _comparison); \
	}
//※探索値指定版：標準比較関数と値で比較
#define GASHA_OVERLOAD_SET_FOR_SEARCH_WITH_DEFAULT_COMPARISON_AND_VALUE(func_name) \
	template<class T, typename V> \
	inline T* func_name##Value(T* array, const std::size_t size, const V& value) \
	{ \
		auto _comparison = [&value](T& val1) -> int { return compare_to<T>()(val1, value); }; \
		return func_name(array, size, _comparison); \
	} \
	template<class T, typename V> \
	inline const T* func_name##Value(const T* array, const std::size_t size, const V& value) \
	{ \
		auto _comparison = [&value](const T& val1) -> int { return compare_to<T>()(val1, value); }; \
		return func_name(const_cast<T*>(array), size, _comparison); \
	} \
	template<class T, std::size_t N, typename V> \
	inline T* func_name##Value(T(&array)[N], const V& value) \
	{ \
		auto _comparison = [&value](const T& val1) -> int { return compare_to<T>()(val1, value); }; \
		return func_name(array, N, _comparison); \
	} \
	template<class T, std::size_t N, typename V> \
	inline const T* func_name##Value(const T(&array)[N], const V &value) \
	{ \
		auto _comparison = [&value](const T& val1) -> int { return compare_to<T>()(val1, value); }; \
		return func_name(const_cast<T*>(array), N, _comparison); \
	} \
	template<class T, typename V> \
	inline T* func_name##Value(T* begin, T* end, const V& value) \
	{ \
		const std::size_t size = end - begin; \
		auto _comparison = [&value](const T& val1) -> int { return compare_to<T>()(val1, value); }; \
		return size == 0 ? nullptr : func_name(begin, size, _comparison); \
	} \
	template<class T, typename V> \
	inline const T* func_name##Value(const T* begin, const T* end, const V& value) \
	{ \
		const std::size_t size = end - begin; \
		auto _comparison = [&value](const T& val1) -> int { return compare_to<T>()(val1, value); }; \
		return size == 0 ? nullptr : func_name(const_cast<T*>(begin), size, _comparison); \
	} \
	template<class ITERATOR, typename V> \
	inline typename ITERATOR::value_type* func_name##Value(ITERATOR& begin, ITERATOR& end, const V& value) \
	{ \
		const std::size_t size = end - begin; \
		auto _comparison = [&value](const typename ITERATOR::value_type& val1) -> int { return compare_to<typename ITERATOR::value_type>()(val1, value); }; \
		return size == 0 ? nullptr : func_name(&begin[0], size, _comparison); \
	} \
	template<class ITERATOR, typename V> \
	inline const typename ITERATOR::value_type* func_name##Value(const ITERATOR& begin, const ITERATOR& end, const V& value) \
	{ \
		const std::size_t size = end - begin; \
		auto _comparison = [&value](const typename ITERATOR::value_type& val1) -> int { return compare_to<typename ITERATOR::value_type>()(val1, value); }; \
		return size == 0 ? nullptr : func_name(const_cast<typename ITERATOR::value_type*>(&begin[0]), size, _comparison); \
	} \
	template<class CONTAINER, typename V> \
	inline typename CONTAINER::value_type* func_name##Value(CONTAINER& con, const V& value) \
	{ \
		const std::size_t size = con.size(); \
		auto _comparison = [&value](const typename CONTAINER::value_type& val1) -> int { return compare_to<typename CONTAINER::value_type>()(val1, value); }; \
		return size == 0 ? nullptr : func_name(&(con.at(0)), size, _comparison); \
	} \
	template<class CONTAINER, typename V> \
	inline const typename CONTAINER::value_type* func_name##Value(const CONTAINER& con, const V& value) \
	{ \
		const std::size_t size = con.size(); \
		auto _comparison = [&value](const typename CONTAINER::value_type& val1) -> int { return compare_to<typename CONTAINER::value_type>()(val1, value); }; \
		return size == 0 ? nullptr : func_name(&(const_cast<CONTAINER*>(&con)->at(0)), size, _comparison); \
	}
#define GASHA_OVERLOAD_SET_FOR_SEARCH_WITH_COMPARISON(func_name) \
	GASHA_OVERLOAD_SET_FOR_SEARCH_WITH_PREDICATE_OR_COMPARISON(func_name) \
	GASHA_OVERLOAD_SET_FOR_SEARCH_WITH_COMPARISON_AND_VALUE(func_name) \
	GASHA_OVERLOAD_SET_FOR_SEARCH_WITH_DEFAULT_COMPARISON_AND_VALUE(func_name)

//----------------------------------------
//探索処理オーバーロード関数用マクロ
//※イテレータ対応版
//※探索値指定版：プレディケート関数と値で比較
#define GASHA_OVERLOAD_SET_FOR_ITERATOR_SEARCH_WITH_PREDICATE_AND_VALUE(func_name) \
	template<class ITERATOR, typename V, class PREDICATE> \
	inline ITERATOR func_name##Value(ITERATOR begin, ITERATOR end, const V& value, PREDICATE predicate) \
{ \
	typedef typename ITERATOR::value_type value_type; \
	auto _equal = [&value, &predicate](value_type& val1) -> bool { return predicate(val1, value); }; \
	return func_name(begin, end, _equal); \
}
//※探索値指定版：標準のプレディケート関数と値で比較
#define GASHA_OVERLOAD_SET_FOR_ITERATOR_SEARCH_WITH_DEFAULT_PREDICATE_AND_VALUE(func_name) \
	template<class ITERATOR, typename V> \
	inline ITERATOR func_name##Value(ITERATOR begin, ITERATOR end, const V& value) \
{ \
	typedef typename ITERATOR::value_type value_type; \
	auto _equal = [&value](value_type& val1) -> bool { return equal_to<value_type>()(val1, value); }; \
	return func_name(begin, end, _equal); \
}
#define GASHA_OVERLOAD_SET_FOR_ITERATOR_SEARCH_WITH_PREDICATE(func_name) \
	GASHA_OVERLOAD_SET_FOR_ITERATOR_SEARCH_WITH_PREDICATE_AND_VALUE(func_name) \
	GASHA_OVERLOAD_SET_FOR_ITERATOR_SEARCH_WITH_DEFAULT_PREDICATE_AND_VALUE(func_name)
//※探索値指定版：比較関数と値で比較
#define GASHA_OVERLOAD_SET_FOR_ITERATOR_SEARCH_WITH_COMPARISON_AND_VALUE(func_name) \
	template<class ITERATOR, typename V, class COMPARISON> \
	inline ITERATOR func_name##Value(ITERATOR begin, ITERATOR end, const V& value, COMPARISON comparison) \
	{ \
		typedef typename ITERATOR::value_type value_type; \
		auto _comparison = [&value, &comparison](value_type& val1) -> int { return comparison(val1, value); }; \
		return func_name(begin, end, _comparison); \
	}
//※探索値指定版：標準比較関数と値で比較
#define GASHA_OVERLOAD_SET_FOR_ITERATOR_SEARCH_WITH_DEFAULT_COMPARISON_AND_VALUE(func_name) \
	template<class ITERATOR, typename V> \
	inline ITERATOR func_name##Value(ITERATOR begin, ITERATOR end, const V& value) \
	{ \
		typedef typename ITERATOR::value_type value_type; \
		auto _comparison = [&value](value_type& val1) -> int { return compare_to<value_type>()(val1, value); }; \
		return func_name(begin, end, _comparison); \
	}
#define GASHA_OVERLOAD_SET_FOR_ITERATOR_SEARCH_WITH_COMPARISON(func_name) \
	GASHA_OVERLOAD_SET_FOR_ITERATOR_SEARCH_WITH_COMPARISON_AND_VALUE(func_name) \
	GASHA_OVERLOAD_SET_FOR_ITERATOR_SEARCH_WITH_DEFAULT_COMPARISON_AND_VALUE(func_name)

//----------------------------------------
//探索処理オーバーロード関数用マクロ
//※双方向連結リスト対応版
//※探索値指定版：比較関数と値で比較
#define GASHA_OVERLOAD_SET_FOR_LINKED_LIST_SEARCH_WITH_COMPARISON_AND_VALUE(func_name) \
	template<class T, class GET_NEXT_FUNC, class GET_PREV_FUNC, typename V, class COMPARISON> \
	inline T* func_name##Value(T* first, GET_NEXT_FUNC get_next_func, GET_PREV_FUNC get_prev_func, const V& value, COMPARISON comparison) \
	{ \
		auto _comparison = [&value, &comparison](T& val1) -> int { return comparison(val1, value); }; \
		return func_name(first, get_next_func, get_prev_func, _comparison); \
	}
//※探索値指定版：標準比較関数と値で比較
#define GASHA_OVERLOAD_SET_FOR_LINKED_LIST_SEARCH_WITH_DEFAULT_COMPARISON_AND_VALUE(func_name) \
	template<class T, class GET_NEXT_FUNC, class GET_PREV_FUNC, typename V> \
	inline T* func_name##Value(T* first, GET_NEXT_FUNC get_next_func, GET_PREV_FUNC get_prev_func, const V& value) \
	{ \
		auto _comparison = [&value](T& val1) -> int { return compare_to<T>()(val1, value); }; \
		return func_name(first, get_next_func, get_prev_func, _comparison); \
	}
#define GASHA_OVERLOAD_SET_FOR_LINKED_LIST_SEARCH_WITH_COMPARISON(func_name) \
	GASHA_OVERLOAD_SET_FOR_LINKED_LIST_SEARCH_WITH_COMPARISON_AND_VALUE(func_name) \
	GASHA_OVERLOAD_SET_FOR_LINKED_LIST_SEARCH_WITH_DEFAULT_COMPARISON_AND_VALUE(func_name)

//----------------------------------------
//探索処理オーバーロード関数用マクロ
//※片方向連結リスト対応版
//※探索値指定版：プレディケート関数と値で比較
#define GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SEARCH_WITH_PREDICATE_AND_VALUE(func_name) \
	template<class T, class GET_NEXT_FUNC, typename V, class PREDICATE> \
	inline T* func_name##Value(T* first, GET_NEXT_FUNC get_next_func, const V& value, PREDICATE predicate) \
	{ \
		auto _equal = [&value, &predicate](T& val1) -> bool { return predicate(val1, value); }; \
		return func_name(first, get_next_func, _equal); \
	}
//※探索値指定版：標準のプレディケート関数と値で比較
#define GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SEARCH_WITH_DEFAULT_PREDICATE_AND_VALUE(func_name) \
	template<class T, class GET_NEXT_FUNC, typename V> \
	inline T* func_name##Value(T* first, GET_NEXT_FUNC get_next_func, const V& value) \
	{ \
		auto _equal = [&value](T& val1) -> bool { return equal_to<T>()(val1, value); }; \
		return func_name(first, get_next_func, _equal); \
	}
#define GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SEARCH_WITH_PREDICATE(func_name) \
	GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SEARCH_WITH_PREDICATE_AND_VALUE(func_name) \
	GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SEARCH_WITH_DEFAULT_PREDICATE_AND_VALUE(func_name)
//※探索値指定版：比較関数と値で比較
#define GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SEARCH_WITH_COMPARISON_AND_VALUE(func_name) \
	template<class T, class GET_NEXT_FUNC, typename V, class COMPARISON> \
	inline T* func_name##Value(T* first, GET_NEXT_FUNC get_next_func, const V& value, COMPARISON comparison) \
	{ \
		auto _comparison = [&value, &comparison](T& val1) -> int { return comparison(val1, value); }; \
		return func_name(first, get_next_func, _comparison); \
	}
//※探索値指定版：標準比較関数と値で比較
#define GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SEARCH_WITH_DEFAULT_COMPARISON_AND_VALUE(func_name) \
	template<class T, class GET_NEXT_FUNC, typename V> \
	inline T* func_name##Value(T* first, GET_NEXT_FUNC get_next_func, const V& value) \
	{ \
		auto _comparison = [&value](T& val1) -> int { return compare_to<T>()(val1, value); }; \
		return func_name(first, get_next_func, _comparison); \
	}
#define GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SEARCH_WITH_COMPARISON(func_name) \
	GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SEARCH_WITH_COMPARISON_AND_VALUE(func_name) \
	GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SEARCH_WITH_DEFAULT_COMPARISON_AND_VALUE(func_name)

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SEARCH_BASIC_H

// End of file

﻿#pragma once
#ifndef GASHA_INCLUDED_UTILITY_INL
#define GASHA_INCLUDED_UTILITY_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ】
// utility.inl
// 汎用ユーティリティ【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/utility.h>//汎用ユーティリティ【宣言部】

#include <type_traits>//c++11 std::conditional

#ifdef GASHA_SWAP_VALUES_USE_MEMCPY
#include <memory.h>//memcpy()
#endif//GASHA_SWAP_VALUES_USE_MEMCPY

#include <utility>//C++11 std::move, std::forward

//【VC++】例外を無効化した状態で <chrono> 内関数を使用すると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//最大値取得
//----------------------------------------

//値が二つの max()
template<typename T1, typename T2>
constexpr inline T1 max(const T1& n1, const T2& n2){ return static_cast<T1>(n2) < n1 ? n1 : static_cast<T1>(n2); }
template<typename T>
constexpr inline const T& maxObj(const T& obj1, const T& obj2){ return obj2 < obj1 ? obj1 : obj2; }
template<typename T>
constexpr inline T& maxObj(T&& obj1, T&& obj2){ return obj2 < obj1 ? obj1 : obj2; }

//値が三つ以上の max() : 再帰処理
template<typename T1, typename T2, typename T3, typename... Tx>
constexpr inline T1 max(const T1& n1, const T2& n2, const T3& n3, const Tx&... nx){ return GASHA_ max(GASHA_ max(n1, n2), n3, nx...); }
template<typename T1, typename T2, typename T3, typename... Tx>
constexpr inline const T1& maxObj(const T1& obj1, const T2& obj2, const T3& obj3, const Tx&... nx){ return GASHA_ maxObj(GASHA_ maxObj(obj1, obj2), obj3, nx...); }
template<typename T1, typename T2, typename T3, typename... Tx>
constexpr inline T1& maxObj(T1&& obj1, T2&& obj2, T3&& obj3, Tx&&... nx){ return GASHA_ maxObj(GASHA_ maxObj(obj1, obj2), obj3, std::forward<Tx>(nx)...); }

//----------------------------------------
//最小値取得
//----------------------------------------

//値が二つの min()
template<typename T1, typename T2>
constexpr inline T1 min(const T1& n1, const T2& n2){ return n1 < static_cast<T1>(n2) ? n1 : static_cast<T1>(n2); }
template<typename T>
constexpr inline const T& minObj(const T& obj1, const T& obj2){ return obj1 < obj2 ? obj1 : obj2; }
template<typename T>
constexpr inline T& minObj(T&& obj1, T&& obj2){ return obj1 < obj2 ? obj1 : obj2; }

//値が三つ以上の min() : 再帰処理
template<typename T1, typename T2, typename T3, typename... Tx>
constexpr inline T1 min(const T1& n1, const T2& n2, const T3& n3, const Tx&... nx){ return GASHA_ min(GASHA_ min(n1, n2), n3, nx...); }
template<typename T1, typename T2, typename T3, typename... Tx>
constexpr inline const T1& minObj(const T1& obj1, const T2& obj2, const T3& obj3, const Tx&... nx){ return GASHA_ minObj(GASHA_ minObj(obj1, obj2), obj3, nx...); }
template<typename T1, typename T2, typename T3, typename... Tx>
constexpr inline T1& minObj(T1&& obj1, T2&& obj2, T3&& obj3, Tx&&... nx){ return GASHA_ minObj(GASHA_ minObj(obj1, obj2), obj3, std::forward<Tx>(nx)...); }

//--------------------------------------------------------------------------------
//値の交換
//--------------------------------------------------------------------------------

namespace _private
{
	//----------------------------------------
	//型に応じた処理クラス
	template<class T>
	struct _swapArithmetic{
		inline static void exec(T& val1, T& val2)
		{
			T tmp;
			tmp = val2;
			val2 = val1;
			val1 = tmp;
		}
	};
	template<class T>
	struct _swapArithmetic<T*>{
		inline static void exec(T*& val1, T*& val2)
		{
			T* tmp;
			tmp = val2;
			val2 = val1;
			val1 = tmp;
		}
	};
	template<class T>
	struct _swapObjects{
		inline static void exec(T& val1, T& val2)
		{
		#ifdef GASHA_SWAP_VALUES_USE_MEMCPY//コンストラクタ／オペレータの呼び出しを避けて単純なメモリコピー
			char tmp[sizeof(T)];
			memcpy(tmp, &val2, sizeof(T));
			memcpy(&val2, &val1, sizeof(T));
			memcpy(&val1, tmp, sizeof(T));
		#else//GASHA_SWAP_VALUES_USE_MEMCPY//ムーブコンストラクタとムーブオペレータを使用して入れ替え（#include <utility> の std::swap() と同じ）
			T tmp = std::move(val2);
			val2 = std::move(val1);
			val1 = std::move(tmp);
		#endif//GASHA_SWAP_VALUES_USE_MEMCPY
		}
	};
	template<class T>
	struct _swapObjects<T*>{
		inline static void exec(T*& val1, T*& val2)
		{
			_swapArithmetic<T*>::exec(val1, val2);
		}
	};
}//namespace _private

//----------------------------------------
//値の交換
//※直接使用する関数
template<class T>
inline void swapValues(T& val1, T& val2)
{
	std::conditional<std::is_arithmetic<T>::value,
		_private::_swapArithmetic<T>,
		_private::_swapObjects<T>
	>::type::exec(val1, val2);
}

//----------------------------------------
//値の交換
//※イテレータ対応版
template<class ITERATOR>
inline void iteratorSwapValues(ITERATOR val1, ITERATOR val2)
{
	typedef typename ITERATOR::value_type value_type;
#ifdef GASHA_SWAP_VALUES_USE_MEMCPY//コンストラクタ／オペレータの呼び出しを避けて単純なメモリコピー
	char tmp[sizeof(value_type)];
	memcpy(tmp, &*val2, sizeof(value_type));
	memcpy(&*val2, &*val1, sizeof(value_type));
	memcpy(&*val1, tmp, sizeof(value_type));
#else//GASHA_SWAP_VALUES_USE_MEMCPY//ムーブコンストラクタとムーブオペレータを使用して入れ替え（#include <utility> の std::swap() と同じ）
	value_type tmp = std::move(*val2);
	*val2 = std::move(*val1);
	*val1 = std::move(tmp);
#endif//GASHA_SWAP_VALUES_USE_MEMCPY
}

//--------------------------------------------------------------------------------
//複数の値（要素）のローテーション
//--------------------------------------------------------------------------------

namespace _private
{
	//----------------------------------------
	//型に応じた処理クラス
	template<class T>
	struct _rotateArithmetic{
		inline static void exec(T* val1, T* val2, int step)
		{
			T tmp;
			tmp = *val2;
			while (val1 != val2)
			{
				T* val2_prev = val2 - step;
				*val2 = *val2_prev;
				val2 = val2_prev;
			}
			*val1 = tmp;
		}
	};
	template<class T>
	struct _rotateArithmetic<T*>{
		inline static void exec(T** val1, T** val2, int step)
		{
			T* tmp;
			tmp = *val2;
			while (val1 != val2)
			{
				T** val2_prev = val2 - step;
				*val2 = *val2_prev;
				val2 = val2_prev;
			}
			*val1 = tmp;
		}
	};
	template<class T>
	struct _rotateObjects{
		inline static void exec(T* val1, T* val2, int step)
		{
		#ifdef GASHA_SWAP_VALUES_USE_MEMCPY//コンストラクタ／オペレータの呼び出しを避けて単純なメモリコピー
			char tmp[sizeof(T)];
			memcpy(tmp, val2, sizeof(T));
			while (val1 != val2)
			{
				T* val2_prev = val2 - step;
				memcpy(val2, val2_prev, sizeof(T));
				val2 = val2_prev;
			}
			memcpy(val1, tmp, sizeof(T));
		#else//GASHA_SWAP_VALUES_USE_MEMCPY//ムーブコンストラクタとムーブオペレータを使用して入れ替え
			T tmp = std::move(*val2);
			while (val1 != val2)
			{
				T* val2_prev = val2 - step;
				*val2 = std::move(*val2_prev);
				val2 = val2_prev;
			}
			*val1 = std::move(tmp);
		#endif//GASHA_SWAP_VALUES_USE_MEMCPY
		}
	};
	template<class T>
	struct _rotateObjects<T*>{
		inline static void exec(T** val1, T** val2, int step)
		{
			_rotateArithmetic<T*>::exec(val1, val2, step);
		}
	};
}//namespace _private

//----------------------------------------
//値のローテーション
//※直接使用する関数
template<class T>
inline void rotateValues(T* val1, T* val2, int step)
{
	std::conditional<std::is_arithmetic<T>::value,
		_private::_rotateArithmetic<T>,
		_private::_rotateObjects<T>
	>::type::exec(val1, val2, step);
}

//----------------------------------------
//値のローテーション
//※イテレータ対応版
//※直接使用する関数
template<class ITERATOR>
inline void iteratorRotateValues(ITERATOR val1, ITERATOR val2, typename ITERATOR::difference_type step)
{
	typedef typename ITERATOR::value_type value_type;
#ifdef GASHA_SWAP_VALUES_USE_MEMCPY//コンストラクタ／オペレータの呼び出しを避けて単純なメモリコピー
	char tmp[sizeof(value_type)];
	memcpy(tmp, &*val2, sizeof(value_type));
	while (val1 != val2)
	{
		ITERATOR val2_prev = val2 - step;
		memcpy(&*val2, &*val2_prev, sizeof(value_type));
		val2 = val2_prev;
	}
	memcpy(&*val1, tmp, sizeof(value_type));
#else//GASHA_SWAP_VALUES_USE_MEMCPY//ムーブコンストラクタとムーブオペレータを使用して入れ替え
	value_type tmp = std::move(*val2);
	while (val1 != val2)
	{
		ITERATOR val2_prev = val2 - step;
		*val2 = std::move(*val2_prev);
		val2 = val2_prev;
	}
	*val1 = std::move(tmp);
#endif//GASHA_SWAP_VALUES_USE_MEMCPY
}

//--------------------------------------------------------------------------------
//時間
//--------------------------------------------------------------------------------

//----------------------------------------
//経過時間を算出
inline std::chrono::system_clock::time_point nowTime()
{
	return std::chrono::system_clock::now();
}
inline double calcElapsedTime(const std::chrono::system_clock::time_point begin, const std::chrono::system_clock::time_point end)
{
	const auto duration = end - begin;
	const auto duration_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
	const auto duration_count = duration_nano.count();
	const double second = static_cast<double>(duration_count) / 1000000000.;
	return second;
}
inline double calcElapsedTime(const std::chrono::system_clock::time_point begin)
{
	return calcElapsedTime(begin, nowTime());
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_UTILITY_H

// End of file

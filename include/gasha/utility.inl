﻿#pragma once
#ifndef GASHA_INCLUDED_UTILITY_INL
#define GASHA_INCLUDED_UTILITY_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// utility.inl
// 汎用ユーティリティ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/utility.h>//汎用ユーティリティ【宣言部】

#include <type_traits>//c++11 std::conditional

#ifdef GASHA_SWAP_VALUES_USE_MEMCPY
#include <cstring>//std::memcpy()
#endif//GASHA_SWAP_VALUES_USE_MEMCPY

#include <utility>//C++11 std::move, std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//最大値／最小値
//--------------------------------------------------------------------------------

//----------------------------------------
//最大値取得
//----------------------------------------

//値が二つの max()
template<typename T1, typename T2>
inline constexpr T1 max(const T1 n1, const T2 n2){ return static_cast<T1>(n2) < n1 ? n1 : static_cast<T1>(n2); }
template<typename T>
inline constexpr const T& maxObj(const T& obj1, const T& obj2){ return obj2 < obj1 ? obj1 : obj2; }
template<typename T>
inline constexpr T& maxObj(T&& obj1, T&& obj2){ return obj2 < obj1 ? obj1 : obj2; }

//値が三つ以上の max() : 再帰処理
template<typename T1, typename T2, typename T3, typename... Tx>
inline constexpr T1 max(const T1 n1, const T2 n2, const T3 n3, const Tx... nx){ return GASHA_ max(GASHA_ max(n1, n2), n3, nx...); }
template<typename T1, typename T2, typename T3, typename... Tx>
inline constexpr const T1& maxObj(const T1& obj1, const T2& obj2, const T3& obj3, const Tx&... nx){ return GASHA_ maxObj(GASHA_ maxObj(obj1, obj2), obj3, nx...); }
template<typename T1, typename T2, typename T3, typename... Tx>
inline constexpr T1& maxObj(T1&& obj1, T2&& obj2, T3&& obj3, Tx&&... nx){ return GASHA_ maxObj(GASHA_ maxObj(obj1, obj2), obj3, std::forward<Tx>(nx)...); }

//値が二つの max()
//ただし、n1 が NULL_VALUE なら、無条件で n2 を返す
template<typename T1, typename T2>
inline constexpr T1 maxIf(const T1 n1, const T2 n2, const T1 null_value){ return n1 == null_value ? static_cast<T1>(n2) : max(n1, n2); }

//----------------------------------------
//最小値取得
//----------------------------------------

//値が二つの min()
template<typename T1, typename T2>
inline constexpr T1 min(const T1 n1, const T2 n2){ return n1 < static_cast<T1>(n2) ? n1 : static_cast<T1>(n2); }
template<typename T>
inline constexpr const T& minObj(const T& obj1, const T& obj2){ return obj1 < obj2 ? obj1 : obj2; }
template<typename T>
inline constexpr T& minObj(T&& obj1, T&& obj2){ return obj1 < obj2 ? obj1 : obj2; }

//値が三つ以上の min() : 再帰処理
template<typename T1, typename T2, typename T3, typename... Tx>
inline constexpr T1 min(const T1 n1, const T2 n2, const T3 n3, const Tx... nx){ return GASHA_ min(GASHA_ min(n1, n2), n3, nx...); }
template<typename T1, typename T2, typename T3, typename... Tx>
inline constexpr const T1& minObj(const T1& obj1, const T2& obj2, const T3& obj3, const Tx&... nx){ return GASHA_ minObj(GASHA_ minObj(obj1, obj2), obj3, nx...); }
template<typename T1, typename T2, typename T3, typename... Tx>
inline constexpr T1& minObj(T1&& obj1, T2&& obj2, T3&& obj3, Tx&&... nx){ return GASHA_ minObj(GASHA_ minObj(obj1, obj2), obj3, std::forward<Tx>(nx)...); }

//値が二つの min()
//ただし、n1 が NULL_VALUE なら、無条件で n2 を返す
template<typename T1, typename T2>
inline constexpr T1 minIf(const T1 n1, const T2 n2, const T1 null_value){ return n1 == null_value ? static_cast<T1>(n2) : min(n1, n2); }

//----------------------------------------
//0以外の値を取得
//----------------------------------------

//値が一つの nvl()
template<typename T>
inline constexpr const T* nvl(const T* n1){ return n1 != nullptr ? n1 : "(null)"; }

//値が二つの nvl()
template<typename T>
inline constexpr const T* nvl(const T* n1, const T* n2){ return n1 != nullptr ? n1 : n2; }
template<typename T>
inline constexpr T* nvl(T* n1, T* n2){ return n1 != nullptr ? n1 : n2; }

//値が三つ以上の nvl() : 再帰処理
template<typename T, typename... Tx>
inline constexpr const T* nvl(const T* n1, const T* n2, const T* n3, const Tx&... nx){ return n1 != nullptr ? n1 : GASHA_ nvl(n2, n3, nx...); }
template<typename T, typename... Tx>
inline constexpr T* nvl(T* n1, T* n2, T* n3, Tx&&... nx){ return n1 != nullptr ? n1 : GASHA_ nvl(n2, n3, std::forward<Tx>(nx)...); }

//--------------------------------------------------------------------------------
//値の交換
//--------------------------------------------------------------------------------

namespace _private
{
	//----------------------------------------
	//型に応じた処理クラス
	template<class T>
	struct swapArithmetic{
		inline static void exec(T& val1, T& val2)
		{
			T tmp;
			tmp = val2;
			val2 = val1;
			val1 = tmp;
		}
	};
	template<class T>
	struct swapArithmetic<T*>{
		inline static void exec(T*& val1, T*& val2)
		{
			T* tmp;
			tmp = val2;
			val2 = val1;
			val1 = tmp;
		}
	};
	template<class T>
	struct swapObjects{
		inline static void exec(T& val1, T& val2)
		{
		#ifdef GASHA_SWAP_VALUES_USE_MEMCPY//コンストラクタ／オペレータの呼び出しを避けて単純なメモリコピー
			char tmp[sizeof(T)];
			std::memcpy(tmp, &val2, sizeof(T));
			std::memcpy(&val2, &val1, sizeof(T));
			std::memcpy(&val1, tmp, sizeof(T));
		#else//GASHA_SWAP_VALUES_USE_MEMCPY//ムーブコンストラクタとムーブオペレータを使用して入れ替え（#include <utility> の std::swap() と同じ）
			T tmp = std::move(val2);
			val2 = std::move(val1);
			val1 = std::move(tmp);
		#endif//GASHA_SWAP_VALUES_USE_MEMCPY
		}
	};
	template<class T>
	struct swapObjects<T*>{
		inline static void exec(T*& val1, T*& val2)
		{
			swapArithmetic<T*>::exec(val1, val2);
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
		_private::swapArithmetic<T>,
		_private::swapObjects<T>
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
	std::memcpy(tmp, &*val2, sizeof(value_type));
	std::memcpy(&*val2, &*val1, sizeof(value_type));
	std::memcpy(&*val1, tmp, sizeof(value_type));
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
	struct rotateArithmetic{
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
	struct rotateArithmetic<T*>{
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
	struct rotateObjects{
		inline static void exec(T* val1, T* val2, int step)
		{
		#ifdef GASHA_SWAP_VALUES_USE_MEMCPY//コンストラクタ／オペレータの呼び出しを避けて単純なメモリコピー
			char tmp[sizeof(T)];
			std::memcpy(tmp, val2, sizeof(T));
			while (val1 != val2)
			{
				T* val2_prev = val2 - step;
				std::memcpy(val2, val2_prev, sizeof(T));
				val2 = val2_prev;
			}
			std::memcpy(val1, tmp, sizeof(T));
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
	struct rotateObjects<T*>{
		inline static void exec(T** val1, T** val2, int step)
		{
			rotateArithmetic<T*>::exec(val1, val2, step);
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
		_private::rotateArithmetic<T>,
		_private::rotateObjects<T>
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
	std::memcpy(tmp, &*val2, sizeof(value_type));
	while (val1 != val2)
	{
		ITERATOR val2_prev = val2 - step;
		std::memcpy(&*val2, &*val2_prev, sizeof(value_type));
		val2 = val2_prev;
	}
	std::memcpy(&*val1, tmp, sizeof(value_type));
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

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_UTILITY_H

// End of file

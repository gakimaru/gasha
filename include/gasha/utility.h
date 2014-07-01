#pragma once
#ifndef GASHA_INCLUDED_UTILITY_H
#define GASHA_INCLUDED_UTILITY_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ】
// utility.h
// 汎用ユーティリティ
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <type_traits>//c++11 std::conditional

#ifdef GASHA_SWAP_VALUES_USE_MEMCPY
#include <memory.h>//memcpy()
#endif//GASHA_SWAP_VALUES_USE_MEMCPY

#include <utility>//C++11 std::move, std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//最大値／最小値
//--------------------------------------------------------------------------------

//※max(), min()は、プリミティブ型専用。戻り値に値を返す。引数の型がそれぞれ異なる場合、第一引数の型にキャストされる。
//※maxObj(), minObj()は、オブジェクトに対応。戻り値に、いずれかの引数の参照を返す。全ての引数の型が一致している必要がある。なお、型は operator<() を実装している必要がある。

//----------------------------------------
//最大値取得
//----------------------------------------

//値が二つの max()
template<typename T1, typename T2>
inline T1 max(const T1& n1, const T2& n2){ return static_cast<T1>(n2) < n1 ? n1 : static_cast<T1>(n2); }
template<typename T>
inline const T& maxObj(const T& obj1, const T& obj2){ return obj2 < obj1 ? obj1 : obj2; }
template<typename T>
inline T& maxObj(T&& obj1, T&& obj2){ return obj2 < obj1 ? obj1 : obj2; }

//値が三つ以上の max() : 再帰処理
//※テンプレートの特殊化ではなく、関数のオーバーロードで再起を終結させる
template<typename T1, typename T2, typename T3, typename... Tx>
inline T1 max(const T1& n1, const T2& n2, const T3& n3, const Tx&... nx){ return GASHA_ max(GASHA_ max(n1, n2), n3, nx...); }//nxが空になったら、引数が二つの関数が呼ばれる
template<typename T1, typename T2, typename T3, typename... Tx>
inline const T1& maxObj(const T1& obj1, const T2& obj2, const T3& obj3, const Tx&... nx){ return GASHA_ maxObj(GASHA_ maxObj(obj1, obj2), obj3, nx...); }//nxが空になったら、引数が二つの関数が呼ばれる
template<typename T1, typename T2, typename T3, typename... Tx>
inline T1& maxObj(T1&& obj1, T2&& obj2, T3&& obj3, Tx&&... nx){ return GASHA_ maxObj(GASHA_ maxObj(obj1, obj2), obj3, std::forward<Tx>(nx)...); }//nxが空になったら、引数が二つの関数が呼ばれる

//----------------------------------------
//最小値取得
//----------------------------------------

//値が二つの min()
template<typename T1, typename T2>
inline T1 min(const T1& n1, const T2& n2){ return n1 < static_cast<T1>(n2) ? n1 : static_cast<T1>(n2); }
template<typename T>
inline const T& minObj(const T& obj1, const T& obj2){ return obj1 < obj2 ? obj1 : obj2; }
template<typename T>
inline T& minObj(T&& obj1, T&& obj2){ return obj1 < obj2 ? obj1 : obj2; }

//値が三つ以上の min() : 再帰処理
//※テンプレートの特殊化ではなく、関数のオーバーロードで再起を終結させる
template<typename T1, typename T2, typename T3, typename... Tx>
inline T1 min(const T1& n1, const T2& n2, const T3& n3, const Tx&... nx){ return GASHA_ min(GASHA_ min(n1, n2), n3, nx...); }//nxが空になったら、引数が二つの関数が呼ばれる
template<typename T1, typename T2, typename T3, typename... Tx>
inline const T1& minObj(const T1& obj1, const T2& obj2, const T3& obj3, const Tx&... nx){ return GASHA_ minObj(GASHA_ minObj(obj1, obj2), obj3, nx...); }//nxが空になったら、引数が二つの関数が呼ばれる
template<typename T1, typename T2, typename T3, typename... Tx>
inline T1& minObj(T1&& obj1, T2&& obj2, T3&& obj3, Tx&&... nx){ return GASHA_ minObj(GASHA_ minObj(obj1, obj2), obj3, std::forward<Tx>(nx)...); }//nxが空になったら、引数が二つの関数が呼ばれる

//--------------------------------------------------------------------------------
//値の交換
//--------------------------------------------------------------------------------

//----------------------------------------
//データ型に応じた処理クラス
//----------------------------------------
//※直接使用しない
//----------------------------------------
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
		GASHA_ _swapArithmetic<T*>::exec(val1, val2);
	}
};

//----------------------------------------
//値の交換
//----------------------------------------
//※直接使用する関数
//----------------------------------------
template<class T>
inline void swapValues(T& val1, T& val2)
{
	std::conditional<std::is_arithmetic<T>::value,
		GASHA_ _swapArithmetic<T>,
		GASHA_ _swapObjects<T>
	>::type::exec(val1, val2);
}

//----------------------------------------
//値の交換
//----------------------------------------
//※イテレータ対応版
//※直接使用する関数
//----------------------------------------
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

//----------------------------------------
//データ型に応じた処理クラス
//----------------------------------------
//※直接使用しない
//----------------------------------------
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
		GASHA_ _rotateArithmetic<T*>::exec(val1, val2, step);
	}
};

//----------------------------------------
//値のローテーション
//----------------------------------------
//※直接使用する関数
//----------------------------------------
template<class T>
inline void rotateValues(T* val1, T* val2, int step)
{
	std::conditional<std::is_arithmetic<T>::value,
		GASHA_ _rotateArithmetic<T>,
		GASHA_ _rotateObjects<T>
	>::type::exec(val1, val2, step);
}

//----------------------------------------
//値のローテーション
//----------------------------------------
//※イテレータ対応版
//※直接使用する関数
//----------------------------------------
//データのローテーション
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

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_UTILITY_H

// End of file

#pragma once
#ifndef GASHA_INCLUDED_FAST_MATH_H
#define GASHA_INCLUDED_FAST_MATH_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// fast_math.h
// 高速算術用【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#ifdef GASHA_USE_SSE
#include <xmmintrin.h>//SSE1
#endif//GASHA_USE_SSE

#ifdef GASHA_USE_SSE2
#include <emmintrin.h>//SSE2
#endif//GASHA_USE_SSE2

#ifdef GASHA_USE_AVX
#include <immintrin.h>//AVX
#endif//GASHA_USE_AVX

#include <utility>//C++11 std::move

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//高速演算
//--------------------------------------------------------------------------------

//クラス宣言
template <typename T>
struct fastArith;
template <typename T>
struct fastestArith;
template <typename T>
struct semifastArith;
template <typename T>
struct sseArith;
template <typename T>
struct normalArith;
template <typename T>
struct dummyArith;

//定義用マクロ
#define GASHA_FAST_ARITH_CLASS(CLASS_NAME) \
	template <typename T> \
	struct CLASS_NAME \
	{ \
		typedef T value_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastArith<value_type> fastArith_type; \
		typedef fastestArith<value_type> fastestArith_type; \
		typedef semifastArith<value_type> semifastArith_type; \
		typedef sseArith<value_type> sseArith_type; \
		typedef normalArith<value_type> normalArith_type; \
		typedef dummyArith<value_type> dummyArith_type; \
		value_type m_val; \
		inline operator value_type() const { return m_val; } \
		inline value_type operator*() const { return m_val; } \
		inline CLASS_NAME& operator=(const value_type val); \
		inline CLASS_NAME& operator=(const fastArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastArith<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestArith<value_type>& val); \
		inline CLASS_NAME& operator=(const semifastArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const semifastArith<value_type>& val); \
		inline CLASS_NAME& operator=(const sseArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseArith<value_type>& val); \
		inline CLASS_NAME& operator=(const normalArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const normalArith<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyArith<value_type>& val); \
		inline CLASS_NAME(const value_type val); \
		inline CLASS_NAME(const fastArith<value_type>&& val); \
		inline CLASS_NAME(const fastArith<value_type>& val); \
		inline CLASS_NAME(const fastestArith<value_type>&& val); \
		inline CLASS_NAME(const fastestArith<value_type>& val); \
		inline CLASS_NAME(const semifastArith<value_type>&& val); \
		inline CLASS_NAME(const semifastArith<value_type>& val); \
		inline CLASS_NAME(const sseArith<value_type>&& val); \
		inline CLASS_NAME(const sseArith<value_type>& val); \
		inline CLASS_NAME(const normalArith<value_type>&& val); \
		inline CLASS_NAME(const normalArith<value_type>& val); \
		inline CLASS_NAME(const dummyArith<value_type>&& val); \
		inline CLASS_NAME(const dummyArith<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_FLOAT(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<float> \
	{ \
		typedef float value_type; \
		typedef __m128 sse_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastArith<value_type> fastArith_type; \
		typedef fastestArith<value_type> fastestArith_type; \
		typedef semifastArith<value_type> semifastArith_type; \
		typedef sseArith<value_type> sseArith_type; \
		typedef normalArith<value_type> normalArith_type; \
		typedef dummyArith<value_type> dummyArith_type; \
		sse_type m_val; \
		inline operator value_type() const { value_type val; _mm_store_ss(&val, m_val); return val; } \
		inline operator sse_type() const { return m_val; } \
		inline value_type operator*() const { return static_cast<value_type>(*this); } \
		inline CLASS_NAME& operator=(const value_type val); \
		inline CLASS_NAME& operator=(const sse_type&& val); \
		inline CLASS_NAME& operator=(const sse_type& val); \
		inline CLASS_NAME& operator=(const fastArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastArith<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestArith<value_type>& val); \
		inline CLASS_NAME& operator=(const semifastArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const semifastArith<value_type>& val); \
		inline CLASS_NAME& operator=(const sseArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseArith<value_type>& val); \
		inline CLASS_NAME& operator=(const normalArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const normalArith<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyArith<value_type>& val); \
		inline CLASS_NAME(const value_type val); \
		inline CLASS_NAME(const sse_type&& val); \
		inline CLASS_NAME(const sse_type& val); \
		inline CLASS_NAME(const fastArith<value_type>&& val); \
		inline CLASS_NAME(const fastArith<value_type>& val); \
		inline CLASS_NAME(const fastestArith<value_type>&& val); \
		inline CLASS_NAME(const fastestArith<value_type>& val); \
		inline CLASS_NAME(const semifastArith<value_type>&& val); \
		inline CLASS_NAME(const semifastArith<value_type>& val); \
		inline CLASS_NAME(const sseArith<value_type>&& val); \
		inline CLASS_NAME(const sseArith<value_type>& val); \
		inline CLASS_NAME(const normalArith<value_type>&& val); \
		inline CLASS_NAME(const normalArith<value_type>& val); \
		inline CLASS_NAME(const dummyArith<value_type>&& val); \
		inline CLASS_NAME(const dummyArith<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_DOUBLE(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<double> \
	{ \
		typedef double value_type; \
		typedef __m128d sse_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastArith<value_type> fastArith_type; \
		typedef fastestArith<value_type> fastestArith_type; \
		typedef semifastArith<value_type> semifastArith_type; \
		typedef sseArith<value_type> sseArith_type; \
		typedef normalArith<value_type> normalArith_type; \
		typedef dummyArith<value_type> dummyArith_type; \
		sse_type m_val; \
		inline operator value_type() const { value_type val; _mm_store_sd(&val, m_val); return val; } \
		inline operator sse_type() const { return m_val; } \
		inline value_type operator*() const { return static_cast<value_type>(*this); } \
		inline CLASS_NAME& operator=(const value_type val); \
		inline CLASS_NAME& operator=(const sse_type&& val); \
		inline CLASS_NAME& operator=(const sse_type& val); \
		inline CLASS_NAME& operator=(const fastArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastArith<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestArith<value_type>& val); \
		inline CLASS_NAME& operator=(const semifastArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const semifastArith<value_type>& val); \
		inline CLASS_NAME& operator=(const sseArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseArith<value_type>& val); \
		inline CLASS_NAME& operator=(const normalArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const normalArith<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyArith<value_type>& val); \
		inline CLASS_NAME(const value_type val); \
		inline CLASS_NAME(const sse_type&& val); \
		inline CLASS_NAME(const sse_type& val); \
		inline CLASS_NAME(const fastArith<value_type>&& val); \
		inline CLASS_NAME(const fastArith<value_type>& val); \
		inline CLASS_NAME(const fastestArith<value_type>&& val); \
		inline CLASS_NAME(const fastestArith<value_type>& val); \
		inline CLASS_NAME(const semifastArith<value_type>&& val); \
		inline CLASS_NAME(const semifastArith<value_type>& val); \
		inline CLASS_NAME(const sseArith<value_type>&& val); \
		inline CLASS_NAME(const sseArith<value_type>& val); \
		inline CLASS_NAME(const normalArith<value_type>&& val); \
		inline CLASS_NAME(const normalArith<value_type>& val); \
		inline CLASS_NAME(const dummyArith<value_type>&& val); \
		inline CLASS_NAME(const dummyArith<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_M128(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<__m128> \
	{ \
		typedef __m128 value_type; \
		typedef __m128 sse_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastArith<value_type> fastArith_type; \
		typedef fastestArith<value_type> fastestArith_type; \
		typedef semifastArith<value_type> semifastArith_type; \
		typedef sseArith<value_type> sseArith_type; \
		typedef normalArith<value_type> normalArith_type; \
		typedef dummyArith<value_type> dummyArith_type; \
		value_type m_val; \
		inline operator value_type() const { return m_val; } \
		inline value_type operator*() const { return static_cast<value_type>(*this); } \
		inline CLASS_NAME& operator=(const value_type&& val); \
		inline CLASS_NAME& operator=(const value_type& val); \
		inline CLASS_NAME& operator=(const fastArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastArith<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestArith<value_type>& val); \
		inline CLASS_NAME& operator=(const semifastArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const semifastArith<value_type>& val); \
		inline CLASS_NAME& operator=(const sseArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseArith<value_type>& val); \
		inline CLASS_NAME& operator=(const normalArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const normalArith<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyArith<value_type>& val); \
		inline CLASS_NAME(const value_type&& val); \
		inline CLASS_NAME(const value_type& val); \
		inline CLASS_NAME(const fastArith<value_type>&& val); \
		inline CLASS_NAME(const fastArith<value_type>& val); \
		inline CLASS_NAME(const fastestArith<value_type>&& val); \
		inline CLASS_NAME(const fastestArith<value_type>& val); \
		inline CLASS_NAME(const semifastArith<value_type>&& val); \
		inline CLASS_NAME(const semifastArith<value_type>& val); \
		inline CLASS_NAME(const sseArith<value_type>&& val); \
		inline CLASS_NAME(const sseArith<value_type>& val); \
		inline CLASS_NAME(const normalArith<value_type>&& val); \
		inline CLASS_NAME(const normalArith<value_type>& val); \
		inline CLASS_NAME(const dummyArith<value_type>&& val); \
		inline CLASS_NAME(const dummyArith<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_M128D(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<__m128d> \
	{ \
		typedef __m128d value_type; \
		typedef __m128d sse_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastArith<value_type> fastArith_type; \
		typedef fastestArith<value_type> fastestArith_type; \
		typedef semifastArith<value_type> semifastArith_type; \
		typedef sseArith<value_type> sseArith_type; \
		typedef normalArith<value_type> normalArith_type; \
		typedef dummyArith<value_type> dummyArith_type; \
		value_type m_val; \
		inline operator value_type() const { return m_val; } \
		inline value_type operator*() const { return static_cast<value_type>(*this); } \
		inline CLASS_NAME& operator=(const value_type&& val); \
		inline CLASS_NAME& operator=(const value_type& val); \
		inline CLASS_NAME& operator=(const fastArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastArith<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestArith<value_type>& val); \
		inline CLASS_NAME& operator=(const semifastArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const semifastArith<value_type>& val); \
		inline CLASS_NAME& operator=(const sseArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseArith<value_type>& val); \
		inline CLASS_NAME& operator=(const normalArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const normalArith<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyArith<value_type>& val); \
		inline CLASS_NAME(const value_type&& val); \
		inline CLASS_NAME(const value_type& val); \
		inline CLASS_NAME(const fastArith<value_type>&& val); \
		inline CLASS_NAME(const fastArith<value_type>& val); \
		inline CLASS_NAME(const fastestArith<value_type>&& val); \
		inline CLASS_NAME(const fastestArith<value_type>& val); \
		inline CLASS_NAME(const semifastArith<value_type>&& val); \
		inline CLASS_NAME(const semifastArith<value_type>& val); \
		inline CLASS_NAME(const sseArith<value_type>&& val); \
		inline CLASS_NAME(const sseArith<value_type>& val); \
		inline CLASS_NAME(const normalArith<value_type>&& val); \
		inline CLASS_NAME(const normalArith<value_type>& val); \
		inline CLASS_NAME(const dummyArith<value_type>&& val); \
		inline CLASS_NAME(const dummyArith<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_M256(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<__m256> \
	{ \
		typedef __m256 value_type; \
		typedef __m256 sse_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastArith<value_type> fastArith_type; \
		typedef fastestArith<value_type> fastestArith_type; \
		typedef semifastArith<value_type> semifastArith_type; \
		typedef sseArith<value_type> sseArith_type; \
		typedef normalArith<value_type> normalArith_type; \
		typedef dummyArith<value_type> dummyArith_type; \
		value_type m_val; \
		inline operator value_type() const { return m_val; } \
		inline value_type operator*() const { return static_cast<value_type>(*this); } \
		inline CLASS_NAME& operator=(const value_type&& val); \
		inline CLASS_NAME& operator=(const value_type& val); \
		inline CLASS_NAME& operator=(const fastArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastArith<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestArith<value_type>& val); \
		inline CLASS_NAME& operator=(const semifastArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const semifastArith<value_type>& val); \
		inline CLASS_NAME& operator=(const sseArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseArith<value_type>& val); \
		inline CLASS_NAME& operator=(const normalArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const normalArith<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyArith<value_type>& val); \
		inline CLASS_NAME(const value_type&& val); \
		inline CLASS_NAME(const value_type& val); \
		inline CLASS_NAME(const fastArith<value_type>&& val); \
		inline CLASS_NAME(const fastArith<value_type>& val); \
		inline CLASS_NAME(const fastestArith<value_type>&& val); \
		inline CLASS_NAME(const fastestArith<value_type>& val); \
		inline CLASS_NAME(const semifastArith<value_type>&& val); \
		inline CLASS_NAME(const semifastArith<value_type>& val); \
		inline CLASS_NAME(const sseArith<value_type>&& val); \
		inline CLASS_NAME(const sseArith<value_type>& val); \
		inline CLASS_NAME(const normalArith<value_type>&& val); \
		inline CLASS_NAME(const normalArith<value_type>& val); \
		inline CLASS_NAME(const dummyArith<value_type>&& val); \
		inline CLASS_NAME(const dummyArith<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_M256D(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<__m256d> \
	{ \
		typedef __m256d value_type; \
		typedef __m256d sse_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastArith<value_type> fastArith_type; \
		typedef fastestArith<value_type> fastestArith_type; \
		typedef semifastArith<value_type> semifastArith_type; \
		typedef sseArith<value_type> sseArith_type; \
		typedef normalArith<value_type> normalArith_type; \
		typedef dummyArith<value_type> dummyArith_type; \
		value_type m_val; \
		inline operator value_type() const { return m_val; } \
		inline value_type operator*() const { return static_cast<value_type>(*this); } \
		inline CLASS_NAME& operator=(const value_type&& val); \
		inline CLASS_NAME& operator=(const value_type& val); \
		inline CLASS_NAME& operator=(const fastArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastArith<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestArith<value_type>& val); \
		inline CLASS_NAME& operator=(const semifastArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const semifastArith<value_type>& val); \
		inline CLASS_NAME& operator=(const sseArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseArith<value_type>& val); \
		inline CLASS_NAME& operator=(const normalArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const normalArith<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyArith<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyArith<value_type>& val); \
		inline CLASS_NAME(const value_type&& val); \
		inline CLASS_NAME(const value_type& val); \
		inline CLASS_NAME(const fastArith<value_type>&& val); \
		inline CLASS_NAME(const fastArith<value_type>& val); \
		inline CLASS_NAME(const fastestArith<value_type>&& val); \
		inline CLASS_NAME(const fastestArith<value_type>& val); \
		inline CLASS_NAME(const semifastArith<value_type>&& val); \
		inline CLASS_NAME(const semifastArith<value_type>& val); \
		inline CLASS_NAME(const sseArith<value_type>&& val); \
		inline CLASS_NAME(const sseArith<value_type>& val); \
		inline CLASS_NAME(const normalArith<value_type>&& val); \
		inline CLASS_NAME(const normalArith<value_type>& val); \
		inline CLASS_NAME(const dummyArith<value_type>&& val); \
		inline CLASS_NAME(const dummyArith<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};

#ifdef GASHA_FAST_ARITH_USE_SSE
#define GASHA_FAST_ARITH_CLASS_SSE(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_SSE_FLOAT(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_M128(CLASS_NAME);
#else//GASHA_FAST_ARITH_USE_SSE
#define GASHA_FAST_ARITH_CLASS_SSE(CLASS_NAME)
#endif//GASHA_FAST_ARITH_USE_SSE

#ifdef GASHA_FAST_ARITH_USE_SSE2
#define GASHA_FAST_ARITH_CLASS_SSE2(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_SSE_DOUBLE(CLASS_NAME); \
GASHA_FAST_ARITH_CLASS_SSE_M128D(CLASS_NAME);
#else//GASHA_FAST_ARITH_USE_SSE2
#define GASHA_FAST_ARITH_CLASS_SSE2(CLASS_NAME)
#endif//GASHA_FAST_ARITH_USE_SSE2

#ifdef GASHA_FAST_ARITH_USE_AVX
#define GASHA_FAST_ARITH_CLASS_AVX(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_SSE_M256(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_M256D(CLASS_NAME);
#else//GASHA_FAST_ARITH_USE_SSE
#define GASHA_FAST_ARITH_CLASS_AVX(CLASS_NAME)
#endif//GASHA_FAST_ARITH_USE_SSE

#define GASHA_FAST_ARITH_CLASS_SET(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE2(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_AVX(CLASS_NAME);

//----------------------------------------
//高速演算クラス
//※データ型に基づき、適切な高速演算を適用する。
//※通常の演算に比べて、精度が若干落ちる。
//※float型／__m128型の場合、高速化のために、除算に逆数を用いる。
//　逆数は、近似値を元にニュートン法を1回適用して精度を高める。
//※その他、できる限りSSE命令を用いる。
//※内部でSSE型を用いるなどするため、データサイズが大きくなることがある。
//※SSE型のまま四則演算するので、演算を連続で行った時に高速になる。
GASHA_FAST_ARITH_CLASS_SET(fastArith);
//----------------------------------------
//最高速演算クラス
//※データ型に基づき、適切な高速演算を適用する。
//※通常の演算に比べて、精度が悪い。
//※float型／__m128型の場合、高速化のために、除算に逆数を用いる。
//　逆数は、近似値をそのまま使用する。
//※その他、できる限りSSE命令を用いる。
//※内部でSSE型を用いるなどするため、データサイズが大きくなることがある。
//※SSE型のまま四則演算するので、演算を連続で行った時に高速になる。
GASHA_FAST_ARITH_CLASS_SET(fastestArith);
//----------------------------------------
//準高速演算クラス
//※データ型に基づき、適切な高速演算を適用する。
//※通常の演算に比べて、精度が若干落ちるが、可能な限り高めるようにする。
//※float型／__m128型の場合、高速化のために、除算に逆数を用いる。
//　逆数は、近似値を元にニュートン法を2回適用して精度を高める。
//※その他、できる限りSSE命令を用いる。
//※内部でSSE型を用いるなどするため、データサイズが大きくなることがある。
//※SSE型のまま四則演算するので、演算を連続で行った時に高速になる。
GASHA_FAST_ARITH_CLASS_SET(semifastArith);
//----------------------------------------
//SSE高速演算クラス
//※データ型に基づき、適切な高速演算を適用する。
//※できる限りSSE命令を用いる。逆数を用いた除算は行わない。
//※内部でSSE型を用いるなどするため、データサイズが大きくなることがある。
//※SSE型のまま四則演算するので、演算を連続で行った時に高速になる。
GASHA_FAST_ARITH_CLASS_SET(sseArith);
//----------------------------------------
//通常演算クラス
//※そのままのデータ型で普通の演算を行う。
//※SSE命令型の場合はSSE命令を用いる。
GASHA_FAST_ARITH_CLASS_SET(normalArith);
//----------------------------------------
//ダミー演算クラス
//※何もせず、元の値の参照を扱う。元の値のままとして処理する。
//※最適化により、クラスを通さず直接値を扱うのと同等になる。
//※他の演算クラスと挙動やパフォーマンスの違いを確認したい時に用いる。
//※他の演算クラスとインターフェースが異なるので注意。
template <typename T>
struct dummyArith
{
	typedef T value_type;
	typedef value_type result_type;
	typedef fastArith<value_type> fastArith_type;
	typedef fastestArith<value_type> fastestArith_type;
	typedef semifastArith<value_type> semifastArith_type;
	typedef sseArith<value_type> sseArith_type;
	typedef normalArith<value_type> normalArith_type;
	typedef dummyArith<value_type> dummyArith_type;
	const value_type& m_val;
	inline operator value_type() const { return m_val; }
	value_type operator*() const { return m_val; }
	inline dummyArith(const value_type&& val) : m_val(val){}
	inline dummyArith(const value_type& val) : m_val(val){}
	dummyArith() = delete;
	inline ~dummyArith(){}
};

//----------------------------------------
//高速演算クラスの別名
using fastArith_f = fastArith<float>;
using fastArith_d = fastArith<double>;
using fastestArith_f = fastestArith<float>;
using fastestArith_d = fastestArith<double>;
using semifastArith_f = semifastArith<float>;
using semifastArith_d = semifastArith<double>;
using sseArith_f = sseArith<float>;
using sseArith_d = sseArith<double>;
using normalArith_f = normalArith<float>;
using normalArith_d = normalArith<double>;
using dummyArith_f = dummyArith<float>;
using dummyArith_d = dummyArith<double>;

//----------------------------------------
//高速演算
#define GASHA_FAST_ARITH_OPERATOR(CLASS_NAME) \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>&& lvalue, const fastArith<T>& rvalue){ return std::move(lvalue) + std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) + std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const fastArith<T>& rvalue){ return std::move(lvalue) + std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>&& lvalue, const fastestArith<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const fastestArith<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>&& lvalue, const semifastArith<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const semifastArith<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>&& lvalue, const sseArith<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const sseArith<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>&& lvalue, const normalArith<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const normalArith<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>&& lvalue, const dummyArith<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const dummyArith<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>&& lvalue, const T rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const T rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline T operator+(const T lvalue, const CLASS_NAME<T>&& rvalue){ return CLASS_NAME<T>(lvalue) +std::move(rvalue); } \
	template<class T> inline T operator+(const T lvalue, const CLASS_NAME<T>& rvalue){ return CLASS_NAME<T>(lvalue) +std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>&& lvalue, const fastArith<T>& rvalue){ return std::move(lvalue) - std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) - std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const fastArith<T>& rvalue){ return std::move(lvalue) - std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>&& lvalue, const fastestArith<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const fastestArith<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>&& lvalue, const semifastArith<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const semifastArith<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>&& lvalue, const sseArith<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const sseArith<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>&& lvalue, const normalArith<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const normalArith<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>&& lvalue, const dummyArith<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const dummyArith<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>&& lvalue, const T rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const T rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline T operator-(const T lvalue, const CLASS_NAME<T>&& rvalue){ return CLASS_NAME<T>(lvalue) - std::move(rvalue); } \
	template<class T> inline T operator-(const T lvalue, const CLASS_NAME<T>& rvalue){ return CLASS_NAME<T>(lvalue) - std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>&& lvalue, const fastArith<T>& rvalue){ return std::move(lvalue) * std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) * std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const fastArith<T>& rvalue){ return std::move(lvalue) * std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>&& lvalue, const fastestArith<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const fastestArith<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>&& lvalue, const semifastArith<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const semifastArith<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>&& lvalue, const sseArith<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const sseArith<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>&& lvalue, const normalArith<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const normalArith<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>&& lvalue, const dummyArith<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const dummyArith<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>&& lvalue, const T rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const T rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline T operator*(const T lvalue, const CLASS_NAME<T>&& rvalue){ return CLASS_NAME<T>(lvalue) * std::move(rvalue); } \
	template<class T> inline T operator*(const T lvalue, const CLASS_NAME<T>& rvalue){ return CLASS_NAME<T>(lvalue) * std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>&& lvalue, const fastArith<T>& rvalue){ return std::move(lvalue) / std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) / std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const fastArith<T>& rvalue){ return std::move(lvalue) / std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>&& lvalue, const fastestArith<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const fastestArith<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>&& lvalue, const semifastArith<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const semifastArith<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>&& lvalue, const sseArith<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const sseArith<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>&& lvalue, const normalArith<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const normalArith<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>&& lvalue, const dummyArith<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const dummyArith<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>&& lvalue, const T rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const T rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline T operator/(const T lvalue, const CLASS_NAME<T>&& rvalue){ return CLASS_NAME<T>(lvalue) / std::move(rvalue); } \
	template<class T> inline T operator/(const T lvalue, const CLASS_NAME<T>& rvalue){ return CLASS_NAME<T>(lvalue) / std::move(rvalue); } \

template<class T> inline fastArith<T> operator+(const fastArith<T>&& lvalue, const fastArith<T>&& rvalue){ return fastArith<T>(*lvalue + *rvalue); }
template<class T> inline fastArith<T> operator+(const fastArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) + fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator+(const fastArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) + fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator+(const fastArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) + fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator+(const fastArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) + fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator+(const fastArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) + fastArith<T>(rvalue); }

template<class T> inline fastestArith<T> operator+(const fastestArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) + fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator+(const fastestArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return fastestArith<T>(*lvalue + *rvalue); }
template<class T> inline fastestArith<T> operator+(const fastestArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) + fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator+(const fastestArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) + fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator+(const fastestArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) + fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator+(const fastestArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) + fastestArith<T>(rvalue); }

template<class T> inline semifastArith<T> operator+(const semifastArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) + semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator+(const semifastArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) + semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator+(const semifastArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return semifastArith<T>(*lvalue + *rvalue); }
template<class T> inline semifastArith<T> operator+(const semifastArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) + semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator+(const semifastArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) + semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator+(const semifastArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) + semifastArith<T>(rvalue); }

template<class T> inline sseArith<T> operator+(const sseArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) + sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator+(const sseArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) + sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator+(const sseArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) + sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator+(const sseArith<T>&& lvalue, const sseArith<T>&& rvalue){ return sseArith<T>(*lvalue + *rvalue); }
template<class T> inline sseArith<T> operator+(const sseArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) + sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator+(const sseArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) + sseArith<T>(rvalue); }

template<class T> inline normalArith<T> operator+(const normalArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) + normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator+(const normalArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) + normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator+(const normalArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) + normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator+(const normalArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) + normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator+(const normalArith<T>&& lvalue, const normalArith<T>&& rvalue){ return normalArith<T>(*lvalue + *rvalue); }
template<class T> inline normalArith<T> operator+(const normalArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return normalArith<T>(*lvalue + *rvalue); }

template<class T> inline T operator+(const dummyArith<T>&& lvalue, const fastArith<T>&& rvalue){ return *lvalue + *rvalue; }
template<class T> inline T operator+(const dummyArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return *lvalue + *rvalue; }
template<class T> inline T operator+(const dummyArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return *lvalue + *rvalue; }
template<class T> inline T operator+(const dummyArith<T>&& lvalue, const sseArith<T>&& rvalue){ return *lvalue + *rvalue; }
template<class T> inline T operator+(const dummyArith<T>&& lvalue, const normalArith<T>&& rvalue){ return *lvalue + *rvalue; }
template<class T> inline T operator+(const dummyArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return *lvalue + *rvalue; }

template<class T> inline fastArith<T> operator-(const fastArith<T>&& lvalue, const fastArith<T>&& rvalue){ return fastArith<T>(*lvalue - *rvalue); }
template<class T> inline fastArith<T> operator-(const fastArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) - fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator-(const fastArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) - fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator-(const fastArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) - fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator-(const fastArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) - fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator-(const fastArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) - fastArith<T>(rvalue); }

template<class T> inline fastestArith<T> operator-(const fastestArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) - fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator-(const fastestArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return fastestArith<T>(*lvalue - *rvalue); }
template<class T> inline fastestArith<T> operator-(const fastestArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) - fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator-(const fastestArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) - fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator-(const fastestArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) - fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator-(const fastestArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) - fastestArith<T>(rvalue); }

template<class T> inline semifastArith<T> operator-(const semifastArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) - semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator-(const semifastArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) - semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator-(const semifastArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return semifastArith<T>(*lvalue - *rvalue); }
template<class T> inline semifastArith<T> operator-(const semifastArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) - semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator-(const semifastArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) - semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator-(const semifastArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) - semifastArith<T>(rvalue); }

template<class T> inline sseArith<T> operator-(const sseArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) - sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator-(const sseArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) - sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator-(const sseArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) - sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator-(const sseArith<T>&& lvalue, const sseArith<T>&& rvalue){ return sseArith<T>(*lvalue - *rvalue); }
template<class T> inline sseArith<T> operator-(const sseArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) - sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator-(const sseArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) - sseArith<T>(rvalue); }

template<class T> inline normalArith<T> operator-(const normalArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) - normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator-(const normalArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) - normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator-(const normalArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) - normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator-(const normalArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) - normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator-(const normalArith<T>&& lvalue, const normalArith<T>&& rvalue){ return normalArith<T>(*lvalue - *rvalue); }
template<class T> inline normalArith<T> operator-(const normalArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return normalArith<T>(*lvalue - *rvalue); }

template<class T> inline T operator-(const dummyArith<T>&& lvalue, const fastArith<T>&& rvalue){ return *lvalue - *rvalue; }
template<class T> inline T operator-(const dummyArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return *lvalue - *rvalue; }
template<class T> inline T operator-(const dummyArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return *lvalue - *rvalue; }
template<class T> inline T operator-(const dummyArith<T>&& lvalue, const sseArith<T>&& rvalue){ return *lvalue - *rvalue; }
template<class T> inline T operator-(const dummyArith<T>&& lvalue, const normalArith<T>&& rvalue){ return *lvalue - *rvalue; }
template<class T> inline T operator-(const dummyArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return *lvalue - *rvalue; }

template<class T> inline fastArith<T> operator*(const fastArith<T>&& lvalue, const fastArith<T>&& rvalue){ return fastArith<T>(*lvalue * *rvalue); }
template<class T> inline fastArith<T> operator*(const fastArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) * fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator*(const fastArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) * fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator*(const fastArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) * fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator*(const fastArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) * fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator*(const fastArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) * fastArith<T>(rvalue); }

template<class T> inline fastestArith<T> operator*(const fastestArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) * fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator*(const fastestArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return fastestArith<T>(*lvalue * *rvalue); }
template<class T> inline fastestArith<T> operator*(const fastestArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) * fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator*(const fastestArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) * fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator*(const fastestArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) * fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator*(const fastestArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) * fastestArith<T>(rvalue); }

template<class T> inline semifastArith<T> operator*(const semifastArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) * semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator*(const semifastArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) * semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator*(const semifastArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return semifastArith<T>(*lvalue * *rvalue); }
template<class T> inline semifastArith<T> operator*(const semifastArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) * semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator*(const semifastArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) * semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator*(const semifastArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) * semifastArith<T>(rvalue); }

template<class T> inline sseArith<T> operator*(const sseArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) * sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator*(const sseArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) * sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator*(const sseArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) * sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator*(const sseArith<T>&& lvalue, const sseArith<T>&& rvalue){ return sseArith<T>(*lvalue * *rvalue); }
template<class T> inline sseArith<T> operator*(const sseArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) * sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator*(const sseArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) * sseArith<T>(rvalue); }

template<class T> inline normalArith<T> operator*(const normalArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) * normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator*(const normalArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) * normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator*(const normalArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) * normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator*(const normalArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) * normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator*(const normalArith<T>&& lvalue, const normalArith<T>&& rvalue){ return normalArith<T>(*lvalue * *rvalue); }
template<class T> inline normalArith<T> operator*(const normalArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return normalArith<T>(*lvalue * *rvalue); }

template<class T> inline T operator*(const dummyArith<T>&& lvalue, const fastArith<T>&& rvalue){ return *lvalue * *rvalue; }
template<class T> inline T operator*(const dummyArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return *lvalue * *rvalue; }
template<class T> inline T operator*(const dummyArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return *lvalue * *rvalue; }
template<class T> inline T operator*(const dummyArith<T>&& lvalue, const sseArith<T>&& rvalue){ return *lvalue * *rvalue; }
template<class T> inline T operator*(const dummyArith<T>&& lvalue, const normalArith<T>&& rvalue){ return *lvalue * *rvalue; }
template<class T> inline T operator*(const dummyArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return *lvalue * *rvalue; }

template<class T> inline fastArith<T> operator/(const fastArith<T>&& lvalue, const fastArith<T>&& rvalue){ return fastArith<T>(*lvalue / *rvalue); }
template<class T> inline fastArith<T> operator/(const fastArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) / fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator/(const fastArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) / fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator/(const fastArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) / fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator/(const fastArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) / fastArith<T>(rvalue); }
template<class T> inline fastArith<T> operator/(const fastArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) / fastArith<T>(rvalue); }

template<class T> inline fastestArith<T> operator/(const fastestArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) / fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator/(const fastestArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return fastestArith<T>(*lvalue / *rvalue); }
template<class T> inline fastestArith<T> operator/(const fastestArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) / fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator/(const fastestArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) / fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator/(const fastestArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) / fastestArith<T>(rvalue); }
template<class T> inline fastestArith<T> operator/(const fastestArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) / fastestArith<T>(rvalue); }

template<class T> inline semifastArith<T> operator/(const semifastArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) / semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator/(const semifastArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) / semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator/(const semifastArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return semifastArith<T>(*lvalue / *rvalue); }
template<class T> inline semifastArith<T> operator/(const semifastArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) / semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator/(const semifastArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) / semifastArith<T>(rvalue); }
template<class T> inline semifastArith<T> operator/(const semifastArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) / semifastArith<T>(rvalue); }

template<class T> inline sseArith<T> operator/(const sseArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) / sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator/(const sseArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) / sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator/(const sseArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) / sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator/(const sseArith<T>&& lvalue, const sseArith<T>&& rvalue){ return sseArith<T>(*lvalue / *rvalue); }
template<class T> inline sseArith<T> operator/(const sseArith<T>&& lvalue, const normalArith<T>&& rvalue){ return std::move(lvalue) / sseArith<T>(rvalue); }
template<class T> inline sseArith<T> operator/(const sseArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return std::move(lvalue) / sseArith<T>(rvalue); }

template<class T> inline normalArith<T> operator/(const normalArith<T>&& lvalue, const fastArith<T>&& rvalue){ return std::move(lvalue) / normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator/(const normalArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return std::move(lvalue) / normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator/(const normalArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return std::move(lvalue) / normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator/(const normalArith<T>&& lvalue, const sseArith<T>&& rvalue){ return std::move(lvalue) / normalArith<T>(rvalue); }
template<class T> inline normalArith<T> operator/(const normalArith<T>&& lvalue, const normalArith<T>&& rvalue){ return normalArith<T>(*lvalue / *rvalue); }
template<class T> inline normalArith<T> operator/(const normalArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return normalArith<T>(*lvalue / *rvalue); }

template<class T> inline T operator/(const dummyArith<T>&& lvalue, const fastArith<T>&& rvalue){ return *lvalue / *rvalue; }
template<class T> inline T operator/(const dummyArith<T>&& lvalue, const fastestArith<T>&& rvalue){ return *lvalue / *rvalue; }
template<class T> inline T operator/(const dummyArith<T>&& lvalue, const semifastArith<T>&& rvalue){ return *lvalue / *rvalue; }
template<class T> inline T operator/(const dummyArith<T>&& lvalue, const sseArith<T>&& rvalue){ return *lvalue / *rvalue; }
template<class T> inline T operator/(const dummyArith<T>&& lvalue, const normalArith<T>&& rvalue){ return *lvalue / *rvalue; }
template<class T> inline T operator/(const dummyArith<T>&& lvalue, const dummyArith<T>&& rvalue){ return *lvalue / *rvalue; }

GASHA_FAST_ARITH_OPERATOR(fastArith);
GASHA_FAST_ARITH_OPERATOR(fastestArith);
GASHA_FAST_ARITH_OPERATOR(semifastArith);
GASHA_FAST_ARITH_OPERATOR(sseArith);
GASHA_FAST_ARITH_OPERATOR(normalArith);

//----------------------------------------
//高速演算：特殊化
#ifdef GASHA_FAST_ARITH_USE_SSE
template<> inline fastArith<float> operator+(const fastArith<float>&& lvalue, const fastArith<float>&& rvalue);
template<> inline fastArith<__m128> operator+(const fastArith<__m128>&& lvalue, const fastArith<__m128>&& rvalue);

template<> inline fastArith<float> operator-(const fastArith<float>&& lvalue, const fastArith<float>&& rvalue);
template<> inline fastArith<__m128> operator-(const fastArith<__m128>&& lvalue, const fastArith<__m128>&& rvalue);

template<> inline fastArith<float> operator*(const fastArith<float>&& lvalue, const fastArith<float>&& rvalue);
template<> inline fastArith<__m128> operator*(const fastArith<__m128>&& lvalue, const fastArith<__m128>&& rvalue);

template<> inline fastArith<float> operator/(const fastArith<float>&& lvalue, const fastArith<float>&& rvalue);
template<> inline fastArith<__m128> operator/(const fastArith<__m128>&& lvalue, const fastArith<__m128>&& rvalue);

template<> inline fastestArith<float> operator+(const fastestArith<float>&& lvalue, const fastestArith<float>&& rvalue);
template<> inline fastestArith<__m128> operator+(const fastestArith<__m128>&& lvalue, const fastestArith<__m128>&& rvalue);

template<> inline fastestArith<float> operator-(const fastestArith<float>&& lvalue, const fastestArith<float>&& rvalue);
template<> inline fastestArith<__m128> operator-(const fastestArith<__m128>&& lvalue, const fastestArith<__m128>&& rvalue);

template<> inline fastestArith<float> operator*(const fastestArith<float>&& lvalue, const fastestArith<float>&& rvalue);
template<> inline fastestArith<__m128> operator*(const fastestArith<__m128>&& lvalue, const fastestArith<__m128>&& rvalue);

template<> inline fastestArith<float> operator/(const fastestArith<float>&& lvalue, const fastestArith<float>&& rvalue);
template<> inline fastestArith<__m128> operator/(const fastestArith<__m128>&& lvalue, const fastestArith<__m128>&& rvalue);

template<> inline semifastArith<float> operator+(const semifastArith<float>&& lvalue, const semifastArith<float>&& rvalue);
template<> inline semifastArith<__m128> operator+(const semifastArith<__m128>&& lvalue, const semifastArith<__m128>&& rvalue);

template<> inline semifastArith<float> operator-(const semifastArith<float>&& lvalue, const semifastArith<float>&& rvalue);
template<> inline semifastArith<__m128> operator-(const semifastArith<__m128>&& lvalue, const semifastArith<__m128>&& rvalue);

template<> inline semifastArith<float> operator*(const semifastArith<float>&& lvalue, const semifastArith<float>&& rvalue);
template<> inline semifastArith<__m128> operator*(const semifastArith<__m128>&& lvalue, const semifastArith<__m128>&& rvalue);

template<> inline semifastArith<float> operator/(const semifastArith<float>&& lvalue, const semifastArith<float>&& rvalue);
template<> inline semifastArith<__m128> operator/(const semifastArith<__m128>&& lvalue, const semifastArith<__m128>&& rvalue);

template<> inline sseArith<float> operator+(const sseArith<float>&& lvalue, const sseArith<float>&& rvalue);
template<> inline sseArith<__m128> operator+(const sseArith<__m128>&& lvalue, const sseArith<__m128>&& rvalue);

template<> inline sseArith<float> operator-(const sseArith<float>&& lvalue, const sseArith<float>&& rvalue);
template<> inline sseArith<__m128> operator-(const sseArith<__m128>&& lvalue, const sseArith<__m128>&& rvalue);

template<> inline sseArith<float> operator*(const sseArith<float>&& lvalue, const sseArith<float>&& rvalue);
template<> inline sseArith<__m128> operator*(const sseArith<__m128>&& lvalue, const sseArith<__m128>&& rvalue);

template<> inline sseArith<float> operator/(const sseArith<float>&& lvalue, const sseArith<float>&& rvalue);
template<> inline sseArith<__m128> operator/(const sseArith<__m128>&& lvalue, const sseArith<__m128>&& rvalue);

template<> inline normalArith<__m128> operator+(const normalArith<__m128>&& lvalue, const normalArith<__m128>&& rvalue);

template<> inline normalArith<__m128> operator-(const normalArith<__m128>&& lvalue, const normalArith<__m128>&& rvalue);

template<> inline normalArith<__m128> operator*(const normalArith<__m128>&& lvalue, const normalArith<__m128>&& rvalue);

template<> inline normalArith<__m128> operator/(const normalArith<__m128>&& lvalue, const normalArith<__m128>&& rvalue);

template<> inline __m128 operator+(const dummyArith<__m128>&& lvalue, const dummyArith<__m128>&& rvalue);

template<> inline __m128 operator-(const dummyArith<__m128>&& lvalue, const dummyArith<__m128>&& rvalue);

template<> inline __m128 operator*(const dummyArith<__m128>&& lvalue, const dummyArith<__m128>&& rvalue);

template<> inline __m128 operator/(const dummyArith<__m128>&& lvalue, const dummyArith<__m128>&& rvalue);
#endif//GASHA_FAST_ARITH_USE_SSE
#ifdef GASHA_FAST_ARITH_USE_SSE2
template<> inline fastArith<double> operator+(const fastArith<double>&& lvalue, const fastArith<double>&& rvalue);
template<> inline fastArith<__m128d> operator+(const fastArith<__m128d>&& lvalue, const fastArith<__m128d>&& rvalue);

template<> inline fastArith<double> operator-(const fastArith<double>&& lvalue, const fastArith<double>&& rvalue);
template<> inline fastArith<__m128d> operator-(const fastArith<__m128d>&& lvalue, const fastArith<__m128d>&& rvalue);

template<> inline fastArith<double> operator*(const fastArith<double>&& lvalue, const fastArith<double>&& rvalue);
template<> inline fastArith<__m128d> operator*(const fastArith<__m128d>&& lvalue, const fastArith<__m128d>&& rvalue);

template<> inline fastArith<double> operator/(const fastArith<double>&& lvalue, const fastArith<double>&& rvalue);
template<> inline fastArith<__m128d> operator/(const fastArith<__m128d>&& lvalue, const fastArith<__m128d>&& rvalue);

template<> inline fastestArith<double> operator+(const fastestArith<double>&& lvalue, const fastestArith<double>&& rvalue);
template<> inline fastestArith<__m128d> operator+(const fastestArith<__m128d>&& lvalue, const fastestArith<__m128d>&& rvalue);

template<> inline fastestArith<double> operator-(const fastestArith<double>&& lvalue, const fastestArith<double>&& rvalue);
template<> inline fastestArith<__m128d> operator-(const fastestArith<__m128d>&& lvalue, const fastestArith<__m128d>&& rvalue);

template<> inline fastestArith<double> operator*(const fastestArith<double>&& lvalue, const fastestArith<double>&& rvalue);
template<> inline fastestArith<__m128d> operator*(const fastestArith<__m128d>&& lvalue, const fastestArith<__m128d>&& rvalue);

template<> inline fastestArith<double> operator/(const fastestArith<double>&& lvalue, const fastestArith<double>&& rvalue);
template<> inline fastestArith<__m128d> operator/(const fastestArith<__m128d>&& lvalue, const fastestArith<__m128d>&& rvalue);

template<> inline semifastArith<double> operator+(const semifastArith<double>&& lvalue, const semifastArith<double>&& rvalue);
template<> inline semifastArith<__m128d> operator+(const semifastArith<__m128d>&& lvalue, const semifastArith<__m128d>&& rvalue);

template<> inline semifastArith<double> operator-(const semifastArith<double>&& lvalue, const semifastArith<double>&& rvalue);
template<> inline semifastArith<__m128d> operator-(const semifastArith<__m128d>&& lvalue, const semifastArith<__m128d>&& rvalue);

template<> inline semifastArith<double> operator*(const semifastArith<double>&& lvalue, const semifastArith<double>&& rvalue);
template<> inline semifastArith<__m128d> operator*(const semifastArith<__m128d>&& lvalue, const semifastArith<__m128d>&& rvalue);

template<> inline semifastArith<double> operator/(const semifastArith<double>&& lvalue, const semifastArith<double>&& rvalue);
template<> inline semifastArith<__m128d> operator/(const semifastArith<__m128d>&& lvalue, const semifastArith<__m128d>&& rvalue);

template<> inline sseArith<double> operator+(const sseArith<double>&& lvalue, const sseArith<double>&& rvalue);
template<> inline sseArith<__m128d> operator+(const sseArith<__m128d>&& lvalue, const sseArith<__m128d>&& rvalue);

template<> inline sseArith<double> operator-(const sseArith<double>&& lvalue, const sseArith<double>&& rvalue);
template<> inline sseArith<__m128d> operator-(const sseArith<__m128d>&& lvalue, const sseArith<__m128d>&& rvalue);

template<> inline sseArith<double> operator*(const sseArith<double>&& lvalue, const sseArith<double>&& rvalue);
template<> inline sseArith<__m128d> operator*(const sseArith<__m128d>&& lvalue, const sseArith<__m128d>&& rvalue);

template<> inline sseArith<double> operator/(const sseArith<double>&& lvalue, const sseArith<double>&& rvalue);
template<> inline sseArith<__m128d> operator/(const sseArith<__m128d>&& lvalue, const sseArith<__m128d>&& rvalue);

template<> inline normalArith<__m128d> operator+(const normalArith<__m128d>&& lvalue, const normalArith<__m128d>&& rvalue);

template<> inline normalArith<__m128d> operator-(const normalArith<__m128d>&& lvalue, const normalArith<__m128d>&& rvalue);

template<> inline normalArith<__m128d> operator*(const normalArith<__m128d>&& lvalue, const normalArith<__m128d>&& rvalue);

template<> inline normalArith<__m128d> operator/(const normalArith<__m128d>&& lvalue, const normalArith<__m128d>&& rvalue);

template<> inline __m128d operator+(const dummyArith<__m128d>&& lvalue, const dummyArith<__m128d>&& rvalue);

template<> inline __m128d operator-(const dummyArith<__m128d>&& lvalue, const dummyArith<__m128d>&& rvalue);

template<> inline __m128d operator*(const dummyArith<__m128d>&& lvalue, const dummyArith<__m128d>&& rvalue);

template<> inline __m128d operator/(const dummyArith<__m128d>&& lvalue, const dummyArith<__m128d>&& rvalue);
#endif//GASHA_FAST_ARITH_USE_SSE2
#ifdef GASHA_FAST_ARITH_USE_AVX
template<> inline fastArith<__m256> operator+(const fastArith<__m256>&& lvalue, const fastArith<__m256>&& rvalue);
template<> inline fastArith<__m256d> operator+(const fastArith<__m256d>&& lvalue, const fastArith<__m256d>&& rvalue);

template<> inline fastArith<__m256> operator-(const fastArith<__m256>&& lvalue, const fastArith<__m256>&& rvalue);
template<> inline fastArith<__m256d> operator-(const fastArith<__m256d>&& lvalue, const fastArith<__m256d>&& rvalue);

template<> inline fastArith<__m256> operator*(const fastArith<__m256>&& lvalue, const fastArith<__m256>&& rvalue);
template<> inline fastArith<__m256d> operator*(const fastArith<__m256d>&& lvalue, const fastArith<__m256d>&& rvalue);

template<> inline fastArith<__m256> operator/(const fastArith<__m256>&& lvalue, const fastArith<__m256>&& rvalue);
template<> inline fastArith<__m256d> operator/(const fastArith<__m256d>&& lvalue, const fastArith<__m256d>&& rvalue);

template<> inline fastestArith<__m256> operator+(const fastestArith<__m256>&& lvalue, const fastestArith<__m256>&& rvalue);
template<> inline fastestArith<__m256d> operator+(const fastestArith<__m256d>&& lvalue, const fastestArith<__m256d>&& rvalue);

template<> inline fastestArith<__m256> operator-(const fastestArith<__m256>&& lvalue, const fastestArith<__m256>&& rvalue);
template<> inline fastestArith<__m256d> operator-(const fastestArith<__m256d>&& lvalue, const fastestArith<__m256d>&& rvalue);

template<> inline fastestArith<__m256> operator*(const fastestArith<__m256>&& lvalue, const fastestArith<__m256>&& rvalue);
template<> inline fastestArith<__m256d> operator*(const fastestArith<__m256d>&& lvalue, const fastestArith<__m256d>&& rvalue);

template<> inline fastestArith<__m256> operator/(const fastestArith<__m256>&& lvalue, const fastestArith<__m256>&& rvalue);
template<> inline fastestArith<__m256d> operator/(const fastestArith<__m256d>&& lvalue, const fastestArith<__m256d>&& rvalue);

template<> inline semifastArith<__m256> operator+(const semifastArith<__m256>&& lvalue, const semifastArith<__m256>&& rvalue);
template<> inline semifastArith<__m256d> operator+(const semifastArith<__m256d>&& lvalue, const semifastArith<__m256d>&& rvalue);

template<> inline semifastArith<__m256> operator-(const semifastArith<__m256>&& lvalue, const semifastArith<__m256>&& rvalue);
template<> inline semifastArith<__m256d> operator-(const semifastArith<__m256d>&& lvalue, const semifastArith<__m256d>&& rvalue);

template<> inline semifastArith<__m256> operator*(const semifastArith<__m256>&& lvalue, const semifastArith<__m256>&& rvalue);
template<> inline semifastArith<__m256d> operator*(const semifastArith<__m256d>&& lvalue, const semifastArith<__m256d>&& rvalue);

template<> inline semifastArith<__m256> operator/(const semifastArith<__m256>&& lvalue, const semifastArith<__m256>&& rvalue);
template<> inline semifastArith<__m256d> operator/(const semifastArith<__m256d>&& lvalue, const semifastArith<__m256d>&& rvalue);

template<> inline sseArith<__m256> operator+(const sseArith<__m256>&& lvalue, const sseArith<__m256>&& rvalue);
template<> inline sseArith<__m256d> operator+(const sseArith<__m256d>&& lvalue, const sseArith<__m256d>&& rvalue);

template<> inline sseArith<__m256> operator-(const sseArith<__m256>&& lvalue, const sseArith<__m256>&& rvalue);
template<> inline sseArith<__m256d> operator-(const sseArith<__m256d>&& lvalue, const sseArith<__m256d>&& rvalue);

template<> inline sseArith<__m256> operator*(const sseArith<__m256>&& lvalue, const sseArith<__m256>&& rvalue);
template<> inline sseArith<__m256d> operator*(const sseArith<__m256d>&& lvalue, const sseArith<__m256d>&& rvalue);

template<> inline sseArith<__m256> operator/(const sseArith<__m256>&& lvalue, const sseArith<__m256>&& rvalue);
template<> inline sseArith<__m256d> operator/(const sseArith<__m256d>&& lvalue, const sseArith<__m256d>&& rvalue);

template<> inline normalArith<__m256> operator+(const normalArith<__m256>&& lvalue, const normalArith<__m256>&& rvalue);
template<> inline normalArith<__m256d> operator+(const normalArith<__m256d>&& lvalue, const normalArith<__m256d>&& rvalue);

template<> inline normalArith<__m256> operator-(const normalArith<__m256>&& lvalue, const normalArith<__m256>&& rvalue);
template<> inline normalArith<__m256d> operator-(const normalArith<__m256d>&& lvalue, const normalArith<__m256d>&& rvalue);

template<> inline normalArith<__m256> operator*(const normalArith<__m256>&& lvalue, const normalArith<__m256>&& rvalue);
template<> inline normalArith<__m256d> operator*(const normalArith<__m256d>&& lvalue, const normalArith<__m256d>&& rvalue);

template<> inline normalArith<__m256> operator/(const normalArith<__m256>&& lvalue, const normalArith<__m256>&& rvalue);
template<> inline normalArith<__m256d> operator/(const normalArith<__m256d>&& lvalue, const normalArith<__m256d>&& rvalue);

template<> inline __m256 operator+(const dummyArith<__m256>&& lvalue, const dummyArith<__m256>&& rvalue);
template<> inline __m256d operator+(const dummyArith<__m256d>&& lvalue, const dummyArith<__m256d>&& rvalue);

template<> inline __m256 operator-(const dummyArith<__m256>&& lvalue, const dummyArith<__m256>&& rvalue);
template<> inline __m256d operator-(const dummyArith<__m256d>&& lvalue, const dummyArith<__m256d>&& rvalue);

template<> inline __m256 operator*(const dummyArith<__m256>&& lvalue, const dummyArith<__m256>&& rvalue);
template<> inline __m256d operator*(const dummyArith<__m256d>&& lvalue, const dummyArith<__m256d>&& rvalue);

template<> inline __m256 operator/(const dummyArith<__m256>&& lvalue, const dummyArith<__m256>&& rvalue);
template<> inline __m256d operator/(const dummyArith<__m256d>&& lvalue, const dummyArith<__m256d>&& rvalue);
#endif//GASHA_FAST_ARITH_USE_AVX

//----------------------------------------
//高速平方根
template<typename T> inline T sqrt(const T value);
template<typename T> inline fastArith<T> sqrt(const fastArith<T>&& value);
template<typename T> inline fastArith<T> sqrt(const fastArith<T>& value){ return sqrt(std::move(value)); }
template<typename T> inline fastestArith<T> sqrt(const fastestArith<T>&& value);
template<typename T> inline fastestArith<T> sqrt(const fastestArith<T>& value){ return sqrt(std::move(value)); }
template<typename T> inline semifastArith<T> sqrt(const semifastArith<T>&& value);
template<typename T> inline semifastArith<T> sqrt(const semifastArith<T>& value){ return sqrt(std::move(value)); }
template<typename T> inline sseArith<T> sqrt(const sseArith<T>&& value);
template<typename T> inline sseArith<T> sqrt(const sseArith<T>& value){ return sqrt(std::move(value)); }
template<typename T> inline normalArith<T> sqrt(const normalArith<T>&& value);
template<typename T> inline normalArith<T> sqrt(const normalArith<T>& value){ return sqrt(std::move(value)); }
template<typename T> inline T sqrt(const dummyArith<T>&& value);
template<typename T> inline T sqrt(const dummyArith<T>& value){ return sqrt(std::move(value)); }

//----------------------------------------
//高速平方根：特殊化
#ifdef GASHA_FAST_ARITH_USE_SSE
template<> inline fastArith<float> sqrt(const fastArith<float>&& value);
template<> inline fastestArith<float> sqrt(const fastestArith<float>&& value);
template<> inline semifastArith<float> sqrt(const semifastArith<float>&& value);
template<> inline sseArith<float> sqrt(const sseArith<float>&& value);

template<> inline fastArith<__m128> sqrt(const fastArith<__m128>&& value);
template<> inline fastestArith<__m128> sqrt(const fastestArith<__m128>&& value);
template<> inline semifastArith<__m128> sqrt(const semifastArith<__m128>&& value);
template<> inline sseArith<__m128> sqrt(const sseArith<__m128>&& value);
template<> inline normalArith<__m128> sqrt(const normalArith<__m128>&& value);
template<> inline __m128 sqrt(const dummyArith<__m128>&& value);
#endif//GASHA_FAST_ARITH_USE_SSE
#ifdef GASHA_FAST_ARITH_USE_SSE2
template<> inline fastArith<double> sqrt(const fastArith<double>&& value);
template<> inline fastestArith<double> sqrt(const fastestArith<double>&& value);
template<> inline semifastArith<double> sqrt(const semifastArith<double>&& value);
template<> inline sseArith<double> sqrt(const sseArith<double>&& value);

template<> inline fastArith<__m128d> sqrt(const fastArith<__m128d>&& value);
template<> inline fastestArith<__m128d> sqrt(const fastestArith<__m128d>&& value);
template<> inline semifastArith<__m128d> sqrt(const semifastArith<__m128d>&& value);
template<> inline sseArith<__m128d> sqrt(const sseArith<__m128d>&& value);
template<> inline normalArith<__m128d> sqrt(const normalArith<__m128d>&& value);
template<> inline __m128d sqrt(const dummyArith<__m128d>&& value);
#endif//GASHA_FAST_ARITH_USE_SSE2
#ifdef GASHA_FAST_ARITH_USE_AVX
template<> inline fastArith<__m256> sqrt(const fastArith<__m256>&& value);
template<> inline fastestArith<__m256> sqrt(const fastestArith<__m256>&& value);
template<> inline semifastArith<__m256> sqrt(const semifastArith<__m256>&& value);
template<> inline sseArith<__m256> sqrt(const sseArith<__m256>&& value);
template<> inline normalArith<__m256> sqrt(const normalArith<__m256>&& value);
template<> inline __m256 sqrt(const dummyArith<__m256>&& value);

template<> inline fastArith<__m256d> sqrt(const fastArith<__m256d>&& value);
template<> inline fastestArith<__m256d> sqrt(const fastestArith<__m256d>&& value);
template<> inline semifastArith<__m256d> sqrt(const semifastArith<__m256d>&& value);
template<> inline sseArith<__m256d> sqrt(const sseArith<__m256d>&& value);
template<> inline normalArith<__m256d> sqrt(const normalArith<__m256d>&& value);
template<> inline __m256d sqrt(const dummyArith<__m256d>&& value);
#endif//GASHA_FAST_ARITH_USE_AVX

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルードする
#include <gasha/fast_math.inl>

#endif//GASHA_INCLUDED_FAST_MATH_H

// End of file

#pragma once
#ifndef GASHA_INCLUDED_FAST_MATH_H
#define GASHA_INCLUDED_FAST_MATH_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// fast_math.h
// 高速算術【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/basic_math.h>//基本算術

#include <cstdint>//C++11 std::int*_t
#include <utility>//C++11 std::move

#ifdef GASHA_USE_SSE
#include <xmmintrin.h>//SSE1
#endif//GASHA_USE_SSE

#ifdef GASHA_USE_SSE2
#include <emmintrin.h>//SSE2
#endif//GASHA_USE_SSE2

#ifdef GASHA_USE_SSE4_1
#include <smmintrin.h>//SSE4.1
#endif//GASHA_USE_SSE4_1

#ifdef GASHA_USE_AVX
#include <immintrin.h>//AVX
#endif//GASHA_USE_AVX

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//SSE補助処理
//--------------------------------------------------------------------------------

#ifdef GASHA_USE_SSE
//__m128型のキャスト用
union m128_cast
{
	__m128 m_xmm;
	float m_float[4];
	m128_cast(__m128 xmm) :
		m_xmm(xmm)
	{}
};
#endif//GASHA_USE_SSE

#ifdef GASHA_USE_SSE2
//__m128d型のキャスト用
union m128d_cast
{
	__m128d m_xmm;
	float m_double[2];
	m128d_cast(__m128d xmm) :
		m_xmm(xmm)
	{}
};
#endif//GASHA_USE_SSE2

#ifdef GASHA_USE_SSE2
//__m128i型のキャスト用
union m128i_cast
{
	__m128i m_xmm;
	std::int64_t m_i64[2];
	std::uint64_t m_u64[2];
	std::int32_t m_i32[4];
	std::uint32_t m_u32[4];
	std::int16_t m_i16[8];
	std::uint16_t m_u16[8];
	std::int8_t m_i8[16];
	std::uint8_t m_u8[16];
	m128i_cast(__m128i xmm) :
		m_xmm(xmm)
	{}
};
#endif//GASHA_USE_SSE2

#ifdef GASHA_USE_AVX
//__m256型のキャスト用
union m256_cast
{
	__m256 m_ymm;
	float m_float[8];
	m256_cast(__m256 ymm) :
		m_ymm(ymm)
	{}
};
#endif//GASHA_USE_AVX

#ifdef GASHA_USE_AVX
//__m256d型のキャスト用
union m256d_cast
{
	__m256d m_ymm;
	float m_double[4];
	m256d_cast(__m256d ymm) :
		m_ymm(ymm)
	{}
};
#endif//GASHA_USE_AVX

#ifdef GASHA_USE_AVX2
//__m256i型のキャスト用
union m256i_cast
{
	__m256i m_ymm;
	std::int64_t m_i64[4];
	std::uint64_t m_u64[4];
	std::int32_t m_i32[8];
	std::uint32_t m_u32[8];
	std::int16_t m_i16[16];
	std::uint16_t m_u16[16];
	std::int8_t m_i8[32];
	std::uint8_t m_u8[32];
	m256i_cast(__m256i ymm) :
		m_ymm(ymm)
	{}
};
#endif//GASHA_USE_AVX2

//--------------------------------------------------------------------------------
//高速四則演算
//--------------------------------------------------------------------------------

//クラス宣言
//・fastA    ... fast arithmetic（高速演算）
//・fastestA ... fastest arithmetic（最高速演算）※精度が悪いが速い
//・semiA    ... semi-fast arithmetic（準高速演算）※精度が良いが遅い（通常演算より精度がよくなることはない）
//・sseA     ... sse arithmetic（SSE高速演算）
//・normA    ... normal arithmetic（通常演算）
//・dummyA   ... dummy arithmetic（通常演算）※他の型は計算結果として同じ型（fastA型など）を返すが、dummyAはfloatなどの元の値を返す
template <typename T> struct fastA;
template <typename T> struct fastestA;
template <typename T> struct semiA;
template <typename T> struct sseA;
template <typename T> struct normA;
template <typename T> struct dummyA;

//定義用マクロ
#define GASHA_FAST_ARITH_CLASS(CLASS_NAME) \
	template <typename T> \
	struct CLASS_NAME \
	{ \
		static const std::size_t rank = 0; \
		static const std::size_t extent = 1; \
		static const std::size_t extent1 = 0; \
		static const std::size_t extent2 = 0; \
		typedef T basic_value_type; \
		typedef T value_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastA<value_type> fastA_type; \
		typedef fastestA<value_type> fastestA_type; \
		typedef semiA<value_type> semiA_type; \
		typedef sseA<value_type> sseA_type; \
		typedef normA<value_type> normA_type; \
		typedef dummyA<value_type> dummyA_type; \
		value_type m_val; \
		inline operator value_type() const { return m_val; } \
		inline value_type operator*() const { return m_val; } \
		inline CLASS_NAME& operator=(const value_type val); \
		inline CLASS_NAME& operator=(const fastA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>&& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>& val); \
		inline CLASS_NAME& operator=(const normA<value_type>&& val); \
		inline CLASS_NAME& operator=(const normA<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>& val); \
		inline CLASS_NAME(const value_type val); \
		inline CLASS_NAME(const fastA<value_type>&& val); \
		inline CLASS_NAME(const fastA<value_type>& val); \
		inline CLASS_NAME(const fastestA<value_type>&& val); \
		inline CLASS_NAME(const fastestA<value_type>& val); \
		inline CLASS_NAME(const semiA<value_type>&& val); \
		inline CLASS_NAME(const semiA<value_type>& val); \
		inline CLASS_NAME(const sseA<value_type>&& val); \
		inline CLASS_NAME(const sseA<value_type>& val); \
		inline CLASS_NAME(const normA<value_type>&& val); \
		inline CLASS_NAME(const normA<value_type>& val); \
		inline CLASS_NAME(const dummyA<value_type>&& val); \
		inline CLASS_NAME(const dummyA<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_ARRAY_CLASS(CLASS_NAME) \
	template <typename T, std::size_t N> \
	struct CLASS_NAME<T[N]> \
	{ \
		static const std::size_t rank = 1; \
		static const std::size_t extent = N; \
		static const std::size_t extent1 = N; \
		static const std::size_t extent2 = 0; \
		typedef T basic_value_type; \
		typedef T value_type[N]; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastA<value_type> fastA_type; \
		typedef fastestA<value_type> fastestA_type; \
		typedef semiA<value_type> semiA_type; \
		typedef sseA<value_type> sseA_type; \
		typedef normA<value_type> normA_type; \
		typedef dummyA<value_type> dummyA_type; \
		value_type m_val; \
		inline operator const value_type&() const { return m_val; } \
		inline operator value_type&(){ return m_val; } \
		inline basic_value_type operator[](const int index) const { return m_val[index]; } \
		inline basic_value_type& operator[](const int index){ return m_val[index]; } \
		inline const value_type& operator*() const { return static_cast<const value_type&>(*this); } \
		inline value_type& operator*(){ return static_cast<value_type&>(*this); } \
		inline CLASS_NAME& operator=(const value_type&& val); \
		inline CLASS_NAME& operator=(const value_type& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>&& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>& val); \
		inline CLASS_NAME& operator=(const normA<value_type>&& val); \
		inline CLASS_NAME& operator=(const normA<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>& val); \
		inline CLASS_NAME(const value_type&& val); \
		inline CLASS_NAME(const value_type& val); \
		inline CLASS_NAME(const fastA<value_type>&& val); \
		inline CLASS_NAME(const fastA<value_type>& val); \
		inline CLASS_NAME(const fastestA<value_type>&& val); \
		inline CLASS_NAME(const fastestA<value_type>& val); \
		inline CLASS_NAME(const semiA<value_type>&& val); \
		inline CLASS_NAME(const semiA<value_type>& val); \
		inline CLASS_NAME(const sseA<value_type>&& val); \
		inline CLASS_NAME(const sseA<value_type>& val); \
		inline CLASS_NAME(const normA<value_type>&& val); \
		inline CLASS_NAME(const normA<value_type>& val); \
		inline CLASS_NAME(const dummyA<value_type>&& val); \
		inline CLASS_NAME(const dummyA<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	}; \
	template <typename T, std::size_t N, std::size_t M> \
	struct CLASS_NAME<T[N][M]> \
	{ \
		static const std::size_t rank = 2; \
		static const std::size_t extent = N * M; \
		static const std::size_t extent1 = N; \
		static const std::size_t extent2 = M; \
		typedef T basic_value_type; \
		typedef T value_type[N][M]; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastA<value_type> fastA_type; \
		typedef fastestA<value_type> fastestA_type; \
		typedef semiA<value_type> semiA_type; \
		typedef sseA<value_type> sseA_type; \
		typedef normA<value_type> normA_type; \
		typedef dummyA<value_type> dummyA_type; \
		value_type m_val; \
		inline operator const value_type&() const { return m_val; } \
		inline operator value_type&(){ return m_val; } \
		struct dim2 \
		{ \
			typedef basic_value_type dim2_value_type[extent2]; \
			dim2_value_type& m_val2; \
			inline basic_value_type operator[](int index) const { return m_val2[index]; } \
			inline basic_value_type& operator[](int index){ return m_val2[index]; } \
			inline dim2(const dim2_value_type& val2) : m_val2(const_cast<dim2_value_type&>(val2)){} \
			inline dim2(dim2_value_type& val2) : m_val2(val2){} \
		}; \
		inline const dim2 operator[](const int index) const { return dim2(m_val[index]); } \
		inline dim2 operator[](const int index){ return dim2(m_val[index]); } \
		inline const value_type& operator*() const { return static_cast<const value_type&>(*this); } \
		inline value_type& operator*(){ return static_cast<value_type&>(*this); } \
		inline CLASS_NAME& operator=(const value_type&& val); \
		inline CLASS_NAME& operator=(const value_type& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>&& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>& val); \
		inline CLASS_NAME& operator=(const normA<value_type>&& val); \
		inline CLASS_NAME& operator=(const normA<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>& val); \
		inline CLASS_NAME(const value_type&& val); \
		inline CLASS_NAME(const value_type& val); \
		inline CLASS_NAME(const fastA<value_type>&& val); \
		inline CLASS_NAME(const fastA<value_type>& val); \
		inline CLASS_NAME(const fastestA<value_type>&& val); \
		inline CLASS_NAME(const fastestA<value_type>& val); \
		inline CLASS_NAME(const semiA<value_type>&& val); \
		inline CLASS_NAME(const semiA<value_type>& val); \
		inline CLASS_NAME(const sseA<value_type>&& val); \
		inline CLASS_NAME(const sseA<value_type>& val); \
		inline CLASS_NAME(const normA<value_type>&& val); \
		inline CLASS_NAME(const normA<value_type>& val); \
		inline CLASS_NAME(const dummyA<value_type>&& val); \
		inline CLASS_NAME(const dummyA<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_FLOAT(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<float> \
	{ \
		static const std::size_t rank = 0; \
		static const std::size_t extent = 1; \
		static const std::size_t extent1 = 0; \
		static const std::size_t extent2 = 0; \
		typedef float basic_value_type; \
		typedef float value_type; \
		typedef __m128 sse_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastA<value_type> fastA_type; \
		typedef fastestA<value_type> fastestA_type; \
		typedef semiA<value_type> semiA_type; \
		typedef sseA<value_type> sseA_type; \
		typedef normA<value_type> normA_type; \
		typedef dummyA<value_type> dummyA_type; \
		sse_type m_val; \
		inline operator value_type() const { value_type val; _mm_store_ss(&val, m_val); return val; } \
		inline operator sse_type() const { return m_val; } \
		inline value_type operator*() const { return static_cast<value_type>(*this); } \
		inline CLASS_NAME& operator=(const value_type val); \
		inline CLASS_NAME& operator=(const sse_type&& val); \
		inline CLASS_NAME& operator=(const sse_type& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>&& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>& val); \
		inline CLASS_NAME& operator=(const normA<value_type>&& val); \
		inline CLASS_NAME& operator=(const normA<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>& val); \
		inline CLASS_NAME(const value_type val); \
		inline CLASS_NAME(const sse_type&& val); \
		inline CLASS_NAME(const sse_type& val); \
		inline CLASS_NAME(const fastA<value_type>&& val); \
		inline CLASS_NAME(const fastA<value_type>& val); \
		inline CLASS_NAME(const fastestA<value_type>&& val); \
		inline CLASS_NAME(const fastestA<value_type>& val); \
		inline CLASS_NAME(const semiA<value_type>&& val); \
		inline CLASS_NAME(const semiA<value_type>& val); \
		inline CLASS_NAME(const sseA<value_type>&& val); \
		inline CLASS_NAME(const sseA<value_type>& val); \
		inline CLASS_NAME(const normA<value_type>&& val); \
		inline CLASS_NAME(const normA<value_type>& val); \
		inline CLASS_NAME(const dummyA<value_type>&& val); \
		inline CLASS_NAME(const dummyA<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_DOUBLE(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<double> \
	{ \
		static const std::size_t rank = 0; \
		static const std::size_t extent = 1; \
		static const std::size_t extent1 = 0; \
		static const std::size_t extent2 = 0; \
		typedef double basic_value_type; \
		typedef double value_type; \
		typedef __m128d sse_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastA<value_type> fastA_type; \
		typedef fastestA<value_type> fastestA_type; \
		typedef semiA<value_type> semiA_type; \
		typedef sseA<value_type> sseA_type; \
		typedef normA<value_type> normA_type; \
		typedef dummyA<value_type> dummyA_type; \
		sse_type m_val; \
		inline operator value_type() const { value_type val; _mm_store_sd(&val, m_val); return val; } \
		inline operator sse_type() const { return m_val; } \
		inline value_type operator*() const { return static_cast<value_type>(*this); } \
		inline CLASS_NAME& operator=(const value_type val); \
		inline CLASS_NAME& operator=(const sse_type&& val); \
		inline CLASS_NAME& operator=(const sse_type& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>&& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>& val); \
		inline CLASS_NAME& operator=(const normA<value_type>&& val); \
		inline CLASS_NAME& operator=(const normA<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>& val); \
		inline CLASS_NAME(const value_type val); \
		inline CLASS_NAME(const sse_type&& val); \
		inline CLASS_NAME(const sse_type& val); \
		inline CLASS_NAME(const fastA<value_type>&& val); \
		inline CLASS_NAME(const fastA<value_type>& val); \
		inline CLASS_NAME(const fastestA<value_type>&& val); \
		inline CLASS_NAME(const fastestA<value_type>& val); \
		inline CLASS_NAME(const semiA<value_type>&& val); \
		inline CLASS_NAME(const semiA<value_type>& val); \
		inline CLASS_NAME(const sseA<value_type>&& val); \
		inline CLASS_NAME(const sseA<value_type>& val); \
		inline CLASS_NAME(const normA<value_type>&& val); \
		inline CLASS_NAME(const normA<value_type>& val); \
		inline CLASS_NAME(const dummyA<value_type>&& val); \
		inline CLASS_NAME(const dummyA<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_M128(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<__m128> \
	{ \
		static const std::size_t rank = 0; \
		static const std::size_t extent = 1; \
		static const std::size_t extent1 = 0; \
		static const std::size_t extent2 = 0; \
		typedef __m128 basic_value_type; \
		typedef __m128 value_type; \
		typedef __m128 sse_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastA<value_type> fastA_type; \
		typedef fastestA<value_type> fastestA_type; \
		typedef semiA<value_type> semiA_type; \
		typedef sseA<value_type> sseA_type; \
		typedef normA<value_type> normA_type; \
		typedef dummyA<value_type> dummyA_type; \
		value_type m_val; \
		inline operator value_type() const { return m_val; } \
		inline value_type operator*() const { return static_cast<value_type>(*this); } \
		inline CLASS_NAME& operator=(const value_type&& val); \
		inline CLASS_NAME& operator=(const value_type& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>&& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>& val); \
		inline CLASS_NAME& operator=(const normA<value_type>&& val); \
		inline CLASS_NAME& operator=(const normA<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>& val); \
		inline CLASS_NAME(const value_type&& val); \
		inline CLASS_NAME(const value_type& val); \
		inline CLASS_NAME(const fastA<value_type>&& val); \
		inline CLASS_NAME(const fastA<value_type>& val); \
		inline CLASS_NAME(const fastestA<value_type>&& val); \
		inline CLASS_NAME(const fastestA<value_type>& val); \
		inline CLASS_NAME(const semiA<value_type>&& val); \
		inline CLASS_NAME(const semiA<value_type>& val); \
		inline CLASS_NAME(const sseA<value_type>&& val); \
		inline CLASS_NAME(const sseA<value_type>& val); \
		inline CLASS_NAME(const normA<value_type>&& val); \
		inline CLASS_NAME(const normA<value_type>& val); \
		inline CLASS_NAME(const dummyA<value_type>&& val); \
		inline CLASS_NAME(const dummyA<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_M128D(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<__m128d> \
	{ \
		static const std::size_t rank = 0; \
		static const std::size_t extent = 1; \
		static const std::size_t extent1 = 0; \
		static const std::size_t extent2 = 0; \
		typedef __m128d basic_value_type; \
		typedef __m128d value_type; \
		typedef __m128d sse_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastA<value_type> fastA_type; \
		typedef fastestA<value_type> fastestA_type; \
		typedef semiA<value_type> semiA_type; \
		typedef sseA<value_type> sseA_type; \
		typedef normA<value_type> normA_type; \
		typedef dummyA<value_type> dummyA_type; \
		value_type m_val; \
		inline operator value_type() const { return m_val; } \
		inline value_type operator*() const { return static_cast<value_type>(*this); } \
		inline CLASS_NAME& operator=(const value_type&& val); \
		inline CLASS_NAME& operator=(const value_type& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>&& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>& val); \
		inline CLASS_NAME& operator=(const normA<value_type>&& val); \
		inline CLASS_NAME& operator=(const normA<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>& val); \
		inline CLASS_NAME(const value_type&& val); \
		inline CLASS_NAME(const value_type& val); \
		inline CLASS_NAME(const fastA<value_type>&& val); \
		inline CLASS_NAME(const fastA<value_type>& val); \
		inline CLASS_NAME(const fastestA<value_type>&& val); \
		inline CLASS_NAME(const fastestA<value_type>& val); \
		inline CLASS_NAME(const semiA<value_type>&& val); \
		inline CLASS_NAME(const semiA<value_type>& val); \
		inline CLASS_NAME(const sseA<value_type>&& val); \
		inline CLASS_NAME(const sseA<value_type>& val); \
		inline CLASS_NAME(const normA<value_type>&& val); \
		inline CLASS_NAME(const normA<value_type>& val); \
		inline CLASS_NAME(const dummyA<value_type>&& val); \
		inline CLASS_NAME(const dummyA<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_M256(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<__m256> \
	{ \
		static const std::size_t rank = 0; \
		static const std::size_t extent = 1; \
		static const std::size_t extent1 = 0; \
		static const std::size_t extent2 = 0; \
		typedef __m256 basic_value_type; \
		typedef __m256 value_type; \
		typedef __m256 sse_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastA<value_type> fastA_type; \
		typedef fastestA<value_type> fastestA_type; \
		typedef semiA<value_type> semiA_type; \
		typedef sseA<value_type> sseA_type; \
		typedef normA<value_type> normA_type; \
		typedef dummyA<value_type> dummyA_type; \
		value_type m_val; \
		inline operator value_type() const { return m_val; } \
		inline value_type operator*() const { return static_cast<value_type>(*this); } \
		inline CLASS_NAME& operator=(const value_type&& val); \
		inline CLASS_NAME& operator=(const value_type& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>&& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>& val); \
		inline CLASS_NAME& operator=(const normA<value_type>&& val); \
		inline CLASS_NAME& operator=(const normA<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>& val); \
		inline CLASS_NAME(const value_type&& val); \
		inline CLASS_NAME(const value_type& val); \
		inline CLASS_NAME(const fastA<value_type>&& val); \
		inline CLASS_NAME(const fastA<value_type>& val); \
		inline CLASS_NAME(const fastestA<value_type>&& val); \
		inline CLASS_NAME(const fastestA<value_type>& val); \
		inline CLASS_NAME(const semiA<value_type>&& val); \
		inline CLASS_NAME(const semiA<value_type>& val); \
		inline CLASS_NAME(const sseA<value_type>&& val); \
		inline CLASS_NAME(const sseA<value_type>& val); \
		inline CLASS_NAME(const normA<value_type>&& val); \
		inline CLASS_NAME(const normA<value_type>& val); \
		inline CLASS_NAME(const dummyA<value_type>&& val); \
		inline CLASS_NAME(const dummyA<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_M256D(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<__m256d> \
	{ \
		static const std::size_t rank = 0; \
		static const std::size_t extent = 1; \
		static const std::size_t extent1 = 0; \
		static const std::size_t extent2 = 0; \
		typedef __m256d basic_value_type; \
		typedef __m256d value_type; \
		typedef __m256d sse_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastA<value_type> fastA_type; \
		typedef fastestA<value_type> fastestA_type; \
		typedef semiA<value_type> semiA_type; \
		typedef sseA<value_type> sseA_type; \
		typedef normA<value_type> normA_type; \
		typedef dummyA<value_type> dummyA_type; \
		value_type m_val; \
		inline operator value_type() const { return m_val; } \
		inline value_type operator*() const { return static_cast<value_type>(*this); } \
		inline CLASS_NAME& operator=(const value_type&& val); \
		inline CLASS_NAME& operator=(const value_type& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>&& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>& val); \
		inline CLASS_NAME& operator=(const normA<value_type>&& val); \
		inline CLASS_NAME& operator=(const normA<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>& val); \
		inline CLASS_NAME(const value_type&& val); \
		inline CLASS_NAME(const value_type& val); \
		inline CLASS_NAME(const fastA<value_type>&& val); \
		inline CLASS_NAME(const fastA<value_type>& val); \
		inline CLASS_NAME(const fastestA<value_type>&& val); \
		inline CLASS_NAME(const fastestA<value_type>& val); \
		inline CLASS_NAME(const semiA<value_type>&& val); \
		inline CLASS_NAME(const semiA<value_type>& val); \
		inline CLASS_NAME(const sseA<value_type>&& val); \
		inline CLASS_NAME(const sseA<value_type>& val); \
		inline CLASS_NAME(const normA<value_type>&& val); \
		inline CLASS_NAME(const normA<value_type>& val); \
		inline CLASS_NAME(const dummyA<value_type>&& val); \
		inline CLASS_NAME(const dummyA<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_FLOAT3(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<float[3]> \
	{ \
		static const std::size_t rank = 1; \
		static const std::size_t extent = 3; \
		static const std::size_t extent1 = 3; \
		static const std::size_t extent2 = 0; \
		typedef float basic_value_type; \
		typedef float value_type[3]; \
		typedef __m128 sse_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastA<value_type> fastA_type; \
		typedef fastestA<value_type> fastestA_type; \
		typedef semiA<value_type> semiA_type; \
		typedef sseA<value_type> sseA_type; \
		typedef normA<value_type> normA_type; \
		typedef dummyA<value_type> dummyA_type; \
		union \
		{ \
			sse_type m_val; \
			value_type m_array; \
			float m_array4[4]; \
		}; \
		inline operator const value_type&() const { return m_array; } \
		inline operator value_type&(){ return m_array; } \
		inline operator sse_type() const { return m_val; } \
		inline float operator[](const int index) const { return m_array[index]; } \
		inline float& operator[](const int index){ return m_array[index]; } \
		inline const value_type& operator*() const { return m_array; } \
		inline value_type& operator*(){ return m_array; } \
		inline CLASS_NAME& operator=(const value_type&& val); \
		inline CLASS_NAME& operator=(const value_type& val); \
		inline CLASS_NAME& operator=(const sse_type&& val); \
		inline CLASS_NAME& operator=(const sse_type& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>&& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>& val); \
		inline CLASS_NAME& operator=(const normA<value_type>&& val); \
		inline CLASS_NAME& operator=(const normA<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>& val); \
		inline CLASS_NAME(const value_type&& val); \
		inline CLASS_NAME(const value_type& val); \
		inline CLASS_NAME(const sse_type&& val); \
		inline CLASS_NAME(const sse_type& val); \
		inline CLASS_NAME(const fastA<value_type>&& val); \
		inline CLASS_NAME(const fastA<value_type>& val); \
		inline CLASS_NAME(const fastestA<value_type>&& val); \
		inline CLASS_NAME(const fastestA<value_type>& val); \
		inline CLASS_NAME(const semiA<value_type>&& val); \
		inline CLASS_NAME(const semiA<value_type>& val); \
		inline CLASS_NAME(const sseA<value_type>&& val); \
		inline CLASS_NAME(const sseA<value_type>& val); \
		inline CLASS_NAME(const normA<value_type>&& val); \
		inline CLASS_NAME(const normA<value_type>& val); \
		inline CLASS_NAME(const dummyA<value_type>&& val); \
		inline CLASS_NAME(const dummyA<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_FLOAT4(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<float[4]> \
	{ \
		static const std::size_t rank = 1; \
		static const std::size_t extent = 4; \
		static const std::size_t extent1 = 4; \
		static const std::size_t extent2 = 0; \
		typedef float basic_value_type; \
		typedef float value_type[4]; \
		typedef __m128 sse_type; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastA<value_type> fastA_type; \
		typedef fastestA<value_type> fastestA_type; \
		typedef semiA<value_type> semiA_type; \
		typedef sseA<value_type> sseA_type; \
		typedef normA<value_type> normA_type; \
		typedef dummyA<value_type> dummyA_type; \
		union \
		{ \
			sse_type m_val; \
			value_type m_array; \
		}; \
		inline operator const value_type&() const { return m_array; } \
		inline operator value_type&(){ return m_array; } \
		inline operator sse_type() const { return m_val; } \
		inline float operator[](const int index) const { return m_array[index]; } \
		inline float& operator[](const int index){ return m_array[index]; } \
		inline const value_type& operator*() const { return static_cast<const value_type&>(*this); } \
		inline value_type& operator*(){ return static_cast<value_type&>(*this); } \
		inline CLASS_NAME& operator=(const value_type&& val); \
		inline CLASS_NAME& operator=(const value_type& val); \
		inline CLASS_NAME& operator=(const sse_type&& val); \
		inline CLASS_NAME& operator=(const sse_type& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>&& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>& val); \
		inline CLASS_NAME& operator=(const normA<value_type>&& val); \
		inline CLASS_NAME& operator=(const normA<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>& val); \
		inline CLASS_NAME(const value_type&& val); \
		inline CLASS_NAME(const value_type& val); \
		inline CLASS_NAME(const sse_type&& val); \
		inline CLASS_NAME(const sse_type& val); \
		inline CLASS_NAME(const fastA<value_type>&& val); \
		inline CLASS_NAME(const fastA<value_type>& val); \
		inline CLASS_NAME(const fastestA<value_type>&& val); \
		inline CLASS_NAME(const fastestA<value_type>& val); \
		inline CLASS_NAME(const semiA<value_type>&& val); \
		inline CLASS_NAME(const semiA<value_type>& val); \
		inline CLASS_NAME(const sseA<value_type>&& val); \
		inline CLASS_NAME(const sseA<value_type>& val); \
		inline CLASS_NAME(const normA<value_type>&& val); \
		inline CLASS_NAME(const normA<value_type>& val); \
		inline CLASS_NAME(const dummyA<value_type>&& val); \
		inline CLASS_NAME(const dummyA<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};
#define GASHA_FAST_ARITH_CLASS_SSE_FLOAT4X4(CLASS_NAME) \
	template <> \
	struct CLASS_NAME<float[4][4]> \
	{ \
		static const std::size_t rank = 2; \
		static const std::size_t extent = 4 * 4; \
		static const std::size_t extent1 = 4; \
		static const std::size_t extent2 = 4; \
		typedef float basic_value_type; \
		typedef __m128 basic_sse_type; \
		typedef float value_type[4][4]; \
		typedef __m128 sse_type[4]; \
		typedef CLASS_NAME<value_type> result_type; \
		typedef fastA<value_type> fastA_type; \
		typedef fastestA<value_type> fastestA_type; \
		typedef semiA<value_type> semiA_type; \
		typedef sseA<value_type> sseA_type; \
		typedef normA<value_type> normA_type; \
		typedef dummyA<value_type> dummyA_type; \
		union \
		{ \
			sse_type m_val; \
			value_type m_array; \
		}; \
		inline operator const value_type&() const { return m_array; } \
		inline operator value_type&(){ return m_array; } \
		inline operator const sse_type&() const { return m_val; } \
		inline operator sse_type&(){ return m_val; } \
		struct dim2 \
		{ \
			typedef basic_value_type dim2_value_type[extent2]; \
			dim2_value_type& m_val2; \
			inline basic_value_type operator[](int index) const { return m_val2[index]; } \
			inline basic_value_type& operator[](int index){ return m_val2[index]; } \
			inline dim2(const dim2_value_type& val2) : m_val2(const_cast<dim2_value_type&>(val2)){} \
			inline dim2(dim2_value_type& val2) : m_val2(val2){} \
		}; \
		inline const dim2 operator[](const int index) const { return dim2(m_array[index]); } \
		inline dim2 operator[](const int index){ return dim2(m_array[index]); } \
		inline const value_type& operator*() const { return static_cast<const value_type&>(*this); } \
		inline value_type& operator*(){ return static_cast<value_type&>(*this); } \
		inline CLASS_NAME& operator=(const value_type&& val); \
		inline CLASS_NAME& operator=(const value_type& val); \
		inline CLASS_NAME& operator=(const sse_type&& val); \
		inline CLASS_NAME& operator=(const sse_type& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastA<value_type>& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>&& val); \
		inline CLASS_NAME& operator=(const fastestA<value_type>& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>&& val); \
		inline CLASS_NAME& operator=(const semiA<value_type>& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>&& val); \
		inline CLASS_NAME& operator=(const sseA<value_type>& val); \
		inline CLASS_NAME& operator=(const normA<value_type>&& val); \
		inline CLASS_NAME& operator=(const normA<value_type>& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>&& val); \
		inline CLASS_NAME& operator=(const dummyA<value_type>& val); \
		inline CLASS_NAME(const value_type&& val); \
		inline CLASS_NAME(const value_type& val); \
		inline CLASS_NAME(const sse_type&& val); \
		inline CLASS_NAME(const sse_type& val); \
		inline CLASS_NAME(const fastA<value_type>&& val); \
		inline CLASS_NAME(const fastA<value_type>& val); \
		inline CLASS_NAME(const fastestA<value_type>&& val); \
		inline CLASS_NAME(const fastestA<value_type>& val); \
		inline CLASS_NAME(const semiA<value_type>&& val); \
		inline CLASS_NAME(const semiA<value_type>& val); \
		inline CLASS_NAME(const sseA<value_type>&& val); \
		inline CLASS_NAME(const sseA<value_type>& val); \
		inline CLASS_NAME(const normA<value_type>&& val); \
		inline CLASS_NAME(const normA<value_type>& val); \
		inline CLASS_NAME(const dummyA<value_type>&& val); \
		inline CLASS_NAME(const dummyA<value_type>& val); \
		inline CLASS_NAME(); \
		inline ~CLASS_NAME(); \
	};

#ifdef GASHA_FAST_ARITH_USE_SSE
#define GASHA_FAST_ARITH_CLASS_SSE(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_SSE_FLOAT(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_M128(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_FLOAT3(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_FLOAT4(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_FLOAT4X4(CLASS_NAME);
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
	GASHA_FAST_ARITH_ARRAY_CLASS(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE2(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_AVX(CLASS_NAME);

//----------------------------------------
//高速演算クラス（fast arithmetic）
//※データ型に基づき、適切な高速演算を適用する。
//※通常の演算に比べて、精度が若干落ちる。
//※float型／__m128型の場合、高速化のために、除算に逆数を用いる。
//　逆数は、近似値を元にニュートン法を1回適用して精度を高める。
//※その他、できる限りSSE命令を用いる。
//※内部でSSE型を用いるなどするため、データサイズが大きくなることがある。
//※SSE型のまま四則演算するので、演算を連続で行った時に高速になる。
//※演算結果として左辺値と同じ型（normA型）を返す。
GASHA_FAST_ARITH_CLASS_SET(fastA);
//----------------------------------------
//最高速演算クラス（fastest arithmetic）
//※データ型に基づき、適切な高速演算を適用する。
//※通常の演算に比べて、精度が悪い。
//※float型／__m128型の場合、高速化のために、除算に逆数を用いる。
//　逆数は、近似値をそのまま使用する。
//※その他、できる限りSSE命令を用いる。
//※内部でSSE型を用いるなどするため、データサイズが大きくなることがある。
//※SSE型のまま四則演算するので、演算を連続で行った時に高速になる。
//※演算結果として左辺値と同じ型（normA型）を返す。
GASHA_FAST_ARITH_CLASS_SET(fastestA);
//----------------------------------------
//準高速演算クラス（semi-fast arithmetic）
//※データ型に基づき、適切な高速演算を適用する。
//※通常の演算に比べて、精度が若干落ちるが、可能な限り高めるようにする。
//　通常演算より精度が高まることはない。
//※float型／__m128型の場合、高速化のために、除算に逆数を用いる。
//　逆数は、近似値を元にニュートン法を2回適用して精度を高める。
//※その他、できる限りSSE命令を用いる。
//※内部でSSE型を用いるなどするため、データサイズが大きくなることがある。
//※SSE型のまま四則演算するので、演算を連続で行った時に高速になる。
//※演算結果として左辺値と同じ型（normA型）を返す。
GASHA_FAST_ARITH_CLASS_SET(semiA);
//----------------------------------------
//SSE高速演算クラス（sse arithmetic）
//※データ型に基づき、適切な高速演算を適用する。
//※できる限りSSE命令を用いる。逆数を用いた除算は行わない。
//※SSE型のまま四則演算するので、演算を連続で行った時に高速になる。
//※演算結果として左辺値と同じ型（normA型）を返す。
GASHA_FAST_ARITH_CLASS_SET(sseA);
//----------------------------------------
//通常演算クラス（normal arithmetic）
//※そのままのデータ型で普通の演算を行う。
//※演算結果として左辺値と同じ型（normA型）を返す。
//※SSE命令型の場合はSSE命令を用いる。
GASHA_FAST_ARITH_CLASS_SET(normA);
//----------------------------------------
//ダミー演算クラス（dummy arithmetic）
//※値をコピーせず、元の値の参照を扱う。
//※他の型は演算結果として同じ型（fastA型など）を返すが、dummyAはfloatなどの元の値を返す。
//※最適化により、クラスを通さず直接値を扱うのと同等になる。
//※他の演算クラスと挙動やパフォーマンスの違いを確認したい時に用いる。
//※他の演算クラスとインターフェースが異なるので注意。
template <typename T>
struct dummyA
{
	static const std::size_t rank = 0;
	static const std::size_t extent = 1;
	static const std::size_t extent1 = 0;
	static const std::size_t extent2 = 0;
	typedef T basic_value_type;
	typedef T value_type;
	typedef value_type result_type;
	typedef fastA<value_type> fastA_type;
	typedef fastestA<value_type> fastestA_type;
	typedef semiA<value_type> semiA_type;
	typedef sseA<value_type> sseA_type;
	typedef normA<value_type> normA_type;
	typedef dummyA<value_type> dummyA_type;
	const value_type& m_val;
	inline operator value_type() const { return m_val; }
	value_type operator*() const { return m_val; }
	inline dummyA(const value_type&& val) : m_val(val){}
	inline dummyA(const value_type& val) : m_val(val){}
	dummyA() = delete;
	inline ~dummyA(){}
};
template <typename T, std::size_t N>
struct dummyA<T[N]>
{
	static const std::size_t rank = 1;
	static const std::size_t extent = N;
	static const std::size_t extent1 = N;
	static const std::size_t extent2 = 0;
	typedef T basic_value_type;
	typedef T value_type[N];
	typedef value_type result_type;
	typedef fastA<value_type> fastA_type;
	typedef fastestA<value_type> fastestA_type;
	typedef semiA<value_type> semiA_type;
	typedef sseA<value_type> sseA_type;
	typedef normA<value_type> normA_type;
	typedef dummyA<value_type> dummyA_type;
	const T(&m_val)[N];
	inline operator const value_type&() const { return m_val; }
	inline basic_value_type operator[](const int index) const { return m_val[index]; }
	inline const value_type& operator*() const { return static_cast<const value_type&>(*this); }
	inline dummyA(const value_type&& val) : m_val(val){}
	inline dummyA(const value_type& val) : m_val(val){}
	dummyA() = delete;
	inline ~dummyA(){}
};
template <typename T, std::size_t N, std::size_t M>
struct dummyA<T[N][M]>
{
	static const std::size_t rank = 2;
	static const std::size_t extent = N * M;
	static const std::size_t extent1 = N;
	static const std::size_t extent2 = M;
	typedef T basic_value_type;
	typedef T value_type[N][M];
	typedef value_type result_type;
	typedef fastA<value_type> fastA_type;
	typedef fastestA<value_type> fastestA_type;
	typedef semiA<value_type> semiA_type;
	typedef sseA<value_type> sseA_type;
	typedef normA<value_type> normA_type;
	typedef dummyA<value_type> dummyA_type;
	const basic_value_type(&m_val)[extent1][extent2];
	inline operator const value_type&() const { return m_val; }
	struct dim2
	{
		typedef basic_value_type dim2_value_type[extent2]; \
		const dim2_value_type& m_val2;
		inline basic_value_type operator[](int index) const { return m_val2[index]; }
		inline dim2(const dim2_value_type&val2) : m_val2(val2){}
	};
	inline const dim2 operator[](const int index) const { return dim2(m_val[index]); }
	inline const value_type& operator*() const { return static_cast<const value_type&>(*this); }
	inline dummyA(const value_type&& val) : m_val(val){}
	inline dummyA(const value_type& val) : m_val(val){}
	dummyA() = delete;
	inline ~dummyA(){}
};

//----------------------------------------
//高速演算クラスの別名
using fastA_f = fastA<float>;
using fastA_2f = fastA<float[2]>;
using fastA_3f = fastA<float[3]>;
using fastA_4f = fastA<float[4]>;
using fastA_22f = fastA<float[2][2]>;
using fastA_33f = fastA<float[3][3]>;
using fastA_44f = fastA<float[4][4]>;
using fastA_d = fastA<double>;
using fastA_2d = fastA<double[2]>;
using fastA_3d = fastA<double[3]>;
using fastA_4d = fastA<double[4]>;
using fastA_22d = fastA<double[2][2]>;
using fastA_33d = fastA<double[3][3]>;
using fastA_44d = fastA<double[4][4]>;
using fastestA_f = fastestA<float>;
using fastestA_2f = fastestA<float[2]>;
using fastestA_3f = fastestA<float[3]>;
using fastestA_4f = fastestA<float[4]>;
using fastestA_22f = fastestA<float[2][2]>;
using fastestA_33f = fastestA<float[3][3]>;
using fastestA_44f = fastestA<float[4][4]>;
using fastestA_d = fastestA<double>;
using fastestA_2d = fastestA<double[2]>;
using fastestA_3d = fastestA<double[3]>;
using fastestA_4d = fastestA<double[4]>;
using fastestA_22d = fastestA<double[2][2]>;
using fastestA_33d = fastestA<double[3][3]>;
using fastestA_44d = fastestA<double[4][4]>;
using semiA_f = semiA<float>;
using semiA_2f = semiA<float[2]>;
using semiA_3f = semiA<float[3]>;
using semiA_4f = semiA<float[4]>;
using semiA_22f = semiA<float[2][2]>;
using semiA_33f = semiA<float[3][3]>;
using semiA_44f = semiA<float[4][4]>;
using semiA_d = semiA<double>;
using semiA_2d = semiA<double[2]>;
using semiA_3d = semiA<double[3]>;
using semiA_4d = semiA<double[4]>;
using semiA_22d = semiA<double[2][2]>;
using semiA_33d = semiA<double[3][3]>;
using semiA_44d = semiA<double[4][4]>;
using sseA_f = sseA<float>;
using sseA_2f = sseA<float[2]>;
using sseA_3f = sseA<float[3]>;
using sseA_4f = sseA<float[4]>;
using sseA_22f = sseA<float[2][2]>;
using sseA_33f = sseA<float[3][3]>;
using sseA_44f = sseA<float[4][4]>;
using sseA_d = sseA<double>;
using sseA_2d = sseA<double[2]>;
using sseA_3d = sseA<double[3]>;
using sseA_4d = sseA<double[4]>;
using sseA_22d = sseA<double[2][2]>;
using sseA_33d = sseA<double[3][3]>;
using sseA_44d = sseA<double[4][4]>;
using normA_f = normA<float>;
using normA_2f = normA<float[2]>;
using normA_3f = normA<float[3]>;
using normA_4f = normA<float[4]>;
using normA_22f = normA<float[2][2]>;
using normA_33f = normA<float[3][3]>;
using normA_44f = normA<float[4][4]>;
using normA_d = normA<double>;
using normA_2d = normA<double[2]>;
using normA_3d = normA<double[3]>;
using normA_4d = normA<double[4]>;
using normA_22d = normA<double[2][2]>;
using normA_33d = normA<double[3][3]>;
using normA_44d = normA<double[4][4]>;
using dummyA_f = dummyA<float>;
using dummyA_2f = dummyA<float[2]>;
using dummyA_3f = dummyA<float[3]>;
using dummyA_4f = dummyA<float[4]>;
using dummyA_22f = dummyA<float[2][2]>;
using dummyA_33f = dummyA<float[3][3]>;
using dummyA_44f = dummyA<float[4][4]>;
using dummyA_d = dummyA<double>;
using dummyA_2d = dummyA<double[2]>;
using dummyA_3d = dummyA<double[3]>;
using dummyA_4d = dummyA<double[4]>;
using dummyA_22d = dummyA<double[2][2]>;
using dummyA_33d = dummyA<double[3][3]>;
using dummyA_44d = dummyA<double[4][4]>;

//----------------------------------------
//高速演算
#define GASHA_FAST_ARITH_OPERATOR(CLASS_NAME) \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>&& lvalue, const fastA<T>& rvalue){ return std::move(lvalue) + std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) + std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const fastA<T>& rvalue){ return std::move(lvalue) + std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>&& lvalue, const fastestA<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const fastestA<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>&& lvalue, const semiA<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const semiA<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>&& lvalue, const sseA<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const sseA<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>&& lvalue, const normA<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const normA<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>&& lvalue, const dummyA<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const dummyA<T>& rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>&& lvalue, const T rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator+(const CLASS_NAME<T>& lvalue, const T rvalue){ return std::move(lvalue) + CLASS_NAME<T>(rvalue); } \
	template<class T> inline T operator+(const T lvalue, const CLASS_NAME<T>&& rvalue){ return CLASS_NAME<T>(lvalue) +std::move(rvalue); } \
	template<class T> inline T operator+(const T lvalue, const CLASS_NAME<T>& rvalue){ return CLASS_NAME<T>(lvalue) +std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>&& lvalue, const fastA<T>& rvalue){ return std::move(lvalue) - std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) - std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const fastA<T>& rvalue){ return std::move(lvalue) - std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>&& lvalue, const fastestA<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const fastestA<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>&& lvalue, const semiA<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const semiA<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>&& lvalue, const sseA<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const sseA<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>&& lvalue, const normA<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const normA<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>&& lvalue, const dummyA<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const dummyA<T>& rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>&& lvalue, const T rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator-(const CLASS_NAME<T>& lvalue, const T rvalue){ return std::move(lvalue) - CLASS_NAME<T>(rvalue); } \
	template<class T> inline T operator-(const T lvalue, const CLASS_NAME<T>&& rvalue){ return CLASS_NAME<T>(lvalue) - std::move(rvalue); } \
	template<class T> inline T operator-(const T lvalue, const CLASS_NAME<T>& rvalue){ return CLASS_NAME<T>(lvalue) - std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>&& lvalue, const fastA<T>& rvalue){ return std::move(lvalue) * std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) * std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const fastA<T>& rvalue){ return std::move(lvalue) * std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>&& lvalue, const fastestA<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const fastestA<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>&& lvalue, const semiA<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const semiA<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>&& lvalue, const sseA<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const sseA<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>&& lvalue, const normA<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const normA<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>&& lvalue, const dummyA<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const dummyA<T>& rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>&& lvalue, const T rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator*(const CLASS_NAME<T>& lvalue, const T rvalue){ return std::move(lvalue) * CLASS_NAME<T>(rvalue); } \
	template<class T> inline T operator*(const T lvalue, const CLASS_NAME<T>&& rvalue){ return CLASS_NAME<T>(lvalue) * std::move(rvalue); } \
	template<class T> inline T operator*(const T lvalue, const CLASS_NAME<T>& rvalue){ return CLASS_NAME<T>(lvalue) * std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>&& lvalue, const fastA<T>& rvalue){ return std::move(lvalue) / std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) / std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const fastA<T>& rvalue){ return std::move(lvalue) / std::move(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>&& lvalue, const fastestA<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const fastestA<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>&& lvalue, const semiA<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const semiA<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>&& lvalue, const sseA<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const sseA<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>&& lvalue, const normA<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const normA<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>&& lvalue, const dummyA<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const dummyA<T>& rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>&& lvalue, const T rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline typename CLASS_NAME<T>::result_type operator/(const CLASS_NAME<T>& lvalue, const T rvalue){ return std::move(lvalue) / CLASS_NAME<T>(rvalue); } \
	template<class T> inline T operator/(const T lvalue, const CLASS_NAME<T>&& rvalue){ return CLASS_NAME<T>(lvalue) / std::move(rvalue); } \
	template<class T> inline T operator/(const T lvalue, const CLASS_NAME<T>& rvalue){ return CLASS_NAME<T>(lvalue) / std::move(rvalue); } \

template<class T> inline fastA<T> operator+(const fastA<T>&& lvalue, const fastA<T>&& rvalue){ return fastA<T>(*lvalue + *rvalue); }
template<class T> inline fastA<T> operator+(const fastA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) + fastA<T>(rvalue); }
template<class T> inline fastA<T> operator+(const fastA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) + fastA<T>(rvalue); }
template<class T> inline fastA<T> operator+(const fastA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) + fastA<T>(rvalue); }
template<class T> inline fastA<T> operator+(const fastA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) + fastA<T>(rvalue); }
template<class T> inline fastA<T> operator+(const fastA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) + fastA<T>(rvalue); }

template<class T> inline fastestA<T> operator+(const fastestA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) + fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator+(const fastestA<T>&& lvalue, const fastestA<T>&& rvalue){ return fastestA<T>(*lvalue + *rvalue); }
template<class T> inline fastestA<T> operator+(const fastestA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) + fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator+(const fastestA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) + fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator+(const fastestA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) + fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator+(const fastestA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) + fastestA<T>(rvalue); }

template<class T> inline semiA<T> operator+(const semiA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) + semiA<T>(rvalue); }
template<class T> inline semiA<T> operator+(const semiA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) + semiA<T>(rvalue); }
template<class T> inline semiA<T> operator+(const semiA<T>&& lvalue, const semiA<T>&& rvalue){ return semiA<T>(*lvalue + *rvalue); }
template<class T> inline semiA<T> operator+(const semiA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) + semiA<T>(rvalue); }
template<class T> inline semiA<T> operator+(const semiA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) + semiA<T>(rvalue); }
template<class T> inline semiA<T> operator+(const semiA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) + semiA<T>(rvalue); }

template<class T> inline sseA<T> operator+(const sseA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) + sseA<T>(rvalue); }
template<class T> inline sseA<T> operator+(const sseA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) + sseA<T>(rvalue); }
template<class T> inline sseA<T> operator+(const sseA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) + sseA<T>(rvalue); }
template<class T> inline sseA<T> operator+(const sseA<T>&& lvalue, const sseA<T>&& rvalue){ return sseA<T>(*lvalue + *rvalue); }
template<class T> inline sseA<T> operator+(const sseA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) + sseA<T>(rvalue); }
template<class T> inline sseA<T> operator+(const sseA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) + sseA<T>(rvalue); }

template<class T> inline normA<T> operator+(const normA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) + normA<T>(rvalue); }
template<class T> inline normA<T> operator+(const normA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) + normA<T>(rvalue); }
template<class T> inline normA<T> operator+(const normA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) + normA<T>(rvalue); }
template<class T> inline normA<T> operator+(const normA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) + normA<T>(rvalue); }
template<class T> inline normA<T> operator+(const normA<T>&& lvalue, const normA<T>&& rvalue){ return normA<T>(*lvalue + *rvalue); }
template<class T> inline normA<T> operator+(const normA<T>&& lvalue, const dummyA<T>&& rvalue){ return normA<T>(*lvalue + *rvalue); }

template<class T> inline T operator+(const dummyA<T>&& lvalue, const fastA<T>&& rvalue){ return *lvalue + *rvalue; }
template<class T> inline T operator+(const dummyA<T>&& lvalue, const fastestA<T>&& rvalue){ return *lvalue + *rvalue; }
template<class T> inline T operator+(const dummyA<T>&& lvalue, const semiA<T>&& rvalue){ return *lvalue + *rvalue; }
template<class T> inline T operator+(const dummyA<T>&& lvalue, const sseA<T>&& rvalue){ return *lvalue + *rvalue; }
template<class T> inline T operator+(const dummyA<T>&& lvalue, const normA<T>&& rvalue){ return *lvalue + *rvalue; }
template<class T> inline T operator+(const dummyA<T>&& lvalue, const dummyA<T>&& rvalue){ return *lvalue + *rvalue; }

template<class T> inline fastA<T> operator-(const fastA<T>&& lvalue, const fastA<T>&& rvalue){ return fastA<T>(*lvalue - *rvalue); }
template<class T> inline fastA<T> operator-(const fastA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) - fastA<T>(rvalue); }
template<class T> inline fastA<T> operator-(const fastA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) - fastA<T>(rvalue); }
template<class T> inline fastA<T> operator-(const fastA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) - fastA<T>(rvalue); }
template<class T> inline fastA<T> operator-(const fastA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) - fastA<T>(rvalue); }
template<class T> inline fastA<T> operator-(const fastA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) - fastA<T>(rvalue); }

template<class T> inline fastestA<T> operator-(const fastestA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) - fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator-(const fastestA<T>&& lvalue, const fastestA<T>&& rvalue){ return fastestA<T>(*lvalue - *rvalue); }
template<class T> inline fastestA<T> operator-(const fastestA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) - fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator-(const fastestA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) - fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator-(const fastestA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) - fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator-(const fastestA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) - fastestA<T>(rvalue); }

template<class T> inline semiA<T> operator-(const semiA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) - semiA<T>(rvalue); }
template<class T> inline semiA<T> operator-(const semiA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) - semiA<T>(rvalue); }
template<class T> inline semiA<T> operator-(const semiA<T>&& lvalue, const semiA<T>&& rvalue){ return semiA<T>(*lvalue - *rvalue); }
template<class T> inline semiA<T> operator-(const semiA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) - semiA<T>(rvalue); }
template<class T> inline semiA<T> operator-(const semiA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) - semiA<T>(rvalue); }
template<class T> inline semiA<T> operator-(const semiA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) - semiA<T>(rvalue); }

template<class T> inline sseA<T> operator-(const sseA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) - sseA<T>(rvalue); }
template<class T> inline sseA<T> operator-(const sseA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) - sseA<T>(rvalue); }
template<class T> inline sseA<T> operator-(const sseA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) - sseA<T>(rvalue); }
template<class T> inline sseA<T> operator-(const sseA<T>&& lvalue, const sseA<T>&& rvalue){ return sseA<T>(*lvalue - *rvalue); }
template<class T> inline sseA<T> operator-(const sseA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) - sseA<T>(rvalue); }
template<class T> inline sseA<T> operator-(const sseA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) - sseA<T>(rvalue); }

template<class T> inline normA<T> operator-(const normA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) - normA<T>(rvalue); }
template<class T> inline normA<T> operator-(const normA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) - normA<T>(rvalue); }
template<class T> inline normA<T> operator-(const normA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) - normA<T>(rvalue); }
template<class T> inline normA<T> operator-(const normA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) - normA<T>(rvalue); }
template<class T> inline normA<T> operator-(const normA<T>&& lvalue, const normA<T>&& rvalue){ return normA<T>(*lvalue - *rvalue); }
template<class T> inline normA<T> operator-(const normA<T>&& lvalue, const dummyA<T>&& rvalue){ return normA<T>(*lvalue - *rvalue); }

template<class T> inline T operator-(const dummyA<T>&& lvalue, const fastA<T>&& rvalue){ return *lvalue - *rvalue; }
template<class T> inline T operator-(const dummyA<T>&& lvalue, const fastestA<T>&& rvalue){ return *lvalue - *rvalue; }
template<class T> inline T operator-(const dummyA<T>&& lvalue, const semiA<T>&& rvalue){ return *lvalue - *rvalue; }
template<class T> inline T operator-(const dummyA<T>&& lvalue, const sseA<T>&& rvalue){ return *lvalue - *rvalue; }
template<class T> inline T operator-(const dummyA<T>&& lvalue, const normA<T>&& rvalue){ return *lvalue - *rvalue; }
template<class T> inline T operator-(const dummyA<T>&& lvalue, const dummyA<T>&& rvalue){ return *lvalue - *rvalue; }

template<class T> inline fastA<T> operator*(const fastA<T>&& lvalue, const fastA<T>&& rvalue){ return fastA<T>(*lvalue * *rvalue); }
template<class T> inline fastA<T> operator*(const fastA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) * fastA<T>(rvalue); }
template<class T> inline fastA<T> operator*(const fastA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) * fastA<T>(rvalue); }
template<class T> inline fastA<T> operator*(const fastA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) * fastA<T>(rvalue); }
template<class T> inline fastA<T> operator*(const fastA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) * fastA<T>(rvalue); }
template<class T> inline fastA<T> operator*(const fastA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) * fastA<T>(rvalue); }

template<class T> inline fastestA<T> operator*(const fastestA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) * fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator*(const fastestA<T>&& lvalue, const fastestA<T>&& rvalue){ return fastestA<T>(*lvalue * *rvalue); }
template<class T> inline fastestA<T> operator*(const fastestA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) * fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator*(const fastestA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) * fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator*(const fastestA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) * fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator*(const fastestA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) * fastestA<T>(rvalue); }

template<class T> inline semiA<T> operator*(const semiA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) * semiA<T>(rvalue); }
template<class T> inline semiA<T> operator*(const semiA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) * semiA<T>(rvalue); }
template<class T> inline semiA<T> operator*(const semiA<T>&& lvalue, const semiA<T>&& rvalue){ return semiA<T>(*lvalue * *rvalue); }
template<class T> inline semiA<T> operator*(const semiA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) * semiA<T>(rvalue); }
template<class T> inline semiA<T> operator*(const semiA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) * semiA<T>(rvalue); }
template<class T> inline semiA<T> operator*(const semiA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) * semiA<T>(rvalue); }

template<class T> inline sseA<T> operator*(const sseA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) * sseA<T>(rvalue); }
template<class T> inline sseA<T> operator*(const sseA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) * sseA<T>(rvalue); }
template<class T> inline sseA<T> operator*(const sseA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) * sseA<T>(rvalue); }
template<class T> inline sseA<T> operator*(const sseA<T>&& lvalue, const sseA<T>&& rvalue){ return sseA<T>(*lvalue * *rvalue); }
template<class T> inline sseA<T> operator*(const sseA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) * sseA<T>(rvalue); }
template<class T> inline sseA<T> operator*(const sseA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) * sseA<T>(rvalue); }

template<class T> inline normA<T> operator*(const normA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) * normA<T>(rvalue); }
template<class T> inline normA<T> operator*(const normA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) * normA<T>(rvalue); }
template<class T> inline normA<T> operator*(const normA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) * normA<T>(rvalue); }
template<class T> inline normA<T> operator*(const normA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) * normA<T>(rvalue); }
template<class T> inline normA<T> operator*(const normA<T>&& lvalue, const normA<T>&& rvalue){ return normA<T>(*lvalue * *rvalue); }
template<class T> inline normA<T> operator*(const normA<T>&& lvalue, const dummyA<T>&& rvalue){ return normA<T>(*lvalue * *rvalue); }

template<class T> inline T operator*(const dummyA<T>&& lvalue, const fastA<T>&& rvalue){ return *lvalue * *rvalue; }
template<class T> inline T operator*(const dummyA<T>&& lvalue, const fastestA<T>&& rvalue){ return *lvalue * *rvalue; }
template<class T> inline T operator*(const dummyA<T>&& lvalue, const semiA<T>&& rvalue){ return *lvalue * *rvalue; }
template<class T> inline T operator*(const dummyA<T>&& lvalue, const sseA<T>&& rvalue){ return *lvalue * *rvalue; }
template<class T> inline T operator*(const dummyA<T>&& lvalue, const normA<T>&& rvalue){ return *lvalue * *rvalue; }
template<class T> inline T operator*(const dummyA<T>&& lvalue, const dummyA<T>&& rvalue){ return *lvalue * *rvalue; }

template<class T> inline fastA<T> operator/(const fastA<T>&& lvalue, const fastA<T>&& rvalue){ return fastA<T>(*lvalue / *rvalue); }
template<class T> inline fastA<T> operator/(const fastA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) / fastA<T>(rvalue); }
template<class T> inline fastA<T> operator/(const fastA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) / fastA<T>(rvalue); }
template<class T> inline fastA<T> operator/(const fastA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) / fastA<T>(rvalue); }
template<class T> inline fastA<T> operator/(const fastA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) / fastA<T>(rvalue); }
template<class T> inline fastA<T> operator/(const fastA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) / fastA<T>(rvalue); }

template<class T> inline fastestA<T> operator/(const fastestA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) / fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator/(const fastestA<T>&& lvalue, const fastestA<T>&& rvalue){ return fastestA<T>(*lvalue / *rvalue); }
template<class T> inline fastestA<T> operator/(const fastestA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) / fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator/(const fastestA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) / fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator/(const fastestA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) / fastestA<T>(rvalue); }
template<class T> inline fastestA<T> operator/(const fastestA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) / fastestA<T>(rvalue); }

template<class T> inline semiA<T> operator/(const semiA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) / semiA<T>(rvalue); }
template<class T> inline semiA<T> operator/(const semiA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) / semiA<T>(rvalue); }
template<class T> inline semiA<T> operator/(const semiA<T>&& lvalue, const semiA<T>&& rvalue){ return semiA<T>(*lvalue / *rvalue); }
template<class T> inline semiA<T> operator/(const semiA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) / semiA<T>(rvalue); }
template<class T> inline semiA<T> operator/(const semiA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) / semiA<T>(rvalue); }
template<class T> inline semiA<T> operator/(const semiA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) / semiA<T>(rvalue); }

template<class T> inline sseA<T> operator/(const sseA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) / sseA<T>(rvalue); }
template<class T> inline sseA<T> operator/(const sseA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) / sseA<T>(rvalue); }
template<class T> inline sseA<T> operator/(const sseA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) / sseA<T>(rvalue); }
template<class T> inline sseA<T> operator/(const sseA<T>&& lvalue, const sseA<T>&& rvalue){ return sseA<T>(*lvalue / *rvalue); }
template<class T> inline sseA<T> operator/(const sseA<T>&& lvalue, const normA<T>&& rvalue){ return std::move(lvalue) / sseA<T>(rvalue); }
template<class T> inline sseA<T> operator/(const sseA<T>&& lvalue, const dummyA<T>&& rvalue){ return std::move(lvalue) / sseA<T>(rvalue); }

template<class T> inline normA<T> operator/(const normA<T>&& lvalue, const fastA<T>&& rvalue){ return std::move(lvalue) / normA<T>(rvalue); }
template<class T> inline normA<T> operator/(const normA<T>&& lvalue, const fastestA<T>&& rvalue){ return std::move(lvalue) / normA<T>(rvalue); }
template<class T> inline normA<T> operator/(const normA<T>&& lvalue, const semiA<T>&& rvalue){ return std::move(lvalue) / normA<T>(rvalue); }
template<class T> inline normA<T> operator/(const normA<T>&& lvalue, const sseA<T>&& rvalue){ return std::move(lvalue) / normA<T>(rvalue); }
template<class T> inline normA<T> operator/(const normA<T>&& lvalue, const normA<T>&& rvalue){ return normA<T>(*lvalue / *rvalue); }
template<class T> inline normA<T> operator/(const normA<T>&& lvalue, const dummyA<T>&& rvalue){ return normA<T>(*lvalue / *rvalue); }

template<class T> inline T operator/(const dummyA<T>&& lvalue, const fastA<T>&& rvalue){ return *lvalue / *rvalue; }
template<class T> inline T operator/(const dummyA<T>&& lvalue, const fastestA<T>&& rvalue){ return *lvalue / *rvalue; }
template<class T> inline T operator/(const dummyA<T>&& lvalue, const semiA<T>&& rvalue){ return *lvalue / *rvalue; }
template<class T> inline T operator/(const dummyA<T>&& lvalue, const sseA<T>&& rvalue){ return *lvalue / *rvalue; }
template<class T> inline T operator/(const dummyA<T>&& lvalue, const normA<T>&& rvalue){ return *lvalue / *rvalue; }
template<class T> inline T operator/(const dummyA<T>&& lvalue, const dummyA<T>&& rvalue){ return *lvalue / *rvalue; }

GASHA_FAST_ARITH_OPERATOR(fastA);
GASHA_FAST_ARITH_OPERATOR(fastestA);
GASHA_FAST_ARITH_OPERATOR(semiA);
GASHA_FAST_ARITH_OPERATOR(sseA);
GASHA_FAST_ARITH_OPERATOR(normA);

//----------------------------------------
//高速演算：特殊化
#ifdef GASHA_FAST_ARITH_USE_SSE
template<> inline fastA<float> operator+(const fastA<float>&& lvalue, const fastA<float>&& rvalue);
template<> inline fastA<__m128> operator+(const fastA<__m128>&& lvalue, const fastA<__m128>&& rvalue);

template<> inline fastA<float> operator-(const fastA<float>&& lvalue, const fastA<float>&& rvalue);
template<> inline fastA<__m128> operator-(const fastA<__m128>&& lvalue, const fastA<__m128>&& rvalue);

template<> inline fastA<float> operator*(const fastA<float>&& lvalue, const fastA<float>&& rvalue);
template<> inline fastA<__m128> operator*(const fastA<__m128>&& lvalue, const fastA<__m128>&& rvalue);

template<> inline fastA<float> operator/(const fastA<float>&& lvalue, const fastA<float>&& rvalue);
template<> inline fastA<__m128> operator/(const fastA<__m128>&& lvalue, const fastA<__m128>&& rvalue);

template<> inline fastestA<float> operator+(const fastestA<float>&& lvalue, const fastestA<float>&& rvalue);
template<> inline fastestA<__m128> operator+(const fastestA<__m128>&& lvalue, const fastestA<__m128>&& rvalue);

template<> inline fastestA<float> operator-(const fastestA<float>&& lvalue, const fastestA<float>&& rvalue);
template<> inline fastestA<__m128> operator-(const fastestA<__m128>&& lvalue, const fastestA<__m128>&& rvalue);

template<> inline fastestA<float> operator*(const fastestA<float>&& lvalue, const fastestA<float>&& rvalue);
template<> inline fastestA<__m128> operator*(const fastestA<__m128>&& lvalue, const fastestA<__m128>&& rvalue);

template<> inline fastestA<float> operator/(const fastestA<float>&& lvalue, const fastestA<float>&& rvalue);
template<> inline fastestA<__m128> operator/(const fastestA<__m128>&& lvalue, const fastestA<__m128>&& rvalue);

template<> inline semiA<float> operator+(const semiA<float>&& lvalue, const semiA<float>&& rvalue);
template<> inline semiA<__m128> operator+(const semiA<__m128>&& lvalue, const semiA<__m128>&& rvalue);

template<> inline semiA<float> operator-(const semiA<float>&& lvalue, const semiA<float>&& rvalue);
template<> inline semiA<__m128> operator-(const semiA<__m128>&& lvalue, const semiA<__m128>&& rvalue);

template<> inline semiA<float> operator*(const semiA<float>&& lvalue, const semiA<float>&& rvalue);
template<> inline semiA<__m128> operator*(const semiA<__m128>&& lvalue, const semiA<__m128>&& rvalue);

template<> inline semiA<float> operator/(const semiA<float>&& lvalue, const semiA<float>&& rvalue);
template<> inline semiA<__m128> operator/(const semiA<__m128>&& lvalue, const semiA<__m128>&& rvalue);

template<> inline sseA<float> operator+(const sseA<float>&& lvalue, const sseA<float>&& rvalue);
template<> inline sseA<__m128> operator+(const sseA<__m128>&& lvalue, const sseA<__m128>&& rvalue);

template<> inline sseA<float> operator-(const sseA<float>&& lvalue, const sseA<float>&& rvalue);
template<> inline sseA<__m128> operator-(const sseA<__m128>&& lvalue, const sseA<__m128>&& rvalue);

template<> inline sseA<float> operator*(const sseA<float>&& lvalue, const sseA<float>&& rvalue);
template<> inline sseA<__m128> operator*(const sseA<__m128>&& lvalue, const sseA<__m128>&& rvalue);

template<> inline sseA<float> operator/(const sseA<float>&& lvalue, const sseA<float>&& rvalue);
template<> inline sseA<__m128> operator/(const sseA<__m128>&& lvalue, const sseA<__m128>&& rvalue);

template<> inline normA<__m128> operator+(const normA<__m128>&& lvalue, const normA<__m128>&& rvalue);

template<> inline normA<__m128> operator-(const normA<__m128>&& lvalue, const normA<__m128>&& rvalue);

template<> inline normA<__m128> operator*(const normA<__m128>&& lvalue, const normA<__m128>&& rvalue);

template<> inline normA<__m128> operator/(const normA<__m128>&& lvalue, const normA<__m128>&& rvalue);

template<> inline __m128 operator+(const dummyA<__m128>&& lvalue, const dummyA<__m128>&& rvalue);

template<> inline __m128 operator-(const dummyA<__m128>&& lvalue, const dummyA<__m128>&& rvalue);

template<> inline __m128 operator*(const dummyA<__m128>&& lvalue, const dummyA<__m128>&& rvalue);

template<> inline __m128 operator/(const dummyA<__m128>&& lvalue, const dummyA<__m128>&& rvalue);
#endif//GASHA_FAST_ARITH_USE_SSE
#ifdef GASHA_FAST_ARITH_USE_SSE2
template<> inline fastA<double> operator+(const fastA<double>&& lvalue, const fastA<double>&& rvalue);
template<> inline fastA<__m128d> operator+(const fastA<__m128d>&& lvalue, const fastA<__m128d>&& rvalue);

template<> inline fastA<double> operator-(const fastA<double>&& lvalue, const fastA<double>&& rvalue);
template<> inline fastA<__m128d> operator-(const fastA<__m128d>&& lvalue, const fastA<__m128d>&& rvalue);

template<> inline fastA<double> operator*(const fastA<double>&& lvalue, const fastA<double>&& rvalue);
template<> inline fastA<__m128d> operator*(const fastA<__m128d>&& lvalue, const fastA<__m128d>&& rvalue);

template<> inline fastA<double> operator/(const fastA<double>&& lvalue, const fastA<double>&& rvalue);
template<> inline fastA<__m128d> operator/(const fastA<__m128d>&& lvalue, const fastA<__m128d>&& rvalue);

template<> inline fastestA<double> operator+(const fastestA<double>&& lvalue, const fastestA<double>&& rvalue);
template<> inline fastestA<__m128d> operator+(const fastestA<__m128d>&& lvalue, const fastestA<__m128d>&& rvalue);

template<> inline fastestA<double> operator-(const fastestA<double>&& lvalue, const fastestA<double>&& rvalue);
template<> inline fastestA<__m128d> operator-(const fastestA<__m128d>&& lvalue, const fastestA<__m128d>&& rvalue);

template<> inline fastestA<double> operator*(const fastestA<double>&& lvalue, const fastestA<double>&& rvalue);
template<> inline fastestA<__m128d> operator*(const fastestA<__m128d>&& lvalue, const fastestA<__m128d>&& rvalue);

template<> inline fastestA<double> operator/(const fastestA<double>&& lvalue, const fastestA<double>&& rvalue);
template<> inline fastestA<__m128d> operator/(const fastestA<__m128d>&& lvalue, const fastestA<__m128d>&& rvalue);

template<> inline semiA<double> operator+(const semiA<double>&& lvalue, const semiA<double>&& rvalue);
template<> inline semiA<__m128d> operator+(const semiA<__m128d>&& lvalue, const semiA<__m128d>&& rvalue);

template<> inline semiA<double> operator-(const semiA<double>&& lvalue, const semiA<double>&& rvalue);
template<> inline semiA<__m128d> operator-(const semiA<__m128d>&& lvalue, const semiA<__m128d>&& rvalue);

template<> inline semiA<double> operator*(const semiA<double>&& lvalue, const semiA<double>&& rvalue);
template<> inline semiA<__m128d> operator*(const semiA<__m128d>&& lvalue, const semiA<__m128d>&& rvalue);

template<> inline semiA<double> operator/(const semiA<double>&& lvalue, const semiA<double>&& rvalue);
template<> inline semiA<__m128d> operator/(const semiA<__m128d>&& lvalue, const semiA<__m128d>&& rvalue);

template<> inline sseA<double> operator+(const sseA<double>&& lvalue, const sseA<double>&& rvalue);
template<> inline sseA<__m128d> operator+(const sseA<__m128d>&& lvalue, const sseA<__m128d>&& rvalue);

template<> inline sseA<double> operator-(const sseA<double>&& lvalue, const sseA<double>&& rvalue);
template<> inline sseA<__m128d> operator-(const sseA<__m128d>&& lvalue, const sseA<__m128d>&& rvalue);

template<> inline sseA<double> operator*(const sseA<double>&& lvalue, const sseA<double>&& rvalue);
template<> inline sseA<__m128d> operator*(const sseA<__m128d>&& lvalue, const sseA<__m128d>&& rvalue);

template<> inline sseA<double> operator/(const sseA<double>&& lvalue, const sseA<double>&& rvalue);
template<> inline sseA<__m128d> operator/(const sseA<__m128d>&& lvalue, const sseA<__m128d>&& rvalue);

template<> inline normA<__m128d> operator+(const normA<__m128d>&& lvalue, const normA<__m128d>&& rvalue);

template<> inline normA<__m128d> operator-(const normA<__m128d>&& lvalue, const normA<__m128d>&& rvalue);

template<> inline normA<__m128d> operator*(const normA<__m128d>&& lvalue, const normA<__m128d>&& rvalue);

template<> inline normA<__m128d> operator/(const normA<__m128d>&& lvalue, const normA<__m128d>&& rvalue);

template<> inline __m128d operator+(const dummyA<__m128d>&& lvalue, const dummyA<__m128d>&& rvalue);

template<> inline __m128d operator-(const dummyA<__m128d>&& lvalue, const dummyA<__m128d>&& rvalue);

template<> inline __m128d operator*(const dummyA<__m128d>&& lvalue, const dummyA<__m128d>&& rvalue);

template<> inline __m128d operator/(const dummyA<__m128d>&& lvalue, const dummyA<__m128d>&& rvalue);
#endif//GASHA_FAST_ARITH_USE_SSE2
#ifdef GASHA_FAST_ARITH_USE_AVX
template<> inline fastA<__m256> operator+(const fastA<__m256>&& lvalue, const fastA<__m256>&& rvalue);
template<> inline fastA<__m256d> operator+(const fastA<__m256d>&& lvalue, const fastA<__m256d>&& rvalue);

template<> inline fastA<__m256> operator-(const fastA<__m256>&& lvalue, const fastA<__m256>&& rvalue);
template<> inline fastA<__m256d> operator-(const fastA<__m256d>&& lvalue, const fastA<__m256d>&& rvalue);

template<> inline fastA<__m256> operator*(const fastA<__m256>&& lvalue, const fastA<__m256>&& rvalue);
template<> inline fastA<__m256d> operator*(const fastA<__m256d>&& lvalue, const fastA<__m256d>&& rvalue);

template<> inline fastA<__m256> operator/(const fastA<__m256>&& lvalue, const fastA<__m256>&& rvalue);
template<> inline fastA<__m256d> operator/(const fastA<__m256d>&& lvalue, const fastA<__m256d>&& rvalue);

template<> inline fastestA<__m256> operator+(const fastestA<__m256>&& lvalue, const fastestA<__m256>&& rvalue);
template<> inline fastestA<__m256d> operator+(const fastestA<__m256d>&& lvalue, const fastestA<__m256d>&& rvalue);

template<> inline fastestA<__m256> operator-(const fastestA<__m256>&& lvalue, const fastestA<__m256>&& rvalue);
template<> inline fastestA<__m256d> operator-(const fastestA<__m256d>&& lvalue, const fastestA<__m256d>&& rvalue);

template<> inline fastestA<__m256> operator*(const fastestA<__m256>&& lvalue, const fastestA<__m256>&& rvalue);
template<> inline fastestA<__m256d> operator*(const fastestA<__m256d>&& lvalue, const fastestA<__m256d>&& rvalue);

template<> inline fastestA<__m256> operator/(const fastestA<__m256>&& lvalue, const fastestA<__m256>&& rvalue);
template<> inline fastestA<__m256d> operator/(const fastestA<__m256d>&& lvalue, const fastestA<__m256d>&& rvalue);

template<> inline semiA<__m256> operator+(const semiA<__m256>&& lvalue, const semiA<__m256>&& rvalue);
template<> inline semiA<__m256d> operator+(const semiA<__m256d>&& lvalue, const semiA<__m256d>&& rvalue);

template<> inline semiA<__m256> operator-(const semiA<__m256>&& lvalue, const semiA<__m256>&& rvalue);
template<> inline semiA<__m256d> operator-(const semiA<__m256d>&& lvalue, const semiA<__m256d>&& rvalue);

template<> inline semiA<__m256> operator*(const semiA<__m256>&& lvalue, const semiA<__m256>&& rvalue);
template<> inline semiA<__m256d> operator*(const semiA<__m256d>&& lvalue, const semiA<__m256d>&& rvalue);

template<> inline semiA<__m256> operator/(const semiA<__m256>&& lvalue, const semiA<__m256>&& rvalue);
template<> inline semiA<__m256d> operator/(const semiA<__m256d>&& lvalue, const semiA<__m256d>&& rvalue);

template<> inline sseA<__m256> operator+(const sseA<__m256>&& lvalue, const sseA<__m256>&& rvalue);
template<> inline sseA<__m256d> operator+(const sseA<__m256d>&& lvalue, const sseA<__m256d>&& rvalue);

template<> inline sseA<__m256> operator-(const sseA<__m256>&& lvalue, const sseA<__m256>&& rvalue);
template<> inline sseA<__m256d> operator-(const sseA<__m256d>&& lvalue, const sseA<__m256d>&& rvalue);

template<> inline sseA<__m256> operator*(const sseA<__m256>&& lvalue, const sseA<__m256>&& rvalue);
template<> inline sseA<__m256d> operator*(const sseA<__m256d>&& lvalue, const sseA<__m256d>&& rvalue);

template<> inline sseA<__m256> operator/(const sseA<__m256>&& lvalue, const sseA<__m256>&& rvalue);
template<> inline sseA<__m256d> operator/(const sseA<__m256d>&& lvalue, const sseA<__m256d>&& rvalue);

template<> inline normA<__m256> operator+(const normA<__m256>&& lvalue, const normA<__m256>&& rvalue);
template<> inline normA<__m256d> operator+(const normA<__m256d>&& lvalue, const normA<__m256d>&& rvalue);

template<> inline normA<__m256> operator-(const normA<__m256>&& lvalue, const normA<__m256>&& rvalue);
template<> inline normA<__m256d> operator-(const normA<__m256d>&& lvalue, const normA<__m256d>&& rvalue);

template<> inline normA<__m256> operator*(const normA<__m256>&& lvalue, const normA<__m256>&& rvalue);
template<> inline normA<__m256d> operator*(const normA<__m256d>&& lvalue, const normA<__m256d>&& rvalue);

template<> inline normA<__m256> operator/(const normA<__m256>&& lvalue, const normA<__m256>&& rvalue);
template<> inline normA<__m256d> operator/(const normA<__m256d>&& lvalue, const normA<__m256d>&& rvalue);

template<> inline __m256 operator+(const dummyA<__m256>&& lvalue, const dummyA<__m256>&& rvalue);
template<> inline __m256d operator+(const dummyA<__m256d>&& lvalue, const dummyA<__m256d>&& rvalue);

template<> inline __m256 operator-(const dummyA<__m256>&& lvalue, const dummyA<__m256>&& rvalue);
template<> inline __m256d operator-(const dummyA<__m256d>&& lvalue, const dummyA<__m256d>&& rvalue);

template<> inline __m256 operator*(const dummyA<__m256>&& lvalue, const dummyA<__m256>&& rvalue);
template<> inline __m256d operator*(const dummyA<__m256d>&& lvalue, const dummyA<__m256d>&& rvalue);

template<> inline __m256 operator/(const dummyA<__m256>&& lvalue, const dummyA<__m256>&& rvalue);
template<> inline __m256d operator/(const dummyA<__m256d>&& lvalue, const dummyA<__m256d>&& rvalue);
#endif//GASHA_FAST_ARITH_USE_AVX

//--------------------------------------------------------------------------------
//高速平方根
//--------------------------------------------------------------------------------

//----------------------------------------
//高速平方根
template<typename T> inline fastA<T> sqr(const fastA<T>&& value);
template<typename T> inline fastA<T> sqr(const fastA<T>& value){ return sqr(std::move(value)); }
template<typename T> inline fastestA<T> sqr(const fastestA<T>&& value);
template<typename T> inline fastestA<T> sqr(const fastestA<T>& value){ return sqr(std::move(value)); }
template<typename T> inline semiA<T> sqr(const semiA<T>&& value);
template<typename T> inline semiA<T> sqr(const semiA<T>& value){ return sqr(std::move(value)); }
template<typename T> inline sseA<T> sqr(const sseA<T>&& value);
template<typename T> inline sseA<T> sqr(const sseA<T>& value){ return sqr(std::move(value)); }
template<typename T> inline normA<T> sqr(const normA<T>&& value);
template<typename T> inline normA<T> sqr(const normA<T>& value){ return sqr(std::move(value)); }
template<typename T> inline T sqr(const dummyA<T>&& value);
template<typename T> inline T sqr(const dummyA<T>& value){ return sqr(std::move(value)); }

//----------------------------------------
//高速平方根：特殊化
#ifdef GASHA_FAST_ARITH_USE_SSE
template<> inline fastA<float> sqr(const fastA<float>&& value);
template<> inline fastestA<float> sqr(const fastestA<float>&& value);
template<> inline semiA<float> sqr(const semiA<float>&& value);
template<> inline sseA<float> sqr(const sseA<float>&& value);

template<> inline fastA<__m128> sqr(const fastA<__m128>&& value);
template<> inline fastestA<__m128> sqr(const fastestA<__m128>&& value);
template<> inline semiA<__m128> sqr(const semiA<__m128>&& value);
template<> inline sseA<__m128> sqr(const sseA<__m128>&& value);
template<> inline normA<__m128> sqr(const normA<__m128>&& value);
template<> inline __m128 sqr(const dummyA<__m128>&& value);
#endif//GASHA_FAST_ARITH_USE_SSE
#ifdef GASHA_FAST_ARITH_USE_SSE2
template<> inline fastA<double> sqr(const fastA<double>&& value);
template<> inline fastestA<double> sqr(const fastestA<double>&& value);
template<> inline semiA<double> sqr(const semiA<double>&& value);
template<> inline sseA<double> sqr(const sseA<double>&& value);

template<> inline fastA<__m128d> sqr(const fastA<__m128d>&& value);
template<> inline fastestA<__m128d> sqr(const fastestA<__m128d>&& value);
template<> inline semiA<__m128d> sqr(const semiA<__m128d>&& value);
template<> inline sseA<__m128d> sqr(const sseA<__m128d>&& value);
template<> inline normA<__m128d> sqr(const normA<__m128d>&& value);
template<> inline __m128d sqr(const dummyA<__m128d>&& value);
#endif//GASHA_FAST_ARITH_USE_SSE2
#ifdef GASHA_FAST_ARITH_USE_AVX
template<> inline fastA<__m256> sqr(const fastA<__m256>&& value);
template<> inline fastestA<__m256> sqr(const fastestA<__m256>&& value);
template<> inline semiA<__m256> sqr(const semiA<__m256>&& value);
template<> inline sseA<__m256> sqr(const sseA<__m256>&& value);
template<> inline normA<__m256> sqr(const normA<__m256>&& value);
template<> inline __m256 sqr(const dummyA<__m256>&& value);

template<> inline fastA<__m256d> sqr(const fastA<__m256d>&& value);
template<> inline fastestA<__m256d> sqr(const fastestA<__m256d>&& value);
template<> inline semiA<__m256d> sqr(const semiA<__m256d>&& value);
template<> inline sseA<__m256d> sqr(const sseA<__m256d>&& value);
template<> inline normA<__m256d> sqr(const normA<__m256d>&& value);
template<> inline __m256d sqr(const dummyA<__m256d>&& value);
#endif//GASHA_FAST_ARITH_USE_AVX

//--------------------------------------------------------------------------------
//高速ベクトル演算
//--------------------------------------------------------------------------------

#define GASHA_FAST_VECTOR_SET(CLASS_NAME) \
	template<typename T, std::size_t N> inline T norm(const CLASS_NAME<T[N]>& vec); \
	template<typename T, std::size_t N> inline T normSq(const CLASS_NAME<T[N]>& vec); \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> merge(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2); \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> difference(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2); \
	template<typename T, std::size_t N> inline T length(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2); \
	template<typename T, std::size_t N> inline T lengthSq(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2); \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> normalize(const CLASS_NAME<T[N]>& vec); \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> mul(const CLASS_NAME<T[N]>& vec, const T scalar); \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> forward(const CLASS_NAME<T[N]>& vec, const T scalar); \
	template<typename T, std::size_t N> inline T dot(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2); \
	template<typename T, std::size_t N> inline T normalizedDot(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2); \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> cross(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2);

GASHA_FAST_VECTOR_SET(fastA);
GASHA_FAST_VECTOR_SET(fastestA);
GASHA_FAST_VECTOR_SET(semiA);
GASHA_FAST_VECTOR_SET(sseA);
GASHA_FAST_VECTOR_SET(normA);
template<typename T, std::size_t N> inline T norm(const dummyA<T[N]>& vec);
template<typename T, std::size_t N> inline T normSq(const dummyA<T[N]>& vec);
template<typename T, std::size_t N> inline void merge(T (&result)[N], const dummyA<T[N]>& vec1, const dummyA<T[N]>& vec2);
template<typename T, std::size_t N> inline void difference(T (&result)[N], const dummyA<T[N]>& vec1, const dummyA<T[N]>& vec2);
template<typename T, std::size_t N> inline T length(const dummyA<T[N]>& vec1, const dummyA<T[N]>& vec2);
template<typename T, std::size_t N> inline T lengthSq(const dummyA<T[N]>& vec1, const dummyA<T[N]>& vec2);
template<typename T, std::size_t N> inline void normalize(T(&result)[N], const dummyA<T[N]>& vec);
template<typename T, std::size_t N> inline void mul(T(&result)[N], const dummyA<T[N]>& vec, const T scalar);
template<typename T, std::size_t N> inline void forward(T(&result)[N], const dummyA<T[N]>& vec, const T scalar);
template<typename T, std::size_t N> inline T dot(const dummyA<T[N]>& vec1, const dummyA<T[N]>& vec2);
template<typename T, std::size_t N> inline T normalizedDot(const dummyA<T[N]>& vec1, const dummyA<T[N]>& vec2);
template<typename T, std::size_t N> inline void cross(T (&result)[N], const dummyA<T[N]>& vec1, const dummyA<T[N]>& vec2);

//----------------------------------------
//高速ベクトル演算：特殊化

#ifdef GASHA_USE_SSE4_1
//※SSE命令に特化し、4次元／4次元ベクトル用特殊化

//SSE命令専用関数
inline __m128 m128_norm(const __m128 vec_m128);//ノルム
inline __m128 m128_normSq(const __m128 vec_m128);//ノルムの二乗
inline __m128 m128_merge(const __m128 vec1_m128, const __m128 vec2_m128);//合成（加算）
inline __m128 m128_difference(const __m128 vec1_m128, const __m128 vec2_m128);//二点間の差（減算）
inline __m128 m128_length(const __m128 vec1_m128, const __m128 vec2_m128);//二点間の長さ
inline __m128 m128_lengthSq(const __m128 vec1_m128, const __m128 vec2_m128);//二点間の長さの二乗
inline __m128 m128_normalize(const __m128 vec_m128);//正規化
inline __m128 m128_mul(const __m128 vec_m128, const float scalar);//スカラー倍
inline __m128 m128_forward(const __m128 vec_m128, const float scalar);//スカラー長の進行
inline __m128 m128_dot(const __m128 vec1_m128, const __m128 vec2_m128);//内積
inline __m128 m128_normalizedDot(const __m128 vec1_m128, const __m128 vec2_m128);//正規化して内積
inline __m128 m128_cross(const __m128 vec1_m128, const __m128 vec2_m128);//外積

#ifdef GASHA_FAST_ARITH_USE_SSE4_1

#define GASHA_FAST_VECTOR_SET_SPECIALIZATION(CLASS_NAME, T, N) \
	template<> inline T norm<T, N>(const CLASS_NAME<T[N]>& vec); \
	template<> inline T normSq<T, N>(const CLASS_NAME<T[N]>& vec); \
	template<> inline CLASS_NAME<T[N]> merge<T, N>(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2); \
	template<> inline CLASS_NAME<T[N]> difference<T, N>(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2); \
	template<> inline T length<T, N>(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2); \
	template<> inline T lengthSq<T, N>(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2); \
	template<> inline CLASS_NAME<T[N]> normalize<T, N>(const CLASS_NAME<T[N]>& vec); \
	template<> inline CLASS_NAME<T[N]> mul<T, N>(const CLASS_NAME<T[N]>& vec, const T scalar); \
	template<> inline CLASS_NAME<T[N]> forward<T, N>(const CLASS_NAME<T[N]>& vec, const T scalar); \
	template<> inline T dot<T, N>(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2); \
	template<> inline T normalizedDot<T, N>(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2); \
	template<> inline CLASS_NAME<T[N]> cross<T, N>(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2);

GASHA_FAST_VECTOR_SET_SPECIALIZATION(fastA, float, 3);
GASHA_FAST_VECTOR_SET_SPECIALIZATION(fastestA, float, 3);
GASHA_FAST_VECTOR_SET_SPECIALIZATION(semiA, float, 3);
GASHA_FAST_VECTOR_SET_SPECIALIZATION(sseA, float, 3);

GASHA_FAST_VECTOR_SET_SPECIALIZATION(fastA, float, 4);
GASHA_FAST_VECTOR_SET_SPECIALIZATION(fastestA, float, 4);
GASHA_FAST_VECTOR_SET_SPECIALIZATION(semiA, float, 4);
GASHA_FAST_VECTOR_SET_SPECIALIZATION(sseA, float, 4);

#endif//GASHA_FAST_ARITH_USE_SSE4_1

#ifdef GASHA_VECTOR_OPERATION_ALWAYS_USE_SSE4_1
//fastArithと無関係に、常にSSE命令を有効にする場合

#define GASHA_FAST_VECTOR_OPERATION_SPECIALIZATION(T, N) \
	template<> inline T norm<T, N>(const T(&vec)[N]); \
	template<> inline T normSq<T, N>(const T(&vec)[N]); \
	template<> inline void merge<T, N>(T(&result)[N], const T(&vec1)[N], const T(&vec2)[N]); \
	template<> inline void difference<T, N>(T(&result)[N], const T(&vec1)[N], const T(&vec2)[N]); \
	template<> inline T length<T, N>(const T(&vec1)[N], const T(&vec2)[N]); \
	template<> inline T lengthSq<T, N>(const T(&vec1)[N], const T(&vec2)[N]); \
	template<> inline void normalize<T, N>(T(&result)[N], const T(&vec)[N]); \
	template<> inline void mul<T, N>(T(&result)[N], const T(&vec)[N], const T scalar); \
	template<> inline void forward<T, N>(T(&result)[N], const T(&vec)[N], const T scalar); \
	template<> inline T dot<T, N>(const T(&vec1)[N], const T(&vec2)[N]); \
	template<> inline T normalizedDot<T, N>(const T(&vec1)[N], const T(&vec2)[N]); \
	template<> inline void cross<T, N>(T(&result)[N], const T(&vec1)[N], const T(&vec2)[N]);

GASHA_FAST_VECTOR_OPERATION_SPECIALIZATION(float, 3);
GASHA_FAST_VECTOR_OPERATION_SPECIALIZATION(float, 4);

#endif//GASHA_VECTOR_OPERATION_ALWAYS_USE_SSE4_1

#endif//GASHA_USE_SSE4_1

//--------------------------------------------------------------------------------
//テンプレート行列
//※fast_math.hをインクルードすることで、float×4×4の時に特殊化して、高速演算を利用可能
//※fastArithと無関係にSSE命令が有効なら適用する。
//--------------------------------------------------------------------------------

#define GASHA_FAST_MATRIX_SET(CLASS_NAME) \
	template<typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]> add(const CLASS_NAME<T[N][M]>& mat1, const CLASS_NAME<T[N][M]>& mat2); \
	template<typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]> sub(const CLASS_NAME<T[N][M]>& mat1, const CLASS_NAME<T[N][M]>& mat2); \
	template<typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]> mul(const CLASS_NAME<T[N][M]>& mat1, const T scalar); \
	template<typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]> mul(const CLASS_NAME<T[N][M]>& mat1, const CLASS_NAME<T[N][M]>& mat2);

GASHA_FAST_MATRIX_SET(fastA);
GASHA_FAST_MATRIX_SET(fastestA);
GASHA_FAST_MATRIX_SET(semiA);
GASHA_FAST_MATRIX_SET(sseA);
GASHA_FAST_MATRIX_SET(normA);
template<typename T, std::size_t N, std::size_t M> inline void add(T(&result)[N][M], const dummyA<T[N][M]>& mat1, const dummyA<T[N][M]>& mat2);
template<typename T, std::size_t N, std::size_t M> inline void sub(T(&result)[N][M], const dummyA<T[N][M]>& mat1, const dummyA<T[N][M]>& mat2);
template<typename T, std::size_t N, std::size_t M> inline void mul(T(&result)[N][M], const dummyA<T[N][M]>& mat1, const T scalar);
template<typename T, std::size_t N, std::size_t M> inline void mul(T(&result)[N][M], const dummyA<T[N][M]>& mat1, const dummyA<T[N][M]>& mat2);

#ifdef GASHA_USE_SSE
//※SSE命令に特化し、4×4行列用特殊化

//SSE命令専用関数
inline void m128_add(__m128 (&mat_result)[4], const __m128 (&mat1_m128)[4], const __m128 (&mat2_m128)[4]);//加算
inline void m128_sub(__m128 (&mat_result)[4], const __m128 (&mat1_m128)[4], const __m128 (&mat2_m128)[4]);//減算
inline void m128_mul(__m128 (&mat_result)[4], const __m128 (&mat1_m128)[4], const float scalar);//スカラー倍
#ifdef GASHA_USE_SSE4_1
inline void m128_mul(__m128 (&mat_result)[4], const __m128 (&mat1_m128)[4], const __m128 (&mat2_m128)[4]);//乗算
#endif//GASHA_USE_SSE4_1

#ifdef GASHA_FAST_ARITH_USE_SSE

#define GASHA_FAST_MATRIX_SET_SPECIALIZATION(CLASS_NAME, T, N, M) \
	template<> inline CLASS_NAME<T[N][M]> add<T, N, M>(const CLASS_NAME<T[N][M]>& mat1, const CLASS_NAME<T[N][M]>& mat2); \
	template<> inline CLASS_NAME<T[N][M]> sub<T, N, M>(const CLASS_NAME<T[N][M]>& mat1, const CLASS_NAME<T[N][M]>& mat2); \
	template<> inline CLASS_NAME<T[N][M]> mul<T, N, M>(const CLASS_NAME<T[N][M]>& mat, const T scalar);

GASHA_FAST_MATRIX_SET_SPECIALIZATION(fastA, float, 4, 4);
GASHA_FAST_MATRIX_SET_SPECIALIZATION(fastestA, float, 4, 4);
GASHA_FAST_MATRIX_SET_SPECIALIZATION(semiA, float, 4, 4);

#endif//GASHA_FAST_ARITH_USE_SSE

#ifdef GASHA_FAST_ARITH_USE_SSE4_1

#define GASHA_FAST_MATRIX_SET_SPECIALIZATION4_1(CLASS_NAME, T, N, M) \
	template<> inline CLASS_NAME<T[N][M]> mul<T, N, M>(const CLASS_NAME<T[N][M]>& mat1, const CLASS_NAME<T[N][M]>& mat2);

GASHA_FAST_MATRIX_SET_SPECIALIZATION4_1(sseA, float, 4, 4);

#endif//GASHA_FAST_ARITH_USE_SSE4_1

#ifdef GASHA_MATRIX_OPERATION_ALWAYS_USE_SSE4_1
//----------
//行列の加算
template<>
void add<float, 4, 4>(float(&mat_result)[4][4], const float(&mat1)[4][4], const float(&mat2)[4][4]);

//----------
//行列の減算
template<>
void sub<float, 4, 4>(float(&mat_result)[4][4], const float(&mat1)[4][4], const float(&mat2)[4][4]);

//----------
//行列のスカラー倍
template<>
void mul<float, 4, 4>(float(&mat_result)[4][4], const float(&mat)[4][4], const float scalar);

#ifdef GASHA_USE_SSE4_1
//----------
//行列の乗算
template<>
void mul<float, 4, 4, 4>(float(&mat_result)[4][4], const float(&mat1)[4][4], const float(&mat2)[4][4]);
#endif//GASHA_USE_SSE4_1

#endif//GASHA_MATRIX_OPERATION_ALWAYS_USE_SSE4_1

#endif//GASHA_USE_SSE

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/fast_math.inl>

#endif//GASHA_INCLUDED_FAST_MATH_H

// End of file

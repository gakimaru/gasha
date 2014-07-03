#pragma once
#ifndef GASHA_INCLUDED_FAST_MATH_INL
#define GASHA_INCLUDED_FAST_MATH_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// fast_math.h
// 高速算術用【インライン関数部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/fast_math.h>//高速算術【宣言部】

#include <utility>//C++11 std::move
#include <cmath>//std::sqrt()

#ifdef GASHA_USE_SSE
#include <xmmintrin.h>//SSE1
#endif//GASHA_USE_SSE

#ifdef GASHA_USE_AVX
#include <immintrin.h>//AVX
#endif//GASHA_USE_AVX

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//高速除算
//--------------------------------------------------------------------------------

//補助マクロ
#define GASHA_FAST_ARITH_CLASS_INSTANCING(CLASS_NAME) \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const T val){ m_val = val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const fastArith<T>&& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const fastArith<T>& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const fastestArith<T>&& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const fastestArith<T>& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const semifastArith<T>&& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const semifastArith<T>& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const sseArith<T>&& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const sseArith<T>& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const normalArith<T>&& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const normalArith<T>& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const dummyArith<T>&& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const dummyArith<T>& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const T val) : m_val(val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const fastArith<T>&& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const fastArith<T>& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const fastestArith<T>&& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const fastestArith<T>& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const semifastArith<T>&& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const semifastArith<T>& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const sseArith<T>&& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const sseArith<T>& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const normalArith<T>&& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const normalArith<T>& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const dummyArith<T>&& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const dummyArith<T>& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME() : m_val(static_cast<T>(0)){} \
	template <typename T> inline CLASS_NAME<T>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_FLOAT_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const float val){ m_val = _mm_set_ss(val); return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const __m128&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const __m128& val){ m_val = val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const fastArith<float>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const fastArith<float>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const fastestArith<float>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const fastestArith<float>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const semifastArith<float>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const semifastArith<float>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const sseArith<float>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const sseArith<float>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const normalArith<float>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const normalArith<float>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const dummyArith<float>&& val){ m_val = _mm_set_ss(val.m_val); return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const dummyArith<float>& val){ m_val = _mm_set_ss(val.m_val); return *this; } \
	inline CLASS_NAME<float>::CLASS_NAME(const float val) : m_val(_mm_set_ss(val)){} \
	inline CLASS_NAME<float>::CLASS_NAME(const __m128&& val) : m_val(val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const __m128& val) : m_val(val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const fastArith<float>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const fastArith<float>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const fastestArith<float>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const fastestArith<float>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const semifastArith<float>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const semifastArith<float>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const sseArith<float>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const sseArith<float>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const normalArith<float>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const normalArith<float>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const dummyArith<float>&& val) : m_val(_mm_set_ss(val.m_val)){} \
	inline CLASS_NAME<float>::CLASS_NAME(const dummyArith<float>& val) : m_val(_mm_set_ss(val.m_val)){} \
	inline CLASS_NAME<float>::CLASS_NAME() : m_val(_mm_setzero_ps()){} \
	inline CLASS_NAME<float>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_DOUBLE_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const double val){ m_val = _mm_set_sd(val); return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const __m128d&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const __m128d& val){ m_val = val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const fastArith<double>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const fastArith<double>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const fastestArith<double>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const fastestArith<double>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const semifastArith<double>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const semifastArith<double>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const sseArith<double>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const sseArith<double>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const normalArith<double>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const normalArith<double>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const dummyArith<double>&& val){ m_val = _mm_set_sd(val.m_val); return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const dummyArith<double>& val){ m_val = _mm_set_sd(val.m_val); return *this; } \
	inline CLASS_NAME<double>::CLASS_NAME(const double val) : m_val(_mm_set_sd(val)){} \
	inline CLASS_NAME<double>::CLASS_NAME(const __m128d&& val) : m_val(val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const __m128d& val) : m_val(val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const fastArith<double>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const fastArith<double>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const fastestArith<double>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const fastestArith<double>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const semifastArith<double>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const semifastArith<double>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const sseArith<double>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const sseArith<double>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const normalArith<double>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const normalArith<double>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const dummyArith<double>&& val) : m_val(_mm_set_sd(val.m_val)){} \
	inline CLASS_NAME<double>::CLASS_NAME(const dummyArith<double>& val) : m_val(_mm_set_sd(val.m_val)){} \
	inline CLASS_NAME<double>::CLASS_NAME() : m_val(_mm_setzero_pd()){} \
	inline CLASS_NAME<double>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_M128_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const __m128&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const __m128& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const fastArith<__m128>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const fastArith<__m128>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const fastestArith<__m128>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const fastestArith<__m128>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const semifastArith<__m128>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const semifastArith<__m128>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const sseArith<__m128>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const sseArith<__m128>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const normalArith<__m128>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const normalArith<__m128>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const dummyArith<__m128>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const dummyArith<__m128>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>::CLASS_NAME(const __m128&& val) : m_val(val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const __m128& val) : m_val(val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const fastArith<__m128>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const fastArith<__m128>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const fastestArith<__m128>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const fastestArith<__m128>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const semifastArith<__m128>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const semifastArith<__m128>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const sseArith<__m128>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const sseArith<__m128>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const normalArith<__m128>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const normalArith<__m128>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const dummyArith<__m128>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const dummyArith<__m128>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME() : m_val(_mm_setzero_ps()){} \
	inline CLASS_NAME<__m128>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_M128D_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const __m128d&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const __m128d& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const fastArith<__m128d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const fastArith<__m128d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const fastestArith<__m128d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const fastestArith<__m128d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const semifastArith<__m128d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const semifastArith<__m128d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const sseArith<__m128d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const sseArith<__m128d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const normalArith<__m128d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const normalArith<__m128d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const dummyArith<__m128d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const dummyArith<__m128d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const __m128d&& val) : m_val(val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const __m128d& val) : m_val(val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const fastArith<__m128d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const fastArith<__m128d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const fastestArith<__m128d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const fastestArith<__m128d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const semifastArith<__m128d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const semifastArith<__m128d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const sseArith<__m128d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const sseArith<__m128d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const normalArith<__m128d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const normalArith<__m128d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const dummyArith<__m128d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const dummyArith<__m128d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME() : m_val(_mm_setzero_pd()){} \
	inline CLASS_NAME<__m128d>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_M256_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const __m256&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const __m256& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const fastArith<__m256>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const fastArith<__m256>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const fastestArith<__m256>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const fastestArith<__m256>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const semifastArith<__m256>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const semifastArith<__m256>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const sseArith<__m256>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const sseArith<__m256>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const normalArith<__m256>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const normalArith<__m256>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const dummyArith<__m256>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const dummyArith<__m256>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>::CLASS_NAME(const __m256&& val) : m_val(val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const __m256& val) : m_val(val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const fastArith<__m256>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const fastArith<__m256>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const fastestArith<__m256>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const fastestArith<__m256>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const semifastArith<__m256>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const semifastArith<__m256>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const sseArith<__m256>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const sseArith<__m256>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const normalArith<__m256>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const normalArith<__m256>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const dummyArith<__m256>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const dummyArith<__m256>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME() : m_val(_mm256_setzero_ps()){} \
	inline CLASS_NAME<__m256>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_M256D_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const __m256d&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const __m256d& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const fastArith<__m256d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const fastArith<__m256d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const fastestArith<__m256d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const fastestArith<__m256d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const semifastArith<__m256d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const semifastArith<__m256d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const sseArith<__m256d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const sseArith<__m256d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const normalArith<__m256d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const normalArith<__m256d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const dummyArith<__m256d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const dummyArith<__m256d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const __m256d&& val) : m_val(val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const __m256d& val) : m_val(val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const fastArith<__m256d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const fastArith<__m256d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const fastestArith<__m256d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const fastestArith<__m256d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const semifastArith<__m256d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const semifastArith<__m256d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const sseArith<__m256d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const sseArith<__m256d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const normalArith<__m256d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const normalArith<__m256d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const dummyArith<__m256d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const dummyArith<__m256d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME() : m_val(_mm256_setzero_pd()){} \
	inline CLASS_NAME<__m256d>::~CLASS_NAME(){} \

#ifdef GASHA_FAST_ARITH_USE_SSE
#define GASHA_FAST_ARITH_CLASS_SSE_INSTANCING(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_SSE_FLOAT_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_M128_INSTANCING(CLASS_NAME);
#else//GASHA_FAST_ARITH_USE_SSE
#define GASHA_FAST_ARITH_CLASS_SSE_INSTANCING(CLASS_NAME)
#endif//GASHA_FAST_ARITH_USE_SSE

#ifdef GASHA_FAST_ARITH_USE_SSE2
#define GASHA_FAST_ARITH_CLASS_SSE2_INSTANCING(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_SSE_DOUBLE_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_M128D_INSTANCING(CLASS_NAME);
#else//GASHA_FAST_ARITH_USE_SSE2
#define GASHA_FAST_ARITH_CLASS_SSE2_INSTANCING(CLASS_NAME)
#endif//GASHA_FAST_ARITH_USE_SSE2

#ifdef GASHA_FAST_ARITH_USE_AVX
#define GASHA_FAST_ARITH_CLASS_AVX_INSTANCING(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_SSE_M256_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_M256D_INSTANCING(CLASS_NAME);
#else//GASHA_FAST_ARITH_USE_SSE
#define GASHA_FAST_ARITH_CLASS_AVX_INSTANCING(CLASS_NAME)
#endif//GASHA_FAST_ARITH_USE_SSE

#define GASHA_FAST_ARITH_CLASS_SET_INSTANCING(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE2_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_AVX_INSTANCING(CLASS_NAME);

//----------------------------------------
//高速演算クラス
GASHA_FAST_ARITH_CLASS_SET_INSTANCING(fastArith);
//----------------------------------------
//最高速演算クラス
GASHA_FAST_ARITH_CLASS_SET_INSTANCING(fastestArith);
//----------------------------------------
//準高速演算クラス
GASHA_FAST_ARITH_CLASS_SET_INSTANCING(semifastArith);
//----------------------------------------
//SSE高速演算クラス
GASHA_FAST_ARITH_CLASS_SET_INSTANCING(sseArith);
//----------------------------------------
//通常演算クラス
GASHA_FAST_ARITH_CLASS_SET_INSTANCING(normalArith);

//----------------------------------------
//高速演算：特殊化
#ifdef GASHA_FAST_ARITH_USE_SSE
template<> inline fastArith<float> operator+(const fastArith<float>&& lvalue, const fastArith<float>&& rvalue)
{
	return _mm_add_ss(lvalue, rvalue);
}
template<> inline fastArith<__m128> operator+(const fastArith<__m128>&& lvalue, const fastArith<__m128>&& rvalue)
{
	return _mm_add_ps(lvalue, rvalue);
}

template<> inline fastArith<float> operator-(const fastArith<float>&& lvalue, const fastArith<float>&& rvalue)
{
	return _mm_sub_ss(lvalue, rvalue);
}
template<> inline fastArith<__m128> operator-(const fastArith<__m128>&& lvalue, const fastArith<__m128>&& rvalue)
{
	return _mm_sub_ps(lvalue, rvalue);
}

template<> inline fastArith<float> operator*(const fastArith<float>&& lvalue, const fastArith<float>&& rvalue)
{
	return _mm_mul_ss(lvalue, rvalue);
}
template<> inline fastArith<__m128> operator*(const fastArith<__m128>&& lvalue, const fastArith<__m128>&& rvalue)
{
	return _mm_mul_ps(lvalue, rvalue);
}

template<> inline fastArith<float> operator/(const fastArith<float>&& lvalue, const fastArith<float>&& rvalue)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で除算
	//【ステップ１】逆数の近似値を取得
	__m128 rcp_div_m128 = _mm_rcp_ps(rvalue);//逆数の近似値を高速に算出するSSE命令
	//【ステップ２】ニュートン法で逆数の精度を高める（以下、Wikipedia他から引用した式）
	//  ニュートン法による漸化式：rcp = rcp * (2.f - div * rcp)
	//    rcp   = (1 / div) の近似値
	//    f(x)  = div * x - 1   ... x = 1 / div の時に 0 になる関数①
	//    f(x)  = 1 / x - div   ... x = 1 / div の時に 0 になる関数②：これを反復式に用いる
	//    f'(x) = -(1 / x ^ 2)  ... 関数②の1次導関数
	//    ニュートン-ラプソンの反復式：
	//      x[i+1] = x[i] - f(x[i]) / f'(x[i])
	//             = x[i] - (1 / x[i] - div) / -(1 / x[i] ^ 2)
	//             = x[i] + x[i] * (1 - div * x[i])
	//             = x[i] * (2 - div * x[i])
	rcp_div_m128 = _mm_mul_ss(rcp_div_m128, _mm_sub_ss(_mm_set_ss(2.f), _mm_mul_ss(rvalue, rcp_div_m128)));
	//【ステップ３】逆数を乗算
	return _mm_mul_ss(lvalue, rcp_div_m128);
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
	return _mm_div_ss(lvalue, rvalue);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
}
template<> inline fastArith<__m128> operator/(const fastArith<__m128>&& lvalue, const fastArith<__m128>&& rvalue)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で除算
	__m128 rcp_div_m128 = _mm_rcp_ps(rvalue);//逆数の近似値算出
	rcp_div_m128 = _mm_mul_ps(rcp_div_m128, _mm_sub_ps(_mm_set1_ps(2.f), _mm_mul_ps(rvalue, rcp_div_m128)));//逆数の精度向上
	return _mm_mul_ps(lvalue, rcp_div_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
	return _mm_div_ps(lvalue, rvalue);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
}

template<> inline fastestArith<float> operator+(const fastestArith<float>&& lvalue, const fastestArith<float>&& rvalue)
{
	return _mm_add_ss(lvalue, rvalue);
}
template<> inline fastestArith<__m128> operator+(const fastestArith<__m128>&& lvalue, const fastestArith<__m128>&& rvalue)
{
	return _mm_add_ps(lvalue, rvalue);
}

template<> inline fastestArith<float> operator-(const fastestArith<float>&& lvalue, const fastestArith<float>&& rvalue)
{
	return _mm_sub_ss(lvalue, rvalue);
}
template<> inline fastestArith<__m128> operator-(const fastestArith<__m128>&& lvalue, const fastestArith<__m128>&& rvalue)
{
	return _mm_sub_ps(lvalue, rvalue);
}

template<> inline fastestArith<float> operator*(const fastestArith<float>&& lvalue, const fastestArith<float>&& rvalue)
{
	return _mm_mul_ss(lvalue, rvalue);
}
template<> inline fastestArith<__m128> operator*(const fastestArith<__m128>&& lvalue, const fastestArith<__m128>&& rvalue)
{
	return _mm_mul_ps(lvalue, rvalue);
}

template<> inline fastestArith<float> operator/(const fastestArith<float>&& lvalue, const fastestArith<float>&& rvalue)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で除算
	__m128 rcp_div_m128 = _mm_rcp_ss(rvalue);//逆数の近似値算出
	return _mm_mul_ss(lvalue, rcp_div_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
	return _mm_div_ss(lvalue, rvalue);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
}
template<> inline fastestArith<__m128> operator/(const fastestArith<__m128>&& lvalue, const fastestArith<__m128>&& rvalue)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で除算
	__m128 rcp_div_m128 = _mm_rcp_ps(rvalue);//逆数の近似値算出
	return _mm_mul_ps(lvalue, rcp_div_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
	return _mm_div_ps(lvalue, rvalue);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
}

template<> inline semifastArith<float> operator+(const semifastArith<float>&& lvalue, const semifastArith<float>&& rvalue)
{
	return _mm_add_ss(lvalue, rvalue);
}
template<> inline semifastArith<__m128> operator+(const semifastArith<__m128>&& lvalue, const semifastArith<__m128>&& rvalue)
{
	return _mm_add_ps(lvalue, rvalue);
}

template<> inline semifastArith<float> operator-(const semifastArith<float>&& lvalue, const semifastArith<float>&& rvalue)
{
	return _mm_sub_ss(lvalue, rvalue);
}
template<> inline semifastArith<__m128> operator-(const semifastArith<__m128>&& lvalue, const semifastArith<__m128>&& rvalue)
{
	return _mm_sub_ps(lvalue, rvalue);
}

template<> inline semifastArith<float> operator*(const semifastArith<float>&& lvalue, const semifastArith<float>&& rvalue)
{
	return _mm_mul_ss(lvalue, rvalue);
}
template<> inline semifastArith<__m128> operator*(const semifastArith<__m128>&& lvalue, const semifastArith<__m128>&& rvalue)
{
	return _mm_mul_ps(lvalue, rvalue);
}

template<> inline semifastArith<float> operator/(const semifastArith<float>&& lvalue, const semifastArith<float>&& rvalue)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で除算
	__m128 rcp_div_m128 = _mm_rcp_ss(rvalue);//逆数の近似値算出
	rcp_div_m128 = _mm_mul_ss(rcp_div_m128, _mm_sub_ss(_mm_set_ss(2.f), _mm_mul_ps(rvalue, rcp_div_m128)));//逆数の精度向上
	rcp_div_m128 = _mm_mul_ss(rcp_div_m128, _mm_sub_ss(_mm_set_ss(2.f), _mm_mul_ps(rvalue, rcp_div_m128)));//2回目
	return _mm_mul_ss(lvalue, rcp_div_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
	return _mm_div_ss(lvalue, rvalue);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
}
template<> inline semifastArith<__m128> operator/(const semifastArith<__m128>&& lvalue, const semifastArith<__m128>&& rvalue)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で除算
	__m128 rcp_div_m128 = _mm_rcp_ps(rvalue);//逆数の近似値算出
	rcp_div_m128 = _mm_mul_ps(rcp_div_m128, _mm_sub_ps(_mm_set1_ps(2.f), _mm_mul_ps(rvalue, rcp_div_m128)));//逆数の精度向上
	rcp_div_m128 = _mm_mul_ps(rcp_div_m128, _mm_sub_ps(_mm_set1_ps(2.f), _mm_mul_ps(rvalue, rcp_div_m128)));//2回目
	return _mm_mul_ps(lvalue, rcp_div_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
	return _mm_div_ps(lvalue, rvalue);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
}

template<> inline sseArith<float> operator+(const sseArith<float>&& lvalue, const sseArith<float>&& rvalue)
{
	return _mm_add_ss(lvalue, rvalue);
}
template<> inline sseArith<__m128> operator+(const sseArith<__m128>&& lvalue, const sseArith<__m128>&& rvalue)
{
	return _mm_add_ps(lvalue, rvalue);
}

template<> inline sseArith<float> operator-(const sseArith<float>&& lvalue, const sseArith<float>&& rvalue)
{
	return _mm_sub_ss(lvalue, rvalue);
}
template<> inline sseArith<__m128> operator-(const sseArith<__m128>&& lvalue, const sseArith<__m128>&& rvalue)
{
	return _mm_sub_ps(lvalue, rvalue);
}

template<> inline sseArith<float> operator*(const sseArith<float>&& lvalue, const sseArith<float>&& rvalue)
{
	return _mm_mul_ss(lvalue, rvalue);
}
template<> inline sseArith<__m128> operator*(const sseArith<__m128>&& lvalue, const sseArith<__m128>&& rvalue)
{
	return _mm_mul_ps(lvalue, rvalue);
}

template<> inline sseArith<float> operator/(const sseArith<float>&& lvalue, const sseArith<float>&& rvalue)
{
	return _mm_div_ss(lvalue, rvalue);
}
template<> inline sseArith<__m128> operator/(const sseArith<__m128>&& lvalue, const sseArith<__m128>&& rvalue)
{
	return _mm_div_ps(lvalue, rvalue);
}

template<> inline normalArith<__m128> operator+(const normalArith<__m128>&& lvalue, const normalArith<__m128>&& rvalue)
{
	return _mm_add_ps(lvalue, rvalue);
}

template<> inline normalArith<__m128> operator-(const normalArith<__m128>&& lvalue, const normalArith<__m128>&& rvalue)
{
	return _mm_sub_ps(lvalue, rvalue);
}

template<> inline normalArith<__m128> operator*(const normalArith<__m128>&& lvalue, const normalArith<__m128>&& rvalue)
{
	return _mm_mul_ps(lvalue, rvalue);
}

template<> inline normalArith<__m128> operator/(const normalArith<__m128>&& lvalue, const normalArith<__m128>&& rvalue)
{
	return _mm_div_ps(lvalue, rvalue);
}

template<> inline __m128 operator+(const dummyArith<__m128>&& lvalue, const dummyArith<__m128>&& rvalue)
{
	return _mm_add_ps(lvalue, rvalue);
}

template<> inline __m128 operator-(const dummyArith<__m128>&& lvalue, const dummyArith<__m128>&& rvalue)
{
	return _mm_sub_ps(lvalue, rvalue);
}

template<> inline __m128 operator*(const dummyArith<__m128>&& lvalue, const dummyArith<__m128>&& rvalue)
{
	return _mm_mul_ps(lvalue, rvalue);
}

template<> inline __m128 operator/(const dummyArith<__m128>&& lvalue, const dummyArith<__m128>&& rvalue)
{
	return _mm_div_ps(lvalue, rvalue);
}
#endif//GASHA_FAST_ARITH_USE_SSE
#ifdef GASHA_FAST_ARITH_USE_SSE2
template<> inline fastArith<double> operator+(const fastArith<double>&& lvalue, const fastArith<double>&& rvalue)
{
	return _mm_add_sd(lvalue, rvalue);
}
template<> inline fastArith<__m128d> operator+(const fastArith<__m128d>&& lvalue, const fastArith<__m128d>&& rvalue)
{
	return _mm_add_pd(lvalue, rvalue);
}

template<> inline fastArith<double> operator-(const fastArith<double>&& lvalue, const fastArith<double>&& rvalue)
{
	return _mm_sub_sd(lvalue, rvalue);
}
template<> inline fastArith<__m128d> operator-(const fastArith<__m128d>&& lvalue, const fastArith<__m128d>&& rvalue)
{
	return _mm_sub_pd(lvalue, rvalue);
}

template<> inline fastArith<double> operator*(const fastArith<double>&& lvalue, const fastArith<double>&& rvalue)
{
	return _mm_mul_sd(lvalue, rvalue);
}
template<> inline fastArith<__m128d> operator*(const fastArith<__m128d>&& lvalue, const fastArith<__m128d>&& rvalue)
{
	return _mm_mul_pd(lvalue, rvalue);
}

template<> inline fastArith<double> operator/(const fastArith<double>&& lvalue, const fastArith<double>&& rvalue)
{
	return _mm_div_sd(lvalue, rvalue);
}
template<> inline fastArith<__m128d> operator/(const fastArith<__m128d>&& lvalue, const fastArith<__m128d>&& rvalue)
{
	return _mm_div_pd(lvalue, rvalue);
}

template<> inline fastestArith<double> operator+(const fastestArith<double>&& lvalue, const fastestArith<double>&& rvalue)
{
	return _mm_add_sd(lvalue, rvalue);
}
template<> inline fastestArith<__m128d> operator+(const fastestArith<__m128d>&& lvalue, const fastestArith<__m128d>&& rvalue)
{
	return _mm_add_pd(lvalue, rvalue);
}

template<> inline fastestArith<double> operator-(const fastestArith<double>&& lvalue, const fastestArith<double>&& rvalue)
{
	return _mm_sub_sd(lvalue, rvalue);
}
template<> inline fastestArith<__m128d> operator-(const fastestArith<__m128d>&& lvalue, const fastestArith<__m128d>&& rvalue)
{
	return _mm_sub_sd(lvalue, rvalue);
}

template<> inline fastestArith<double> operator*(const fastestArith<double>&& lvalue, const fastestArith<double>&& rvalue)
{
	return _mm_mul_sd(lvalue, rvalue);
}
template<> inline fastestArith<__m128d> operator*(const fastestArith<__m128d>&& lvalue, const fastestArith<__m128d>&& rvalue)
{
	return _mm_mul_pd(lvalue, rvalue);
}

template<> inline fastestArith<double> operator/(const fastestArith<double>&& lvalue, const fastestArith<double>&& rvalue)
{
	return _mm_div_sd(lvalue, rvalue);
}
template<> inline fastestArith<__m128d> operator/(const fastestArith<__m128d>&& lvalue, const fastestArith<__m128d>&& rvalue)
{
	return _mm_div_pd(lvalue, rvalue);
}

template<> inline semifastArith<double> operator+(const semifastArith<double>&& lvalue, const semifastArith<double>&& rvalue)
{
	return _mm_add_sd(lvalue, rvalue);
}
template<> inline semifastArith<__m128d> operator+(const semifastArith<__m128d>&& lvalue, const semifastArith<__m128d>&& rvalue)
{
	return _mm_add_pd(lvalue, rvalue);
}

template<> inline semifastArith<double> operator-(const semifastArith<double>&& lvalue, const semifastArith<double>&& rvalue)
{
	return _mm_sub_sd(lvalue, rvalue);
}
template<> inline semifastArith<__m128d> operator-(const semifastArith<__m128d>&& lvalue, const semifastArith<__m128d>&& rvalue)
{
	return _mm_sub_pd(lvalue, rvalue);
}

template<> inline semifastArith<double> operator*(const semifastArith<double>&& lvalue, const semifastArith<double>&& rvalue)
{
	return _mm_mul_sd(lvalue, rvalue);
}
template<> inline semifastArith<__m128d> operator*(const semifastArith<__m128d>&& lvalue, const semifastArith<__m128d>&& rvalue)
{
	return _mm_mul_pd(lvalue, rvalue);
}

template<> inline semifastArith<double> operator/(const semifastArith<double>&& lvalue, const semifastArith<double>&& rvalue)
{
	return _mm_div_sd(lvalue, rvalue);
}
template<> inline semifastArith<__m128d> operator/(const semifastArith<__m128d>&& lvalue, const semifastArith<__m128d>&& rvalue)
{
	return _mm_div_pd(lvalue, rvalue);
}

template<> inline sseArith<double> operator+(const sseArith<double>&& lvalue, const sseArith<double>&& rvalue)
{
	return _mm_add_sd(lvalue, rvalue);
}
template<> inline sseArith<__m128d> operator+(const sseArith<__m128d>&& lvalue, const sseArith<__m128d>&& rvalue)
{
	return _mm_add_pd(lvalue, rvalue);
}

template<> inline sseArith<double> operator-(const sseArith<double>&& lvalue, const sseArith<double>&& rvalue)
{
	return _mm_sub_sd(lvalue, rvalue);
}
template<> inline sseArith<__m128d> operator-(const sseArith<__m128d>&& lvalue, const sseArith<__m128d>&& rvalue)
{
	return _mm_sub_pd(lvalue, rvalue);
}

template<> inline sseArith<double> operator*(const sseArith<double>&& lvalue, const sseArith<double>&& rvalue)
{
	return _mm_mul_sd(lvalue, rvalue);
}
template<> inline sseArith<__m128d> operator*(const sseArith<__m128d>&& lvalue, const sseArith<__m128d>&& rvalue)
{
	return _mm_mul_pd(lvalue, rvalue);
}

template<> inline sseArith<double> operator/(const sseArith<double>&& lvalue, const sseArith<double>&& rvalue)
{
	return _mm_div_sd(lvalue, rvalue);
}
template<> inline sseArith<__m128d> operator/(const sseArith<__m128d>&& lvalue, const sseArith<__m128d>&& rvalue)
{
	return _mm_div_pd(lvalue, rvalue);
}

template<> inline normalArith<__m128d> operator+(const normalArith<__m128d>&& lvalue, const normalArith<__m128d>&& rvalue)
{
	return _mm_add_pd(lvalue, rvalue);
}

template<> inline normalArith<__m128d> operator-(const normalArith<__m128d>&& lvalue, const normalArith<__m128d>&& rvalue)
{
	return _mm_sub_pd(lvalue, rvalue);
}

template<> inline normalArith<__m128d> operator*(const normalArith<__m128d>&& lvalue, const normalArith<__m128d>&& rvalue)
{
	return _mm_mul_pd(lvalue, rvalue);
}

template<> inline normalArith<__m128d> operator/(const normalArith<__m128d>&& lvalue, const normalArith<__m128d>&& rvalue)
{
	return _mm_div_pd(lvalue, rvalue);
}

template<> inline __m128d operator+(const dummyArith<__m128d>&& lvalue, const dummyArith<__m128d>&& rvalue)
{
	return _mm_add_pd(lvalue, rvalue);
}

template<> inline __m128d operator-(const dummyArith<__m128d>&& lvalue, const dummyArith<__m128d>&& rvalue)
{
	return _mm_sub_pd(lvalue, rvalue);
}

template<> inline __m128d operator*(const dummyArith<__m128d>&& lvalue, const dummyArith<__m128d>&& rvalue)
{
	return _mm_mul_pd(lvalue, rvalue);
}

template<> inline __m128d operator/(const dummyArith<__m128d>&& lvalue, const dummyArith<__m128d>&& rvalue)
{
	return _mm_div_pd(lvalue, rvalue);
}
#endif//GASHA_FAST_ARITH_USE_SSE2
#ifdef GASHA_FAST_ARITH_USE_AVX
template<> inline fastArith<__m256> operator+(const fastArith<__m256>&& lvalue, const fastArith<__m256>&& rvalue)
{
	return _mm256_add_ps(lvalue, rvalue);
}
template<> inline fastArith<__m256d> operator+(const fastArith<__m256d>&& lvalue, const fastArith<__m256d>&& rvalue)
{
	return _mm256_add_pd(lvalue, rvalue);
}

template<> inline fastArith<__m256> operator-(const fastArith<__m256>&& lvalue, const fastArith<__m256>&& rvalue)
{
	return _mm256_sub_ps(lvalue, rvalue);
}
template<> inline fastArith<__m256d> operator-(const fastArith<__m256d>&& lvalue, const fastArith<__m256d>&& rvalue)
{
	return _mm256_sub_pd(lvalue, rvalue);
}

template<> inline fastArith<__m256> operator*(const fastArith<__m256>&& lvalue, const fastArith<__m256>&& rvalue)
{
	return _mm256_mul_ps(lvalue, rvalue);
}
template<> inline fastArith<__m256d> operator*(const fastArith<__m256d>&& lvalue, const fastArith<__m256d>&& rvalue)
{
	return _mm256_mul_pd(lvalue, rvalue);
}

template<> inline fastArith<__m256> operator/(const fastArith<__m256>&& lvalue, const fastArith<__m256>&& rvalue)
{
	return _mm256_div_ps(lvalue, rvalue);
}
template<> inline fastArith<__m256d> operator/(const fastArith<__m256d>&& lvalue, const fastArith<__m256d>&& rvalue)
{
	return _mm256_div_pd(lvalue, rvalue);
}

template<> inline fastestArith<__m256> operator+(const fastestArith<__m256>&& lvalue, const fastestArith<__m256>&& rvalue)
{
	return _mm256_add_ps(lvalue, rvalue);
}
template<> inline fastestArith<__m256d> operator+(const fastestArith<__m256d>&& lvalue, const fastestArith<__m256d>&& rvalue)
{
	return _mm256_add_pd(lvalue, rvalue);
}

template<> inline fastestArith<__m256> operator-(const fastestArith<__m256>&& lvalue, const fastestArith<__m256>&& rvalue)
{
	return _mm256_sub_ps(lvalue, rvalue);
}
template<> inline fastestArith<__m256d> operator-(const fastestArith<__m256d>&& lvalue, const fastestArith<__m256d>&& rvalue)
{
	return _mm256_sub_pd(lvalue, rvalue);
}

template<> inline fastestArith<__m256> operator*(const fastestArith<__m256>&& lvalue, const fastestArith<__m256>&& rvalue)
{
	return _mm256_mul_ps(lvalue, rvalue);
}
template<> inline fastestArith<__m256d> operator*(const fastestArith<__m256d>&& lvalue, const fastestArith<__m256d>&& rvalue)
{
	return _mm256_mul_pd(lvalue, rvalue);
}

template<> inline fastestArith<__m256> operator/(const fastestArith<__m256>&& lvalue, const fastestArith<__m256>&& rvalue)
{
	return _mm256_div_ps(lvalue, rvalue);
}
template<> inline fastestArith<__m256d> operator/(const fastestArith<__m256d>&& lvalue, const fastestArith<__m256d>&& rvalue)
{
	return _mm256_div_pd(lvalue, rvalue);
}

template<> inline semifastArith<__m256> operator+(const semifastArith<__m256>&& lvalue, const semifastArith<__m256>&& rvalue)
{
	return _mm256_add_ps(lvalue, rvalue);
}
template<> inline semifastArith<__m256d> operator+(const semifastArith<__m256d>&& lvalue, const semifastArith<__m256d>&& rvalue)
{
	return _mm256_add_pd(lvalue, rvalue);
}

template<> inline semifastArith<__m256> operator-(const semifastArith<__m256>&& lvalue, const semifastArith<__m256>&& rvalue)
{
	return _mm256_sub_ps(lvalue, rvalue);
}
template<> inline semifastArith<__m256d> operator-(const semifastArith<__m256d>&& lvalue, const semifastArith<__m256d>&& rvalue)
{
	return _mm256_sub_pd(lvalue, rvalue);
}

template<> inline semifastArith<__m256> operator*(const semifastArith<__m256>&& lvalue, const semifastArith<__m256>&& rvalue)
{
	return _mm256_mul_ps(lvalue, rvalue);
}
template<> inline semifastArith<__m256d> operator*(const semifastArith<__m256d>&& lvalue, const semifastArith<__m256d>&& rvalue)
{
	return _mm256_mul_pd(lvalue, rvalue);
}

template<> inline semifastArith<__m256> operator/(const semifastArith<__m256>&& lvalue, const semifastArith<__m256>&& rvalue)
{
	return _mm256_div_ps(lvalue, rvalue);
}
template<> inline semifastArith<__m256d> operator/(const semifastArith<__m256d>&& lvalue, const semifastArith<__m256d>&& rvalue)
{
	return _mm256_div_pd(lvalue, rvalue);
}

template<> inline sseArith<__m256> operator+(const sseArith<__m256>&& lvalue, const sseArith<__m256>&& rvalue)
{
	return _mm256_add_ps(lvalue, rvalue);
}
template<> inline sseArith<__m256d> operator+(const sseArith<__m256d>&& lvalue, const sseArith<__m256d>&& rvalue)
{
	return _mm256_add_pd(lvalue, rvalue);
}

template<> inline sseArith<__m256> operator-(const sseArith<__m256>&& lvalue, const sseArith<__m256>&& rvalue)
{
	return _mm256_sub_ps(lvalue, rvalue);
}
template<> inline sseArith<__m256d> operator-(const sseArith<__m256d>&& lvalue, const sseArith<__m256d>&& rvalue)
{
	return _mm256_sub_pd(lvalue, rvalue);
}

template<> inline sseArith<__m256> operator*(const sseArith<__m256>&& lvalue, const sseArith<__m256>&& rvalue)
{
	return _mm256_mul_ps(lvalue, rvalue);
}
template<> inline sseArith<__m256d> operator*(const sseArith<__m256d>&& lvalue, const sseArith<__m256d>&& rvalue)
{
	return _mm256_mul_pd(lvalue, rvalue);
}

template<> inline sseArith<__m256> operator/(const sseArith<__m256>&& lvalue, const sseArith<__m256>&& rvalue)
{
	return _mm256_div_ps(lvalue, rvalue);
}
template<> inline sseArith<__m256d> operator/(const sseArith<__m256d>&& lvalue, const sseArith<__m256d>&& rvalue)
{
	return _mm256_div_pd(lvalue, rvalue);
}

template<> inline normalArith<__m256> operator+(const normalArith<__m256>&& lvalue, const normalArith<__m256>&& rvalue)
{
	return _mm256_add_ps(lvalue, rvalue);
}
template<> inline normalArith<__m256d> operator+(const normalArith<__m256d>&& lvalue, const normalArith<__m256d>&& rvalue)
{
	return _mm256_add_pd(lvalue, rvalue);
}

template<> inline normalArith<__m256> operator-(const normalArith<__m256>&& lvalue, const normalArith<__m256>&& rvalue)
{
	return _mm256_sub_ps(lvalue, rvalue);
}
template<> inline normalArith<__m256d> operator-(const normalArith<__m256d>&& lvalue, const normalArith<__m256d>&& rvalue)
{
	return _mm256_sub_pd(lvalue, rvalue);
}

template<> inline normalArith<__m256> operator*(const normalArith<__m256>&& lvalue, const normalArith<__m256>&& rvalue)
{
	return _mm256_mul_ps(lvalue, rvalue);
}
template<> inline normalArith<__m256d> operator*(const normalArith<__m256d>&& lvalue, const normalArith<__m256d>&& rvalue)
{
	return _mm256_mul_pd(lvalue, rvalue);
}

template<> inline normalArith<__m256> operator/(const normalArith<__m256>&& lvalue, const normalArith<__m256>&& rvalue)
{
	return _mm256_div_ps(lvalue, rvalue);
}
template<> inline normalArith<__m256d> operator/(const normalArith<__m256d>&& lvalue, const normalArith<__m256d>&& rvalue)
{
	return _mm256_div_pd(lvalue, rvalue);
}

template<> inline __m256 operator+(const dummyArith<__m256>&& lvalue, const dummyArith<__m256>&& rvalue)
{
	return _mm256_add_ps(lvalue, rvalue);
}
template<> inline __m256d operator+(const dummyArith<__m256d>&& lvalue, const dummyArith<__m256d>&& rvalue)
{
	return _mm256_add_pd(lvalue, rvalue);
}

template<> inline __m256 operator-(const dummyArith<__m256>&& lvalue, const dummyArith<__m256>&& rvalue)
{
	return _mm256_sub_ps(lvalue, rvalue);
}
template<> inline __m256d operator-(const dummyArith<__m256d>&& lvalue, const dummyArith<__m256d>&& rvalue)
{
	return _mm256_sub_pd(lvalue, rvalue);
}

template<> inline __m256 operator*(const dummyArith<__m256>&& lvalue, const dummyArith<__m256>&& rvalue)
{
	return _mm256_mul_ps(lvalue, rvalue);
}
template<> inline __m256d operator*(const dummyArith<__m256d>&& lvalue, const dummyArith<__m256d>&& rvalue)
{
	return _mm256_mul_pd(lvalue, rvalue);
}

template<> inline __m256 operator/(const dummyArith<__m256>&& lvalue, const dummyArith<__m256>&& rvalue)
{
	return _mm256_div_ps(lvalue, rvalue);
}
template<> inline __m256d operator/(const dummyArith<__m256d>&& lvalue, const dummyArith<__m256d>&& rvalue)
{
	return _mm256_div_pd(lvalue, rvalue);
}
#endif//GASHA_FAST_ARITH_USE_AVX

//----------------------------------------
//高速平方根
template<typename T> inline T sqrt(const T value){ return std::sqrt(value); }
template<typename T> inline fastArith<T> sqrt(const fastArith<T>&& value){ return std::sqrt(value); }
template<typename T> inline fastestArith<T> sqrt(const fastestArith<T>&& value){ return std::sqrt(value); }
template<typename T> inline semifastArith<T> sqrt(const semifastArith<T>&& value){ return std::sqrt(value); }
template<typename T> inline sseArith<T> sqrt(const sseArith<T>&& value){ return std::sqrt(value); }
template<typename T> inline normalArith<T> sqrt(const normalArith<T>&& value){ return std::sqrt(value); }
template<typename T> inline T sqrt(const dummyArith<T>&& value){ return std::sqrt(value); }

//----------------------------------------
//高速平方根：特殊化
#ifdef GASHA_FAST_ARITH_USE_SSE
template<> inline fastArith<float> sqrt(const fastArith<float>&& value)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	//【ステップ１】平方根の逆数の近似値を取得
	__m128 rcp_sqrt_m128 = _mm_rsqrt_ss(value);
	//【ステップ２】ニュートン法で逆数の精度を高める
	//ニュートン法による漸化式：rsqrt = rsqrt * (3 - value * rsqrt * rsqrt) * 0.5f
	//  平方根の漸化式:      x[i+1] = (x[i] + value / x[i]) * 0.5
	//  平方根の逆数の漸化式:x[i+1] = x[i] * (3 - value * x[i] ^ 2) * 0.5
	rcp_sqrt_m128 = _mm_mul_ss(_mm_mul_ss(rcp_sqrt_m128, _mm_sub_ss(_mm_set_ss(3.f), _mm_mul_ss(value, _mm_mul_ss(rcp_sqrt_m128, rcp_sqrt_m128)))), _mm_set_ss(0.5f));
	//【ステップ３】逆数を乗算
	const __m128 result_m128 = _mm_mul_ss(value, rcp_sqrt_m128);
	return result_m128;
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	return _mm_sqrt_ss(value);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
}
template<> inline fastestArith<float> sqrt(const fastestArith<float>&& value)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	__m128 rcp_sqrt_m128 = _mm_rsqrt_ss(value);//逆数の近似値算出
	return _mm_mul_ss(value, rcp_sqrt_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	return _mm_sqrt_ss(value);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
}
template<> inline semifastArith<float> sqrt(const semifastArith<float>&& value)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	__m128 rcp_sqrt_m128 = _mm_rsqrt_ss(value);//逆数の近似値算出
	rcp_sqrt_m128 = _mm_mul_ss(_mm_mul_ss(rcp_sqrt_m128, _mm_sub_ss(_mm_set_ss(3.f), _mm_mul_ss(value, _mm_mul_ss(rcp_sqrt_m128, rcp_sqrt_m128)))), _mm_set_ss(0.5f));//逆数の精度向上
	rcp_sqrt_m128 = _mm_mul_ss(_mm_mul_ss(rcp_sqrt_m128, _mm_sub_ss(_mm_set_ss(3.f), _mm_mul_ss(value, _mm_mul_ss(rcp_sqrt_m128, rcp_sqrt_m128)))), _mm_set_ss(0.5f));//2回目
	return _mm_mul_ss(value, rcp_sqrt_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	return _mm_sqrt_ss(value);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
}
template<> inline sseArith<float> sqrt(const sseArith<float>&& value)
{
	return _mm_sqrt_ss(value);
}

template<> inline fastArith<__m128> sqrt(const fastArith<__m128>&& value)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	__m128 rcp_sqrt_m128 = _mm_rsqrt_ps(value);//逆数の近似値算出
	rcp_sqrt_m128 = _mm_mul_ss(_mm_mul_ps(rcp_sqrt_m128, _mm_sub_ps(_mm_set1_ps(3.f), _mm_mul_ps(value, _mm_mul_ps(rcp_sqrt_m128, rcp_sqrt_m128)))), _mm_set1_ps(0.5f));//逆数の精度向上
	return _mm_mul_ps(value, rcp_sqrt_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	return _mm_sqrt_ps(value);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
}
template<> inline fastestArith<__m128> sqrt(const fastestArith<__m128>&& value)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	__m128 rcp_sqrt_m128 = _mm_rsqrt_ps(value);//逆数の近似値算出
	return _mm_mul_ps(value, rcp_sqrt_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	return _mm_sqrt_ps(value);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
}
template<> inline semifastArith<__m128> sqrt(const semifastArith<__m128>&& value)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	__m128 rcp_sqrt_m128 = _mm_rsqrt_ps(value);//逆数の近似値算出
	rcp_sqrt_m128 = _mm_mul_ss(_mm_mul_ps(rcp_sqrt_m128, _mm_sub_ps(_mm_set1_ps(3.f), _mm_mul_ps(value, _mm_mul_ps(rcp_sqrt_m128, rcp_sqrt_m128)))), _mm_set1_ps(0.5f));//逆数の精度向上
	rcp_sqrt_m128 = _mm_mul_ss(_mm_mul_ps(rcp_sqrt_m128, _mm_sub_ps(_mm_set1_ps(3.f), _mm_mul_ps(value, _mm_mul_ps(rcp_sqrt_m128, rcp_sqrt_m128)))), _mm_set1_ps(0.5f));//2回目
	return _mm_mul_ps(value, rcp_sqrt_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	return _mm_sqrt_ps(value);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
}
template<> inline sseArith<__m128> sqrt(const sseArith<__m128>&& value)
{
	return _mm_sqrt_ps(value);
}
template<> inline normalArith<__m128> sqrt(const normalArith<__m128>&& value)
{
	return _mm_sqrt_ps(value);
}
template<> inline __m128 sqrt(const dummyArith<__m128>&& value)
{
	return _mm_sqrt_ps(value);
}
#endif//GASHA_FAST_ARITH_USE_SSE
#ifdef GASHA_FAST_ARITH_USE_SSE2
template<> inline fastArith<double> sqrt(const fastArith<double>&& value)
{
	return _mm_sqrt_sd(value, value);
}
template<> inline fastestArith<double> sqrt(const fastestArith<double>&& value)
{
	return _mm_sqrt_sd(value, value);
}
template<> inline semifastArith<double> sqrt(const semifastArith<double>&& value)
{
	return _mm_sqrt_sd(value, value);
}
template<> inline sseArith<double> sqrt(const sseArith<double>&& value)
{
	return _mm_sqrt_sd(value, value);
}

template<> inline fastArith<__m128d> sqrt(const fastArith<__m128d>&& value)
{
	return _mm_sqrt_pd(value);
}
template<> inline fastestArith<__m128d> sqrt(const fastestArith<__m128d>&& value)
{
	return _mm_sqrt_pd(value);
}
template<> inline semifastArith<__m128d> sqrt(const semifastArith<__m128d>&& value)
{
	return _mm_sqrt_pd(value);
}
template<> inline sseArith<__m128d> sqrt(const sseArith<__m128d>&& value)
{
	return _mm_sqrt_pd(value);
}
template<> inline normalArith<__m128d> sqrt(const normalArith<__m128d>&& value)
{
	return _mm_sqrt_pd(value);
}
template<> inline __m128d sqrt(const dummyArith<__m128d>&& value)
{
	return _mm_sqrt_pd(value);
}
#endif//GASHA_FAST_ARITH_USE_SSE2
#ifdef GASHA_FAST_ARITH_USE_AVX
template<> inline fastArith<__m256> sqrt(const fastArith<__m256>&& value)
{
	return _mm256_sqrt_ps(value);
}
template<> inline fastestArith<__m256> sqrt(const fastestArith<__m256>&& value)
{
	return _mm256_sqrt_ps(value);
}
template<> inline semifastArith<__m256> sqrt(const semifastArith<__m256>&& value)
{
	return _mm256_sqrt_ps(value);
}
template<> inline sseArith<__m256> sqrt(const sseArith<__m256>&& value)
{
	return _mm256_sqrt_ps(value);
}
template<> inline normalArith<__m256> sqrt(const normalArith<__m256>&& value)
{
	return _mm256_sqrt_ps(value);
}
template<> inline __m256 sqrt(const dummyArith<__m256>&& value)
{
	return _mm256_sqrt_ps(value);
}

template<> inline fastArith<__m256d> sqrt(const fastArith<__m256d>&& value)
{
	return _mm256_sqrt_pd(value);
}
template<> inline fastestArith<__m256d> sqrt(const fastestArith<__m256d>&& value)
{
	return _mm256_sqrt_pd(value);
}
template<> inline semifastArith<__m256d> sqrt(const semifastArith<__m256d>&& value)
{
	return _mm256_sqrt_pd(value);
}
template<> inline sseArith<__m256d> sqrt(const sseArith<__m256d>&& value)
{
	return _mm256_sqrt_pd(value);
}
template<> inline normalArith<__m256d> sqrt(const normalArith<__m256d>&& value)
{
	return _mm256_sqrt_pd(value);
}
template<> inline __m256d sqrt(const dummyArith<__m256d>&& value)
{
	return _mm256_sqrt_pd(value);
}
#endif//GASHA_FAST_ARITH_USE_AVX

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_FAST_MATH_INL

// End of file

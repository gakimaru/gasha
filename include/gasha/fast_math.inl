#pragma once
#ifndef GASHA_INCLUDED_FAST_MATH_INL
#define GASHA_INCLUDED_FAST_MATH_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// fast_math.h
// 高速算術【インライン関数部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/fast_math.h>//高速算術【宣言部】

#include <cmath>//std::sqrt()
#include <memory.h>//memcpy(), memset()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//高速除算
//--------------------------------------------------------------------------------

//定義用マクロ
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

#define GASHA_FAST_ARITH_CLASS_ARRAY_INSTANCING(CLASS_NAME, TYPE, SIZE) \
	inline CLASS_NAME<TYPE[SIZE]>& CLASS_NAME<TYPE[SIZE]>::operator=(const TYPE val[SIZE]){ memcpy(m_val, val, sizeof(value_type)); return *this; } \
	inline CLASS_NAME<TYPE[SIZE]>& CLASS_NAME<TYPE[SIZE]>::operator=(const fastArith<TYPE[SIZE]>&& val){ memcpy(m_val, val.m_val, sizeof(value_type)); return *this; } \
	inline CLASS_NAME<TYPE[SIZE]>& CLASS_NAME<TYPE[SIZE]>::operator=(const fastArith<TYPE[SIZE]>& val){ memcpy(m_val, val.m_val, sizeof(value_type)); return *this; } \
	inline CLASS_NAME<TYPE[SIZE]>& CLASS_NAME<TYPE[SIZE]>::operator=(const fastestArith<TYPE[SIZE]>&& val){ memcpy(m_val, val.m_val, sizeof(value_type)); return *this; } \
	inline CLASS_NAME<TYPE[SIZE]>& CLASS_NAME<TYPE[SIZE]>::operator=(const fastestArith<TYPE[SIZE]>& val){ memcpy(m_val, val.m_val, sizeof(value_type)); return *this; } \
	inline CLASS_NAME<TYPE[SIZE]>& CLASS_NAME<TYPE[SIZE]>::operator=(const semifastArith<TYPE[SIZE]>&& val){ memcpy(m_val, val.m_val, sizeof(value_type)); return *this; } \
	inline CLASS_NAME<TYPE[SIZE]>& CLASS_NAME<TYPE[SIZE]>::operator=(const semifastArith<TYPE[SIZE]>& val){ memcpy(m_val, val.m_val, sizeof(value_type)); return *this; } \
	inline CLASS_NAME<TYPE[SIZE]>& CLASS_NAME<TYPE[SIZE]>::operator=(const sseArith<TYPE[SIZE]>&& val){ memcpy(m_val, val.m_val, sizeof(value_type)); return *this; } \
	inline CLASS_NAME<TYPE[SIZE]>& CLASS_NAME<TYPE[SIZE]>::operator=(const sseArith<TYPE[SIZE]>& val){ memcpy(m_val, val.m_val, sizeof(value_type)); return *this; } \
	inline CLASS_NAME<TYPE[SIZE]>& CLASS_NAME<TYPE[SIZE]>::operator=(const normalArith<TYPE[SIZE]>&& val){ memcpy(m_val, val.m_val, sizeof(value_type)); return *this; } \
	inline CLASS_NAME<TYPE[SIZE]>& CLASS_NAME<TYPE[SIZE]>::operator=(const normalArith<TYPE[SIZE]>& val){ memcpy(m_val, val.m_val, sizeof(value_type)); return *this; } \
	inline CLASS_NAME<TYPE[SIZE]>& CLASS_NAME<TYPE[SIZE]>::operator=(const dummyArith<TYPE[SIZE]>&& val){ memcpy(m_val, val.m_val, sizeof(value_type)); return *this; } \
	inline CLASS_NAME<TYPE[SIZE]>& CLASS_NAME<TYPE[SIZE]>::operator=(const dummyArith<TYPE[SIZE]>& val){ memcpy(m_val, val.m_val, sizeof(value_type)); return *this; } \
	inline CLASS_NAME<TYPE[SIZE]>::CLASS_NAME(const TYPE val[SIZE]){ memcpy(m_val, val, sizeof(value_type)); } \
	inline CLASS_NAME<TYPE[SIZE]>::CLASS_NAME(const fastArith<TYPE[SIZE]>&& val){ memcpy(m_val, val.m_val, sizeof(value_type)); } \
	inline CLASS_NAME<TYPE[SIZE]>::CLASS_NAME(const fastArith<TYPE[SIZE]>& val){ memcpy(m_val, val.m_val, sizeof(value_type)); } \
	inline CLASS_NAME<TYPE[SIZE]>::CLASS_NAME(const fastestArith<TYPE[SIZE]>&& val){ memcpy(m_val, val.m_val, sizeof(value_type)); } \
	inline CLASS_NAME<TYPE[SIZE]>::CLASS_NAME(const fastestArith<TYPE[SIZE]>& val){ memcpy(m_val, val.m_val, sizeof(value_type)); } \
	inline CLASS_NAME<TYPE[SIZE]>::CLASS_NAME(const semifastArith<TYPE[SIZE]>&& val){ memcpy(m_val, val.m_val, sizeof(value_type)); } \
	inline CLASS_NAME<TYPE[SIZE]>::CLASS_NAME(const semifastArith<TYPE[SIZE]>& val){ memcpy(m_val, val.m_val, sizeof(value_type)); } \
	inline CLASS_NAME<TYPE[SIZE]>::CLASS_NAME(const sseArith<TYPE[SIZE]>&& val){ memcpy(m_val, val.m_val, sizeof(value_type)); } \
	inline CLASS_NAME<TYPE[SIZE]>::CLASS_NAME(const sseArith<TYPE[SIZE]>& val){ memcpy(m_val, val.m_val, sizeof(value_type)); } \
	inline CLASS_NAME<TYPE[SIZE]>::CLASS_NAME(const normalArith<TYPE[SIZE]>&& val){ memcpy(m_val, val.m_val, sizeof(value_type)); } \
	inline CLASS_NAME<TYPE[SIZE]>::CLASS_NAME(const normalArith<TYPE[SIZE]>& val){ memcpy(m_val, val.m_val, sizeof(value_type)); } \
	inline CLASS_NAME<TYPE[SIZE]>::CLASS_NAME(const dummyArith<TYPE[SIZE]>&& val){ memcpy(m_val, val.m_val, sizeof(value_type)); } \
	inline CLASS_NAME<TYPE[SIZE]>::CLASS_NAME(const dummyArith<TYPE[SIZE]>& val){ memcpy(m_val, val.m_val, sizeof(value_type)); } \
	inline CLASS_NAME<TYPE[SIZE]>::CLASS_NAME(){ memset(m_val, 0, sizeof(value_type)); } \
	inline CLASS_NAME<TYPE[SIZE]>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_FLOAT3_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const float val[3]){ m_val = _mm_loadu_ps(val); return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const __m128&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const __m128& val){ m_val = val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const fastArith<float[3]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const fastArith<float[3]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const fastestArith<float[3]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const fastestArith<float[3]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const semifastArith<float[3]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const semifastArith<float[3]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const sseArith<float[3]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const sseArith<float[3]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const normalArith<float[3]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const normalArith<float[3]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const dummyArith<float[3]>&& val){ m_val = _mm_loadu_ps(val.m_val); return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const dummyArith<float[3]>& val){ m_val = _mm_loadu_ps(val.m_val); return *this; } \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const float val[3]) : m_val(_mm_loadu_ps(val)){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const __m128&& val) : m_val(val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const __m128& val) : m_val(val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const fastArith<float[3]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const fastArith<float[3]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const fastestArith<float[3]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const fastestArith<float[3]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const semifastArith<float[3]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const semifastArith<float[3]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const sseArith<float[3]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const sseArith<float[3]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const normalArith<float[3]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const normalArith<float[3]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const dummyArith<float[3]>&& val) : m_val(_mm_loadu_ps(val.m_val)){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const dummyArith<float[3]>& val) : m_val(_mm_loadu_ps(val.m_val)){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME() : m_val(_mm_setzero_ps()){} \
	inline CLASS_NAME<float[3]>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_FLOAT4_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const float val[4]){ m_val = _mm_loadu_ps(val); return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const __m128&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const __m128& val){ m_val = val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const fastArith<float[4]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const fastArith<float[4]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const fastestArith<float[4]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const fastestArith<float[4]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const semifastArith<float[4]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const semifastArith<float[4]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const sseArith<float[4]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const sseArith<float[4]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const normalArith<float[4]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const normalArith<float[4]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const dummyArith<float[4]>&& val){ m_val = _mm_loadu_ps(val.m_val); return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const dummyArith<float[4]>& val){ m_val = _mm_loadu_ps(val.m_val); return *this; } \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const float val[4]) : m_val(_mm_loadu_ps(val)){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const __m128&& val) : m_val(val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const __m128& val) : m_val(val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const fastArith<float[4]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const fastArith<float[4]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const fastestArith<float[4]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const fastestArith<float[4]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const semifastArith<float[4]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const semifastArith<float[4]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const sseArith<float[4]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const sseArith<float[4]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const normalArith<float[4]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const normalArith<float[4]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const dummyArith<float[4]>&& val) : m_val(_mm_loadu_ps(val.m_val)){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const dummyArith<float[4]>& val) : m_val(_mm_loadu_ps(val.m_val)){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME() : m_val(_mm_setzero_ps()){} \
	inline CLASS_NAME<float[4]>::~CLASS_NAME(){} \

#ifdef GASHA_FAST_ARITH_USE_SSE
#define GASHA_FAST_ARITH_CLASS_SSE_INSTANCING(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_SSE_FLOAT_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_M128_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_ARRAY_INSTANCING(CLASS_NAME, float, 2); \
	GASHA_FAST_ARITH_CLASS_SSE_FLOAT3_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_FLOAT4_INSTANCING(CLASS_NAME);
#else//GASHA_FAST_ARITH_USE_SSE
#define GASHA_FAST_ARITH_CLASS_SSE_INSTANCING(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_ARRAY_INSTANCING(CLASS_NAME, float, 3); \
	GASHA_FAST_ARITH_CLASS_ARRAY_INSTANCING(CLASS_NAME, float, 4);
#endif//GASHA_FAST_ARITH_USE_SSE

#ifdef GASHA_FAST_ARITH_USE_SSE2
#define GASHA_FAST_ARITH_CLASS_SSE2_INSTANCING(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_SSE_DOUBLE_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_M128D_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_ARRAY_INSTANCING(CLASS_NAME, double, 2);
#else//GASHA_FAST_ARITH_USE_SSE2
#define GASHA_FAST_ARITH_CLASS_SSE2_INSTANCING(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_ARRAY_INSTANCING(CLASS_NAME, double, 2);
#endif//GASHA_FAST_ARITH_USE_SSE2

#ifdef GASHA_FAST_ARITH_USE_AVX
#define GASHA_FAST_ARITH_CLASS_AVX_INSTANCING(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_SSE_M256_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_M256D_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_ARRAY_INSTANCING(CLASS_NAME, double, 3); \
	GASHA_FAST_ARITH_CLASS_ARRAY_INSTANCING(CLASS_NAME, double, 4);
#else//GASHA_FAST_ARITH_USE_SSE
#define GASHA_FAST_ARITH_CLASS_AVX_INSTANCING(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_ARRAY_INSTANCING(CLASS_NAME, double, 3); \
	GASHA_FAST_ARITH_CLASS_ARRAY_INSTANCING(CLASS_NAME, double, 4);
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
	const __m128 lvalue_m128 = lvalue;
	const __m128 rvalue_m128 = rvalue;
	//【ステップ１】逆数の近似値を取得
	__m128 rcp_div_m128 = _mm_rcp_ps(rvalue_m128);//逆数の近似値を高速に算出するSSE命令
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
	const __m128 const2_m128 = _mm_set_ss(2.f);
	rcp_div_m128 = _mm_mul_ss(rcp_div_m128, _mm_sub_ss(const2_m128, _mm_mul_ss(rvalue_m128, rcp_div_m128)));
	//【ステップ３】逆数を乗算
	return _mm_mul_ss(lvalue_m128, rcp_div_m128);
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
	return _mm_div_ss(lvalue, rvalue);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
}
template<> inline fastArith<__m128> operator/(const fastArith<__m128>&& lvalue, const fastArith<__m128>&& rvalue)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で除算
	const __m128 lvalue_m128 = lvalue;
	const __m128 rvalue_m128 = rvalue;
	__m128 rcp_div_m128 = _mm_rcp_ps(rvalue_m128);//逆数の近似値算出
	const __m128 const2_m128 = _mm_set1_ps(2.f);
	rcp_div_m128 = _mm_mul_ps(rcp_div_m128, _mm_sub_ps(const2_m128, _mm_mul_ps(rvalue_m128, rcp_div_m128)));//逆数の精度向上
	return _mm_mul_ps(lvalue_m128, rcp_div_m128);//逆数を乗算
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
	const __m128 lvalue_m128 = lvalue;
	const __m128 rvalue_m128 = rvalue;
	const __m128 rcp_div_m128 = _mm_rcp_ss(rvalue_m128);//逆数の近似値算出
	return _mm_mul_ss(lvalue_m128, rcp_div_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
	return _mm_div_ss(lvalue, rvalue);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
}
template<> inline fastestArith<__m128> operator/(const fastestArith<__m128>&& lvalue, const fastestArith<__m128>&& rvalue)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で除算
	const __m128 lvalue_m128 = lvalue;
	const __m128 rvalue_m128 = rvalue;
	const __m128 rcp_div_m128 = _mm_rcp_ps(rvalue_m128);//逆数の近似値算出
	return _mm_mul_ps(lvalue_m128, rcp_div_m128);//逆数を乗算
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
	const __m128 lvalue_m128 = lvalue;
	const __m128 rvalue_m128 = rvalue;
	__m128 rcp_div_m128 = _mm_rcp_ss(rvalue_m128);//逆数の近似値算出
	const __m128 const2_m128 = _mm_set_ss(2.f);
	rcp_div_m128 = _mm_mul_ss(rcp_div_m128, _mm_sub_ss(const2_m128, _mm_mul_ps(rvalue_m128, rcp_div_m128)));//逆数の精度向上
	rcp_div_m128 = _mm_mul_ss(rcp_div_m128, _mm_sub_ss(const2_m128, _mm_mul_ps(rvalue_m128, rcp_div_m128)));//2回目
	return _mm_mul_ss(lvalue_m128, rcp_div_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
	return _mm_div_ss(lvalue, rvalue);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
}
template<> inline semifastArith<__m128> operator/(const semifastArith<__m128>&& lvalue, const semifastArith<__m128>&& rvalue)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で除算
	const __m128 lvalue_m128 = lvalue;
	const __m128 rvalue_m128 = rvalue;
	__m128 rcp_div_m128 = _mm_rcp_ps(rvalue);//逆数の近似値算出
	const __m128 const2_m128 = _mm_set1_ps(2.f);
	rcp_div_m128 = _mm_mul_ps(rcp_div_m128, _mm_sub_ps(const2_m128, _mm_mul_ps(rvalue_m128, rcp_div_m128)));//逆数の精度向上
	rcp_div_m128 = _mm_mul_ps(rcp_div_m128, _mm_sub_ps(const2_m128, _mm_mul_ps(rvalue_m128, rcp_div_m128)));//2回目
	return _mm_mul_ps(lvalue_m128, rcp_div_m128);//逆数を乗算
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
	const __m128 value_m128 = value;
	//【ステップ１】平方根の逆数の近似値を取得
	__m128 rcp_sqrt_m128 = _mm_rsqrt_ss(value_m128);
	//【ステップ２】ニュートン法で逆数の精度を高める
	//ニュートン法による漸化式：rsqrt = rsqrt * (3 - value * rsqrt * rsqrt) * 0.5f
	//  平方根の漸化式:      x[i+1] = (x[i] + value / x[i]) * 0.5
	//  平方根の逆数の漸化式:x[i+1] = x[i] * (3 - value * x[i] ^ 2) * 0.5
	const __m128 const3_m128 = _mm_set_ss(3.f);
	const __m128 const0_5_m128 = _mm_set_ss(0.5f);
	rcp_sqrt_m128 = _mm_mul_ss(_mm_mul_ss(rcp_sqrt_m128, _mm_sub_ss(const3_m128, _mm_mul_ss(value_m128, _mm_mul_ss(rcp_sqrt_m128, rcp_sqrt_m128)))), const0_5_m128);
	//【ステップ３】逆数を乗算
	return _mm_mul_ss(value_m128, rcp_sqrt_m128);
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	return _mm_sqrt_ss(value);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
}
template<> inline fastestArith<float> sqrt(const fastestArith<float>&& value)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	const __m128 value_m128 = value;
	const __m128 rcp_sqrt_m128 = _mm_rsqrt_ss(value_m128);//逆数の近似値算出
	return _mm_mul_ss(value_m128, rcp_sqrt_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	return _mm_sqrt_ss(value);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
}
template<> inline semifastArith<float> sqrt(const semifastArith<float>&& value)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	const __m128 value_m128 = value;
	__m128 rcp_sqrt_m128 = _mm_rsqrt_ss(value_m128);//逆数の近似値算出
	const __m128 const3_m128 = _mm_set_ss(3.f);
	const __m128 const0_5_m128 = _mm_set_ss(0.5f);
	rcp_sqrt_m128 = _mm_mul_ss(_mm_mul_ss(rcp_sqrt_m128, _mm_sub_ss(const3_m128, _mm_mul_ss(value_m128, _mm_mul_ss(rcp_sqrt_m128, rcp_sqrt_m128)))), const0_5_m128);//逆数の精度向上
	rcp_sqrt_m128 = _mm_mul_ss(_mm_mul_ss(rcp_sqrt_m128, _mm_sub_ss(const3_m128, _mm_mul_ss(value_m128, _mm_mul_ss(rcp_sqrt_m128, rcp_sqrt_m128)))), const0_5_m128);//2回目
	return _mm_mul_ss(value_m128, rcp_sqrt_m128);//逆数を乗算
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
	const __m128 value_m128 = value;
	__m128 rcp_sqrt_m128 = _mm_rsqrt_ps(value_m128);//逆数の近似値算出
	const __m128 const3_m128 = _mm_set1_ps(3.f);
	const __m128 const0_5_m128 = _mm_set1_ps(0.5f);
	rcp_sqrt_m128 = _mm_mul_ps(_mm_mul_ps(rcp_sqrt_m128, _mm_sub_ps(const3_m128, _mm_mul_ps(value_m128, _mm_mul_ps(rcp_sqrt_m128, rcp_sqrt_m128)))), const0_5_m128);//逆数の精度向上
	return _mm_mul_ps(value_m128, rcp_sqrt_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	return _mm_sqrt_ps(value);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
}
template<> inline fastestArith<__m128> sqrt(const fastestArith<__m128>&& value)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	const __m128 value_m128 = value;
	const __m128 rcp_sqrt_m128 = _mm_rsqrt_ps(value_m128);//逆数の近似値算出
	return _mm_mul_ps(value_m128, rcp_sqrt_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	return _mm_sqrt_ps(value);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
}
template<> inline semifastArith<__m128> sqrt(const semifastArith<__m128>&& value)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	const __m128 value_m128 = value;
	__m128 rcp_sqrt_m128 = _mm_rsqrt_ps(value_m128);//逆数の近似値算出
	const __m128 const3_m128 = _mm_set1_ps(3.f);
	const __m128 const0_5_m128 = _mm_set1_ps(0.5f);
	rcp_sqrt_m128 = _mm_mul_ps(_mm_mul_ps(rcp_sqrt_m128, _mm_sub_ps(const3_m128, _mm_mul_ps(value_m128, _mm_mul_ps(rcp_sqrt_m128, rcp_sqrt_m128)))), const0_5_m128);//逆数の精度向上
	rcp_sqrt_m128 = _mm_mul_ps(_mm_mul_ps(rcp_sqrt_m128, _mm_sub_ps(const3_m128, _mm_mul_ps(value_m128, _mm_mul_ps(rcp_sqrt_m128, rcp_sqrt_m128)))), const0_5_m128);//2回目
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

//--------------------------------------------------------------------------------
//高速ベクトル演算
//--------------------------------------------------------------------------------

#define GASHA_FAST_VECTOR_SET_INSTANCING(CLASS_NAME) \
	template<typename T, std::size_t N> inline T norm(const CLASS_NAME<T[N]>& vec){ return GASHA_ norm(*vec); } \
	template<typename T, std::size_t N> inline T normSq(const CLASS_NAME<T[N]>& vec){ return GASHA_ normSq(*vec); } \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> merge(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2){ T result[N] = { 0 }; GASHA_ merge(result, *vec1, *vec2); return CLASS_NAME <T[N]>(result); } \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> difference(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2){ T result[N] = { 0 }; GASHA_ difference(result, *vec1, *vec2); return CLASS_NAME <T[N]>(result); } \
	template<typename T, std::size_t N> inline T length(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2){ return GASHA_ length(*vec1, *vec2); } \
	template<typename T, std::size_t N> inline T lengthSq(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2){ return GASHA_ lengthSq(*vec1, *vec2); } \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> normalize(const CLASS_NAME<T[N]>& vec){ T result[N] = { 0 }; GASHA_ normalize(result, *vec); return CLASS_NAME <T[N]>(result); } \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> mul(const CLASS_NAME<T[N]>& vec, const T scalar){ T result[N] = { 0 }; GASHA_ mul(result, *vec, scalar); return CLASS_NAME <T[N]>(result); } \
	template<typename T, std::size_t N> inline T dot(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2){ return GASHA_ dot(*vec1, *vec2); } \
	template<typename T, std::size_t N> inline T normalizedDot(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2){ return GASHA_ normalizedDot(*vec1, *vec2); } \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> cross(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2){ T result[N] = { 0 }; GASHA_ cross(result, *vec1, *vec2); return CLASS_NAME <T[N]>(result); }

GASHA_FAST_VECTOR_SET_INSTANCING(fastArith);
GASHA_FAST_VECTOR_SET_INSTANCING(fastestArith);
GASHA_FAST_VECTOR_SET_INSTANCING(semifastArith);
GASHA_FAST_VECTOR_SET_INSTANCING(sseArith);
GASHA_FAST_VECTOR_SET_INSTANCING(normalArith);

template<typename T, std::size_t N> inline T norm(const dummyArith<T[N]>& vec){ return GASHA_ norm(*vec); }
template<typename T, std::size_t N> inline T normSq(const dummyArith<T[N]>& vec){ return GASHA_ normSq(*vec); }
template<typename T, std::size_t N> inline void merge(T (&result)[N], const dummyArith<T[N]>& vec1, const dummyArith<T[N]>& vec2){ GASHA_ merge(result, *vec1, *vec2); }
template<typename T, std::size_t N> inline void difference(T(&result)[N], const dummyArith<T[N]>& vec1, const dummyArith<T[N]>& vec2){ GASHA_ difference(result, *vec1, *vec2); }
template<typename T, std::size_t N> inline T length(const dummyArith<T[N]>& vec1, const dummyArith<T[N]>& vec2){ return GASHA_ length(*vec1, *vec2); }
template<typename T, std::size_t N> inline T lengthSq(const dummyArith<T[N]>& vec1, const dummyArith<T[N]>& vec2){ return GASHA_ lengthSq(*vec1, *vec2); }
template<typename T, std::size_t N> inline void normalize(T(&result)[N], const dummyArith<T[N]>& vec){ GASHA_ normalize(result, *vec); }
template<typename T, std::size_t N> inline void mul(T(&result)[N], const dummyArith<T[N]>& vec, const T scalar){ GASHA_ mul(result, *vec, scalar); }
template<typename T, std::size_t N> inline T dot(const dummyArith<T[N]>& vec1, const dummyArith<T[N]>& vec2){ return GASHA_ dot(*vec1, *vec2); }
template<typename T, std::size_t N> inline T normalizedDot(const dummyArith<T[N]>& vec1, const dummyArith<T[N]>& vec2){ return GASHA_ normalizedDot(*vec1, *vec2); }
template<typename T, std::size_t N> inline void cross(T(&result)[N], const dummyArith<T[N]>& vec1, const dummyArith<T[N]>& vec2){ GASHA_ cross(result, *vec1, *vec2); }

//----------------------------------------
//高速ベクトル演算：特殊化
#ifdef GASHA_FAST_ARITH_USE_SSE4_1

//--------------------
//SSE命令専用

//ノルム
inline __m128 m128_norm(const __m128 vec_m128)
{
	return _mm_sqrt_ss(_mm_dp_ps(vec_m128, vec_m128, 0x71));
}
//ノルムの二乗
inline __m128 m128_normSq(const __m128 vec_m128)
{
	return _mm_dp_ps(vec_m128, vec_m128, 0x71);
}
//合成（加算）
inline __m128 m128_merge(const __m128 vec1_m128, const __m128 vec2_m128)
{
	return _mm_add_ps(vec1_m128, vec2_m128);
}
//二点間の差（減算）
inline __m128 m128_difference(const __m128 vec1_m128, const __m128 vec2_m128)
{
	return _mm_sub_ps(vec1_m128, vec2_m128);
}
//二点間の長さ
inline __m128 m128_length(const __m128 vec1_m128, const __m128 vec2_m128)
{
	const __m128 diff_m128 = _mm_sub_ps(vec1_m128, vec2_m128);
	return _mm_sqrt_ss(_mm_dp_ps(diff_m128, diff_m128, 0x71));
}
//二点間の長さの二乗
inline __m128 m128_lengthSq(const __m128 vec1_m128, const __m128 vec2_m128)
{
	const __m128 diff_m128 = _mm_sub_ps(vec1_m128, vec2_m128);
	return _mm_dp_ps(diff_m128, diff_m128, 0x71);
}
//正規化
inline __m128 m128_normalize(const __m128 vec_m128)
{
	const __m128 norm_m128 = _mm_sqrt_ps(_mm_dp_ps(vec_m128, vec_m128, 0x7f));
	return _mm_div_ps(vec_m128, norm_m128);
}
//スカラー倍
inline __m128 m128_mul(const __m128 vec_m128, const float scalar)
{
	const __m128 scalar_m128 = _mm_set1_ps(scalar);
	return _mm_mul_ps(vec_m128, scalar_m128);
}
//内積
inline __m128 m128_dot(const __m128 vec1_m128, const __m128 vec2_m128)
{
	return _mm_dp_ps(vec1_m128, vec2_m128, 0x71);
}
//正規化して内積
inline __m128 m128_normalizedDot(const __m128 vec1_m128, const __m128 vec2_m128)
{
	//vec1正規化
	__m128 norm1_m128 = _mm_sqrt_ps(_mm_dp_ps(vec1_m128, vec1_m128, 0x7f));
	const __m128 nvec1_m128 = _mm_div_ps(vec1_m128, norm1_m128);
	//vec2正規化
	__m128 norm2_m128 = _mm_sqrt_ps(_mm_dp_ps(vec2_m128, vec2_m128, 0x7f));
	const __m128 nvec2_m128 = _mm_div_ps(vec2_m128, norm2_m128);
	//内積
	return _mm_dp_ps(nvec1_m128, nvec2_m128, 0x71);
}
//外積
inline __m128 m128_cross(const __m128 vec1_m128, const __m128 vec2_m128)
{
	const __m128 vec1_a_m128 = _mm_shuffle_ps(vec1_m128, vec1_m128, _MM_SHUFFLE(3, 0, 2, 1));
	const __m128 vec2_a_m128 = _mm_shuffle_ps(vec2_m128, vec2_m128, _MM_SHUFFLE(3, 1, 0, 2));
	const __m128 vec1_b_m128 = _mm_shuffle_ps(vec1_m128, vec1_m128, _MM_SHUFFLE(3, 1, 0, 2));
	const __m128 vec2_b_m128 = _mm_shuffle_ps(vec2_m128, vec2_m128, _MM_SHUFFLE(3, 0, 2, 1));
	const __m128 vec_a_m128 = _mm_mul_ps(vec1_a_m128, vec2_a_m128);
	const __m128 vec_b_m128 = _mm_mul_ps(vec1_b_m128, vec2_b_m128);
	return _mm_sub_ps(vec_a_m128, vec_b_m128);
}

//--------------------
//特殊化

#define GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(CLASS_NAME, T, N) \
	template<> inline T norm<T, N>(const CLASS_NAME<T[N]>& vec) \
	{ \
		T result; \
		_mm_store_ss(&result, m128_norm(vec)); \
		return result; \
	} \
	template<> inline T normSq<T, N>(const CLASS_NAME<T[N]>& vec) \
	{ \
		T result; \
		_mm_store_ss(&result, m128_normSq(vec)); \
		return result; \
	} \
	template<> inline CLASS_NAME<T[N]> merge<T, N>(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2) \
	{ \
		return m128_merge(vec1, vec2); \
	} \
	template<> inline CLASS_NAME<T[N]> difference<T, N>(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2) \
	{ \
		return m128_difference(vec1, vec2); \
	} \
	template<> inline T length<T, N>(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2) \
	{ \
		T result; \
		_mm_store_ss(&result, m128_length(vec1, vec2)); \
		return result; \
	} \
	template<> inline T lengthSq<T, N>(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2) \
	{ \
		T result; \
		_mm_store_ss(&result, m128_lengthSq(vec1, vec2)); \
		return result; \
	} \
	template<> inline CLASS_NAME<T[N]> normalize<T, N>(const CLASS_NAME<T[N]>& vec) \
	{ \
		return m128_normalize(vec); \
	} \
	template<> inline CLASS_NAME<T[N]> mul<T, N>(const CLASS_NAME<T[N]>& vec, const T scalar) \
	{ \
		return m128_mul(vec, scalar); \
	} \
	template<> inline T dot<T, N>(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2) \
	{ \
		T result; \
		_mm_store_ss(&result, m128_dot(vec1, vec2)); \
		return result; \
	} \
	template<> inline T normalizedDot<T, N>(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2) \
	{ \
		T result; \
		_mm_store_ss(&result, m128_normalizedDot(vec1, vec2)); \
		return result; \
	} \
	template<> inline CLASS_NAME<T[N]> cross<T, N>(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2) \
	{ \
		return m128_cross(vec1, vec2); \
	}

GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(fastArith, float, 3);
GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(fastestArith, float, 3);
GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(semifastArith, float, 3);
GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(sseArith, float, 3);

GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(fastArith, float, 4);
GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(fastestArith, float, 4);
GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(semifastArith, float, 4);
GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(sseArith, float, 4);
#endif//GASHA_FAST_ARITH_USE_SSE4_1

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_FAST_MATH_INL

// End of file

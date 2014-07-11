#pragma once
#ifndef GASHA_INCLUDED_FAST_MATH_INL
#define GASHA_INCLUDED_FAST_MATH_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// fast_math.inl
// 高速算術【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/fast_math.h>//高速算術【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//高速除算
//--------------------------------------------------------------------------------

//定義用マクロ
#define GASHA_FAST_ARITH_CLASS_INSTANCING(CLASS_NAME) \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const T val){ m_val = val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const fastA<T>&& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const fastA<T>& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const fastestA<T>&& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const fastestA<T>& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const semiA<T>&& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const semiA<T>& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const sseA<T>&& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const sseA<T>& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const normA<T>&& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const normA<T>& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const dummyA<T>&& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>& CLASS_NAME<T>::operator=(const dummyA<T>& val){ m_val = val.m_val; return *this; } \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const T val) : m_val(val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const fastA<T>&& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const fastA<T>& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const fastestA<T>&& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const fastestA<T>& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const semiA<T>&& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const semiA<T>& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const sseA<T>&& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const sseA<T>& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const normA<T>&& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const normA<T>& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const dummyA<T>&& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME(const dummyA<T>& val) : m_val(val.m_val){} \
	template <typename T> inline CLASS_NAME<T>::CLASS_NAME() : m_val(static_cast<T>(0)){} \
	template <typename T> inline CLASS_NAME<T>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_ARRAY_INSTANCING(CLASS_NAME) \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>& CLASS_NAME<T[N]>::operator=(const T(&&val)[N]){ for (int i = 0; i < N; ++i) m_val[i] = val[i]; return *this; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>& CLASS_NAME<T[N]>::operator=(const T(&val)[N]){ for (int i = 0; i < N; ++i) m_val[i] = val[i]; return *this; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>& CLASS_NAME<T[N]>::operator=(const fastA<T[N]>&& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; return *this; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>& CLASS_NAME<T[N]>::operator=(const fastA<T[N]>& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; return *this; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>& CLASS_NAME<T[N]>::operator=(const fastestA<T[N]>&& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; return *this; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>& CLASS_NAME<T[N]>::operator=(const fastestA<T[N]>& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; return *this; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>& CLASS_NAME<T[N]>::operator=(const semiA<T[N]>&& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; return *this; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>& CLASS_NAME<T[N]>::operator=(const semiA<T[N]>& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; return *this; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>& CLASS_NAME<T[N]>::operator=(const sseA<T[N]>&& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; return *this; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>& CLASS_NAME<T[N]>::operator=(const sseA<T[N]>& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; return *this; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>& CLASS_NAME<T[N]>::operator=(const normA<T[N]>&& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; return *this; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>& CLASS_NAME<T[N]>::operator=(const normA<T[N]>& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; return *this; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>& CLASS_NAME<T[N]>::operator=(const dummyA<T[N]>&& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; return *this; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>& CLASS_NAME<T[N]>::operator=(const dummyA<T[N]>& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; return *this; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(const T(&&val)[N]){ for (int i = 0; i < N; ++i) m_val[i] = val[i]; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(const T(&val)[N]){ for (int i = 0; i < N; ++i) m_val[i] = val[i]; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(const fastA<T[N]>&& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(const fastA<T[N]>& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(const fastestA<T[N]>&& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(const fastestA<T[N]>& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(const semiA<T[N]>&& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(const semiA<T[N]>& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(const sseA<T[N]>&& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(const sseA<T[N]>& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(const normA<T[N]>&& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(const normA<T[N]>& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(const dummyA<T[N]>&& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(const dummyA<T[N]>& val){ for(int i = 0; i < N; ++i) m_val[i] = val.m_val[i]; } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::CLASS_NAME(){ for (int i = 0; i < N; ++i) m_val[i] = static_cast<T>(0); } \
	template <typename T, std::size_t N> inline CLASS_NAME<T[N]>::~CLASS_NAME(){} \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>& CLASS_NAME<T[N][M]>::operator=(const T(&&val)[N][M]){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val[i][j]; return *this; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>& CLASS_NAME<T[N][M]>::operator=(const T(&val)[N][M]){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val[i][j]; return *this; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>& CLASS_NAME<T[N][M]>::operator=(const fastA<T[N][M]>&& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; return *this; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>& CLASS_NAME<T[N][M]>::operator=(const fastA<T[N][M]>& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; return *this; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>& CLASS_NAME<T[N][M]>::operator=(const fastestA<T[N][M]>&& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; return *this; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>& CLASS_NAME<T[N][M]>::operator=(const fastestA<T[N][M]>& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; return *this; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>& CLASS_NAME<T[N][M]>::operator=(const semiA<T[N][M]>&& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; return *this; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>& CLASS_NAME<T[N][M]>::operator=(const semiA<T[N][M]>& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; return *this; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>& CLASS_NAME<T[N][M]>::operator=(const sseA<T[N][M]>&& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; return *this; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>& CLASS_NAME<T[N][M]>::operator=(const sseA<T[N][M]>& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; return *this; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>& CLASS_NAME<T[N][M]>::operator=(const normA<T[N][M]>&& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; return *this; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>& CLASS_NAME<T[N][M]>::operator=(const normA<T[N][M]>& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; return *this; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>& CLASS_NAME<T[N][M]>::operator=(const dummyA<T[N][M]>&& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; return *this; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>& CLASS_NAME<T[N][M]>::operator=(const dummyA<T[N][M]>& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; return *this; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(const T(&&val)[N][M]){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val[i][j]; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(const T(&val)[N][M]){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val[i][j]; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(const fastA<T[N][M]>&& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(const fastA<T[N][M]>& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(const fastestA<T[N][M]>&& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(const fastestA<T[N][M]>& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(const semiA<T[N][M]>&& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(const semiA<T[N][M]>& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(const sseA<T[N][M]>&& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(const sseA<T[N][M]>& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(const normA<T[N][M]>&& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(const normA<T[N][M]>& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(const dummyA<T[N][M]>&& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(const dummyA<T[N][M]>& val){ for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) m_val[i][j] = val.m_val[i][j]; } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::CLASS_NAME(){ for (int i = 0; i < N; ++i) for (int j = 0; i < M; ++i) m_val[i][j] = static_cast<T>(0); } \
	template <typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_FLOAT_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const float val){ m_val = _mm_set_ss(val); return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const __m128&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const __m128& val){ m_val = val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const fastA<float>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const fastA<float>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const fastestA<float>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const fastestA<float>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const semiA<float>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const semiA<float>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const sseA<float>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const sseA<float>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const normA<float>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const normA<float>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const dummyA<float>&& val){ m_val = _mm_set_ss(val.m_val); return *this; } \
	inline CLASS_NAME<float>& CLASS_NAME<float>::operator=(const dummyA<float>& val){ m_val = _mm_set_ss(val.m_val); return *this; } \
	inline CLASS_NAME<float>::CLASS_NAME(const float val) : m_val(_mm_set_ss(val)){} \
	inline CLASS_NAME<float>::CLASS_NAME(const __m128&& val) : m_val(val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const __m128& val) : m_val(val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const fastA<float>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const fastA<float>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const fastestA<float>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const fastestA<float>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const semiA<float>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const semiA<float>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const sseA<float>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const sseA<float>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const normA<float>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const normA<float>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float>::CLASS_NAME(const dummyA<float>&& val) : m_val(_mm_set_ss(val.m_val)){} \
	inline CLASS_NAME<float>::CLASS_NAME(const dummyA<float>& val) : m_val(_mm_set_ss(val.m_val)){} \
	inline CLASS_NAME<float>::CLASS_NAME() : m_val(_mm_setzero_ps()){} \
	inline CLASS_NAME<float>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_DOUBLE_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const double val){ m_val = _mm_set_sd(val); return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const __m128d&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const __m128d& val){ m_val = val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const fastA<double>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const fastA<double>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const fastestA<double>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const fastestA<double>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const semiA<double>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const semiA<double>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const sseA<double>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const sseA<double>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const normA<double>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const normA<double>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const dummyA<double>&& val){ m_val = _mm_set_sd(val.m_val); return *this; } \
	inline CLASS_NAME<double>& CLASS_NAME<double>::operator=(const dummyA<double>& val){ m_val = _mm_set_sd(val.m_val); return *this; } \
	inline CLASS_NAME<double>::CLASS_NAME(const double val) : m_val(_mm_set_sd(val)){} \
	inline CLASS_NAME<double>::CLASS_NAME(const __m128d&& val) : m_val(val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const __m128d& val) : m_val(val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const fastA<double>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const fastA<double>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const fastestA<double>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const fastestA<double>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const semiA<double>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const semiA<double>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const sseA<double>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const sseA<double>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const normA<double>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const normA<double>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<double>::CLASS_NAME(const dummyA<double>&& val) : m_val(_mm_set_sd(val.m_val)){} \
	inline CLASS_NAME<double>::CLASS_NAME(const dummyA<double>& val) : m_val(_mm_set_sd(val.m_val)){} \
	inline CLASS_NAME<double>::CLASS_NAME() : m_val(_mm_setzero_pd()){} \
	inline CLASS_NAME<double>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_M128_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const __m128&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const __m128& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const fastA<__m128>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const fastA<__m128>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const fastestA<__m128>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const fastestA<__m128>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const semiA<__m128>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const semiA<__m128>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const sseA<__m128>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const sseA<__m128>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const normA<__m128>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const normA<__m128>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const dummyA<__m128>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>& CLASS_NAME<__m128>::operator=(const dummyA<__m128>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128>::CLASS_NAME(const __m128&& val) : m_val(val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const __m128& val) : m_val(val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const fastA<__m128>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const fastA<__m128>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const fastestA<__m128>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const fastestA<__m128>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const semiA<__m128>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const semiA<__m128>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const sseA<__m128>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const sseA<__m128>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const normA<__m128>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const normA<__m128>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const dummyA<__m128>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME(const dummyA<__m128>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128>::CLASS_NAME() : m_val(_mm_setzero_ps()){} \
	inline CLASS_NAME<__m128>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_M128D_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const __m128d&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const __m128d& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const fastA<__m128d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const fastA<__m128d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const fastestA<__m128d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const fastestA<__m128d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const semiA<__m128d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const semiA<__m128d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const sseA<__m128d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const sseA<__m128d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const normA<__m128d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const normA<__m128d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const dummyA<__m128d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>& CLASS_NAME<__m128d>::operator=(const dummyA<__m128d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const __m128d&& val) : m_val(val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const __m128d& val) : m_val(val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const fastA<__m128d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const fastA<__m128d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const fastestA<__m128d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const fastestA<__m128d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const semiA<__m128d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const semiA<__m128d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const sseA<__m128d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const sseA<__m128d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const normA<__m128d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const normA<__m128d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const dummyA<__m128d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME(const dummyA<__m128d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m128d>::CLASS_NAME() : m_val(_mm_setzero_pd()){} \
	inline CLASS_NAME<__m128d>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_M256_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const __m256&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const __m256& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const fastA<__m256>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const fastA<__m256>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const fastestA<__m256>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const fastestA<__m256>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const semiA<__m256>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const semiA<__m256>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const sseA<__m256>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const sseA<__m256>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const normA<__m256>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const normA<__m256>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const dummyA<__m256>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>& CLASS_NAME<__m256>::operator=(const dummyA<__m256>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256>::CLASS_NAME(const __m256&& val) : m_val(val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const __m256& val) : m_val(val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const fastA<__m256>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const fastA<__m256>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const fastestA<__m256>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const fastestA<__m256>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const semiA<__m256>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const semiA<__m256>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const sseA<__m256>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const sseA<__m256>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const normA<__m256>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const normA<__m256>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const dummyA<__m256>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME(const dummyA<__m256>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256>::CLASS_NAME() : m_val(_mm256_setzero_ps()){} \
	inline CLASS_NAME<__m256>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_M256D_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const __m256d&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const __m256d& val){ m_val = val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const fastA<__m256d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const fastA<__m256d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const fastestA<__m256d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const fastestA<__m256d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const semiA<__m256d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const semiA<__m256d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const sseA<__m256d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const sseA<__m256d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const normA<__m256d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const normA<__m256d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const dummyA<__m256d>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>& CLASS_NAME<__m256d>::operator=(const dummyA<__m256d>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const __m256d&& val) : m_val(val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const __m256d& val) : m_val(val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const fastA<__m256d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const fastA<__m256d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const fastestA<__m256d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const fastestA<__m256d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const semiA<__m256d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const semiA<__m256d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const sseA<__m256d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const sseA<__m256d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const normA<__m256d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const normA<__m256d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const dummyA<__m256d>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME(const dummyA<__m256d>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<__m256d>::CLASS_NAME() : m_val(_mm256_setzero_pd()){} \
	inline CLASS_NAME<__m256d>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_FLOAT3_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const float (&&val)[3]){ m_val = _mm_loadu_ps(val); return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const float (&val)[3]){ m_val = _mm_loadu_ps(val); return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const __m128&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const __m128& val){ m_val = val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const fastA<float[3]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const fastA<float[3]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const fastestA<float[3]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const fastestA<float[3]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const semiA<float[3]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const semiA<float[3]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const sseA<float[3]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const sseA<float[3]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const normA<float[3]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const normA<float[3]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const dummyA<float[3]>&& val){ m_val = _mm_loadu_ps(val.m_val); return *this; } \
	inline CLASS_NAME<float[3]>& CLASS_NAME<float[3]>::operator=(const dummyA<float[3]>& val){ m_val = _mm_loadu_ps(val.m_val); return *this; } \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const float (&&val)[3]) : m_val(_mm_loadu_ps(val)){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const float (&val)[3]) : m_val(_mm_loadu_ps(val)){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const __m128&& val) : m_val(val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const __m128& val) : m_val(val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const fastA<float[3]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const fastA<float[3]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const fastestA<float[3]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const fastestA<float[3]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const semiA<float[3]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const semiA<float[3]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const sseA<float[3]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const sseA<float[3]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const normA<float[3]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const normA<float[3]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const dummyA<float[3]>&& val) : m_val(_mm_loadu_ps(val.m_val)){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME(const dummyA<float[3]>& val) : m_val(_mm_loadu_ps(val.m_val)){} \
	inline CLASS_NAME<float[3]>::CLASS_NAME() : m_val(_mm_setzero_ps()){} \
	inline CLASS_NAME<float[3]>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_FLOAT4_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const float (&&val)[4]){ m_val = _mm_loadu_ps(val); return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const float (&val)[4]){ m_val = _mm_loadu_ps(val); return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const __m128&& val){ m_val = val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const __m128& val){ m_val = val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const fastA<float[4]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const fastA<float[4]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const fastestA<float[4]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const fastestA<float[4]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const semiA<float[4]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const semiA<float[4]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const sseA<float[4]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const sseA<float[4]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const normA<float[4]>&& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const normA<float[4]>& val){ m_val = val.m_val; return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const dummyA<float[4]>&& val){ m_val = _mm_loadu_ps(val.m_val); return *this; } \
	inline CLASS_NAME<float[4]>& CLASS_NAME<float[4]>::operator=(const dummyA<float[4]>& val){ m_val = _mm_loadu_ps(val.m_val); return *this; } \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const float (&&val)[4]) : m_val(_mm_loadu_ps(val)){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const float (&val)[4]) : m_val(_mm_loadu_ps(val)){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const __m128&& val) : m_val(val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const __m128& val) : m_val(val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const fastA<float[4]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const fastA<float[4]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const fastestA<float[4]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const fastestA<float[4]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const semiA<float[4]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const semiA<float[4]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const sseA<float[4]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const sseA<float[4]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const normA<float[4]>&& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const normA<float[4]>& val) : m_val(val.m_val){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const dummyA<float[4]>&& val) : m_val(_mm_loadu_ps(val.m_val)){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME(const dummyA<float[4]>& val) : m_val(_mm_loadu_ps(val.m_val)){} \
	inline CLASS_NAME<float[4]>::CLASS_NAME() : m_val(_mm_setzero_ps()){} \
	inline CLASS_NAME<float[4]>::~CLASS_NAME(){} \

#define GASHA_FAST_ARITH_CLASS_SSE_FLOAT4X4_INSTANCING(CLASS_NAME) \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const float(&&val)[4][4]){ for (int i = 0; i < 4; ++ i) m_val[i] = _mm_loadu_ps(val[i]); return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const float(&val)[4][4]){ for (int i = 0; i < 4; ++i) m_val[i] = _mm_loadu_ps(val[i]); return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const __m128(&&val)[4]){ for (int i = 0; i < 4; ++i) m_val[i] = val[i]; return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const __m128(&val)[4]){ for (int i = 0; i < 4; ++i) m_val[i] = val[i]; return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const fastA<float[4][4]>&& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const fastA<float[4][4]>& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const fastestA<float[4][4]>&& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const fastestA<float[4][4]>& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const semiA<float[4][4]>&& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const semiA<float[4][4]>& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const sseA<float[4][4]>&& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const sseA<float[4][4]>& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const normA<float[4][4]>&& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const normA<float[4][4]>& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const dummyA<float[4][4]>&& val){ for (int i = 0; i < 4; ++i) m_val[i] = _mm_loadu_ps(val.m_val[i]); return *this; } \
	inline CLASS_NAME<float[4][4]>& CLASS_NAME<float[4][4]>::operator=(const dummyA<float[4][4]>& val){ for (int i = 0; i < 4; ++i) m_val[i] = _mm_loadu_ps(val.m_val[i]); return *this; } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const float(&&val)[4][4]){ for (int i = 0; i < 4; ++i) m_val[i] = _mm_loadu_ps(val[i]); } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const float(&val)[4][4]){ for (int i = 0; i < 4; ++i) m_val[i] = _mm_loadu_ps(val[i]); } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const __m128(&&val)[4]){ for (int i = 0; i < 4; ++i) m_val[i] = val[i]; } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const __m128(&val)[4]){ for (int i = 0; i < 4; ++i) m_val[i] = val[i]; } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const fastA<float[4][4]>&& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const fastA<float[4][4]>& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const fastestA<float[4][4]>&& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const fastestA<float[4][4]>& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const semiA<float[4][4]>&& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const semiA<float[4][4]>& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const sseA<float[4][4]>&& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const sseA<float[4][4]>& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const normA<float[4][4]>&& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const normA<float[4][4]>& val){ for (int i = 0; i < 4; ++i) m_val[i] = val.m_val[i]; } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const dummyA<float[4][4]>&& val){ for (int i = 0; i < 4; ++i) m_val[i] = _mm_loadu_ps(val.m_val[i]); } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(const dummyA<float[4][4]>& val){ for (int i = 0; i < 4; ++i) m_val[i] = _mm_loadu_ps(val.m_val[i]); } \
	inline CLASS_NAME<float[4][4]>::CLASS_NAME(){ for (int i = 0; i < 4; ++i) m_val[i] = _mm_setzero_ps(); } \
	inline CLASS_NAME<float[4][4]>::~CLASS_NAME(){} \

#ifdef GASHA_FAST_ARITH_USE_SSE
#define GASHA_FAST_ARITH_CLASS_SSE_INSTANCING(CLASS_NAME) \
	GASHA_FAST_ARITH_CLASS_SSE_FLOAT_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_M128_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_FLOAT3_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_FLOAT4_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_FLOAT4X4_INSTANCING(CLASS_NAME)
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
	GASHA_FAST_ARITH_CLASS_ARRAY_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_SSE2_INSTANCING(CLASS_NAME); \
	GASHA_FAST_ARITH_CLASS_AVX_INSTANCING(CLASS_NAME);

//----------------------------------------
//高速演算クラス
GASHA_FAST_ARITH_CLASS_SET_INSTANCING(fastA);
//----------------------------------------
//最高速演算クラス
GASHA_FAST_ARITH_CLASS_SET_INSTANCING(fastestA);
//----------------------------------------
//準高速演算クラス
GASHA_FAST_ARITH_CLASS_SET_INSTANCING(semiA);
//----------------------------------------
//SSE高速演算クラス
GASHA_FAST_ARITH_CLASS_SET_INSTANCING(sseA);
//----------------------------------------
//通常演算クラス
GASHA_FAST_ARITH_CLASS_SET_INSTANCING(normA);

//----------------------------------------
//高速演算：特殊化
#ifdef GASHA_FAST_ARITH_USE_SSE
template<> inline fastA<float> operator+(const fastA<float>&& lvalue, const fastA<float>&& rvalue)
{
	return _mm_add_ss(lvalue, rvalue);
}
template<> inline fastA<__m128> operator+(const fastA<__m128>&& lvalue, const fastA<__m128>&& rvalue)
{
	return _mm_add_ps(lvalue, rvalue);
}

template<> inline fastA<float> operator-(const fastA<float>&& lvalue, const fastA<float>&& rvalue)
{
	return _mm_sub_ss(lvalue, rvalue);
}
template<> inline fastA<__m128> operator-(const fastA<__m128>&& lvalue, const fastA<__m128>&& rvalue)
{
	return _mm_sub_ps(lvalue, rvalue);
}

template<> inline fastA<float> operator*(const fastA<float>&& lvalue, const fastA<float>&& rvalue)
{
	return _mm_mul_ss(lvalue, rvalue);
}
template<> inline fastA<__m128> operator*(const fastA<__m128>&& lvalue, const fastA<__m128>&& rvalue)
{
	return _mm_mul_ps(lvalue, rvalue);
}

template<> inline fastA<float> operator/(const fastA<float>&& lvalue, const fastA<float>&& rvalue)
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
template<> inline fastA<__m128> operator/(const fastA<__m128>&& lvalue, const fastA<__m128>&& rvalue)
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

template<> inline fastestA<float> operator+(const fastestA<float>&& lvalue, const fastestA<float>&& rvalue)
{
	return _mm_add_ss(lvalue, rvalue);
}
template<> inline fastestA<__m128> operator+(const fastestA<__m128>&& lvalue, const fastestA<__m128>&& rvalue)
{
	return _mm_add_ps(lvalue, rvalue);
}

template<> inline fastestA<float> operator-(const fastestA<float>&& lvalue, const fastestA<float>&& rvalue)
{
	return _mm_sub_ss(lvalue, rvalue);
}
template<> inline fastestA<__m128> operator-(const fastestA<__m128>&& lvalue, const fastestA<__m128>&& rvalue)
{
	return _mm_sub_ps(lvalue, rvalue);
}

template<> inline fastestA<float> operator*(const fastestA<float>&& lvalue, const fastestA<float>&& rvalue)
{
	return _mm_mul_ss(lvalue, rvalue);
}
template<> inline fastestA<__m128> operator*(const fastestA<__m128>&& lvalue, const fastestA<__m128>&& rvalue)
{
	return _mm_mul_ps(lvalue, rvalue);
}

template<> inline fastestA<float> operator/(const fastestA<float>&& lvalue, const fastestA<float>&& rvalue)
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
template<> inline fastestA<__m128> operator/(const fastestA<__m128>&& lvalue, const fastestA<__m128>&& rvalue)
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

template<> inline semiA<float> operator+(const semiA<float>&& lvalue, const semiA<float>&& rvalue)
{
	return _mm_add_ss(lvalue, rvalue);
}
template<> inline semiA<__m128> operator+(const semiA<__m128>&& lvalue, const semiA<__m128>&& rvalue)
{
	return _mm_add_ps(lvalue, rvalue);
}

template<> inline semiA<float> operator-(const semiA<float>&& lvalue, const semiA<float>&& rvalue)
{
	return _mm_sub_ss(lvalue, rvalue);
}
template<> inline semiA<__m128> operator-(const semiA<__m128>&& lvalue, const semiA<__m128>&& rvalue)
{
	return _mm_sub_ps(lvalue, rvalue);
}

template<> inline semiA<float> operator*(const semiA<float>&& lvalue, const semiA<float>&& rvalue)
{
	return _mm_mul_ss(lvalue, rvalue);
}
template<> inline semiA<__m128> operator*(const semiA<__m128>&& lvalue, const semiA<__m128>&& rvalue)
{
	return _mm_mul_ps(lvalue, rvalue);
}

template<> inline semiA<float> operator/(const semiA<float>&& lvalue, const semiA<float>&& rvalue)
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
template<> inline semiA<__m128> operator/(const semiA<__m128>&& lvalue, const semiA<__m128>&& rvalue)
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

template<> inline sseA<float> operator+(const sseA<float>&& lvalue, const sseA<float>&& rvalue)
{
	return _mm_add_ss(lvalue, rvalue);
}
template<> inline sseA<__m128> operator+(const sseA<__m128>&& lvalue, const sseA<__m128>&& rvalue)
{
	return _mm_add_ps(lvalue, rvalue);
}

template<> inline sseA<float> operator-(const sseA<float>&& lvalue, const sseA<float>&& rvalue)
{
	return _mm_sub_ss(lvalue, rvalue);
}
template<> inline sseA<__m128> operator-(const sseA<__m128>&& lvalue, const sseA<__m128>&& rvalue)
{
	return _mm_sub_ps(lvalue, rvalue);
}

template<> inline sseA<float> operator*(const sseA<float>&& lvalue, const sseA<float>&& rvalue)
{
	return _mm_mul_ss(lvalue, rvalue);
}
template<> inline sseA<__m128> operator*(const sseA<__m128>&& lvalue, const sseA<__m128>&& rvalue)
{
	return _mm_mul_ps(lvalue, rvalue);
}

template<> inline sseA<float> operator/(const sseA<float>&& lvalue, const sseA<float>&& rvalue)
{
	return _mm_div_ss(lvalue, rvalue);
}
template<> inline sseA<__m128> operator/(const sseA<__m128>&& lvalue, const sseA<__m128>&& rvalue)
{
	return _mm_div_ps(lvalue, rvalue);
}

template<> inline normA<__m128> operator+(const normA<__m128>&& lvalue, const normA<__m128>&& rvalue)
{
	return _mm_add_ps(lvalue, rvalue);
}

template<> inline normA<__m128> operator-(const normA<__m128>&& lvalue, const normA<__m128>&& rvalue)
{
	return _mm_sub_ps(lvalue, rvalue);
}

template<> inline normA<__m128> operator*(const normA<__m128>&& lvalue, const normA<__m128>&& rvalue)
{
	return _mm_mul_ps(lvalue, rvalue);
}

template<> inline normA<__m128> operator/(const normA<__m128>&& lvalue, const normA<__m128>&& rvalue)
{
	return _mm_div_ps(lvalue, rvalue);
}

template<> inline __m128 operator+(const dummyA<__m128>&& lvalue, const dummyA<__m128>&& rvalue)
{
	return _mm_add_ps(lvalue, rvalue);
}

template<> inline __m128 operator-(const dummyA<__m128>&& lvalue, const dummyA<__m128>&& rvalue)
{
	return _mm_sub_ps(lvalue, rvalue);
}

template<> inline __m128 operator*(const dummyA<__m128>&& lvalue, const dummyA<__m128>&& rvalue)
{
	return _mm_mul_ps(lvalue, rvalue);
}

template<> inline __m128 operator/(const dummyA<__m128>&& lvalue, const dummyA<__m128>&& rvalue)
{
	return _mm_div_ps(lvalue, rvalue);
}
#endif//GASHA_FAST_ARITH_USE_SSE
#ifdef GASHA_FAST_ARITH_USE_SSE2
template<> inline fastA<double> operator+(const fastA<double>&& lvalue, const fastA<double>&& rvalue)
{
	return _mm_add_sd(lvalue, rvalue);
}
template<> inline fastA<__m128d> operator+(const fastA<__m128d>&& lvalue, const fastA<__m128d>&& rvalue)
{
	return _mm_add_pd(lvalue, rvalue);
}

template<> inline fastA<double> operator-(const fastA<double>&& lvalue, const fastA<double>&& rvalue)
{
	return _mm_sub_sd(lvalue, rvalue);
}
template<> inline fastA<__m128d> operator-(const fastA<__m128d>&& lvalue, const fastA<__m128d>&& rvalue)
{
	return _mm_sub_pd(lvalue, rvalue);
}

template<> inline fastA<double> operator*(const fastA<double>&& lvalue, const fastA<double>&& rvalue)
{
	return _mm_mul_sd(lvalue, rvalue);
}
template<> inline fastA<__m128d> operator*(const fastA<__m128d>&& lvalue, const fastA<__m128d>&& rvalue)
{
	return _mm_mul_pd(lvalue, rvalue);
}

template<> inline fastA<double> operator/(const fastA<double>&& lvalue, const fastA<double>&& rvalue)
{
	return _mm_div_sd(lvalue, rvalue);
}
template<> inline fastA<__m128d> operator/(const fastA<__m128d>&& lvalue, const fastA<__m128d>&& rvalue)
{
	return _mm_div_pd(lvalue, rvalue);
}

template<> inline fastestA<double> operator+(const fastestA<double>&& lvalue, const fastestA<double>&& rvalue)
{
	return _mm_add_sd(lvalue, rvalue);
}
template<> inline fastestA<__m128d> operator+(const fastestA<__m128d>&& lvalue, const fastestA<__m128d>&& rvalue)
{
	return _mm_add_pd(lvalue, rvalue);
}

template<> inline fastestA<double> operator-(const fastestA<double>&& lvalue, const fastestA<double>&& rvalue)
{
	return _mm_sub_sd(lvalue, rvalue);
}
template<> inline fastestA<__m128d> operator-(const fastestA<__m128d>&& lvalue, const fastestA<__m128d>&& rvalue)
{
	return _mm_sub_sd(lvalue, rvalue);
}

template<> inline fastestA<double> operator*(const fastestA<double>&& lvalue, const fastestA<double>&& rvalue)
{
	return _mm_mul_sd(lvalue, rvalue);
}
template<> inline fastestA<__m128d> operator*(const fastestA<__m128d>&& lvalue, const fastestA<__m128d>&& rvalue)
{
	return _mm_mul_pd(lvalue, rvalue);
}

template<> inline fastestA<double> operator/(const fastestA<double>&& lvalue, const fastestA<double>&& rvalue)
{
	return _mm_div_sd(lvalue, rvalue);
}
template<> inline fastestA<__m128d> operator/(const fastestA<__m128d>&& lvalue, const fastestA<__m128d>&& rvalue)
{
	return _mm_div_pd(lvalue, rvalue);
}

template<> inline semiA<double> operator+(const semiA<double>&& lvalue, const semiA<double>&& rvalue)
{
	return _mm_add_sd(lvalue, rvalue);
}
template<> inline semiA<__m128d> operator+(const semiA<__m128d>&& lvalue, const semiA<__m128d>&& rvalue)
{
	return _mm_add_pd(lvalue, rvalue);
}

template<> inline semiA<double> operator-(const semiA<double>&& lvalue, const semiA<double>&& rvalue)
{
	return _mm_sub_sd(lvalue, rvalue);
}
template<> inline semiA<__m128d> operator-(const semiA<__m128d>&& lvalue, const semiA<__m128d>&& rvalue)
{
	return _mm_sub_pd(lvalue, rvalue);
}

template<> inline semiA<double> operator*(const semiA<double>&& lvalue, const semiA<double>&& rvalue)
{
	return _mm_mul_sd(lvalue, rvalue);
}
template<> inline semiA<__m128d> operator*(const semiA<__m128d>&& lvalue, const semiA<__m128d>&& rvalue)
{
	return _mm_mul_pd(lvalue, rvalue);
}

template<> inline semiA<double> operator/(const semiA<double>&& lvalue, const semiA<double>&& rvalue)
{
	return _mm_div_sd(lvalue, rvalue);
}
template<> inline semiA<__m128d> operator/(const semiA<__m128d>&& lvalue, const semiA<__m128d>&& rvalue)
{
	return _mm_div_pd(lvalue, rvalue);
}

template<> inline sseA<double> operator+(const sseA<double>&& lvalue, const sseA<double>&& rvalue)
{
	return _mm_add_sd(lvalue, rvalue);
}
template<> inline sseA<__m128d> operator+(const sseA<__m128d>&& lvalue, const sseA<__m128d>&& rvalue)
{
	return _mm_add_pd(lvalue, rvalue);
}

template<> inline sseA<double> operator-(const sseA<double>&& lvalue, const sseA<double>&& rvalue)
{
	return _mm_sub_sd(lvalue, rvalue);
}
template<> inline sseA<__m128d> operator-(const sseA<__m128d>&& lvalue, const sseA<__m128d>&& rvalue)
{
	return _mm_sub_pd(lvalue, rvalue);
}

template<> inline sseA<double> operator*(const sseA<double>&& lvalue, const sseA<double>&& rvalue)
{
	return _mm_mul_sd(lvalue, rvalue);
}
template<> inline sseA<__m128d> operator*(const sseA<__m128d>&& lvalue, const sseA<__m128d>&& rvalue)
{
	return _mm_mul_pd(lvalue, rvalue);
}

template<> inline sseA<double> operator/(const sseA<double>&& lvalue, const sseA<double>&& rvalue)
{
	return _mm_div_sd(lvalue, rvalue);
}
template<> inline sseA<__m128d> operator/(const sseA<__m128d>&& lvalue, const sseA<__m128d>&& rvalue)
{
	return _mm_div_pd(lvalue, rvalue);
}

template<> inline normA<__m128d> operator+(const normA<__m128d>&& lvalue, const normA<__m128d>&& rvalue)
{
	return _mm_add_pd(lvalue, rvalue);
}

template<> inline normA<__m128d> operator-(const normA<__m128d>&& lvalue, const normA<__m128d>&& rvalue)
{
	return _mm_sub_pd(lvalue, rvalue);
}

template<> inline normA<__m128d> operator*(const normA<__m128d>&& lvalue, const normA<__m128d>&& rvalue)
{
	return _mm_mul_pd(lvalue, rvalue);
}

template<> inline normA<__m128d> operator/(const normA<__m128d>&& lvalue, const normA<__m128d>&& rvalue)
{
	return _mm_div_pd(lvalue, rvalue);
}

template<> inline __m128d operator+(const dummyA<__m128d>&& lvalue, const dummyA<__m128d>&& rvalue)
{
	return _mm_add_pd(lvalue, rvalue);
}

template<> inline __m128d operator-(const dummyA<__m128d>&& lvalue, const dummyA<__m128d>&& rvalue)
{
	return _mm_sub_pd(lvalue, rvalue);
}

template<> inline __m128d operator*(const dummyA<__m128d>&& lvalue, const dummyA<__m128d>&& rvalue)
{
	return _mm_mul_pd(lvalue, rvalue);
}

template<> inline __m128d operator/(const dummyA<__m128d>&& lvalue, const dummyA<__m128d>&& rvalue)
{
	return _mm_div_pd(lvalue, rvalue);
}
#endif//GASHA_FAST_ARITH_USE_SSE2
#ifdef GASHA_FAST_ARITH_USE_AVX
template<> inline fastA<__m256> operator+(const fastA<__m256>&& lvalue, const fastA<__m256>&& rvalue)
{
	return _mm256_add_ps(lvalue, rvalue);
}
template<> inline fastA<__m256d> operator+(const fastA<__m256d>&& lvalue, const fastA<__m256d>&& rvalue)
{
	return _mm256_add_pd(lvalue, rvalue);
}

template<> inline fastA<__m256> operator-(const fastA<__m256>&& lvalue, const fastA<__m256>&& rvalue)
{
	return _mm256_sub_ps(lvalue, rvalue);
}
template<> inline fastA<__m256d> operator-(const fastA<__m256d>&& lvalue, const fastA<__m256d>&& rvalue)
{
	return _mm256_sub_pd(lvalue, rvalue);
}

template<> inline fastA<__m256> operator*(const fastA<__m256>&& lvalue, const fastA<__m256>&& rvalue)
{
	return _mm256_mul_ps(lvalue, rvalue);
}
template<> inline fastA<__m256d> operator*(const fastA<__m256d>&& lvalue, const fastA<__m256d>&& rvalue)
{
	return _mm256_mul_pd(lvalue, rvalue);
}

template<> inline fastA<__m256> operator/(const fastA<__m256>&& lvalue, const fastA<__m256>&& rvalue)
{
	return _mm256_div_ps(lvalue, rvalue);
}
template<> inline fastA<__m256d> operator/(const fastA<__m256d>&& lvalue, const fastA<__m256d>&& rvalue)
{
	return _mm256_div_pd(lvalue, rvalue);
}

template<> inline fastestA<__m256> operator+(const fastestA<__m256>&& lvalue, const fastestA<__m256>&& rvalue)
{
	return _mm256_add_ps(lvalue, rvalue);
}
template<> inline fastestA<__m256d> operator+(const fastestA<__m256d>&& lvalue, const fastestA<__m256d>&& rvalue)
{
	return _mm256_add_pd(lvalue, rvalue);
}

template<> inline fastestA<__m256> operator-(const fastestA<__m256>&& lvalue, const fastestA<__m256>&& rvalue)
{
	return _mm256_sub_ps(lvalue, rvalue);
}
template<> inline fastestA<__m256d> operator-(const fastestA<__m256d>&& lvalue, const fastestA<__m256d>&& rvalue)
{
	return _mm256_sub_pd(lvalue, rvalue);
}

template<> inline fastestA<__m256> operator*(const fastestA<__m256>&& lvalue, const fastestA<__m256>&& rvalue)
{
	return _mm256_mul_ps(lvalue, rvalue);
}
template<> inline fastestA<__m256d> operator*(const fastestA<__m256d>&& lvalue, const fastestA<__m256d>&& rvalue)
{
	return _mm256_mul_pd(lvalue, rvalue);
}

template<> inline fastestA<__m256> operator/(const fastestA<__m256>&& lvalue, const fastestA<__m256>&& rvalue)
{
	return _mm256_div_ps(lvalue, rvalue);
}
template<> inline fastestA<__m256d> operator/(const fastestA<__m256d>&& lvalue, const fastestA<__m256d>&& rvalue)
{
	return _mm256_div_pd(lvalue, rvalue);
}

template<> inline semiA<__m256> operator+(const semiA<__m256>&& lvalue, const semiA<__m256>&& rvalue)
{
	return _mm256_add_ps(lvalue, rvalue);
}
template<> inline semiA<__m256d> operator+(const semiA<__m256d>&& lvalue, const semiA<__m256d>&& rvalue)
{
	return _mm256_add_pd(lvalue, rvalue);
}

template<> inline semiA<__m256> operator-(const semiA<__m256>&& lvalue, const semiA<__m256>&& rvalue)
{
	return _mm256_sub_ps(lvalue, rvalue);
}
template<> inline semiA<__m256d> operator-(const semiA<__m256d>&& lvalue, const semiA<__m256d>&& rvalue)
{
	return _mm256_sub_pd(lvalue, rvalue);
}

template<> inline semiA<__m256> operator*(const semiA<__m256>&& lvalue, const semiA<__m256>&& rvalue)
{
	return _mm256_mul_ps(lvalue, rvalue);
}
template<> inline semiA<__m256d> operator*(const semiA<__m256d>&& lvalue, const semiA<__m256d>&& rvalue)
{
	return _mm256_mul_pd(lvalue, rvalue);
}

template<> inline semiA<__m256> operator/(const semiA<__m256>&& lvalue, const semiA<__m256>&& rvalue)
{
	return _mm256_div_ps(lvalue, rvalue);
}
template<> inline semiA<__m256d> operator/(const semiA<__m256d>&& lvalue, const semiA<__m256d>&& rvalue)
{
	return _mm256_div_pd(lvalue, rvalue);
}

template<> inline sseA<__m256> operator+(const sseA<__m256>&& lvalue, const sseA<__m256>&& rvalue)
{
	return _mm256_add_ps(lvalue, rvalue);
}
template<> inline sseA<__m256d> operator+(const sseA<__m256d>&& lvalue, const sseA<__m256d>&& rvalue)
{
	return _mm256_add_pd(lvalue, rvalue);
}

template<> inline sseA<__m256> operator-(const sseA<__m256>&& lvalue, const sseA<__m256>&& rvalue)
{
	return _mm256_sub_ps(lvalue, rvalue);
}
template<> inline sseA<__m256d> operator-(const sseA<__m256d>&& lvalue, const sseA<__m256d>&& rvalue)
{
	return _mm256_sub_pd(lvalue, rvalue);
}

template<> inline sseA<__m256> operator*(const sseA<__m256>&& lvalue, const sseA<__m256>&& rvalue)
{
	return _mm256_mul_ps(lvalue, rvalue);
}
template<> inline sseA<__m256d> operator*(const sseA<__m256d>&& lvalue, const sseA<__m256d>&& rvalue)
{
	return _mm256_mul_pd(lvalue, rvalue);
}

template<> inline sseA<__m256> operator/(const sseA<__m256>&& lvalue, const sseA<__m256>&& rvalue)
{
	return _mm256_div_ps(lvalue, rvalue);
}
template<> inline sseA<__m256d> operator/(const sseA<__m256d>&& lvalue, const sseA<__m256d>&& rvalue)
{
	return _mm256_div_pd(lvalue, rvalue);
}

template<> inline normA<__m256> operator+(const normA<__m256>&& lvalue, const normA<__m256>&& rvalue)
{
	return _mm256_add_ps(lvalue, rvalue);
}
template<> inline normA<__m256d> operator+(const normA<__m256d>&& lvalue, const normA<__m256d>&& rvalue)
{
	return _mm256_add_pd(lvalue, rvalue);
}

template<> inline normA<__m256> operator-(const normA<__m256>&& lvalue, const normA<__m256>&& rvalue)
{
	return _mm256_sub_ps(lvalue, rvalue);
}
template<> inline normA<__m256d> operator-(const normA<__m256d>&& lvalue, const normA<__m256d>&& rvalue)
{
	return _mm256_sub_pd(lvalue, rvalue);
}

template<> inline normA<__m256> operator*(const normA<__m256>&& lvalue, const normA<__m256>&& rvalue)
{
	return _mm256_mul_ps(lvalue, rvalue);
}
template<> inline normA<__m256d> operator*(const normA<__m256d>&& lvalue, const normA<__m256d>&& rvalue)
{
	return _mm256_mul_pd(lvalue, rvalue);
}

template<> inline normA<__m256> operator/(const normA<__m256>&& lvalue, const normA<__m256>&& rvalue)
{
	return _mm256_div_ps(lvalue, rvalue);
}
template<> inline normA<__m256d> operator/(const normA<__m256d>&& lvalue, const normA<__m256d>&& rvalue)
{
	return _mm256_div_pd(lvalue, rvalue);
}

template<> inline __m256 operator+(const dummyA<__m256>&& lvalue, const dummyA<__m256>&& rvalue)
{
	return _mm256_add_ps(lvalue, rvalue);
}
template<> inline __m256d operator+(const dummyA<__m256d>&& lvalue, const dummyA<__m256d>&& rvalue)
{
	return _mm256_add_pd(lvalue, rvalue);
}

template<> inline __m256 operator-(const dummyA<__m256>&& lvalue, const dummyA<__m256>&& rvalue)
{
	return _mm256_sub_ps(lvalue, rvalue);
}
template<> inline __m256d operator-(const dummyA<__m256d>&& lvalue, const dummyA<__m256d>&& rvalue)
{
	return _mm256_sub_pd(lvalue, rvalue);
}

template<> inline __m256 operator*(const dummyA<__m256>&& lvalue, const dummyA<__m256>&& rvalue)
{
	return _mm256_mul_ps(lvalue, rvalue);
}
template<> inline __m256d operator*(const dummyA<__m256d>&& lvalue, const dummyA<__m256d>&& rvalue)
{
	return _mm256_mul_pd(lvalue, rvalue);
}

template<> inline __m256 operator/(const dummyA<__m256>&& lvalue, const dummyA<__m256>&& rvalue)
{
	return _mm256_div_ps(lvalue, rvalue);
}
template<> inline __m256d operator/(const dummyA<__m256d>&& lvalue, const dummyA<__m256d>&& rvalue)
{
	return _mm256_div_pd(lvalue, rvalue);
}
#endif//GASHA_FAST_ARITH_USE_AVX

//----------------------------------------
//高速平方根
template<typename T> inline fastA<T> sqr(const fastA<T>&& value){ return std::sqrt(value); }
template<typename T> inline fastestA<T> sqr(const fastestA<T>&& value){ return std::sqrt(value); }
template<typename T> inline semiA<T> sqr(const semiA<T>&& value){ return std::sqrt(value); }
template<typename T> inline sseA<T> sqr(const sseA<T>&& value){ return std::sqrt(value); }
template<typename T> inline normA<T> sqr(const normA<T>&& value){ return std::sqrt(value); }
template<typename T> inline T sqr(const dummyA<T>&& value){ return std::sqrt(value); }

//----------------------------------------
//高速平方根：特殊化
#ifdef GASHA_FAST_ARITH_USE_SSE
template<> inline fastA<float> sqr(const fastA<float>&& value)
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
template<> inline fastestA<float> sqr(const fastestA<float>&& value)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	const __m128 value_m128 = value;
	const __m128 rcp_sqrt_m128 = _mm_rsqrt_ss(value_m128);//逆数の近似値算出
	return _mm_mul_ss(value_m128, rcp_sqrt_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	return _mm_sqrt_ss(value);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
}
template<> inline semiA<float> sqr(const semiA<float>&& value)
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
template<> inline sseA<float> sqr(const sseA<float>&& value)
{
	return _mm_sqrt_ss(value);
}

template<> inline fastA<__m128> sqr(const fastA<__m128>&& value)
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
template<> inline fastestA<__m128> sqr(const fastestA<__m128>&& value)
{
#ifdef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	const __m128 value_m128 = value;
	const __m128 rcp_sqrt_m128 = _mm_rsqrt_ps(value_m128);//逆数の近似値算出
	return _mm_mul_ps(value_m128, rcp_sqrt_m128);//逆数を乗算
#else//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
	return _mm_sqrt_ps(value);
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION//逆数で平方根算出
}
template<> inline semiA<__m128> sqr(const semiA<__m128>&& value)
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
template<> inline sseA<__m128> sqr(const sseA<__m128>&& value)
{
	return _mm_sqrt_ps(value);
}
template<> inline normA<__m128> sqr(const normA<__m128>&& value)
{
	return _mm_sqrt_ps(value);
}
template<> inline __m128 sqr(const dummyA<__m128>&& value)
{
	return _mm_sqrt_ps(value);
}
#endif//GASHA_FAST_ARITH_USE_SSE
#ifdef GASHA_FAST_ARITH_USE_SSE2
template<> inline fastA<double> sqr(const fastA<double>&& value)
{
	return _mm_sqrt_sd(value, value);
}
template<> inline fastestA<double> sqr(const fastestA<double>&& value)
{
	return _mm_sqrt_sd(value, value);
}
template<> inline semiA<double> sqr(const semiA<double>&& value)
{
	return _mm_sqrt_sd(value, value);
}
template<> inline sseA<double> sqr(const sseA<double>&& value)
{
	return _mm_sqrt_sd(value, value);
}

template<> inline fastA<__m128d> sqr(const fastA<__m128d>&& value)
{
	return _mm_sqrt_pd(value);
}
template<> inline fastestA<__m128d> sqr(const fastestA<__m128d>&& value)
{
	return _mm_sqrt_pd(value);
}
template<> inline semiA<__m128d> sqr(const semiA<__m128d>&& value)
{
	return _mm_sqrt_pd(value);
}
template<> inline sseA<__m128d> sqr(const sseA<__m128d>&& value)
{
	return _mm_sqrt_pd(value);
}
template<> inline normA<__m128d> sqr(const normA<__m128d>&& value)
{
	return _mm_sqrt_pd(value);
}
template<> inline __m128d sqr(const dummyA<__m128d>&& value)
{
	return _mm_sqrt_pd(value);
}
#endif//GASHA_FAST_ARITH_USE_SSE2
#ifdef GASHA_FAST_ARITH_USE_AVX
template<> inline fastA<__m256> sqr(const fastA<__m256>&& value)
{
	return _mm256_sqrt_ps(value);
}
template<> inline fastestA<__m256> sqr(const fastestA<__m256>&& value)
{
	return _mm256_sqrt_ps(value);
}
template<> inline semiA<__m256> sqr(const semiA<__m256>&& value)
{
	return _mm256_sqrt_ps(value);
}
template<> inline sseA<__m256> sqr(const sseA<__m256>&& value)
{
	return _mm256_sqrt_ps(value);
}
template<> inline normA<__m256> sqr(const normA<__m256>&& value)
{
	return _mm256_sqrt_ps(value);
}
template<> inline __m256 sqr(const dummyA<__m256>&& value)
{
	return _mm256_sqrt_ps(value);
}

template<> inline fastA<__m256d> sqr(const fastA<__m256d>&& value)
{
	return _mm256_sqrt_pd(value);
}
template<> inline fastestA<__m256d> sqr(const fastestA<__m256d>&& value)
{
	return _mm256_sqrt_pd(value);
}
template<> inline semiA<__m256d> sqr(const semiA<__m256d>&& value)
{
	return _mm256_sqrt_pd(value);
}
template<> inline sseA<__m256d> sqr(const sseA<__m256d>&& value)
{
	return _mm256_sqrt_pd(value);
}
template<> inline normA<__m256d> sqr(const normA<__m256d>&& value)
{
	return _mm256_sqrt_pd(value);
}
template<> inline __m256d sqr(const dummyA<__m256d>&& value)
{
	return _mm256_sqrt_pd(value);
}
#endif//GASHA_FAST_ARITH_USE_AVX

//--------------------------------------------------------------------------------
//高速ベクトル演算
//--------------------------------------------------------------------------------

#define GASHA_FAST_VECTOR_SET_INSTANCING(CLASS_NAME) \
	template<typename T, std::size_t N> inline T norm(const CLASS_NAME<T[N]>& vec) \
	{ \
		return GASHA_ norm(*vec); \
	} \
	template<typename T, std::size_t N> inline T normSq(const CLASS_NAME<T[N]>& vec) \
	{ \
		return GASHA_ normSq(*vec); \
	} \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> merge(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2) \
	{ \
		T result[N] = { 0 }; \
		GASHA_ merge(result, *vec1, *vec2); \
		return CLASS_NAME<T[N]>(result); \
	} \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> difference(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2) \
	{ \
		T result[N] = { 0 }; \
		GASHA_ difference(result, *vec1, *vec2); \
		return CLASS_NAME<T[N]>(result); \
	} \
	template<typename T, std::size_t N> inline T length(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2) \
	{ \
		return GASHA_ length(*vec1, *vec2); \
	} \
	template<typename T, std::size_t N> inline T lengthSq(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2) \
	{ \
		return GASHA_ lengthSq(*vec1, *vec2); \
	} \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> normalize(const CLASS_NAME<T[N]>& vec) \
	{ \
		T result[N] = { 0 }; \
		GASHA_ normalize(result, *vec); \
		return CLASS_NAME<T[N]>(result); \
	} \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> mul(const CLASS_NAME<T[N]>& vec, const T scalar) \
	{ \
		T result[N] = { 0 }; \
		GASHA_ mul(result, *vec, scalar); \
		return CLASS_NAME<T[N]>(result); \
	} \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> forward(const CLASS_NAME<T[N]>& vec, const T scalar) \
	{ \
		T result[N] = { 0 }; \
		GASHA_ forward(result, *vec, scalar); \
		return CLASS_NAME<T[N]>(result); \
	} \
	template<typename T, std::size_t N> inline T dot(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2) \
	{ \
		return GASHA_ dot(*vec1, *vec2); \
	} \
	template<typename T, std::size_t N> inline T normalizedDot(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2) \
	{ \
		return GASHA_ normalizedDot(*vec1, *vec2); \
	} \
	template<typename T, std::size_t N> inline CLASS_NAME<T[N]> cross(const CLASS_NAME<T[N]>& vec1, const CLASS_NAME<T[N]>& vec2) \
	{ \
		T result[N] = { 0 }; \
		GASHA_ cross(result, *vec1, *vec2); \
		return CLASS_NAME<T[N]>(result); \
	}

GASHA_FAST_VECTOR_SET_INSTANCING(fastA);
GASHA_FAST_VECTOR_SET_INSTANCING(fastestA);
GASHA_FAST_VECTOR_SET_INSTANCING(semiA);
GASHA_FAST_VECTOR_SET_INSTANCING(sseA);
GASHA_FAST_VECTOR_SET_INSTANCING(normA);

template<typename T, std::size_t N> inline T norm(const dummyA<T[N]>& vec)
{
	return GASHA_ norm(*vec);
}
template<typename T, std::size_t N> inline T normSq(const dummyA<T[N]>& vec)
{
	return GASHA_ normSq(*vec);
}
template<typename T, std::size_t N> inline void merge(T (&result)[N], const dummyA<T[N]>& vec1, const dummyA<T[N]>& vec2)
{
	GASHA_ merge(result, *vec1, *vec2);
}
template<typename T, std::size_t N> inline void difference(T(&result)[N], const dummyA<T[N]>& vec1, const dummyA<T[N]>& vec2)
{
	GASHA_ difference(result, *vec1, *vec2);
}
template<typename T, std::size_t N> inline T length(const dummyA<T[N]>& vec1, const dummyA<T[N]>& vec2)
{
	return GASHA_ length(*vec1, *vec2);
}
template<typename T, std::size_t N> inline T lengthSq(const dummyA<T[N]>& vec1, const dummyA<T[N]>& vec2)
{
	return GASHA_ lengthSq(*vec1, *vec2);
}
template<typename T, std::size_t N> inline void normalize(T(&result)[N], const dummyA<T[N]>& vec)
{
	GASHA_ normalize(result, *vec);
}
template<typename T, std::size_t N> inline void mul(T(&result)[N], const dummyA<T[N]>& vec, const T scalar)
{
	GASHA_ mul(result, *vec, scalar);
}
template<typename T, std::size_t N> inline void forward(T(&result)[N], const dummyA<T[N]>& vec, const T scalar)
{
	GASHA_ forward(result, *vec, scalar);
}
template<typename T, std::size_t N> inline T dot(const dummyA<T[N]>& vec1, const dummyA<T[N]>& vec2)
{
	return GASHA_ dot(*vec1, *vec2);
}
template<typename T, std::size_t N> inline T normalizedDot(const dummyA<T[N]>& vec1, const dummyA<T[N]>& vec2)
{
	return GASHA_ normalizedDot(*vec1, *vec2);
}
template<typename T, std::size_t N> inline void cross(T(&result)[N], const dummyA<T[N]>& vec1, const dummyA<T[N]>& vec2)
{
	GASHA_ cross(result, *vec1, *vec2);
}

//----------------------------------------
//高速ベクトル演算：特殊化

#ifdef GASHA_USE_SSE4_1
//※SSE命令に特化し、4次元／4次元ベクトル用特殊化

//--------------------
//SSE命令専用関数

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
//スカラー長の進行
inline __m128 m128_forward(const __m128 vec_m128, const float scalar)
{
	//正規化
	const __m128 norm_m128 = _mm_sqrt_ps(_mm_dp_ps(vec_m128, vec_m128, 0x7f));
	const __m128 normal_m128 = _mm_div_ps(vec_m128, norm_m128);
	//正規ベクトルのスカラー倍
	const __m128 scalar_m128 = _mm_set1_ps(scalar);
	const __m128 forward_m128 = _mm_mul_ps(normal_m128, scalar_m128);
	//加算
	return _mm_add_ps(vec_m128, forward_m128);
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

#ifdef GASHA_FAST_ARITH_USE_SSE4_1

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
	template<> inline CLASS_NAME<T[N]> forward<T, N>(const CLASS_NAME<T[N]>& vec, const T scalar) \
	{ \
		return m128_forward(vec, scalar); \
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

GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(fastA, float, 3);
GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(fastestA, float, 3);
GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(semiA, float, 3);
GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(sseA, float, 3);

GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(fastA, float, 4);
GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(fastestA, float, 4);
GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(semiA, float, 4);
GASHA_FAST_VECTOR_SET_SSE_SPECIALIZATION_INSTANCING(sseA, float, 4);

#endif//GASHA_FAST_ARITH_USE_SSE4_1

#ifdef GASHA_VECTOR_OPERATION_ALWAYS_USE_SSE
//fastArithと無関係に、常にSSE命令を有効にする場合

void mm_store_vec3(float(&dst)[3], const __m128 src)
{
	const float* _src = reinterpret_cast<const float*>(&src);
	dst[0] = _src[0];
	dst[1] = _src[1];
	dst[2] = _src[2];
}

template<> inline float norm<float, 3>(const float(&vec)[3])
{
	const __m128 vec_m128 = _mm_loadu_ps(vec);
	float result;
	_mm_store_ss(&result, m128_norm(vec_m128));
	return result;
}
template<> inline float normSq<float, 3>(const float(&vec)[3])
{
	const __m128 vec_m128 = _mm_loadu_ps(vec);
	float result;
	_mm_store_ss(&result, m128_normSq(vec_m128));
	return result;
}
template<> inline void merge<float, 3>(float(&result)[3], const float(&vec1)[3], const float(&vec2)[3])
{
	const __m128 vec1_m128 = _mm_loadu_ps(vec1);
	const __m128 vec2_m128 = _mm_loadu_ps(vec2);
	mm_store_vec3(result, m128_merge(vec1_m128, vec2_m128));
}
template<> inline void difference<float, 3>(float(&result)[3], const float(&vec1)[3], const float(&vec2)[3])
{
	const __m128 vec1_m128 = _mm_loadu_ps(vec1);
	const __m128 vec2_m128 = _mm_loadu_ps(vec2);
	mm_store_vec3(result, m128_difference(vec1_m128, vec2_m128));
} 
template<> inline float length<float, 3>(const float(&vec1)[3], const float(&vec2)[3])
{
	const __m128 vec1_m128 = _mm_loadu_ps(vec1);
	const __m128 vec2_m128 = _mm_loadu_ps(vec2);
	float result;
	_mm_store_ss(&result, m128_length(vec1_m128, vec2_m128));
	return result;
}
template<> inline float lengthSq<float, 3>(const float(&vec1)[3], const float(&vec2)[3])
{
const __m128 vec1_m128 = _mm_loadu_ps(vec1);
	const __m128 vec2_m128 = _mm_loadu_ps(vec2);
	float result;
	_mm_store_ss(&result, m128_lengthSq(vec1_m128, vec2_m128));
	return result;
}
template<> inline void normalize<float, 3>(float(&result)[3], const float(&vec)[3])
{
	const __m128 vec_m128 = _mm_loadu_ps(vec);
	mm_store_vec3(result, m128_normalize(vec_m128));
}
template<> inline void mul<float, 3>(float(&result)[3], const float(&vec)[3], const float scalar)
{
	const __m128 vec_m128 = _mm_loadu_ps(vec);
	mm_store_vec3(result, m128_mul(vec_m128, scalar));
}
template<> inline void forward<float, 3>(float(&result)[3], const float(&vec)[3], const float scalar)
{
	const __m128 vec_m128 = _mm_loadu_ps(vec);
	mm_store_vec3(result, m128_forward(vec_m128, scalar));
}
template<> inline float dot<float, 3>(const float(&vec1)[3], const float(&vec2)[3])
{
	const __m128 vec1_m128 = _mm_loadu_ps(vec1);
	const __m128 vec2_m128 = _mm_loadu_ps(vec2);
	float result;
	_mm_store_ss(&result, m128_dot(vec1_m128, vec2_m128));
	return result;
}
template<> inline float normalizedDot<float, 3>(const float(&vec1)[3], const float(&vec2)[3])
{
	const __m128 vec1_m128 = _mm_loadu_ps(vec1);
	const __m128 vec2_m128 = _mm_loadu_ps(vec2);
	float result;
	_mm_store_ss(&result, m128_normalizedDot(vec1_m128, vec2_m128));
	return result;
}
template<> inline void cross<float, 3>(float(&result)[3], const float(&vec1)[3], const float(&vec2)[3])
{
	const __m128 vec1_m128 = _mm_loadu_ps(vec1);
	const __m128 vec2_m128 = _mm_loadu_ps(vec2);
	mm_store_vec3(result, m128_cross(vec1_m128, vec2_m128));
}

template<> inline float norm<float, 4>(const float(&vec)[4])
{
	const __m128 vec_m128 = _mm_loadu_ps(vec);
	float result;
	_mm_store_ss(&result, m128_norm(vec_m128));
	return result;
}
template<> inline float normSq<float, 4>(const float(&vec)[4])
{
	const __m128 vec_m128 = _mm_loadu_ps(vec);
	float result;
	_mm_store_ss(&result, m128_normSq(vec_m128));
	return result;
}
template<> inline void merge<float, 4>(float(&result)[4], const float(&vec1)[4], const float(&vec2)[4])
{
	const __m128 vec1_m128 = _mm_loadu_ps(vec1);
	const __m128 vec2_m128 = _mm_loadu_ps(vec2);
	_mm_storeu_ps(result, m128_merge(vec1_m128, vec2_m128));
}
template<> inline void difference<float, 4>(float(&result)[4], const float(&vec1)[4], const float(&vec2)[4])
{
	const __m128 vec1_m128 = _mm_loadu_ps(vec1);
	const __m128 vec2_m128 = _mm_loadu_ps(vec2);
	_mm_storeu_ps(result, m128_difference(vec1_m128, vec2_m128));
}
template<> inline float length<float, 4>(const float(&vec1)[4], const float(&vec2)[4])
{
	const __m128 vec1_m128 = _mm_loadu_ps(vec1);
	const __m128 vec2_m128 = _mm_loadu_ps(vec2);
	float result;
	_mm_store_ss(&result, m128_length(vec1_m128, vec2_m128));
	return result;
}
template<> inline float lengthSq<float, 4>(const float(&vec1)[4], const float(&vec2)[4])
{
	const __m128 vec1_m128 = _mm_loadu_ps(vec1);
	const __m128 vec2_m128 = _mm_loadu_ps(vec2);
	float result;
	_mm_store_ss(&result, m128_lengthSq(vec1_m128, vec2_m128));
	return result;
}
template<> inline void normalize<float, 4>(float(&result)[4], const float(&vec)[4])
{
	const __m128 vec_m128 = _mm_loadu_ps(vec);
	_mm_storeu_ps(result, m128_normalize(vec_m128));
}
template<> inline void mul<float, 4>(float(&result)[4], const float(&vec)[4], const float scalar)
{
	const __m128 vec_m128 = _mm_loadu_ps(vec);
	_mm_storeu_ps(result, m128_mul(vec_m128, scalar));
}
template<> inline void forward<float, 4>(float(&result)[4], const float(&vec)[4], const float scalar)
{
	const __m128 vec_m128 = _mm_loadu_ps(vec);
	_mm_storeu_ps(result, m128_forward(vec_m128, scalar));
}
template<> inline float dot<float, 4>(const float(&vec1)[4], const float(&vec2)[4])
{
	const __m128 vec1_m128 = _mm_loadu_ps(vec1);
	const __m128 vec2_m128 = _mm_loadu_ps(vec2);
	float result;
	_mm_store_ss(&result, m128_dot(vec1_m128, vec2_m128));
	return result;
}
template<> inline float normalizedDot<float, 4>(const float(&vec1)[4], const float(&vec2)[4])
{
	const __m128 vec1_m128 = _mm_loadu_ps(vec1);
	const __m128 vec2_m128 = _mm_loadu_ps(vec2);
	float result;
	_mm_store_ss(&result, m128_normalizedDot(vec1_m128, vec2_m128));
	return result;
}
template<> inline void cross<float, 4>(float(&result)[4], const float(&vec1)[4], const float(&vec2)[4])
{
	const __m128 vec1_m128 = _mm_loadu_ps(vec1);
	const __m128 vec2_m128 = _mm_loadu_ps(vec2);
	_mm_storeu_ps(result, m128_cross(vec1_m128, vec2_m128));
}

#endif//GASHA_VECTOR_OPERATION_ALWAYS_USE_SSE

#endif//GASHA_USE_SSE4_1

//--------------------------------------------------------------------------------
//テンプレート行列
//--------------------------------------------------------------------------------

#define GASHA_FAST_MATRIX_SET_INSTANCING(CLASS_NAME) \
	template<typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]> add(const CLASS_NAME<T[N][M]>& mat1, const CLASS_NAME<T[N][M]>& mat2) \
	{ \
		T result[N][M]; \
		GASHA_ add(result, *mat1, *mat2); \
		return CLASS_NAME<T[N][M]>(result); \
	} \
	template<typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]> sub(const CLASS_NAME<T[N][M]>& mat1, const CLASS_NAME<T[N][M]>& mat2) \
	{ \
		T result[N][M]; \
		GASHA_ sub(result, *mat1, *mat2); \
		return CLASS_NAME<T[N][M]>(result); \
	} \
	template<typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]> mul(const CLASS_NAME<T[N][M]>& mat, const T scalar) \
	{ \
		T result[N][M]; \
		GASHA_ mul(result, *mat, scalar); \
		return CLASS_NAME<T[N][M]>(result); \
	} \
	template<typename T, std::size_t N, std::size_t M> inline CLASS_NAME<T[N][M]> mul(const CLASS_NAME<T[N][M]>& mat1, const CLASS_NAME<T[N][M]>& mat2) \
	{ \
		T result[N][M]; \
		GASHA_ mul(result, *mat1, *mat2); \
		return CLASS_NAME<T[N][M]>(result); \
	} \

GASHA_FAST_MATRIX_SET_INSTANCING(fastA);
GASHA_FAST_MATRIX_SET_INSTANCING(fastestA);
GASHA_FAST_MATRIX_SET_INSTANCING(semiA);
GASHA_FAST_MATRIX_SET_INSTANCING(sseA);
GASHA_FAST_MATRIX_SET_INSTANCING(normA);

template<typename T, std::size_t N, std::size_t M> inline void add(T(&result)[N][M], const dummyA<T[N][M]>& mat1, const dummyA<T[N][M]>& mat2)
{
	GASHA_ add(result, *mat1, *mat2);
}
template<typename T, std::size_t N, std::size_t M> inline void sub(T(&result)[N][M], const dummyA<T[N][M]>& mat1, const dummyA<T[N][M]>& mat2)
{
	GASHA_ sub(result, *mat1, *mat2);
}
template<typename T, std::size_t N, std::size_t M> inline void mul(T(&result)[N][M], const dummyA<T[N][M]>& mat1, const T scalar)
{
	GASHA_ mul(result, *mat1, scalar);
}
template<typename T, std::size_t N, std::size_t M> inline void mul(T(&result)[N][M], const dummyA<T[N][M]>& mat1, const dummyA<T[N][M]>& mat2)
{
	GASHA_ mul(result, *mat1, *mat2);
}

#ifdef GASHA_USE_SSE
//※SSE命令に特化し、float×4×4行列用特殊化

//--------------------
//SSE命令専用関数

//加算
inline void m128_add(__m128 (&mat_result)[4], const __m128 (&mat1_m128)[4], const __m128 (&mat2_m128)[4])
{
	mat_result[0] = _mm_add_ps(mat1_m128[0], mat2_m128[0]);
	mat_result[1] = _mm_add_ps(mat1_m128[1], mat2_m128[1]);
	mat_result[2] = _mm_add_ps(mat1_m128[2], mat2_m128[2]);
	mat_result[3] = _mm_add_ps(mat1_m128[3], mat2_m128[3]);
}

//減算
inline void m128_sub(__m128 (&mat_result)[4], const __m128 (&mat1_m128)[4], const __m128 (&mat2_m128)[4])
{
	mat_result[0] = _mm_sub_ps(mat1_m128[0], mat2_m128[0]);
	mat_result[1] = _mm_sub_ps(mat1_m128[1], mat2_m128[1]);
	mat_result[2] = _mm_sub_ps(mat1_m128[2], mat2_m128[2]);
	mat_result[3] = _mm_sub_ps(mat1_m128[3], mat2_m128[3]);
}

//スカラー倍
inline void m128_mul(__m128 (&mat_result)[4], const __m128 (&mat_m128)[4], const float scalar)
{
	const __m128 scalar_m128 = _mm_set1_ps(scalar);
	mat_result[0] = _mm_sub_ps(mat_m128[0], scalar_m128);
	mat_result[1] = _mm_sub_ps(mat_m128[1], scalar_m128);
	mat_result[2] = _mm_sub_ps(mat_m128[2], scalar_m128);
	mat_result[3] = _mm_sub_ps(mat_m128[3], scalar_m128);
}

#ifdef GASHA_USE_SSE4_1
//乗算
inline void m128_mul(__m128 (&mat_result)[4], const __m128 (&mat1_m128)[4], const __m128 (&mat2_m128)[4])
{
	//mat2_m128を転置
	const __m128 _mat2t_tmp[4] =
	{
		_mm_unpacklo_ps(mat2_m128[0], mat2_m128[1]),
		_mm_unpacklo_ps(mat2_m128[2], mat2_m128[3]),
		_mm_unpackhi_ps(mat2_m128[0], mat2_m128[1]),
		_mm_unpackhi_ps(mat2_m128[2], mat2_m128[3])
	};
	const __m128 _mat2t[4] =
	{
		_mm_shuffle_ps(_mat2t_tmp[0], _mat2t_tmp[1], 0x44),
		_mm_shuffle_ps(_mat2t_tmp[0], _mat2t_tmp[1], 0xee),
		_mm_shuffle_ps(_mat2t_tmp[2], _mat2t_tmp[3], 0x44),
		_mm_shuffle_ps(_mat2t_tmp[2], _mat2t_tmp[3], 0xee)
	};
	//乗算
	mat_result[0] =
		_mm_shuffle_ps(
			_mm_unpacklo_ps(
				_mm_dp_ps(mat1_m128[0], _mat2t[0], 0xf1),
				_mm_dp_ps(mat1_m128[0], _mat2t[1], 0xf1)),
			_mm_unpacklo_ps(
				_mm_dp_ps(mat1_m128[0], _mat2t[2], 0xf1),
				_mm_dp_ps(mat1_m128[0], _mat2t[3], 0xf1)),
			0x44);
	mat_result[1] =
		_mm_shuffle_ps(
			_mm_unpacklo_ps(
				_mm_dp_ps(mat1_m128[1], _mat2t[0], 0xf1),
				_mm_dp_ps(mat1_m128[1], _mat2t[1], 0xf1)),
			_mm_unpacklo_ps(
				_mm_dp_ps(mat1_m128[1], _mat2t[2], 0xf1),
				_mm_dp_ps(mat1_m128[1], _mat2t[3], 0xf1)),
			0x44);
	mat_result[2] =
		_mm_shuffle_ps(
			_mm_unpacklo_ps(
				_mm_dp_ps(mat1_m128[2], _mat2t[0], 0xf1),
				_mm_dp_ps(mat1_m128[2], _mat2t[1], 0xf1)),
			_mm_unpacklo_ps(
				_mm_dp_ps(mat1_m128[2], _mat2t[2], 0xf1),
				_mm_dp_ps(mat1_m128[2], _mat2t[3], 0xf1)),
			0x44);
	mat_result[3] =
		_mm_shuffle_ps(
			_mm_unpacklo_ps(
				_mm_dp_ps(mat1_m128[3], _mat2t[0], 0xf1),
				_mm_dp_ps(mat1_m128[3], _mat2t[1], 0xf1)),
			_mm_unpacklo_ps(
				_mm_dp_ps(mat1_m128[3], _mat2t[2], 0xf1),
				_mm_dp_ps(mat1_m128[3], _mat2t[3], 0xf1)),
			0x44);
}
#endif//GASHA_USE_SSE4_1


//--------------------
//特殊化

#ifdef GASHA_FAST_ARITH_USE_SSE

#define GASHA_FAST_MATRIX_SET_SSE_SPECIALIZATION_INSTANCING(CLASS_NAME, T, N, M) \
	template<> inline CLASS_NAME<T[N][M]> add<T, N, M>(const CLASS_NAME<T[N][M]>& mat1, const CLASS_NAME<T[N][M]>& mat2) \
	{ \
		CLASS_NAME<T[N][M]> result; \
		m128_add(result.m_val, mat1.m_val, mat2.m_val); \
		return result; \
	} \
	template<> inline CLASS_NAME<T[N][M]> sub<T, N, M>(const CLASS_NAME<T[N][M]>& mat1, const CLASS_NAME<T[N][M]>& mat2) \
	{ \
		CLASS_NAME<T[N][M]> result; \
		m128_sub(result.m_val, mat1.m_val, mat2.m_val); \
		return result; \
	} \
	template<> inline CLASS_NAME<T[N][M]> mul<T, N, M>(const CLASS_NAME<T[N][M]>& mat, const T scalar) \
	{ \
		CLASS_NAME<T[N][M]> result; \
		m128_mul(result.m_val, mat.m_val, scalar); \
		return result; \
	} \

GASHA_FAST_MATRIX_SET_SSE_SPECIALIZATION_INSTANCING(fastA, float, 4, 4);
GASHA_FAST_MATRIX_SET_SSE_SPECIALIZATION_INSTANCING(fastestA, float, 4, 4);
GASHA_FAST_MATRIX_SET_SSE_SPECIALIZATION_INSTANCING(semiA, float, 4, 4);
GASHA_FAST_MATRIX_SET_SSE_SPECIALIZATION_INSTANCING(sseA, float, 4, 4);

#endif//GASHA_FAST_ARITH_USE_SSE

#ifdef GASHA_FAST_ARITH_USE_SSE4_1

#define GASHA_FAST_MATRIX_SET_SSE_SPECIALIZATION4_1_INSTANCING(CLASS_NAME, T, N, M) \
	template<> inline CLASS_NAME<T[N][M]> mul<T, N, M>(const CLASS_NAME<T[N][M]>& mat1, const CLASS_NAME<T[N][M]>& mat2) \
	{ \
		CLASS_NAME<T[N][M]> result; \
		m128_mul(result.m_val, mat1.m_val, mat2.m_val); \
		return result; \
	} \

GASHA_FAST_MATRIX_SET_SSE_SPECIALIZATION4_1_INSTANCING(fastA, float, 4, 4);
GASHA_FAST_MATRIX_SET_SSE_SPECIALIZATION4_1_INSTANCING(fastestA, float, 4, 4);
GASHA_FAST_MATRIX_SET_SSE_SPECIALIZATION4_1_INSTANCING(semiA, float, 4, 4);
GASHA_FAST_MATRIX_SET_SSE_SPECIALIZATION4_1_INSTANCING(sseA, float, 4, 4);

#endif//GASHA_FAST_ARITH_USE_SSE4_1

#ifdef GASHA_MATRIX_OPERATION_ALWAYS_USE_SSE

//----------
//行列の加算
template<>
void add<float, 4, 4>(float(&mat_result)[4][4], const float(&mat1)[4][4], const float(&mat2)[4][4])
{
	//mat1,mat2をロード
	//※16バイトアラインメントが保証されているなら、_mm_loadu_ps() ではなく、_mm_load_ps() を使用した方が高速
	const __m128 _mat1[4] = { _mm_loadu_ps(mat1[0]), _mm_loadu_ps(mat1[1]), _mm_loadu_ps(mat1[2]), _mm_loadu_ps(mat1[3]) };
	const __m128 _mat2[4] = { _mm_loadu_ps(mat2[0]), _mm_loadu_ps(mat2[1]), _mm_loadu_ps(mat2[2]), _mm_loadu_ps(mat2[3]) };
	//加算
	__m128 _mat_result[4];
	m128_add(_mat_result, _mat1, _mat2);
	for (int i = 0; i < 4; ++i)
		_mm_storeu_ps(mat_result[i], _mat_result[i]);
}

//----------
//行列の減算
template<>
void sub<float, 4, 4>(float(&mat_result)[4][4], const float(&mat1)[4][4], const float(&mat2)[4][4])
{
	//mat1,mat2をロード
	//※16バイトアラインメントが保証されているなら、_mm_loadu_ps() ではなく、_mm_load_ps() を使用した方が高速
	const __m128 _mat1[4] = { _mm_loadu_ps(mat1[0]), _mm_loadu_ps(mat1[1]), _mm_loadu_ps(mat1[2]), _mm_loadu_ps(mat1[3]) };
	const __m128 _mat2[4] = { _mm_loadu_ps(mat2[0]), _mm_loadu_ps(mat2[1]), _mm_loadu_ps(mat2[2]), _mm_loadu_ps(mat2[3]) };
	//減算
	__m128 _mat_result[4];
	m128_sub(_mat_result, _mat1, _mat2);
	for (int i = 0; i < 4; ++i)
		_mm_storeu_ps(mat_result[i], _mat_result[i]);
}

//----------
//行列のスカラー倍
template<>
void mul<float, 4, 4>(float(&mat_result)[4][4], const float(&mat)[4][4], const float scalar)
{
	//mat1をロード
	//※16バイトアラインメントが保証されているなら、_mm_loadu_ps() ではなく、_mm_load_ps() を使用した方が高速
	const __m128 _mat[4] = { _mm_loadu_ps(mat[0]), _mm_loadu_ps(mat[1]), _mm_loadu_ps(mat[2]), _mm_loadu_ps(mat[3]) };
	//乗算
	__m128 _mat_result[4];
	m128_mul(_mat_result, _mat, scalar);
	for (int i = 0; i < 4; ++i)
		_mm_storeu_ps(mat_result[i], _mat_result[i]);
}

#ifdef GASHA_USE_SSE4_1
//----------
//行列の乗算
template<>
void mul<float, 4, 4, 4>(float(&mat_result)[4][4], const float(&mat1)[4][4], const float(&mat2)[4][4])
{
	//mat1,mat2をロード
	//※16バイトアラインメントが保証されているなら、_mm_loadu_ps() ではなく、_mm_load_ps() を使用した方が高速
	const __m128 _mat1[4] = { _mm_loadu_ps(mat1[0]), _mm_loadu_ps(mat1[1]), _mm_loadu_ps(mat1[2]), _mm_loadu_ps(mat1[3]) };
	const __m128 _mat2[4] = { _mm_loadu_ps(mat2[0]), _mm_loadu_ps(mat2[1]), _mm_loadu_ps(mat2[2]), _mm_loadu_ps(mat2[3]) };
	__m128 _mat_result[4];
	m128_mul(_mat_result, _mat1, _mat2);
	for (int i = 0; i < 4; ++i)
		_mm_storeu_ps(mat_result[i], _mat_result[i]);
}
#endif//GASHA_USE_SSE4_1

#endif//GASHA_MATRIX_OPERATION_ALWAYS_USE_SSE

#endif//GASHA_USE_SSE

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_FAST_MATH_INL

// End of file

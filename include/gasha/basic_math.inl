#pragma once
#ifndef GASHA_INCLUDED_BASIC_MATH_INL
#define GASHA_INCLUDED_BASIC_MATH_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// basic_math.h
// 基本算術【インライン関数部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/basic_math.h>//基本算術【宣言部】

#include <cmath>//std::sqrt()
#include <assert.h>//assert()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//べき乗
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】べき乗（power）
template<typename T, int E>
inline T pow(const T n)
{
	static_assert(E > 0, "E is only supported greater than 0.");
	return n * GASHA_ pow<T, E - 1>(n);
}

//----------------------------------------
//【ランタイム版】負の指数のべき乗（negative power）
template<typename T, int E>
inline T negaPow(const T n)
{
	static_assert(E <= 0, "E is only supported less than 0.");
	return static_cast<T>(1) / GASHA_ pow<T, -E>(n);
}

//--------------------------------------------------------------------------------
//素数計算
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】指定の値と同じか、それより小さい最初の素数を算出
inline unsigned int makePrimeLE(const unsigned int n)
{
	return isPrime(n) ? n : GASHA_ makePrimeLT(n);
}

//----------------------------------------
//【ランタイム版】指定の値と同じか、それより大きい最初の素数を算出
inline unsigned int makePrimeGE(const unsigned int n)
{
	return isPrime(n) ? n : GASHA_ makePrimeGT(n);
}

//--------------------------------------------------------------------------------
//ビット計算
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】指定の値以上（Greater than or Equal）で最も小さい2のべき乗を算出
inline unsigned int calcNearPow2GE(const unsigned int value)
{
	if (value == 0)
		return 0;
	const unsigned int msb = GASHA_ calcMSB(value - 1) + 1;
	return msb == 32 ? 0 : 1 << msb;
}

//----------------------------------------
//【ランタイム版】指定の値以下（Less than or Equal）で最も大きい2のべき乗を算出
inline unsigned int calcNearPow2LE(const unsigned int value)
{
	if (value == 0)
		return 0;
	const unsigned int msb = GASHA_ calcMSB(value);
	return msb == 32 ? 0 : 1 << msb;
}

//--------------------------------------------------------------------------------
//アラインメント調整
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】アラインメント調整
template<unsigned int ALIGN>
inline unsigned int adjustAlign(const unsigned int value)
{
	static_assert(countStaticBits<ALIGN>::value == 1, "ALIGN is only supported power of 2.");//2のべき乗（=ビット数が1の値）でなければNG
	static const unsigned int ALIGN_1 = ALIGN - 1;
	//if ((value | ALIGN_1) == 0xffffffff)
	//	return 0;//value & ~ALIGN_1;
	return (value + ALIGN_1) & ~ALIGN_1;
}

//--------------------------------------------------------------------------------
//平方根
//--------------------------------------------------------------------------------

//----------------------------------------
//平方根
template<typename T> inline T sqrt(const T value){ return std::sqrt(value); }

//--------------------------------------------------------------------------------
//ベクトル演算
//--------------------------------------------------------------------------------

//----------------------------------------
//ノルム
template<typename T, std::size_t N>
inline T norm(const T(&vec)[N])
{
	T norm_sq = 0.f;
	for (int i = 0; i < N; ++i)
		norm_sq += vec[i] * vec[i];
	return GASHA_ sqrt(norm_sq);
}

//----------------------------------------
//ノルムの二乗
template<typename T, std::size_t N>
inline T normSq(const T(&vec)[N])
{
	T norm_sq = 0.f;
	for (int i = 0; i < N; ++i)
		norm_sq += vec[i] * vec[i];
	return norm_sq;
}

//----------------------------------------
//合成（加算）
template<typename T, std::size_t N>
inline void merge(T(&result)[N], const T(&vec1)[N], const T(&vec2)[N])
{
	for (int i = 0; i < N; ++i)
		result[i] = vec1[i] + vec2[i];
}

//----------------------------------------
//二点間の差（減算）
template<typename T, std::size_t N>
inline void difference(T(&result)[N], const T(&vec1)[N], const T(&vec2)[N])
{
	for (int i = 0; i < N; ++i)
		result[i] = vec1[i] - vec2[i];
}

//----------------------------------------
//二点間の長さ
template<typename T, std::size_t N>
inline T length(const T(&vec1)[N], const T(&vec2)[N])
{
	T len_sq = 0.f;
	for (int i = 0; i < N; ++i)
	{
		const T diff = vec1[i] - vec2[i];
		len_sq += diff * diff;
	}
	return GASHA_ sqrt(len_sq);
}

//----------------------------------------
//二点間の長さの二乗
template<typename T, std::size_t N>
inline T lengthSq(const T(&vec1)[N], const T(&vec2)[N])
{
	T len_sq = 0.f;
	for (int i = 0; i < N; ++i)
	{
		const T diff = vec1[i] - vec2[i];
		len_sq += diff * diff;
	}
	return len_sq;
}

//----------------------------------------
//正規化
template<typename T, std::size_t N>
inline void normalize(T(&result)[N], const T(&vec)[N])
{
	const T n = norm(vec);
	for (int i = 0; i < N; ++i)
		result[i] = vec[i] / n;
}

//----------------------------------------
//スカラー倍
template<typename T, std::size_t N>
inline void mul(T(&result)[N], const T(&vec)[N], const T scalar)
{
	for (int i = 0; i < N; ++i)
		result[i] = vec[i] * scalar;
}

//----------------------------------------
//内積
template<typename T, std::size_t N>
inline T dot(const T(&vec1)[N], const T(&vec2)[N])
{
	T dot = 0.f;
	for (int i = 0; i < N; ++i)
		dot += (vec1[i] * vec2[i]);
	return dot;
}

//----------------------------------------
//正規化して内積
template<typename T, std::size_t N>
inline T normalizedDot(const T(&vec1)[N], const T(&vec2)[N])
{
	const T n1 = norm(vec1);
	const T n2 = norm(vec2);
	T dot = 0.f;
	for (int i = 0; i < N; ++i)
		dot += ((vec1[i] / n1) * (vec2[i] / n2));
	return dot;
}

//----------------------------------------
//外積
template<typename T, std::size_t N>
inline void cross(T(&result)[N], const T(&vec1)[N], const T(&vec2)[N])
{
	//static_assert(N == 3 || N == 4, "cross is only used by 3D vector.");
	assert(N == 3 || N == 4);
	result[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
	result[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
	result[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_BASIC_MATH_INL

// End of file

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

#ifdef GASHA_USE_AVX
#include <immintrin.h>//AVX
#endif//GASHA_USE_AVX

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//高速除算
//--------------------------------------------------------------------------------

//----------------------------------------
//高速除数クラス
//※除数（除算の右辺値）に用いると、高速除算が適用される
//※精度は良い
//※除数が定数の場合は使用しないこと
//　⇒ 通常定数の除算はコンパイラの最適化によって定数の乗算に変更されるが、
//     このクラスを使うとそのような最適化が適用されなくなる。
template <typename T>
struct fast_div
{
	const T& m_val;
	inline operator T() const { return m_val; }
	inline fast_div(const T&& val) :
		m_val(val)
	{}
	inline fast_div(const T& val) :
		m_val(val)
	{}
};
//----------------------------------------
//最高速除数クラス
//※精度は悪いが、粗い精度でも高速性が求められる場合に使える
//  （同じ条件で計算した結果を比較したい場合など）
template <typename T>
struct fastest_div
{
	const T& m_val;
	inline operator T() const { return m_val; }
	inline fastest_div(const T&& val) :
		m_val(val)
	{}
	inline fastest_div(const T& val) :
		m_val(val)
	{}
};
//----------------------------------------
//【評価用】準高速除数クラス
//※高速除算より遅いが精度が良い ... はずが、実際には高速除算と同じ精度（速度は遅い）
template <typename T>
struct semifast_div
{
	const T& m_val;
	inline operator T() const { return m_val; }
	inline semifast_div(const T&& val) :
		m_val(val)
	{}
	inline semifast_div(const T& val) :
		m_val(val)
	{}
};
//----------------------------------------
//SSE除算命令用クラス
//※SSEの除算命令による除算（逆数を使用した演算ではない）
template <typename T>
struct sse_div
{
	const T& m_val;
	inline operator T() const { return m_val; }
	inline sse_div(const T&& val) :
		m_val(val)
	{}
	inline sse_div(const T& val) :
		m_val(val)
	{}
};
//----------------------------------------
//通常除算用除数クラス
//※普通の除算を行う
template <typename T>
struct normal_div
{
	const T& m_val;
	inline operator T() const { return m_val; }
	inline normal_div(const T&& val) :
		m_val(val)
	{}
	inline normal_div(const T& val) :
		m_val(val)
	{}
};
//----------------------------------------
//高速除数クラスの別名
using fast_div_f = fast_div<float>;
using fast_div_d = fast_div<double>;
#ifdef GASHA_USE_SSE
using fast_div_m128 = fast_div<__m128>;
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
using fast_div_m256 = fast_div<__m256>;
#endif//GASHA_USE_AVX
//最高速除数クラス（速く精度が悪い）の別名
using fastest_div_f = fastest_div<float>;
using fastest_div_d = fastest_div<double>;
#ifdef GASHA_USE_SSE
using fastest_div_m128 = fastest_div<__m128>;
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
using fastest_div_m256 = fastest_div<__m256>;
#endif//GASHA_USE_AVX
//準高速除数クラス（やや遅いがやや精度が良い）の別名
using semifast_div_f = semifast_div<float>;
using semifast_div_d = semifast_div<double>;
#ifdef GASHA_USE_SSE
using semifast_div_m128 = semifast_div<__m128>;
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
using semifast_div_m256 = semifast_div<__m256>;
#endif//GASHA_USE_AVX
//SSE除算命令用除数クラスの別名
using sse_div_f = sse_div<float>;
using sse_div_d = sse_div<double>;
#ifdef GASHA_USE_SSE
using sse_div_m128 = sse_div<__m128>;
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
using sse_div_m256 = sse_div<__m256>;
#endif//GASHA_USE_AVX
//通通常除算用除数クラスの別名
using normal_div_f = normal_div<float>;
using normal_div_d = normal_div<double>;
#ifdef GASHA_USE_SSE
using normal_div_m128 = normal_div<__m128>;
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
using normal_div_m256 = normal_div<__m256>;
#endif//GASHA_USE_AVX

//----------------------------------------
//高速除算
template<class T>
inline T operator/(const T lvalue, const fast_div<T>&& rvalue);
template<class T>
inline T operator/(const T lvalue, const fast_div<T>& rvalue);
#ifdef GASHA_USE_SSE
//※__m128(float×4)に特殊化し、逆数の乗算を行う
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const fast_div<__m128>&& rvalue);
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const fast_div<__m128>& rvalue);
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m256(float×8)に特殊化し、逆数の乗算を行う
//※__m256d(double×4)には非対応（64bitの逆数近似値算出命令がないため）
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const fast_div<__m256>&& rvalue);
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const fast_div<__m256>& rvalue);
#endif//GASHA_USE_AVX
//※floatに特殊化し、逆数の乗算を行う
template<>
inline float operator/<float>(const float lvalue, const fast_div<float>&& rvalue);
template<>
inline float operator/<float>(const float lvalue, const fast_div<float>& rvalue);

//----------------------------------------
//最高速除算
template<class T>
inline T operator/(const T lvalue, const fastest_div<T>&& rvalue);
template<class T>
inline T operator/(const T lvalue, const fastest_div<T>& rvalue);
#ifdef GASHA_USE_SSE
//※__m128(float×4)に特殊化し、逆数の乗算を行う
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const fastest_div<__m128>&& rvalue);
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const fastest_div<__m128>& rvalue);
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m256(float×8)に特殊化し、逆数の乗算を行う
//※__m256d(double×4)には非対応（64bitの逆数近似値算出命令がないため）
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const fastest_div<__m256>&& rvalue);
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const fastest_div<__m256>& rvalue);
#endif//GASHA_USE_AVX
//※floatに特殊化し、逆数の乗算を行う
template<>
inline float operator/<float>(const float lvalue, const fastest_div<float>&& rvalue);
template<>
inline float operator/<float>(const float lvalue, const fastest_div<float>& rvalue);

//----------------------------------------
//準高速除算
template<class T>
inline T operator/(const T lvalue, const semifast_div<T>&& rvalue);
template<class T>
inline T operator/(const T lvalue, const semifast_div<T>& rvalue);
#ifdef GASHA_USE_SSE
//※__m128(float×4)に特殊化し、逆数の乗算を行う
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const semifast_div<__m128>&& rvalue);
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const semifast_div<__m128>& rvalue);
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m256(float×8)に特殊化し、逆数の乗算を行う
//※__m256d(double×4)には非対応（64bitの逆数近似値算出命令がないため）
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const semifast_div<__m256>&& rvalue);
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const semifast_div<__m256>& rvalue);
#endif//GASHA_USE_AVX
//※floatに特殊化し、逆数の乗算を行う
template<>
inline float operator/<float>(const float lvalue, const semifast_div<float>&& rvalue);
template<>
inline float operator/<float>(const float lvalue, const semifast_div<float>& rvalue);

//----------------------------------------
//SSE除算
//※SSE命令が使用できない場合、通常除算として処理する
template<class T>
inline T operator/(const T lvalue, const sse_div<T>&& rvalue);
template<class T>
inline T operator/(const T lvalue, const sse_div<T>& rvalue);
#ifdef GASHA_USE_SSE
//※__m128(float×4)に特殊化し、SSE除算命令で除算する
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const sse_div<__m128>&& rvalue);
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const sse_div<__m128>& rvalue);
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m256(float×8)に特殊化し、SSE除算命令で除算する
//※__m256d(double×4)には非対応（64bitの逆数近似値算出命令がないため）
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const sse_div<__m256>&& rvalue);
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const sse_div<__m256>& rvalue);
#endif//GASHA_USE_AVX
//※floatに特殊化し、SSE除算命令で除算する
template<>
inline float operator/<float>(const float lvalue, const sse_div<float>&& rvalue);
template<>
inline float operator/<float>(const float lvalue, const sse_div<float>& rvalue);

//----------------------------------------
//通常除算
template<class T>
inline T operator/(const T lvalue, const normal_div<T>&& rvalue);
template<class T>
inline T operator/(const T lvalue, const normal_div<T>& rvalue);
#ifdef GASHA_USE_SSE
//※__m128(float×4)に特殊化し、SSE除算命令で除算する
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const normal_div<__m128>&& rvalue);
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const normal_div<__m128>& rvalue);
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m256(float×8)に特殊化し、SSE除算命令で除算する
//※__m256d(double×4)には非対応（64bitの逆数近似値算出命令がないため）
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const normal_div<__m256>&& rvalue);
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const normal_div<__m256>& rvalue);
#endif//GASHA_USE_AVX

//----------------------------------------
//高速平方根
template<typename T>
inline T fast_sqrt(const T value);
#ifdef GASHA_USE_SSE
//※__m128(float×4)の平方根を算出
template<>
inline __m128 fast_sqrt<__m128>(const __m128 value);
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m128(float×8)の平方根を算出
//※__m256d(double×4)には非対応（64bitの逆数近似値算出命令がないため）
template<>
inline __m256 fast_sqrt<__m256>(const __m256 value);
#endif//GASHA_USE_AVX
//※float型
template<>
inline float fast_sqrt<float>(const float value);

//----------------------------------------
//準高速平方根
template<typename T>
inline T semifast_sqrt(const T value);
#ifdef GASHA_USE_SSE
//※__m128(float×4)の平方根を算出
template<>
inline __m128 semifast_sqrt<__m128>(const __m128 value);
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m128(float×8)の平方根を算出
//※__m256d(double×4)には非対応（64bitの逆数近似値算出命令がないため）
template<>
inline __m256 semifast_sqrt<__m256>(const __m256 value);
#endif//GASHA_USE_AVX
//※float型
template<>
inline float semifast_sqrt<float>(const float value);

//----------------------------------------
//SSE平方根
//※SSE命令が使用できない場合、通常平方根として処理する
template<typename T>
inline T sse_sqrt(const T value);
//※逆数は使用しない
#ifdef GASHA_USE_SSE
//※__m128(float×4)の平方根を算出
template<>
inline __m128 sse_sqrt<__m128>(const __m128 value);
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m128(float×8)の平方根を算出
//※__m256d(double×4)には非対応（64bitの逆数近似値算出命令がないため）
template<>
inline __m256 sse_sqrt<__m256>(const __m256 value);
#endif//GASHA_USE_AVX
//※float型
template<>
inline float sse_sqrt<float>(const float value);

//----------------------------------------
//通常平方根
template<typename T>
inline T normal_sqrt(const T value);
#ifdef GASHA_USE_SSE
//※__m128(float×4)の平方根を算出
template<>
inline __m128 normal_sqrt<__m128>(const __m128 value);
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m128(float×8)の平方根を算出
//※__m256d(double×4)には非対応（64bitの逆数近似値算出命令がないため）
template<>
inline __m256 normal_sqrt<__m256>(const __m256 value);
#endif//GASHA_USE_AVX

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルードする
#include <gasha/fast_math.inl>

#endif//GASHA_INCLUDED_FAST_MATH_H

// End of file

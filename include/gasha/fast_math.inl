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

//----------------------------------------
//高速除算
template<class T>
inline T operator/(const T lvalue, const fast_div<T>&& rvalue)
{
	return lvalue / static_cast<T>(rvalue);
}
template<class T>
inline T operator/(const T lvalue, const fast_div<T>& rvalue)
{
	return lvalue / static_cast<T>(rvalue);
}
#ifdef GASHA_USE_SSE
//※__m128(float×4)に特殊化し、逆数の乗算を行う
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const fast_div<__m128>&& rvalue)
{
	const __m128 div_m128 = static_cast<__m128>(rvalue);//SSEレジスタに除数を代入
	//【ステップ１】逆数の近似値を取得
	__m128 rcp_div_m128 = _mm_rcp_ps(div_m128);//逆数の近似値を高速に算出するSSE命令
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
	rcp_div_m128 = _mm_mul_ps(rcp_div_m128, _mm_sub_ps(_mm_set1_ps(2.f), _mm_mul_ps(div_m128, rcp_div_m128)));
	//【ステップ３】逆数を乗算
	const __m128 result_m128 = _mm_mul_ps(lvalue, rcp_div_m128);
	return result_m128;
}
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const fast_div<__m128>& rvalue)
{
	return lvalue / std::move(rvalue);
}
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m256(float×8)に特殊化し、逆数の乗算を行う
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const fast_div<__m256>&& rvalue)
{
	const __m256 div_m256 = static_cast<__m256>(rvalue);//SSEレジスタに除数を代入
	__m256 rcp_div_m256 = _mm256_rcp_ps(div_m256);//逆数の近似値を高速に算出するSSE命令
	rcp_div_m256 = _mm256_mul_ps(rcp_div_m256, _mm256_sub_ps(_mm256_set1_ps(2.f), _mm256_mul_ps(div_m256, rcp_div_m256)));//ニュートン法で逆数の精度を高める
	const __m256 result_m256 = _mm256_mul_ps(lvalue, rcp_div_m256);//逆数を乗算
	return result_m256;
}
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const fast_div<__m256>& rvalue)
{
	return lvalue / std::move(rvalue);
}
#endif//GASHA_USE_AVX
//※floatに特殊化し、逆数の乗算を行う
template<>
inline float operator/<float>(const float lvalue, const fast_div<float>&& rvalue)
{
#ifdef GASHA_FAST_DIV_FLOAT_USE_SSE
	float result;
	_mm_store_ss(&result, _mm_set1_ps(lvalue) / std::move(fast_div<__m128>(_mm_set1_ps(rvalue))));
	return result;
#else//GASHA_FAST_DIV_FLOAT_USE_SSE
	return lvalue / static_cast<float>(rvalue);
#endif//GASHA_FAST_DIV_FLOAT_USE_SSE
}
template<>
inline float operator/<float>(const float lvalue, const fast_div<float>& rvalue)
{
	return lvalue / std::move(rvalue);
}

//----------------------------------------
//最高速除算
template<class T>
inline T operator/(const T lvalue, const fastest_div<T>&& rvalue)
{
	return lvalue / static_cast<T>(rvalue);
}
template<class T>
inline T operator/(const T lvalue, const fastest_div<T>& rvalue)
{
	return lvalue / static_cast<T>(rvalue);
}
#ifdef GASHA_USE_SSE
//※__m128(float×4)に特殊化し、逆数の乗算を行う
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const fastest_div<__m128>&& rvalue)
{
	const __m128 div_m128 = static_cast<__m128>(rvalue);//SSEレジスタに除数を代入
	__m128 rcp_div_m128 = _mm_rcp_ps(div_m128);//逆数の近似値を高速に算出するSSE命令
	const __m128 result_m128 = _mm_mul_ps(lvalue, rcp_div_m128);//逆数を乗算
	return result_m128;
}
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const fastest_div<__m128>& rvalue)
{
	return lvalue / std::move(rvalue);
}
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m256(float×8)に特殊化し、逆数の乗算を行う
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const fastest_div<__m256>&& rvalue)
{
	const __m256 div_m256 = static_cast<__m256>(rvalue);//SSEレジスタに除数を代入
	__m256 rcp_div_m256 = _mm256_rcp_ps(div_m256);//逆数の近似値を高速に算出するSSE命令
	const __m256 result_m256 = _mm256_mul_ps(lvalue, rcp_div_m256);//逆数を乗算
	return result_m256;
}
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const fastest_div<__m256>& rvalue)
{
	return lvalue / std::move(rvalue);
}
#endif//GASHA_USE_AVX
//※floatに特殊化し、逆数の乗算を行う
template<>
inline float operator/<float>(const float lvalue, const fastest_div<float>&& rvalue)
{
#ifdef GASHA_FAST_DIV_FLOAT_USE_SSE
	float result;
	_mm_store_ss(&result, _mm_set1_ps(lvalue) / std::move(fastest_div<__m128>(_mm_set1_ps(rvalue))));
	return result;
#else//GASHA_FAST_DIV_FLOAT_USE_SSE
	return lvalue / static_cast<float>(rvalue);
#endif//GASHA_FAST_DIV_FLOAT_USE_SSE
}
template<>
inline float operator/<float>(const float lvalue, const fastest_div<float>& rvalue)
{
	return lvalue / std::move(rvalue);
}

//----------------------------------------
//準高速除算
template<class T>
inline T operator/(const T lvalue, const semifast_div<T>&& rvalue)
{
	return lvalue / static_cast<T>(rvalue);
}
template<class T>
inline T operator/(const T lvalue, const semifast_div<T>& rvalue)
{
	return lvalue / static_cast<T>(rvalue);
}
#ifdef GASHA_USE_SSE
//※__m128(float×4)に特殊化し、逆数の乗算を行う
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const semifast_div<__m128>&& rvalue)
{
	const __m128 div_m128 = static_cast<__m128>(rvalue);//SSEレジスタに除数を代入
	__m128 rcp_div_m128 = _mm_rcp_ps(div_m128);//逆数の近似値を高速に算出するSSE命令
	rcp_div_m128 = _mm_mul_ps(rcp_div_m128, _mm_sub_ps(_mm_set1_ps(2.f), _mm_mul_ps(div_m128, rcp_div_m128)));//ニュートン法で逆数の精度を高める
	rcp_div_m128 = _mm_mul_ps(rcp_div_m128, _mm_sub_ps(_mm_set1_ps(2.f), _mm_mul_ps(div_m128, rcp_div_m128)));//2回目
	const __m128 result_m128 = _mm_mul_ps(lvalue, rcp_div_m128);//逆数を乗算
	return result_m128;
}
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const semifast_div<__m128>& rvalue)
{
	return lvalue / std::move(rvalue);
}
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m256(float×8)に特殊化し、逆数の乗算を行う
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const semifast_div<__m256>&& rvalue)
{
	const __m256 div_m256 = static_cast<__m256>(rvalue);//SSEレジスタに除数を代入
	__m256 rcp_div_m256 = _mm256_rcp_ps(div_m256);//逆数の近似値を高速に算出するSSE命令
	rcp_div_m256 = _mm256_mul_ps(rcp_div_m256, _mm256_sub_ps(_mm256_set1_ps(2.f), _mm256_mul_ps(div_m256, rcp_div_m256)));//ニュートン法で逆数の精度を高める
	rcp_div_m256 = _mm256_mul_ps(rcp_div_m256, _mm256_sub_ps(_mm256_set1_ps(2.f), _mm256_mul_ps(div_m256, rcp_div_m256)));//2回目
	const __m256 result_m256 = _mm256_mul_ps(lvalue, rcp_div_m256);//逆数を乗算
	return result_m256;
}
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const semifast_div<__m256>& rvalue)
{
	return lvalue / std::move(rvalue);
}
#endif//GASHA_USE_AVX
//※floatに特殊化し、逆数の乗算を行う
template<>
inline float operator/<float>(const float lvalue, const semifast_div<float>&& rvalue)
{
#ifdef GASHA_FAST_DIV_FLOAT_USE_SSE
	float result;
	_mm_store_ss(&result, _mm_set1_ps(lvalue) / std::move(semifast_div<__m128>(_mm_set1_ps(rvalue))));
	return result;
#else//GASHA_FAST_DIV_FLOAT_USE_SSE
	return lvalue / static_cast<float>(rvalue);
#endif//GASHA_FAST_DIV_FLOAT_USE_SSE
}
template<>
inline float operator/<float>(const float lvalue, const semifast_div<float>& rvalue)
{
	return lvalue / std::move(rvalue);
}

//----------------------------------------
//SSE除算
template<class T>
inline T operator/(const T lvalue, const sse_div<T>&& rvalue)
{
	return lvalue / static_cast<T>(rvalue);
}
template<class T>
inline T operator/(const T lvalue, const sse_div<T>& rvalue)
{
	return lvalue / static_cast<T>(rvalue);
}
#ifdef GASHA_USE_SSE
//※__m128(float×4)に特殊化し、SSE除算命令で除算する
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const sse_div<__m128>&& rvalue)
{
	const __m128 div_m128 = static_cast<__m128>(rvalue);//SSEレジスタに除数を代入
	return _mm_div_ps(lvalue, div_m128);//除算
}
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const sse_div<__m128>& rvalue)
{
	return lvalue / std::move(rvalue);
}
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m256(float×8)に特殊化し、SSE除算命令で除算する
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const sse_div<__m256>&& rvalue)
{
	const __m256 div_m256 = static_cast<__m256>(rvalue);//SSEレジスタに除数を代入
	return _mm256_div_ps(lvalue, div_m256);//除算
}
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const sse_div<__m256>& rvalue)
{
	return lvalue / std::move(rvalue);
}
#endif//GASHA_USE_AVX
//※floatに特殊化し、SSE除算命令で除算する
template<>
inline float operator/<float>(const float lvalue, const sse_div<float>&& rvalue)
{
#ifdef GASHA_FAST_DIV_FLOAT_USE_SSE
	float result;
	_mm_store_ss(&result, _mm_set1_ps(lvalue) / std::move(sse_div<__m128>(_mm_set1_ps(rvalue))));
	return result;
#else//GASHA_FAST_DIV_FLOAT_USE_SSE
	return lvalue / static_cast<float>(rvalue);
#endif//GASHA_FAST_DIV_FLOAT_USE_SSE
}
template<>
inline float operator/<float>(const float lvalue, const sse_div<float>& rvalue)
{
	return lvalue / std::move(rvalue);
}

//----------------------------------------
//通常除算
template<class T>
inline T operator/(const T lvalue, const normal_div<T>&& rvalue)
{
	return lvalue / static_cast<T>(rvalue);
}
template<class T>
inline T operator/(const T lvalue, const normal_div<T>& rvalue)
{
	return lvalue / static_cast<T>(rvalue);
}
#ifdef GASHA_USE_SSE
//※__m128(float×4)に特殊化し、SSE除算命令で除算する
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const normal_div<__m128>&& rvalue)
{
	const __m128 div_m128 = static_cast<__m128>(rvalue);//SSEレジスタに除数を代入
	return _mm_div_ps(lvalue, div_m128);//除算
}
template<>
inline __m128 operator/<__m128>(const __m128 lvalue, const normal_div<__m128>& rvalue)
{
	return lvalue / std::move(rvalue);
}
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m256(float×8)に特殊化し、SSE除算命令で除算する
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const normal_div<__m256>&& rvalue)
{
	const __m256 div_m256 = static_cast<__m256>(rvalue);//SSEレジスタに除数を代入
	return _mm256_div_ps(lvalue, div_m256);//除算
}
template<>
inline __m256 operator/<__m256>(const __m256 lvalue, const normal_div<__m256>& rvalue)
{
	return lvalue / std::move(rvalue);
}
#endif//GASHA_USE_AVX

//----------------------------------------
//高速平方根
template<typename T>
inline T fast_sqrt(const T value)
{
	return std::sqrt(value);
}
#ifdef GASHA_USE_SSE
//※__m128(float×4)の平方根を算出
template<>
inline __m128 fast_sqrt<__m128>(const __m128 value)
{
	__m128 rcp_sqrt_m128 = _mm_rsqrt_ps(value);//平方根の逆数の近似値を取得
	const __m128 result_m128 = _mm_mul_ps(value, rcp_sqrt_m128);//逆数を乗算
	return result_m128;
}
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m128(float×8)の平方根を算出
template<>
inline __m256 fast_sqrt<__m256>(const __m256 value)
{
	__m256 rcp_sqrt_m256 = _mm256_rsqrt_ps(value);//平方根の逆数の近似値を取得
	const __m256 result_m256 = _mm256_mul_ps(value, rcp_sqrt_m256);//逆数を乗算
	return result_m256;
}
#endif//GASHA_USE_AVX
//※float型
template<>
inline float fast_sqrt<float>(const float value)
{
#ifdef GASHA_FAST_SQRT_FLOAT_USE_SSE
	float result;
	_mm_store_ss(&result, fast_sqrt(_mm_set1_ps(value)));
	return result;
#else//GASHA_FAST_SQRT_FLOAT_USE_SSE
	return std::sqrt(value);
#endif//GASHA_FAST_SQRT_FLOAT_USE_SSE
}

//----------------------------------------
//準高速平方根
template<typename T>
inline T semifast_sqrt(const T value)
{
	return std::sqrt(value);
}
#ifdef GASHA_USE_SSE
//※__m128(float×4)の平方根を算出
template<>
inline __m128 semifast_sqrt<__m128>(const __m128 value)
{
	//【ステップ１】平方根の逆数の近似値を取得
	__m128 rcp_sqrt_m128 = _mm_rsqrt_ps(value);
	//【ステップ２】ニュートン法で逆数の精度を高める
	//ニュートン法による漸化式：rsqrt = rsqrt * (3 - value * rsqrt * rsqrt) * 0.5f
	//  平方根の漸化式:      x[i+1] = (x[i] + value / x[i]) * 0.5
	//  平方根の逆数の漸化式:x[i+1] = x[i] * (3 - value * x[i] ^ 2) * 0.5
	rcp_sqrt_m128 = _mm_mul_ps(_mm_mul_ps(rcp_sqrt_m128, _mm_sub_ps(_mm_set1_ps(3.f), _mm_mul_ps(value, _mm_mul_ps(rcp_sqrt_m128, rcp_sqrt_m128)))), _mm_set1_ps(0.5f));
	//【ステップ３】逆数を乗算
	const __m128 result_m128 = _mm_mul_ps(value, rcp_sqrt_m128);
	return result_m128;
}
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m128(float×8)の平方根を算出
template<>
inline __m256 semifast_sqrt<__m256>(const __m256 value)
{
	__m256 rcp_sqrt_m256 = _mm256_rsqrt_ps(value);//平方根の逆数の近似値を取得
	rcp_sqrt_m256 = _mm256_mul_ps(_mm256_mul_ps(rcp_sqrt_m256, _mm256_sub_ps(_mm256_set1_ps(3.f), _mm256_mul_ps(value, _mm256_mul_ps(rcp_sqrt_m256, rcp_sqrt_m256)))), _mm256_set1_ps(0.5f));//ニュートン法で逆数の精度を高める
	const __m256 result_m256 = _mm256_mul_ps(value, rcp_sqrt_m256);//逆数を乗算
	return result_m256;
}
#endif//GASHA_USE_AVX
//※float型
template<>
inline float semifast_sqrt<float>(const float value)
{
#ifdef GASHA_SEMIFAST_SQRT_FLOAT_USE_SSE
	float result;
	_mm_store_ss(&result, semifast_sqrt(_mm_set1_ps(value)));
	return result;
#else//GASHA_SEMIFAST_SQRT_FLOAT_USE_SSE
	return std::sqrt(value);
#endif//GASHA_SEMIFAST_SQRT_FLOAT_USE_SSE
}

//----------------------------------------
//SSE平方根
template<typename T>
inline T sse_sqrt(const T value)
{
	return std::sqrt(value);
}
#ifdef GASHA_USE_SSE
//※__m128(float×4)の平方根を算出
template<>
inline __m128 sse_sqrt<__m128>(const __m128 value)
{
	return _mm_sqrt_ps(value);//平方根算出
}
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m128(float×8)の平方根を算出
template<>
inline __m256 sse_sqrt<__m256>(const __m256 value)
{
	return _mm256_sqrt_ps(value);//平方根算出
}
#endif//GASHA_USE_AVX
//※float型
template<>
inline float sse_sqrt<float>(const float value)
{
#ifdef GASHA_FAST_SQRT_FLOAT_USE_SSE
	float result;
	_mm_store_ss(&result, sse_sqrt(_mm_set1_ps(value)));
	return result;
#else//GASHA_FAST_SQRT_FLOAT_USE_SSE
	return std::sqrt(value);
#endif//GASHA_FAST_SQRT_FLOAT_USE_SSE
}

//----------------------------------------
//通常平方根
template<typename T>
inline T normal_sqrt(const T value)
{
	return std::sqrt(value);
}
#ifdef GASHA_USE_SSE
//※__m128(float×4)の平方根を算出
template<>
inline __m128 normal_sqrt<__m128>(const __m128 value)
{
	return _mm_sqrt_ps(value);//平方根算出
}
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_AVX
//※__m128(float×8)の平方根を算出
template<>
inline __m256 normal_sqrt<__m256>(const __m256 value)
{
	return _mm256_sqrt_ps(value);//平方根算出
}
#endif//GASHA_USE_AVX

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_FAST_MATH_INL

// End of file

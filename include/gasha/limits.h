﻿#pragma once
#ifndef GASHA_INCLUDED_LIMITS_H
#define GASHA_INCLUDED_LIMITS_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ】
// limits.h
// 限界値【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstddef>//std::size_t
#include <limits>//std::numeric_limits
#include <climits>//***_MIN, ***_MAX
#include <cstdint>//C++11 std::int**_t, std::uint**_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//型の限界値情報
template<typename T>
class numeric_limits : public std::numeric_limits<T>
{
public:
	typedef T signed_type;//同サイズの符号付き型
	typedef T unsigned_type;//同サイズの符号なし型
	typedef T contained_signed_type;//値全域を含む符号付き型
	typedef T range_type;//値の範囲型（符号なし型）
	typedef T signed_range_type;//値の範囲型（符号付き型）
	static const T MIN = 0;//最小値
	static const T MAX = 0;//最大値
	static const T ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(T);//サイズ
	inline static constexpr T zero(){ return 0; }//ゼロ
};
//型の限界値情報：bool型
template<>
class numeric_limits<bool> : public std::numeric_limits<bool>
{
public:
	typedef bool signed_type;//同サイズの符号付き型
	typedef bool unsigned_type;//同サイズの符号なし型
	typedef bool contained_signed_type;//値全域を含む符号付き型
	typedef bool range_type;//値の範囲型（符号なし型）
	typedef bool signed_range_type;//値の範囲型（符号付き型）
	static const bool MIN = false;//最小値
	static const bool MAX = true;//最大値
	static const bool ZERO = false;//ゼロ
	static const std::size_t SIZE = sizeof(bool);//サイズ
	inline static constexpr bool zero(){ return false; }//ゼロ
};
#if 0//整数型は、std::int*_t, std::uint*_t の特殊化のみ使用
//型の限界値情報：char型
template<>
class numeric_limits<char> : public std::numeric_limits<char>
{
public:
	typedef char signed_type;//同サイズの符号付き型
	typedef unsigned char unsigned_type;//同サイズの符号なし型
	typedef char contained_signed_type;//値全域を含む符号付き型
	typedef unsigned char range_type;//値の範囲型（符号なし型）
	typedef short signed_range_type;//値の範囲型（符号付き型）
	static const char MIN = CHAR_MIN;//最小値
	static const char MAX = CHAR_MAX;//最大値
	static const char ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(char);//サイズ
	inline static constexpr char zero(){ return 0; }//ゼロ
};
//型の限界値情報：unsigned char型
template<>
class numeric_limits<unsigned char> : public std::numeric_limits<unsigned char>
{
public:
	typedef char signed_type;//同サイズの符号付き型
	typedef unsigned char unsigned_type;//同サイズの符号なし型
	typedef short contained_signed_type;//値全域を含む符号付き型
	typedef unsigned char range_type;//値の範囲型（符号なし型）
	typedef short signed_range_type;//値の範囲型（符号付き型）
	static const unsigned char MIN = 0;//最小値
	static const unsigned char MAX = UCHAR_MAX;//最大値
	static const unsigned char ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(unsigned char);//サイズ
	inline static constexpr unsigned char zero(){ return 0; }//ゼロ
};
//型の限界値情報：short型
template<>
class numeric_limits<short> : public std::numeric_limits<short>
{
public:
	typedef short signed_type;//同サイズの符号付き型
	typedef unsigned short unsigned_type;//同サイズの符号なし型
	typedef short contained_signed_type;//値全域を含む符号付き型
	typedef unsigned short range_type;//値の範囲型（符号なし型）
	typedef int signed_range_type;//値の範囲型（符号付き型）
	static const short MIN = SHRT_MIN;//最小値
	static const short MAX = SHRT_MAX;//最大値
	static const short ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(short);//サイズ
	inline static constexpr short zero(){ return 0; }//ゼロ
};
//型の限界値情報：unsigned short型
template<>
class numeric_limits<unsigned short> : public std::numeric_limits<unsigned short>
{
public:
	typedef short signed_type;//同サイズの符号付き型
	typedef unsigned short unsigned_type;//同サイズの符号なし型
	typedef int contained_signed_type;//値全域を含む符号付き型
	typedef unsigned short range_type;//値の範囲型（符号なし型）
	typedef int signed_range_type;//値の範囲型（符号付き型）
	static const unsigned short MIN = 0;//最小値
	static const unsigned short MAX = USHRT_MAX;//最大値
	static const unsigned short ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(unsigned short);//サイズ
	inline static constexpr unsigned short zero(){ return 0; }//ゼロ
};
//型の限界値情報：int型
template<>
class numeric_limits<int> : public std::numeric_limits<int>
{
public:
	typedef int signed_type;//同サイズの符号付き型
	typedef unsigned int unsigned_type;//同サイズの符号なし型
	typedef int contained_signed_type;//値全域を含む符号付き型
	typedef unsigned int range_type;//値の範囲型（符号なし型）
	typedef long long signed_range_type;//値の範囲型（符号付き型）
	static const int MIN = INT_MIN;//最小値
	static const int MAX = INT_MAX;//最大値
	static const int ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(int);//サイズ
	inline static constexpr int zero(){ return 0; }//ゼロ
};
//型の限界値情報：unsigned int型
template<>
class numeric_limits<unsigned int> : public std::numeric_limits<unsigned int>
{
public:
	typedef int signed_type;//同サイズの符号付き型
	typedef unsigned int unsigned_type;//同サイズの符号なし型
	typedef long long contained_signed_type;//値全域を含む符号付き型
	typedef unsigned int range_type;//値の範囲型（符号なし型）
	typedef long long signed_range_type;//値の範囲型（符号付き型）
	static const unsigned int MIN = 0;//最小値
	static const unsigned int MAX = UINT_MAX;//最大値
	static const unsigned int ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(unsigned int);//サイズ
	inline static constexpr unsigned int zero(){ return 0; }//ゼロ
};
//型の限界値情報：long型
template<>
class numeric_limits<long> : public std::numeric_limits<long>
{
public:
	typedef long signed_type;//同サイズの符号付き型
	typedef unsigned long unsigned_type;//同サイズの符号なし型
	typedef long contained_signed_type;//値全域を含む符号付き型
	typedef unsigned long range_type;//値の範囲型（符号なし型）
	typedef long long signed_range_type;//値の範囲型（符号付き型）
	static const long MIN = LONG_MIN;//最小値
	static const long MAX = LONG_MAX;//最大値
	static const long ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(long);//サイズ
	inline static constexpr long zero(){ return 0; }//ゼロ
};
//型の限界値情報：unsigned int型
template<>
class numeric_limits<unsigned long> : public std::numeric_limits<unsigned long>
{
public:
	typedef long signed_type;//同サイズの符号付き型
	typedef unsigned long unsigned_type;//同サイズの符号なし型
	typedef long long contained_signed_type;//値全域を含む符号付き型
	typedef unsigned long range_type;//値の範囲型（符号なし型）
	typedef long long signed_range_type;//値の範囲型（符号付き型）
	static const unsigned long MIN = 0;//最小値
	static const unsigned long MAX = ULONG_MAX;//最大値
	static const unsigned long ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(unsigned long);//サイズ
	inline static constexpr unsigned long zero(){ return 0; }//ゼロ
};
//型の限界値情報：long long型
template<>
class numeric_limits<long long> : public std::numeric_limits<long long>
{
public:
	typedef long long signed_type;//同サイズの符号付き型
	typedef unsigned long long unsigned_type;//同サイズの符号なし型
	typedef long long contained_signed_type;//値全域を含む符号付き型
	typedef unsigned long range_type;//値の範囲型（符号なし型）
	typedef long long signed_range_type;//値の範囲型（符号付き型） ※やむを得ず
	static const long long MIN = LLONG_MIN;//最小値
	static const long long MAX = LLONG_MAX;//最大値
	static const long long ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(long long);//サイズ
	inline static constexpr long long zero(){ return 0; }//ゼロ
};
//型の限界値情報：unsigned long long型
template<>
class numeric_limits<unsigned long long> : public std::numeric_limits<unsigned long long>
{
public:
	typedef long long signed_type;//同サイズの符号付き型
	typedef unsigned long long unsigned_type;//同サイズの符号なし型
	typedef long long contained_signed_type;//値全域を含む符号付き型 ※やむを得ず
	typedef unsigned long range_type;//値の範囲型（符号なし型）
	typedef long long signed_range_type;//値の範囲型（符号付き型） ※やむを得ず
	static const unsigned long long MIN = 0;//最小値
	static const unsigned long long MAX = ULLONG_MAX;//最大値
	static const unsigned long long ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(unsigned long long);//サイズ
	inline static constexpr unsigned long long zero(){ return 0; }//ゼロ
};
#endif
//型の限界値情報：float型
template<>
class numeric_limits<float> : public std::numeric_limits<float>
{
public:
	typedef float signed_type;//同サイズの符号付き型
	typedef float unsigned_type;//同サイズの符号なし型
	typedef float contained_signed_type;//値全域を含む符号付き型
	typedef float range_type;//値の範囲型（符号なし型）
	typedef float signed_range_type;//値の範囲型（符号付き型）
	static const float MIN;//最小値
	static const float MAX;//最大値
	static const float ZERO;//ゼロ
	static const std::size_t SIZE = sizeof(float);//サイズ
	inline static constexpr float zero(){ return 0.f; }//ゼロ
};
//型の限界値情報：double型
template<>
class numeric_limits<double> : public std::numeric_limits<double>
{
public:
	typedef double signed_type;//同サイズの符号付き型
	typedef double unsigned_type;//同サイズの符号なし型
	typedef double contained_signed_type;//値全域を含む符号付き型
	typedef double range_type;//値の範囲型（符号なし型）
	typedef double signed_range_type;//値の範囲型（符号付き型）
	static const double MIN;//最小値
	static const double MAX;//最大値
	static const double ZERO;//ゼロ
	static const std::size_t SIZE = sizeof(double);//サイズ
	inline static constexpr double zero(){ return 0.; }//ゼロ
};
//型の限界値情報：std::int8_t型
template<>
class numeric_limits<std::int8_t> : public std::numeric_limits<std::int8_t>
{
public:
	typedef std::int8_t signed_type;//同サイズの符号付き型
	typedef std::uint8_t unsigned_type;//同サイズの符号なし型
	typedef std::int8_t contained_signed_type;//値全域を含む符号付き型
	typedef std::uint8_t range_type;//値の範囲型（符号なし型）
	typedef std::int16_t signed_range_type;//値の範囲型（符号付き型）
	static const std::int8_t MIN = INT8_MIN;//最小値
	static const std::int8_t MAX = INT8_MAX;//最大値
	static const std::int8_t ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(std::int8_t);//サイズ
	inline static constexpr std::int8_t zero(){ return 0; }//ゼロ
};
//型の限界値情報：std::uint8_t型
template<>
class numeric_limits<std::uint8_t> : public std::numeric_limits<std::uint8_t>
{
public:
	typedef std::int8_t signed_type;//同サイズの符号付き型
	typedef std::uint8_t unsigned_type;//同サイズの符号なし型
	typedef std::int16_t contained_signed_type;//値全域を含む符号付き型
	typedef std::uint8_t range_type;//値の範囲型（符号なし型）
	typedef std::int16_t signed_range_type;//値の範囲型（符号付き型）
	static const std::uint8_t MIN = 0;//最小値
	static const std::uint8_t MAX = UINT8_MAX;//最大値
	static const std::uint8_t ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(std::uint8_t);//サイズ
	inline static constexpr std::uint8_t zero(){ return 0; }//ゼロ
};
//型の限界値情報：std::int16_t型
template<>
class numeric_limits<std::int16_t> : public std::numeric_limits<std::int16_t>
{
public:
	typedef std::int16_t signed_type;//同サイズの符号付き型
	typedef std::uint16_t unsigned_type;//同サイズの符号なし型
	typedef std::int16_t contained_signed_type;//値全域を含む符号付き型
	typedef std::uint16_t range_type;//値の範囲型（符号なし型）
	typedef std::int32_t signed_range_type;//値の範囲型（符号付き型）
	static const std::int16_t MIN = INT16_MIN;//最小値
	static const std::int16_t MAX = INT16_MAX;//最大値
	static const std::int16_t ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(std::int16_t);//サイズ
	inline static constexpr std::int16_t zero(){ return 0; }//ゼロ
};
//型の限界値情報：std::uint16_t型
template<>
class numeric_limits<std::uint16_t> : public std::numeric_limits<std::uint16_t>
{
public:
	typedef std::int16_t signed_type;//同サイズの符号付き型
	typedef std::uint16_t unsigned_type;//同サイズの符号なし型
	typedef std::int32_t contained_signed_type;//値全域を含む符号付き型
	typedef std::uint16_t range_type;//値の範囲型（符号なし型）
	typedef std::int32_t signed_range_type;//値の範囲型（符号付き型）
	static const std::uint16_t MIN = 0;//最小値
	static const std::uint16_t MAX = UINT16_MAX;//最大値
	static const std::uint16_t ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(std::uint16_t);//サイズ
	inline static constexpr std::uint16_t zero(){ return 0; }//ゼロ
};
//型の限界値情報：std::int32_t型
template<>
class numeric_limits<std::int32_t> : public std::numeric_limits<std::int32_t>
{
public:
	typedef std::int32_t signed_type;//同サイズの符号付き型
	typedef std::uint32_t unsigned_type;//同サイズの符号なし型
	typedef std::int32_t contained_signed_type;//値全域を含む符号付き型
	typedef std::uint32_t range_type;//値の範囲型（符号なし型）
	typedef std::int64_t signed_range_type;//値の範囲型（符号付き型）
	static const std::int32_t MIN = INT32_MIN;//最小値
	static const std::int32_t MAX = INT32_MAX;//最大値
	static const std::int32_t ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(std::int32_t);//サイズ
	inline static constexpr std::int32_t zero(){ return 0; }//ゼロ
};
//型の限界値情報：std::uint32_t型
template<>
class numeric_limits<std::uint32_t> : public std::numeric_limits<std::uint32_t>
{
public:
	typedef std::int32_t signed_type;//同サイズの符号付き型
	typedef std::uint32_t unsigned_type;//同サイズの符号なし型
	typedef std::int64_t contained_signed_type;//値全域を含む符号付き型
	typedef std::uint32_t range_type;//値の範囲型（符号なし型）
	typedef std::int64_t signed_range_type;//値の範囲型（符号付き型）
	static const std::uint32_t MIN = 0;//最小値
	static const std::uint32_t MAX = UINT32_MAX;//最大値
	static const std::uint32_t ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(std::uint32_t);//サイズ
	inline static constexpr std::uint32_t zero(){ return 0; }//ゼロ
};
//型の限界値情報：std::int64_t型
template<>
class numeric_limits<std::int64_t> : public std::numeric_limits<std::int64_t>
{
public:
	typedef std::int64_t signed_type;//同サイズの符号付き型
	typedef std::uint64_t unsigned_type;//同サイズの符号なし型
	typedef std::int64_t contained_signed_type;//値全域を含む符号付き型
	typedef std::uint64_t range_type;//値の範囲型（符号なし型）
	typedef std::int64_t signed_range_type;//値の範囲型（符号付き型） ※やむを得ず
	static const std::int64_t MIN = INT64_MIN;//最小値
	static const std::int64_t MAX = INT64_MAX;//最大値
	static const std::int64_t ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(std::int64_t);//サイズ
	inline static constexpr std::int64_t zero(){ return 0; }//ゼロ
};
//型の限界値情報：std::uint64_t型
template<>
class numeric_limits<std::uint64_t> : public std::numeric_limits<std::uint64_t>
{
public:
	typedef std::int64_t signed_type;//同サイズの符号付き型
	typedef std::uint64_t unsigned_type;//同サイズの符号なし型
	typedef std::int16_t contained_signed_type;//値全域を含む符号付き型 ※やむを得ず
	typedef std::uint64_t range_type;//値の範囲型（符号なし型）
	typedef std::int64_t signed_range_type;//値の範囲型（符号付き型） ※やむを得ず
	static const std::uint64_t MIN = 0;//最小値
	static const std::uint64_t MAX = UINT64_MAX;//最大値
	static const std::uint64_t ZERO = 0;//ゼロ
	static const std::size_t SIZE = sizeof(std::uint64_t);//サイズ
	inline static constexpr std::uint64_t zero(){ return 0; }//ゼロ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/limits.inl>

#endif//GASHA_INCLUDED_LIMITS_H

// End of file

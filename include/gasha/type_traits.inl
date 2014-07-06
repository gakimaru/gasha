#pragma once
#ifndef GASHA_INCLUDED_TYPE_TRAITS_INL
#define GASHA_INCLUDED_TYPE_TRAITS_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ】
// type_traits.inl
// 型特性ユーティリティ【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/type_traits.h>//型特性ユーティリティ【宣言部】

#include <gasha/fast_string.h>//高速文字列：strcpy_fast()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//汎用ユニオン
//--------------------------------------------------------------------------------

//----------------------------------------
//汎用ユニオン
inline void unionTypes::clear()
{
	for (std::size_t i = 0; i < unionTypesSize / sizeof(long long); ++i)
		m_long[i] = 0;
}
inline unionTypes::unionTypes(const char value)
{
	clear();
	m_char[0] = value;
}
inline unionTypes::unionTypes(const unsigned char value)
{
	clear();
	m_uchar[0] = value;
}
inline unionTypes::unionTypes(const short value)
{
	clear();
	m_short[0] = value;
}
inline unionTypes::unionTypes(const unsigned short value)
{
	clear();
	m_ushort[0] = value;
}
inline unionTypes::unionTypes(const int value)
{
	clear();
	m_int[0] = value;
}
inline unionTypes::unionTypes(const unsigned int value)
{
	clear();
	m_uint[0] = value;
}
inline unionTypes::unionTypes(const long value)
{
	clear();
	m_long[0] = value;
}
inline unionTypes::unionTypes(const unsigned long value)
{
	clear();
	m_ulong[0] = value;
}
inline unionTypes::unionTypes(const long long value)
{
	clear();
	m_llong[0] = value;
}
inline unionTypes::unionTypes(const unsigned long long value)
{
	clear();
	m_ullong[0] = value;
}
inline unionTypes::unionTypes(const float value)
{
	clear();
	m_float[0] = value;
}
inline unionTypes::unionTypes(const double value)
{
	clear();
	m_double[0] = value;
}

//--------------------------------------------------------------------------------
//文字列化
//--------------------------------------------------------------------------------

//値を文字列に変換
template<typename T>
inline const char* toStr(const T value)
{
	return "(unknown)";
}
template<typename T>
inline const char* toStr(char* buff, const T value)
{
	return strcpy_fast(buff, toStr(value));
}
template<typename T>
inline const char* toNumStr(char* buff, const T value)
{
	return strcpy_fast(buff, toStr(value));
}
template<typename T>
inline const char* toHexStr(char* buff, const T value)
{
	return strcpy_fast(buff, toStr(value));
}
template<typename T>
inline const char* toByteStr(char* buff, const T value)
{
	return strcpy_fast(buff, toStr(value));
}

//※bool型に特殊化
template<>
inline const char* toStr<bool>(const bool value)
{
	return value ? "true" : "false";
}
template<>
inline const char* toNumStr<bool>(char* buff, const bool value)
{
	return strcpy_fast(buff, value ? "1" : "0");
}
template<>
inline const char* toHexStr<bool>(char* buff, const bool value)
{
	return strcpy_fast(buff, value ? "1" : "0");
}
template<>
inline const char* toByteStr<bool>(char* buff, const bool value)
{
	buff[0] = value;
	buff[1] = '\0';
	return buff;
}
//※char型に特殊化
template<>
inline const char* toStr<char>(char* buff, const char value)
{
	return toByteStr(buff, value);
}
//※unsinged char型に特殊化
template<>
inline const char* toStr<unsigned char>(char* buff, const unsigned char value)
{
	return toNumStr(buff, value);
}
//※short型に特殊化
template<>
inline const char* toStr<short>(char* buff, const short value)
{
	return toNumStr(buff, value);
}
//※unsinged short型に特殊化
template<>
inline const char* toStr<unsigned short>(char* buff, const unsigned short value)
{
	return toNumStr(buff, value);
}
//※int型に特殊化
template<>
inline const char* toStr<int>(char* buff, const int value)
{
	return toNumStr(buff, value);
}
//※unsinged int型に特殊化
template<>
inline const char* toStr<unsigned int>(char* buff, const unsigned int value)
{
	return toNumStr(buff, value);
}
//※long型に特殊化
template<>
inline const char* toStr<long>(char* buff, const long value)
{
	return toNumStr(buff, value);
}
//※unsinged long型に特殊化
template<>
inline const char* toStr<unsigned long>(char* buff, const unsigned long value)
{
	return toNumStr(buff, value);
}
//※long long型に特殊化
template<>
inline const char* toStr<long long>(char* buff, const long long value)
{
	return toNumStr(buff, value);
}
//※unsinged long long型に特殊化
template<>
inline const char* toStr<unsigned long long>(char* buff, const unsigned long long value)
{
	return toNumStr(buff, value);
}
//※float型に特殊化
template<>
inline const char* toStr<float>(char* buff, const float value)
{
	return toNumStr(buff, value);
}
//※double型に特殊化
template<>
inline const char* toStr<double>(char* buff, const double value)
{
	return toNumStr(buff, value);
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_TYPE_TRAITS_INL

// End of file

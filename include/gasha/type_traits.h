#pragma once
#ifndef GASHA_INCLUDED_TYPE_TRAITS_H
#define GASHA_INCLUDED_TYPE_TRAITS_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ】
// type_traits.h
// 型特性ユーティリティ【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <type_traits>//C++11 type_traits
#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//多くの型情報は #include <type_traits> で扱えます
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//配列型情報
//--------------------------------------------------------------------------------

//----------------------------------------
//配列要素数／次元数／要素サイズ取得
//----------------------------------------
//※四次元配列まで対応。
//※std::extentと異なり、型ではなく変数を指定する。
//※std::extentと異なり、extentof()は全次元の合計要素数を返す。（std::extentは対象次元をテンプレート引数で指定。デフォルトは一次元目）
//----------------------------------------

//非配列用
template<typename T> inline constexpr std::size_t rankof(const T& data){ return 0; }
template<typename T> inline constexpr std::size_t sizeofelemof(const T& data){ return sizeof(T); }
template<typename T> inline constexpr std::size_t extentof(const T& data){ return 0; }
template<typename T> inline constexpr std::size_t extent1of(const T& data){ return 0; }
template<typename T> inline constexpr std::size_t extent2of(const T& data){ return 0; }
template<typename T> inline constexpr std::size_t extent3of(const T& data){ return 0; }
template<typename T> inline constexpr std::size_t extent4of(const T& data){ return 0; }
template<typename T> inline constexpr std::size_t extent5of(const T& data){ return 0; }

//一次配列用
template<typename T, std::size_t N1> inline constexpr std::size_t rankof(const T(&data)[N1]){ return 1; }
template<typename T, std::size_t N1> inline constexpr std::size_t sizeofelemof(const T(&data)[N1]){ return sizeof(T); }
template<typename T, std::size_t N1> inline constexpr std::size_t extentof(const T(&data)[N1]){ return N1; }
template<typename T, std::size_t N1> inline constexpr std::size_t extent1of(const T(&data)[N1]){ return N1; }
//template<typename T, std::size_t N1> inline constexpr std::size_t extent2of(const T(&data)[N1]){ return 0; }//※不要
//template<typename T, std::size_t N1> inline constexpr std::size_t extent3of(const T(&data)[N1]){ return 0; }//※不要
//template<typename T, std::size_t N1> inline constexpr std::size_t extent4of(const T(&data)[N1]){ return 0; }//※不要
//template<typename T, std::size_t N1> inline constexpr std::size_t extent5of(const T(&data)[N1]){ return 0; }//※不要

//二次配列用
template<typename T, std::size_t N1, std::size_t N2> inline constexpr std::size_t rankof(const T(&data)[N1][N2]){ return 2; }
template<typename T, std::size_t N1, std::size_t N2> inline constexpr std::size_t sizeofelemof(const T(&data)[N1][N2]){ return sizeof(T); }
template<typename T, std::size_t N1, std::size_t N2> inline constexpr std::size_t extentof(const T(&data)[N1][N2]){ return N1 * N2; }
//template<typename T, std::size_t N1, std::size_t N2> inline constexpr std::size_t extent1of(const T(&data)[N1][N2]){ return N1; }//※不要
template<typename T, std::size_t N1, std::size_t N2> inline constexpr std::size_t extent2of(const T(&data)[N1][N2]){ return N2; }
//template<typename T, std::size_t N1, std::size_t N2> inline constexpr std::size_t extent3of(const T(&data)[N1][N2]){ return 0; }//※不要
//template<typename T, std::size_t N1, std::size_t N2> inline constexpr std::size_t extent4of(const T(&data)[N1][N2]){ return 0; }//※不要
//template<typename T, std::size_t N1, std::size_t N2> inline constexpr std::size_t extent5of(const T(&data)[N1][N2]){ return 0; }//※不要

//三次配列用
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline constexpr std::size_t rankof(const T(&data)[N1][N2][N3]){ return 3; }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline constexpr std::size_t sizeofelemof(const T(&data)[N1][N2][N3]){ return sizeof(T); }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline constexpr std::size_t extentof(const T(&data)[N1][N2][N3]){ return N1 * N2 * N3; }
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline constexpr std::size_t extent1of(const T(&data)[N1][N2][N3]){ return N1; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline constexpr std::size_t extent2of(const T(&data)[N1][N2][N3]){ return N2; }//※不要
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline constexpr std::size_t extent3of(const T(&data)[N1][N2][N3]){ return N3; }
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline constexpr std::size_t extent4of(const T(&data)[N1][N2][N3]){ return 0; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline constexpr std::size_t extent5of(const T(&data)[N1][N2][N3]){ return 0; }//※不要

//四次配列用
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline constexpr std::size_t rankof(const T(&data)[N1][N2][N3][N4]){ return 4; }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline constexpr std::size_t sizeofelemof(const T(&data)[N1][N2][N3][N4]){ return sizeof(T); }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline constexpr std::size_t extentof(const T(&data)[N1][N2][N3][N4]){ return N1 * N2 * N3 * N4; }
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline constexpr std::size_t extent1of(const T(&data)[N1][N2][N3][N4]){ return N1; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline constexpr std::size_t extent2of(const T(&data)[N1][N2][N3][N4]){ return N2; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline constexpr std::size_t extent3of(const T(&data)[N1][N2][N3][N4]){ return N3; }//※不要
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline constexpr std::size_t extent4of(const T(&data)[N1][N2][N3][N4]){ return N4; }
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline constexpr std::size_t extent5of(const T(&data)[N1][N2][N3][N4]){ return 0; }

//五次配列用
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> inline constexpr std::size_t rankof(const T(&data)[N1][N2][N3][N4][N5]){ return 5; }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> inline constexpr std::size_t sizeofelemof(const T(&data)[N1][N2][N3][N4][N5]){ return sizeof(T); }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> inline constexpr std::size_t extentof(const T(&data)[N1][N2][N3][N4][N5]){ return N1 * N2 * N3 * N4 * N5; }
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> inline constexpr std::size_t extent1of(const T(&data)[N1][N2][N3][N4][N5]){ return N1; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> inline constexpr std::size_t extent2of(const T(&data)[N1][N2][N3][N4][N5]){ return N2; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> inline constexpr std::size_t extent3of(const T(&data)[N1][N2][N3][N4][N5]){ return N3; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> inline constexpr std::size_t extent4of(const T(&data)[N1][N2][N3][N4][N5]){ return N4; }//※不要
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> inline constexpr std::size_t extent5of(const T(&data)[N1][N2][N3][N4][N5]){ return N5; }

//※constexprが使えない環境でのメタプログラミング用（extent*ofだけマクロで用意）
#define GASHA_EXTENT1OF(data) (sizeof(data) / sizeof(data[0]))
#define GASHA_EXTENT2OF(data) (sizeof(data[0]) / sizeof(data[0][0]))
#define GASHA_EXTENT3OF(data) (sizeof(data[0][0]) / sizeof(data[0][0][0]))
#define GASHA_EXTENT4OF(data) (sizeof(data[0][0][0]) / sizeof(data[0][0][0][0]))
#define GASHA_EXTENT5OF(data) (sizeof(data[0][0][0][0]) / sizeof(data[0][0][0][0][0]))

//--------------------------------------------------------------------------------
//ポインター型情報
//--------------------------------------------------------------------------------

//----------------------------------------
//ポインターでも実体でも値（クラス／構造体）のサイズを返す sizeof()
//----------------------------------------
template<typename T> inline constexpr std::size_t sizeofvalueof(T val){ return sizeof(T); }
template<typename T> inline constexpr std::size_t sizeofvalueof(T* val){ return sizeof(T); }

//--------------------------------------------------------------------------------
//クラス／構造体に比較演算子を自動定義するためのCRPTクラス
//--------------------------------------------------------------------------------
//使用方法：
//  struct derived : public operatorCRTP<derived, int>//テンプレート引数には、「自身の型」と「比較に用いる型」を指定する
//  {                                                 //※自身の型には、「比較に用いる型」へのキャストオペレータを実装しておく必要がある
//      operator int() const { return m_primaryData; }//int型にキャスト
//      int m_primaryData;//主要データ（比較に用いる）
//      int m_otherData;//他のデータ
//  };
template<class base, typename cast_type>
class operatorCRTP
{
public:
	friend inline bool operator==(const base& lhs, const base& rhs){ return static_cast<cast_type>(lhs) == static_cast<cast_type>(rhs); }
	friend inline bool operator!=(const base& lhs, const base& rhs){ return static_cast<cast_type>(lhs) != static_cast<cast_type>(rhs); }
	friend inline bool operator<(const base& lhs, const base& rhs){ return static_cast<cast_type>(lhs) < static_cast<cast_type>(rhs); }
	friend inline bool operator>(const base& lhs, const base& rhs){ return static_cast<cast_type>(lhs) > static_cast<cast_type>(rhs); }
	friend inline bool operator<=(const base& lhs, const base& rhs){ return static_cast<cast_type>(lhs) <= static_cast<cast_type>(rhs); }
	friend inline bool operator>=(const base& lhs, const base& rhs){ return static_cast<cast_type>(lhs) >= static_cast<cast_type>(rhs); }
};

//--------------------------------------------------------------------------------
//汎用ユニオン
//--------------------------------------------------------------------------------

//----------------------------------------
//汎用ユニオン
//※（現状）8バイト
static const std::size_t unionTypesSize = 8;// sizeof(long long);
union unionTypes
{
	char m_char[unionTypesSize / sizeof(char)];
	unsigned char m_uchar[unionTypesSize / sizeof(unsigned char)];
	short m_short[unionTypesSize / sizeof(short)];
	unsigned short m_ushort[unionTypesSize / sizeof(unsigned short)];
	int m_int[unionTypesSize / sizeof(int)];
	unsigned int m_uint[unionTypesSize / sizeof(unsigned int)];
	long m_long[unionTypesSize / sizeof(long)];
	unsigned long m_ulong[unionTypesSize / sizeof(unsigned long)];
	long long m_llong[unionTypesSize / sizeof(long long)];
	unsigned long long m_ullong[unionTypesSize / sizeof(unsigned long long)];
	float m_float[unionTypesSize / sizeof(float)];
	double m_double[unionTypesSize / sizeof(double)];
	inline void clear();
	inline unionTypes(const char value);
	inline unionTypes(const unsigned char value);
	inline unionTypes(const short value);
	inline unionTypes(const unsigned short value);
	inline unionTypes(const int value);
	inline unionTypes(const unsigned int value);
	inline unionTypes(const long value);
	inline unionTypes(const unsigned long value);
	inline unionTypes(const long long value);
	inline unionTypes(const unsigned long long value);
	inline unionTypes(const float value);
	inline unionTypes(const double value);
};

//--------------------------------------------------------------------------------
//文字列化
//--------------------------------------------------------------------------------

//----------------------------------------
//値を文字列に変換
template<typename T> inline const char* toStr(const T value);//値に対応する固定文字列取得（bool型のみ対応）
template<typename T> inline const char* toStr(char* buff, const T value);//文字列化 ※型に応じたデフォルトの文字列化。
template<typename T> inline const char* toNumStr(char* buff, const T value);//10進数文字列化
template<typename T> inline const char* toHexStr(char* buff, const T value);//16進数文字列化 ※頭に "0x" が付く。
template<typename T> inline const char* toByteStr(char* buff, const T value);//バイト文字列化 ※基本的にそのままの値のコピー。ターミネータが付く。
//※bool型に特殊化
template<>
inline const char* toStr<bool>(const bool value);
template<>
inline const char* toNumStr<bool>(char* buff, const bool value);
template<>
inline const char* toHexStr<bool>(char* buff, const bool value);
template<>
inline const char* toByteStr<bool>(char* buff, const bool value);
//※nullptr_t型に特殊化
template<>
inline const char* toStr<std::nullptr_t>(const std::nullptr_t value);
template<>
inline const char* toNumStr<std::nullptr_t>(char* buff, const std::nullptr_t value);
template<>
inline const char* toHexStr<std::nullptr_t>(char* buff, const std::nullptr_t value);
template<>
inline const char* toByteStr<std::nullptr_t>(char* buff, const std::nullptr_t value);
//※char型に特殊化
template<>
inline const char* toStr<char>(char* buff, const char value);
template<>
const char* toNumStr<char>(char* buff, const char value);
template<>
const char* toHexStr<char>(char* buff, const char value);
template<>
const char* toByteStr<char>(char* buff, const char value);
//※unsinged char型に特殊化
template<>
inline const char* toStr<unsigned char>(char* buff, const unsigned char value);
template<>
const char* toNumStr<unsigned char>(char* buff, const unsigned char value);
template<>
const char* toHexStr<unsigned char>(char* buff, const unsigned char value);
template<>
const char* toByteStr<unsigned char>(char* buff, const unsigned char value);
//※short型に特殊化
template<>
inline const char* toStr<short>(char* buff, const short value);
template<>
const char* toNumStr<short>(char* buff, const short value);
template<>
const char* toHexStr<short>(char* buff, const short value);
template<>
const char* toByteStr<short>(char* buff, const short value);
//※unsinged short型に特殊化
template<>
inline const char* toStr<unsigned short>(char* buff, const unsigned short value);
template<>
const char* toNumStr<unsigned short>(char* buff, const unsigned short value);
template<>
const char* toHexStr<unsigned short>(char* buff, const unsigned short value);
template<>
const char* toByteStr<unsigned short>(char* buff, const unsigned short value);
//※int型に特殊化
template<>
inline const char* toStr<int>(char* buff, const int value);
template<>
const char* toNumStr<int>(char* buff, const int value);
template<>
const char* toHexStr<int>(char* buff, const int value);
template<>
const char* toByteStr<int>(char* buff, const int value);
//※unsinged int型に特殊化
template<>
inline const char* toStr<unsigned int>(char* buff, const unsigned int value);
template<>
const char* toNumStr<unsigned int>(char* buff, const unsigned int value);
template<>
const char* toHexStr<unsigned int>(char* buff, const unsigned int value);
template<>
const char* toByteStr<unsigned int>(char* buff, const unsigned int value);
//※long型に特殊化
template<>
inline const char* toStr<long>(char* buff, const long value);
template<>
const char* toNumStr<long>(char* buff, const long value);
template<>
const char* toHexStr<long>(char* buff, const long value);
template<>
const char* toByteStr<long>(char* buff, const long value);
//※unsinged long型に特殊化
template<>
inline const char* toStr<unsigned long>(char* buff, const unsigned long value);
template<>
const char* toNumStr<unsigned long>(char* buff, const unsigned long value);
template<>
const char* toHexStr<unsigned long>(char* buff, const unsigned long value);
template<>
const char* toByteStr<unsigned long>(char* buff, const unsigned long value);
//※long long型に特殊化
template<>
inline const char* toStr<long long>(char* buff, const long long value);
template<>
const char* toNumStr<long long>(char* buff, const long long value);
template<>
const char* toHexStr<long long>(char* buff, const long long value);
template<>
const char* toByteStr<long long>(char* buff, const long long value);
//※unsinged long long型に特殊化
template<>
inline const char* toStr<unsigned long long>(char* buff, const unsigned long long value);
template<>
const char* toNumStr<unsigned long long>(char* buff, const unsigned long long value);
template<>
const char* toHexStr<unsigned long long>(char* buff, const unsigned long long value);
template<>
const char* toByteStr<unsigned long long>(char* buff, const unsigned long long value);
//※float型に特殊化
template<>
inline const char* toStr<float>(char* buff, const float value);
template<>
const char* toNumStr<float>(char* buff, const float value);
template<>
const char* toHexStr<float>(char* buff, const float value);
template<>
const char* toByteStr<float>(char* buff, const float value);
//※double型に特殊化
template<>
inline const char* toStr<double>(char* buff, const double value);
template<>
const char* toNumStr<double>(char* buff, const double value);
template<>
const char* toHexStr<double>(char* buff, const double value);
template<>
const char* toByteStr<double>(char* buff, const double value);

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/type_traits.inl>

#endif//GASHA_INCLUDED_TYPE_TRAITS_H

// End of file

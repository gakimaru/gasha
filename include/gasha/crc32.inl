#pragma once
#ifndef GASHA_INCLUDED_CRC32_INL
#define GASHA_INCLUDED_CRC32_INL

//--------------------------------------------------------------------------------
// crc32.inl
// CRC32計算【インライン関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/crc32.h>//CRC32計算【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------
//メタプログラミング用（コンパイル時計算用）
namespace _private
{
	//--------------------
	//CRC3232生成多項式マクロと定数を定義
	//※constexpr関数内では変数/定数が使えないため、定数にマクロを使用
#ifndef GASHA_CRC32_IS_CRC32C
	//IEEE勧告の標準的なCRC32の生成多項式
	//#define _POLYNOMIAL 0x04c11db7u//（標準）
	#define _POLYNOMIAL 0xedb88320u//（反転）
#else//GASHA_CRC32_IS_CRC32C
	//CRC-32C(Castagnoli)の生成多項式
	//#define _POLYNOMIAL 0x1edc6f41u//（標準）
	#define _POLYNOMIAL 0x82f63b78u//（反転）
#endif//GASHA_CRC32_IS_CRC32C
	static const crc32_t POLYNOMIAL = _POLYNOMIAL;//CRC32生成多項式定数

	//--------------------
	//CRC32多項式計算
	//※constexpr関数内ではラムダ式が使えないため、関数を分割（ラムダ式を使用するとコンパイル時に評価されなくなる）
	constexpr inline GASHA_ crc32_t calcPoly_core(const GASHA_ crc32_t poly)
	{
		//多項式計算
		return poly & 1 ? _POLYNOMIAL ^ (poly >> 1) : (poly >> 1);
	}
	constexpr inline GASHA_ crc32_t calcPoly(const GASHA_ crc32_t poly)
	{
		//多項式計算
		return calcPoly_core(
		         calcPoly_core(
		           calcPoly_core(
		             calcPoly_core(
		               calcPoly_core(
		                 calcPoly_core(
		                   calcPoly_core(
		                     calcPoly_core(poly)
		                   )
		                 )
		               )
		             )
		           )
		         )
		       );
	}

	//--------------------
	//CRC32生成多項式マクロを破棄
	#undef POLYNO_POLYNOMIALMIAL
	
	//--------------------
	//文字列からCRC算出用（再帰処理）
	//※constexpr関数内ではSSE命令に非対応（使用するとコンパイル時に評価されなくなる）
	constexpr inline GASHA_ crc32_t calcStr(const GASHA_ crc32_t crc, const char* str)
	{
		return *str == '\0' ? crc : calcStr(calcPoly(static_cast<GASHA_ crc32_t>((crc ^ *str) & 0xffu)) ^ (crc >> 8), str + 1);//CRC多項式(生成多項式から計算)を合成
	}
	//--------------------
	//データ長を指定してCRC算出用（再帰処理）
	constexpr inline GASHA_ crc32_t calcData(const GASHA_ crc32_t crc, const char* data, const std::size_t len)
	{
		return len == 0 ? crc : calcData(calcPoly(static_cast<crc32_t>((crc ^ *data) & 0xffu)) ^ (crc >> 8), data + 1, len - 1);//CRC多項式(生成多項式から計算)を合成
	}
}//namespace _private

//--------------------
//【メタプログラミング：constexpr版】文字列から算出
constexpr inline crc32_t calcStaticCRC32(const char* str)
{
	return ~_private::calcStr(~0u, str);
}
//--------------------
//【メタプログラミング：constexpr版】バイナリデータから算出
constexpr inline crc32_t calcStaticCRC32(const char* data, const std::size_t len)
{
	return ~_private::calcData(~0u, data, len);
}
#ifdef GASHA_HAS_USER_DEFINED_LITERAL
//--------------------
//【メタプログラミング：ユーザー定義リテラル版】（基本的に）文字列から算出
constexpr inline crc32_t operator "" _crc32(const char* str, const std::size_t len)
{
	return calcStaticCRC32(str, len);
}
#endif//GASHA_HAS_USER_DEFINED_LITERAL

//--------------------
//【ランタイム関数版】文字列から算出
inline crc32_t calcCRC32(const char* str)
{
#ifdef GASHA_CRC32_USE_SSE
	return calcCRC32_sse(str);
#else//GASHA_CRC32_USE_SSE
#ifdef GASHA_CRC32_USE_STATIC_TABLE
	return calcCRC32_table(str);
#else//GASHA_CRC32_USE_STATIC_TABLE
	return calcCRC32_loop(str);
#endif//GASHA_CRC32_USE_STATIC_TABLE
#endif//GASHA_CRC32_USE_SSE
}
//--------------------
//【ランタイム関数版】バイナリデータから算出
inline crc32_t calcCRC32(const char* data, const std::size_t len)
{
#ifdef GASHA_CRC32_USE_SSE
	return calcCRC32_sse(data, len);
#else//GASHA_CRC32_USE_SSE
#ifdef GASHA_CRC32_USE_STATIC_TABLE
	return calcCRC32_table(data, len);
#else//GASHA_CRC32_USE_STATIC_TABLE
	return calcCRC32_loop(data, len);
#endif//GASHA_CRC32_USE_STATIC_TABLE
#endif//GASHA_CRC32_USE_SSE
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_CRC32_INL

// End of file

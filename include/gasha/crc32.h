#pragma once
#ifndef __CRC32_H_
#define __CRC32_H_

//--------------------------------------------------------------------------------
// crc32.h
// CRC32計算
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t

#if defined(GASHA_USE_SSE4_2) && defined(GASHA_CRC32_IS_CRC32C)
#define CALC_RUNTIME_CRC32_BY_SSE//ランタイムCRC32の算出にSSE命令を使用する（CRC-32Cを算出する）
#endif

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------
//型
typedef std::uint32_t crc32_t;//CRC32型

//--------------------
//メタプログラミング用（コンパイル時計算用）
namespace _private//直接使用しない処理を隠ぺいするためのネームスペース
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
	//※constexpr関数内ではSSE命令に非対応（使用するとコンパイル時に評価されなくなる）
	constexpr inline GASHA_ crc32_t calcData(const GASHA_ crc32_t crc, const char* data, const std::size_t len)
	{
		return len == 0 ? crc : calcData(calcPoly(static_cast<crc32_t>((crc ^ *data) & 0xffu)) ^ (crc >> 8), data + 1, len - 1);//CRC多項式(生成多項式から計算)を合成
	}
}//namespace _private

//--------------------
//【constexpr版】文字列からCRC算出
constexpr inline crc32_t calcConstCRC32(const char* str)
{
	return ~_private::calcStr(~0u, str);
}
//--------------------
//【constexpr版】データ長を指定してCRC算出
constexpr inline crc32_t calcConstCRC32(const char* data, const std::size_t len)
{
	return ~_private::calcData(~0u, data, len);
}
#ifdef GASHA_HAS_USER_DEFINED_LITERAL
//--------------------
//【ユーザー定義リテラル版】文字列と文字列長を指定してCRC算出
//※operator "" の後に空白が必要なことに注意
constexpr inline crc32_t operator "" _crc32(const char* str, const std::size_t len)
{
	return calcConstCRC32(str, len);
}
#endif//GASHA_HAS_USER_DEFINED_LITERAL

//--------------------
//【ランタイム関数版】各種CRC32計算
//※いずれも、通常直接使用しない関数
crc32_t calcCRC32_recursive(const char* str);//再帰処理版
crc32_t calcCRC32_recursive(const char* data, const std::size_t len);//再帰処理版
crc32_t calcCRC32_loop(const char* str);//ループ処理版
crc32_t calcCRC32_loop(const char* data, const std::size_t len);//ループ処理版
#ifdef GASHA_CRC32_USE_STATIC_TABLE
crc32_t calcCRC32_table(const char* str);//事前計算済み多項式テーブル処理版
crc32_t calcCRC32_table(const char* data, const std::size_t len);//事前計算済み多項式テーブル処理版
#endif//GASHA_CRC32_USE_STATIC_TABLE
#ifdef GASHA_USE_SSE4_2
crc32_t calcCRC32_sse(const char* str);//SSE命令版
crc32_t calcCRC32_sse(const char* data, const std::size_t len);//SSE命令版
#endif//GASHA_USE_SSE4_2

//--------------------
//【ランタイム関数版】文字列のCRC32計算
inline crc32_t calcCRC32(const char* str)
{
#ifdef CALC_RUNTIME_CRC32_BY_SSE
	return calcCRC32_sse(str);
#else//CALC_RUNTIME_CRC32_BY_SSE
#ifdef GASHA_CRC32_USE_STATIC_TABLE
	return calcCRC32_table(str);
#else//GASHA_CRC32_USE_STATIC_TABLE
	return calcCRC32_loop(str);
#endif//GASHA_CRC32_USE_STATIC_TABLE
#endif//CALC_RUNTIME_CRC32_BY_SSE
}
//--------------------
//【ランタイム関数版】指定長データのCRC32計算
inline crc32_t calcCRC32(const char* data, const std::size_t len)
{
#ifdef CALC_RUNTIME_CRC32_BY_SSE
	return calcCRC32_sse(data, len);
#else//CALC_RUNTIME_CRC32_BY_SSE
#ifdef GASHA_CRC32_USE_STATIC_TABLE
	return calcCRC32_table(data, len);
#else//GASHA_CRC32_USE_STATIC_TABLE
	return calcCRC32_loop(data, len);
#endif//GASHA_CRC32_USE_STATIC_TABLE
#endif//CALC_RUNTIME_CRC32_BY_SSE
}

//--------------------
//【プログラム作成補助処理】CRC多項式テーブルの作成と表示
//※ソースコードを生成して標準出力に出力する。
void makeAndPrintPolyTable();

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__CRC32_H_

// End of file

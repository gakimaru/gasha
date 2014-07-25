#pragma once
#ifndef GASHA_INCLUDED_CRC32_H
#define GASHA_INCLUDED_CRC32_H

//--------------------------------------------------------------------------------
// crc32.h
// CRC32計算【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/limits.h>//限界値

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------
//型
typedef std::uint32_t crc32_t;//CRC32型

//--------------------
//【メタプログラミング：constexpr版】CRC32算出
//※constexpr対応コンパイラなら、コンパイル時にCRCを計算できる。
//　（例）const crc32_t text_crc = calcStaticCRC32("text");//※変数にconstまたはconstexprを付けないと、コンパイル時に処理されないので注意（その場合、ランタイム処理になる）
inline constexpr crc32_t calcStaticCRC32(const char* str);//文字列から算出
inline constexpr crc32_t calcStaticCRC32(const char* data, const std::size_t len);//バイナリデータから算出

#ifdef GASHA_HAS_USER_DEFINED_LITERAL
//--------------------
//【メタプログラミング：ユーザー定義リテラル版】
//※ユーザー定義リテラル対応コンパイラなら、コンパイル時にCRCを計算できる。
//　（例）const crc32_t text_crc = "text"_crc32;//※変数にconstまたはconstexprを付けないと、コンパイル時に処理されないので注意（その場合、ランタイム処理になる）
//※ユーザー定義リテラルの宣言では、operator "" の後に空白が必要なことに注意
inline constexpr crc32_t operator "" _crc32(const char* str, const std::size_t len);//（基本的に）文字列から算出
#endif//GASHA_HAS_USER_DEFINED_LITERAL

//--------------------
//【ランタイム関数版】
//※処理方法は、コンパイル時の設定に応じて、SSE命令版、事前計算済みテーブル版、ループ処理版のいずれかが適用される。
inline crc32_t calcCRC32(const char* str);//文字列から算出
inline crc32_t calcCRC32(const char* data, const std::size_t len);//バイナリデータから算出

//--------------------
//【ランタイム関数版】※処理方法別の個別関数
//通常版：ループ処理版
crc32_t calcCRC32_loop(const char* str);//文字列から算出
crc32_t calcCRC32_loop(const char* data, const std::size_t len);//バイナリデータから算出
//通常版：再帰処理版 ※メタプログラミング版処理動作確認用
crc32_t calcCRC32_recursive(const char* str);//文字列から算出
crc32_t calcCRC32_recursive(const char* data, const std::size_t len);//バイナリデータから算出
//事前計算済み多項式テーブル版
//※事前計算済みテーブルが使用できない場合でも、関数は実行可能。その場合、通常版として処理する。
crc32_t calcCRC32_table(const char* str);//文字列から算出
crc32_t calcCRC32_table(const char* data, const std::size_t len);//バイナリデータから算出
//SSE命命令版
//※SSE命令が使用できない場合でも、関数は実行可能。その場合、事前計算済みテーブル版、もしくは、通常版として処理する。
crc32_t calcCRC32_sse(const char* str);//文字列から算出
crc32_t calcCRC32_sse(const char* data, const std::size_t len);//バイナリデータから算出

//--------------------
//【プログラム作成補助処理】CRC多項式テーブルの作成と表示（事前生成済みテーブル作成用）
//※標準出力に出力されたテキストをコピペしてソースコードに適用する。
void makeAndPrintPolyTable();

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/crc32.inl>

#endif//GASHA_INCLUDED_CRC32_H

// End of file

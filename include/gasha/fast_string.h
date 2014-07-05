#pragma once
#ifndef GASHA_INCLUDED_FAST_STRING_H
#define GASHA_INCLUDED_FAST_STRING_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// fast_string.h
// 高速文字列処理【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstddef>//std::size_t
#include <string.h>//strlen(), strcmp(), strncmp(), strchr(), strrchr(), strstr(), strcpy(), strncpy()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <functional> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <functional>//C++11 std::function

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//strlen
inline std::size_t strlen_fast(const char* str);//高速版
//std::size_t strlen(const char* str);//通常版
std::size_t strlen_sse(const char* str);//SSE版

//----------------------------------------
//strnlen
//※最大長までの長さを計測
inline std::size_t strnlen_fast(const char* str, const std::size_t max_len);//高速版
#ifdef GASHA_IS_GCC
std::size_t strnlen(const char* str, const std::size_t max_len);//通常版
#endif//GASHA_IS_GCC
std::size_t strnlen_sse(const char* str, const std::size_t max_len);//SSE版

//----------------------------------------
//strcmp
inline int strcmp_fast(const char* str1, const char* str2);//高速版
//int strcmp(const char* str1, const char* str2);//通常版
int strcmp_sse(const char* str1, const char* str2);//SSE版

//----------------------------------------
//高速版strncmp
inline int strncmp_fast(const char* str1, const char* str2, const std::size_t max_len);//高速版
//int strncmp(const char* str1, const char* str2, const std::size_t max_len);//通常版
int strncmp_sse(const char* str1, const char* str2, const std::size_t max_len);//SSE版

//----------------------------------------
//高速版strchr
inline const char* strchr_fast(const char* str, const char c);//高速版
//const char* strchr(const char* str, const char c);//通常版
const char* strchr_sse(const char* str, const char c);//SSE版

//----------------------------------------
//高速版strrchr
inline const char* strrchr_fast(const char* str, const char c);//高速版
//const char* strrchr(const char* str, const char c);//通常版
const char* strrchr_sse(const char* str, const char c);//SSE版

//----------------------------------------
//高速版strstr
inline const char* strstr_fast(const char* str, const char* pattern);//高速版
//const char* strstr(const char* str, const char* pattern);//通常版
const char* strstr_sse(const char* str, const char* pattern);//SSE版

//----------------------------------------
//高速版strstr(BM法)
//※BM法（Boyer-Moore法）バージョン
//※文字列が長い時（とくにパターンが長い時）には有利なアルゴリズムだが、
//　短い場合は、スキップ文字数を事前計算する分遅くなる。
//※コールバック処理を渡し、探索の続行を判定可能。
//  コールバック処理プロトタイプ：
//    bool found_it(const char* found, const char* str);
//      ・found ... 見付かった文字列
//      ・str   ... 元の文字列 ※見付かった位置を算出する場合は、found - str を計算
//      戻り値  ... trueで探索終了、falseで探索続行
inline const char* strstrbm_fast(const char* str, const char* pattern, std::function<bool(const char*, const char*)> found_it);//高速版
const char* strstrbm(const char* str, const char* pattern, std::function<bool(const char*, const char*)> found_it);//通常版
const char* strstrbm_sse(const char* str, const char* pattern, std::function<bool(const char*, const char*)> found_it);//SSE版

//----------------------------------------
//高速版strstr0
//※strの先頭からのみpatternを照合する
//※strがpatternより長くても、patternの全文が先頭にあれば一致と見なす。
inline const char* strstr0_fast(const char* str, const char* pattern);//高速版
const char* strstr0(const char* str, const char* pattern);//通常版
const char* strstr0_sse(const char* str, const char* pattern);//SSE版

//----------------------------------------
//高速版strcpy
inline const char* strcpy_fast(char* dst, const char* src);//高速版
//const char* strcpy(char* dst, const char* src);//通常版
const char* strcpy_sse(char* dst, const char* src);//SSE版

//----------------------------------------
//高速版strncpy
inline const char* strncpy_fast(char* dst, const char* src, const std::size_t max_len);//高速版
//const char* strncpy(char* dst, const char* src, const std::size_t max_len);//通常版
const char* strncpy_sse(char* dst, const char* src, const std::size_t max_len);//SSE版

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルードする
#ifdef GASHA_FAST_STRING_ALLWAYS_TOGETHER_INL
#include <gasha/fast_string.inl>
#endif//GASHA_FAST_STRING_ALLWAYS_TOGETHER_INL

#endif//GASHA_INCLUDED_FAST_STRING_H

// End of file

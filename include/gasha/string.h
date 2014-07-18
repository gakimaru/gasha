#pragma once
#ifndef GASHA_INCLUDED_STRING_H
#define GASHA_INCLUDED_STRING_H

//--------------------------------------------------------------------------------
// string.h
// 文字列処理【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstddef>//std::size_t

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <functional> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <functional>//C++11 std::function

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//文字列処理
//--------------------------------------------------------------------------------
//【注意】UTF-16, UTF-32（通常文字の文字コードに'\0'が含まれるもの）には非対応
//--------------------------------------------------------------------------------

//----------------------------------------
//strlen
#ifdef GASHA_USE_NAME_SPACE
inline std::size_t strlen(const char* str);
#endif//GASHA_USE_NAME_SPACE

//----------------------------------------
//strnlen
//※最大長までの長さを計測
#if !defined(GASHA_STDSTRNLENFUNC)
inline std::size_t strnlen(const char* str, const std::size_t max_len);
#else//GASHA_STDSTRNLENFUNC
#ifdef GASHA_USE_NAME_SPACE
inline std::size_t strnlen(const char* str, const std::size_t max_len);
#endif//GASHA_USE_NAME_SPACE
#endif//GASHA_STDSTRNLENFUNC

//----------------------------------------
//strcmp
#ifdef GASHA_USE_NAME_SPACE
inline int strcmp(const char* str1, const char* str2);
#endif//GASHA_USE_NAME_SPACE

//----------------------------------------
//strncmp
#ifdef GASHA_USE_NAME_SPACE
inline int strncmp(const char* str1, const char* str2, const std::size_t max_len);
#endif//GASHA_USE_NAME_SPACE

//----------------------------------------
//strchr
#ifdef GASHA_USE_NAME_SPACE
inline const char* strchr(const char* str, const char c);
#endif//GASHA_USE_NAME_SPACE

//----------------------------------------
//strrchr
#ifdef GASHA_USE_NAME_SPACE
inline const char* strrchr(const char* str, const char c);
#endif//GASHA_USE_NAME_SPACE

//----------------------------------------
//strstr
#ifdef GASHA_USE_NAME_SPACE
inline const char* strstr(const char* str, const char* pattern);
#endif//GASHA_USE_NAME_SPACE

//----------------------------------------
//strstr(BM法)
//※BM法（Boyer-Moore法）バージョン
//※文字列が長い時（とくにパターンが長い時）には有利なアルゴリズムだが、
//　短い場合は、スキップ文字数を事前計算する分遅くなる。
//※コールバック処理を渡し、探索の続行を判定可能。
//  コールバック処理プロトタイプ：
//    bool found_it(const char* found, const char* str);
//      ・found ... 見付かった文字列
//      ・str   ... 元の文字列 ※見付かった位置を算出する場合は、found - str を計算
//      戻り値  ... trueで探索終了、falseで探索続行
const char* strstrbm(const char* str, const char* pattern, std::function<bool(const char*, const char*)> found_it);

//----------------------------------------
//strstr0
//※strの先頭からのみpatternを照合する
//※strがpatternより長くても、patternの全文が先頭にあれば一致と見なす。
inline const char* strstr0(const char* str, const char* pattern);

//----------------------------------------
//strcpy
#ifdef GASHA_USE_NAME_SPACE
inline const char* strcpy(char* dst, const char* src);
#endif//GASHA_USE_NAME_SPACE

//----------------------------------------
//strncpy
#ifdef GASHA_USE_NAME_SPACE
inline const char* strncpy(char* dst, const char* src, const std::size_t max_len);
#endif//GASHA_USE_NAME_SPACE

//----------------------------------------
//sprintf
#ifdef GASHA_USE_NAME_SPACE
template<typename... Tx>
inline int sprintf(char* dst, const char* fmt, Tx&&... args);
#endif//GASHA_USE_NAME_SPACE
//【VC++】VC++の64bit版でsprintf()に可変長テンプレート引数でパラメータを渡すと、C4313が発生する
//関数名が sprintf じゃなければ大丈夫なため、下記 spprintf の出力位置指定なし版も用意

//----------------------------------------
//spprintf
//※変則sprintf/snprintf。2文字目の p は progressive の意。漸進的な書き込みを行うため。または「pos」指定版。
//・max_size には dst の最大サイズを指定（終端を必ず書き込むため、最大長 + 1 のサイズとする）
//・dst + pos の位置から出力
//・pos は終端位置に更新される
//・戻り値は、全文字書き込みできたら書き込んだ文字数を返す（終端を含まない長さ）
//・指定サイズを超える場合、書き込めるところまで書き込み、終端も必ず付けた上で、0 を返す（書き込んだ文字数は pos から判断できる）
template<typename... Tx>
int spprintf(char* dst, std::size_t& pos, const char* fmt, Tx&&... args);//最大バッファサイズ指定なし版
template<typename... Tx>
int spprintf(char* dst, const std::size_t max_size, std::size_t& pos, const char* fmt, Tx&&... args);//最大バッファサイズ指定あり版
template<typename... Tx>
int spprintf(char* dst, const char* fmt, Tx&&... args);//最大バッファサイズ／出力位置指定なし版

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/string.inl>

#endif//GASHA_INCLUDED_STRING_H

// End of file

#pragma once
#ifndef GASHA_INCLUDED_FAST_STRING_H
#define GASHA_INCLUDED_FAST_STRING_H

//--------------------------------------------------------------------------------
// fast_string.h
// 高速文字列処理【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstddef>//std::size_t

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <functional>//C++11 std::function
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//高速文字列処理
//--------------------------------------------------------------------------------
//【注意】UTF-16, UTF-32（通常文字の文字コードに'\0'が含まれるもの）には非対応
//--------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//高速版
//------------------------------------------------------------------------------
//※各高速版関数は、ビルド設定に応じて、通常版かSSE版を使用する。
//　基本的に、そのプラットフォームおよびコンパイラで最速の関数を
//　透過的に呼び出すため、高速版の関数だけを使用していれば良い。
//------------------------------------------------------------------------------

//----------------------------------------
//高速版strlen
inline std::size_t strlen_fast(const char* str);

//----------------------------------------
//高速版strnlen
//※最大長までの長さを計測
inline std::size_t strnlen_fast(const char* str, const std::size_t max_len);

//----------------------------------------
//高速版strcmp
inline int strcmp_fast(const char* str1, const char* str2);

//----------------------------------------
//高速版strncmp
inline int strncmp_fast(const char* str1, const char* str2, const std::size_t max_len);

//----------------------------------------
//高速版strchr
inline const char* strchr_fast(const char* str, const char c);

//----------------------------------------
//高速版strrchr
inline const char* strrchr_fast(const char* str, const char c);

//----------------------------------------
//高速版strstr
inline const char* strstr_fast(const char* str, const char* pattern);

//----------------------------------------
//高速版strstr(BM法)
//※BM法（Boyer-Moore法）バージョン
//※詳しい説明は string.h の通常版 strstrbm のヘッダ参照。
inline const char* strstrbm_fast(const char* str, const char* pattern, std::function<bool(const char*, const char*)> found_it);

//----------------------------------------
//高速版strstr0
//※strの先頭からのみpatternを照合する
//※strがpatternより長くても、patternの全文が先頭にあれば一致と見なす。
inline const char* strstr0_fast(const char* str, const char* pattern);

//----------------------------------------
//高速版strcpy
inline const char* strcpy_fast(char* dst, const char* src);

//----------------------------------------
//高速版strncpy
inline const char* strncpy_fast(char* dst, const char* src, const std::size_t max_len);

//------------------------------------------------------------------------------
//SSE版
//------------------------------------------------------------------------------

//----------------------------------------
//SSE版strlen
inline std::size_t strlen_sse(const char* str);

//----------------------------------------
//SSE版strnlen
//※最大長までの長さを計測
inline std::size_t strnlen_sse(const char* str, const std::size_t max_len);

//----------------------------------------
//SSE版strcmp
inline int strcmp_sse(const char* str1, const char* str2);

//----------------------------------------
//SSE版strncmp
inline int strncmp_sse(const char* str1, const char* str2, const std::size_t max_len);

//----------------------------------------
//SSE版strchr
inline const char* strchr_sse(const char* str, const char c);

//----------------------------------------
//SSE版strrchr
inline const char* strrchr_sse(const char* str, const char c);

//----------------------------------------
//SSE版strstr
inline const char* strstr_sse(const char* str, const char* pattern);

//----------------------------------------
//SSE版strstr(BM法)
//※BM法（Boyer-Moore法）バージョン
//※詳しい説明は string.h の通常版 strstrbm のヘッダ参照。
inline const char* strstrbm_sse(const char* str, const char* pattern, std::function<bool(const char*, const char*)> found_it);

//----------------------------------------
//SSE版strstr0
//※strの先頭からのみpatternを照合する
//※strがpatternより長くても、patternの全文が先頭にあれば一致と見なす。
inline const char* strstr0_sse(const char* str, const char* pattern);

//----------------------------------------
//SSE版strcpy
inline const char* strcpy_sse(char* dst, const char* src);

//----------------------------------------
//SSE版strncpy
inline const char* strncpy_sse(char* dst, const char* src, const std::size_t max_len);

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/fast_string.inl>

#endif//GASHA_INCLUDED_FAST_STRING_H

// End of file

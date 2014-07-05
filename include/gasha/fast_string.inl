#pragma once
#ifndef GASHA_INCLUDED_FAST_STRING_INL
#define GASHA_INCLUDED_FAST_STRING_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// fast_string.inl
// 高速文字列処理【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/fast_string.h>//高速文字列処理【宣言部】

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】str*** を使用すると、error C4996 が発生する
//  error C4996: 'str***': This function or variable may be unsafe. Consider using strncpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//高速版strlen
inline std::size_t strlen_fast(const char* str)
{
#ifdef GASHA_STRLEN_FAST_USE_SSE4_2
	return strlen_sse(str);
#else//GASHA_STRLEN_FAST_USE_SSE4_2
	return strlen(str);
#endif//GASHA_STRLEN_FAST_USE_SSE4_2
}

//----------------------------------------
//高速版strnlen
inline std::size_t strnlen_fast(const char* str, const std::size_t max_len)
{
#ifdef GASHA_STRNLEN_FAST_USE_SSE4_2
	return strnlen_sse(str, max_len);
#else//GASHA_STRNLEN_FAST_USE_SSE4_2
	return strnlen(str, max_len);
#endif//GASHA_STRNLEN_FAST_USE_SSE4_2
}

//----------------------------------------
//高速版strcmp
inline int strcmp_fast(const char* str1, const char* str2)
{
#ifdef GASHA_STRCMP_FAST_USE_SSE4_2
	return strcmp_sse(str1, str2);
#else//GASHA_STRCMP_FAST_USE_SSE4_2
	return strcmp(str1, str2);
#endif//GASHA_STRCMP_FAST_USE_SSE4_2
}

//----------------------------------------
//高速版strncmp
inline int strncmp_fast(const char* str1, const char* str2, const std::size_t max_len)
{
#ifdef GASHA_STRNCMP_FAST_USE_SSE4_2
	return strncmp_sse(str1, str2, max_len);
#else//GASHA_STRNCMP_FAST_USE_SSE4_2
	return strncmp(str1, str2, max_len);
#endif//GASHA_STRNCMP_FAST_USE_SSE4_2
}

//----------------------------------------
//高速版strchr
inline const char* strchr_fast(const char* str, const char c)
{
#ifdef GASHA_STRCHR_FAST_USE_SSE4_2
	return strchr_sse(str, c);
#else//GASHA_STRCHR_FAST_USE_SSE4_2
	return strchr(str, c);
#endif//GASHA_STRCHR_FAST_USE_SSE4_2
}

//----------------------------------------
//高速版strrchr
inline const char* strrchr_fast(const char* str, const char c)
{
#ifdef GASHA_STRRCHR_FAST_USE_SSE4_2
	return strrchr_sse(str, c);
#else//GASHA_STRRCHR_FAST_USE_SSE4_2
	return strrchr(str, c);
#endif//GASHA_STRRCHR_FAST_USE_SSE4_2
}

//----------------------------------------
//高速版strstr
inline const char* strstr_fast(const char* str, const char* pattern)
{
#ifdef GASHA_STRSTR_FAST_USE_SSE4_2
	return strstr_sse(str, pattern);
#else//GASHA_STRSTR_FAST_USE_SSE4_2
	return strstr(str, pattern);
#endif//GASHA_STRSTR_FAST_USE_SSE4_2
}

//----------------------------------------
//高速版strstr(BM法)
inline const char* strstrbm_fast(const char* str, const char* pattern, std::function<bool(const char*, const char*)> found_it)
{
#ifdef GASHA_STRSTRBM_FAST_USE_SSE4_2
	return strstrbm_sse(str, pattern, found_it);
#else//GASHA_STRSTRBM_FAST_USE_SSE4_2
	return strstrbm(str, pattern, found_it);
#endif//GASHA_STRSTRBM_FAST_USE_SSE4_2
}

//----------------------------------------
//高速版strstr0
inline const char* strstr0_fast(const char* str, const char* pattern)
{
#ifdef GASHA_STRSTR0_FAST_USE_SSE4_2
	return strstr0_sse(str, pattern);
#else//GASHA_STRSTR0_FAST_USE_SSE4_2
	return strstr0(str, pattern);
#endif//GASHA_STRSTR0_FAST_USE_SSE4_2
}

//----------------------------------------
//高速版strcpy
inline const char* strcpy_fast(char* dst, const char* src)
{
#ifdef GASHA_STRCPY_FAST_USE_SSE4_2
	return strcpy_sse(dst, src);
#else//GASHA_STRCPY_FAST_USE_SSE4_2
	return strcpy(dst, src);
#endif//GASHA_STRCPY_FAST_USE_SSE4_2
}

//----------------------------------------
//高速版strncpy
inline const char* strncpy_fast(char* dst, const char* src, const std::size_t max_len)
{
#ifdef GASHA_STRNCPY_FAST_USE_SSE4_2
	return strncpy_sse(dst, src, max_len);
#else//GASHA_STRNCPY_FAST_USE_SSE4_2
	return strncpy(dst, src, max_len);
#endif//GASHA_STRNCPY_FAST_USE_SSE4_2
}

//【VC++】ワーニング設定を復元
#pragma warning(pop)

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_FAST_STRING_INL

// End of file

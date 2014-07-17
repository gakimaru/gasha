#pragma once
#ifndef GASHA_INCLUDED_STRING_INL
#define GASHA_INCLUDED_STRING_INL

//--------------------------------------------------------------------------------
// string.inl
// 文字列処理【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/string.h>//文字列処理【宣言部】

#include <cstring>//std::strlen(), std::strcmp(), std::strncmp(), std::strchr(), std::strrchr(), std::strstr(), std::strcpy(), std::strncpy()
                  //::strnlen() ※VC++
#include <cstdio>//::snprintf() ※GCC, ::_snprintf_s() ※VC++
#include <utility>//C++11 std::forward

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】str*** を使用すると、error C4996 が発生する
//  error C4996: 'str***': This function or variable may be unsafe. Consider using strncpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//strlen
#ifdef GASHA_USE_NAME_SPACE
inline std::size_t strlen(const char* str)
{
	return std::strlen(str);
}
#endif//GASHA_USE_NAME_SPACE

//----------------------------------------
//strnlen
#if !defined(GASHA_STDSTRNLENFUNC)
inline std::size_t strnlen(const char* str, const std::size_t max_len)
{
	//nullチェックしない
	//	if (!str)
	//		return 0;
	const char* p_end = str + max_len;
	const char* p = str;
	while (p <= p_end)
	{
		if (!*p)
			return static_cast<std::size_t>(p - str);
		++p;
	}
	return max_len;
}
#else//GASHA_STDSTRNLENFUNC
#ifdef GASHA_USE_NAME_SPACE
inline std::size_t strnlen(const char* str, const std::size_t max_len)
{
	return GASHA_STDSTRNLENFUNC(str, max_len);
}
#endif//GASHA_USE_NAME_SPACE
#endif//GASHA_STDSTRNLENFUNC

#ifdef GASHA_USE_NAME_SPACE
//----------------------------------------
//strcmp
inline int strcmp(const char* str1, const char* str2)
{
	return std::strcmp(str1, str2);
}
#endif//GASHA_USE_NAME_SPACE

//----------------------------------------
//strncmp
#ifdef GASHA_USE_NAME_SPACE
inline int strncmp(const char* str1, const char* str2, const std::size_t max_len)
{
	return std::strncmp(str1, str2, max_len);
}
#endif//GASHA_USE_NAME_SPACE

#ifdef GASHA_USE_NAME_SPACE
//----------------------------------------
//strchr
inline const char* strchr(const char* str, const char c)
{
	return std::strchr(str, c);
}
#endif//GASHA_USE_NAME_SPACE

#ifdef GASHA_USE_NAME_SPACE
//----------------------------------------
//strrchr
inline const char* strrchr(const char* str, const char c)
{
	return std::strrchr(str, c);
}
#endif//GASHA_USE_NAME_SPACE

#ifdef GASHA_USE_NAME_SPACE
//----------------------------------------
//strstr
inline const char* strstr(const char* str, const char* pattern)
{
	return std::strstr(str, pattern);
}
#endif//GASHA_USE_NAME_SPACE

//----------------------------------------
//strstr0
inline const char* strstr0(const char* str, const char* pattern)
{
	const std::size_t pattern_len = GASHA_ strlen(pattern);
	const int ret = GASHA_ strncmp(str, pattern, pattern_len);
	if (ret == 0)
		return str;
	return nullptr;
}

#ifdef GASHA_USE_NAME_SPACE
//----------------------------------------
//strcpy
inline const char* strcpy(char* dst, const char* src)
{
	return std::strcpy(dst, src);
}
#endif//GASHA_USE_NAME_SPACE

#ifdef GASHA_USE_NAME_SPACE
//----------------------------------------
//strcpy
inline const char* strncpy(char* dst, const char* src, const std::size_t max_len)
{
	return std::strncpy(dst, src, max_len);
}
#endif//GASHA_USE_NAME_SPACE

#ifdef GASHA_USE_NAME_SPACE
//----------------------------------------
//sprintf
template<typename... Tx>
inline int sprintf(char* dst, const char* fmt, Tx&&... args)
{
	return std::sprintf(dst, fmt, std::forward<Tx>(args)...);
}
#endif//GASHA_USE_NAME_SPACE

//----------------------------------------
//spprintf
template<typename... Tx>
inline int spprintf(char* dst, std::size_t& pos, const char* fmt, Tx&&... args)
{
	int ret = std::sprintf(dst + pos, fmt, std::forward<Tx>(args)...);
	pos += ret;
	return ret;
}
template<typename... Tx>
int spprintf(char* dst, const std::size_t max_len, std::size_t& pos, const char* fmt, Tx&&... args)
{
	char* dst_now = dst + pos;
	const std::size_t remain = max_len - pos;
	int ret = 0;
#ifdef GASHA_IS_VC
	ret = ::_snprintf_s(dst_now, remain, remain - 1, fmt, std::forward<Tx>(args)...);
	if (ret < 0)
	{
		pos += (remain - 1);
		ret = 0;
	}
	else
		pos += ret;
#endif
#ifdef GASHA_IS_GCC
	ret = ::snprintf(dst_now, remain, fmt, std::forward<Tx>(args)...);
	if (ret >= remain)
	{
		pos += (remain - 1);
		ret = 0;
	}
	else
		pos += ret;
#endif
	return ret;
}
template<typename... Tx>
inline int spprintf(char* dst, const char* fmt, Tx&&... args)
{
	int ret = std::sprintf(dst, fmt, std::forward<Tx>(args)...);
	return ret;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_STRING_INL

// End of file

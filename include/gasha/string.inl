#pragma once
#ifndef GASHA_INCLUDED_STRING_INL
#define GASHA_INCLUDED_STRING_INL

//--------------------------------------------------------------------------------
// string.inl
// 文字列処理【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/string.h>//文字列処理【宣言部】

#include <cstring>//std::strlen(), std::strcmp(), std::strncmp(), std::strchr(), std::strrchr(), std::strstr(), std::strcpy(), std::strncpy()
                  //::strnlen() ※GCC, VC++
#include <cstdio>//::snprintf() ※GCC, ::_snprintf_s() ※VC++
#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//strlen
#ifdef GASHA_USE_NAME_SPACE
inline std::size_t strlen(const char* str)
{
#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4996)//【VC++】C4996を抑える
	return std::strlen(str);
#pragma warning(pop)//【VC++】ワーニング設定を復元
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
#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4996)//【VC++】C4996を抑える
	return std::strcmp(str1, str2);
#pragma warning(pop)//【VC++】ワーニング設定を復元
}
#endif//GASHA_USE_NAME_SPACE

//----------------------------------------
//strncmp
#ifdef GASHA_USE_NAME_SPACE
inline int strncmp(const char* str1, const char* str2, const std::size_t max_len)
{
#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4996)//【VC++】C4996を抑える
	return std::strncmp(str1, str2, max_len);
#pragma warning(pop)//【VC++】ワーニング設定を復元
}
#endif//GASHA_USE_NAME_SPACE

#ifdef GASHA_USE_NAME_SPACE
//----------------------------------------
//strchr
inline const char* strchr(const char* str, const char c)
{
#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4996)//【VC++】C4996を抑える
	return std::strchr(str, c);
#pragma warning(pop)//【VC++】ワーニング設定を復元
}
#endif//GASHA_USE_NAME_SPACE

#ifdef GASHA_USE_NAME_SPACE
//----------------------------------------
//strrchr
inline const char* strrchr(const char* str, const char c)
{
#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4996)//【VC++】C4996を抑える
	return std::strrchr(str, c);
#pragma warning(pop)//【VC++】ワーニング設定を復元
}
#endif//GASHA_USE_NAME_SPACE

#ifdef GASHA_USE_NAME_SPACE
//----------------------------------------
//strstr
inline const char* strstr(const char* str, const char* pattern)
{
#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4996)//【VC++】C4996を抑える
	return std::strstr(str, pattern);
#pragma warning(pop)//【VC++】ワーニング設定を復元
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
#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4996)//【VC++】C4996を抑える
	return std::strcpy(dst, src);
#pragma warning(pop)//【VC++】ワーニング設定を復元
}
#endif//GASHA_USE_NAME_SPACE

#ifdef GASHA_USE_NAME_SPACE
//----------------------------------------
//strcpy
inline const char* strncpy(char* dst, const char* src, const std::size_t max_len)
{
#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4996)//【VC++】C4996を抑える
	return std::strncpy(dst, src, max_len);
#pragma warning(pop)//【VC++】ワーニング設定を復元
}
#endif//GASHA_USE_NAME_SPACE

#ifdef GASHA_USE_NAME_SPACE
//----------------------------------------
//sprintf
template<typename... Tx>
inline int sprintf(char* dst, const char* fmt, Tx&&... args)
{
#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4996)//【VC++】C4996を抑える
	return std::sprintf(dst, fmt, std::forward<Tx>(args)...);
#pragma warning(pop)//【VC++】ワーニング設定を復元
}
#endif//GASHA_USE_NAME_SPACE

//----------------------------------------
//spprintf
template<typename... Tx>
inline int spprintf(char* dst, std::size_t& pos, const char* fmt, Tx&&... args)
{
#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4996)//【VC++】C4996を抑える
	int ret = std::sprintf(dst + pos, fmt, std::forward<Tx>(args)...);
#pragma warning(pop)//【VC++】ワーニング設定を復元
	pos += ret;
	return ret;
}
template<typename... Tx>
int spprintf(char* dst, const std::size_t max_size, std::size_t& pos, const char* fmt, Tx&&... args)
{
	char* dst_now = dst + pos;
	const std::size_t remain = max_size - pos;
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
	if (ret >= static_cast<int>(remain))
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
#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4996)//【VC++】C4996を抑える
	int ret = std::sprintf(dst, fmt, std::forward<Tx>(args)...);
#pragma warning(pop)//【VC++】ワーニング設定を復元
	return ret;
}

//最大バッファサイズのみ指定版
//※既存の snprintf との衝突を避けた変則名
template<typename... Tx>
int spnprintf(char* dst, const std::size_t max_size, const char* fmt, Tx&&... args)
{
	std::size_t pos = 0;
	return spprintf(dst, max_size, pos, fmt, std::forward<Tx>(args)...);
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_STRING_INL

// End of file

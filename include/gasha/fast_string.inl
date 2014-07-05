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

#ifdef GASHA_USE_SSE4_2
#include <nmmintrin.h>//SSE4.2
#endif//GASHA_USE_SSE4_2

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】str*** を使用すると、error C4996 が発生する
//  error C4996: 'str***': This function or variable may be unsafe. Consider using strncpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

#ifdef GASHA_IS_GCC
//----------------------------------------
//通常版strnlen
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
#endif//GASHA_IS_GCC

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
//strstr0
inline const char* strstr0(const char* str, const char* pattern)
{
	const std::size_t pattern_len = strlen(pattern);
	const int ret = strncmp(str, pattern, pattern_len);
	if (ret == 0)
		return str;
	return nullptr;
}

#ifdef GASHA_USE_SSE4_2

//----------------------------------------
//SSE版strlen
inline std::size_t strlen_sse(const char* str)
{
//nullチェックしない
//	if (!str)
//		return 0;
	static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
	const __m128i null = _mm_setzero_si128();
	const char* p = str;
	const std::size_t str_over = reinterpret_cast<intptr_t>(str)& 0xf;
	if (str_over != 0)
	{
		//非16バイトアランイメント時
		const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
		const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
		const std::size_t pos = _mm_cmpistri(null, str16, flags);
		if (zf)
			return pos;
		p += (16 - str_over);
	}
	//16バイトアランイメント時
	const __m128i* p128 = reinterpret_cast<const __m128i*>(p);
	while (true)
	{
		const __m128i str16 = _mm_load_si128(p128);
		const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
		const std::size_t pos = _mm_cmpistri(null, str16, flags);
		if (zf)
			return (reinterpret_cast<const char*>(p128)-str) + pos;
		++p128;
	}
	return 0;//dummy
}

//----------------------------------------
//SSE版strnlen
inline std::size_t strnlen_sse(const char* str, const std::size_t max_len)
{
//nullチェックしない
//	if (!str)
//		return 0;
	static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
	const __m128i null = _mm_setzero_si128();
	const char* p_end = str + max_len;
	const char* p_end_16 = p_end - 16;
	const char* p = str;
	const std::size_t str_over = reinterpret_cast<intptr_t>(str)& 0xf;
	if (str_over != 0)
	{
		//非16バイトアランイメント時
		const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
		const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
		const std::size_t pos = _mm_cmpistri(null, str16, flags);
		if (zf)
		{
			if (pos < max_len)
				return pos;
			return max_len;
		}
		p += (16 - str_over);
	}
	//16バイトアランイメント時
	const __m128i* p128 = reinterpret_cast<const __m128i*>(p);
	while (reinterpret_cast<const char*>(p128) <= p_end_16)
	{
		const __m128i str16 = _mm_load_si128(p128);
		const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
		const std::size_t pos = _mm_cmpistri(null, str16, flags);
		if (zf)
			return (reinterpret_cast<const char*>(p128)-str) + pos;
		++p128;
	}
	if (reinterpret_cast<const char*>(p128) < p_end)
	{
		const __m128i str16 = _mm_load_si128(p128);
		const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
		const std::size_t pos = _mm_cmpistri(null, str16, flags);
		if (zf)
		{
			const std::size_t len = (reinterpret_cast<const char*>(p128)-str) + pos;
			if (len <= max_len)
				return len;
		}
	}
	return max_len;
}

//----------------------------------------
//SSE版strcmp
namespace _private
{
	//SSE版strcmp補助関数
	//アラインメント判定
	//戻り値:
	//  0 ... 16バイトアラインメント＋16バイトアラインメント
	//  1 ... 16バイトアラインメント＋非16バイトアラインメント
	//  2 ... 非16バイトアラインメント＋16バイトアラインメント
	//  3 ... 非16バイトアラインメント＋非16バイトアラインメント
	static inline int sse_str_pattern(const char* str1, const char* str2)
	{
		return (((reinterpret_cast<intptr_t>(str1)& 0xf) != 0) << 0) |
			(((reinterpret_cast<intptr_t>(str2)& 0xf) != 0) << 1);
	}
	//SSE版strcmp:関数テーブル
	typedef int(*strcmp_sse_t)(const char* str1, const char* str2);
	extern const strcmp_sse_t strcmp_sse_table[4];
}//namespace _private
//SSE版strcmp
inline int strcmp_sse(const char* str1, const char* str2)
{
//nullチェックしない
//	if (!str1) if (!str2) return 0;
//	           else       return -1;
//	else       if (!str2) return 1;
//	//         else       continue...
	return _private::strcmp_sse_table[_private::sse_str_pattern(str1, str2)](str1, str2);
}

//----------------------------------------
//SSE版strncmp
namespace _private
{
	//SSE版strncmp:関数テーブル
	typedef int(*strncmp_sse_t)(const char* str1, const char* str2, const std::size_t max_len);
	extern const strncmp_sse_t strncmp_sse_table[4];
}//namespace _private
//SSE版strncmp
inline int strncmp_sse(const char* str1, const char* str2, const std::size_t max_len)
{
//nullチェックしない
//	if (!str1) if (!str2) return 0;
//	           else       return -1;
//	else       if (!str2) return 1;
//	//         else       continue...
	return _private::strncmp_sse_table[_private::sse_str_pattern(str1, str2)](str1, str2, max_len);
}

//----------------------------------------
//SSE版strchr
inline const char* strchr_sse(const char* str, const char c)
{
//nullチェックしない
//	if (!str)
//		return 0;
	static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
	const __m128i c16 = _mm_set1_epi8(c);
	const char* p = str;
	const std::size_t str_over = reinterpret_cast<intptr_t>(str)& 0xf;
	if (str_over != 0)
	{
		//非16バイトアランイメント時
		const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
		const int         cf  = _mm_cmpistrc(c16, str16, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
		const int         zf  = _mm_cmpistrz(c16, str16, flags);
		const std::size_t pos = _mm_cmpistri(c16, str16, flags);
		if (cf)
			return reinterpret_cast<const char*>(p)+pos;
		if (zf)
			return nullptr;
		p += (16 - str_over);
	}
	//16バイトアランイメント時
	const __m128i* p128 = reinterpret_cast<const __m128i*>(p);
	while (true)
	{
		const __m128i str16 = _mm_load_si128(p128);
		const int         cf  = _mm_cmpistrc(c16, str16, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
		const int         zf  = _mm_cmpistrz(c16, str16, flags);
		const std::size_t pos = _mm_cmpistri(c16, str16, flags);
		if (cf)
			return reinterpret_cast<const char*>(p128)+pos;
		if (zf)
			return nullptr;
		++p128;
	}
	return nullptr;//dummy
}

//----------------------------------------
//SSE版strrchr
inline const char* strrchr_sse(const char* str, const char c)
{
//nullチェックしない
//	if (!str)
//		return 0;
	const char* found_p = nullptr;
	static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_MOST_SIGNIFICANT;
	const __m128i c16 = _mm_set1_epi8(c);
	const char* p = str;
	const std::size_t str_over = reinterpret_cast<intptr_t>(str)& 0xf;
	if (str_over != 0)
	{
		//非16バイトアランイメント時
		const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
		const int         cf  = _mm_cmpistrc(c16, str16, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
		const int         zf  = _mm_cmpistrz(c16, str16, flags);
		const std::size_t pos = _mm_cmpistri(c16, str16, flags);
		if (cf)
			found_p = p + pos;
		if (zf)
			return found_p;
		p += (16 - str_over);
	}
	//16バイトアランイメント時
	const __m128i* p128 = reinterpret_cast<const __m128i*>(p);
	while (true)
	{
		const __m128i str16 = _mm_load_si128(p128);
		const int         cf  = _mm_cmpistrc(c16, str16, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
		const int         zf  = _mm_cmpistrz(c16, str16, flags);
		const std::size_t pos = _mm_cmpistri(c16, str16, flags);
		if (cf)
			found_p = reinterpret_cast<const char*>(p128)+pos;
		if (zf)
			return found_p;
		++p128;
	}
	return nullptr;//dummy
}

//----------------------------------------
//SSE版strstr
namespace _private
{
	//SSE版strstr:補助関数テーブル
	typedef int(*strstr_sse_strcmp_t)(const char* str, const char* pattern);
	extern const strstr_sse_strcmp_t strstr_sse_strcmp_table[4];
	
	//SSE版strstr補助関数
	//戻り値: 0 ... 不一致, 1 ... 一致, 2 ... strの方が短い
	inline static int strstr_sse_strcmp(const char* str, const char* pattern)
	{
		return strstr_sse_strcmp_table[sse_str_pattern(str, pattern)](str, pattern);
	}
	//SSE版strstr
	const char* strstr_sse(const char* str, const char* pattern);
}//namespace _private
//SSE版strstr
inline const char* strstr_sse(const char* str, const char* pattern)
{
	return _private::strstr_sse(str, pattern);
}

//----------------------------------------
//SSE版strstr(BM法)
namespace _private
{
	//SSE版strstr(BM法)
	const char* strstrbm_sse(const char* str, const char* pattern, std::function<bool(const char*, const char*)> found_it);
}//namespace _private
//SSE版strstr(BM法)
inline const char* strstrbm_sse(const char* str, const char* pattern, std::function<bool(const char*, const char*)> found_it)
{
	return _private::strstrbm_sse(str, pattern, found_it);
}

//----------------------------------------
//SSE版strstr0
inline const char* strstr0_sse(const char* str, const char* pattern)
{
	const int ret = _private::strstr_sse_strcmp(str, pattern);
	if (ret == 1)
		return str;
	return nullptr;
}

//----------------------------------------
//SSE版strcpy
namespace _private
{
	//SSE版strcpy:関数テーブル
	typedef const char* (*strcpy_sse_t)(char* dst, const char* src);
	extern const strcpy_sse_t strcpy_sse_table[4];
}//namespace _private
//SSE版strcpy
inline const char* strcpy_sse(char* dst, const char* src)
{
//nullチェックしない
//	if (!dst || !src)
//		return nullptr;
	return _private::strcpy_sse_table[_private::sse_str_pattern(dst, src)](dst, src);
}

//----------------------------------------
//SSE版strncpy
namespace _private
{
	//SSE版strncpy:関数テーブル
	typedef const char* (*strncpy_sse_t)(char* dst, const char* src, const std::size_t max_len);
	extern const strncpy_sse_t strncpy_sse_table[4];
}//namespace _private
//SSE版strncpy
inline const char* strncpy_sse(char* dst, const char* src, const std::size_t max_len)
{
//nullチェックしない
//	if (!dst || !src)
//		return nullptr;
	return _private::strncpy_sse_table[_private::sse_str_pattern(dst, src)](dst, src, max_len);
}

#else//GASHA_USE_SSE4_2

//----------------------------------------
//SSE版strlen
inline std::size_t strlen_sse(const char* str)
{
	return strlen(str);
}

//----------------------------------------
//SSE版strnlen
inline std::size_t strnlen_sse(const char* str, const std::size_t max_len)
{
	return strnlen(str, max_len);
}

//----------------------------------------
//SSE版strcmp
inline int strcmp_sse(const char* str1, const char* str2)
{
	return strcmp(str1, str2);
}

//----------------------------------------
//SSE版strncmp
inline int strncmp_sse(const char* str1, const char* str2, const std::size_t max_len)
{
	return strncmp(str1, str2, max_len);
}

//----------------------------------------
//SSE版strchr
inline const char* strchr_sse(const char* str, const char c)
{
	return strchr(str, c);
}

//----------------------------------------
//SSE版strrchr
inline const char* strrchr_sse(const char* str, const char c)
{
	return strrchr(str, c);
}

//----------------------------------------
//SSE版strstr
inline const char* strstr_sse(const char* str, const char* pattern)
{
	return strstr(str, pattern);
}

//----------------------------------------
//SSE版strstr(BM法)
inline const char* strstrbm_sse(const char* str, const char* pattern, std::function<bool(const char*, const char*)> found_it)
{
	return strstrbm(str, pattern, found_it);
}

//----------------------------------------
//SSE版strstr0
inline const char* strstr0_sse(const char* str, const char* pattern)
{
	return strstr0(str, pattern);
}

//----------------------------------------
//SSE版strcpy
inline const char* strcpy_sse(char* dst, const char* src)
{
	return strcpy(dst, src);
}

//----------------------------------------
//SSE版strncpy
inline const char* strncpy_sse(char* dst, const char* src, const std::size_t max_len)
{
	return strncpy(dst, src, max_len);
}

#endif//GASHA_USE_SSE4_2

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

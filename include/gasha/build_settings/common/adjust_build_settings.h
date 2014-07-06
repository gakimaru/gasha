#pragma once
#ifndef GASHA_INCLUDED_ADJUST_BUILD_SETTINGS_H
#define GASHA_INCLUDED_ADJUST_BUILD_SETTINGS_H

//--------------------------------------------------------------------------------
// adjust_build_settings.h
// ビルド設定調整
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//【全体設定：SSE命令設定】

#ifdef GASHA_IS_X86//x86,x64系CPUの場合のみ設定可

	#if !defined(GASHA_USE_SSE) && (defined(GASHA_USE_SSE2) || defined(GASHA_USE_SSE3) || defined(GASHA_USE_SSE4_1) || defined(GASHA_USE_SSE4_2) || defined(GASHA_USE_AVX) || defined(GASHA_USE_AVX2))
		#define GASHA_USE_SSE
	#endif//GASHA_USE_SSE

	#if !defined(GASHA_USE_SSE2) && (defined(GASHA_USE_SSE3) || defined(GASHA_USE_SSE4_1) || defined(GASHA_USE_SSE4_2) || defined(GASHA_USE_AVX) || defined(GASHA_USE_AVX2))
		#define GASHA_USE_SSE2
	#endif//GASHA_USE_SSE2

	#if !defined(GASHA_USE_SSE3) && (defined(GASHA_USE_SSE4_1) || defined(GASHA_USE_SSE4_2) || defined(GASHA_USE_AVX) || defined(GASHA_USE_AVX2))
		#define GASHA_USE_SSE3
	#endif//GASHA_USE_SSE3

	#if !defined(GASHA_USE_SSE4_1) && (defined(GASHA_USE_SSE4_2) || defined(GASHA_USE_AVX) || defined(GASHA_USE_AVX2))
		#define GASHA_USE_SSE4_1
	#endif//GASHA_USE_SSE4_1

	#if !defined(GASHA_USE_SSE4_2) && (defined(GASHA_USE_AVX) || defined(GASHA_USE_AVX2))
		#define GASHA_USE_SSE4_2
	#endif//GASHA_USE_SSE4_2

	#if !defined(GASHA_USE_AVX) && defined(GASHA_USE_AVX2)
		#define GASHA_USE_AVX
	#endif//GASHA_USE_AVX

#else//GASHA_IS_X86

	#ifdef GASHA_USE_SSE
		#undef GASHA_USE_SSE
	#endif//GASHA_USE_SSE

	#ifdef GASHA_USE_SSE2
		#undef GASHA_USE_SSE2
	#endif//GASHA_USE_SSE2

	#ifdef GASHA_USE_SSE3
		#undef GASHA_USE_SSE3
	#endif//GASHA_USE_SSE3

	#ifdef GASHA_USE_SSE4A
		#undef GASHA_USE_SSE4A
	#endif//GASHA_USE_SSE4A

	#ifdef GASHA_USE_SSE4_1
		#undef GASHA_USE_SSE4_1
	#endif//GASHA_USE_SSE4_1

	#ifdef GASHA_USE_SSE4_2
		#undef GASHA_USE_SSE4_2
	#endif//GASHA_USE_SSE4_2

	#ifdef GASHA_USE_POPCNT
		#undef GASHA_USE_POPCNT
	#endif//GASHA_USE_POPCNT

	#ifdef GASHA_USE_AES
		#undef GASHA_USE_AES
	#endif//GASHA_USE_AES

	#ifdef GASHA_USE_AVX
		#undef GASHA_USE_AVX
	#endif//GASHA_USE_AVX

	#ifdef GASHA_USE_AVX2
		#undef GASHA_USE_AVX2
	#endif//GASHA_USE_AVX2

	#ifdef GASHA_USE_FMA4
		#undef GASHA_USE_FMA4
	#endif//GASHA_USE_FMA4

	#ifdef GASHA_USE_FMA3
		#undef GASHA_USE_FMA3
	#endif//GASHA_USE_FMA3

#endif//GASHA_IS_X86

//--------------------------------------------------------------------------------
//【算術設定：高速算術】
//※ライブラリの再ビルド不要

#if defined(GASHA_FAST_ARITH_USE_SSE) && !defined(GASHA_USE_SSE)
	#undef GASHA_FAST_ARITH_USE_SSE
#endif//GASHA_FAST_ARITH_USE_SSE

#if defined(GASHA_FAST_ARITH_USE_SSE2) && !defined(GASHA_USE_SSE2)
	#undef GASHA_FAST_ARITH_USE_SSE2
#endif//GASHA_FAST_ARITH_USE_SSE2

#if defined(GASHA_FAST_ARITH_USE_SSE4_1) && !defined(GASHA_USE_SSE4_1)
	#undef GASHA_FAST_ARITH_USE_SSE4_1
#endif//GASHA_FAST_ARITH_USE_SSE4_1

#if defined(GASHA_FAST_ARITH_USE_AVX) && !defined(GASHA_USE_AVX)
	#undef GASHA_FAST_ARITH_USE_AVX
#endif//GASHA_FAST_ARITH_USE_AVX

#if defined(GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION) && !defined(GASHA_FAST_ARITH_USE_SSE)
//#if defined(GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION) && (!defined(GASHA_FAST_ARITH_USE_SSE) && !defined(GASHA_FAST_ARITH_USE_***))//その他のSIMD演算に対応するなら条件を追加する
	#undef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION

#if defined(GASHA_VECTOR_OPERATION_ALWAYS_USE_SSE) && !defined(GASHA_USE_SSE4_1)
	#undef GASHA_VECTOR_OPERATION_ALWAYS_USE_SSE
#endif//GASHA_VECTOR_OPERATION_ALWAYS_USE_SSE

#if defined(GASHA_MATRIX_OPERATION_ALWAYS_USE_SSE) && !defined(GASHA_USE_SSE4_1)
	#undef GASHA_MATRIX_OPERATION_ALWAYS_USE_SSE
#endif//GASHA_MATRIX_OPERATION_ALWAYS_USE_SSE

//--------------------------------------------------------------------------------
//【算術設定：CRC32】

#if defined(GASHA_CRC32_USE_SSE) && (!defined(GASHA_CRC32_IS_CRC32C) || !defined(GASHA_USE_SSE4_2))
	#undef GASHA_CRC32_USE_SSE
#endif//GASHA_CRC32_USE_SSE

//--------------------------------------------------------------------------------
//【文字列設定：高速文字列】

#if defined(GASHA_FASE_STRING_USE_SSE4_2) && !defined(GASHA_USE_SSE4_2)
	#undef GASHA_FASE_STRING_USE_SSE4_2
#endif//GASHA_FASE_STRING_USE_SSE4_2

#if defined(GASHA_STRLEN_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRLEN_FAST_USE_SSE4_2
#endif//GASHA_STRLEN_FAST_USE_SSE4_2

#if defined(GASHA_STRNLEN_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRNLEN_FAST_USE_SSE4_2
#endif//GASHA_STRNLEN_FAST_USE_SSE4_2

#if defined(GASHA_STRCMP_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRCMP_FAST_USE_SSE4_2
#endif//GASHA_STRCMP_FAST_USE_SSE4_2

#if defined(GASHA_STRNCMP_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRNCMP_FAST_USE_SSE4_2
#endif//GASHA_STRNCMP_FAST_USE_SSE4_2

#if defined(GASHA_STRCHR_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRCHR_FAST_USE_SSE4_2
#endif//GASHA_STRCHR_FAST_USE_SSE4_2

#if defined(GASHA_STRRCHR_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRRCHR_FAST_USE_SSE4_2
#endif//GASHA_STRRCHR_FAST_USE_SSE4_2

#if defined(GASHA_STRSTR_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRSTR_FAST_USE_SSE4_2
#endif//GASHA_STRSTR_FAST_USE_SSE4_2

#if defined(GASHA_STRSTRBM_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRSTRBM_FAST_USE_SSE4_2
#endif//GASHA_STRSTRBM_FAST_USE_SSE4_2

#if defined(GASHA_STRSTR0_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRSTR0_FAST_USE_SSE4_2
#endif//GASHA_STRSTR0_FAST_USE_SSE4_2

#if defined(GASHA_STRCPY_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRCPY_FAST_USE_SSE4_2
#endif//GASHA_STRCPY_FAST_USE_SSE4_2

#if defined(GASHA_STRNCPY_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRNCPY_FAST_USE_SSE4_2
#endif//GASHA_STRNCPY_FAST_USE_SSE4_2


//--------------------------------------------------------------------------------
//【シングルトンデバッグ用処理】
//※ライブラリの再ビルド不要（ただし、ライブラリ内で使用している可能性があるため、できるだけ再ビルドした方が良い）

#if defined(GASHA_SINGLETON_DEBUG_ENABLED) && !defined(GASHA_HAS_DEBUG_FEATURE)
	#undef GASHA_SINGLETON_DEBUG_ENABLED
#endif//GASHA_SINGLETON_DEBUG_ENABLED

#endif//GASHA_INCLUDED_ADJUST_BUILD_SETTINGS_H

// End of file

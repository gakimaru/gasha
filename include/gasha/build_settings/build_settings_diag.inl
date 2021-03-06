﻿#pragma once
#ifndef GASHA_INCLUDED_BUIILD_SETTINGS_DIAG_INL
#define GASHA_INCLUDED_BUIILD_SETTINGS_DIAG_INL

//--------------------------------------------------------------------------------
// build_settings/build_settings_diag.inl
// ビルド設定診断【インライン関数／テンプレート関数定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/build_settings_diag.h>//ビルド設定診断【宣言部】

#include <gasha/fast_string.h>//高速文字列：strncpy_fast()
#include <gasha/cpuid.h>//CPU情報（x86系CPU用）
#include <gasha/string.h>//文字列処理：spprintf()

#include <type_traits>//C++11 std::is_same

#ifdef GASHA_USE_AVX
#include <immintrin.h>//AVX
#endif//GASHA_USE_AVX

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ビルド設定診断

//--------------------
//診断モード

//ライブラリビルド時の状態を診断
struct diagForLibrary_internal_t{};//※内部処理用（直接使用しない）

//--------------------
//ビルド設定が実行環境に適合するか診断
template<class MODE>
bool buildSettingsDiagnosticTest(char* message, const std::size_t max_size, std::size_t& message_len, const MODE mode)
{
	message_len = 0;
	message[0] = '\0';
	
	GASHA_ spprintf(message, max_size, message_len, "==============================================================================\n");
	if (std::is_same<MODE, diagForLibrary_internal_t>::value)
		GASHA_ spprintf(message, max_size, message_len, "Diagnostic Mode: Library\n");
	else
		GASHA_ spprintf(message, max_size, message_len, "Diagnostic Mode: Current\n");
	GASHA_ spprintf(message, max_size, message_len, "==============================================================================\n");
	GASHA_ spprintf(message, max_size, message_len, "\n");

	GASHA_ spprintf(message, max_size, message_len, "------------------------------------------------------------------------------\n");
	GASHA_ spprintf(message, max_size, message_len, "[ GASHA version : %d.%d ]\n", GASHA_MAJOR, GASHA_MINOR);
	GASHA_ spprintf(message, max_size, message_len, "\n");

	GASHA_ spprintf(message, max_size, message_len, "------------------------------------------------------------------------------\n");
	GASHA_ spprintf(message, max_size, message_len, "[ Build-settings diagnostic test ]\n");
	GASHA_ spprintf(message, max_size, message_len, "\n");
	
	bool has_error = false;

	//デバッガ用ブレークポイント割り込み利用可能
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
	GASHA_ spprintf(message, max_size, message_len, "[ENABLED] Poly-allocator(override on standard new/delete operator) is AVAILABLE.\n");
#else//GASHA_ENABLE_POLY_ALLOCATOR
	GASHA_ spprintf(message, max_size, message_len, "[DISABLED] Poly-allocator(override on standard new/delete operator) is NOT available.\n");
#endif//GASHA_ENABLE_POLY_ALLOCATOR

	//デバッガ用ブレークポイント割り込み利用可能
#ifdef GASHA_DEBUGGER_BREAK_IS_AVAILABLE
	GASHA_ spprintf(message, max_size, message_len, "[OK] Debugger-break-point is AVAILABLE.\n");
#else//GASHA_DEBUGGER_BREAK_IS_AVAILABLE
	GASHA_ spprintf(message, max_size, message_len, "[--] Debugger-break-point is NOT available.\n");
#endif//GASHA_DEBUGGER_BREAK_IS_AVAILABLE

	//デバッグログ有効化
#ifdef GASHA_LOG_IS_ENABLED
	GASHA_ spprintf(message, max_size, message_len, "[OK] Debug-log is ENABLED.\n");
#else//GASHA_LOG_IS_ENABLED
	GASHA_ spprintf(message, max_size, message_len, "[--] Debug-log is NOT enabled.\n");
#endif//GASHA_LOG_IS_ENABLED

#ifdef GASHA_LOG_IS_ENABLED
	//ログキュー使用
#ifdef GASHA_LOG_PRINT_USE_QUEUE
	GASHA_ spprintf(message, max_size, message_len, "[OK] Debug-log USE log-queue.\n");
#else//GASHA_LOG_PRINT_USE_QUEUE
	GASHA_ spprintf(message, max_size, message_len, "[--] Debug-log NOT use log-queue.\n");
#endif//GASHA_LOG_PRINT_USE_QUEUE
#endif//GASHA_LOG_IS_ENABLED

	//TLS対応
#ifdef GASHA_INCOMPLETE_TLS_INITIALIZER
	GASHA_ spprintf(message, max_size, message_len, "[NG] TLS initializer is INCOMPLETE!\n");
#else//GASHA_INCOMPLETE_TLS_INITIALIZER
	GASHA_ spprintf(message, max_size, message_len, "[OK] TLS initializer is COMPLETE.\n");
#endif//GASHA_INCOMPLETE_TLS_INITIALIZER

	//標準入力利用可能
#ifdef GASHA_STDIN_IS_AVAILABLE
	GASHA_ spprintf(message, max_size, message_len, "[OK] `stdin` is AVAILABLE.\n");
#else//GASHA_STDIN_IS_AVAILABLE
	GASHA_ spprintf(message, max_size, message_len, "[NG] `stdin` is NOT available!\n");
#endif//GASHA_STDIN_IS_AVAILABLE

	//CPU機能判定
#ifdef GASHA_IS_X86
	//x86系CPU情報取得
	char cpu_info_str[12] = { 0 };
	int cpu_info[4] = { 0, 0, 0, 0 };
	__cpuid(cpu_info, 0);//CPU情報取得：Type0
	strncpy_fast(cpu_info_str, reinterpret_cast<const char*>(&cpu_info[1]), sizeof(cpu_info_str) - 1);
	GASHA_ spprintf(message, max_size, message_len, "cpu_string=\"%s\"\n", cpu_info_str);
	__cpuid(cpu_info, 1);//CPU情報取得：Type1

#ifdef GASHA_USE_SSE
	const bool sse_is_supported = (cpu_info[3] & (1 << 25)) || false;//SSE対応
	if (!sse_is_supported)
	{
		has_error = true;
		GASHA_ spprintf(message, max_size, message_len, "[NG] 'SSE' is NOT supported!\n");
	}
	GASHA_ spprintf(message, max_size, message_len, "[OK] 'SSE' is SUPPORTED.\n");
#else//GASHA_USE_SSE
	GASHA_ spprintf(message, max_size, message_len, "[--] 'SSE' is not used.\n");
#endif//GASHA_USE_SSE

#ifdef GASHA_USE_SSE2
	const bool sse2_is_supported = (cpu_info[3] & (1 << 26)) || false;//SSE2対応
	if (!sse2_is_supported)
	{
		has_error = true;
		GASHA_ spprintf(message, max_size, message_len, "[NG] 'SSE2' is NOT supported!\n");
	}
	GASHA_ spprintf(message, max_size, message_len, "[OK] 'SSE2' is SUPPORTED.\n");
#else//GASHA_USE_SSE2
	GASHA_ spprintf(message, max_size, message_len, "[--] 'SSE2' is not used.\n");
#endif//GASHA_USE_SSE2

#ifdef GASHA_USE_SSE3
	const bool sse3_is_supported = (cpu_info[2] & (1 << 9)) || false;//SSE3対応
	if (!sse3_is_supported)
	{
		has_error = true;
		GASHA_ spprintf(message, max_size, message_len, "[NG] 'SSE3' is NOT supported!\n");
	}
	GASHA_ spprintf(message, max_size, message_len, "[OK] 'SSE3' is SUPPORTED.\n");
#else//GASHA_USE_SSE3
	GASHA_ spprintf(message, max_size, message_len, "[--] 'SSE3' is not used.\n");
#endif//GASHA_USE_SSE3

#ifdef GASHA_USE_SSE4A
	const bool sse4a_is_supported = (cpu_info[2] & (1 << 6)) || false;//SSE4a対応
	if (!sse4a_is_supported)
	{
		has_error = true;
		GASHA_ spprintf(message, max_size, message_len, "[NG] 'SSE4a' is NOT supported!\n");
	}
	GASHA_ spprintf(message, max_size, message_len, "[OK] 'SSE4a' is SUPPORTED.\n");
#else//GASHA_USE_SSE4A
	GASHA_ spprintf(message, max_size, message_len, "[--] 'SSE4a' is not used.\n");
#endif//GASHA_USE_SSE4A

#ifdef GASHA_USE_SSE4_1
	const bool sse4_1_is_supported = (cpu_info[2] & (1 << 19)) || false;//SSE4.1対応
	if (!sse4_1_is_supported)
	{
		has_error = true;
		GASHA_ spprintf(message, max_size, message_len, "[NG] 'SSE4.1' is NOT supported!\n");
	}
	GASHA_ spprintf(message, max_size, message_len, "[OK] 'SSE4.1' is SUPPORTED.\n");
#else//GASHA_USE_SSE4_1
	GASHA_ spprintf(message, max_size, message_len, "[--] 'SSE4.1' is not used.\n");
#endif//GASHA_USE_SSE4_1

#ifdef GASHA_USE_SSE4_2
	const bool sse4_2_is_supported = (cpu_info[2] & (1 << 20)) || false;//SSE4.2対応
	if (!sse4_2_is_supported)
	{
		has_error = true;
		GASHA_ spprintf(message, max_size, message_len, "[NG] 'SSE4.2' is NOT supported!\n");
	}
	GASHA_ spprintf(message, max_size, message_len, "[OK] 'SSE4.2' is SUPPORTED.\n");
#else//GASHA_USE_SSE4_2
	GASHA_ spprintf(message, max_size, message_len, "[--] 'SSE4.2' is not used.\n");
#endif//GASHA_USE_SSE4_2

#ifdef GASHA_USE_POPCNT
	const bool poopcnt_is_supported = (cpu_info[2] & (1 << 23)) || false;//POPCNT対応
	if (!poopcnt_is_supported)
	{
		has_error = true;
		GASHA_ spprintf(message, max_size, message_len, "[NG] 'POPCNT' is NOT supported!\n");
	}
	GASHA_ spprintf(message, max_size, message_len, "[OK] 'POPCNT' is SUPPORTED.\n");
#else//GASHA_USE_POPCNT
	GASHA_ spprintf(message, max_size, message_len, "[--] 'POPCNT' is not used.\n");
#endif//GASHA_USE_POPCNT

#ifdef GASHA_USE_AES
	const bool aes_is_supported = (cpu_info[2] & (1 << 25)) || false;//AES対応
	if (!aes_is_supported)
	{
		has_error = true;
		GASHA_ spprintf(message, max_size, message_len, "[NG] 'AES' is NOT supported!\n");
	}
	GASHA_ spprintf(message, max_size, message_len, "[OK] 'AES' is SUPPORTED.\n");
#else//GASHA_USE_AES
	GASHA_ spprintf(message, max_size, message_len, "[--] 'AES' is not used.\n");
#endif//GASHA_USE_AES

#ifdef GASHA_USE_AVX
	const bool osxsave_is_supported = (cpu_info[2] & (1 << 27)) || false;//OSXSAVE対応
	const bool _avx_is_supported = (cpu_info[2] & (1 << 28)) || false;//AVX対応（基本）
#endif//GASHA_USE_AVX

#ifdef GASHA_USE_AVX
	bool avx_is_supported = false;//AVX対応
	if (osxsave_is_supported && _avx_is_supported)
	{
		unsigned long long xcr_feature_enabled = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
        avx_is_supported = (xcr_feature_enabled & 0x6) || false;
	}
	if(!avx_is_supported)
	{
		has_error = true;
		GASHA_ spprintf(message, max_size, message_len, "[NG] 'AVX' is NOT supported!\n");
	}
	GASHA_ spprintf(message, max_size, message_len, "[OK] 'AVX' is SUPPORTED.\n");
#else//GASHA_USE_AVX

	GASHA_ spprintf(message, max_size, message_len, "[--] 'AVX' is not used.\n");
#endif//GASHA_USE_AVX
#ifdef GASHA_USE_AVX2//※判定未対応
	GASHA_ spprintf(message, max_size, message_len, "[OK?] 'AVX2' is supported?\n");
#else//GASHA_USE_AVX2
	GASHA_ spprintf(message, max_size, message_len, "[--] 'AVX2' is not used.\n");
#endif//GASHA_USE_AVX2

#endif//GASHA_IS_X86

	GASHA_ spprintf(message, max_size, message_len, "\n");
	GASHA_ spprintf(message, max_size, message_len, "------------------------------------------------------------------------------\n");
	GASHA_ spprintf(message, max_size, message_len, "[ Compiler specification ]\n");
	
	//CPUアーキテクチャ情報
	GASHA_ spprintf(message, max_size, message_len, "\n");
	GASHA_ spprintf(message, max_size, message_len, "Platform: %s (Target Versionn=%d.%d)\n", GASHA_PLATFORM_NAME, GASHA_PLATFORM_VER, GASHA_PLATFORM_MINOR);
	GASHA_ spprintf(message, max_size, message_len, "CPU Architecture: %s (%d bit, %s endian)", GASHA_PLATFORM_ARCHITECTURE_NAME, GASHA_PLATFORM_ARCHITECTURE_BITS, GASHA_ENDIAN_NAME);
#ifdef GASHA_IS_X86
	GASHA_ spprintf(message, max_size, message_len, " [x86]");
#endif//GASHA_IS_X86
#ifdef GASHA_IS_X64
	GASHA_ spprintf(message, max_size, message_len, " [x64]");
#endif//GASHA_IS_X64
	GASHA_ spprintf(message, max_size, message_len, "\n");

	//コンパイラ情報
	GASHA_ spprintf(message, max_size, message_len, "\n");
	GASHA_ spprintf(message, max_size, message_len, "Compiler: %s (Version=%d.%d)", GASHA_COMPILER_NAME, GASHA_COMPILER_VER, GASHA_COMPILER_MINOR);
#ifdef GASHA_IS_VC
	GASHA_ spprintf(message, max_size, message_len, " ... ");
	switch (GASHA_COMPILER_VER)
	{
	case 600: GASHA_ spprintf(message, max_size, message_len, "MS-C Compiler 6.0"); break;
	case 700: GASHA_ spprintf(message, max_size, message_len, "MS-C/C++ Compiler 7.0"); break;
	case 800: GASHA_ spprintf(message, max_size, message_len, "Visual C++ 1.0"); break;
	case 900: GASHA_ spprintf(message, max_size, message_len, "Visual C++ 2.0"); break;
	case 1000: GASHA_ spprintf(message, max_size, message_len, "Visual C++ 4.0"); break;
	case 1010: GASHA_ spprintf(message, max_size, message_len, "Visual C++ 4.1"); break;
	case 1020: GASHA_ spprintf(message, max_size, message_len, "Visual C++ 4.2"); break;
	case 1100: GASHA_ spprintf(message, max_size, message_len, "Visual C++ 5.0 (Visual Studio 97)"); break;
	case 1200: GASHA_ spprintf(message, max_size, message_len, "Visual C++ 6.0 (Visual Studio 6.0)"); break;
	case 1300: GASHA_ spprintf(message, max_size, message_len, "Visual C++ 7.0 (Visual Studio.NET 2002)"); break;
	case 1310: GASHA_ spprintf(message, max_size, message_len, "Visual C++ 7.1 (Visual Studio.NET 2003)"); break;
	case 1400: GASHA_ spprintf(message, max_size, message_len, "Visual C++ 8.0 (Visual Studio 2005)"); break;
	case 1500: GASHA_ spprintf(message, max_size, message_len, "Visual C++ 9.0 (Visual Studio 2008)"); break;
	case 1600: GASHA_ spprintf(message, max_size, message_len, "Visual C++ 10.0 (Visual Studio 2010)"); break;
	case 1700: GASHA_ spprintf(message, max_size, message_len, "Visual C++ 11.0 (Visual Studio 2012)"); break;
	case 1800: GASHA_ spprintf(message, max_size, message_len, "Visual C++ 12.0 (Visual Studio 2013)"); break;
	default: GASHA_ spprintf(message, max_size, message_len, "Unknown"); break;
	}
#endif//GASHA_IS_VC
	
	//言語仕様判定
	GASHA_ spprintf(message, max_size, message_len, "\n");
	GASHA_ spprintf(message, max_size, message_len, "Language: %s", GASHA_PROGRAM_LANGUAGE_NAME);
#ifdef __cplusplus
	GASHA_ spprintf(message, max_size, message_len, " (__cplusplus=%ld)", __cplusplus);
#endif//__cplusplus
#ifdef GASHA_HAS_CPP98
	GASHA_ spprintf(message, max_size, message_len, " [C++98]");
#endif//GASHA_HAS_CPP98
#ifdef GASHA_HAS_CPP03
	GASHA_ spprintf(message, max_size, message_len, " [C++03]");
#endif//GASHA_HAS_CPP03
#ifdef GASHA_HAS_CPP11
	GASHA_ spprintf(message, max_size, message_len, " [C++11]");
#endif//GASHA_HAS_CPP11
#ifdef GASHA_HAS_CPP14
	GASHA_ spprintf(message, max_size, message_len, " [C++14]");
#endif//GASHA_HAS_CPP14
	GASHA_ spprintf(message, max_size, message_len, "\n");

	GASHA_ spprintf(message, max_size, message_len, "\n");

	//【C++11仕様】nullptr
#ifdef GASHA_HAS_NULLPTR
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `nullptr` is AVAILABLE.\n");
#else//GASHA_HAS_NULLPTR
#ifdef GASHA_HAS_CAMOUFLAGE_NULLPTR
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`nullptr` is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_NULLPTR
	GASHA_ spprintf(message, max_size, message_len, "[No]        `nullptr` is NOT available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_NULLPTR
#endif//GASHA_HAS_NULLPTR

	//【C++11仕様】override
#ifdef GASHA_HAS_OVERRIDE
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `override` is AVAILABLE.\n");
#else//GASHA_HAS_OVERRIDE
#ifdef GASHA_HAS_DUMMY_OVERRIDE
	GASHA_ spprintf(message, max_size, message_len, "[Dummy]     `override` is available.\n");
#else//GASHA_HAS_DUMMY_OVERRIDE
	GASHA_ spprintf(message, max_size, message_len, "[No]        `override` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_OVERRIDE
#endif//GASHA_HAS_OVERRIDE

	//【C++11仕様】final
#ifdef GASHA_HAS_FINAL
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `final` is AVAILABLE.\n");
#else//GASHA_HAS_FINAL
#ifdef GASHA_HAS_DUMMY_FINAL
	GASHA_ spprintf(message, max_size, message_len, "[Dummy]     `final` is available.\n");
#else//GASHA_HAS_DUMMY_FINAL
	GASHA_ spprintf(message, max_size, message_len, "[No]        `final` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_FINAL
#endif//GASHA_HAS_FINAL

	//【C++11仕様】auto型推論
#ifdef GASHA_HAS_AUTO
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `auto` is AVAILABLE.\n");
#else//GASHA_HAS_AUTO
	GASHA_ spprintf(message, max_size, message_len, "[No]        `auto` is NOT available.\n");
#endif//GASHA_HAS_AUTO

	//【C++11仕様】decltype型指定子
#ifdef GASHA_HAS_DECLTYPE
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `decltype()` is AVAILABLE.\n");
#else//GASHA_HAS_DECLTYPE
	GASHA_ spprintf(message, max_size, message_len, "[No]        `decltype()` is NOT available.\n");
#endif//GASHA_HAS_DECLTYPE
	
	//【C++11仕様】default/delete宣言
#ifdef GASHA_HAS_DEFAULT_AND_DELETE
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `default` & `delete` is AVAILABLE.\n");
#else//GASHA_HAS_DEFAULT_AND_DELETE
	GASHA_ spprintf(message, max_size, message_len, "[No]        `default` & `delete` is NOT available.\n");
#endif//GASHA_HAS_DEFAULT_AND_DELETE

	//【C++11仕様】constexpr
#ifdef GASHA_HAS_CONSTEXPR
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `constexpr` is AVAILABLE.\n");
#else//GASHA_HAS_CONSTEXPR
#ifdef GASHA_HAS_DUMMY_CONSTEXPR
	GASHA_ spprintf(message, max_size, message_len, "[Dummy]     `constexpr` is available.\n");
#else//GASHA_HAS_DUMMY_CONSTEXPR
	GASHA_ spprintf(message, max_size, message_len, "[No]        `constexpr` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_CONSTEXPR
#endif//GASHA_HAS_CONSTEXPR

	//【C++11仕様】ユーザー定義リテラル
#ifdef GASHA_HAS_USER_DEFINED_LITERAL
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       'User-defined-literal' is AVAILABLE.\n");
#else//GASHA_HAS_USER_DEFINED_LITERAL
	GASHA_ spprintf(message, max_size, message_len, "[No]        'User-defined-literal' is NOT available.\n");
#endif//GASHA_HAS_USER_DEFINED_LITERAL

	//【C++11仕様】ラムダ式
#ifdef GASHA_HAS_LAMBDA_EXPRESSION
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       'Lambda-expression' is AVAILABLE.\n");
#else//GASHA_HAS_LAMBDA_EXPRESSION
	GASHA_ spprintf(message, max_size, message_len, "[No]        'Lambda-expression' is NOT available.\n");
#endif//GASHA_HAS_LAMBDA_EXPRESSION

	//【C++11仕様】範囲に基づくforループ
#ifdef GASHA_HAS_RANGE_BASED_FOR
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       'Range-based `for`' is AVAILABLE.\n");
#else//GASHA_HAS_RANGE_BASED_FOR
	GASHA_ spprintf(message, max_size, message_len, "[No]        'Range-based `for`' is NOT available.\n");
#endif//GASHA_HAS_RANGE_BASED_FOR

	//【C++11仕様】強い型付けを持った列挙型（enumの型指定）
#ifdef GASHA_HAS_ENUM_CLASS
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       '`enum` class' is AVAILABLE.\n");
#else//GASHA_HAS_ENUM_CLASS
	GASHA_ spprintf(message, max_size, message_len, "[No]        '`enum` class' is NOT available.\n");
#endif//GASHA_HAS_ENUM_CLASS

	//【C++11仕様】可変長引数テンプレート
#ifdef GASHA_HAS_VARIADIC_TEMPLATE
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       'Variadic-template' is AVAILABLE.\n");
#else//GASHA_HAS_VARIADIC_TEMPLATE
	GASHA_ spprintf(message, max_size, message_len, "[No]        'Variadic-template' is NOT available.\n");
#endif//GASHA_HAS_VARIADIC_TEMPLATE

	//【C++11仕様】テンプレートエイリアス（テンプレートの別名using）
#ifdef GASHA_HAS_TEMPLATE_ALIASES
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       'Template-aliases' is AVAILABLE.\n");
#else//GASHA_HAS_TEMPLATE_ALIASES
	GASHA_ spprintf(message, max_size, message_len, "[No]        'Template-aliases' is NOT available.\n");
#endif//GASHA_HAS_TEMPLATE_ALIASES

	//【C++11仕様】委譲コンストラクタ
#ifdef GASHA_HAS_DELEGATING_CONSTRUCTORS
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       'Delegating-constructors' is AVAILABLE.\n");
#else//GASHA_HAS_DELEGATING_CONSTRUCTORS
	GASHA_ spprintf(message, max_size, message_len, "[No]        'Delegating-constructors' is NOT available.\n");
#endif//GASHA_HAS_DELEGATING_CONSTRUCTORS

	//【C++11仕様】継承コンストラクタ
#ifdef GASHA_HAS_INHERITING_CONSTRUCTORS
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       'Inheriting-constructors' is AVAILABLE.\n");
#else//GASHA_HAS_INHERITING_CONSTRUCTORS
	GASHA_ spprintf(message, max_size, message_len, "[No]        'Inheriting-constructors' is NOT available.\n");
#endif//GASHA_HAS_INHERITING_CONSTRUCTORS

	//【C++11仕様】一様初期化
#ifdef GASHA_HAS_UNIFORM_INITIALIZATION
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       'Uniform-initializer' is AVAILABLE.\n");
#else//GASHA_HAS_UNIFORM_INITIALIZATION
	GASHA_ spprintf(message, max_size, message_len, "[No]        'Uniform-initializer' is NOT available.\n");
#endif//GASHA_HAS_UNIFORM_INITIALIZATION

	//【C++11仕様】初期化リスト
#ifdef GASHA_HAS_INITIALIZER_LIST
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       'Initializer-list' is AVAILABLE.\n");
#else//GASHA_HAS_INITIALIZER_LIST
	GASHA_ spprintf(message, max_size, message_len, "[No]        'Initializer-list' is NOT available.\n");
#endif//GASHA_HAS_INITIALIZER_LIST

	//【C++11仕様】右辺値参照とムーブセマンティクス
#ifdef GASHA_HAS_RVALUE_REFERENCE
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       'Rvalue-reference' & 'std::move' is AVAILABLE.\n");
#else//GASHA_HAS_RVALUE_REFERENCE
	GASHA_ spprintf(message, max_size, message_len, "[No]        'Rvalue-reference' & 'std::move' is NOT available.\n");
#endif//GASHA_HAS_RVALUE_REFERENCE

	//【C++11仕様】静的アサーション
#ifdef GASHA_HAS_STATIC_ASSERT
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `static_assert()` is AVAILABLE.\n");
#else//GASHA_HAS_STATIC_ASSERT
#ifdef GASHA_HAS_CUSTOM_STATIC_ASSERT
	GASHA_ spprintf(message, max_size, message_len, "[Original]  `static_assert()` is available.\n");
#else//GASHA_HAS_CUSTOM_STATIC_ASSERT
	GASHA_ spprintf(message, max_size, message_len, "[No]        `static_assert()` is NOT available.\n");
#endif//GASHA_HAS_CUSTOM_STATIC_ASSERT
#endif//GASHA_HAS_STATIC_ASSERT

	//【C++11仕様】TLS指定子
#ifdef GASHA_HAS_THREAD_LOCAL
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `thread_local` is AVAILABLE.\n");
#else//GASHA_HAS_THREAD_LOCAL
#ifdef GASHA_HAS_CAMOUFLAGE_THREAD_LOCAL
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`thread_local` is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_THREAD_LOCAL
	GASHA_ spprintf(message, max_size, message_len, "[No]        `thread_local` is NOT available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_THREAD_LOCAL
#endif//GASHA_HAS_THREAD_LOCAL

	//【C++11仕様】例外無効
#ifdef GASHA_HAS_NOEXCEPT
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `noexcept` is AVAILABLE. -> [Yes] `GASHA_NOEXCEPT` is AVAILABLE.\n");
#else//GASHA_HAS_NOEXCEPT
#ifdef GASHA_HAS_CAMOUFLAGE_NOEXCEPT
	GASHA_ spprintf(message, max_size, message_len, "[No]        `noexcept` is NOT available. -> [Camouflage]`GASHA_NOEXCEPT` is AVAILABLE.\n");
#else//GASHA_HAS_CAMOUFLAGE_NOEXCEPT
	GASHA_ spprintf(message, max_size, message_len, "[No]        `noexcept` is NOT available. -> [No] `GASHA_NOEXCEPT` is AVAILABLE.\n");
#endif//GASHA_HAS_CAMOUFLAGE_NOEXCEPT
#endif//GASHA_HAS_NOEXCEPT

	//【C++11仕様】alignas
#ifdef GASHA_HAS_ALIGNAS
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `alignas()` is AVAILABLE.\n");
#else//GASHA_HAS_ALIGNAS
#ifdef GASHA_HAS_CAMOUFLAGE_ALIGNAS
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`alignas()` is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_ALIGNAS
	GASHA_ spprintf(message, max_size, message_len, "[No]        `alignas()` is NOT available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_ALIGNAS
#endif//GASHA_HAS_ALIGNAS
#ifdef GASHA_ALIGNAS_LIMIT
	GASHA_ spprintf(message, max_size, message_len, "            liomit of `alignas()` = %d\n", GASHA_ALIGNAS_LIMIT);
#endif//GASHA_ALIGNAS_LIMIT

	//【C++11仕様】alignof
#ifdef GASHA_HAS_ALIGNOF
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `alignof()` is AVAILABLE.\n");
#else//GASHA_HAS_ALIGNOF
#ifdef GASHA_HAS_CAMOUFLAGE_ALIGNOF
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`alignof()` is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_ALIGNOF
	GASHA_ spprintf(message, max_size, message_len, "[No]        `alignof()` is NOT available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_ALIGNOF
#endif//GASHA_HAS_ALIGNOF

	//【C++11仕様】alignas(alignof(T))
#ifdef GASHA_IS_ALLOWED_ALIGNAS_OF
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `alignas(alignof())` is AVAILABLE. -> [Yes] `GASHA_ALIGNAS_OF()` is AVAILABLE.\n");
#else//GASHA_HAS_CAMOUFLAGE_ALIGNAS_OF
#ifdef GASHA_HAS_CAMOUFLAGE_ALIGNAS_OF
	GASHA_ spprintf(message, max_size, message_len, "[No]        `alignas(alignof())` is NOT available. -> [Comouflage]`GASHA_ALIGNAS_OF()` is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_ALIGNAS_OF
	GASHA_ spprintf(message, max_size, message_len, "[No]        `alignas(alignof())` is NOT available. -> [No] `GASHA_ALIGNAS_OF()` is NOT available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_ALIGNAS_OF
#endif//GASHA_IS_ALLOWED_ALIGNAS_OF

	GASHA_ spprintf(message, max_size, message_len, "\n");
	
	//_aligned_malloc 
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
#ifdef GASHA_HAS_ALIGNED_MALLOC
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `_aligned_malloc()`(VC++ style) is AVAILABLE.\n");
#else//GASHA_HAS_ALIGNED_MALLOC
#ifdef GASHA_HAS_CAMOUFLAGE_ALIGNED_MALLOC
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`_aligned_malloc()`(VC++ style) is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_ALIGNED_MALLOC
	GASHA_ spprintf(message, max_size, message_len, "[No]        `_aligned_malloc()`(VC++ style) is available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_ALIGNED_MALLOC
#endif//GASHA_HAS_ALIGNED_MALLOC
	
	//_aligned_free
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
#ifdef GASHA_HAS_ALIGNED_FREE
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `_aligned_free()`(VC++ style) is AVAILABLE.\n");
#else//GASHA_HAS_ALIGNED_FREE
#ifdef GASHA_HAS_CAMOUFLAGE_ALIGNED_FREE
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`_aligned_free()`(VC++ style) is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_ALIGNED_FREE
	GASHA_ spprintf(message, max_size, message_len, "[No]        `_aligned_free()`(VC++ style) is available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_ALIGNED_FREE
#endif//GASHA_HAS_ALIGNED_FREE

	//mallinfo
	//※GCC独自実装
#ifdef GASHA_HAS_MALLINFO
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `mallinfo()`(GCC style) is AVAILABLE.\n");
#else//GASHA_HAS_MALLINFO
	GASHA_ spprintf(message, max_size, message_len, "[No]        `mallinfo()`(GCC style) is available.\n");
#endif//GASHA_HAS_MALLINFO

	//__attribute__((constructor)), __attribute__((destructor))
#ifdef GASHA_HAS_CONSTRUCTOR_ATTRIBUTE
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__attribute__((constructor))`(GCC style) is AVAILABLE.\n");
#else//GASHA_HAS_CONSTRUCTOR_ATTRIBUTE
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__attribute__((constructor))`(GCC style) is AVAILABLE.\n");
#endif//GASHA_HAS_CONSTRUCTOR_ATTRIBUTE
#ifdef GASHA_HAS_CONSTRUCTOR_ATTRIBUTE_WITH_PRIORITY
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__attribute__((constructor(PRIORITY)))`(GCC style) is AVAILABLE.\n");
#else//GASHA_HAS_CONSTRUCTOR_ATTRIBUTE_WITH_PRIORITY
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__attribute__((constructor(PRIORITY)))`(GCC style) is AVAILABLE.\n");
#endif//GASHA_HAS_CONSTRUCTOR_ATTRIBUTE_WITH_PRIORITY
#ifdef GASHA_HAS_DESTRUCTOR_ATTRIBUTE
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__attribute__((destructor))`(GCC style) is AVAILABLE.\n");
#else//GASHA_HAS_DESTRUCTOR_ATTRIBUTE
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__attribute__((destructor))`(GCC style) is AVAILABLE.\n");
#endif//GASHA_HAS_DESTRUCTOR_ATTRIBUTE
#ifdef GASHA_HAS_DESTRUCTOR_ATTRIBUTE_WITH_PRIORITY
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__attribute__((destructor(PRIORITY)))`(GCC style) is AVAILABLE.\n");
#else//GASHA_HAS_DESTRUCTOR_ATTRIBUTE_WITH_PRIORITY
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__attribute__((destructor(PRIORITY)))`(GCC style) is AVAILABLE.\n");
#endif//GASHA_HAS_DESTRUCTOR_ATTRIBUTE_WITH_PRIORITY
#ifdef GASHA_HAS_INIT_PRIORITY_ATTRIBUTE
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__attribute__((init_priority(PRIORITY)))`(GCC style) is AVAILABLE.\n");
#else//GASHA_HAS_INIT_PRIORITY_ATTRIBUTE
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__attribute__((init_priority(PRIORITY)))`(GCC style) is AVAILABLE.\n");
#endif//GASHA_HAS_INIT_PRIORITY_ATTRIBUTE

	//__cpuid 
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
#ifdef GASHA_HAS_CPUID
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__cpuid()`,_`xgetbv()`,`_XCR_XFEATURE_ENABLED_MASK`(VC++ style) is AVAILABLE.\n");
#else//GASHA_HAS_CPUID
#ifdef GASHA_HAS_CAMOUFLAGE_CPUID
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`__cpuid()`,_`xgetbv()`,`_XCR_XFEATURE_ENABLED_MASK`(VC++ style) is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_CPUID
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__cpuid()`,_`xgetbv()`,`_XCR_XFEATURE_ENABLED_MASK`(VC++ style) is available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_CPUID
#endif//GASHA_HAS_CPUID

	//no_inline
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
#ifdef GASHA_HAS_NO_INLINE
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `GASHA_NOINLINE` is AVAILABLE.\n");
#else//GASHA_HAS_NO_INLINE
#ifdef GASHA_HAS_CAMOUFLAGE_NO_INLINE
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`GASHA_NOINLINE` is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_NO_INLINE
	GASHA_ spprintf(message, max_size, message_len, "[No]        `GASHA_NOINLINE` is NOT available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_NO_INLINE
#endif//GASHA_HAS_NO_INLINE

	//always_inline
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
#ifdef GASHA_HAS_ALWAYS_INLINE
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `GASHA_ALWAYS_INLINE` is AVAILABLE.\n");
#else//GASHA_HAS_ALWAYS_INLINE
#ifdef GASHA_HAS_CAMOUFLAGE_ALWAYS_INLINE
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`GASHA_ALWAYS_INLINE` is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_ALWAYS_INLINE
	GASHA_ spprintf(message, max_size, message_len, "[No]        `GASHA_ALWAYS_INLINE` is NOT available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_ALWAYS_INLINE
#endif//GASHA_HAS_ALWAYS_INLINE

	//マクロ
	GASHA_ spprintf(message, max_size, message_len, "\n");

	//マクロ：__FILE__
#ifdef GASHA_HAS_MACRO_FILE
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__FILE__` is AVAILABLE. (ex. __FILE__ = \"%s\")\n", __FILE__);
#else//GASHA_HAS_MACRO_FILE
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_FILE
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`__FILE__` is available. (ex. __FILE__ = \"%s\")\n", __FILE__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_FILE
#ifdef GASHA_HAS_DUMMY_MACRO_FILE
	GASHA_ spprintf(message, max_size, message_len, "[Dummy]     `__FILE__` is available. (ex. __FILE__ = \"%s\")\n", __FILE__);
#else//GASHA_HAS_DUMMY_MACRO_FILE
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__FILE__` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_FILE
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_FILE
#endif//GASHA_HAS_MACRO_FILE

	//マクロ：__LINE__
#ifdef GASHA_HAS_MACRO_LINE
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__LINE__` is AVAILABLE. (ex. __LINE__ = %d)\n", __LINE__);
#else//GASHA_HAS_MACRO_LINE
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_LINE
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`__LINE__` is available. (ex. __LINE__ = %d)\n", __LINE__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_LINE
#ifdef GASHA_HAS_DUMMY_MACRO_LINE
	GASHA_ spprintf(message, max_size, message_len, "[Dummy]     `__LINE__` is available. (ex. __LINE__ = %d)\n", __LINE__);
#else//GASHA_HAS_DUMMY_MACRO_LINE
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__LINE__` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_LINE
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_LINE
#endif//GASHA_HAS_MACRO_LINE

	//マクロ：__DATE__
#ifdef GASHA_HAS_MACRO_DATE
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__DATE__` is AVAILABLE. (ex. __DATE__ = \"%s\")\n", __DATE__);
#else//GASHA_HAS_MACRO_DATE
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_DATE
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`__DATE__` is available. (ex. __DATE__ = \"%s\")\n", __DATE__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_DATE
#ifdef GASHA_HAS_DUMMY_MACRO_DATE
	GASHA_ spprintf(message, max_size, message_len, "[Dummy]     `__DATE__` is available. (ex. __DATE__ = \"%s\")\n", __DATE__);
#else//GASHA_HAS_DUMMY_MACRO_DATE
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__DATE__` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_DATE
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_DATE
#endif//GASHA_HAS_MACRO_DATE

	//マクロ：__TIME__
#ifdef GASHA_HAS_MACRO_TIME
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__TIME__` is AVAILABLE. (ex. __TIME__ = \"%s\")\n", __TIME__);
#else//GASHA_HAS_MACRO_TIME
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_TIME
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`__TIME__` is available. (ex. __TIME__ = \"%s\")\n", __TIME__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_TIME
#ifdef GASHA_HAS_DUMMY_MACRO_TIME
	GASHA_ spprintf(message, max_size, message_len, "[Dummy]     `__TIME__` is available. (ex. __TIME__ = \"%s\")\n", __TIME__);
#else//GASHA_HAS_DUMMY_MACRO_TIME
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__TIME__` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_TIME
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_TIME
#endif//GASHA_HAS_MACRO_TIME

	//マクロ：__TIMESTAMP__
#ifdef GASHA_HAS_MACRO_TIMESTAMP
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__TIMESTAMP__` is AVAILABLE. (ex. __TIMESTAMP__ = \"%s\")\n", __TIMESTAMP__);
#else//GASHA_HAS_MACRO_TIMESTAMP
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_TIMESTAMP
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`__TIMESTAMP__` is available. (ex. __TIMESTAMP__ = \"%s\")\n", __TIMESTAMP__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_TIMESTAMP
#ifdef GASHA_HAS_DUMMY_MACRO_TIMESTAMP
	GASHA_ spprintf(message, max_size, message_len, "[Dummy]     `__TIMESTAMP__` is available. (ex. __TIMESTAMP__ = \"%s\")\n", __TIMESTAMP__);
#else//GASHA_HAS_DUMMY_MACRO_TIMESTAMP
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__TIMESTAMP__` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_TIMESTAMP
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_TIMESTAMP
#endif//GASHA_HAS_MACRO_TIMESTAMP

	//マクロ：__FUNCTION__
#ifdef GASHA_HAS_MACRO_FUNCTION
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__FUNCTION__` is AVAILABLE. (ex. __FUNCTION__ = \"%s\")\n", __FUNCTION__);
#else//GASHA_HAS_MACRO_FUNCTION
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_FUNCTION
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`__FUNCTION__` is available. (ex. __FUNCTION__ = \"%s\")\n", __FUNCTION__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_FUNCTION
#ifdef GASHA_HAS_DUMMY_MACRO_FUNCTION
	GASHA_ spprintf(message, max_size, message_len, "[Dummy]     `__FUNCTION__` is available. (ex. __FUNCTION__ = \"%s\")\n", __FUNCTION__);
#else//GASHA_HAS_DUMMY_MACRO_FUNCTION
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__FUNCTION__` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_FUNCTION
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_FUNCTION
#endif//GASHA_HAS_MACRO_FUNCTION

	//マクロ：__PRETTY_FUNCTION__
	//※GCC固有仕様
#ifdef GASHA_HAS_MACRO_PRETTY_FUNCTION
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__PRETTY_FUNCTION__`(GCC Style) is AVAILABLE. (ex. __PRETTY_FUNCTION__ = \"%s\")\n", __PRETTY_FUNCTION__);
#else//GASHA_HAS_MACRO_PRETTY_FUNCTION
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_PRETTY_FUNCTION
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`__PRETTY_FUNCTION__`(GCC Style) is available. (ex. __PRETTY_FUNCTION__ = \"%s\")\n", __PRETTY_FUNCTION__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_PRETTY_FUNCTION
#ifdef GASHA_HAS_DUMMY_MACRO_PRETTY_FUNCTION
	GASHA_ spprintf(message, max_size, message_len, "[Dummy]     `__PRETTY_FUNCTION__`(GCC Style) is available. (ex. __PRETTY_FUNCTION__ = \"%s\")\n", __PRETTY_FUNCTION__);
#else//GASHA_HAS_DUMMY_MACRO_PRETTY_FUNCTION
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__PRETTY_FUNCTION__`(GCC Style) is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_PRETTY_FUNCTION
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_PRETTY_FUNCTION
#endif//GASHA_HAS_MACRO_PRETTY_FUNCTION

	//マクロ：__func__
	//※GCC固有仕様
#ifdef GASHA_HAS_MACRO_func
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__func__`(GCC Style) is AVAILABLE. (ex. __func__ = \"%s\")\n", __func__);
#else//GASHA_HAS_MACRO_func
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_func
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`__func__`(GCC Style) is available. (ex. __func__ = \"%s\")\n", __func__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_func
#ifdef GASHA_HAS_DUMMY_MACRO_func
	GASHA_ spprintf(message, max_size, message_len, "[Dummy]     `__func__`(GCC Style) is available. (ex. __func__ = \"%s\")\n", __func__);
#else//GASHA_HAS_DUMMY_MACRO_func
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__func__`(GCC Style) is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_func
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_func
#endif//GASHA_HAS_MACRO_func

	//マクロ：__FUNCSIG__
	//※VC++固有仕様
#ifdef GASHA_HAS_MACRO_FUNCSIG
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__FUNCSIG__`(VC++ Style) is AVAILABLE. (ex. __FUNCSIG__ = \"%s\")\n", __FUNCSIG__);
#else//GASHA_HAS_MACRO_FUNCSIG
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_FUNCSIG
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`__FUNCSIG__`(VC++ Style) is available. (ex. __FUNCSIG__ = \"%s\")\n", __FUNCSIG__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_FUNCSIG
#ifdef GASHA_HAS_DUMMY_MACRO_FUNCSIG
	GASHA_ spprintf(message, max_size, message_len, "[Dummy]     `__FUNCSIG__`(VC++ Style) is available. (ex. __FUNCSIG__ = \"%s\")\n", __FUNCSIG__);
#else//GASHA_HAS_DUMMY_MACRO_FUNCSIG
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__FUNCSIG__`(VC++ Style) is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_FUNCSIG
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_FUNCSIG
#endif//GASHA_HAS_MACRO_FUNCSIG

	//マクロ：__FUNCDNAME__
	//※VC++固有仕様
#ifdef GASHA_HAS_MACRO_FUNCDNAME
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `__FUNCDNAME__`(VC++ Style) is AVAILABLE. (ex. __FUNCDNAME__ = \"%s\")\n", __FUNCDNAME__);
#else//GASHA_HAS_MACRO_FUNCDNAME
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_FUNCDNAME
	GASHA_ spprintf(message, max_size, message_len, "[Camouflage]`__FUNCDNAME__`(VC++ Style) is available. (ex. __FUNCDNAME__ = \"%s\")\n", __FUNCDNAME__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_FUNCDNAME
#ifdef GASHA_HAS_DUMMY_MACRO_FUNCDNAME
	GASHA_ spprintf(message, max_size, message_len, "[Dummy]     `__FUNCDNAME__`(VC++ Style) is available. (ex. __FUNCDNAME__ = \"%s\")\n", __FUNCDNAME__);
#else//GASHA_HAS_DUMMY_MACRO_FUNCDNAME
	GASHA_ spprintf(message, max_size, message_len, "[No]        `__FUNCDNAME__`(VC++ Style) is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_FUNCDNAME
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_FUNCDNAME
#endif//GASHA_HAS_MACRO_FUNCDNAME

	//独自拡張マクロ
	GASHA_ spprintf(message, max_size, message_len, "\n");
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `GASHA_FUNC_NAME()` is AVAILABLE. (ex. GASHA_FUNC_NAME() = \"%s\")\n", GASHA_FUNC_NAME());
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `GASHA_SRC_FILE()` is AVAILABLE. (ex. GASHA_SRC_FILE() = \"%s\")\n", GASHA_SRC_FILE());
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `GASHA_SRC_FILE_LINE()` is AVAILABLE. (ex. GASHA_SRC_FILE_LINE() = \"%s\")\n", GASHA_SRC_FILE_LINE());
	GASHA_ spprintf(message, max_size, message_len, "[Yes]       `GASHA_SRC_FILE_LINE_TIME()` is AVAILABLE. (ex. GASHA_SRC_FILE_LINE_TIME() = \"%s\")\n", GASHA_SRC_FILE_LINE_TIME());

	GASHA_ spprintf(message, max_size, message_len, "\n");
	GASHA_ spprintf(message, max_size, message_len, "------------------------------------------------------------------------------\n");
	GASHA_ spprintf(message, max_size, message_len, "[ Build-configuratoin ]\n");

	//ビルド設定
	
	GASHA_ spprintf(message, max_size, message_len, "\n");
	GASHA_ spprintf(message, max_size, message_len, "Configuration name: %s\n", GASHA_BUILD_CONFIG_NAME);
	GASHA_ spprintf(message, max_size, message_len, "\n");

	//#define GASHA_BUILD_CONFIG_IS_DEBUG//フルデバッグ設定
	//#define GASHA_BUILD_CONFIG_IS_DEBUG_MODERATE//プログラム開発向け設定
	//#define GASHA_BUILD_CONFIG_IS_DEBUG_OPT//コンテンツ制作・QA向け設定
	//#define GASHA_BUILD_CONFIG_IS_REGRESSION_TEST//自動回帰テスト向け設定
	//#define GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE//製品テスト向け設定
	//#define GASHA_BUILD_CONFIG_IS_RELEASE//製品向け設定

#ifdef GASHA_DEBUG_FEATURE_IS_ENABLED//デバッグ機能有効化
	GASHA_ spprintf(message, max_size, message_len, "[Yes] 'Debug-feature' is AVAILABLE.");
#ifdef GASHA_VERBOSE_DEBUG_IS_ENABLED//冗長デバッグ機能有効化
	GASHA_ spprintf(message, max_size, message_len, " (with VERBOSE mode)");
#endif//GASHA_VERBOSE_DEBUG_IS_ENABLED
#else//GASHA_DEBUG_FEATURE_IS_ENABLED
	GASHA_ spprintf(message, max_size, message_len, "[No]  'Debug-feature' is NOT available.");
#endif//GASHA_DEBUG_FEATURE_IS_ENABLED
	GASHA_ spprintf(message, max_size, message_len, "\n");

#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション／ブレークポイント／ウォッチポイント有効化
	GASHA_ spprintf(message, max_size, message_len, "[Yes] 'Assertion' is ENABLED.");
#else//GASHA_ASSERTION_IS_ENABLED
	GASHA_ spprintf(message, max_size, message_len, "[No]  'Assertion' is DISABLED.");
#endif//GASHA_ASSERTION_IS_ENABLED
	GASHA_ spprintf(message, max_size, message_len, "\n");

#ifdef GASHA_NO_OPTIMIZED//最適化なし
	GASHA_ spprintf(message, max_size, message_len, "[No]  'Optimize' is none.");
#endif//GASHA_NO_OPTIMIZED
#ifdef GASHA_OPTIMIZED_MODERATELY//適度に最適化
	GASHA_ spprintf(message, max_size, message_len, "[Yes] 'Optimize' is moderate.");
#endif//GASHA_OPTIMIZED_MODERATELY
#ifdef GASHA_OPTIMIZED//最大限の最適化
	GASHA_ spprintf(message, max_size, message_len, "[Yes] 'Optimize' is MAX.");
#endif//GASHA_OPTIMIZED
	GASHA_ spprintf(message, max_size, message_len, "\n");
	
#ifdef GASHA_FILE_SYSTEM_IS_ROM//ROM専用ファイルシステム
	GASHA_ spprintf(message, max_size, message_len, "[---] 'File-sytem' is for ROM.");
#endif//GASHA_FILE_SYSTEM_IS_ROM
#ifdef GASHA_FILE_SYSTEM_IS_FLEXIBLE//ローカル／ROM切り替えファイルシステム（ローカルデータで設定）
	GASHA_ spprintf(message, max_size, message_len, "[---] 'File-system' is flexible (for ROM or for local-PC).");
#endif//GASHA_FILE_SYSTEM_IS_FLEXIBLE
	GASHA_ spprintf(message, max_size, message_len, "\n");

#ifdef GASHA_UNIT_TEST_ENABLED//ユニットテスト組み込み
	GASHA_ spprintf(message, max_size, message_len, "[Yes] 'Unit-test' is AVAILABLE.");
#ifdef GASHA_UNIT_TEST_AUTO_RUN_ENABLED//ユニットテスト自動実行
	GASHA_ spprintf(message, max_size, message_len, " (with auto-run for regression-test)");
#endif//GASHA_UNIT_TEST_AUTO_RUN_ENABLED
#else//GASHA_UNIT_TEST_ENABLED
	GASHA_ spprintf(message, max_size, message_len, "[[No] 'Unit-test' is NOT available.");
#endif//GASHA_UNIT_TEST_ENABLED
	GASHA_ spprintf(message, max_size, message_len, "\n");

#ifdef GASHA_HAS_SYNBOMS//シンボル情報あり
	GASHA_ spprintf(message, max_size, message_len, "[Yes] 'Symbols' are contained.");
#endif//GASHA_HAS_SYNBOMS
#ifdef GASHA_IS_STRIPPED_SYMBOLS//シンボル情報なし
	GASHA_ spprintf(message, max_size, message_len, "[No]  'Symbols' are STRIPPED.");
#endif//GASHA_IS_STRIPPED_SYMBOLS
	GASHA_ spprintf(message, max_size, message_len, "\n");

	GASHA_ spprintf(message, max_size, message_len, "\n");
	GASHA_ spprintf(message, max_size, message_len, "------------------------------------------------------------------------------\n");

	return !has_error;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_BUIILD_SETTINGS_DIAG_INL

// End of file

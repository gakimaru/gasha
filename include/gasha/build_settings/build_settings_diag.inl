﻿#pragma once
#ifndef GASHA_INCLUDED_BUIILD_SETTINGS_DIAG_INL
#define GASHA_INCLUDED_BUIILD_SETTINGS_DIAG_INL

//--------------------------------------------------------------------------------
// build_settings/build_settings_diag.inl
// ビルド設定診断【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/build_settings_diag.h>//ビルド設定診断【宣言部】

#include <gasha/fast_string.h>//高速文字列：strncpy_fast()
#include <gasha/cpuid.h>//CPU情報（x86系CPU用）

#include <type_traits>//C++11 std::is_same
#include <cstdio>//sprintf()

#ifdef GASHA_USE_AVX
#include <immintrin.h>//AVX
#endif//GASHA_USE_AVX

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】sprintf を使用すると、error C4996 が発生する
//  error C4996: 'sprintf': This function or variable may be unsafe. Consider using strncpy_fast_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

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
bool buildSettingsDiagnosticTest(char* message, std::size_t& size, const MODE mode)
{
	size = 0;
	message[0] = '\0';
	
	size += std::sprintf(message + size, "==============================================================================\n");
	if (std::is_same<MODE, diagForLibrary_internal_t>::value)
		size += std::sprintf(message + size, "Diagnostic Mode: Library\n");
	else
		size += std::sprintf(message + size, "Diagnostic Mode: Current\n");
	size += std::sprintf(message + size, "==============================================================================\n");
	size += std::sprintf(message + size, "\n");

	size += std::sprintf(message + size, "------------------------------------------------------------------------------\n");
	size += std::sprintf(message + size, "[ Build-settings diagnostic test ]\n");
	size += std::sprintf(message + size, "\n");
	
	bool has_error = false;
	
	//CPU機能判定
#ifdef GASHA_IS_X86
	//x86系CPU情報取得
	char cpu_info_str[12] = { 0 };
	int cpu_info[4] = { 0, 0, 0, 0 };
	__cpuid(cpu_info, 0);//CPU情報取得：Type0
	strncpy_fast(cpu_info_str, reinterpret_cast<const char*>(&cpu_info[1]), sizeof(cpu_info_str) - 1);
	size += std::sprintf(message + size, "cpu_string=\"%s\"\n", cpu_info_str);
	__cpuid(cpu_info, 1);//CPU情報取得：Type1

#ifdef GASHA_USE_SSE
	const bool sse_is_supported = (cpu_info[3] & (1 << 25)) || false;//SSE対応
	if (!sse_is_supported)
	{
		has_error = true;
		size += std::sprintf(message + size, "[NG] 'SSE' is NOT supported!\n");
	}
	size += std::sprintf(message + size, "[OK] 'SSE' is SUPPORTED.\n");
#else//GASHA_USE_SSE
	size += std::sprintf(message + size, "[--] 'SSE' is not used.\n");
#endif//GASHA_USE_SSE

#ifdef GASHA_USE_SSE2
	const bool sse2_is_supported = (cpu_info[3] & (1 << 26)) || false;//SSE2対応
	if (!sse2_is_supported)
	{
		has_error = true;
		size += std::sprintf(message + size, "[NG] 'SSE2' is NOT supported!\n");
	}
	size += std::sprintf(message + size, "[OK] 'SSE2' is SUPPORTED.\n");
#else//GASHA_USE_SSE2
	size += std::sprintf(message + size, "[--] 'SSE2' is not used.\n");
#endif//GASHA_USE_SSE2

#ifdef GASHA_USE_SSE3
	const bool sse3_is_supported = (cpu_info[2] & (1 << 9)) || false;//SSE3対応
	if (!sse3_is_supported)
	{
		has_error = true;
		size += std::sprintf(message + size, "[NG] 'SSE3' is NOT supported!\n");
	}
	size += std::sprintf(message + size, "[OK] 'SSE3' is SUPPORTED.\n");
#else//GASHA_USE_SSE3
	size += std::sprintf(message + size, "[--] 'SSE3' is not used.\n");
#endif//GASHA_USE_SSE3

#ifdef GASHA_USE_SSE4A
	const bool sse4a_is_supported = (cpu_info[2] & (1 << 6)) || false;//SSE4a対応
	if (!sse4a_is_supported)
	{
		has_error = true;
		size += std::sprintf(message + size, "[NG] 'SSE4a' is NOT supported!\n");
	}
	size += std::sprintf(message + size, "[OK] 'SSE4a' is SUPPORTED.\n");
#else//GASHA_USE_SSE4A
	size += std::sprintf(message + size, "[--] 'SSE4a' is not used.\n");
#endif//GASHA_USE_SSE4A

#ifdef GASHA_USE_SSE4_1
	const bool sse4_1_is_supported = (cpu_info[2] & (1 << 19)) || false;//SSE4.1対応
	if (!sse4_1_is_supported)
	{
		has_error = true;
		size += std::sprintf(message + size, "[NG] 'SSE4.1' is NOT supported!\n");
	}
	size += std::sprintf(message + size, "[OK] 'SSE4.1' is SUPPORTED.\n");
#else//GASHA_USE_SSE4_1
	size += std::sprintf(message + size, "[--] 'SSE4.1' is not used.\n");
#endif//GASHA_USE_SSE4_1

#ifdef GASHA_USE_SSE4_2
	const bool sse4_2_is_supported = (cpu_info[2] & (1 << 20)) || false;//SSE4.2対応
	if (!sse4_2_is_supported)
	{
		has_error = true;
		size += std::sprintf(message + size, "[NG] 'SSE4.2' is NOT supported!\n");
	}
	size += std::sprintf(message + size, "[OK] 'SSE4.2' is SUPPORTED.\n");
#else//GASHA_USE_SSE4_2
	size += std::sprintf(message + size, "[--] 'SSE4.2' is not used.\n");
#endif//GASHA_USE_SSE4_2

#ifdef GASHA_USE_POPCNT
	const bool poopcnt_is_supported = (cpu_info[2] & (1 << 23)) || false;//POPCNT対応
	if (!poopcnt_is_supported)
	{
		has_error = true;
		size += std::sprintf(message + size, "[NG] 'POPCNT' is NOT supported!\n");
	}
	size += std::sprintf(message + size, "[OK] 'POPCNT' is SUPPORTED.\n");
#else//GASHA_USE_POPCNT
	size += std::sprintf(message + size, "[--] 'POPCNT' is not used.\n");
#endif//GASHA_USE_POPCNT

#ifdef GASHA_USE_AES
	const bool aes_is_supported = (cpu_info[2] & (1 << 25)) || false;//AES対応
	if (!aes_is_supported)
	{
		has_error = true;
		size += std::sprintf(message + size, "[NG] 'AES' is NOT supported!\n");
	}
	size += std::sprintf(message + size, "[OK] 'AES' is SUPPORTED.\n");
#else//GASHA_USE_AES
	size += std::sprintf(message + size, "[--] 'AES' is not used.\n");
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
		size += std::sprintf(message + size, "[NG] 'AVX' is NOT supported!\n");
	}
	size += std::sprintf(message + size, "[OK] 'AVX' is SUPPORTED.\n");
#else//GASHA_USE_AVX

	size += std::sprintf(message + size, "[--] 'AVX' is not used.\n");
#endif//GASHA_USE_AVX
#ifdef GASHA_USE_AVX2//※判定未対応
	size += std::sprintf(message + size, "[OK?] 'AVX2' is supported?\n");
#else//GASHA_USE_AVX2
	size += std::sprintf(message + size, "[--] 'AVX2' is not used.\n");
#endif//GASHA_USE_AVX2

#endif//GASHA_IS_X86

	size += std::sprintf(message + size, "\n");
	size += std::sprintf(message + size, "------------------------------------------------------------------------------\n");
	size += std::sprintf(message + size, "[ Compiler specification ]\n");
	
	//CPUアーキテクチャ情報
	size += std::sprintf(message + size, "\n");
	size += std::sprintf(message + size, "Platform: %s (Target Versionn=%d.%d)\n", GASHA_PLATFORM_NAME, GASHA_PLATFORM_VER, GASHA_PLATFORM_MINOR);
	size += std::sprintf(message + size, "CPU Architecture: %s (%d bit, %s endian)", GASHA_PLATFORM_ARCHITECTURE_NAME, GASHA_PLATFORM_ARCHITECTURE_BITS, GASHA_ENDIAN_NAME);
#ifdef GASHA_IS_X86
	size += std::sprintf(message + size, " [x86]");
#endif//GASHA_IS_X86
#ifdef GASHA_IS_X64
	size += std::sprintf(message + size, " [x64]");
#endif//GASHA_IS_X64
	size += std::sprintf(message + size, "\n");

	//コンパイラ情報
	size += std::sprintf(message + size, "\n");
	size += std::sprintf(message + size, "Compiler: %s (Version=%d.%d)", GASHA_COMPILER_NAME, GASHA_COMPILER_VER, GASHA_COMPILER_MINOR);
#ifdef GASHA_IS_VC
	size += std::sprintf(message + size, " ... ");
	switch (GASHA_COMPILER_VER)
	{
	case 600: size += std::sprintf(message + size, "MS-C Compiler 6.0"); break;
	case 700: size += std::sprintf(message + size, "MS-C/C++ Compiler 7.0"); break;
	case 800: size += std::sprintf(message + size, "Visual C++ 1.0"); break;
	case 900: size += std::sprintf(message + size, "Visual C++ 2.0"); break;
	case 1000: size += std::sprintf(message + size, "Visual C++ 4.0"); break;
	case 1010: size += std::sprintf(message + size, "Visual C++ 4.1"); break;
	case 1020: size += std::sprintf(message + size, "Visual C++ 4.2"); break;
	case 1100: size += std::sprintf(message + size, "Visual C++ 5.0 (Visual Studio 97)"); break;
	case 1200: size += std::sprintf(message + size, "Visual C++ 6.0 (Visual Studio 6.0)"); break;
	case 1300: size += std::sprintf(message + size, "Visual C++ 7.0 (Visual Studio.NET 2002)"); break;
	case 1310: size += std::sprintf(message + size, "Visual C++ 7.1 (Visual Studio.NET 2003)"); break;
	case 1400: size += std::sprintf(message + size, "Visual C++ 8.0 (Visual Studio 2005)"); break;
	case 1500: size += std::sprintf(message + size, "Visual C++ 9.0 (Visual Studio 2008)"); break;
	case 1600: size += std::sprintf(message + size, "Visual C++ 10.0 (Visual Studio 2010)"); break;
	case 1700: size += std::sprintf(message + size, "Visual C++ 11.0 (Visual Studio 2012)"); break;
	case 1800: size += std::sprintf(message + size, "Visual C++ 12.0 (Visual Studio 2013)"); break;
	default: size += std::sprintf(message + size, "Unknown"); break;
	}
#endif//GASHA_IS_VC
	
	//言語仕様判定
	size += std::sprintf(message + size, "\n");
	size += std::sprintf(message + size, "Language: %s", GASHA_PROGRAM_LANGUAGE_NAME);
#ifdef __cplusplus
	size += std::sprintf(message + size, " (__cplusplus=%ld)", __cplusplus);
#endif//__cplusplus
#ifdef GASHA_HAS_CPP98
	size += std::sprintf(message + size, " [C++98]");
#endif//GASHA_HAS_CPP98
#ifdef GASHA_HAS_CPP03
	size += std::sprintf(message + size, " [C++03]");
#endif//GASHA_HAS_CPP03
#ifdef GASHA_HAS_CPP11
	size += std::sprintf(message + size, " [C++11]");
#endif//GASHA_HAS_CPP11
#ifdef GASHA_HAS_CPP14
	size += std::sprintf(message + size, " [C++14]");
#endif//GASHA_HAS_CPP14
	size += std::sprintf(message + size, "\n");

	size += std::sprintf(message + size, "\n");

	//【C++11仕様】nullptr
#ifdef GASHA_HAS_NULLPTR
	size += std::sprintf(message + size, "[Yes]       `nullptr` is AVAILABLE.\n");
#else//GASHA_HAS_NULLPTR
#ifdef GASHA_HAS_CAMOUFLAGE_NULLPTR
	size += std::sprintf(message + size, "[Camouflage]`nullptr` is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_NULLPTR
	size += std::sprintf(message + size, "[No]        `nullptr` is NOT available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_NULLPTR
#endif//GASHA_HAS_NULLPTR

	//【C++11仕様】override
#ifdef GASHA_HAS_OVERRIDE
	size += std::sprintf(message + size, "[Yes]       `override` is AVAILABLE.\n");
#else//GASHA_HAS_OVERRIDE
#ifdef GASHA_HAS_DUMMY_OVERRIDE
	size += std::sprintf(message + size, "[Dummy]     `override` is available.\n");
#else//GASHA_HAS_DUMMY_OVERRIDE
	size += std::sprintf(message + size, "[No]        `override` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_OVERRIDE
#endif//GASHA_HAS_OVERRIDE

	//【C++11仕様】final
#ifdef GASHA_HAS_FINAL
	size += std::sprintf(message + size, "[Yes]       `final` is AVAILABLE.\n");
#else//GASHA_HAS_FINAL
#ifdef GASHA_HAS_DUMMY_FINAL
	size += std::sprintf(message + size, "[Dummy]     `final` is available.\n");
#else//GASHA_HAS_DUMMY_FINAL
	size += std::sprintf(message + size, "[No]        `final` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_FINAL
#endif//GASHA_HAS_FINAL

	//【C++11仕様】auto型推論
#ifdef GASHA_HAS_AUTO
	size += std::sprintf(message + size, "[Yes]       `auto` is AVAILABLE.\n");
#else//GASHA_HAS_AUTO
	size += std::sprintf(message + size, "[No]        `auto` is NOT available.\n");
#endif//GASHA_HAS_AUTO

	//【C++11仕様】decltype型指定子
#ifdef GASHA_HAS_DECLTYPE
	size += std::sprintf(message + size, "[Yes]       `decltype()` is AVAILABLE.\n");
#else//GASHA_HAS_DECLTYPE
	size += std::sprintf(message + size, "[No]        `decltype()` is NOT available.\n");
#endif//GASHA_HAS_DECLTYPE
	
	//【C++11仕様】default/delete宣言
#ifdef GASHA_HAS_DEFAULT_AND_DELETE
	size += std::sprintf(message + size, "[Yes]       `default` & `delete` is AVAILABLE.\n");
#else//GASHA_HAS_DEFAULT_AND_DELETE
	size += std::sprintf(message + size, "[No]        `default` & `delete` is NOT available.\n");
#endif//GASHA_HAS_DEFAULT_AND_DELETE

	//【C++11仕様】constexpr
#ifdef GASHA_HAS_CONSTEXPR
	size += std::sprintf(message + size, "[Yes]       `constexpr` is AVAILABLE.\n");
#else//GASHA_HAS_CONSTEXPR
#ifdef GASHA_HAS_DUMMY_CONSTEXPR
	size += std::sprintf(message + size, "[Dummy]     `constexpr` is available.\n");
#else//GASHA_HAS_DUMMY_CONSTEXPR
	size += std::sprintf(message + size, "[No]        `constexpr` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_CONSTEXPR
#endif//GASHA_HAS_CONSTEXPR

	//【C++11仕様】ユーザー定義リテラル
#ifdef GASHA_HAS_USER_DEFINED_LITERAL
	size += std::sprintf(message + size, "[Yes]       'User-defined-literal' is AVAILABLE.\n");
#else//GASHA_HAS_USER_DEFINED_LITERAL
	size += std::sprintf(message + size, "[No]        'User-defined-literal' is NOT available.\n");
#endif//GASHA_HAS_USER_DEFINED_LITERAL

	//【C++11仕様】ラムダ式
#ifdef GASHA_HAS_LAMBDA_EXPRESSION
	size += std::sprintf(message + size, "[Yes]       'Lambda-expression' is AVAILABLE.\n");
#else//GASHA_HAS_LAMBDA_EXPRESSION
	size += std::sprintf(message + size, "[No]        'Lambda-expression' is NOT available.\n");
#endif//GASHA_HAS_LAMBDA_EXPRESSION

	//【C++11仕様】範囲に基づくforループ
#ifdef GASHA_HAS_RANGE_BASED_FOR
	size += std::sprintf(message + size, "[Yes]       'Range-based `for`' is AVAILABLE.\n");
#else//GASHA_HAS_RANGE_BASED_FOR
	size += std::sprintf(message + size, "[No]        'Range-based `for`' is NOT available.\n");
#endif//GASHA_HAS_RANGE_BASED_FOR

	//【C++11仕様】強い型付けを持った列挙型（enumの型指定）
#ifdef GASHA_HAS_ENUM_CLASS
	size += std::sprintf(message + size, "[Yes]       '`enum` class' is AVAILABLE.\n");
#else//GASHA_HAS_ENUM_CLASS
	size += std::sprintf(message + size, "[No]        '`enum` class' is NOT available.\n");
#endif//GASHA_HAS_ENUM_CLASS

	//【C++11仕様】可変長引数テンプレート
#ifdef GASHA_HAS_VARIADIC_TEMPLATE
	size += std::sprintf(message + size, "[Yes]       'Variadic-template' is AVAILABLE.\n");
#else//GASHA_HAS_VARIADIC_TEMPLATE
	size += std::sprintf(message + size, "[No]        'Variadic-template' is NOT available.\n");
#endif//GASHA_HAS_VARIADIC_TEMPLATE

	//【C++11仕様】テンプレートエイリアス（テンプレートの別名using）
#ifdef GASHA_HAS_TEMPLATE_ALIASES
	size += std::sprintf(message + size, "[Yes]       'Template-aliases' is AVAILABLE.\n");
#else//GASHA_HAS_TEMPLATE_ALIASES
	size += std::sprintf(message + size, "[No]        'Template-aliases' is NOT available.\n");
#endif//GASHA_HAS_TEMPLATE_ALIASES

	//【C++11仕様】委譲コンストラクタ
#ifdef GASHA_HAS_DELEGATING_CONSTRUCTORS
	size += std::sprintf(message + size, "[Yes]       'Delegating-constructors' is AVAILABLE.\n");
#else//GASHA_HAS_DELEGATING_CONSTRUCTORS
	size += std::sprintf(message + size, "[No]        'Delegating-constructors' is NOT available.\n");
#endif//GASHA_HAS_DELEGATING_CONSTRUCTORS

	//【C++11仕様】継承コンストラクタ
#ifdef GASHA_HAS_INHERITING_CONSTRUCTORS
	size += std::sprintf(message + size, "[Yes]       'Inheriting-constructors' is AVAILABLE.\n");
#else//GASHA_HAS_INHERITING_CONSTRUCTORS
	size += std::sprintf(message + size, "[No]        'Inheriting-constructors' is NOT available.\n");
#endif//GASHA_HAS_INHERITING_CONSTRUCTORS

	//【C++11仕様】一様初期化
#ifdef GASHA_HAS_UNIFORM_INITIALIZATION
	size += std::sprintf(message + size, "[Yes]       'Uniform-initializer' is AVAILABLE.\n");
#else//GASHA_HAS_UNIFORM_INITIALIZATION
	size += std::sprintf(message + size, "[No]        'Uniform-initializer' is NOT available.\n");
#endif//GASHA_HAS_UNIFORM_INITIALIZATION

	//【C++11仕様】初期化リスト
#ifdef GASHA_HAS_INITIALIZER_LIST
	size += std::sprintf(message + size, "[Yes]       'Initializer-list' is AVAILABLE.\n");
#else//GASHA_HAS_INITIALIZER_LIST
	size += std::sprintf(message + size, "[No]        'Initializer-list' is NOT available.\n");
#endif//GASHA_HAS_INITIALIZER_LIST

	//【C++11仕様】右辺値参照とムーブセマンティクス
#ifdef GASHA_HAS_RVALUE_REFERENCE
	size += std::sprintf(message + size, "[Yes]       'Rvalue-reference' & 'std::move' is AVAILABLE.\n");
#else//GASHA_HAS_RVALUE_REFERENCE
	size += std::sprintf(message + size, "[No]        'Rvalue-reference' & 'std::move' is NOT available.\n");
#endif//GASHA_HAS_RVALUE_REFERENCE

	//【C++11仕様】静的アサーション
#ifdef GASHA_HAS_STATIC_ASSERT
	size += std::sprintf(message + size, "[Yes]       `static_assert()` is AVAILABLE.\n");
#else//GASHA_HAS_STATIC_ASSERT
#ifdef GASHA_HAS_ORIGINAL_STATIC_ASSERT
	size += std::sprintf(message + size, "[Original]  `static_assert()` is available.\n");
#else//GASHA_HAS_ORIGINAL_STATIC_ASSERT
	size += std::sprintf(message + size, "[No]        `static_assert()` is NOT available.\n");
#endif//GASHA_HAS_ORIGINAL_STATIC_ASSERT
#endif//GASHA_HAS_STATIC_ASSERT

	//【C++11仕様】TLS指定子
#ifdef GASHA_HAS_THREAD_LOCAL
	size += std::sprintf(message + size, "[Yes]       `thread_local` is AVAILABLE.\n");
#else//GASHA_HAS_THREAD_LOCAL
#ifdef GASHA_HAS_CAMOUFLAGE_THREAD_LOCAL
	size += std::sprintf(message + size, "[Camouflage]`thread_local` is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_THREAD_LOCAL
	size += std::sprintf(message + size, "[No]        `thread_local` is NOT available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_THREAD_LOCAL
#endif//GASHA_HAS_THREAD_LOCAL

	//【C++11仕様】例外無効
#ifdef GASHA_HAS_NOEXCEPT
	size += std::sprintf(message + size, "[Yes]       `noexcept` is AVAILABLE. -> [Yes] `GASHA_NOEXCEPT` is AVAILABLE.\n");
#else//GASHA_HAS_NOEXCEPT
#ifdef GASHA_HAS_CAMOUFLAGE_NOEXCEPT
	size += std::sprintf(message + size, "[No]        `noexcept` is NOT available. -> [Camouflage]`GASHA_NOEXCEPT` is AVAILABLE.\n");
#else//GASHA_HAS_CAMOUFLAGE_NOEXCEPT
	size += std::sprintf(message + size, "[No]        `noexcept` is NOT available. -> [No] `GASHA_NOEXCEPT` is AVAILABLE.\n");
#endif//GASHA_HAS_CAMOUFLAGE_NOEXCEPT
#endif//GASHA_HAS_NOEXCEPT

	//【C++11仕様】alignas
#ifdef GASHA_HAS_ALIGNAS
	size += std::sprintf(message + size, "[Yes]       `alignas()` is AVAILABLE.\n");
#else//GASHA_HAS_ALIGNAS
#ifdef GASHA_HAS_CAMOUFLAGE_ALIGNAS
	size += std::sprintf(message + size, "[Camouflage]`alignas()` is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_ALIGNAS
	size += std::sprintf(message + size, "[No]        `alignas()` is NOT available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_ALIGNAS
#endif//GASHA_HAS_ALIGNAS
#ifdef GASHA_ALIGNAS_LIMIT
	size += std::sprintf(message + size, "            liomit of `alignas()` = %d\n", GASHA_ALIGNAS_LIMIT);
#endif//GASHA_ALIGNAS_LIMIT

	//【C++11仕様】alignof
#ifdef GASHA_HAS_ALIGNOF
	size += std::sprintf(message + size, "[Yes]       `alignof()` is AVAILABLE.\n");
#else//GASHA_HAS_ALIGNOF
#ifdef GASHA_HAS_CAMOUFLAGE_ALIGNOF
	size += std::sprintf(message + size, "[Camouflage]`alignof()` is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_ALIGNOF
	size += std::sprintf(message + size, "[No]        `alignof()` is NOT available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_ALIGNOF
#endif//GASHA_HAS_ALIGNOF

	//【C++11仕様】alignas(alignof(T))
#ifdef GASHA_IS_ALLOWED_ALIGNAS_OF
	size += std::sprintf(message + size, "[Yes]       `alignas(alignof())` is AVAILABLE. -> [Yes] `GASHA_ALIGNAS_OF()` is AVAILABLE.\n");
#else//GASHA_HAS_CAMOUFLAGE_ALIGNAS_OF
#ifdef GASHA_HAS_CAMOUFLAGE_ALIGNAS_OF
	size += std::sprintf(message + size, "[No]        `alignas(alignof())` is NOT available. -> [Comouflage]`GASHA_ALIGNAS_OF()` is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_ALIGNAS_OF
	size += std::sprintf(message + size, "[No]        `alignas(alignof())` is NOT available. -> [No] `GASHA_ALIGNAS_OF()` is NOT available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_ALIGNAS_OF
#endif//GASHA_IS_ALLOWED_ALIGNAS_OF

	size += std::sprintf(message + size, "\n");
	
	//_aligned_malloc 
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
#ifdef GASHA_HAS_ALIGNED_MALLOC
	size += std::sprintf(message + size, "[Yes]       `_aligned_malloc()`(VC++ style) is AVAILABLE.\n");
#else//GASHA_HAS_ALIGNED_MALLOC
#ifdef GASHA_HASCAMOUFLAGE_ALIGNED_MALLOC
	size += std::sprintf(message + size, "[Camouflage]`_aligned_malloc()`(VC++ style) is available.\n");
#else//GASHA_HASCAMOUFLAGE_ALIGNED_MALLOC
	size += std::sprintf(message + size, "[No]        `_aligned_malloc()`(VC++ style) is available.\n");
#endif//GASHA_HASCAMOUFLAGE_ALIGNED_MALLOC
#endif//GASHA_HAS_ALIGNED_MALLOC
	
	//_aligned_free
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
#ifdef GASHA_HAS_ALIGNED_FREE
	size += std::sprintf(message + size, "[Yes]       `_aligned_free()`(VC++ style) is AVAILABLE.\n");
#else//GASHA_HAS_ALIGNED_FREE
#ifdef GASHA_HASCAMOUFLAGE_ALIGNED_FREE
	size += std::sprintf(message + size, "[Camouflage]`_aligned_free()`(VC++ style) is available.\n");
#else//GASHA_HASCAMOUFLAGE_ALIGNED_FREE
	size += std::sprintf(message + size, "[No]        `_aligned_free()`(VC++ style) is available.\n");
#endif//GASHA_HASCAMOUFLAGE_ALIGNED_FREE
#endif//GASHA_HAS_ALIGNED_FREE

	//mallinfo
	//※GCC独自実装
#ifdef GASHA_HAS_MALLINFO
	size += std::sprintf(message + size, "[Yes]       `mallinfo()`(GCC style) is AVAILABLE.\n");
#else//GASHA_HAS_MALLINFO
	size += std::sprintf(message + size, "[No]        `mallinfo()`(GCC style) is available.\n");
#endif//GASHA_HAS_MALLINFO

	//no_inline
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
#ifdef GASHA_HAS_NO_INLINE
	size += std::sprintf(message + size, "[Yes]       `GASHA_NOINLINE` is AVAILABLE.\n");
#else//GASHA_HAS_NO_INLINE
#ifdef GASHA_HAS_CAMOUFLAGE_NO_INLINE
	size += std::sprintf(message + size, "[Camouflage]`GASHA_NOINLINE` is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_NO_INLINE
	size += std::sprintf(message + size, "[No]        `GASHA_NOINLINE` is NOT available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_NO_INLINE
#endif//GASHA_HAS_NO_INLINE

	//always_inline
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
#ifdef GASHA_HAS_ALWAYS_INLINE
	size += std::sprintf(message + size, "[Yes]       `GASHA_ALWAYS_INLINE` is AVAILABLE.\n");
#else//GASHA_HAS_ALWAYS_INLINE
#ifdef GASHA_HAS_CAMOUFLAGE_ALWAYS_INLINE
	size += std::sprintf(message + size, "[Camouflage]`GASHA_ALWAYS_INLINE` is available.\n");
#else//GASHA_HAS_CAMOUFLAGE_ALWAYS_INLINE
	size += std::sprintf(message + size, "[No]        `GASHA_ALWAYS_INLINE` is NOT available.\n");
#endif//GASHA_HAS_CAMOUFLAGE_ALWAYS_INLINE
#endif//GASHA_HAS_ALWAYS_INLINE

	//マクロ
	size += std::sprintf(message + size, "\n");

	//マクロ：__FILE__
#ifdef GASHA_HAS_MACRO_FILE
	size += std::sprintf(message + size, "[Yes]       `__FILE__` is AVAILABLE. (ex. __FILE__ = \"%s\")\n", __FILE__);
#else//GASHA_HAS_MACRO_FILE
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_FILE
	size += std::sprintf(message + size, "[Camouflage]`__FILE__` is available. (ex. __FILE__ = \"%s\")\n", __FILE__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_FILE
#ifdef GASHA_HAS_DUMMY_MACRO_FILE
	size += std::sprintf(message + size, "[Dummy]     `__FILE__` is available. (ex. __FILE__ = \"%s\")\n", __FILE__);
#else//GASHA_HAS_DUMMY_MACRO_FILE
	size += std::sprintf(message + size, "[No]        `__FILE__` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_FILE
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_FILE
#endif//GASHA_HAS_MACRO_FILE

	//マクロ：__LINE__
#ifdef GASHA_HAS_MACRO_LINE
	size += std::sprintf(message + size, "[Yes]       `__LINE__` is AVAILABLE. (ex. __LINE__ = %d)\n", __LINE__);
#else//GASHA_HAS_MACRO_LINE
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_LINE
	size += std::sprintf(message + size, "[Camouflage]`__LINE__` is available. (ex. __LINE__ = %d)\n", __LINE__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_LINE
#ifdef GASHA_HAS_DUMMY_MACRO_LINE
	size += std::sprintf(message + size, "[Dummy]     `__LINE__` is available. (ex. __LINE__ = %d)\n", __LINE__);
#else//GASHA_HAS_DUMMY_MACRO_LINE
	size += std::sprintf(message + size, "[No]        `__LINE__` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_LINE
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_LINE
#endif//GASHA_HAS_MACRO_LINE

	//マクロ：__DATE__
#ifdef GASHA_HAS_MACRO_DATE
	size += std::sprintf(message + size, "[Yes]       `__DATE__` is AVAILABLE. (ex. __DATE__ = \"%s\")\n", __DATE__);
#else//GASHA_HAS_MACRO_DATE
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_DATE
	size += std::sprintf(message + size, "[Camouflage]`__DATE__` is available. (ex. __DATE__ = \"%s\")\n", __DATE__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_DATE
#ifdef GASHA_HAS_DUMMY_MACRO_DATE
	size += std::sprintf(message + size, "[Dummy]     `__DATE__` is available. (ex. __DATE__ = \"%s\")\n", __DATE__);
#else//GASHA_HAS_DUMMY_MACRO_DATE
	size += std::sprintf(message + size, "[No]        `__DATE__` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_DATE
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_DATE
#endif//GASHA_HAS_MACRO_DATE

	//マクロ：__TIME__
#ifdef GASHA_HAS_MACRO_TIME
	size += std::sprintf(message + size, "[Yes]       `__TIME__` is AVAILABLE. (ex. __TIME__ = \"%s\")\n", __TIME__);
#else//GASHA_HAS_MACRO_TIME
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_TIME
	size += std::sprintf(message + size, "[Camouflage]`__TIME__` is available. (ex. __TIME__ = \"%s\")\n", __TIME__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_TIME
#ifdef GASHA_HAS_DUMMY_MACRO_TIME
	size += std::sprintf(message + size, "[Dummy]     `__TIME__` is available. (ex. __TIME__ = \"%s\")\n", __TIME__);
#else//GASHA_HAS_DUMMY_MACRO_TIME
	size += std::sprintf(message + size, "[No]        `__TIME__` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_TIME
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_TIME
#endif//GASHA_HAS_MACRO_TIME

	//マクロ：__TIMESTAMP__
#ifdef GASHA_HAS_MACRO_TIMESTAMP
	size += std::sprintf(message + size, "[Yes]       `__TIMESTAMP__` is AVAILABLE. (ex. __TIMESTAMP__ = \"%s\")\n", __TIMESTAMP__);
#else//GASHA_HAS_MACRO_TIMESTAMP
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_TIMESTAMP
	size += std::sprintf(message + size, "[Camouflage]`__TIMESTAMP__` is available. (ex. __TIMESTAMP__ = \"%s\")\n", __TIMESTAMP__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_TIMESTAMP
#ifdef GASHA_HAS_DUMMY_MACRO_TIMESTAMP
	size += std::sprintf(message + size, "[Dummy]     `__TIMESTAMP__` is available. (ex. __TIMESTAMP__ = \"%s\")\n", __TIMESTAMP__);
#else//GASHA_HAS_DUMMY_MACRO_TIMESTAMP
	size += std::sprintf(message + size, "[No]        `__TIMESTAMP__` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_TIMESTAMP
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_TIMESTAMP
#endif//GASHA_HAS_MACRO_TIMESTAMP

	//マクロ：__FUNCTION__
#ifdef GASHA_HAS_MACRO_FUNCTION
	size += std::sprintf(message + size, "[Yes]       `__FUNCTION__` is AVAILABLE. (ex. __FUNCTION__ = \"%s\")\n", __FUNCTION__);
#else//GASHA_HAS_MACRO_FUNCTION
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_FUNCTION
	size += std::sprintf(message + size, "[Camouflage]`__FUNCTION__` is available. (ex. __FUNCTION__ = \"%s\")\n", __FUNCTION__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_FUNCTION
#ifdef GASHA_HAS_DUMMY_MACRO_FUNCTION
	size += std::sprintf(message + size, "[Dummy]     `__FUNCTION__` is available. (ex. __FUNCTION__ = \"%s\")\n", __FUNCTION__);
#else//GASHA_HAS_DUMMY_MACRO_FUNCTION
	size += std::sprintf(message + size, "[No]        `__FUNCTION__` is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_FUNCTION
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_FUNCTION
#endif//GASHA_HAS_MACRO_FUNCTION

	//マクロ：__PRETTY_FUNCTION__
	//※GCC固有仕様
#ifdef GASHA_HAS_MACRO_PRETTY_FUNCTION
	size += std::sprintf(message + size, "[Yes]       `__PRETTY_FUNCTION__`(GCC Style) is AVAILABLE. (ex. __PRETTY_FUNCTION__ = \"%s\")\n", __PRETTY_FUNCTION__);
#else//GASHA_HAS_MACRO_PRETTY_FUNCTION
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_PRETTY_FUNCTION
	size += std::sprintf(message + size, "[Camouflage]`__PRETTY_FUNCTION__`(GCC Style) is available. (ex. __PRETTY_FUNCTION__ = \"%s\")\n", __PRETTY_FUNCTION__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_PRETTY_FUNCTION
#ifdef GASHA_HAS_DUMMY_MACRO_PRETTY_FUNCTION
	size += std::sprintf(message + size, "[Dummy]     `__PRETTY_FUNCTION__`(GCC Style) is available. (ex. __PRETTY_FUNCTION__ = \"%s\")\n", __PRETTY_FUNCTION__);
#else//GASHA_HAS_DUMMY_MACRO_PRETTY_FUNCTION
	size += std::sprintf(message + size, "[No]        `__PRETTY_FUNCTION__`(GCC Style) is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_PRETTY_FUNCTION
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_PRETTY_FUNCTION
#endif//GASHA_HAS_MACRO_PRETTY_FUNCTION

	//マクロ：__func__
	//※GCC固有仕様
#ifdef GASHA_HAS_MACRO_func
	size += std::sprintf(message + size, "[Yes]       `__func__`(GCC Style) is AVAILABLE. (ex. __func__ = \"%s\")\n", __func__);
#else//GASHA_HAS_MACRO_func
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_func
	size += std::sprintf(message + size, "[Camouflage]`__func__`(GCC Style) is available. (ex. __func__ = \"%s\")\n", __func__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_func
#ifdef GASHA_HAS_DUMMY_MACRO_func
	size += std::sprintf(message + size, "[Dummy]     `__func__`(GCC Style) is available. (ex. __func__ = \"%s\")\n", __func__);
#else//GASHA_HAS_DUMMY_MACRO_func
	size += std::sprintf(message + size, "[No]        `__func__`(GCC Style) is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_func
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_func
#endif//GASHA_HAS_MACRO_func

	//マクロ：__FUNCSIG__
	//※VC++固有仕様
#ifdef GASHA_HAS_MACRO_FUNCSIG
	size += std::sprintf(message + size, "[Yes]       `__FUNCSIG__`(VC++ Style) is AVAILABLE. (ex. __FUNCSIG__ = \"%s\")\n", __FUNCSIG__);
#else//GASHA_HAS_MACRO_FUNCSIG
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_FUNCSIG
	size += std::sprintf(message + size, "[Camouflage]`__FUNCSIG__`(VC++ Style) is available. (ex. __FUNCSIG__ = \"%s\")\n", __FUNCSIG__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_FUNCSIG
#ifdef GASHA_HAS_DUMMY_MACRO_FUNCSIG
	size += std::sprintf(message + size, "[Dummy]     `__FUNCSIG__`(VC++ Style) is available. (ex. __FUNCSIG__ = \"%s\")\n", __FUNCSIG__);
#else//GASHA_HAS_DUMMY_MACRO_FUNCSIG
	size += std::sprintf(message + size, "[No]        `__FUNCSIG__`(VC++ Style) is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_FUNCSIG
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_FUNCSIG
#endif//GASHA_HAS_MACRO_FUNCSIG

	//マクロ：__FUNCDNAME__
	//※VC++固有仕様
#ifdef GASHA_HAS_MACRO_FUNCDNAME
	size += std::sprintf(message + size, "[Yes]       `__FUNCDNAME__`(VC++ Style) is AVAILABLE. (ex. __FUNCDNAME__ = \"%s\")\n", __FUNCDNAME__);
#else//GASHA_HAS_MACRO_FUNCDNAME
#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_FUNCDNAME
	size += std::sprintf(message + size, "[Camouflage]`__FUNCDNAME__`(VC++ Style) is available. (ex. __FUNCDNAME__ = \"%s\")\n", __FUNCDNAME__);
#else//GASHA_HAS_CAMOUFLAGE_MACRO_FUNCDNAME
#ifdef GASHA_HAS_DUMMY_MACRO_FUNCDNAME
	size += std::sprintf(message + size, "[Dummy]     `__FUNCDNAME__`(VC++ Style) is available. (ex. __FUNCDNAME__ = \"%s\")\n", __FUNCDNAME__);
#else//GASHA_HAS_DUMMY_MACRO_FUNCDNAME
	size += std::sprintf(message + size, "[No]        `__FUNCDNAME__`(VC++ Style) is NOT available.\n");
#endif//GASHA_HAS_DUMMY_MACRO_FUNCDNAME
#endif//GASHA_HAS_CAMOUFLAGE_MACRO_FUNCDNAME
#endif//GASHA_HAS_MACRO_FUNCDNAME

	//独自拡張マクロ
	size += std::sprintf(message + size, "\n");
	size += std::sprintf(message + size, "[Yes]       `GASHA_FUNC_NAME()` is AVAILABLE. (ex. GASHA_FUNC_NAME() = \"%s\")\n", GASHA_FUNC_NAME());
	size += std::sprintf(message + size, "[Yes]       `GASHA_SRC_FILE()` is AVAILABLE. (ex. GASHA_SRC_FILE() = \"%s\")\n", GASHA_SRC_FILE());
	size += std::sprintf(message + size, "[Yes]       `GASHA_SRC_FILE_LINE()` is AVAILABLE. (ex. GASHA_SRC_FILE_LINE() = \"%s\")\n", GASHA_SRC_FILE_LINE());
	size += std::sprintf(message + size, "[Yes]       `GASHA_SRC_FILE_LINE_TIME()` is AVAILABLE. (ex. GASHA_SRC_FILE_LINE_TIME() = \"%s\")\n", GASHA_SRC_FILE_LINE_TIME());

	size += std::sprintf(message + size, "\n");
	size += std::sprintf(message + size, "------------------------------------------------------------------------------\n");
	size += std::sprintf(message + size, "[ Build-configuratoin ]\n");

	//ビルド設定
	
	size += std::sprintf(message + size, "\n");
	size += std::sprintf(message + size, "Configuration name: %s\n", GASHA_BUILD_CONFIG_NAME);
	size += std::sprintf(message + size, "\n");

	//#define GASHA_BUILD_CONFIG_IS_DEBUG//フルデバッグ設定
	//#define GASHA_BUILD_CONFIG_IS_DEBUG_MODERATE//プログラム開発向け設定
	//#define GASHA_BUILD_CONFIG_IS_DEBUG_OPT//コンテンツ制作・QA向け設定
	//#define GASHA_BUILD_CONFIG_IS_REGRESSION_TEST//自動回帰テスト向け設定
	//#define GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE//製品テスト向け設定
	//#define GASHA_BUILD_CONFIG_IS_RELEASE//製品向け設定

#ifdef GASHA_HAS_DEBUG_FEATURE//デバッグ機能有効化
	size += std::sprintf(message + size, "[Yes] 'Debug-feature' is AVAILABLE.");
#ifdef GASHA_HAS_VERBOSE_DEBUG//冗長デバッグ機能有効化
	size += std::sprintf(message + size, " (with VERBOSE mode)");
#endif//GASHA_HAS_VERBOSE_DEBUG
#else//GASHA_HAS_DEBUG_FEATURE
	size += std::sprintf(message + size, "[No]  'Debug-feature' is NOT available.");
#endif//GASHA_HAS_DEBUG_FEATURE
	size += std::sprintf(message + size, "\n");

#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション有効化
	size += std::sprintf(message + size, "[Yes] 'Assertion' is ENABLED.");
#else//GASHA_ASSERTION_IS_ENABLED
	size += std::sprintf(message + size, "[No]  'Assertion' is DISABLED.");
#endif//GASHA_ASSERTION_IS_ENABLED
	size += std::sprintf(message + size, "\n");

#ifdef GASHA_NO_OPTIMIZED//最適化なし
	size += std::sprintf(message + size, "[No]  'Optimize' is none.");
#endif//GASHA_NO_OPTIMIZED
#ifdef GASHA_OPTIMIZED_MODERATELY//適度に最適化
	size += std::sprintf(message + size, "[Yes] 'Optimize' is moderate.");
#endif//GASHA_OPTIMIZED_MODERATELY
#ifdef GASHA_OPTIMIZED//最大限の最適化
	size += std::sprintf(message + size, "[Yes] 'Optimize' is MAX.");
#endif//GASHA_OPTIMIZED
	size += std::sprintf(message + size, "\n");
	
#ifdef GASHA_FILE_SYSTEM_IS_ROM//ROM専用ファイルシステム
	size += std::sprintf(message + size, "[---] 'File-sytem' is for ROM.");
#endif//GASHA_FILE_SYSTEM_IS_ROM
#ifdef GASHA_FILE_SYSTEM_IS_FLEXIBLE//ローカル／ROM切り替えファイルシステム（ローカルデータで設定）
	size += std::sprintf(message + size, "[---] 'File-system' is flexible (for ROM or for local-PC).");
#endif//GASHA_FILE_SYSTEM_IS_FLEXIBLE
	size += std::sprintf(message + size, "\n");

#ifdef GASHA_UNITE_TEST_ENABLED//ユニットテスト（の仕組みが）有効
	size += std::sprintf(message + size, "[Yes] 'Unit-test' is AVAILABLE.");
#ifdef GASHA_IS_REGRESSION_TEST//回帰テストモード有効
	size += std::sprintf(message + size, " (woth REGRESSION-TEST)");
#endif//GASHA_IS_REGRESSION_TEST
#else//GASHA_UNITE_TEST_ENABLED
	size += std::sprintf(message + size, "[[No] 'Unit-test' is NOT available.");
#endif//GASHA_UNITE_TEST_ENABLED
	size += std::sprintf(message + size, "\n");

#ifdef GASHA_HAS_SYNBOMS//シンボル情報あり
	size += std::sprintf(message + size, "[Yes] 'Symbols' are contained.");
#endif//GASHA_HAS_SYNBOMS
#ifdef GASHA_IS_STRIPPED_SYMBOLS//シンボル情報なし
	size += std::sprintf(message + size, "[No]  'Symbols' are STRIPPED.");
#endif//GASHA_IS_STRIPPED_SYMBOLS
	size += std::sprintf(message + size, "\n");

	size += std::sprintf(message + size, "\n");
	size += std::sprintf(message + size, "------------------------------------------------------------------------------\n");

	return !has_error;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_BUIILD_SETTINGS_DIAG_INL

// End of file

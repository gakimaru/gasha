#pragma once
#ifndef GASHA_INCLUDED_LANGUAGE_AUTO_SETTINGS_H_
#define GASHA_INCLUDED_LANGUAGE_AUTO_SETTINGS_H_

//--------------------------------------------------------------------------------
// build_settings/language_auto_settings.h
// 言語仕様自動判別・設定
//
// 依存するヘッダー：project_first_settings.h//プロジェクト固有のビルド設定（先行設定）
//                   compiler_auto_settings.h//コンパイラ自動判別・設定
//                   platform_auto_settings.h//プラットフォーム自動判別・設定
//                   ※build_settings.h により、依存関係順にインクルード
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//言語機能自動判別・設定

//キーワード偽装禁止設定の調整
//※ライブラリビルド時はキーワード偽装の禁止を行わない
#ifdef GASHA_BUILD_FOR_LIB

	#ifdef GASHA_NODEF_NULLPTR
		#undef GASHA_NODEF_NULLPTR//nullptr の偽装を許可しない
	#endif//GASHA_NODEF_NULLPTR

	#ifdef GASHA_NODEF_OVERRIDE
		#undef GASHA_NODEF_OVERRIDE//overrideの偽装を許可しない
	#endif//GASHA_NODEF_OVERRIDE

	#ifdef GASHA_NODEF_FINAL
		#undef GASHA_NODEF_FINAL//finalの偽装を許可しない
	#endif//GASHA_NODEF_FINAL

	#ifdef GASHA_NODEF_CONSTEXPR
		#undef GASHA_NODEF_CONSTEXPR//constexprの偽装を許可しない
	#endif//GASHA_NODEF_CONSTEXPR

	#ifdef GASHA_NODEF_STATIC_ASSERT
		#undef GASHA_NODEF_STATIC_ASSERT//static_assertの偽装を許可しない
	#endif//GASHA_NODEF_STATIC_ASSERT

	#ifdef GASHA_NODEF_THREAD_LOCAL
		#undef GASHA_NODEF_THREAD_LOCAL//thread_localの偽装を許可しない
	#endif//GASHA_NODEF_THREAD_LOCAL

	#ifdef GASHA_NODEF_NOEXCEPT
		#undef GASHA_NODEF_NOEXCEPT//noexceptの偽装を許可しない
	#endif//GASHA_NODEF_NOEXCEPT

	#ifdef GASHA_NODEF_ALIGNAS
		#undef GASHA_NODEF_ALIGNAS//alignas()の偽装を許可しない
	#endif//GASHA_NODEF_ALIGNAS

	#ifdef GASHA_NODEF_ALIGNOF
		#undef GASHA_NODEF_ALIGNOF//alignof()の偽装を許可しない
	#endif//GASHA_NODEF_ALIGNOF

	#ifdef GASHA_NODEF_PRETTY_FUNCTION
		#undef GASHA_NODEF_PRETTY_FUNCTION//__PRETTY_FUNCTION__の偽装を許可しない
	#endif//GASHA_NODEF_PRETTY_FUNCTION

	#ifdef GASHA_NODEF_func
		#undef GASHA_NODEF_func//__func__の偽装を許可しない
	#endif//GASHA_NODEF_func

	#ifdef GASHA_NODEF_FUNCSIG
		#undef GASHA_NODEF_FUNCSIG//__FUNCSIG__の偽装を許可しない
	#endif//GASHA_NODEF_FUNCSIG

	#ifdef GASHA_NODEF_FUNCDNAME
		#undef GASHA_NODEF_FUNCDNAME//__FUNCDNAME__の偽装を許可しない
	#endif//GASHA_NODEF_FUNCDNAME

	#ifdef GASHA_NODEF_ALIGNED_MALLOC
		#undef GASHA_NODEF_ALIGNED_MALLOC//_aligned_malloc()の偽装を許可しない
	#endif//GASHA_NODEF_ALIGNED_MALLOC

	#ifdef GASHA_NODEF_ALIGNED_FREE
		#undef GASHA_NODEF_ALIGNED_FREE//_aligned_free()の偽装を許可しない
	#endif//GASHA_NODEF_ALIGNED_FREE

	#ifdef GASHA_NODEF_CPUID
		#undef GASHA_NODEF_CPUID//__cpuid(), _xgetbv(), _XCR_XFEATURE_ENABLED_MASK の偽装を許可しない
	#endif//GASHA_NODEF_CPUID

#endif//GASHA_BUILD_FOR_LIB

//----------------------------------------
//C++11仕様対応
//参考：https://sites.google.com/site/cpprefjp/implementation-status
//参考(VC++)：http://msdn.microsoft.com/ja-jp/library/hh567368.aspx
//            ※VC++2010以降、C++11仕様を部分実装
//参考(GCC)：http://gcc.gnu.org/projects/cxx0x.html
//           ※GCC 4.3 C++0x Status
//           　GCC 4.4 C++0x Status
//           　GCC 4.5 C++0x Status
//           　GCC 4.6 C++0x Status
//           　GCC 4.7 C++11 Status
//           　GCC 4.8 C++11 Status

//--------------------
//【C++11仕様】nullptr
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1600//VC++10.0(2010)以降
		#define GASHA_HAS_NULLPTR
	#else//_MSC_VER
		//static const void* nullptr = 0;
		#define nullptr 0
		#define GASHA_HAS_CAMOUFLAGE_NULLPTR
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 6)
		#define GASHA_HAS_NULLPTR
	#else//GASHA_HAS_CPP11
		//static const void* nullptr = 0;
		#define nullptr 0
		#define GASHA_HAS_CAMOUFLAGE_NULLPTR
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC
//※偽装禁止チェック
#ifdef GASHA_NODEF_NULLPTR
	#ifdef GASHA_HAS_CAMOUFLAGE_NULLPTR
		#undef nullptr
		#undef GASHA_HAS_CAMOUFLAGE_NULLPTR
	#endif//GASHA_HAS_CAMOUFLAGE_NULLPTR
#endif//GASHA_NODEF_NULLPTR

//--------------------
//【C++11仕様】override指定子
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1400//VC++8.0(2005)以降
		#define GASHA_HAS_OVERRIDE
	#else//_MSC_VER
		#define override//ダミー
		#define GASHA_HAS_DUMMY_OVERRIDE
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 7)
		#define GASHA_HAS_OVERRIDE
	#else//GASHA_HAS_CPP11
		#define override//ダミー
		#define GASHA_HAS_DUMMY_OVERRIDE
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC
//※偽装禁止チェック
#ifdef GASHA_NODEF_OVERRIDE
	#ifdef GASHA_HAS_DUMMY_OVERRIDE
		#undef override
		#undef GASHA_HAS_DUMMY_OVERRIDE
	#endif//GASHA_HAS_DUMMY_OVERRIDE
#endif//GASHA_NODEF_OVERRIDE

//--------------------
//【C++11仕様】final指定子
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1700//VC++11.0(2012)以降
		#define GASHA_HAS_FINAL
	#else//_MSC_VER
		#define final//ダミー
		#define GASHA_HAS_DUMMY_FINAL
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 7)
		#define GASHA_HAS_FINAL
	#else//GASHA_HAS_CPP11
		#define final//ダミー
		#define GASHA_HAS_DUMMY_FINAL
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC
//※偽装禁止チェック
#ifdef GASHA_NODEF_FINAL
	#ifdef GASHA_HAS_DUMMY_FINAL
		#undef final
		#undef GASHA_HAS_DUMMY_FINAL
	#endif//GASHA_HAS_DUMMY_FINAL
#endif//GASHA_NODEF_FINAL

//--------------------
//【C++11仕様】auto型推論
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1600//VC++10.0(2010)以降
		#define GASHA_HAS_AUTO
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 4)
		#define GASHA_HAS_AUTO
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】decltype型指定子
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1600//VC++10.0(2010)以降
		#define GASHA_HAS_DECLTYPE
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 3)
		#define GASHA_HAS_DECLTYPE
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】default/detete宣言
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1800//VC++12.0(2013)以降
		#define GASHA_HAS_DEFAULT_AND_DELETE
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 4)
		#define GASHA_HAS_DEFAULT_AND_DELETE
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】constexpr修飾子
#ifdef GASHA_IS_VC
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）
		#define GASHA_HAS_CONSTEXPR
	#else//_MSC_VER
		//#define constexpr const
		#define constexpr//ダミー
		#define GASHA_HAS_DUMMY_CONSTEXPR
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 6)
		#define GASHA_HAS_CONSTEXPR
	#else//GASHA_HAS_CPP11
		//#define constexpr const
		#define constexpr//ダミー
		#define GASHA_HAS_DUMMY_CONSTEXPR
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC
//※偽装禁止チェック
#ifdef GASHA_NODEF_CONSTEXPR
	#ifdef GASHA_HAS_DUMMY_CONSTEXPR
		#undef constexpr
		#undef GASHA_HAS_DUMMY_CONSTEXPR
	#endif//GASHA_HAS_DUMMY_CONSTEXPR
#endif//GASHA_NODEF_CONSTEXPR

//--------------------
//【C++11仕様】ユーザー定義リテラル
#ifdef GASHA_IS_VC
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）※しばらく対応はなさそう
		#define GASHA_HAS_USER_DEFINED_LITERAL
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 7)
		#define GASHA_HAS_USER_DEFINED_LITERAL
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】ラムダ式
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1600//VC++10.0(2010)以降
		#define GASHA_HAS_LAMBDA_EXPRESSION
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 5)
		#define GASHA_HAS_LAMBDA_EXPRESSION
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】範囲に基づくforループ
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1800//VC++12.0(2013)以降
		#define GASHA_HAS_RANGE_BASED_FOR
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 6)
		#define GASHA_HAS_RANGE_BASED_FOR
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】列挙型の型付け
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1800//VC++11.1(2012)以降 ※VC++12(2013)以降とする
		#define GASHA_HAS_ENUM_CLASS
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 4)
		#define GASHA_HAS_ENUM_CLASS
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】可変長引数テンプレート
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1800//VC++12.1(2013)以降 ※VC++12(2013)以降とする
		#define GASHA_HAS_VARIADIC_TEMPLATE
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 4)
		#define GASHA_HAS_VARIADIC_TEMPLATE
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】エイリアステンプレート
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1800//VC++12.1(2013)以降 ※VC++12(2013)以降とする
		#define GASHA_HAS_TEMPLATE_ALIASES
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 7)
		#define GASHA_HAS_TEMPLATE_ALIASES
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】委譲コンストラクタ
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1800//VC++12.0(2013)以降
		#define GASHA_HAS_DELEGATING_CONSTRUCTORS
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 7)
		#define GASHA_HAS_DELEGATING_CONSTRUCTORS
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】継承コンストラクタ
#ifdef GASHA_IS_VC
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）
		#define GASHA_HAS_INHERITING_CONSTRUCTORS
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 8)
		#define GASHA_HAS_INHERITING_CONSTRUCTORS
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】一様初期化
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1800//VC++12.0(2013)以降
		#define GASHA_HAS_UNIFORM_INITIALIZATION
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 6)
		#define GASHA_HAS_UNIFORM_INITIALIZATION
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】初期化リスト
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1800//VC++12.0(2013)以降
		#define GASHA_HAS_INITIALIZER_LIST
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 4)
		#define GASHA_HAS_INITIALIZER_LIST
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】右辺値参照／ムーブセマンティクス
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1800//VC++12.0(2013)以降
		#define GASHA_HAS_RVALUE_REFERENCE
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 3)
		#define GASHA_HAS_RVALUE_REFERENCE
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】static_assert
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1600//VC++10.0(2010)以降
		#define GASHA_HAS_STATIC_ASSERT
	#else//_MSC_VER
		#define static_assert(expr, msg) typedef char GASHA_STATIC_ASSERT_DECLARATOIN[(expr) ? 1 : -1]
		#define GASHA_HAS_CUSTOM_STATIC_ASSERT
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 3)
		#define GASHA_HAS_STATIC_ASSERT
	#else//GASHA_HAS_CPP11
		#define static_assert(expr, msg) typedef char GASHA_STATIC_ASSERT_DECLARATOIN[(expr) ? 1 : -1]
		#define GASHA_HAS_CUSTOM_STATIC_ASSERT
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC
//※偽装禁止チェック
#ifdef GASHA_NODEF_STATIC_ASSERT
	#ifdef GASHA_HAS_CUSTOM_STATIC_ASSERT
		#undef static_assert
		#undef GASHA_HAS_CUSTOM_STATIC_ASSERT
	#endif//GASHA_HAS_CUSTOM_STATIC_ASSERT
#endif//GASHA_NODEF_STATIC_ASSERT

//--------------------
//【C++11仕様】thread_local：スレッドローカルストレージ（TLS）修飾子
#ifdef GASHA_IS_VC
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）
		#define GASHA_HAS_THREAD_LOCAL
	#else//_MSC_VER
		#define thread_local __declspec(thread)
		#define GASHA_HAS_CAMOUFLAGE_THREAD_LOCAL
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 8)
		#define GASHA_HAS_THREAD_LOCAL
	#else//GASHA_HAS_CPP11
		#define thread_local __thread
		#define GASHA_HAS_CAMOUFLAGE_THREAD_LOCAL
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC
//※偽装禁止チェック
#ifdef GASHA_NODEF_THREAD_LOCAL
	#ifdef GASHA_HAS_CAMOUFLAGE_THREAD_LOCAL
		#undef thread_local
		#undef GASHA_HAS_CAMOUFLAGE_THREAD_LOCAL
	#endif//GASHA_HAS_CAMOUFLAGE_THREAD_LOCAL
#endif//GASHA_NODEF_THREAD_LOCAL

//--------------------
//【C++11仕様】noexcept：例外無効
#ifdef GASHA_IS_WIN
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）
		#define GASHA_HAS_NOEXCEPT
		#define GASHA_NOEXCEPT noexcept
		#define GASHA_NOTHROW throw()
	#else//_MSC_VER
		#define GASHA_HAS_CAMOUFLAGE_NOEXCEPT
		#define GASHA_NOEXCEPT throw()
		#define GASHA_NOTHROW throw()
	#endif//_MSC_VER
#endif//GASHA_IS_WIN
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 6)
		#define GASHA_HAS_NOEXCEPT
		#define GASHA_NOEXCEPT noexcept
		#define GASHA_NOTHROW throw()
	#else//GASHA_HAS_CPP11
		#define GASHA_HAS_CAMOUFLAGE_NOEXCEPT
		#define GASHA_NOEXCEPT throw()
		#define GASHA_NOTHROW throw()
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC
//※偽装禁止チェック
#ifdef GASHA_NODEF_NOEXCEPT
	#ifdef GASHA_HAS_CAMOUFLAGE_NOEXCEPT
		#undef noexcept
		#undef GASHA_HAS_CAMOUFLAGE_NOEXCEPT
	#endif//GASHA_HAS_CAMOUFLAGE_NOEXCEPT
#endif//GASHA_NODEF_NOEXCEPT

//--------------------
//【C++11仕様】alignas：アラインメント修飾子
//【注意】修飾子の指定位置がコンパイラによって異なる
//        ＜変数宣言＞
//        ・C++11 ... alignas(32) float pos[4];
//        ・VC++  ... __declspec(align(16)) float pos[4];
//        ・GCC   ... float pos[4] __attribute((aligned(16));
//        ＜型宣言＞
//        ・C++11 ... struct alignas(32) AVECT{ float pos[4]; };
//        ・VC++  ... __declspec(align(32)) struct vector{ float pos[4]; };
//                    struct __declspec(align(32)) vector{ float pos[4]; };
//        ・GCC   ... struct vector{ float pos[4]; } __attribute((aligned(32));
//                    struct __attribute((aligned(32)) vector{ float pos[4]; };
#ifdef GASHA_IS_WIN
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）
		#define GASHA_HAS_ALIGNAS
	#else//_MSC_VER
		#define alignas(n) __declspec(align(n))
		#define GASHA_HAS_CAMOUFLAGE_ALIGNAS
	#endif//_MSC_VER
#endif//GASHA_IS_WIN
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 8)
		#define GASHA_HAS_ALIGNAS
	#else//GASHA_HAS_CPP11
		#define alignas(n) __attribute__((aligned(n)))
		#define GASHA_HAS_CAMOUFLAGE_ALIGNAS
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC
//※偽装禁止チェック
#ifdef GASHA_NODEF_ALIGNAS
	#ifdef GASHA_HAS_CAMOUFLAGE_ALIGNAS
		#undef alignas
		#undef GASHA_HAS_CAMOUFLAGE_ALIGNAS
	#endif//GASHA_HAS_CAMOUFLAGE_ALIGNAS
#endif//GASHA_NODEF_ALIGNAS

//--------------------
//【C++11仕様】alignasの限界値
#ifdef GASHA_IS_WIN
	#define GASHA_ALIGNAS_LIMIT 128
#endif//GASHA_IS_WIN
#ifdef GASHA_IS_GCC
	#define GASHA_ALIGNAS_LIMIT 256
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】alignof：アラインメント取得関数
#ifdef GASHA_IS_WIN
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）
		#define GASHA_HAS_ALIGNOF
	#else//_MSC_VER
		#define alignof(T) __alignof(T)
		#define GASHA_HAS_CAMOUFLAGE_ALIGNOF
	#endif//_MSC_VER
#endif//GASHA_IS_WIN
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 5)
		#define GASHA_HAS_ALIGNOF
	#else//GASHA_HAS_CPP11
		#define alignof(T) __alignof__(T)
		#define GASHA_HAS_CAMOUFLAGE_ALIGNOF
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC
//※偽装禁止チェック
#ifdef GASHA_NODEF_ALIGNOF
	#ifdef GASHA_HAS_CAMOUFLAGE_ALIGNOF
		#undef alignof
		#undef GASHA_HAS_CAMOUFLAGE_ALIGNOF
	#endif//GASHA_HAS_CAMOUFLAGE_ALIGNOF
#endif//GASHA_NODEF_ALIGNOF

//--------------------
//【C++11仕様】alignof(alignas(T))：アラインメントを取得して適用
#ifdef GASHA_IS_WIN
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）
		#define GASHA_IS_ALLOWED_ALIGNAS_OF
		#define GASHA_ALIGNAS_OF(T) alignas(alignof(T))
	#else//_MSC_VER
		//#define GASHA_HAS_CAMOUFLAGE_ALIGNAS_OF
		//#define GASHA_ALIGNAS_OF(T) __declspec(align(__alignof(T)))
		#define GASHA_ALIGNAS_OF(T)
	#endif//_MSC_VER
#endif//GASHA_IS_WIN
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 5)
		#define GASHA_IS_ALLOWED_ALIGNAS_OF
		#define GASHA_ALIGNAS_OF(T) alignas(alignof(T))
	#else//GASHA_HAS_CPP11
		#define GASHA_HAS_CAMOUFLAGE_ALIGNAS_OF
		#define GASHA_ALIGNAS_OF(T) __attribute__((aligned(__alignof__(T))))
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//----------------------------------------
//非言語仕様対応　※方言吸収

//--------------------
//アラインメント指定付きメモリ確保関数
//※VC++仕様に合わせて下記関数を共通実装
//　void* _aligned_malloc(size_t size, size_t alignment);
//　void _aligned_free(void* p);
#ifdef GASHA_IS_VC
	#define GASHA_HAS_ALIGNED_MALLOC
	#define GASHA_HAS_ALIGNED_FREE
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#define GASHA_HAS_CAMOUFLAGE_ALIGNED_MALLOC
	#define GASHA_HAS_CAMOUFLAGE_ALIGNED_FREE
#endif//GASHA_IS_GCC
//※偽装禁止チェック
#ifdef GASHA_NODEF_ALIGNED_MALLOC
	#ifdef GASHA_HAS_CAMOUFLAGE_ALIGNED_MALLOC
		#undef GASHA_HAS_CAMOUFLAGE_ALIGNED_MALLOC
	#endif//GASHA_HAS_CAMOUFLAGE_ALIGNED_MALLOC
#endif//GASHA_NODEF_ALIGNED_MALLOC
//※偽装禁止チェック
#ifdef GASHA_NODEF_ALIGNED_FREE
	#ifdef GASHA_HAS_CAMOUFLAGE_ALIGNED_FREE
		#undef GASHA_HAS_CAMOUFLAGE_ALIGNED_FREE
	#endif//GASHA_HAS_CAMOUFLAGE_ALIGNED_FREE
#endif//GASHA_NODEF_ALIGNED_FREE

//--------------------
//mallinfo
#ifdef GASHA_IS_GCC//※暫定（GCCだからといって使えるとは限らない）
	#define GASHA_HAS_MALLINFO
#endif//GASHA_IS_GCC

//--------------------
//CPU情報
//※VC++仕様に合わせて下記関数を共通実装
//  void __cpuid(int cpu_info[4], const int type);
//  std::uint64_t _xgetbv(const std::uint32_t xcr);
//  #define _XCR_XFEATURE_ENABLED_MASK 0 ... _xgetbv() 引数用
#ifdef GASHA_IS_VC
	#ifdef GASHA_IS_X86
		#define GASHA_HAS_CPUID
	#endif//GASHA_IS_X86
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#ifdef GASHA_IS_X86
		#define GASHA_HAS_CAMOUFLAGE_CPUID
	#endif//GASHA_IS_X86
#endif//GASHA_IS_GCC
//※偽装禁止チェック
#ifdef GASHA_NODEF_CPUID
	#ifdef GASHA_HAS_CAMOUFLAGE_CPUID
		#undef GASHA_HAS_CAMOUFLAGE_CPUID
	#endif//GASHA_HAS_CAMOUFLAGE_CPUID
#endif//GASHA_NODEF_CPUID

//--------------------
//noinline / always_inline
//※独自の共通化
#ifdef GASHA_IS_VC
	#define GASHA_NOINLINE __declspec(noinline)
	#define GASHA_ALWAYS_INLINE __forceinline
	#define GASHA_HAS_CAMOUFLAGE_NO_INLINE
	#define GASHA_HAS_CAMOUFLAGE_ALWAYS_INLINE
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#define GASHA_NOINLINE __attribute__ ((noinline))
	#define GASHA_ALWAYS_INLINE __attribute__ ((always_inline)) inline
	#define GASHA_HAS_CAMOUFLAGE_NO_INLINE
	#define GASHA_HAS_CAMOUFLAGE_ALWAYS_INLINE
#endif//GASHA_IS_GCC

//--------------------
//標準new/deleteの例外指定
#ifdef GASHA_IS_VC
	#define GASHA_STDNEW_THROW_IS_NOTHROW
	#define GASHA_STDNEW_NOTHROW_IS_NOTHROW
	#define GASHA_STDDELETE_THROW_IS_NOTHROW
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#define GASHA_STDNEW_THROW_IS_THROW
	#define GASHA_STDNEW_NOTHROW_IS_NOTHROW
	#define GASHA_STDDELETE_THROW_IS_NOTHROW
#endif//GASHA_IS_GCC
#ifdef GASHA_STDNEW_THROW_IS_THROW
	#define GASHA_STDNEW_THROW
#else//GASHA_STDNEW_THROW_IS_THROW
	#define GASHA_STDNEW_THROW throw()
#endif//GASHA_STDNEW_THROW_IS_THROW
#ifdef GASHA_STDNEW_NOTHROW_IS_THROW
	#define GASHA_STDNEW_NOTHROW
#else//GASHA_STDNEW_NOTHROW_IS_THROW
	#define GASHA_STDNEW_NOTHROW throw()
#endif//GASHA_STDNEW_NOTHROW_IS_THROW
#ifdef GASHA_STDDELETE_THROW_IS_THROW
	#define GASHA_STDDELETE_THROW
#else//GASHA_STDDELETE_THROW_IS_THROW
	#define GASHA_STDDELETE_THROW throw()
#endif//GASHA_STDDELETE_THROW_IS_THROW

//--------------------
//文字列関数
#ifdef GASHA_IS_VC
	#define GASHA_STDSTRNLENFUNC ::strnlen
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#define GASHA_STDSTRNLENFUNC ::strnlen
#endif//GASHA_IS_GCC

//----------------------------------------
//マクロ対応

//----------------------------------------
//関数名取得マクロ
//【注意】GCCの __FUNCTION__, __PRETTY_FUNCTION__ は関数なので、
//　　　　コンパイル時の文字列結合ができない点に注意
#ifdef GASHA_IS_VC
	#define GASHA_HAS_MACRO_FILE
	#define GASHA_HAS_MACRO_LINE
	#define GASHA_HAS_MACRO_DATE
	#define GASHA_HAS_MACRO_TIME
	#define GASHA_HAS_MACRO_TIMESTAMP
	#define GASHA_HAS_MACRO_FUNCTION
	#define GASHA_HAS_CAMOUFLAGE_MACRO_PRETTY_FUNCTION
	#define GASHA_HAS_CAMOUFLAGE_MACRO_func
	#define GASHA_HAS_MACRO_FUNCSIG
	#define GASHA_HAS_MACRO_FUNCDNAME
	#define __PRETTY_FUNCTION__ __FUNCSIG__
	#define __func__ __FUNCTION__
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#define GASHA_HAS_MACRO_FILE
	#define GASHA_HAS_MACRO_LINE
	#define GASHA_HAS_MACRO_DATE
	#define GASHA_HAS_MACRO_TIME
	#define GASHA_HAS_MACRO_TIMESTAMP
	#define GASHA_HAS_MACRO_FUNCTION
	#define GASHA_HAS_MACRO_PRETTY_FUNCTION
	#define GASHA_HAS_MACRO_func
	#define GASHA_HAS_CAMOUFLAGE_MACRO_FUNCSIG
	#define GASHA_HAS_DUMMY_MACRO_FUNCDNAME
	#define __FUNCSIG__ __PRETTY_FUNCTION__
	#define __FUNCDNAME__ ""
#endif//GASHA_IS_GCC
//※偽装禁止チェック
#ifdef GASHA_NODEF_PRETTY_FUNCTION
	#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_PRETTY_FUNCTION
		#undef __PRETTY_FUNCTION__
		#undef GASHA_HAS_CAMOUFLAGE_MACRO_PRETTY_FUNCTION
	#endif//GASHA_HAS_CAMOUFLAGE_MACRO_PRETTY_FUNCTION
#endif//GASHA_NODEF_PRETTY_FUNCTION
//※偽装禁止チェック
#ifdef GASHA_NODEF_func
	#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_func
		#undef __func__
		#undef GASHA_HAS_CAMOUFLAGE_MACRO_func
	#endif//GASHA_HAS_CAMOUFLAGE_MACRO_func
#endif//GASHA_NODEF_func
//※偽装禁止チェック
#ifdef GASHA_NODEF_FUNCSIG
	#ifdef GASHA_HAS_CAMOUFLAGE_MACRO_FUNCSIG
		#undef __FUNCSIG__
		#undef GASHA_HAS_CAMOUFLAGE_MACRO_FUNCSIG
	#endif//GASHA_HAS_CAMOUFLAGE_MACRO_FUNCSIG
#endif//GASHA_NODEF_FUNCSIG
//※偽装禁止チェック
#ifdef GASHA_NODEF_FUNCDNAME
	#ifdef GASHA_HAS_DUMMY_MACRO_FUNCDNAME
		#undef __FUNCDNAME__
		#undef GASHA_HAS_DUMMY_MACRO_FUNCDNAME
	#endif//GASHA_HAS_DUMMY_MACRO_FUNCDNAME
#endif//GASHA_NODEF_FUNCDNAME

#endif//GASHA_INCLUDED_LANGUAGE_AUTO_SETTINGS_H_

// End of file

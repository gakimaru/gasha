#pragma once
#ifndef GASHA_INCLUDED_LANGUAGE_AUTO_SETTINGS_H_
#define GASHA_INCLUDED_LANGUAGE_AUTO_SETTINGS_H_

//--------------------------------------------------------------------------------
// language_auto_settings.h
// 言語機能自動判別・設定
//
// 依存するヘッダー：project/first_settings.h//プロジェクト固有のビルド設定（先行設定）
//                   compiler_auto_settings.h//コンパイラ自動判別・設定
//                   platform_auto_settings.h//プラットフォーム自動判別・設定
//                   ※build_settings.h により、依存関係順にインクルード
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//言語機能自動判別・設定

//----------------------------------------
//関数名取得マクロ対応

//文字列化マクロ
#define GASHA_TO_STRING(s) #s
#define GASHA_TO_STRING_EX(s) GASHA_TO_STRING(s)

//関数名取得マクロ
//【注意】GCCの関数名 __FUNCTION__, __PRETTY_FUNCTION__ は関数なので、
//　　　　コンパイル時の文字列結合ができない
#ifdef GASHA_IS_VC
	#define __PRETTY_FUNCTION__ __FUNCSIG__
	#define __func__ __FUNCTION__
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#define __FUNCSIG__ __PRETTY_FUNCTION__
	#define __FUNCDNAME__ ""
#endif//GASHA_IS_GCC

//関数名取得マクロ
#define GASHA_FUNC_NAME() __PRETTY_FUNCTION__

//ソースファイル名＋行番号取得マクロ
#define GASHA_FILE_LINE() __FILE__ "(" GASHA_TO_STRING_EX(__LINE__) ")"
#define GASHA_FILE_LINE_TIME() __FILE__ "(" GASHA_TO_STRING_EX(__LINE__) ")[" __TIMESTAMP__ "]"

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
		#define GASHA_HAS_NULLPTR_PROXY
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 6)
		#define GASHA_HAS_NULLPTR
	#else//GASHA_HAS_CPP11
		//static const void* nullptr = 0;
		#define nullptr 0
		#define GASHA_HAS_NULLPTR_PROXY
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】override指定子
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1400//VC++8.0(2005)以降
		#define GASHA_HAS_OVERRIDE
	#else//_MSC_VER
		#define override//ダミー
		#define GASHA_HAS_OVERRIDE_DUMMY
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 7)
		#define GASHA_HAS_OVERRIDE
	#else//GASHA_HAS_CPP11
		#define override//ダミー
		#define GASHA_HAS_OVERRIDE_DUMMY
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】final指定子
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1700//VC++11.0(2012)以降
		#define GASHA_HAS_FINAL
	#else//_MSC_VER
		#define final//ダミー
		#define GASHA_HAS_FINAL_DUMMY
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 7)
		#define GASHA_HAS_FINAL
	#else//GASHA_HAS_CPP11
		#define final//ダミー
		#define GASHA_HAS_FINAL_DUMMY
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

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
		#define GASHA_HAS_CONSTEXPR_DUMMY
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 6)
		#define GASHA_HAS_CONSTEXPR
	#else//GASHA_HAS_CPP11
		//#define constexpr const
		#define constexpr//ダミー
		#define GASHA_HAS_CONSTEXPR_DUMMY
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

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
		#define GASHA_HAS_STATIC_ASSERT_PROXY
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 3)
		#define GASHA_HAS_STATIC_ASSERT
	#else//GASHA_HAS_CPP11
		#define static_assert(expr, msg) typedef char GASHA_STATIC_ASSERT_DECLARATOIN[(expr) ? 1 : -1]
		#define GASHA_HAS_STATIC_ASSERT_PROXY
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】thread_local：スレッドローカルストレージ（TLS）修飾子
#ifdef GASHA_IS_VC
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）
		#define GASHA_HAS_THREAD_LOCAL
	#else//_MSC_VER
		#define thread_local __declspec(thread)
		#define GASHA_HAS_THREAD_LOCAL_PROXY
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 8)
		#define GASHA_HAS_THREAD_LOCAL
	#else//GASHA_HAS_CPP11
		#define thread_local __thread
		#define GASHA_HAS_THREAD_LOCAL_PROXY
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

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
		#define GASHA_HAS_ALIGNAS_PROXY
	#endif//_MSC_VER
#endif//GASHA_IS_WIN
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 8)
		#define GASHA_HAS_ALIGNAS
	#else//GASHA_HAS_CPP11
		#define alignas(n) __attribute__((aligned(n)))
		#define GASHA_HAS_ALIGNAS_PROXY
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】alignof：アラインメント取得関数
#ifdef GASHA_IS_WIN
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）
		#define GASHA_HAS_ALIGNOF
	#else//_MSC_VER
		#define alignof(T) __alignof(T)
		#define GASHA_HAS_ALIGNOF_PROXY
	#endif//_MSC_VER
#endif//GASHA_IS_WIN
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 5)
		#define GASHA_HAS_ALIGNOF
	#else//GASHA_HAS_CPP11
		#define alignof(T) __alignof__(T)
		#define GASHA_HAS_ALIGNOF_PROXY
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//--------------------
//【C++11仕様】alignof(alignas(T))：アラインメントを取得して適用
#ifdef GASHA_IS_WIN
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）
		#define GASHA_IS_ALLOWED_ALIGNAS_OF
		#define GASHA_ALIGNAS_OF(T) alignas(alignof(T))
	#else//_MSC_VER
		//#define GASHA_HAS_ALIGNAS_OF_PROXY
		//#define GASHA_ALIGNAS_OF(T) __declspec(align(__alignof(T)))
		#define GASHA_ALIGNAS_OF(T)
	#endif//_MSC_VER
#endif//GASHA_IS_WIN
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 5)
		#define GASHA_IS_ALLOWED_ALIGNAS_OF
		#define GASHA_ALIGNAS_OF(T) alignas(alignof(T))
	#else//GASHA_HAS_CPP11
		#define GASHA_HAS_ALIGNAS_OF_PROXY
		#define GASHA_ALIGNAS_OF(T) __attribute__((aligned(__alignof__(T))))
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//----------------------------------------
//非言語仕様対応　※方言吸収

//--------------------
//アラインメント指定付きメモリ確保関数
//※VC++仕様に合わせて共通化
#ifdef GASHA_IS_GCC
	#include <cstddef>//std::size_t
	#include <stdlib.h>//posix_memalign()
	#include <memory.h>//free()
	inline void* _aligned_malloc(const std::size_t size, const std::size_t alignment)
	{
		void *p;
		int ret = posix_memalign(&p, alignment, size);
		return (ret == 0) ? p : 0;
	}
	inline void _aligned_free(void* p)
	{
		free(p);
	}
	#define GASHA_HAS_ALIGNED_MALLOC_PROXY
	#define GASHA_HAS_ALIGNED_FREE_PROXY
#endif//GASHA_IS_GCC
#ifdef GASHA_IS_VC
	#include <malloc.h>//_aligned_malloc(), _aligned_free()
	//void* _aligned_malloc(size_t size, size_t alignment);
	//void _aligned_free(void* p);
	#define GASHA_HAS_ALIGNED_MALLOC
	#define GASHA_HAS_ALIGNED_FREE
#endif//GASHA_IS_VC

//--------------------
//noinline / always_inline
#ifdef GASHA_IS_VC
	#define no_inline __declspec(noinline)
	#define always_inline __forceinline
	#define GASHA_HAS_NO_INLINE_PROXY
	#define GASHA_HAS_ALWAYS_INLINE_PROXY
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#define no_inline __attribute__ ((noinline))
	#define always_inline __attribute__ ((always_inline)) inline
	#define GASHA_HAS_NO_INLINE_PROXY
	#define GASHA_HAS_ALWAYS_INLINE_PROXY
#endif//GASHA_IS_GCC

#endif//GASHA_INCLUDED_LANGUAGE_AUTO_SETTINGS_H_

// End of file

#pragma once
#ifndef __LANGUAGE_AUTO_SETTINGS_H_
#define __LANGUAGE_AUTO_SETTINGS_H_

//--------------------------------------------------------------------------------
// language_auto_settings.h
// 言語機能自動判別・設定
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

//依存ヘッダー：project_build_settings_first.h
//              compiler_auto_settings.h
//              platform_auto_settings.h
//依存関係の解消：build_settings.h にてインクルード

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
//非言語仕様対応　※方言吸収

//noinline / always_inline
#ifdef GASHA_IS_VC
	#define no_inline __declspec(noinline)
	#define always_inline __forceinline
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#define no_inline __attribute__ ((noinline))
	#define always_inline __attribute__ ((always_inline)) inline
#endif//GASHA_IS_GCC

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

//【C++11仕様】nullptr
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1600//VC++10.0(2010)以降
		#define GASHA_HAS_NULLPTR
	#else//_MSC_VER
		//static const void* nullptr = 0;
		#define nullptr 0
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 6)
		#define GASHA_HAS_NULLPTR
	#else//GASHA_HAS_CPP11
		//static const void* nullptr = 0;
		#define nullptr 0
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//【C++11仕様】override指定子
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1400//VC++8.0(2005)以降
		#define GASHA_HAS_OVERRIDE
	#else//_MSC_VER
		#define override//ダミー
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 7)
		#define GASHA_HAS_OVERRIDE
	#else//GASHA_HAS_CPP11
		#define override//ダミー
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//【C++11仕様】final指定子
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1700//VC++11.0(2012)以降
		#define GASHA_HAS_FINAL
	#else//_MSC_VER
		#define final//ダミー
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 7)
		#define GASHA_HAS_FINAL
	#else//GASHA_HAS_CPP11
		#define final//ダミー
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//【C++11仕様】constexpr修飾子
#ifdef GASHA_IS_VC
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）
		#define GASHA_HAS_CONSTEXPR
	#else//_MSC_VER
		//#define constexpr const
		#define constexpr
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 6)
		#define GASHA_HAS_CONSTEXPR
	#else//GASHA_HAS_CPP11
		//#define constexpr const
		#define constexpr
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//【C++11仕様】ユーザー定義リテラル
#ifdef GASHA_IS_VC
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）
		#define GASHA_HAS_USER_DEFINED_LITERAL
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 7)
		#define GASHA_HAS_USER_DEFINED_LITERAL
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//【C++11仕様】static_assert
#ifdef GASHA_IS_VC
	#if _MSC_VER >= 1600//VC++10.0(2010)以後
		#define GASHA_HAS_STATIC_ASSERT
	#else//_MSC_VER
		#define STATIC_ASSERT(expr, msg) typedef char __STATIC_ASSERT_TYPE[(expr) ? 1 : -1]
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 3)
		#define GASHA_HAS_STATIC_ASSERT
	#else//GASHA_HAS_CPP11
		#define STATIC_ASSERT(expr, msg) typedef char __STATIC_ASSERT_TYPE[(expr) ? 1 : -1]
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//【C++11仕様】thread_local：スレッドローカルストレージ（TLS）修飾子
#ifdef GASHA_IS_VC
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）
		#define GASHA_HAS_THREAD_LOCAL
	#else//_MSC_VER
		#define thread_local __declspec(thread)
	#endif//_MSC_VER
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 8)
		#define GASHA_HAS_THREAD_LOCAL
	#else//GASHA_HAS_CPP11
		#define thread_local __thread
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

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
	#endif//_MSC_VER
#endif//GASHA_IS_WIN
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 8)
		#define GASHA_HAS_ALIGNAS
	#else//GASHA_HAS_CPP11
		#define alignas(n) __attribute__((aligned(n)))
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

//【C++11仕様】alignof：アラインメント取得関数
#ifdef GASHA_IS_WIN
	#if _MSC_VER > 1800//VC++12.0(2013)以後（暫定）
		#define GASHA_HAS_ALIGNOF
	#else//_MSC_VER
		#define alignof(T) __alignof(T)
	#endif//_MSC_VER
#endif//GASHA_IS_WIN
#ifdef GASHA_IS_GCC
	#if defined(GASHA_HAS_CPP11) && __GNUC_PREREQ(4, 5)
		#define GASHA_HAS_ALIGNOF
	#else//GASHA_HAS_CPP11
		#define alignof(T) __alignof__(T)
	#endif//GASHA_HAS_CPP11
#endif//GASHA_IS_GCC

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
#endif//GASHA_IS_GCC
#ifdef GASHA_IS_VC
#include <malloc.h>//_aligned_malloc(), _aligned_free()
//void* _aligned_malloc(size_t size, size_t alignment);
//void _aligned_free(void* p);
#endif//GASHA_IS_VC

#endif//__LANGUAGE_AUTO_SETTINGS_H_

// End of file

#pragma once
#ifndef __COMPILER_AUTO_SETTINGS_H_
#define __COMPILER_AUTO_SETTINGS_H_

//--------------------------------------------------------------------------------
// compiler_auto_settings.h
// コンパイラ自動判別・設定
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

//依存ヘッダー：project_build_settings_first.h
//依存関係の解消：build_settings.h にてインクルード

#ifdef _MSC_VER 
	#include <cstddef>//VC++では、これを最初にインクルードしておかないと、コンパイルに影響がでることがある。
                      //※先に #define constexpr や #define thread_local を行ったあとで、
	                  //　#include<cstddef>(std::size_tなど用)を行うと、以後の処理で不可解なコンパイルエラーが発生する。
	#define IS_VC//コンパイラ：Visual C++
	#define COMPILER_NAME "VC++"
	#define COMPILER_VER _MSC_VER//コンパイラバージョン（メジャーバージョン）
	#define COMPILER_MINOR 0//コンパイラバージョン（マイナーバージョン）
	//_MSC_VERの値
	//   600 ... C Compiler 6.0
	//   700 ... C/C++ Compiler 7.0
	//   800 ... Visual C++ 1.0
	//   900 ... Visual C++ 2.0
	//  1000 ... Visual C++ 4.0
	//  1010 ... Visual C++ 4.1
	//  1020 ... Visual C++ 4.2
	//  1100 ... Visual C++ 5.0 (Visual Studio 97)
	//  1200 ... Visual C++ 6.0 (Visual Studio 6.0)
	//  1300 ... Visual C++ 7.0 (Visual Studio.NET 2002)
	//  1310 ... Visual C++ 7.1 (Visual Studio.NET 2003)
	//  1400 ... Visual C++ 8.0 (Visual Studio 2005)
	//  1500 ... Visual C++ 9.0 (Visual Studio 2008)
	//  1600 ... Visual C++ 10.0 (Visual Studio 2010)
	//  1700 ... Visual C++ 11.0 (Visual Studio 2012)
	//  1800 ... Visual C++ 12.0 (Visual Studio 2013)
#endif//_MSC_VER
#ifdef __GNUC__
	#include <features.h>//__GNUC_PREREQマクロを使えるようにする
	#define IS_GCC//コンパイラ：GCC
	#define COMPILER_NAME "GCC"
	#define COMPILER_VER __GNUC__//コンパイラバージョン（メジャーバージョン）
	#define COMPILER_MINOR __GNUC_MINOR__//コンパイラバージョン（マイナーバージョン）
#endif//__GNUC__
#ifdef __cplusplus
	#define IS_CPP//言語：C++
	#define COMPILER_LANGUAGE "C++"
	#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
		//#define HAS_CPPTR1//言語：C++TR1対応（暫定）
		#define HAS_CPP11//言語：C++11対応
		#define CPP_VER 2011
	#endif
	#if __cplusplus >= 199711L
		#define HAS_CPP98//言語：C++98対応
		#define HAS_CPP03//言語：C++03対応（暫定）
		#ifndef CPP_VER
			//#define CPP_VER 1998
			#define CPP_VER 2003
		#endif//CPP_VER
	#endif
#else//__cplusplus
	#define IS_C//言語：C
	#define COMPILER_LANGUAGE "C"
	#define CPP_VER 0
#endif//__cplusplus

#endif//__COMPILER_AUTO_SETTINGS_H_

// End of file

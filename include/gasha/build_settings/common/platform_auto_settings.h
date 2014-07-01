#pragma once
#ifndef GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_H
#define GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_H

//--------------------------------------------------------------------------------
// platform_atuo_settings.h
// プラットフォーム自動判別・設定
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

//依存ヘッダー：project_build_settings_first.h
//              compiler_auto_settings.h
//依存関係の解消：build_settings.h にてインクルード

#ifdef _WIN32
	#define GASHA_IS_WIN//プラットフォーム：Windows
	#define GASHA_PLATFORM_NAME "Win"//プラットフォーム名
	#ifdef WINVER
		#define GASHA_PLATFORM_VER WINVER//プラットフォームバージョン ※別途_WIN32_WINNT/WINVERを設定する必要あり
		//_WIN32_WINNT / WINVER に指定可能な値
		//  0x500 ... Windows2000
		//  0x501 ... Windows XP / Windows 2003 Server
		//  0x502 ... Windows XP SP2 / Windows 2003 Server SP1
		//  0x600 ... Windows Vista / Windows 2008 Server
		//  0x700 ... Windows 7
	#else//WINVER	
		#define GASHA_PLATFORM_VER 0//プラットフォームバージョン
	#endif//WINVER
	#define GASHA_PLATFORM_MINOR 0//プラットフォームマイナーバージョン
	#ifdef _WIN64
		#define GASHA_IS_64BIT//プラットフォーム：64bit
		#define GASHA_PLATFORM_ARCHITECTURE_BITS 64//プラットフォームアーキテクチャ（ビット長）
	#else//_WIN64
		#define GASHA_IS_32BIT//プラットフォーム：32bit
		#define GASHA_PLATFORM_ARCHITECTURE_BITS 32//プラットフォームアーキテクチャ（ビット長）
	#endif//_WIN64
#endif//_WIN32
#ifdef __linux__
	#define GASHA_IS_LINUX//プラットフォーム：Linux
	#define GASHA_PLATFORM_VER 0//プラットフォームバージョン
	#define GASHA_PLATFORM_MINOR 0//プラットフォームマイナーバージョン
	#define GASHA_PLATFORM_NAME "Linux"
	#ifdef __x86_64__
		#define GASHA_IS_64BIT//プラットフォーム：64bit
		#define GASHA_PLATFORM_ARCHITECTURE_BITS 64//プラットフォームアーキテクチャ（ビット長）
	#else//__x86_64__
		#define GASHA_IS_32BIT//プラットフォーム：32bit
		#define GASHA_PLATFORM_ARCHITECTURE_BITS 32//プラットフォームアーキテクチャ（ビット長）
	#endif//__x86_64__
#endif//__linux__
#ifdef __CYGWIN__
	#define GASHA_IS_CYGWIN//プラットフォーム：Cygwin
	#define GASHA_PLATFORM_VER 0//プラットフォームバージョン
	#define GASHA_PLATFORM_MINOR 0//プラットフォームマイナーバージョン
	#define GASHA_PLATFORM_NAME "Cygin"
#endif//__CYGWIN__

//CPU
#if defined(__x86_64__) || defined(_M_X64)
	#define GASHA_IS_X86//プラットフォームアーキテクチャ：x86系
	#define GASHA_IS_X64//プラットフォームアーキテクチャ：x64系
	#define GASHA_PLATFORM_ARCHITECTURE_NAME "x64"//プラットフォームアーキテクチャ名
	#define GASHA_IS_64BIT//プラットフォーム：64bit
	#define GASHA_PLATFORM_ARCHITECTURE_BITS 64//プラットフォームアーキテクチャ（ビット長）
	//#define GASHA_IS_LITTLE_ENDIAN//リトルエンディアン
#elif defined(__i386__) || defined(_M_IX86)
	#define GASHA_IS_X86//プラットフォームアーキテクチャ：x86系
	#define GASHA_PLATFORM_ARCHITECTURE_NAME "x86"//プラットフォームアーキテクチャ名
	#define GASHA_IS_32BIT//プラットフォーム：32bit
	#define GASHA_PLATFORM_ARCHITECTURE_BITS 32//プラットフォームアーキテクチャ（ビット長）
	//#define GASHA_IS_LITTLE_ENDIAN//リトルエンディアン
#endif//__x86_64__/__i386__

//エンディアン判定
#if defined(__BIG_ENDIAN__) || defined(GASHA_IS_BIG_ENDIAN)
	#define GASHA_IS_BIG_ENDIAN//プラットフォーム：ビッグエンディアン
	#define GASHA_ENDIAN_NAME "big"//エンディアン名
#endif//__BIG_ENDIAN__ 
#if defined(__LITTLE_ENDIAN__) || defined(GASHA_IS_LITTLE_ENDIAN)
	#define GASHA_IS_LITTLE_ENDIAN//プラットフォーム：リトルエンディアン
	#define GASHA_ENDIAN_NAME "little"//エンディアン名
#endif//__LITTLE_ENDIAN__ 
#if !defined(GASHA_ENDIAN_NAME)
	#define GASHA_ENDIAN_NAME "(unknown)"//エンディアン名
#endif//GASHA_ENDIAN_NAME

#endif//GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_H

// End of file

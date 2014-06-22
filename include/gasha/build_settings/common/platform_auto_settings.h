#pragma once
#ifndef __PLATFORM_AUTO_SETTINGS_H_
#define __PLATFORM_AUTO_SETTINGS_H_

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
	#define IS_WIN//プラットフォーム：Windows
	#define PLATFORM_NAME "Win"//プラットフォーム名
	#ifdef WINVER
		#define PLATFORM_VER WINVER//プラットフォームバージョン ※別途_WIN32_WINNT/WINVERを設定する必要あり
		//_WIN32_WINNT / WINVER に指定可能な値
		//  0x500 ... Windows2000
		//  0x501 ... Windows XP / Windows 2003 Server
		//  0x502 ... Windows XP SP2 / Windows 2003 Server SP1
		//  0x600 ... Windows Vista / Windows 2008 Server
		//  0x700 ... Windows 7
	#else//WINVER	
		#define PLATFORM_VER 0//プラットフォームバージョン
	#endif//WINVER
	#define PLATFORM_MINOR 0//プラットフォームマイナーバージョン
	#ifdef _WIN64
		#define IS_64BIT//プラットフォーム：64bit
		#define PLATFORM_ARCHITECTURE_BITS 64//プラットフォームアーキテクチャ（ビット長）
	#else//_WIN64
		#define IS_32BIT//プラットフォーム：32bit
		#define PLATFORM_ARCHITECTURE_BITS 32//プラットフォームアーキテクチャ（ビット長）
	#endif//_WIN64
#endif//_WIN32
#ifdef __linux__
	#define IS_LINUX//プラットフォーム：Linux
	#define PLATFORM_VER 0//プラットフォームバージョン
	#define PLATFORM_MINOR 0//プラットフォームマイナーバージョン
	#define PLATFORM_NAME "Linux"
	#ifdef __x86_64__
		#define IS_64BIT//プラットフォーム：64bit
		#define PLATFORM_ARCHITECTURE_BITS 64//プラットフォームアーキテクチャ（ビット長）
	#else//__x86_64__
		#define IS_32BIT//プラットフォーム：32bit
		#define PLATFORM_ARCHITECTURE_BITS 32//プラットフォームアーキテクチャ（ビット長）
	#endif//__x86_64__
#endif//__linux__
#ifdef __CYGWIN__
	#define IS_CYGWIN//プラットフォーム：Cygwin
	#define PLATFORM_VER 0//プラットフォームバージョン
	#define PLATFORM_MINOR 0//プラットフォームマイナーバージョン
	#define PLATFORM_NAME "Cygin"
#endif//__CYGWIN__

//CPU
#if defined(__x86_64__) || defined(_M_X64)
	#define IS_X86//プラットフォームアーキテクチャ：x86系
	#define IS_X64//プラットフォームアーキテクチャ：x64系
	#define PLATFORM_ARCHITECTURE "x64"//プラットフォームアーキテクチャ名
	#define IS_64BIT//プラットフォーム：64bit
	#define PLATFORM_ARCHITECTURE_BITS 64//プラットフォームアーキテクチャ（ビット長）
	#ifndef __LITTLE_ENDIAN__	
		#define __LITTLE_ENDIAN__//リトルエンディアン
	#endif//__LITTLE_ENDIAN__	
#elif defined(__i386__) || defined(_M_IX86)
	#define IS_X86//プラットフォームアーキテクチャ：x86系
	#define PLATFORM_ARCHITECTURE "x86"//プラットフォームアーキテクチャ名
	#define IS_32BIT//プラットフォーム：32bit
	#define PLATFORM_ARCHITECTURE_BITS 32//プラットフォームアーキテクチャ（ビット長）
	#ifndef __LITTLE_ENDIAN__	
		#define __LITTLE_ENDIAN__//リトルエンディアン
	#endif//__LITTLE_ENDIAN__	
#endif//__x86_64__/__i386__

//エンディアン判定
#ifdef __BIG_ENDIAN__
	#define IS_BIG_ENDIAN//プラットフォーム：ビッグエンディアン
	#define ENDIAN "big"//エンディアン名
#endif//__BIG_ENDIAN__ 
#ifdef __LITTLE_ENDIAN__
	#define IS_LITTLE_ENDIAN//プラットフォーム：リトルエンディアン
	#define ENDIAN "little"//エンディアン名
#endif//__LITTLE_ENDIAN__ 

#endif//__PLATFORM_AUTO_SETTINGS_H_

// End of file

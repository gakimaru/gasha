#pragma once
#ifndef GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_OS_WIN_H
#define GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_OS_WIN_H

//--------------------------------------------------------------------------------
// build_settings/platform_atuo_settings_os_win.h
// プラットフォーム自動判別・設定：【OS系】Windows用
//
// 依存するヘッダー：project_first_settings.h//プロジェクト固有のビルド設定（先行設定）
//                   compiler_auto_settings.h//コンパイラ自動判別・設定
//                   platform_auto_settings.h//プラットフォーム自動判別・設定
//                   ※build_settings.h, platform_auto_settings.h により、依存関係順にインクルード
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

//OS
#if defined(_WIN32) || defined(GASHA_IS_WIN)
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

#endif//GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_OS_WIN_H

// End of file

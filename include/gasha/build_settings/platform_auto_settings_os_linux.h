#pragma once
#ifndef GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_OS_LINUX_H
#define GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_OS_LINUX_H

//--------------------------------------------------------------------------------
// platform_atuo_settings_os_linux.h
// プラットフォーム自動判別・設定：【OS系】Linux用
//
// 依存するヘッダー：project/first_settings.h//プロジェクト固有のビルド設定（先行設定）
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
#if defined(__linux__) || defined(GASHA_IS_LINUX)
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

#endif//GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_OS_LINUX_H

// End of file

#pragma once
#ifndef GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_CPU_X86_H
#define GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_CPU_X86_H

//--------------------------------------------------------------------------------
// platform_atuo_settings_cpu_x86.h
// プラットフォーム自動判別・設定：【CPU系】x86用
//
// 依存するヘッダー：project_first_settings.h//プロジェクト固有のビルド設定（先行設定）
//                   compiler_auto_settings.h//コンパイラ自動判別・設定
//                   platform_auto_settings.h//プラットフォーム自動判別・設定
//                   platform_auto_settings_***.h//プラットフォーム自動判別・設定：【OS系】
//                   ※build_settings.h, platform_auto_settings.h により、依存関係順にインクルード
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

//CPU
#if defined(__x86_64__) || defined(_M_X64) || defined(GASHA_IS_X64)
	#define GASHA_IS_X86//プラットフォームアーキテクチャ：x86系
	#define GASHA_IS_X64//プラットフォームアーキテクチャ：x64系
	#define GASHA_PLATFORM_ARCHITECTURE_NAME "x64"//プラットフォームアーキテクチャ名
	#define GASHA_IS_64BIT//プラットフォーム：64bit
	#define GASHA_PLATFORM_ARCHITECTURE_BITS 64//プラットフォームアーキテクチャ（ビット長）
	#define GASHA_IS_LITTLE_ENDIAN//リトルエンディアン
#elif defined(__i386__) || defined(_M_IX86) || defined(GASHA_IS_X86)
	#define GASHA_IS_X86//プラットフォームアーキテクチャ：x86系
	#define GASHA_PLATFORM_ARCHITECTURE_NAME "x86"//プラットフォームアーキテクチャ名
	#define GASHA_IS_32BIT//プラットフォーム：32bit
	#define GASHA_PLATFORM_ARCHITECTURE_BITS 32//プラットフォームアーキテクチャ（ビット長）
	#define GASHA_IS_LITTLE_ENDIAN//リトルエンディアン
#endif//__x86_64__/__i386__

#endif//GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_CPU_X86_H

// End of file

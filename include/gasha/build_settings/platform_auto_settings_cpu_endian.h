#pragma once
#ifndef GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_CPU_ENDIAN_H
#define GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_CPU_ENDIAN_H

//--------------------------------------------------------------------------------
// build_settings/platform_atuo_settings_cpu_endian.h
// プラットフォーム自動判別・設定：【CPU系】エンディアン用
//
// 依存するヘッダー：project_first_settings.h//プロジェクト固有のビルド設定（先行設定）
//                   compiler_auto_settings.h//コンパイラ自動判別・設定
//                   platform_auto_settings.h//プラットフォーム自動判別・設定
//                   platform_auto_settings_***.h//プラットフォーム自動判別・設定：【CPU系】
//                   ※build_settings.h, platform_auto_settings.h により、依存関係順にインクルード
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

//CPU：エンディアン
#if defined(__BIG_ENDIAN__) || defined(GASHA_IS_BIG_ENDIAN) || defined(GASHA_IS_BIG_ENDIAN)
	#define GASHA_IS_BIG_ENDIAN//ビッグエンディアン
	#define GASHA_ENDIAN_NAME "big"//エンディアン名
#endif//__BIG_ENDIAN__ 
#if defined(__LITTLE_ENDIAN__) || defined(GASHA_IS_LITTLE_ENDIAN) || defined(GASHA_IS_LITTLE_ENDIAN)
	#define GASHA_IS_LITTLE_ENDIAN//リトルエンディアン
	#define GASHA_ENDIAN_NAME "little"//エンディアン名
#endif//__LITTLE_ENDIAN__ 
#if !defined(GASHA_IS_BIG_ENDIAN) && !defined(GASHA_IS_LITTLE_ENDIAN)
	#define GASHA_IS_UNKNOWN_ENDIAN//エンディアン不明
	#define GASHA_ENDIAN_NAME "(unknown)"//エンディアン名
#endif//GASHA_ENDIAN_NAME

#endif//GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_CPU_ENDIAN_H

// End of file

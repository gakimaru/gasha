#pragma once
#ifndef GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_H
#define GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_H

//--------------------------------------------------------------------------------
// platform_atuo_settings.h
// プラットフォーム自動判別・設定
//
// 依存するヘッダー：project/first_settings.h//プロジェクト固有のビルド設定（先行設定）
//                   compiler_auto_settings.h//コンパイラ自動判別・設定
//                   ※build_settings.h により、依存関係順にインクルード
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/platform_auto_settings_os_win.h>//【OS系】Windows用
#include <gasha/build_settings/platform_auto_settings_os_linux.h>//【OS系】Linux用
#include <gasha/build_settings/platform_auto_settings_os_cygwin.h>//【OS系】Cygwin用

#include <gasha/build_settings/platform_auto_settings_cpu_x86.h>//【CPU系】x86用
#include <gasha/build_settings/platform_auto_settings_cpu_endian.h>//【CPU系】エンディアン用

#endif//GASHA_INCLUDED_PLATFORM_AUTO_SETTINGS_H

// End of file

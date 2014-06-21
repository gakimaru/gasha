#pragma once
#ifndef __BUIILD_SETTINGS_H_
#define __BUIILD_SETTINGS_H_

//--------------------------------------------------------------------------------
// build_settings.h
// ビルド設定
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/project/project_build_settings_first.h>//プロジェクト固有のビルド設定（先行設定） リポジトリ：gasha_settings

#include <gasha/build_settings/common/compiler_auto_settings.h>//コンパイラ自動判別・設定       リポジトリ：gasha
#include <gasha/build_settings/common/platform_auto_settings.h>//プラットフォーム自動判別・設定 リポジトリ：gasha
#include <gasha/build_settings/common/language_auto_settings.h>//言語機能自動判別・設定         リポジトリ：gasha

#include <gasha/build_settings/project/project_build_settings.h>//プロジェクト固有のビルド設定 リポジトリ：gasha_settings

#endif//__BUIILD_SETTINGS_H_

// End of file

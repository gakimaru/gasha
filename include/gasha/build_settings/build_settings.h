#pragma once
#ifndef GASHA_INCLUDED_BUIILD_SETTINGS_H
#define GASHA_INCLUDED_BUIILD_SETTINGS_H

//--------------------------------------------------------------------------------
// build_settings.h
// ビルド設定
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/project_first_settings.h>//プロジェクト固有のビルド設定（先行設定） ※リポジトリ：gasha_settings

#include <gasha/build_settings/compiler_auto_settings.h>//コンパイラ自動判別・設定
#include <gasha/build_settings/platform_auto_settings.h>//プラットフォーム自動判別・設定
#include <gasha/build_settings/language_auto_settings.h>//言語仕様自動判別・設定

#include <gasha/build_settings/build_configuration.h>//ビルド構成

#include <gasha/build_settings/project_last_settings.h>//プロジェクト固有のビルド設定（最終設定） ※リポジトリ：gasha_settings

#include <gasha/build_settings/adjust_build_settings.h>//ビルド設定の調整

#include <gasha/build_settings/project_default_includes.h>//プロジェクト固有のデフォルトインクルード

//----------------------------------------
//ネームスペースを定義
GASHA_NAMESPACE_BEGIN;
//（ネームスペースの定義のみ）
GASHA_NAMESPACE_END;

#endif//GASHA_INCLUDED_BUIILD_SETTINGS_H

// End of file

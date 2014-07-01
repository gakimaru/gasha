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

#include <gasha/build_settings/project/first_settings.h>//プロジェクト固有のビルド設定（先行設定） ※リポジトリ：gasha_settings

#include <gasha/build_settings/common/compiler_auto_settings.h>//コンパイラ自動判別・設定
#include <gasha/build_settings/common/platform_auto_settings.h>//プラットフォーム自動判別・設定
#include <gasha/build_settings/common/language_auto_settings.h>//言語機能自動判別・設定

#include <gasha/build_settings/common/build_configuration.h>//ビルド構成

#include <gasha/build_settings/project/last_settings.h>//プロジェクト固有のビルド設定（最終設定） ※リポジトリ：gasha_settings

#include <gasha/build_settings/common/adjust_build_settings.h>//ビルド設定の調整

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------
//ビルド設定が実行環境に適合するか診断
//※全ての要素が問題なければ true を返す
bool diagnoseBuildSettings(char* message, std::size_t& size);

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_BUIILD_SETTINGS_H

// End of file

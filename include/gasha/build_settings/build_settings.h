#pragma once
#ifndef __BUIILD_SETTINGS_H_
#define __BUIILD_SETTINGS_H_

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

#include <gasha/build_settings/common/build_configuration.h>//ビルド構成

#include <gasha/build_settings/common/compiler_auto_settings.h>//コンパイラ自動判別・設定
#include <gasha/build_settings/common/platform_auto_settings.h>//プラットフォーム自動判別・設定
#include <gasha/build_settings/common/language_auto_settings.h>//言語機能自動判別・設定

#include <gasha/build_settings/project/last_settings.h>//プロジェクト固有のビルド設定（最終設定） ※リポジトリ：gasha_settings

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------
//ビルド設定が実行環境に適合するかチェック
//※ダメな要素があった場合、即abort
void checkBuildSettings();

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__BUIILD_SETTINGS_H_

// End of file

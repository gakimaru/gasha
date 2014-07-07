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
#include <gasha/build_settings/language_auto_settings.h>//言語機能自動判別・設定

#include <gasha/build_settings/build_configuration.h>//ビルド構成

#include <gasha/build_settings/project_last_settings.h>//プロジェクト固有のビルド設定（最終設定） ※リポジトリ：gasha_settings

#include <gasha/build_settings/adjust_build_settings.h>//ビルド設定の調整

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ビルド設定

//--------------------
//診断モード

//ライブラリビルド時の状態を診断
struct diagnosisModeForLibrary_t{};
extern const diagnosisModeForLibrary_t diagnosisModeForLibrary;

//現在の状態を診断
struct diagnosisModeForCurrent_t{};
extern const diagnosisModeForCurrent_t diagnosisModeForCurrent;

//----------------------------------------
//ビルド設定が実行環境に適合するか診断
//※全ての要素が問題なければ true を返す
//※診断結果メッセージとそのサイズを受け取るための変数を引数に渡す（バッファは4KBもあれば十分）。
//※ライブラリビルド時の状態か、現在の状態かを選んで指定可能。
template<class MODE>
bool diagnoseBuildSettings(char* message, std::size_t& size, const MODE mode);

//※ライブラリビルド時の診断モードを特殊化
template<>
bool diagnoseBuildSettings<diagnosisModeForLibrary_t>(char* message, std::size_t& size, const diagnosisModeForLibrary_t mode);

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/build_settings/build_settings.inl>

#endif//GASHA_INCLUDED_BUIILD_SETTINGS_H

// End of file

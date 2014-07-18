#pragma once
#ifndef GASHA_INCLUDED_BUIILD_SETTINGS_DIAG_H
#define GASHA_INCLUDED_BUIILD_SETTINGS_DIAG_H

//--------------------------------------------------------------------------------
// build_settings/build_settings_diag.h
// ビルド設定診断【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ビルド設定診断

//--------------------
//診断モード

//ライブラリビルド時の状態を診断
struct diagForLibrary_t{};
extern const diagForLibrary_t diagForLibrary;

//現在のプロジェクトの状態を診断
struct diagForProject_t{};
extern const diagForProject_t diagForProject;

//----------------------------------------
//ビルド設定が実行環境に適合するか診断
//※全ての要素が問題なければ true を返す
//※診断結果メッセージとそのサイズを受け取るための変数を引数に渡す（バッファは4KBもあれば十分）。
//※ライブラリビルド時の状態か、現在の状態かを選んで指定可能。
template<class MODE>
bool buildSettingsDiagnosticTest(char* message, const std::size_t max_size, std::size_t& size, const MODE mode);

//※ライブラリビルド時の診断モードを特殊化
template<>
bool buildSettingsDiagnosticTest<diagForLibrary_t>(char* message, const std::size_t max_size, std::size_t& size, const diagForLibrary_t mode);

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/build_settings/build_settings_diag.inl>

#endif//GASHA_INCLUDED_BUIILD_SETTINGS_DIAG_H

// End of file

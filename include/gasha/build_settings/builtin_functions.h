#pragma once
#ifndef GASHA_INCLUDED_BUILTIN_FUNCTIONS_H_
#define GASHA_INCLUDED_BUILTIN_FUNCTIONS_H_

//--------------------------------------------------------------------------------
// build_settings/builtin_functions.h
// 組み込み関数・マクロ
//
// 依存するヘッダー：project_first_settings.h//プロジェクト固有のビルド設定（先行設定）
//                   compiler_auto_settings.h//コンパイラ自動判別・設定
//                   platform_auto_settings.h//プラットフォーム自動判別・設定
//                   builtin_functions.h//言語機能自動判別・設定
//                   build_configuration.h//ビルド構成
//                   project_last_settings.h//プロジェクト固有のビルド設定（最終設定）
//                   adjust_build_settings.h//ビルド設定調整
//                   ※build_settings.h により、依存関係順にインクルード
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//組み込み関数

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//ソースファイル名のファイル名部分の抜き出し
inline constexpr const char* getStaticFileName(const char* str);

GASHA_NAMESPACE_END;//ネームスペース：終了

//--------------------------------------------------------------------------------
//組み込みマクロ

//----------------------------------------
//文字列化マクロ
#define GASHA_TO_STR(s) #s
#define GASHA_TO_STR_EX(s) GASHA_TO_STR(s)

//----------------------------------------
//関数名取得マクロ
#define GASHA_FUNC_NAME() __FUNCTION__

//----------------------------------------
//ソースファイル名＋行番号取得マクロ
#define GASHA_SRC_FILE()           GASHA_ getStaticFileName(__FILE__)
#define GASHA_SRC_FILE_LINE()      GASHA_ getStaticFileName(__FILE__ "(" GASHA_TO_STR_EX(__LINE__) ")")
#define GASHA_SRC_FILE_LINE_TIME() GASHA_ getStaticFileName(__FILE__ "(" GASHA_TO_STR_EX(__LINE__) ")[" __TIMESTAMP__ "]")
#define GASHA_FILE_AND_FUNC        GASHA_SRC_FILE_LINE_TIME(), GASHA_FUNC_NAME()

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/build_settings/builtin_functions.inl>

#endif//GASHA_INCLUDED_BUILTIN_FUNCTIONS_H_

// End of file

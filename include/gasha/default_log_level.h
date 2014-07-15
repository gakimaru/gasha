#pragma once
#ifndef GASHA_INCLUDED_DEFAULT_LOG_LEVEL_H
#define GASHA_INCLUDED_DEFAULT_LOG_LEVEL_H

//--------------------------------------------------------------------------------
// default_log_level.h
//既定のログレベル【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_level.h>//ログレベル

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//既定のログレベル
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//既定のログレベル用定数
enum levelEnum : logLevel::level_type
{
	asNormal = MAKE_LOG_LEVEL_VALUE(1, 0),//通常メッセージ
	asVerbose = MAKE_LOG_LEVEL_VALUE(0, 0),//冗長メッセージ
	asDetail = MAKE_LOG_LEVEL_VALUE(0, 1),//詳細メッセージ
	asImportant = MAKE_LOG_LEVEL_VALUE(2, 0),//重要メッセージ
	asWarning = MAKE_LOG_LEVEL_VALUE(3, 0),//警告メッセージ
	asCritical = MAKE_LOG_LEVEL_VALUE(4, 0),//重大メッセージ
	asAbsolute = MAKE_LOG_LEVEL_VALUE(5, 0),//絶対メッセージ（ログレベルに関係なく出力したいメッセージ）
	//以下、ログレベル／画面通知レベル変更用
	asSilent = MAKE_SPECIAL_LOG_LEVEL_VALUE(0),//静寂（絶対メッセージ以外出力しない）
	asSilentAbsolutely = MAKE_SPECIAL_LOG_LEVEL_VALUE(1),//絶対静寂（全てのメッセージを出力しない）
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/default_log_level.inl>

#endif//GASHA_INCLUDED_DEFAULT_LOG_LEVEL_H

// End of file

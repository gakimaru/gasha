#pragma once
#ifndef GASHA_INCLUDED_LOG_COMMON_H
#define GASHA_INCLUDED_LOG_COMMON_H

//--------------------------------------------------------------------------------
// log_common.h
// ログ共通設定【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ共通設定
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログ共通設定

//ログ用途
enum logPurpose : unsigned char
{
	ofLog = 0,//用途：ログ出力
	ofNotice = 1,//用途：画面通知
	LOG_PURPOSE_NUM = 2//用途数
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_common.inl>

#endif//GASHA_INCLUDED_LOG_COMMON_H

// End of file

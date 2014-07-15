#pragma once
#ifndef GASHA_INCLUDED_DEFAULT_LOG_CATEGORY_H
#define GASHA_INCLUDED_DEFAULT_LOG_CATEGORY_H

//--------------------------------------------------------------------------------
// default_log_category.h
// 既定のログカテゴリ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_category.h>//ログカテゴリ

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//既定のログカテゴリ
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//既定のログカテゴリ用定数
enum categoryEnum : logCategory::category_type
{
	forAny = MAKE_LOG_CATEGORY_VALUE(0),//なんでも（カテゴリなし）
	forFileSystem = MAKE_LOG_CATEGORY_VALUE(1),//ファイルシステム関係
	forResource = MAKE_LOG_CATEGORY_VALUE(2),//リソース関係
	for3D = MAKE_LOG_CATEGORY_VALUE(3),//3Dグラフィックス関係
	for2D = MAKE_LOG_CATEGORY_VALUE(4),//2Dグラフィックス関係
	forSound = MAKE_LOG_CATEGORY_VALUE(5),//サウンド関係
	//ログレベル／画面通知レベル変更用
	forEvery = MAKE_SPECIAL_LOG_CATEGORY_VALUE(0),//全部まとめて変更
	//特殊なカテゴリ（プリント時専用）
	forCallPoint = MAKE_SPECIAL_LOG_CATEGORY_VALUE(1),//直近のコールポイントのカテゴリに合わせる（なければforAny扱い）
	forCriticalCallPoint = MAKE_SPECIAL_LOG_CATEGORY_VALUE(2),//直近の重大コールポイントのカテゴリに合わせる（なければforAny扱い）
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/default_log_category.inl>

#endif//GASHA_INCLUDED_DEFAULT_LOG_CATEGORY_H

// End of file

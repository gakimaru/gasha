#pragma once
#ifndef GASHA_INCLUDED_DEFAULT_LOG_LEVEL_INL
#define GASHA_INCLUDED_DEFAULT_LOG_LEVEL_INL

//--------------------------------------------------------------------------------
// default_log_level.inl
//既定のログレベル【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/default_log_level.h>//既定のログレベル【宣言部】

#include <gasha/log_level.inl>//ログレベル【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//既定のログレベル
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//既定のログレベルを登録
void registDefaultLogLevel();

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_DEFAULT_LOG_LEVEL_INL

// End of file

#pragma once
#ifndef GASHA_INCLUDED_I_DEBUG_PAUSE_INL
#define GASHA_INCLUDED_I_DEBUG_PAUSE_INL

//--------------------------------------------------------------------------------
// i_debug_pause.inl
// デバッグポーズインターフェース【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_debug_pause.h>//デバッグポーズカラー【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//デバッグポーズインターフェース
//--------------------------------------------------------------------------------

#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

//----------------------------------------
//デバッグポーズインターフェースクラス

//（なし）

#endif// GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_I_DEBUG_PAUSE_INL

// End of file

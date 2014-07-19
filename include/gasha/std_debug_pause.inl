#pragma once
#ifndef GASHA_INCLUDED_STD_DEBUG_PAUSE_INL
#define GASHA_INCLUDED_STD_DEBUG_PAUSE_INL

//--------------------------------------------------------------------------------
// std_debug_pause.inl
// 標準デバッグポーズ【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/std_debug_pause.h>//標準デバッグポーズ【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準デバッグポーズ
//--------------------------------------------------------------------------------

#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

//（なし）

#endif//GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_STD_DEBUG_PAUSE_H

// End of file

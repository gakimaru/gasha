#pragma once
#ifndef GASHA_INCLUDED_DUMMY_DEBUG_PAUSE_INL
#define GASHA_INCLUDED_DUMMY_DEBUG_PAUSE_INL

//--------------------------------------------------------------------------------
// dummy_debug_pause.inl
// ダミーデバッグポーズ【インライン関数／テンプレート関数定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dummy_debug_pause.h>//ダミーデバッグポーズ【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ダミーデバッグポーズ
//--------------------------------------------------------------------------------

#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

//（なし）

#endif//GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_DUMMY_DEBUG_PAUSE_H

// End of file

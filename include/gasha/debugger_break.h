#pragma once
#ifndef GASHA_INCLUDED_DEBUGGER_BREAK_H
#define GASHA_INCLUDED_DEBUGGER_BREAK_H

//--------------------------------------------------------------------------------
// debugger_break.h
// デバッガ用ブレークポイント割り込み【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//デバッガー用ブレークポイント割り込み
//--------------------------------------------------------------------------------

#ifdef GASHA_DEBUGGER_BREAK_IS_AVAILABLE//デバッガ用ブレークポイント無効時はまるごと無効化

//----------------------------------------
//デバッガ用ブレークポイント割り込み
inline bool debuggerBreak();

#else//GASHA_DEBUGGER_BREAK_IS_AVAILABLE//デバッガ用ブレークポイント無効時はまるごと無効化

//----------------------------------------
//デバッガ用ブレークポイント割り込み
inline bool debuggerBreak(){ return true; }

#endif//GASHA_DEBUGGER_BREAK_IS_AVAILABLE//デバッガ用ブレークポイント無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/debugger_break.inl>

#endif//GASHA_INCLUDED_DEBUGGER_BREAK_H

// End of file

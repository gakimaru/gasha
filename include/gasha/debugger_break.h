﻿#pragma once
#ifndef GASHA_INCLUDED_DEBUGGER_BREAK_H
#define GASHA_INCLUDED_DEBUGGER_BREAK_H

//--------------------------------------------------------------------------------
// debugger_break.h
// デバッガブレークポイント【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_debug_pause.h>//デバッグポーズインターフェース

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//デバッガブレークポイント
//--------------------------------------------------------------------------------

#ifdef GASHA_DEBUGGER_BREAK_IS_AVAILABLE//デバッガ用ブレークポイント無効時はまるごと無効化

//----------------------------------------
//デバッガブレークポイント
inline void debuggerBreak();

#else//GASHA_DEBUGGER_BREAK_IS_AVAILABLE//デバッガ用ブレークポイント無効時はまるごと無効化

//----------------------------------------
//デバッガブレークポイント
inline void debuggerBreak(){}

#endif//GASHA_DEBUGGER_BREAK_IS_AVAILABLE//デバッガ用ブレークポイント無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_DEBUGGER_BREAK_H

// End of file

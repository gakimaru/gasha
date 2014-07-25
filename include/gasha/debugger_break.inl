#pragma once
#ifndef GASHA_INCLUDED_DEBUGGER_BREAK_INL
#define GASHA_INCLUDED_DEBUGGER_BREAK_INL

//--------------------------------------------------------------------------------
// debugger_break.inl
// デバッガ用ブレークポイント割り込み【インライン関数／テンプレート関数定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/debugger_break.h>//デバッガー用ブレークポイント割り込み【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//デバッガー用ブレークポイント割り込み
//--------------------------------------------------------------------------------

#ifdef GASHA_DEBUGGER_BREAK_IS_AVAILABLE//デバッガ用ブレークポイント無効時はまるごと無効化

//----------------------------------------
//デバッガ用ブレークポイント割り込み
inline bool debuggerBreak()
{
	//VC++の場合
#ifdef GASHA_IS_VC
	__debugbreak();
#endif//GASHA_IS_VC

	//x86系かつGCCの場合
#if defined(GASHA_IS_GCC) && defined(GASHA_IS_X86)
	__asm__("int3");
#endif//GASHA_IS_GCC, GASHA_IS_X86

	return true;
}

#endif//GASHA_DEBUGGER_BREAK_IS_AVAILABLE//デバッガ用ブレークポイント無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_DEBUGGER_BREAK_H

// End of file

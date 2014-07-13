#pragma once
#ifndef GASHA_INCLUDED_VS_CONSOLE_INL
#define GASHA_INCLUDED_VS_CONSOLE_INL

//--------------------------------------------------------------------------------
// vs_console.inl
// Visual Studio出力ウインドウ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/vs_console.h>//Visual Studio出力ウインドウ【宣言部】

#ifndef GASHA_USE_VS_CONSOLE
#ifdef GASHA_USE_WINDOWS_CONSOLE
#include <gasha/win_console.inl>//Windowsコマンドプロンプト
#else//GASHA_USE_WINDOWS_CONSOLE
#include <gasha/tty_console.inl>//TTY端末
#endif//GASHA_USE_WINDOWS_CONSOLE
#endif//GASHA_USE_VS_CONSOLE

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//Visual Studio出力ウインドウ
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//Visual Studio出力ウインドウクラス

#ifdef GASHA_USE_VS_CONSOLE
//Visual Studio出力ウインドウクラス有効時

//カラー変更
inline void vsConsole::changeColor(const GASHA_ consoleColor& color)
{
	return changeColor(std::move(*const_cast<GASHA_ consoleColor*>(&color)));
}

//コンストラクタ
inline vsConsole::vsConsole(const char* name) :
	m_name(name)
{}

#endif//GASHA_USE_VS_CONSOLE

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_VS_CONSOLE_INL

// End of file

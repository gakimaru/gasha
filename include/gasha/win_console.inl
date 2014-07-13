#pragma once
#ifndef GASHA_INCLUDED_WIN_CONSOLE_INL
#define GASHA_INCLUDED_WIN_CONSOLE_INL

//--------------------------------------------------------------------------------
// win_console.inl
// Windowsコマンドプロントプト【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/win_console.h>//Windowsコマンドプロントプト【宣言部】

#ifndef GASHA_USE_WINDOWS_CONSOLE
#include <gasha/tty_console.inl>//TTY端末
#endif//GASHA_USE_WINDOWS_CONSOLE

#include <utility>//std:forward
#include <cstdio>//fprintf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//Windowsコマンドプロントプト
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//Windowsコマンドプロントプトクラス

#ifdef GASHA_USE_WINDOWS_CONSOLE
//Windowsコマンドプロンプトクラス有効時

//書式付き出力
template<typename... Tx>
inline int winConsole::printf(const char* fmt, Tx&&... args) const
{
	return ::fprintf(m_handle, fmt, std::forward<Tx>(args)...);
}

//カラー変更
inline void winConsole::changeColor(const GASHA_ consoleColor& color)
{
	return changeColor(std::move(*const_cast<GASHA_ consoleColor*>(&color)));
}

#endif//GASHA_USE_WINDOWS_CONSOLE

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_WIN_CONSOLE_INL

// End of file

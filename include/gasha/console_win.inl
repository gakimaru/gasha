#pragma once
#ifndef GASHA_INCLUDED_CONSOLE_WIN_INL
#define GASHA_INCLUDED_CONSOLE_WIN_INL

//--------------------------------------------------------------------------------
// console_win.inl
// Windowsコマンドプロントプト【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/console_win.h>//Windowsコマンドプロントプト【宣言部】

#ifndef GASHA_USE_WINDOWS_CONSOLE
#include <gasha/console_tty.inl>//TTY端末
#endif//GASHA_USE_WINDOWS_CONSOLE

#include <utility>//std:forward
#include <stdio.h>//fprintf()

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
inline int consoleWin::printf(const char* fmt, Tx&&... args) const
{
	return ::fprintf(m_handle, fmt, std::forward<Tx>(args)...);
}

#endif//GASHA_USE_WINDOWS_CONSOLE

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_CONSOLE_WIN_INL

// End of file

#pragma once
#ifndef GASHA_INCLUDED_CONSOLE_VS_INL
#define GASHA_INCLUDED_CONSOLE_VS_INL

//--------------------------------------------------------------------------------
// console_vs.inl
// Visual Studio出力ウインドウ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/console_vs.h>//Visual Studio出力ウインドウ【宣言部】

#ifndef GASHA_USE_VS_CONSOLE
#ifdef GASHA_USE_WINDOWS_CONSOLE
#include <gasha/console_win.inl>//Windowsコマンドプロンプト
#else//GASHA_USE_WINDOWS_CONSOLE
#include <gasha/console_tty.inl>//TTY端末
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

//コンストラクタ
inline consoleVS::consoleVS()
{}

#endif//GASHA_USE_VS_CONSOLE

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_CONSOLE_VS_INL

// End of file

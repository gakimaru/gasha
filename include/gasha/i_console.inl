#pragma once
#ifndef GASHA_INCLUDED_I_CONSOLE_INL
#define GASHA_INCLUDED_I_CONSOLE_INL

//--------------------------------------------------------------------------------
// i_console.inl
// コンソールインターフェース【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールカラー【宣言部】

#include <gasha/string.h>//文字列処理：spprintf()

#include <utility>//C++11 std::forward, std::move

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//コンソールインターフェース
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//コンソールインターフェースクラス

//書式付き出力
template<typename... Tx>
inline int IConsole::printf(char* message, const char* fmt, Tx&&... args)
{
	const int ret = GASHA_ spprintf(message, fmt, std::forward<Tx>(args)...);
	output(message);
	return ret;
}

//改行出力
//※改行前にカラーリセットも行う
inline void IConsole::outputCr()
{
	resetColor();
	output("\n");
}

//カラー変更
inline void IConsole::changeColor(const GASHA_ consoleColor& color)
{
	return changeColor(std::move(*const_cast<GASHA_ consoleColor*>(&color)));
}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_I_CONSOLE_INL

// End of file

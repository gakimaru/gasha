#pragma once
#ifndef GASHA_INCLUDED_TTY_CONSOLE_INL
#define GASHA_INCLUDED_TTY_CONSOLE_INL

//--------------------------------------------------------------------------------
// tty_console.inl
// TTY端末【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/tty_console.h>//TTY端末【宣言部】

#include <utility>//std:forward
#include <cstdio>//std::fprintf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//TTY端末
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//TTY端末クラス

//書式付き出力
template<typename... Tx>
inline int ttyConsole::printf(const char* fmt, Tx&&... args) const
{
	return std::fprintf(m_handle, fmt, std::forward<Tx>(args)...);
}

//カラー変更
inline void ttyConsole::changeColor(const GASHA_ consoleColor& color)
{
	changeColor(std::move(*const_cast<GASHA_ consoleColor*>(&color)));
}

//コンストラクタ
inline ttyConsole::ttyConsole(std::FILE* handle, const char* name) :
	m_name(name),
	m_handle(handle),
	m_currColor(consoleColor::stdColor)
{}

//----------------------------------------
//カラー出力なしTTY端末クラス

//コンストラクタ
inline monoTtyConsole::monoTtyConsole(std::FILE* handle, const char* name) :
	ttyConsole(handle, name)
{}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_TTY_CONSOLE_INL

// End of file

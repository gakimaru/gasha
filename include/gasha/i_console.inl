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

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//コンソールインターフェースクラス

//書式付き出力
template<typename... Tx>
inline int iConsole::printf(char* work_buff, const char* fmt, Tx&&... args)
{
	const int ret = GASHA_ spprintf(work_buff, fmt, std::forward<Tx>(args)...);
	put(work_buff);
	return ret;
}
template<typename... Tx>
inline int iConsole::printf(char* work_buff, const std::size_t work_buff_size, const char* fmt, Tx&&... args)
{
	std::size_t pos = 0;
	const int ret = GASHA_ spprintf(work_buff, work_buff_size, pos, fmt, std::forward<Tx>(args)...);
	put(work_buff);
	return ret;
}

//カラー変更
inline void iConsole::changeColor(const GASHA_ consoleColor& color)
{
	return changeColor(std::move(*const_cast<GASHA_ consoleColor*>(&color)));
}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_I_CONSOLE_INL

// End of file

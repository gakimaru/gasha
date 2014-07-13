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

#include <utility>//C++11 std::forward
#include <stdio.h>//sprintf()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】sprintf を使用すると、error C4996 が発生する
//  error C4996: 'sprintf': This function or variable may be unsafe. Consider using strncpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

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
	const int ret = sprintf(message, fmt, std::forward<Tx>(args)...);
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

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_I_CONSOLE_INL

// End of file

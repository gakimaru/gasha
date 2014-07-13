#pragma once
#ifndef GASHA_INCLUDED_CONSOLE_TTY_INL
#define GASHA_INCLUDED_CONSOLE_TTY_INL

//--------------------------------------------------------------------------------
// console_tty.inl
// TTY端末【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/console_tty.h>//TTY端末【宣言部】

#include <utility>//std:forward
#include <stdio.h>//fprintf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//TTY端末
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//TTY端末クラス

//書式付き出力
template<typename... Tx>
inline int consoleTTY::printf(const char* fmt, Tx&&... args) const
{
	return ::fprintf(m_handle, fmt, std::forward<Tx>(args)...);
}

//コンストラクタ
inline consoleTTY::consoleTTY(FILE* handle) :
	m_handle(handle)
{}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_CONSOLE_TTY_INL

// End of file

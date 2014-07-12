#pragma once
#ifndef GASHA_INCLUDED_CONSOLE_INL
#define GASHA_INCLUDED_CONSOLE_INL

//--------------------------------------------------------------------------------
// console.inl
// コンソール【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/console.h>//コンソール【宣言部】

#include <utility>//std:forward
#include <stdio.h>//fprintf()

#ifdef GASHA_USE_WINDOWS_CONSOLE
#include <Windows.h>//OutputDebugString()
#endif//GASHA_USE_WINDOWS_CONSOLE

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//コンソール
//--------------------------------------------------------------------------------

//----------------------------------------
//コンソールクラス

//プリント
template<typename... Tx>
inline int console::printf(const char* fmt, Tx&&... args) const
{
#ifdef GASHA_USE_VS_CONSOLE
	if (m_type == VS)
	{
		::OutputDebugStringA(fmt);
		return 0;
	}
#endif//GASHA_USE_VS_CONSOLE
	if (m_handleTTY == nullptr)
		return ::printf(fmt, std::forward<Tx>(args)...);
	else
		return ::fprintf(m_handleTTY, fmt, std::forward<Tx>(args)...);
}

//コンストラクタ：TTY端末向け
inline console::console(const console::TTY_t&, FILE* handle) :
#ifdef GASHA_USE_WINDOWS_CONSOLE
	m_type(TTY),
	m_handleTTY(handle),
	m_handleWin(INVALID_HANDLE_VALUE)
#else//GASHA_USE_WINDOWS_CONSOLE
	m_type(TTY),
	m_handleTTY(handle)
#endif//GASHA_USE_WINDOWS_CONSOLE
{}

//コンストラクタ：VS出力ウインドウ向け
inline console::console(const console::VS_t&) :
#ifdef GASHA_USE_VS_CONSOLE
	m_type(VS),
	m_handleTTY(nullptr),
	m_handleWin(INVALID_HANDLE_VALUE)
#else//GASHA_USE_VS_CONSOLE
	console::console(console_WIN, stdout)//委譲コンストラクタ
#endif//GASHA_USE_VS_CONSOLE
{}

//デストラクタ
inline console::~console()
{

}
GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_CONSOLE_INL

// End of file

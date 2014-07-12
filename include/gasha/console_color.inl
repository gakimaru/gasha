#pragma once
#ifndef GASHA_INCLUDED_CONSOLE_COLOR_INL
#define GASHA_INCLUDED_CONSOLE_COLOR_INL

//--------------------------------------------------------------------------------
// console_color.inl
// コンソールカラー【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/console_color.h>//コンソールカラー【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//コンソールカラー
//--------------------------------------------------------------------------------

//----------------------------------------
//コンソールカラークラス

//カラー変更
inline void consoleColor::changeColor()
{
	changeColor(m_fore, m_back, m_bold, m_underline);
}
inline void consoleColor::resetColor()
{
	changeColor(standard, standard, false, false);
}
inline void consoleColor::changeColor(const consoleColor::color_t fore, const consoleColor::color_t back, const bool bold, const bool underline)
{
	if (*m_console == console_TTY)
		changeColorTTY(fore, back, bold, underline);
	else if (*m_console == console_WIN)
		changeColorWIN(fore, back, bold, underline);
	else if (*m_console == console_VS)
		changeColorWIN(fore, back, bold, underline);
}

//ムーブオペレータ
inline consoleColor& consoleColor::operator=(consoleColor&& rhs)
{
	m_console = rhs.m_console;
	m_fore = rhs.m_fore;
	m_back = rhs.m_back;
	m_bold = rhs.m_bold;
	m_underline = rhs.m_underline;
	return *this;
}

//コピーオペレータ
inline consoleColor& consoleColor::operator=(const consoleColor& rhs)
{
	m_console = rhs.m_console;
	m_fore = rhs.m_fore;
	m_back = rhs.m_back;
	m_bold = rhs.m_bold;
	m_underline = rhs.m_underline;
	return *this;
}
//ムーブコンストラクタ
inline consoleColor::consoleColor(consoleColor&& obj) :
	m_console(obj.m_console),
	m_fore(obj.m_fore),
	m_back(obj.m_back),
	m_bold(obj.m_bold),
	m_underline(obj.m_underline)
{}
//コピーコンストラクタ
inline consoleColor::consoleColor(const consoleColor& obj) :
	m_console(obj.m_console),
	m_fore(obj.m_fore),
	m_back(obj.m_back),
	m_bold(obj.m_bold),
	m_underline(obj.m_underline)
{}
//デフォルトコンストラクタ
inline consoleColor::consoleColor() :
	m_console(nullptr),
	m_fore(standard),
	m_back(standard),
	m_bold(false),
	m_underline(false)
{}
//コンストラクタ
inline consoleColor::consoleColor(const GASHA_ console& _console, const consoleColor::color_t fore, const consoleColor::color_t back, const bool bold, const bool underline) :
	m_console(&_console),
	m_fore(fore),
	m_back(back),
	m_bold(bold),
	m_underline(underline)
{}
//デストラクタ
inline consoleColor::~consoleColor()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_CONSOLE_COLOR_INL

// End of file

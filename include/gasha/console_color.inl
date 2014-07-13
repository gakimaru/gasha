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

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//コンソールカラークラス

//ムーブオペレータ
inline consoleColor& consoleColor::operator=(consoleColor&& rhs)
{
	m_fore = rhs.m_fore;
	m_back = rhs.m_back;
	m_attr = rhs.m_attr;
	return *this;
}

//コピーオペレータ
inline consoleColor& consoleColor::operator=(const consoleColor& rhs)
{
	m_fore = rhs.m_fore;
	m_back = rhs.m_back;
	m_attr = rhs.m_attr;
	return *this;
}

//ムーブコンストラクタ
inline consoleColor::consoleColor(consoleColor&& obj) :
	m_fore(obj.m_fore),
	m_back(obj.m_back),
	m_attr(obj.m_attr)
{}

//コピーコンストラクタ
inline consoleColor::consoleColor(const consoleColor& obj) :
	m_fore(obj.m_fore),
	m_back(obj.m_back),
	m_attr(obj.m_attr)
{}

//コンストラクタ
inline consoleColor::consoleColor(const consoleColor::color_t fore, const consoleColor::color_t back, const attr_t attr) :
	m_fore(fore),
	m_back(back),
	m_attr(attr)
{}

//デストラクタ
inline consoleColor::~consoleColor()
{}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_CONSOLE_COLOR_INL

// End of file

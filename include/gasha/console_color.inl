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

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//コンソールカラークラス

//比較オペレータ
inline bool consoleColor::operator==(consoleColor&& rhs) const
{
	return m_fore == rhs.m_fore && m_back == rhs.m_back && m_attr == rhs.m_attr;
}
inline bool consoleColor::operator==(const consoleColor& rhs) const
{
	return m_fore == rhs.m_fore && m_back == rhs.m_back && m_attr == rhs.m_attr;
}
inline bool consoleColor::operator!=(consoleColor&& rhs) const
{
	return m_fore != rhs.m_fore || m_back != rhs.m_back || m_attr == rhs.m_attr;
}
inline bool consoleColor::operator!=(const consoleColor& rhs) const
{
	return m_fore != rhs.m_fore || m_back != rhs.m_back || m_attr == rhs.m_attr;
}

//標準カラーか？
inline bool consoleColor::isStandard() const
{
	return m_fore == STANDARD && m_back == STANDARD && m_attr == NOATTR;
}

//属性付与
inline void consoleColor::addAttr(const attr_t attr)
{
	m_attr = static_cast<attr_t>(static_cast<unsigned char>(m_attr) | static_cast<unsigned char>(attr));
}

//属性削除
inline void consoleColor::removeAttr(const attr_t attr)
{
	m_attr = static_cast<attr_t>(static_cast<unsigned char>(m_attr)& ~static_cast<unsigned char>(attr));
}

//初期状態にする
inline void consoleColor::reset()
{
	m_fore = STANDARD;
	m_back = STANDARD;
	m_attr = NOATTR;
}

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

//リセット用コンストラクタ
inline consoleColor::consoleColor(const stdColor_type&) :
	m_fore(STANDARD),
	m_back(STANDARD),
	m_attr(NOATTR)
{}

//デフォルトコンストラクタ
//※何もしない
inline consoleColor::consoleColor()
{}

//デストラクタ
inline consoleColor::~consoleColor()
{}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_CONSOLE_COLOR_INL

// End of file

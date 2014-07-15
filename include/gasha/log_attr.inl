#pragma once
#ifndef GASHA_INCLUDED_LOG_ATTR_INL
#define GASHA_INCLUDED_LOG_ATTR_INL

//--------------------------------------------------------------------------------
// log_attr.inl
// ログ属性【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_attr.h>//ログ属性【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ属性
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログ属性

//属性判定
inline bool logAttr::hasAttr(const attr_type attr) const
{
	return (attr & m_attr) != 0x00;
}

//属性付与
inline logAttr::attr_type logAttr::addAttr(const logAttr::attr_type attr)
{
	m_attr |= attr;
	return m_attr;
}

//属性破棄
inline logAttr::attr_type logAttr::removeAttr(const logAttr::attr_type attr)
{
	m_attr &= ~attr;
	return m_attr;
}

//クリア
inline logAttr::attr_type logAttr::clear()
{
	m_attr = noLogAttr;
	return m_attr;
}

//オペレータ
inline logAttr& logAttr::operator=(logAttr&& attr)
{
	m_attr = attr.m_attr;
	return *this;
}
inline logAttr& logAttr::operator=(const logAttr& attr)
{
	m_attr = attr.m_attr;
	return *this;
}
inline logAttr& logAttr::operator=(logAttr::attr_type&& attr)
{
	m_attr = attr;
	return *this;
}
inline logAttr& logAttr::operator=(const logAttr::attr_type& attr)
{
	m_attr = attr;
	return *this;
}

//コンストラクタ
inline logAttr::logAttr(logAttr&& attr) :
	m_attr(attr.m_attr)
{}
inline logAttr::logAttr(const logAttr& attr) :
	m_attr(attr.m_attr)
{}
inline logAttr::logAttr(logAttr::attr_type&& attr) :
	m_attr(attr)
{}
inline logAttr::logAttr(const logAttr::attr_type& attr) :
	m_attr(attr)
{}

//デフォルトコンストラクタ
inline logAttr::logAttr()
{}

//デストラクタ
inline logAttr::~logAttr()
{}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_ATTR_INL

// End of file

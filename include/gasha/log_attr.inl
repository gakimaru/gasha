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
inline bool logAttr::has(const logAttr::attr_type target_attr, const logAttr::attr_type attr)
{
	return (target_attr & attr) != 0x00;
}
inline bool logAttr::has(const attr_type attr) const
{
	return (*m_attrRef & attr) != 0x00;
}

//属性付与
inline logAttr::attr_type logAttr::add(const logAttr::attr_type attr)
{
	*m_attrRef |= attr;
	return *m_attrRef;
}

//属性破棄
inline logAttr::attr_type logAttr::remove(const logAttr::attr_type attr)
{
	*m_attrRef &= ~attr;
	return *m_attrRef;
}

//属性変更
inline logAttr::attr_type logAttr::set(const logAttr::attr_type attr)
{
	*m_attrRef &= attr;
	return *m_attrRef;
}

//属性リセット
inline logAttr::attr_type logAttr::reset()
{
	*m_attrRef = DEFAULT_ATTR;
	return *m_attrRef;
}

//ムーブオペレータ
inline logAttr& logAttr::operator=(logAttr&& rhs)
{
	m_refType = rhs.m_refType;
	m_attrRef = rhs.m_attrRef;
	m_prevTlsAttr = rhs.m_prevTlsAttr;//変更前のTLSログ属性はコピーし、
	rhs.m_prevTlsAttr = nullptr;      //ムーブ元からは削除（ムーブ元はデストラクタで復元しなくなる）
	if (m_refType == isLocal)//ローカルログ属性は、現在の種別がローカルの時だけコピー
		m_localAttr = rhs.m_localAttr;
	return *this;
}

//コピーオペレータ
inline logAttr& logAttr::operator=(const logAttr& rhs)
{
	m_refType = rhs.m_refType;
	m_attrRef = rhs.m_attrRef;
	m_prevTlsAttr = nullptr;//変更前のTLSログ属性はコピーしない（デストラクタで復元しない）
	if (m_refType == isLocal)//ローカルログ属性は、現在の種別がローカルの時だけコピー
		m_localAttr = rhs.m_localAttr;
	return *this;
}

//ムーブコンストラクタ
inline logAttr::logAttr(logAttr&& obj) :
	m_refType(obj.m_refType),
	m_attrRef(obj.m_attrRef),
	m_prevTlsAttr(obj.m_prevTlsAttr)//変更前のTLSログ属性はコピーし、
	                                //ムーブ元からは削除（ムーブ元はデストラクタで復元しなくなる）
{
	obj.m_prevTlsAttr = nullptr;//ムーブ元無効化
	if (m_refType == isLocal)//ローカルログ属性は、現在の種別がローカルの時だけコピー
		m_localAttr = obj.m_localAttr;
}

//コピーコンストラクタ
inline logAttr::logAttr(const logAttr& obj) :
	m_refType(obj.m_refType),
	m_attrRef(obj.m_attrRef),
	m_prevTlsAttr(nullptr)//変更前のTLSログ属性はコピーしない（デストラクタで復元しない）
{
	if (m_refType == isLocal)//ローカルログ属性は、現在の種別がローカルの時だけコピー
		m_localAttr = obj.m_localAttr;
}

//明示的な初期化用コンストラクタ
inline logAttr::logAttr(const explicitInitialize_t&)
{
	std::call_once(m_initialized, initializeOnce);//グローバルログレベルマスク初期化（一回限り）
}

//デストラクタ
inline logAttr::~logAttr()
{
	if (m_refType == isLocal || m_prevTlsAttr)//TLSログ属性を復元
		m_tlsAttrRef = m_prevTlsAttr;
}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_ATTR_INL

// End of file

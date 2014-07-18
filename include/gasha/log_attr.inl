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

#include <cstring>//std::memcpy()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ属性
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログ属性

//ログ用途に応じたビットシフト数
inline int logAttr::shiftBits(const logAttr::purpose_type purpose)
{
	return static_cast<int>(purpose == ofLog ? logShiftBits : purpose == ofNotice ? noticeShiftBits : 0);
}

//属性判定
inline bool logAttr::has(const logAttr::attr_value_type target_attr, const logAttr::attr_value_type attr)
{
	return (target_attr & attr) != 0x00;
}
inline bool logAttr::has(const logAttr::attr_value_type attr) const
{
	return (m_attrRef->m_attr & attr) != 0x00;
}
//※用途別の判定
inline bool logAttr::has(const logAttr::attr_value_type target_attr, const logAttr::purpose_type purpose, const logAttr::attr_value_type attr)
{
	return ((target_attr >> shiftBits(purpose)) & attr) != 0x00;
}
inline bool logAttr::has(const logAttr::purpose_type purpose, const logAttr::attr_value_type attr) const
{
	return ((m_attrRef->m_attr >> shiftBits(purpose)) & attr) != 0x00;
}

//属性付与
inline logAttr::attr_type logAttr::add(const logAttr::attr_value_type attr)
{
	m_attrRef->m_attr |= attr;
	return *m_attrRef;
}
inline logAttr::attr_type logAttr::add(const logAttr::purpose_type purpose, const logAttr::attr_value_type attr)
{
	return add(attr << shiftBits(purpose));
}

//属性破棄
inline logAttr::attr_type logAttr::remove(const logAttr::attr_value_type attr)
{
	m_attrRef->m_attr &= ~attr;
	return *m_attrRef;
}
inline logAttr::attr_type logAttr::remove(const logAttr::purpose_type purpose, const logAttr::attr_value_type attr)
{
	return remove(attr << shiftBits(purpose));
}

//属性取得
inline logAttr::attr_type logAttr::get() const
{
	return *m_attrRef;
}

//属性変更
inline logAttr::attr_type logAttr::set(const logAttr::attr_value_type attr)
{
	m_attrRef->m_attr &= attr;
	return *m_attrRef;
}

//属性リセット
inline logAttr::attr_type logAttr::reset()
{
	m_attrRef->m_attr = DEFAULT_ATTR;
	return *m_attrRef;
}

//ログレベルマスクのセーブ／ロード

//シリアライズに必要なサイズを取得
inline std::size_t logAttr::serializeSize() const
{
	return sizeof(attr_type);
}

//シリアライズ（セーブ用）
inline bool logAttr::serialize(void* dst, const std::size_t dst_size) const
{
	if (dst_size < serializeSize())
		return false;
	std::memcpy(dst, m_attrRef, serializeSize());
	return true;
}

//デシリアライズ（ロード用）
inline bool logAttr::deserialize(const void* src, const std::size_t src_size)
{
	if (src_size != serializeSize())
		return false;
	std::memcpy(m_attrRef, src, serializeSize());
	return true;
}

//ムーブオペレータ
inline logAttr& logAttr::operator=(logAttr&& rhs)
{
	m_refType = rhs.m_refType;
	m_attrRef = rhs.m_attrRef;
	m_prevTlsAttr = rhs.m_prevTlsAttr;//変更前のTLSログ属性はコピーし、
	rhs.m_prevTlsAttr = nullptr;      //ムーブ元からは削除（ムーブ元はデストラクタで復元しなくなる）
	rhs.m_refType = isGlobal;         //（同上）
	rhs.m_attrRef = &m_globalAttr;    //（同上）
	if (m_refType == isLocal)//ローカルログ属性は、現在の種別がローカルの時だけコピー
	{
		m_localAttr = rhs.m_localAttr;
		m_attrRef = &m_localAttr;
		if (m_tlsAttrRef == &rhs.m_localAttr)//末端以外のオブジェクトをムーブすると、ここで不整合を起こすので注意
			m_tlsAttrRef = &m_localAttr;
	}
	return *this;
}

//ムーブコンストラクタ
inline logAttr::logAttr(logAttr&& obj) :
	m_refType(obj.m_refType),
	m_attrRef(obj.m_attrRef),
	m_prevTlsAttr(obj.m_prevTlsAttr)//変更前のTLSログ属性はコピーし、
	                                //ムーブ元からは削除（ムーブ元はデストラクタで復元しなくなる）
{
	obj.m_prevTlsAttr = nullptr;  //ムーブ元を無効化
	obj.m_refType = isGlobal;     //（同上）
	obj.m_attrRef = &m_globalAttr;//（同上）
	if (m_refType == isLocal)//ローカルログ属性は、現在の種別がローカルの時だけコピー
	{
		m_localAttr = obj.m_localAttr;
		m_attrRef = &m_localAttr;
		if (m_tlsAttrRef == &obj.m_localAttr)//末端以外のオブジェクトをムーブすると、ここで不整合を起こすので注意
			m_tlsAttrRef = &m_localAttr;
	}
}


//明示的な初期化用コンストラクタ
inline logAttr::logAttr(const explicitInit_type&) :
	m_refType(isGlobal),
	m_attrRef(&m_globalAttr),
	m_prevTlsAttr(nullptr)
{
	std::call_once(m_initialized, initializeOnce);//グローバルログレベルマスク初期化（一回限り）
}

//デストラクタ
inline logAttr::~logAttr()
{
	if (m_refType == isLocal || m_prevTlsAttr)//TLSログ属性を復元
		m_tlsAttrRef = m_prevTlsAttr;
}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_ATTR_INL

// End of file

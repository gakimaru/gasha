#pragma once
#ifndef GASHA_INCLUDED_THREAD_ID_INL
#define GASHA_INCLUDED_THREAD_ID_INL

//--------------------------------------------------------------------------------
// thread_id.inl
// スレッドID【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/thread_id.h>//スレッドID【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//スレッドID
//--------------------------------------------------------------------------------

//----------------------------------------
//スレッドIDクラス

//ムーブオペレータ
inline threadId& threadId::operator=(const threadId&& rhs)
{
	m_id = rhs.m_id;
	m_name = rhs.m_name;
	m_nameCrc = rhs.m_nameCrc;
	return *this;
}

//コピーオペレータ
inline threadId& threadId::operator=(const threadId& rhs)
{
	m_id = rhs.m_id;
	m_name = rhs.m_name;
	m_nameCrc = rhs.m_nameCrc;
	return *this;
}

//ムーブコンストラクタ
inline threadId::threadId(const threadId&& obj) :
	m_id(obj.m_id),
	m_name(obj.m_name),
	m_nameCrc(obj.m_nameCrc)
{}

//コピーコンストラクタ
inline threadId::threadId(const threadId& obj) :
	m_id(obj.m_id),
	m_name(obj.m_name),
	m_nameCrc(obj.m_nameCrc)
{}

//コンストラクタ
inline threadId::threadId(const char* name) :
	m_id(&m_thisId),
	m_name(&m_thisName)
{
	setThisName(name);
	m_nameCrc = m_thisNameCrc;
}

//デフォルトコンストラクタ
inline threadId::threadId() :
	m_id(&m_thisId),
	m_name(&m_thisName),
	m_nameCrc(m_thisNameCrc)
{
	setThisId();
}

//デストラクタ
inline threadId::~threadId()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_THREAD_ID_INL

// End of file

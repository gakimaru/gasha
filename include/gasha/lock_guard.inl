#pragma once
#ifndef GASHA_INCLUDED_LOCK_GUARD_INL
#define GASHA_INCLUDED_LOCK_GUARD_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lock_guard.inl
// ロックガード（スコープロック）【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_guard.h>//ロックガード（スコープロック）【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//ロックガードクラス（スコープロック）
//ムーブコンストラクタ
template<class T>
inline lock_guard<T>::lock_guard(lock_guard&& obj) :
	m_lock(obj.m_lock),
	m_isLocked(obj.m_isLocked)
{
	obj.m_isLocked = false;
}
//コンストラクタ
template<class T>
inline lock_guard<T>::lock_guard(lock_type& lock) :
	m_lock(lock),
	m_isLocked(true)
{
	m_lock.lock();
}
//デストラクタ
template<class T>
inline lock_guard<T>::~lock_guard()
{
	if(m_isLocked)
		m_lock.unlock();
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOCK_GUARD_INL

// End of file

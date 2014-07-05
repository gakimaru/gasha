#pragma once
#ifndef GASHA_INCLUDED_SHARED_LOCK_GUARD_INL
#define GASHA_INCLUDED_SHARED_LOCK_GUARD_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_lock_guard.inl
// 共有ロックガード（スコープロック）【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shared_lock_guard.h>//共有ロックガード（スコープロック）【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//共有ロックガードクラス（スコープロック）
	//ムーブコンストラクタ
template<class T>
inline shared_lock_guard<T>::shared_lock_guard(shared_lock_guard&& obj) :
	m_lock(obj.m_lock),
	m_isLocked(obj.m_isLocked)
{
	obj.m_isLocked = false;
}
//コンストラクタ
template<class T>
inline shared_lock_guard<T>::shared_lock_guard(lock_type& lock) :
	m_lock(lock),
	m_isLocked(true)
{
	m_lock.lock_shared();
}
//デストラクタ
template<class T>
inline shared_lock_guard<T>::~shared_lock_guard()
{
	if(m_isLocked)
		m_lock.unlock_shared();
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SHARED_LOCK_GUARD_INL

// End of file

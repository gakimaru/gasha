#pragma once
#ifndef GASHA_INCLUDED_SHARED_LOCK_GUARD_INL
#define GASHA_INCLUDED_SHARED_LOCK_GUARD_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_lock_guard.inl
// 共有ロックガード（スコープロック）【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
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

//アップグレード
template<class T>
inline bool shared_lock_guard<T>::upgrade()
{
	if (m_isLocked && !m_isUpgraded)
	{
		m_lock.upgrade();
		m_isUpgraded = true;
		return true;
	}
	return false;
}

//アップグレードを試行
template<class T>
inline bool shared_lock_guard<T>::try_upgrade()
{
	if (m_isLocked && !m_isUpgraded)
	{
		if (m_lock.try_upgrade())
		{
			m_isUpgraded = true;
			return true;
		}
	}
	return false;
}

//ダウングレード
template<class T>
inline bool shared_lock_guard<T>::downgrade()
{
	if (m_isLocked && m_isUpgraded)
	{
		m_lock.downgrade();
		m_isUpgraded = false;
		return true;
	}
	return false;
}

//ムーブコンストラクタ
template<class T>
inline shared_lock_guard<T>::shared_lock_guard(shared_lock_guard&& obj) :
	m_lock(obj.m_lock),
	m_isLocked(obj.m_isLocked),
	m_isUpgraded(obj.m_isUpgraded)
{
	obj.m_isLocked = false;
	obj.m_isUpgraded = false;
}
//コンストラクタ
template<class T>
inline shared_lock_guard<T>::shared_lock_guard(lock_type& lock) :
	m_lock(lock),
	m_isLocked(true),
	m_isUpgraded(false)
{
	m_lock.lock_shared();
}
//デストラクタ
template<class T>
inline shared_lock_guard<T>::~shared_lock_guard()
{
	if (m_isLocked)
	{
		if (m_isUpgraded)
			m_lock.unlock();
		else
			m_lock.unlock_shared();
	}
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SHARED_LOCK_GUARD_INL

// End of file

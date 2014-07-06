#pragma once
#ifndef GASHA_INCLUDED_UNIQUE_SHARED_LOCK_INL
#define GASHA_INCLUDED_UNIQUE_SHARED_LOCK_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// unique_shared_lock.inl
// 単一共有ロック【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/unique_shared_lock.h>//単一共有ロック【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//単一共有ロッククラス

//排他ロック（ライトロック）取得
template<class T>
inline void unique_shared_lock<T>::lock()
{
	if (!m_lock || owns_any_lock())
		return;
	m_lock->lock();
	m_status = LOCKING_EXCLUSIVELY;
}

//排他ロック（ライトロック）取得を試行
template<class T>
inline bool unique_shared_lock<T>::try_lock()
{
	if (!m_lock)
		return false;
	if (m_status == LOCKING_EXCLUSIVELY)
		return true;
	else if (m_status != UNLOCKED)
		return false;
	const bool locked = m_lock->try_lock();
	if(locked)
		m_status = LOCKING_EXCLUSIVELY;
	return locked;
}

//排他ロック（ライトロック）解放
template<class T>
inline void unique_shared_lock<T>::unlock()
{
	if (!m_lock || m_status != LOCKING_EXCLUSIVELY)
		return;
	m_lock->unlock();
	m_status = UNLOCKED;
}

//共有ロック（リードロック）取得
template<class T>
inline void unique_shared_lock<T>::lock_shared()
{
	if (!m_lock || owns_any_lock())
		return;
	m_lock->lock_shared();
	m_status = LOCKING_SHARED;
}

//共有ロック（リードロック）取得を試行
template<class T>
inline bool unique_shared_lock<T>::try_lock_shared()
{
	if (!m_lock)
		return false;
	if (m_status == LOCKING_SHARED)
		return true;
	else if (m_status != UNLOCKED)
		return false;
	const bool locked = m_lock->try_lock_shared();
	if(locked)
		m_status = LOCKING_SHARED;
	return locked;
}

//共有ロック（リードロック）解放
template<class T>
inline void unique_shared_lock<T>::unlock_shared()
{
	if (!m_lock || m_status != LOCKING_SHARED)
		return;
	m_lock->unlock_shared();
	m_status = UNLOCKED;
}

//アップグレード
template<class T>
inline bool unique_shared_lock<T>::upgrade()
{
	if (!m_lock || m_status != LOCKING_SHARED)
		return false;
	m_lock->upgrade();
	m_status = LOCKING_EXCLUSIVELY;
	return true;
}

//アップグレードを試行
template<class T>
inline bool unique_shared_lock<T>::try_upgrade()
{
	if (!m_lock || m_status != LOCKING_SHARED)
		return false;
	const bool locked = m_lock->try_upgrade();
	if (locked)
		m_status = LOCKING_EXCLUSIVELY;
	return locked;
}

//ダウングレード
//※排他ロックから共有ロックにダウングレード
template<class T>
inline bool unique_shared_lock<T>::downgrade()
{
	if (!m_lock || m_status != LOCKING_EXCLUSIVELY)
		return false;
	m_lock->downgrade();
	m_status = LOCKING_SHARED;
	return true;
}

//ロックの所有権を放棄する
template<class T>
typename unique_shared_lock<T>::lock_type* unique_shared_lock<T>::release()
{
	lock_type* lock = m_lock;
	m_lock = nullptr;
	m_status = UNLOCKED;
	return lock;
}

//ロックの所有権を交換する
template<class T>
inline void unique_shared_lock<T>::swap(unique_shared_lock<T>& obj)
{
	lock_type* lock_tmp = m_lock;
	status_t status_tmp = m_status;
	m_lock = obj.m_lock;
	m_status = obj.m_status;
	obj.m_lock = lock_tmp;
	obj.m_status = status_tmp;
}

//ムーブオペレータ
template<class T>
inline unique_shared_lock<T>& unique_shared_lock<T>::operator=(unique_shared_lock<T>&& rhs)
{
	unlock();
	unlock_shared();
	m_lock = rhs.m_lock;
	m_status = rhs.m_status;
	rhs.m_status = UNLOCKED;
	return *this;
}

//ムーブコンストラクタ
template<class T>
inline unique_shared_lock<T>::unique_shared_lock(unique_shared_lock<T>&& obj) :
	m_lock(obj.m_lock),
	m_status(obj.m_status)
{
	obj.m_status = UNLOCKED;
}

//コンストラクタ
template<class T>
inline unique_shared_lock<T>::unique_shared_lock(typename unique_shared_lock<T>::lock_type& obj) :
	m_lock(&obj),
	m_status(UNLOCKED)
{
	lock_shared();
}
template<class T>
inline unique_shared_lock<T>::unique_shared_lock(typename unique_shared_lock<T>::lock_type& obj, const with_lock_t) :
	m_lock(&obj),
	m_status(UNLOCKED)
{
	lock();
}
template<class T>
inline unique_shared_lock<T>::unique_shared_lock(typename unique_shared_lock<T>::lock_type& obj, const with_lock_shared_t) :
	m_lock(&obj),
	m_status(UNLOCKED)
{
	lock_shared();
}
template<class T>
inline unique_shared_lock<T>::unique_shared_lock(typename unique_shared_lock<T>::lock_type& obj, const try_to_lock_t) :
	m_lock(&obj),
	m_status(UNLOCKED)
{
	try_lock();
}
template<class T>
inline unique_shared_lock<T>::unique_shared_lock(typename unique_shared_lock<T>::lock_type& obj, const try_to_lock_shared_t) :
	m_lock(&obj),
	m_status(UNLOCKED)
{
	try_lock_shared();
}
template<class T>
inline unique_shared_lock<T>::unique_shared_lock(typename unique_shared_lock<T>::lock_type& obj, const adopt_lock_t) :
	m_lock(&obj),
	m_status(LOCKING_EXCLUSIVELY)
{}
template<class T>
inline unique_shared_lock<T>::unique_shared_lock(typename unique_shared_lock<T>::lock_type& obj, const adopt_shared_lock_t) :
	m_lock(&obj),
	m_status(LOCKING_SHARED)
{}
template<class T>
inline unique_shared_lock<T>::unique_shared_lock(typename unique_shared_lock<T>::lock_type& obj, const defer_lock_t) :
	m_lock(&obj),
	m_status(UNLOCKED)
{}
//デフォルトコンストラクタ
template<class T>
inline unique_shared_lock<T>::unique_shared_lock() :
	m_lock(nullptr),
	m_status(UNLOCKED)
{}
//デストラクタ
template<class T>
inline unique_shared_lock<T>::~unique_shared_lock()
{
	unlock();
	unlock_shared();
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_UNIQUE_SHARED_LOCK_INL

// End of file

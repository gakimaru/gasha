#pragma once
#ifndef GASHA_INCLUDED_UNIQUE_LOCK_INL
#define GASHA_INCLUDED_UNIQUE_LOCK_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// unique_lock.inl
// 単一ロック【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/unique_lock.h>//単一ロック【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//単一ロッククラス

//ロック取得
template<class T>
inline void unique_lock<T>::lock()
{
	if (!m_lock || m_isLocked)
		return;
	m_lock->lock();
	m_isLocked = true;
}

//ロック取得を試行
template<class T>
inline bool unique_lock<T>::try_lock()
{
	if (!m_lock)
		return false;
	if (m_isLocked)
		return true;
	m_isLocked = m_lock->try_lock();
	return m_isLocked;
}

//ロック解放
template<class T>
inline void unique_lock<T>::unlock()
{
	if (!m_lock || !m_isLocked)
		return;
	m_lock->unlock();
	m_isLocked = false;
}

//ロックの所有権を放棄する
template<class T>
inline typename unique_lock<T>::lock_type* unique_lock<T>::release()
{
	lock_type* lock = m_lock;
	m_lock = nullptr;
	m_isLocked = false;
	return lock;
}

//ロックの所有権を交換する
template<class T>
inline void unique_lock<T>::swap(unique_lock<T>& obj)
{
	lock_type* lock_tmp = m_lock;
	bool is_locked_tmp = m_isLocked;
	m_lock = obj.m_lock;
	m_isLocked = obj.m_isLocked;
	obj.m_lock = lock_tmp;
	obj.m_isLocked = is_locked_tmp;
}

//ムーブオペレータ
template<class T>
inline unique_lock<T>& unique_lock<T>::operator=(unique_lock<T>&& rhs)
{
	unlock();
	m_lock = rhs.m_lock;
	m_isLocked = rhs.m_isLocked;
	rhs.m_isLocked = false;
	return *this;
}

//ムーブコンストラクタ
template<class T>
inline unique_lock<T>::unique_lock(unique_lock<T>&& obj) :
	m_lock(obj.m_lock),
	m_isLocked(obj.m_isLocked)
{
	obj.m_isLocked = false;
}

//コンストラクタ
template<class T>
inline unique_lock<T>::unique_lock(typename unique_lock<T>::lock_type& obj) :
	m_lock(&obj),
	m_isLocked(false)
{
	lock();
}
template<class T>
inline unique_lock<T>::unique_lock(typename unique_lock<T>::lock_type& obj, const with_lock_t) :
	m_lock(&obj),
	m_isLocked(false)
{
	lock();
}
template<class T>
inline unique_lock<T>::unique_lock(typename unique_lock<T>::lock_type& obj, const try_lock_t) :
	m_lock(&obj),
	m_isLocked(false)
{
	try_lock();
}
template<class T>
inline unique_lock<T>::unique_lock(typename unique_lock<T>::lock_type& obj, const adopt_lock_t) :
	m_lock(&obj),
	m_isLocked(true)
{}
template<class T>
inline unique_lock<T>::unique_lock(typename unique_lock<T>::lock_type& obj, const defer_lock_t) :
	m_lock(&obj),
	m_isLocked(false)
{}
//デフォルトコンストラクタ
template<class T>
inline unique_lock<T>::unique_lock() :
	m_lock(nullptr),
	m_isLocked(false)
{}
//デストラクタ
template<class T>
inline unique_lock<T>::~unique_lock()
{
	unlock();
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_UNIQUE_LOCK_INL

// End of file

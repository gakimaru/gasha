﻿#pragma once
#ifndef GASHA_INCLUDED_SIMPLE_SHARED_SPIN_LOCK_INL
#define GASHA_INCLUDED_SIMPLE_SHARED_SPIN_LOCK_INL

//--------------------------------------------------------------------------------
// simple_shared_spin_lock.inl
// 単純共有スピンロック【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/simple_shared_spin_lock.h>//単純共有スピンロック【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//単純共有スピンロッククラス

//単一ロック取得
inline GASHA_ unique_shared_lock<simpleSharedSpinLock> simpleSharedSpinLock::lockUnique(){ GASHA_ unique_shared_lock<simpleSharedSpinLock> lock(*this); return lock; }
inline GASHA_ unique_shared_lock<simpleSharedSpinLock> simpleSharedSpinLock::lockUnique(const GASHA_ with_lock_t&){ GASHA_ unique_shared_lock<simpleSharedSpinLock> lock(*this, GASHA_ with_lock); return lock; }
inline GASHA_ unique_shared_lock<simpleSharedSpinLock> simpleSharedSpinLock::lockUnique(const GASHA_ with_lock_shared_t&){ GASHA_ unique_shared_lock<simpleSharedSpinLock> lock(*this, GASHA_ with_lock_shared); return lock; }
inline GASHA_ unique_shared_lock<simpleSharedSpinLock> simpleSharedSpinLock::lockUnique(const GASHA_ try_to_lock_t&){ GASHA_ unique_shared_lock<simpleSharedSpinLock> lock(*this, GASHA_ try_to_lock); return lock; }
inline GASHA_ unique_shared_lock<simpleSharedSpinLock> simpleSharedSpinLock::lockUnique(const GASHA_ try_to_lock_shared_t&){ GASHA_ unique_shared_lock<simpleSharedSpinLock> lock(*this, GASHA_ try_to_lock_shared); return lock; }
inline GASHA_ unique_shared_lock<simpleSharedSpinLock> simpleSharedSpinLock::lockUnique(const GASHA_ adopt_lock_t&){ GASHA_ unique_shared_lock<simpleSharedSpinLock> lock(*this, GASHA_ adopt_lock); return lock; }
inline GASHA_ unique_shared_lock<simpleSharedSpinLock> simpleSharedSpinLock::lockUnique(const GASHA_ adopt_shared_lock_t&){ GASHA_ unique_shared_lock<simpleSharedSpinLock> lock(*this, GASHA_ adopt_shared_lock); return lock; }
inline GASHA_ unique_shared_lock<simpleSharedSpinLock> simpleSharedSpinLock::lockUnique(const GASHA_ defer_lock_t&){ GASHA_ unique_shared_lock<simpleSharedSpinLock> lock(*this, GASHA_ defer_lock); return lock; }

//排他ロック（ライトロック）用のロックガード取得
inline GASHA_ lock_guard<simpleSharedSpinLock> simpleSharedSpinLock::lockScoped()
{
	GASHA_ lock_guard<simpleSharedSpinLock> lock(*this);
	return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
}

//排他ロック（ライトロック）解放
inline void simpleSharedSpinLock::unlock()
{
	m_lockCounter.fetch_add(SHARED_LOCK_COUNTER_UNLOCKED);//カウンタを戻す
}

//共有ロック（リードロック）用のロックガード取得
inline GASHA_ shared_lock_guard<simpleSharedSpinLock> simpleSharedSpinLock::lockSharedScoped()
{
	GASHA_ shared_lock_guard<simpleSharedSpinLock> lock(*this);
	return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
}

//共有ロック（リードロック）解放
inline void simpleSharedSpinLock::unlock_shared()
{
	m_lockCounter.fetch_add(1);//カウンタを戻す
}

//ダウングレード
void simpleSharedSpinLock::downgrade()
{
	m_lockCounter.fetch_add(SHARED_LOCK_COUNTER_UNLOCKED - 1);//カウンタを戻す
}

//ムーブオペレータ
inline simpleSharedSpinLock& simpleSharedSpinLock::operator=(simpleSharedSpinLock&& rhs)
{
	m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
	rhs.m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
	return *this;
}
//コピーオペレータ
inline simpleSharedSpinLock& simpleSharedSpinLock::operator=(const simpleSharedSpinLock& rhs)
{
	m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
	return *this;
}
//ムーブコンストラクタ
inline simpleSharedSpinLock::simpleSharedSpinLock(simpleSharedSpinLock&& obj)
{
	m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
	obj.m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
}
//コピーコンストラクタ
inline simpleSharedSpinLock::simpleSharedSpinLock(const simpleSharedSpinLock& obj)
{
	m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
}

//コンストラクタ
inline simpleSharedSpinLock::simpleSharedSpinLock()
{
	m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
}

//デストラクタ
inline simpleSharedSpinLock::~simpleSharedSpinLock()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SIMPLE_SHARED_SPIN_LOCK_INL

// End of file

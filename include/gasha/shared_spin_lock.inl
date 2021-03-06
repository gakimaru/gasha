﻿#pragma once
#ifndef GASHA_INCLUDED_SHARED_SPIN_LOCK_INL
#define GASHA_INCLUDED_SHARED_SPIN_LOCK_INL

//--------------------------------------------------------------------------------
// shared_spin_lock.inl
// 共有スピンロック【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shared_spin_lock.h>//共有スピンロック【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//共有スピンロッククラス

//単一ロック取得
inline GASHA_ unique_shared_lock<sharedSpinLock> sharedSpinLock::lockUnique(){ GASHA_ unique_shared_lock<sharedSpinLock> lock(*this); return lock; }
inline GASHA_ unique_shared_lock<sharedSpinLock> sharedSpinLock::lockUnique(const GASHA_ with_lock_t&){ GASHA_ unique_shared_lock<sharedSpinLock> lock(*this, GASHA_ with_lock); return lock; }
inline GASHA_ unique_shared_lock<sharedSpinLock> sharedSpinLock::lockUnique(const GASHA_ with_lock_shared_t&){ GASHA_ unique_shared_lock<sharedSpinLock> lock(*this, GASHA_ with_lock_shared); return lock; }
inline GASHA_ unique_shared_lock<sharedSpinLock> sharedSpinLock::lockUnique(const GASHA_ try_to_lock_t&){ GASHA_ unique_shared_lock<sharedSpinLock> lock(*this, GASHA_ try_to_lock); return lock; }
inline GASHA_ unique_shared_lock<sharedSpinLock> sharedSpinLock::lockUnique(const GASHA_ try_to_lock_shared_t&){ GASHA_ unique_shared_lock<sharedSpinLock> lock(*this, GASHA_ try_to_lock_shared); return lock; }
inline GASHA_ unique_shared_lock<sharedSpinLock> sharedSpinLock::lockUnique(const GASHA_ adopt_lock_t&){ GASHA_ unique_shared_lock<sharedSpinLock> lock(*this, GASHA_ adopt_lock); return lock; }
inline GASHA_ unique_shared_lock<sharedSpinLock> sharedSpinLock::lockUnique(const GASHA_ adopt_shared_lock_t&){ GASHA_ unique_shared_lock<sharedSpinLock> lock(*this, GASHA_ adopt_shared_lock); return lock; }
inline GASHA_ unique_shared_lock<sharedSpinLock> sharedSpinLock::lockUnique(const GASHA_ defer_lock_t&){ GASHA_ unique_shared_lock<sharedSpinLock> lock(*this, GASHA_ defer_lock); return lock; }

//排他ロック（ライトロック）用のロックガード取得
inline GASHA_ lock_guard<sharedSpinLock> sharedSpinLock::lockScoped()
{
	GASHA_ lock_guard<sharedSpinLock> lock(*this);
	return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
}

//排他ロック（ライトロック）解放
inline void sharedSpinLock::unlock()
{
	m_lockCounter.fetch_add(SHARED_LOCK_COUNTER_UNLOCKED);//カウンタを戻す
}

//共有ロック（リードロック）用のロックガード取得
inline GASHA_ shared_lock_guard<sharedSpinLock> sharedSpinLock::lockSharedScoped()
{
	GASHA_ shared_lock_guard<sharedSpinLock> lock(*this);
	return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
}

//共有ロック（リードロック）解放
inline void sharedSpinLock::unlock_shared()
{
	m_lockCounter.fetch_add(1);//カウンタを戻す
}

//ダウングレード
inline void sharedSpinLock::downgrade()
{
	m_lockCounter.fetch_add(SHARED_LOCK_COUNTER_UNLOCKED - 1);//カウンタを戻す
}

//ムーブオペレータ
inline sharedSpinLock& sharedSpinLock::operator=(sharedSpinLock&& rhs)
{
	m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
	rhs.m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
	return *this;
}
//コピーオペレータ
inline sharedSpinLock& sharedSpinLock::operator=(const sharedSpinLock& rhs)
{
	m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
	return *this;
}
//ムーブコンストラクタ
inline sharedSpinLock::sharedSpinLock(sharedSpinLock&& obj)
{
	m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
	obj.m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
}
//コピーコンストラクタ
inline sharedSpinLock::sharedSpinLock(const sharedSpinLock& obj)
{
	m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
}

//コンストラクタ
inline sharedSpinLock::sharedSpinLock()
{
	m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
}

//デストラクタ
inline sharedSpinLock::~sharedSpinLock()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SHARED_SPIN_LOCK_H

// End of file

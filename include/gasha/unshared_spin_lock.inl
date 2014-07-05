#pragma once
#ifndef GASHA_INCLUDED_SIMPLE_UNSHARED_SPIN_LOCK_INL
#define GASHA_INCLUDED_SIMPLE_UNSHARED_SPIN_LOCK_INL

//--------------------------------------------------------------------------------
// unshared_spin_lock.inl
// 非共有スピンロック【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/unshared_spin_lock.h>//非共有スピンロック【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//非共有スピンロッククラス

//単一ロック取得
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this); return lock; }
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(const GASHA_ with_lock_t){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ with_lock); return lock; }
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(const GASHA_ with_lock_shared_t){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ with_lock_shared); return lock; }
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(const GASHA_ try_lock_t){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ try_lock); return lock; }
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(const GASHA_ try_lock_shared_t){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ try_lock_shared); return lock; }
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(const GASHA_ adopt_lock_t){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ adopt_lock); return lock; }
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(const GASHA_ adopt_shared_lock_t){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ adopt_shared_lock); return lock; }
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(const GASHA_ defer_lock_t){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ defer_lock); return lock; }

//排他ロック（ライトロック）用のロックガード取得
inline GASHA_ lock_guard<unsharedSpinLock> unsharedSpinLock::lockScoped(const int spin_count)
{
	GASHA_ lock_guard<unsharedSpinLock> lock(*this);
	return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
}

//排他ロック（ライトロック）取得を試行
inline bool unsharedSpinLock::try_lock()
{
	return m_lock.test_and_set() == false;
}

//排他ロック（ライトロック）解放
inline void unsharedSpinLock::unlock()
{
	m_lock.clear();
}

//共有ロック（リードロック）取得
inline void unsharedSpinLock::lock_shared(const int spin_count)
{
	lock(spin_count);
}

//共有ロック（リードロック）用のロックガード取得
inline GASHA_ shared_lock_guard<unsharedSpinLock> unsharedSpinLock::lockSharedScoped(const int spin_count)
{
	shared_lock_guard<unsharedSpinLock> lock(*this);
	return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
}

//共有ロック（リードロック）取得を試行
inline bool unsharedSpinLock::try_lock_shared()
{
	return m_lock.test_and_set() == false;
}

//共有ロック（リードロック）解放
inline void unsharedSpinLock::unlock_shared()
{
	m_lock.clear();
}

//コンストラクタ
inline unsharedSpinLock::unsharedSpinLock()
{
	m_lock.clear();
}

//デストラクタ
inline unsharedSpinLock::~unsharedSpinLock()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SIMPLE_UNSHARED_SPIN_LOCK_INL

// End of file

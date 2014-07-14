#pragma once
#ifndef GASHA_INCLUDED_SIMPLE_UNSHARED_SPIN_LOCK_INL
#define GASHA_INCLUDED_SIMPLE_UNSHARED_SPIN_LOCK_INL

//--------------------------------------------------------------------------------
// unshared_spin_lock.inl
// 非共有スピンロック【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
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
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(const GASHA_ with_lock_t&){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ with_lock); return lock; }
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(const GASHA_ with_lock_shared_t&){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ with_lock_shared); return lock; }
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(const GASHA_ try_to_lock_t&){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ try_to_lock); return lock; }
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(const GASHA_ try_to_lock_shared_t&){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ try_to_lock_shared); return lock; }
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(const GASHA_ adopt_lock_t&){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ adopt_lock); return lock; }
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(const GASHA_ adopt_shared_lock_t&){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ adopt_shared_lock); return lock; }
inline GASHA_ unique_shared_lock<unsharedSpinLock> unsharedSpinLock::lockUnique(const GASHA_ defer_lock_t&){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ defer_lock); return lock; }

//排他ロック（ライトロック）用のロックガード取得
inline GASHA_ lock_guard<unsharedSpinLock> unsharedSpinLock::lockScoped()
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
inline GASHA_ shared_lock_guard<unsharedSpinLock> unsharedSpinLock::lockSharedScoped()
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

//アップグレード
//※共有ロックから排他ロックにアップグレード
inline void unsharedSpinLock::upgrade(const int spin_count)
{
	//何もしない（常に成功）
	return;
}

//アップグレードを試行
inline bool unsharedSpinLock::try_upgrade()
{
	//何もしない（常に成功）
	return true;
}

//ダウングレード
//※排他ロックから共有ロックにダウングレード
inline void unsharedSpinLock::downgrade()
{
	//何もしない（常に成功）
	return;
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

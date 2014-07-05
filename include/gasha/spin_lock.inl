#pragma once
#ifndef GASHA_INCLUDED_SPIN_LOCK_INL
#define GASHA_INCLUDED_SPIN_LOCK_INL

//--------------------------------------------------------------------------------
// spin_lock.inl
// スピンロック【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/spin_lock.h>//スピンロック【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//スピンロッククラス

//単一ロック取得
inline GASHA_ unique_lock<spinLock> spinLock::lockUnique(){ GASHA_ unique_lock<spinLock> lock(*this); return lock; }
inline GASHA_ unique_lock<spinLock> spinLock::lockUnique(const GASHA_ with_lock_t){ GASHA_ unique_lock<spinLock> lock(*this, GASHA_ with_lock); return lock; }
inline GASHA_ unique_lock<spinLock> spinLock::lockUnique(const GASHA_ try_lock_t){ GASHA_ unique_lock<spinLock> lock(*this, GASHA_ try_lock); return lock; }
inline GASHA_ unique_lock<spinLock> spinLock::lockUnique(const GASHA_ adopt_lock_t){ GASHA_ unique_lock<spinLock> lock(*this, GASHA_ adopt_lock); return lock; }
inline GASHA_ unique_lock<spinLock> spinLock::lockUnique(const GASHA_ defer_lock_t){ GASHA_ unique_lock<spinLock> lock(*this, GASHA_ defer_lock); return lock; }

//ロックガード取得
inline GASHA_ lock_guard<spinLock> spinLock::lockScoped(const int spin_count)
{
	GASHA_ lock_guard<spinLock> lock(*this);
	return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
}

//ロック取得を試行
inline bool spinLock::try_lock()
{
	return m_lock.test_and_set() == false;
}

//ロック解放
inline void spinLock::unlock()
{
	m_lock.clear();
}

//コンストラクタ
inline spinLock::spinLock()
{
	m_lock.clear();
}

//デストラクタ
inline spinLock::~spinLock()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SPIN_LOCK_INL

// End of file

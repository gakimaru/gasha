#pragma once
#ifndef GASHA_INCLUDED_DUMMY_SHARED_LOCK_INL
#define GASHA_INCLUDED_DUMMY_SHARED_LOCK_INL

//--------------------------------------------------------------------------------
// dummy_shared_lock.inl
// ダミー共有ロック【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dummy_shared_lock.h>//ダミー共有ロック【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//ダミー共有ロッククラス

//単一ロック取得
inline GASHA_ unique_shared_lock<dummySharedLock> dummySharedLock::lockUnique(){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this); return lock; }
inline GASHA_ unique_shared_lock<dummySharedLock> dummySharedLock::lockUnique(const GASHA_ with_lock_t){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this, GASHA_ with_lock); return lock; }
inline GASHA_ unique_shared_lock<dummySharedLock> dummySharedLock::lockUnique(const GASHA_ with_lock_shared_t){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this, GASHA_ with_lock_shared); return lock; }
inline GASHA_ unique_shared_lock<dummySharedLock> dummySharedLock::lockUnique(const GASHA_ try_lock_t){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this, GASHA_ try_lock); return lock; }
inline GASHA_ unique_shared_lock<dummySharedLock> dummySharedLock::lockUnique(const GASHA_ try_lock_shared_t){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this, GASHA_ try_lock_shared); return lock; }
inline GASHA_ unique_shared_lock<dummySharedLock> dummySharedLock::lockUnique(const GASHA_ adopt_lock_t){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this, GASHA_ adopt_lock); return lock; }
inline GASHA_ unique_shared_lock<dummySharedLock> dummySharedLock::lockUnique(const GASHA_ adopt_shared_lock_t){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this, GASHA_ adopt_shared_lock); return lock; }
inline GASHA_ unique_shared_lock<dummySharedLock> dummySharedLock::lockUnique(const GASHA_ defer_lock_t){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this, GASHA_ defer_lock); return lock; }

//排他ロック（ライトロック）取得
inline void dummySharedLock::lock(const int spin_count)
{
	//何もしない
}
//排他ロック（ライトロック）用のロックガード取得
inline lock_guard<dummySharedLock> dummySharedLock::lockScoped(const int spin_count)
{
	GASHA_ lock_guard<dummySharedLock> lock(*this);
	return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
}
//排他ロック（ライトロック）取得を試行
inline bool dummySharedLock::try_lock()
{
	//何もしない（常に成功）
	return true;
}
//排他ロック（ライトロック）解放
inline void dummySharedLock::unlock()
{
	//何もしない
}

//共有ロック（リードロック）取得
inline void dummySharedLock::lock_shared(const int spin_count)
{
	//何もしない
}
//共有ロック（リードロック）用のロックガード取得
inline GASHA_ shared_lock_guard<dummySharedLock> dummySharedLock::lockSharedScoped(const int spin_count)
{
	GASHA_ shared_lock_guard<dummySharedLock> lock(*this);
	return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
}
//共有ロック（リードロック）取得を試行
//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
inline bool dummySharedLock::try_lock_shared()
{
	//何もしない（常に成功）
	return true;
}
//共有ロック（リードロック）解放
inline void dummySharedLock::unlock_shared()
{
	//何もしない
}
//コンストラクタ
inline dummySharedLock::dummySharedLock()
{
	//何もしない
}
//デストラクタ
inline dummySharedLock::~dummySharedLock()
{
	//何もしない
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_DUMMY_SHARED_LOCK_INL

// End of file

#pragma once
#ifndef GASHA_INCLUDED_DUMMY_LOCK_INL
#define GASHA_INCLUDED_DUMMY_LOCK_INL

//--------------------------------------------------------------------------------
// dummy_lock.inl
// ダミーロック【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dummy_lock.h>//ダミーロック【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//ダミーロッククラス

//単一ロック取得
inline GASHA_ unique_lock<dummyLock> dummyLock::lockUnique(){ GASHA_ unique_lock<dummyLock> lock(*this); return lock; }
inline GASHA_ unique_lock<dummyLock> dummyLock::lockUnique(const GASHA_ with_lock_t&){ GASHA_ unique_lock<dummyLock> lock(*this, GASHA_ with_lock); return lock; }
inline GASHA_ unique_lock<dummyLock> dummyLock::lockUnique(const GASHA_ try_to_lock_t&){ GASHA_ unique_lock<dummyLock> lock(*this, GASHA_ try_to_lock); return lock; }
inline GASHA_ unique_lock<dummyLock> dummyLock::lockUnique(const GASHA_ adopt_lock_t&){ GASHA_ unique_lock<dummyLock> lock(*this, GASHA_ adopt_lock); return lock; }
inline GASHA_ unique_lock<dummyLock> dummyLock::lockUnique(const GASHA_ defer_lock_t&){ GASHA_ unique_lock<dummyLock> lock(*this, GASHA_ defer_lock); return lock; }

//ロック取得
inline void dummyLock::lock(const int dummy_count)
{
	//何もしない
}
//ロックガード取得
inline GASHA_ lock_guard<dummyLock> dummyLock::lockScoped()
{
	GASHA_ lock_guard<dummyLock> lock(*this);
	return lock;
}
//ロック取得を試行
inline bool dummyLock::try_lock()
{
	//何もしない（常に成功）
	return true;
}
//ロック解放
inline void dummyLock::unlock()
{
	//何もしない
}
//ムーブオペレータ
inline dummyLock& dummyLock::operator=(dummyLock&& rhs)
{
	//何もしない
	return *this;
}
//コピーオペレータ
inline dummyLock& dummyLock::operator=(const dummyLock& rhs)
{
	//何もしない
	return *this;
}
//ムーブコンストラクタ
inline dummyLock::dummyLock(dummyLock&& obj)
{
	//何もしない
}
//コピーコンストラクタ
inline dummyLock::dummyLock(const dummyLock& obj)
{
	//何もしない
}
//コンストラクタ
inline dummyLock::dummyLock()
{
	//何もしない
}
//デストラクタ
inline dummyLock::~dummyLock()
{
	//何もしない
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_DUMMY_LOCK_INL

// End of file

#pragma once
#ifndef __DUMMY_LOCK_H_
#define __DUMMY_LOCK_H_

//--------------------------------------------------------------------------------
// dummyLock.h
// ダミーロック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_common.h>//ロック共通設定

#include <gasha/unique_lock.h>//単一ロック
#include <gasha/lock_guard.h>//ロックガード

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

//----------------------------------------
//ダミーロッククラス
//※std::mutex がモデル。
//※ロッククラスのインターフェースのみ実装し、実際には何もしない。
//※コンテナクラスのロック制御を無効化する際などに使用する。
class dummyLock
{
public:
	//メソッド

	//単一ロック取得
	inline GASHA_ unique_lock<dummyLock> get_unique_lock(){ GASHA_ unique_lock<dummyLock> lock(*this); return lock; }
	inline GASHA_ unique_lock<dummyLock> get_unique_lock(const GASHA_ with_lock_t){ GASHA_ unique_lock<dummyLock> lock(*this, GASHA_ with_lock); return lock; }
	inline GASHA_ unique_lock<dummyLock> get_unique_lock(const GASHA_ try_lock_t){ GASHA_ unique_lock<dummyLock> lock(*this, GASHA_ try_lock); return lock; }
	inline GASHA_ unique_lock<dummyLock> get_unique_lock(const GASHA_ adopt_lock_t){ GASHA_ unique_lock<dummyLock> lock(*this, GASHA_ adopt_lock); return lock; }
	inline GASHA_ unique_lock<dummyLock> get_unique_lock(const GASHA_ defer_lock_t){ GASHA_ unique_lock<dummyLock> lock(*this, GASHA_ defer_lock); return lock; }

	//ロック取得
	inline void lock(const int dummy_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		//何もしない
	}
	//ロックガード取得
	inline GASHA_ lock_guard<dummyLock> lock_scoped(const int dummy_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		GASHA_ lock_guard<dummyLock> lock(*this);
		return lock;
	}
	//ロック取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock()
	{
		//何もしない（常に成功）
		return true;
	}
	//ロック解放
	inline void unlock()
	{
		//何もしない
	}
public:
	//ムーブオペレータ
	dummyLock& operator=(dummyLock&&) = delete;
	//コピーオペレータ
	dummyLock& operator=(const dummyLock&) = delete;
public:
	//ムーブコンストラクタ
	dummyLock(dummyLock&&) = delete;
	//コピーコンストラクタ
	dummyLock(const dummyLock&) = delete;
	//コンストラクタ
	inline dummyLock()
	{
		//何もしない
	}
	//デストラクタ
	inline ~dummyLock()
	{
		//何もしない
	}
};

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__DUMMY_LOCK_H_

// End of file

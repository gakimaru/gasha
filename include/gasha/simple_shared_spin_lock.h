#pragma once
#ifndef __SIMPLE_SHARED_SPIN_LOCK_H_
#define __SIMPLE_SHARED_SPIN_LOCK_H_

//--------------------------------------------------------------------------------
// simpleSharedSpinLock.h
// 単純共有スピンロック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_common.h>//ロック共通設定

#include <gasha/unique_shared_lock.h>//単一共有ロック
#include <gasha/lock_guard.h>//ロックガード
#include <gasha/shared_lock_guard.h>//共有ロックガード

#include <atomic>//C++11 std::atomic

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

//----------------------------------------
//単純共有スピンロッククラス
//※std::shared_mutex がモデル。
//※サイズは4バイト(int型一つ分のサイズ)。
//※読み込み操作（共有ロック）が込み合っていると、
//　書き込み操作（排他ロック）が待たされるので注意。
class simpleSharedSpinLock
{
public:
	//メソッド

	//単一ロック取得
	inline GASHA_ unique_shared_lock<simpleSharedSpinLock> get_unique_lock(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT){ GASHA_ unique_shared_lock<simpleSharedSpinLock> lock(*this, spin_count); return lock; }
	inline GASHA_ unique_shared_lock<simpleSharedSpinLock> get_unique_lock(const GASHA_ with_lock_t, const int spin_count = GASHA_ DEFAULT_SPIN_COUNT){ GASHA_ unique_shared_lock<simpleSharedSpinLock> lock(*this, with_lock, spin_count); return lock; }
	inline GASHA_ unique_shared_lock<simpleSharedSpinLock> get_unique_lock(const GASHA_ with_lock_shared_t, const int spin_count = GASHA_ DEFAULT_SPIN_COUNT){ GASHA_ unique_shared_lock<simpleSharedSpinLock> lock(*this, with_lock_shared, spin_count); return lock; }
	inline GASHA_ unique_shared_lock<simpleSharedSpinLock> get_unique_lock(const GASHA_ adopt_lock_t){ GASHA_ unique_shared_lock<simpleSharedSpinLock> lock(*this, adopt_lock); return lock; }
	inline GASHA_ unique_shared_lock<simpleSharedSpinLock> get_unique_lock(const GASHA_ adopt_shared_lock_t){ GASHA_ unique_shared_lock<simpleSharedSpinLock> lock(*this, adopt_shared_lock); return lock; }
	inline GASHA_ unique_shared_lock<simpleSharedSpinLock> get_unique_lock(const GASHA_ defer_lock_t){ GASHA_ unique_shared_lock<simpleSharedSpinLock> lock(*this, defer_lock); return lock; }

	//排他ロック（ライトロック）取得
	void lock(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT);
	//排他ロック（ライトロック）用のロックガード取得
	//※排他ロック（ライトロック）取得を伴う
	inline GASHA_ lock_guard<simpleSharedSpinLock> lock_scoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		GASHA_ lock_guard<simpleSharedSpinLock> lock(*this);
		return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
	}
	//排他ロック（ライトロック）取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	bool try_lock();
	//排他ロック（ライトロック）解放
	inline void unlock()
	{
		m_lockCounter.fetch_add(SHARED_LOCK_COUNTER_UNLOCKED);//カウンタを戻す
	}

	//共有ロック（リードロック）取得
	void lock_shared(const int spin_count = DEFAULT_SPIN_COUNT);
	//共有ロック（リードロック）用のロックガード取得
	//※共有ロック（リードロック）取得を伴う
	inline GASHA_ shared_lock_guard<simpleSharedSpinLock> lock_shared_scoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		GASHA_ shared_lock_guard<simpleSharedSpinLock> lock(*this);
		return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
	}
	//共有ロック（リードロック）取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	bool try_lock_shared();
	//共有ロック（リードロック）解放
	inline void unlock_shared()
	{
		m_lockCounter.fetch_add(1);//カウンタを戻す
	}
public:
	//ムーブオペレータ
	simpleSharedSpinLock& operator=(simpleSharedSpinLock&&) = delete;
	//コピーオペレータ
	simpleSharedSpinLock& operator=(const simpleSharedSpinLock&) = delete;
public:
	//ムーブコンストラクタ
	simpleSharedSpinLock(simpleSharedSpinLock&&) = delete;
	//コピーコンストラクタ
	simpleSharedSpinLock(const simpleSharedSpinLock&) = delete;
	//コンストラクタ
	inline simpleSharedSpinLock()
	{
		m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
	}
	//デストラクタ
	inline ~simpleSharedSpinLock()
	{}
private:
	//フィールド
	std::atomic<int> m_lockCounter;//ロックカウンタ
};

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__SIMPLE_SHARED_SPIN_LOCK_H_

// End of file

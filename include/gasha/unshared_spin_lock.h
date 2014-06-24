#pragma once
#ifndef __UNSHARED_SPIN_LOCK_H_
#define __UNSHARED_SPIN_LOCK_H_

//--------------------------------------------------------------------------------
// unsharedSpinLock.h
// 非共有スピンロック
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
//非共有スピンロッククラス
//※std::shared_mutex がモデル。
//※サイズは4バイト(std::atomic_flag型一つ分のサイズ)。
//※共有ロックと同じインターフェースを備えるが、実際には
//　通常ロックと同じく全て排他ロックする。
class unsharedSpinLock
{
public:
	//メソッド

	//単一ロック取得
	inline GASHA_ unique_shared_lock<unsharedSpinLock> get_unique_lock(){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this); return lock; }
	inline GASHA_ unique_shared_lock<unsharedSpinLock> get_unique_lock(const GASHA_ with_lock_t){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ with_lock); return lock; }
	inline GASHA_ unique_shared_lock<unsharedSpinLock> get_unique_lock(const GASHA_ with_lock_shared_t){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ with_lock_shared); return lock; }
	inline GASHA_ unique_shared_lock<unsharedSpinLock> get_unique_lock(const GASHA_ try_lock_t){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ try_lock); return lock; }
	inline GASHA_ unique_shared_lock<unsharedSpinLock> get_unique_lock(const GASHA_ try_lock_shared_t){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ try_lock_shared); return lock; }
	inline GASHA_ unique_shared_lock<unsharedSpinLock> get_unique_lock(const GASHA_ adopt_lock_t){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ adopt_lock); return lock; }
	inline GASHA_ unique_shared_lock<unsharedSpinLock> get_unique_lock(const GASHA_ adopt_shared_lock_t){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ adopt_shared_lock); return lock; }
	inline GASHA_ unique_shared_lock<unsharedSpinLock> get_unique_lock(const GASHA_ defer_lock_t){ GASHA_ unique_shared_lock<unsharedSpinLock> lock(*this, GASHA_ defer_lock); return lock; }

	//排他ロック（ライトロック）取得
	void lock(const int spin_count = DEFAULT_SPIN_COUNT);
	//排他ロック（ライトロック）用のロックガード取得
	//※排他ロック（ライトロック）取得を伴う
	inline GASHA_ lock_guard<unsharedSpinLock> lock_scoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		lock_guard<unsharedSpinLock> lock(*this);
		return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
	}
	//排他ロック（ライトロック）取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock()
	{
		return m_lock.test_and_set() == false;
	}
	//排他ロック（ライトロック）解放
	inline void unlock()
	{
		m_lock.clear();
	}

	//共有ロック（リードロック）取得
	void lock_shared(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		lock(spin_count);
	}
	//共有ロック（リードロック）用のロックガード取得
	//※共有ロック（リードロック）取得を伴う
	inline GASHA_ shared_lock_guard<unsharedSpinLock> lock_shared_scoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		shared_lock_guard<unsharedSpinLock> lock(*this);
		return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
	}
	//共有ロック（リードロック）取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock_shared()
	{
		return m_lock.test_and_set() == false;
	}
	//共有ロック（リードロック）解放
	inline void unlock_shared()
	{
		m_lock.clear();
	}
public:
	//ムーブオペレータ
	unsharedSpinLock& operator=(unsharedSpinLock&&) = delete;
	//コピーオペレータ
	unsharedSpinLock& operator=(const unsharedSpinLock&) = delete;
public:
	//ムーブコンストラクタ
	unsharedSpinLock(unsharedSpinLock&&) = delete;
	//コピーコンストラクタ
	unsharedSpinLock(const unsharedSpinLock&) = delete;
	//コンストラクタ
	inline unsharedSpinLock()
	{
		m_lock.clear();
	}
	//デストラクタ
	inline ~unsharedSpinLock()
	{}
private:
	//フィールド
	std::atomic_flag m_lock;//ロック用フラグ
};

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__UNSHARED_SPIN_LOCK_H_

// End of file

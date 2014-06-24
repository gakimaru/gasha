#pragma once
#ifndef __SPIN_LOCK_H_
#define __SPIN_LOCK_H_

//--------------------------------------------------------------------------------
// spinLock.h
// スピンロック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_common.h>//ロック共通設定

#include <gasha/unique_lock.h>//単一ロック
#include <gasha/lock_guard.h>//ロックガード

#include <atomic>//C++11 std::atomic

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//スピンロッククラス
//※std::mutex がモデル。
//※サイズは4バイト(std::atomic_flag一つ分のサイズ)。
class spinLock
{
public:
	//メソッド

	//単一ロック取得
	inline GASHA_ unique_lock<spinLock> get_unique_lock(){ GASHA_ unique_lock<spinLock> lock(*this); return lock; }
	inline GASHA_ unique_lock<spinLock> get_unique_lock(const GASHA_ with_lock_t){ GASHA_ unique_lock<spinLock> lock(*this, GASHA_ with_lock); return lock; }
	inline GASHA_ unique_lock<spinLock> get_unique_lock(const GASHA_ try_lock_t){ GASHA_ unique_lock<spinLock> lock(*this, GASHA_ try_lock); return lock; }
	inline GASHA_ unique_lock<spinLock> get_unique_lock(const GASHA_ adopt_lock_t){ GASHA_ unique_lock<spinLock> lock(*this, GASHA_ adopt_lock); return lock; }
	inline GASHA_ unique_lock<spinLock> get_unique_lock(const GASHA_ defer_lock_t){ GASHA_ unique_lock<spinLock> lock(*this, GASHA_ defer_lock); return lock; }

	//ロック取得
	void lock(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT);
	//ロックガード取得
	//※ロック取得を伴う
	inline GASHA_ lock_guard<spinLock> lock_scoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		GASHA_ lock_guard<spinLock> lock(*this);
		return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
	}
	//ロック取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock()
	{
		return m_lock.test_and_set() == false;
	}
	//ロック解放
	inline void unlock()
	{
		m_lock.clear();
	}
public:
	//ムーブオペレータ
	spinLock& operator=(spinLock&&) = delete;
	//コピーオペレータ
	spinLock& operator=(const spinLock&) = delete;
public:
	//ムーブコンストラクタ
	spinLock(spinLock&&) = delete;
	//コピーコンストラクタ
	spinLock(const spinLock&) = delete;
	//コンストラクタ
	inline spinLock()
	{
		m_lock.clear();
	}
	//デストラクタ
	inline ~spinLock()
	{}
private:
	//フィールド
	std::atomic_flag m_lock;//ロック用フラグ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__SPIN_LOCK_H_

// End of file

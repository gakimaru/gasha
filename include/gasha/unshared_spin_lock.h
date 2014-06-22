#pragma once
#ifndef __UNSHARED_SPIN_LOCK_H_
#define __UNSHARED_SPIN_LOCK_H_

//--------------------------------------------------------------------------------
// unshared_spin_lock.h
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
class unshared_spin_lock
{
public:
	//メソッド

	//単一ロック取得
	inline GASHA_ unique_shared_lock<unshared_spin_lock> get_unique_lock(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT){ GASHA_ unique_shared_lock<unshared_spin_lock> lock(*this, spin_count); return lock; }
	inline GASHA_ unique_shared_lock<unshared_spin_lock> get_unique_lock(const with_lock_t, const int spin_count = GASHA_ DEFAULT_SPIN_COUNT){ GASHA_ unique_shared_lock<unshared_spin_lock> lock(*this, with_lock, spin_count); return lock; }
	inline GASHA_ unique_shared_lock<unshared_spin_lock> get_unique_lock(const with_lock_shared_t, const int spin_count = GASHA_ DEFAULT_SPIN_COUNT){ GASHA_ unique_shared_lock<unshared_spin_lock> lock(*this, with_lock_shared, spin_count); return lock; }
	inline GASHA_ unique_shared_lock<unshared_spin_lock> get_unique_lock(const adopt_lock_t){ GASHA_ unique_shared_lock<unshared_spin_lock> lock(*this, adopt_lock); return lock; }
	inline GASHA_ unique_shared_lock<unshared_spin_lock> get_unique_lock(const adopt_shared_lock_t){ GASHA_ unique_shared_lock<unshared_spin_lock> lock(*this, adopt_shared_lock); return lock; }
	inline GASHA_ unique_shared_lock<unshared_spin_lock> get_unique_lock(const defer_lock_t){ GASHA_ unique_shared_lock<unshared_spin_lock> lock(*this, defer_lock); return lock; }

	//排他ロック（ライトロック）取得
	void lock(const int spin_count = DEFAULT_SPIN_COUNT);
	//排他ロック（ライトロック）用のロックガード取得
	//※排他ロック（ライトロック）取得を伴う
	inline GASHA_ lock_guard<unshared_spin_lock> lock_scoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		lock_guard<unshared_spin_lock> lock(*this);
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
	inline GASHA_ shared_lock_guard<unshared_spin_lock> lock_shared_scoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		shared_lock_guard<unshared_spin_lock> lock(*this);
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
	unshared_spin_lock& operator=(unshared_spin_lock&&) = delete;
	//コピーオペレータ
	unshared_spin_lock& operator=(const unshared_spin_lock&) = delete;
public:
	//ムーブコンストラクタ
	unshared_spin_lock(unshared_spin_lock&&) = delete;
	//コピーコンストラクタ
	unshared_spin_lock(const unshared_spin_lock&) = delete;
	//コンストラクタ
	inline unshared_spin_lock()
	{
		m_lock.clear();
	}
	//デストラクタ
	inline ~unshared_spin_lock()
	{}
private:
	//フィールド
	std::atomic_flag m_lock;//ロック用フラグ
};

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__UNSHARED_SPIN_LOCK_H_

// End of file

#pragma once
#ifndef __LW_SPIN_LOCK_H_
#define __LW_SPIN_LOCK_H_

//--------------------------------------------------------------------------------
// lwSpinLock.h
// サイズ軽量スピンロック
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

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

//----------------------------------------
//サイズ軽量スピンロッククラス
//※std::mutex がモデル。
//※サイズは1バイト(bool型一つ分のサイズ)。
//※spin_lockの方が速い。
class lwSpinLock
{
public:
	//メソッド

	//単一ロック取得
	inline GASHA_ unique_lock<lwSpinLock> get_unique_lock(){ GASHA_ unique_lock<lwSpinLock> lock(*this); return lock; }
	inline GASHA_ unique_lock<lwSpinLock> get_unique_lock(const GASHA_ with_lock_t){ GASHA_ unique_lock<lwSpinLock> lock(*this, GASHA_ with_lock); return lock; }
	inline GASHA_ unique_lock<lwSpinLock> get_unique_lock(const GASHA_ try_lock_t){ GASHA_ unique_lock<lwSpinLock> lock(*this, GASHA_ try_lock); return lock; }
	inline GASHA_ unique_lock<lwSpinLock> get_unique_lock(const GASHA_ adopt_lock_t){ GASHA_ unique_lock<lwSpinLock> lock(*this, GASHA_ adopt_lock); return lock; }
	inline GASHA_ unique_lock<lwSpinLock> get_unique_lock(const GASHA_ defer_lock_t){ GASHA_ unique_lock<lwSpinLock> lock(*this, GASHA_ defer_lock); return lock; }

	//ロック取得
	void lock(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT);
	//ロックガード取得
	//※ロック取得を伴う
	inline GASHA_ lock_guard<lwSpinLock> lock_scoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		GASHA_ lock_guard<lwSpinLock> lock(*this);
		return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
	}
	//ロック取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock()
	{
		bool prev = false;
		return m_lock.compare_exchange_weak(prev, true) == false;
	}
	//ロック解放
	inline void unlock()
	{
		m_lock.store(false);
	}
public:
	//ムーブオペレータ
	lwSpinLock& operator=(lwSpinLock&&) = delete;
	//コピーオペレータ
	lwSpinLock& operator=(const lwSpinLock&) = delete;
public:
	//ムーブコンストラクタ
	lwSpinLock(lwSpinLock&&) = delete;
	//コピーコンストラクタ
	lwSpinLock(const lwSpinLock&) = delete;
	//コンストラクタ
	inline lwSpinLock()
	{
		m_lock.store(false);//ロック用フラグ
	}
	//デストラクタ
	inline ~lwSpinLock()
	{}
private:
	//フィールド
	std::atomic_bool m_lock;//ロック用フラグ
};

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__LW_SPIN_LOCK_H_

// End of file

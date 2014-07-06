#pragma once
#ifndef GASHA_INCLUDED_SIMPLE_UNSHARED_SPIN_LOCK_H
#define GASHA_INCLUDED_SIMPLE_UNSHARED_SPIN_LOCK_H

//--------------------------------------------------------------------------------
// unshared_spin_lock.h
// 非共有スピンロック【宣言部】
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

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

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
	inline GASHA_ unique_shared_lock<unsharedSpinLock> lockUnique();
	inline GASHA_ unique_shared_lock<unsharedSpinLock> lockUnique(const GASHA_ with_lock_t);
	inline GASHA_ unique_shared_lock<unsharedSpinLock> lockUnique(const GASHA_ with_lock_shared_t);
	inline GASHA_ unique_shared_lock<unsharedSpinLock> lockUnique(const GASHA_ try_to_lock_t);
	inline GASHA_ unique_shared_lock<unsharedSpinLock> lockUnique(const GASHA_ try_to_lock_shared_t);
	inline GASHA_ unique_shared_lock<unsharedSpinLock> lockUnique(const GASHA_ adopt_lock_t);
	inline GASHA_ unique_shared_lock<unsharedSpinLock> lockUnique(const GASHA_ adopt_shared_lock_t);
	inline GASHA_ unique_shared_lock<unsharedSpinLock> lockUnique(const GASHA_ defer_lock_t);

	//排他ロック（ライトロック）取得
	void lock(const int spin_count = DEFAULT_SPIN_COUNT);
	//排他ロック（ライトロック）用のロックガード取得
	//※排他ロック（ライトロック）取得を伴う
	inline GASHA_ lock_guard<unsharedSpinLock> lockScoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT);
	//排他ロック（ライトロック）取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock();
	//排他ロック（ライトロック）解放
	inline void unlock();

	//共有ロック（リードロック）取得
	inline void lock_shared(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT);
	//共有ロック（リードロック）用のロックガード取得
	//※共有ロック（リードロック）取得を伴う
	inline GASHA_ shared_lock_guard<unsharedSpinLock> lockSharedScoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT);
	//共有ロック（リードロック）取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock_shared();
	//共有ロック（リードロック）解放
	inline void unlock_shared();

	//アップグレード
	//※共有ロックから排他ロックにアップグレード
	inline void upgrade(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT);
	//アップグレードを試行
	inline bool try_upgrade();
	//ダウングレード
	//※排他ロックから共有ロックにダウングレード
	inline void downgrade();
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
	inline unsharedSpinLock();
	//デストラクタ
	inline ~unsharedSpinLock();
private:
	//フィールド
	std::atomic_flag m_lock;//ロック用フラグ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/unshared_spin_lock.inl>

#endif//GASHA_INCLUDED_SIMPLE_UNSHARED_SPIN_LOCK_H

// End of file

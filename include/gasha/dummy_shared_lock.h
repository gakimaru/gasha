#pragma once
#ifndef __DUMMY_SHARED_LOCK_H_
#define __DUMMY_SHARED_LOCK_H_

//--------------------------------------------------------------------------------
// dummy_shared_lock.h
// ダミー共有ロック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_common.h>//ロック共通設定

#include <gasha/unique_shared_lock.h>//単一共有ロック制御
#include <gasha/lock_guard.h>//ロックガード
#include <gasha/shared_lock_guard.h>//共有ロックガード

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//ダミー共有ロッククラス
//※std::shared_mutex がモデル。
//※共有ロッククラスのインターフェースのみ実装し、実際には何もしない。
//※コンテナクラスのロック制御を無効化する際などに使用する。
class dummySharedLock
{
public:
	//メソッド

	//単一ロック取得
	inline GASHA_ unique_shared_lock<dummySharedLock> lockUnique(){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this); return lock; }
	inline GASHA_ unique_shared_lock<dummySharedLock> lockUnique(const GASHA_ with_lock_t){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this, GASHA_ with_lock); return lock; }
	inline GASHA_ unique_shared_lock<dummySharedLock> lockUnique(const GASHA_ with_lock_shared_t){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this, GASHA_ with_lock_shared); return lock; }
	inline GASHA_ unique_shared_lock<dummySharedLock> lockUnique(const GASHA_ try_lock_t){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this, GASHA_ try_lock); return lock; }
	inline GASHA_ unique_shared_lock<dummySharedLock> lockUnique(const GASHA_ try_lock_shared_t){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this, GASHA_ try_lock_shared); return lock; }
	inline GASHA_ unique_shared_lock<dummySharedLock> lockUnique(const GASHA_ adopt_lock_t){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this, GASHA_ adopt_lock); return lock; }
	inline GASHA_ unique_shared_lock<dummySharedLock> lockUnique(const GASHA_ adopt_shared_lock_t){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this, GASHA_ adopt_shared_lock); return lock; }
	inline GASHA_ unique_shared_lock<dummySharedLock> lockUnique(const GASHA_ defer_lock_t){ GASHA_ unique_shared_lock<dummySharedLock> lock(*this, GASHA_ defer_lock); return lock; }

	//排他ロック（ライトロック）取得
	inline void lock(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		//何もしない
	}
	//排他ロック（ライトロック）用のロックガード取得
	//※排他ロック（ライトロック）取得を伴う
	inline lock_guard<dummySharedLock> lockScoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		GASHA_ lock_guard<dummySharedLock> lock(*this);
		return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
	}
	//排他ロック（ライトロック）取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock()
	{
		//何もしない（常に成功）
		return true;
	}
	//排他ロック（ライトロック）解放
	inline void unlock()
	{
		//何もしない
	}

	//共有ロック（リードロック）取得
	inline void lock_shared(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		//何もしない
	}
	//共有ロック（リードロック）用のロックガード取得
	//※共有ロック（リードロック）取得を伴う
	inline GASHA_ shared_lock_guard<dummySharedLock> lockSharedScoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		GASHA_ shared_lock_guard<dummySharedLock> lock(*this);
		return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
	}
	//共有ロック（リードロック）取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock_shared()
	{
		//何もしない（常に成功）
		return true;
	}
	//共有ロック（リードロック）解放
	inline void unlock_shared()
	{
		//何もしない
	}
public:
	//ムーブオペレータ
	dummySharedLock& operator=(dummySharedLock&&) = delete;
	//コピーオペレータ
	dummySharedLock& operator=(const dummySharedLock&) = delete;
public:
	//ムーブコンストラクタ
	dummySharedLock(dummySharedLock&&) = delete;
	//コピーコンストラクタ
	dummySharedLock(const dummySharedLock&) = delete;
	//コンストラクタ
	inline dummySharedLock()
	{
		//何もしない
	}
	//デストラクタ
	inline ~dummySharedLock()
	{
		//何もしない
	}
};

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__DUMMY_SHARED_LOCK_H_

// End of file

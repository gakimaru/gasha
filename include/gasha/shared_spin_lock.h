#pragma once
#ifndef GASHA_INCLUDED_SHARED_SPIN_LOCK_H
#define GASHA_INCLUDED_SHARED_SPIN_LOCK_H

//--------------------------------------------------------------------------------
// shared_spin_lock.h
// 共有スピンロック【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
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
//共有スピンロッククラス
//※std::shared_mutex がモデル。
//※サイズは4バイト(int型一つ分のサイズ)。
//※排他ロック（ライトロック）を優先する
//※読み込み操作（共有ロック）が込み合っている途中で割り込んで
//　書き込み操作（排他ロック）を行いたい時に用いる
//※排他ロックが常に最優先されるわけではない。
//　共有ロックがロックを開放する前に排他ロックがロックを
//　取得することを許可する仕組みで実装する。排他ロックは、
//　共有ロックが全て解放されるのを待ってから処理を続行する。
//　この時、別の排他ロックが待ち状態になっても、共有ロック
//　より先にロックを取得することは保証しない。
class sharedSpinLock
{
public:
	//メソッド

	//単一ロック取得
	inline GASHA_ unique_shared_lock<sharedSpinLock> lockUnique();
	inline GASHA_ unique_shared_lock<sharedSpinLock> lockUnique(const GASHA_ with_lock_t&);
	inline GASHA_ unique_shared_lock<sharedSpinLock> lockUnique(const GASHA_ with_lock_shared_t&);
	inline GASHA_ unique_shared_lock<sharedSpinLock> lockUnique(const GASHA_ try_to_lock_t&);
	inline GASHA_ unique_shared_lock<sharedSpinLock> lockUnique(const GASHA_ try_to_lock_shared_t&);
	inline GASHA_ unique_shared_lock<sharedSpinLock> lockUnique(const GASHA_ adopt_lock_t&);
	inline GASHA_ unique_shared_lock<sharedSpinLock> lockUnique(const GASHA_ adopt_shared_lock_t&);
	inline GASHA_ unique_shared_lock<sharedSpinLock> lockUnique(const GASHA_ defer_lock_t&);

	//排他ロック（ライトロック）取得
	void lock(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT);
	//排他ロック（ライトロック）用のロックガード取得
	//※排他ロック（ライトロック）取得を伴う
	inline GASHA_ lock_guard<sharedSpinLock> lockScoped();
	//排他ロック（ライトロック）取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	bool try_lock();
	//排他ロック（ライトロック）解放
	inline void unlock();

	//共有ロック（リードロック）取得
	void lock_shared(const int spin_count = DEFAULT_SPIN_COUNT);
	//共有ロック（リードロック）用のロックガード取得
	//※共有ロック（リードロック）取得を伴う
	inline GASHA_ shared_lock_guard<sharedSpinLock> lockSharedScoped();
	//共有ロック（リードロック）取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	bool try_lock_shared();
	//共有ロック（リードロック）解放
	inline void unlock_shared();

	//アップグレード
	//※共有ロックから排他ロックにアップグレード
	void upgrade(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT);
	//アップグレードを試行
	bool try_upgrade();
	//ダウングレード
	//※排他ロックから共有ロックにダウングレード
	inline void downgrade();
public:
	//ムーブオペレータ
	//※ムーブではなく、両者のフラグの状態をリセットするので注意
	inline sharedSpinLock& operator=(sharedSpinLock&& rhs);
	//コピーオペレータ
	//※コピーではなく、フラグの状態をリセットするので注意
	inline sharedSpinLock& operator=(const sharedSpinLock& rhs);
public:
	//ムーブコンストラクタ
	//※ムーブではなく、両者のフラグの状態をリセットするので注意
	inline sharedSpinLock(sharedSpinLock&& obj);
	//コピーコンストラクタ
	//※コピーではなく、フラグの状態をリセットするので注意
	inline sharedSpinLock(const sharedSpinLock& obj);
	//コンストラクタ
	inline sharedSpinLock();
	//デストラクタ
	inline ~sharedSpinLock();
private:
	//フィールド
	std::atomic<int> m_lockCounter;//ロックカウンタ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/shared_spin_lock.inl>

#endif//GASHA_INCLUDED_SHARED_SPIN_LOCK_H

// End of file

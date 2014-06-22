#pragma once
#ifndef __SHARED_SPIN_LOCK_H_
#define __SHARED_SPIN_LOCK_H_

//--------------------------------------------------------------------------------
// shared_spin_lock.h
// 共有スピンロック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/build_settings.h>//ビルド設定

#include <gasha/lock_common.h>//ロック共通設定

#include <gasha/unique_shared_lock.h>//共有安全ロック制御
#include <gasha/lock_guard.h>//ロックガード
#include <gasha/shared_lock_guard.h>//共有ロックガード

#include <atomic>//C++11 std::atomic

NAMESPACE_GASHA_BEGIN//ネームスペース：開始

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
class shared_spin_lock
{
public:
	//メソッド

	//安全ロック制御取得
	//※ロック未取得状態とみなして処理する
	inline unique_shared_lock<shared_spin_lock> unique(const bool is_safe_lock = true)
	{
		unique_shared_lock<shared_spin_lock> lock(*this, is_safe_lock);
		return lock;
	}

	//排他ロック（ライトロック）取得
	void lock(const int spin_count = DEFAULT_SPIN_COUNT);
	//排他ロック（ライトロック）用のロックガード取得
	//※排他ロック（ライトロック）取得を伴う
	inline lock_guard<shared_spin_lock> lock_scoped(const int spin_count = DEFAULT_SPIN_COUNT)
	{
		lock_guard<shared_spin_lock> lock(*this);
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
	inline shared_lock_guard<shared_spin_lock> lock_shared_scoped(const int spin_count = DEFAULT_SPIN_COUNT)
	{
		shared_lock_guard<shared_spin_lock> lock(*this);
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
	shared_spin_lock& operator=(shared_spin_lock&&) = delete;
	//コピーオペレータ
	shared_spin_lock& operator=(const shared_spin_lock&) = delete;
public:
	//ムーブコンストラクタ
	shared_spin_lock(shared_spin_lock&&) = delete;
	//コピーコンストラクタ
	shared_spin_lock(const shared_spin_lock&) = delete;
	//コンストラクタ
	inline shared_spin_lock()
	{
		m_lockCounter.store(SHARED_LOCK_COUNTER_UNLOCKED);
	}
	//デストラクタ
	inline ~shared_spin_lock()
	{}
private:
	//フィールド
	std::atomic<int> m_lockCounter;//ロックカウンタ
};

NAMESPACE_GASHA_END//ネームスペース：終了

#endif//__SHARED_SPIN_LOCK_H_

// End of file

#pragma once
#ifndef __UNIQUE_SHARED_LOCK_H_
#define __UNIQUE_SHARED_LOCK_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// unique_shared_lock.h
// 安全共有ロック制御
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/build_settings.h>//ビルド設定

#include <gasha/lock_common.h>//ロック共通設定

NAMESPACE_GASHA_BEGIN//ネームスペース：開始

//----------------------------------------
//共有安全ロック制御クラス
//※std::unique_lock がモデル。
//※実装を隠ぺいして共有ロックを操作するためのヘルパークラス。
//※デストラクタでロックの解放を確実に行うことができる。
template<class T>
class unique_shared_lock
{
public:
	typedef T lock_type;//ロックオブジェクト型
	enum status_t//ロック取得状態型
	{
		UNLOCKED = 0,//未ロック状態
		LOCKING_EXCLUSIVELY,//排他ロック状態
		LOCKING_SHARED,//共有ロック状態
	};
public:
	//メソッド

	//排他ロック（ライトロック）取得
	inline void lock(const int spin_count = DEFAULT_SPIN_COUNT)
	{
		if(m_isSafeLock && m_status != UNLOCKED)
			return;
		m_lock.lock(spin_count);
		m_status = LOCKING_EXCLUSIVELY;
	}
	//排他ロック（ライトロック）取得を試行
	inline bool try_lock()
	{
		if(m_isSafeLock && m_status != UNLOCKED)
			return true;
		const bool locked = m_lock.try_lock();
		if(locked)
			m_status = LOCKING_EXCLUSIVELY;
		return locked;
	}
	//排他ロック（ライトロック）解放
	inline void unlock()
	{
		if(m_isSafeLock && m_status != LOCKING_EXCLUSIVELY)
			return;
		m_lock.unlock();
		m_status = UNLOCKED;
	}

	//共有ロック（リードロック）取得
	inline void lock_shared(const int spin_count = DEFAULT_SPIN_COUNT)
	{
		if(m_isSafeLock && m_status != UNLOCKED)
			return;
		m_lock.lock_shared(spin_count);
		m_status = LOCKING_SHARED;
	}
	//共有ロック（リードロック）取得を試行
	inline bool try_lock_shared()
	{
		if(m_isSafeLock && m_status != UNLOCKED)
			return true;
		const bool locked = m_lock.try_lock_shared();
		if(locked)
			m_status = LOCKING_SHARED;
		return locked;
	}
	//共有ロック（リードロック）解放
	inline void unlock_shared()
	{
		if(m_isSafeLock && m_status != LOCKING_SHARED)
			return;
		m_lock.unlock_shared();
		m_status = UNLOCKED;
	}
private:
	//ロック取得状態をチェックしてアンロック
	inline void check_and_unlock()
	{
		if(m_isSafeLock)
		{
			if (m_status == LOCKING_EXCLUSIVELY)
				unlock();
			else if(m_status == LOCKING_SHARED)
				unlock_shared();
		}
	}
public:
	//ムーブオペレータ
	inline unique_shared_lock& operator=(unique_shared_lock&& rhs)
	{
		check_and_unlock();
		m_lock = rhs.m_lock;
		m_status = rhs.m_status;
		m_isSafeLock = rhs.m_isSafeLock;
		rhs.m_status = UNLOCKED;
		return *this;
	}
	//コピーオペレータ
	unique_shared_lock& operator=(const unique_shared_lock&) = delete;
public:
	//ムーブコンストラクタ
	inline unique_shared_lock(unique_shared_lock&& obj) :
		m_lock(obj.m_lock),
		m_status(obj.m_status),
		m_isSafeLock(obj.m_isSafeLock)
	{
		obj.m_status = UNLOCKED;
	}
	//コピーコンストラクタ
	unique_shared_lock(const unique_shared_lock&) = delete;
	//コンストラクタ
	inline explicit unique_shared_lock(lock_type& lock, const bool is_safe_lock = true) :
		m_lock(lock),
		m_status(UNLOCKED),
		m_isSafeLock(is_safe_lock)
	{}
	//デストラクタ
	inline ~unique_shared_lock()
	{
		check_and_unlock();
	}
private:
	//フィールド
	lock_type& m_lock;//ロックオブジェクトの参照
	status_t m_status;//ロック取得状態
	bool m_isSafeLock;//ロック安全　※ロック取得中フラグに基づいて、lock, unlock, デストラクタを処理する
};

NAMESPACE_GASHA_END//ネームスペース：終了

#endif//__UNIQUE_SHARED_LOCK_H_

// End of file

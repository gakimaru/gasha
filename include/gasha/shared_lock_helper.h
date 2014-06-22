#pragma once
#ifndef __SHARED_LOCK_HELPER_H_
#define __SHARED_LOCK_HELPER_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_lock_helper.h
// 共有ロックヘルパー
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
//共有ロックヘルパークラス
//※実装を隠ぺいして共有ロックを操作するためのヘルパークラス
template<class T>
class shared_lock_helper
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
		m_status = LOCKING_EXLUSIVELY;
	}
	//排他ロック（ライトロック）取得を試行
	inline bool try_lock()
	{
		if(m_isSafeLock && m_status != UNLOCKED)
			return true;
		const bool locked m_lock.try_lock();
		if(locked)
			m_status = LOCKING_EXLUSIVELY;
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
		const bool locked m_lock.try_lock_shared();
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
public:
	//ムーブオペレータ
	shared_lock_helper& operator=(shared_lock_helper&&) = delete;
	//コピーオペレータ
	shared_lock_helper& operator=(const shared_lock_helper&) = delete;
public:
	//ムーブコンストラクタ
	inline explicit shared_lock_helper(shared_lock_helper&& obj) :
		m_lock(obj.m_lock),
		m_status(obj.m_status),
		m_isAutoUnlock(obj.m_isAutoUnlock)
	{
		obj.m_status = UNLOCKED;
	}
	//コピーコンストラクタ
	//shared_lock_helper(const shared_lock_helper&) = delete;
	//コンストラクタ
	inline explicit shared_lock_helper(lock_type& lock, const bool is_safe_lock = true) :
		m_lock(lock),
		m_status(UNLOCKED),
		m_isSafeLock(is_safe_lock)
	{}
	//デストラクタ
	inline ~shared_lock_helper()
	{
		if(m_isSafeLock)
		{
			if (m_status == LOCKING_EXCLUSIVELY)
				unlock();
			else if(m_status == LOCKING_SHARED)
				unlock_shared();
		}
	}
private:
	//フィールド
	lock_type& m_lock;//ロックオブジェクトの参照
	status_t m_status;//ロック取得状態
	bool m_isSafeLock;//ロック安全　※ロック取得中フラグに基づいて、lock, unlock, デストラクタを処理する
};

NAMESPACE_GASHA_END//ネームスペース：終了

#endif//__SHARED_LOCK_HELPER_H_

// End of file

#pragma once
#ifndef __UNIQUE_SHARED_LOCK_H_
#define __UNIQUE_SHARED_LOCK_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// unique_shared_lock.h
// 単一共有ロック制御
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_common.h>//ロック共通設定

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//単一共有ロッククラス
//※std::unique_shared_lock がモデル。
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
	//キャストオペレータ
	inline operator bool() const { return m_status != UNLOCKED; }//排他／共有ロック取得中か？
public:
	//アクセッサ
	inline bool owns_lock() const { return m_status == LOCKING_EXCLUSIVELY; }//排他ロック取得中か？
	inline bool owns_shared_lock() const { return m_status == LOCKING_SHARED; }//共有ロック取得中か？
public:
	//メソッド

	//排他ロック（ライトロック）取得
	inline void lock()
	{
		if (!m_lock || m_status != UNLOCKED)
			return;
		m_lock->lock();
		m_status = LOCKING_EXCLUSIVELY;
	}
	//排他ロック（ライトロック）取得を試行
	inline bool try_lock()
	{
		if (!m_lock)
			return false;
		if (m_status == LOCKING_EXCLUSIVELY)
			return true;
		else if (m_status != UNLOCKED)
			return false;
		const bool locked = m_lock->try_lock();
		if(locked)
			m_status = LOCKING_EXCLUSIVELY;
		return locked;
	}
	//排他ロック（ライトロック）解放
	inline void unlock()
	{
		if (!m_lock || m_status != LOCKING_EXCLUSIVELY)
			return;
		m_lock->unlock();
		m_status = UNLOCKED;
	}

	//共有ロック（リードロック）取得
	inline void lock_shared()
	{
		if (!m_lock || m_status != UNLOCKED)
			return;
		m_lock->lock_shared();
		m_status = LOCKING_SHARED;
	}
	//共有ロック（リードロック）取得を試行
	inline bool try_lock_shared()
	{
		if (!m_lock)
			return false;
		if (m_status == LOCKING_SHARED)
			return true;
		else if (m_status != UNLOCKED)
			return false;
		const bool locked = m_lock->try_lock_shared();
		if(locked)
			m_status = LOCKING_SHARED;
		return locked;
	}
	//共有ロック（リードロック）解放
	inline void unlock_shared()
	{
		if (!m_lock || m_status != LOCKING_SHARED)
			return;
		m_lock->unlock_shared();
		m_status = UNLOCKED;
	}
	//ロックの所有権を放棄する
	lock_type* release()
	{
		lock_type* lock = m_lock;
		m_lock = nullptr;
		m_status = UNLOCKED;
		return lock;
	}
	//ロックの所有権を交換する
	inline void swap(unique_shared_lock& obj)
	{
		lock_type* lock_tmp = m_lock;
		status_t status_tmp = m_status;
		m_lock = obj.m_lock;
		m_status = obj.m_status;
		obj.m_lock = lock_tmp;
		obj.m_status = status_tmp;
	}
public:
	//ムーブオペレータ
	inline unique_shared_lock& operator=(unique_shared_lock&& rhs)
	{
		unlock();
		unlock_shared();
		m_lock = rhs.m_lock;
		m_status = rhs.m_status;
		rhs.m_status = UNLOCKED;
		return *this;
	}
	//コピーオペレータ
	unique_shared_lock& operator=(const unique_shared_lock&) = delete;
public:
	//ムーブコンストラクタ
	inline unique_shared_lock(unique_shared_lock&& obj) :
		m_lock(obj.m_lock),
		m_status(obj.m_status)
	{
		obj.m_status = UNLOCKED;
	}
	//コピーコンストラクタ
	unique_shared_lock(const unique_shared_lock&) = delete;
	//コンストラクタ
	inline explicit unique_shared_lock(lock_type& obj) :
		m_lock(&obj),
		m_status(UNLOCKED)
	{
		lock_shared();
	}
	inline unique_shared_lock(lock_type& obj, const with_lock_t) :
		m_lock(&obj),
		m_status(UNLOCKED)
	{
		lock();
	}
	inline unique_shared_lock(lock_type& obj, const with_lock_shared_t) :
		m_lock(&obj),
		m_status(UNLOCKED)
	{
		lock_shared();
	}
	inline unique_shared_lock(lock_type& obj, const try_lock_t) :
		m_lock(&obj),
		m_status(UNLOCKED)
	{
		try_lock();
	}
	inline unique_shared_lock(lock_type& obj, const try_lock_shared_t) :
		m_lock(&obj),
		m_status(UNLOCKED)
	{
		try_lock_shared();
	}
	inline unique_shared_lock(lock_type& obj, const adopt_lock_t) :
		m_lock(&obj),
		m_status(LOCKING_EXCLUSIVELY)
	{}
	inline unique_shared_lock(lock_type& obj, const adopt_shared_lock_t) :
		m_lock(&obj),
		m_status(LOCKING_SHARED)
	{}
	inline unique_shared_lock(lock_type& obj, const defer_lock_t) :
		m_lock(&obj),
		m_status(UNLOCKED)
	{}
	//デフォルトコンストラクタ
	inline unique_shared_lock() :
		m_lock(nullptr),
		m_status(UNLOCKED)
	{}
	//デストラクタ
	inline ~unique_shared_lock()
	{
		unlock();
		unlock_shared();
	}
private:
	//フィールド
	lock_type* m_lock;//ロックオブジェクトの参照
	status_t m_status;//ロック取得状態
};

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__UNIQUE_SHARED_LOCK_H_

// End of file

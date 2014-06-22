#pragma once
#ifndef __UNIQUE_LOCK_H_
#define __UNIQUE_LOCK_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// unique_lock.h
// 単一ロック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_common.h>//ロック共通設定

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

//----------------------------------------
//単一ロッククラス
//※std::unique_lock がモデル。
//※実装を隠ぺいしてロックを操作するためのヘルパークラス。
//※デストラクタでロックの解放を確実に行うことができる。
template<class T>
class unique_lock
{
public:
	typedef T lock_type;//ロックオブジェクト型
public:
	//キャストオペレータ
	inline operator bool() const { return m_isLocked; }//ロック取得中か？
public:
	//アクセッサ
	inline bool owns_lock() const { return m_isLocked; }//ロック取得中か？
public:
	//メソッド

	//ロック取得
	inline void lock(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT)
	{
		if (!m_lock || m_isLocked)
			return;
		m_lock->lock(spin_count);
		m_isLocked = true;
	}
	//ロック取得を試行
	inline bool try_lock()
	{
		if (!m_lock)
			return false;
		if (m_isLocked)
			return true;
		m_isLocked = m_lock->try_lock();
		return m_isLocked;
	}
	//ロック解放
	inline void unlock()
	{
		if (!m_lock || !m_isLocked)
			return;
		m_lock->unlock();
		m_isLocked = false;
	}
	//ロックの所有権を放棄する
	inline lock_type* release()
	{
		lock_type* lock = m_lock;
		m_lock = nullptr;
		m_isLocked = false;
		return lock;
	}
	//ロックの所有権を交換する
	inline void swap(unique_lock& obj)
	{
		lock_type* lock_tmp = m_lock;
		bool is_locked_tmp = m_isLocked;
		m_lock = obj.m_lock;
		m_isLocked = obj.m_isLocked;
		obj.m_lock = lock_tmp;
		obj.m_isLocked = is_locked_tmp;
	}
public:
	//ムーブオペレータ
	inline unique_lock& operator=(unique_lock&& rhs)
	{
		unlock();
		m_lock = rhs.m_lock;
		m_isLocked = rhs.m_isLocked;
		rhs.m_isLocked = false;
		return *this;
	}
	//コピーオペレータ
	unique_lock& operator=(const unique_lock&) = delete;
public:
	//ムーブコンストラクタ
	inline unique_lock(unique_lock&& obj) :
		m_lock(obj.m_lock),
		m_isLocked(obj.m_isLocked)
	{
		obj.m_isLocked = false;
	}
	//コピーコンストラクタ
	unique_lock(const unique_lock&) = delete;
	//コンストラクタ
	inline explicit unique_lock(lock_type& obj, const int spin_count = GASHA_ DEFAULT_SPIN_COUNT) :
		m_lock(&obj),
		m_isLocked(false)
	{
		lock(spin_count);
	}
	inline unique_lock(lock_type& obj, const with_lock_t, const int spin_count = GASHA_ DEFAULT_SPIN_COUNT) :
		m_lock(&obj),
		m_isLocked(false)
	{
		lock(spin_count);
	}
	inline unique_lock(lock_type& obj, const try_lock_t) :
		m_lock(&obj),
		m_isLocked(false)
	{
		try_lock();
	}
	inline unique_lock(lock_type& obj, const adopt_lock_t) :
		m_lock(&obj),
		m_isLocked(true)
	{}
	inline unique_lock(lock_type& obj, const defer_lock_t) :
		m_lock(&obj),
		m_isLocked(false)
	{}
	inline explicit unique_lock(lock_type& obj) :
		m_lock(&obj),
		m_isLocked(false)
	{}
	//デフォルトコンストラクタ
	inline unique_lock() :
		m_lock(nullptr),
		m_isLocked(false)
	{}
	//デストラクタ
	inline ~unique_lock()
	{
		unlock();
	}
private:
	//フィールド
	lock_type* m_lock;//ロックオブジェクトの参照
	bool m_isLocked;//ロック取得中フラグ
};

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__UNIQUE_LOCK_H_

// End of file

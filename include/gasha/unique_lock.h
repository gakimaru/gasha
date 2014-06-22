#pragma once
#ifndef __UNIQUE_LOCK_H_
#define __UNIQUE_LOCK_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// unique_lock.h
// 安全ロック制御
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
//安全ロック制御クラス
//※std::unique_lock がモデル。
//※実装を隠ぺいしてロックを操作するためのヘルパークラス。
//※デストラクタでロックの解放を確実に行うことができる。
template<class T>
class unique_lock
{
public:
	typedef T lock_type;//ロックオブジェクト型
public:
	//メソッド

	//ロック取得
	inline void lock(const int spin_count = DEFAULT_SPIN_COUNT)
	{
		if(m_isSafeLock && m_isLocked)
			return;
		m_lock.lock(spin_count);
		m_isLocked = true;
	}
	//ロック取得を試行
	inline bool try_lock()
	{
		if(m_isSafeLock && m_isLocked)
			return true;
		m_isLocked = m_lock.try_lock();
		return m_isLocked;
	}
	//ロック解放
	inline void unlock()
	{
		if(m_isSafeLock && !m_isLocked)
			return;
		m_lock.unlock();
		m_isLocked = false;
	}
private:
	//ロック取得状態をチェックしてアンロック
	inline void check_and_unlock()
	{
		if(m_isSafeLock && m_isLocked)
			unlock();
	}
public:
	//ムーブオペレータ
	inline unique_lock& operator=(unique_lock&& rhs)
	{
		check_and_unlock();
		m_lock = rhs.m_lock;
		m_isLocked = rhs.m_isLocked;
		m_isSafeLock = rhs.m_isSafeLock;
		rhs.m_isLocked = false;
		return *this;
	}
	//コピーオペレータ
	unique_lock& operator=(const unique_lock&) = delete;
public:
	//ムーブコンストラクタ
	inline unique_lock(unique_lock&& obj) :
		m_lock(obj.m_lock),
		m_isLocked(obj.m_isLocked),
		m_isSafeLock(obj.m_isSafeLock)
	{
		obj.m_isLocked = false;
	}
	//コピーコンストラクタ
	unique_lock(const unique_lock&) = delete;
	//コンストラクタ
	inline explicit unique_lock(lock_type& lock, const bool is_safe_lock = true) :
		m_lock(lock),
		m_isLocked(false),
		m_isSafeLock(is_safe_lock)
	{}
	//デストラクタ
	inline ~unique_lock()
	{
		check_and_unlock();
	}
private:
	//フィールド
	lock_type& m_lock;//ロックオブジェクトの参照
	bool m_isLocked;//ロック取得中フラグ
	bool m_isSafeLock;//ロック安全　※ロック取得中フラグに基づいて、lock, unlock, デストラクタを処理する
};

NAMESPACE_GASHA_END//ネームスペース：終了

#endif//__UNIQUE_LOCK_H_

// End of file

#pragma once
#ifndef __LOCK_HELPER_H_
#define __LOCK_HELPER_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lock_helper.h
// ロックヘルパー
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
//ロックヘルパークラス
//※実装を隠ぺいしてロックを操作するためのヘルパークラス。
template<class T>
class lock_helper
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
public:
	//ムーブオペレータ
	lock_helper& operator=(lock_helper&&) = delete;
	//コピーオペレータ
	lock_helper& operator=(const lock_helper&) = delete;
public:
	//ムーブコンストラクタ
	inline explicit lock_helper(lock_helper&& obj) :
		m_lock(obj.m_lock),
		m_isLocked(obj.m_isLocked),
		m_isAutoUnlock(obj.m_isAutoUnlock)
	{
		obj.m_isLocked = false;
	}
	//コピーコンストラクタ
	//lock_helper(const lock_helper&) = delete;
	//コンストラクタ
	inline explicit lock_helper(lock_type& lock, const bool is_safe_lock = true) :
		m_lock(lock),
		m_isLocked(false),
		m_isSafeLock(is_safe_lock)
	{}
	//デストラクタ
	inline ~lock_helper()
	{
		if(m_isSafeLock && m_isLocked)
			unlock();
	}
private:
	//フィールド
	lock_type& m_lock;//ロックオブジェクトの参照
	bool m_isLocked;//ロック取得中フラグ
	bool m_isSafeLock;//ロック安全　※ロック取得中フラグに基づいて、lock, unlock, デストラクタを処理する
};

NAMESPACE_GASHA_END//ネームスペース：終了

#endif//__LOCK_HELPER_H_

// End of file

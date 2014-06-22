#pragma once
#ifndef __SHARED_shared_lock_guard_H_
#define __SHARED_shared_lock_guard_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_shared_lock_guard.h
// 共有ロックガード（スコープロック）
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
//共有ロックガードクラス（スコープロック）
//※std::lock_guard がモデル。
//※スコープロックで共有ロック（リードロック）のロック取得と解放を行う
template<class T>
class shared_lock_guard
{
public:
	typedef T lock_type;//ロックオブジェクト型
public:
	//ムーブオペレータ
	shared_lock_guard& operator=(shared_lock_guard&&) = delete;
	//コピーオペレータ
	shared_lock_guard& operator=(const shared_lock_guard&) = delete;
public:
	//ムーブコンストラクタ
	inline shared_lock_guard(shared_lock_guard&& obj) :
		m_lock(obj.m_lock),
		m_isLocked(obj.m_isLocked)
	{
		obj.m_isLocked = false;
	}
	//コピーコンストラクタ
	shared_lock_guard(const shared_lock_guard&) = delete;
	//コンストラクタ
	inline explicit shared_lock_guard(lock_type& lock, const int spin_count = DEFAULT_SPIN_COUNT) :
		m_lock(lock),
		m_isLocked(true)
	{
		m_lock.lock_shared(spin_count);
	}
	//デストラクタ
	inline ~shared_lock_guard()
	{
		if(m_isLocked)
			m_lock.unlock_shared();
	}
private:
	//フィールド
	lock_type& m_lock;//ロックオブジェクトの参照
	bool m_isLocked;//ロック取得中フラグ
};

NAMESPACE_GASHA_END//ネームスペース：終了

#endif//__SHARED_shared_lock_guard_H_

// End of file

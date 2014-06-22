#pragma once
#ifndef __LOCK_GUARD_H_
#define __LOCK_GUARD_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lock_guard.h
// ロックガード（スコープロック）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_common.h>//ロック共通設定

#include <mutex>

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

//----------------------------------------
//ロックガードクラス（スコープロック）
//※std::lock_guard がモデル。
//※スコープロックで通常ロックもしくは排他ロック（ライトロック）のロック取得と解放を行う。
template<class T>
class lock_guard
{
public:
	typedef T lock_type;//ロックオブジェクト型
public:
	//ムーブオペレータ
	lock_guard& operator=(lock_guard&&) = delete;
	//コピーオペレータ
	lock_guard& operator=(const lock_guard&) = delete;
public:
	//ムーブコンストラクタ
	inline lock_guard(lock_guard&& obj) :
		m_lock(obj.m_lock),
		m_isLocked(obj.m_isLocked)
	{
		obj.m_isLocked = false;
	}
	//コピーコンストラクタ
	lock_guard(const lock_guard&) = delete;
	//コンストラクタ
	inline explicit lock_guard(lock_type& lock, const int spin_count = DEFAULT_SPIN_COUNT) :
		m_lock(lock),
		m_isLocked(true)
	{
		m_lock.lock(spin_count);
	}
	//デストラクタ
	inline ~lock_guard()
	{
		if(m_isLocked)
			m_lock.unlock();
	}
private:
	//フィールド
	lock_type& m_lock;//ロックオブジェクトの参照
	bool m_isLocked;//ロック取得中フラグ
};

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__LOCK_GUARD_H_

// End of file

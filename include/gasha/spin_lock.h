#pragma once
#ifndef __SPIN_LOCK_H_
#define __SPIN_LOCK_H_

//--------------------------------------------------------------------------------
// spin_lock.h
// スピンロック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/build_settings.h>//ビルド設定

#include <gasha/lock_common.h>//ロック共通設定

#include <gasha/unique_lock.h>//安全ロック制御
#include <gasha/lock_guard.h>//ロックガード

#include <atomic>//C++11 std::atomic

NAMESPACE_GASHA_BEGIN//ネームスペース：開始

//----------------------------------------
//スピンロッククラス
//※std::mutex がモデル。
//※サイズは4バイト(std::atomic_flag一つ分のサイズ)。
class spin_lock
{
public:
	//メソッド

	//安全ロック制御取得
	//※ロック未取得状態とみなして処理する
	inline unique_lock<spin_lock> unique(const bool is_safe_lock = true)
	{
		unique_lock<spin_lock> lock(*this, is_safe_lock);
		return lock;
	}
	
	//ロック取得
	void lock(const int spin_count = DEFAULT_SPIN_COUNT);
	//ロックガード取得
	//※ロック取得を伴う
	inline lock_guard<spin_lock> lock_scoped(const int spin_count = DEFAULT_SPIN_COUNT)
	{
		lock_guard<spin_lock> lock(*this);
		return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
	}
	//ロック取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock()
	{
		return m_lock.test_and_set() == false;
	}
	//ロック解放
	inline void unlock()
	{
		m_lock.clear();
	}
public:
	//ムーブオペレータ
	spin_lock& operator=(spin_lock&&) = delete;
	//コピーオペレータ
	spin_lock& operator=(const spin_lock&) = delete;
public:
	//ムーブコンストラクタ
	spin_lock(spin_lock&&) = delete;
	//コピーコンストラクタ
	spin_lock(const spin_lock&) = delete;
	//コンストラクタ
	inline spin_lock()
	{
		m_lock.clear();
	}
	//デストラクタ
	inline ~spin_lock()
	{}
private:
	//フィールド
	std::atomic_flag m_lock;//ロック用フラグ
};

NAMESPACE_GASHA_END//ネームスペース：終了

#endif//__SPIN_LOCK_H_

// End of file

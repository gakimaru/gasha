#pragma once
#ifndef __LW_SPIN_LOCK_H_
#define __LW_SPIN_LOCK_H_

//--------------------------------------------------------------------------------
// lw_spin_lock.h
// サイズ軽量スピンロック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/build_settings.h>//ビルド設定

#include <gasha/lock_common.h>//ロック共通設定

#include <gasha/lock_helper.h>//ロックヘルパー
#include <gasha/lock_guard.h>//ロックガード

#include <atomic>//C++11 std::atomic

NAMESPACE_GASHA_BEGIN//ネームスペース：開始

//----------------------------------------
//サイズ軽量スピンロッククラス
//※std::mutexがモデル。
//※サイズは1バイト(bool型一つ分のサイズ)。
//※spin_lockの方が速い。
class lw_spin_lock
{
public:
	//メソッド

	//ロックヘルパー取得
	//※未ロック状態とみなして処理する
	inline lock_helper<lw_spin_lock> get_helper(const bool is_safe_lock = true)
	{
		lock_helper<lw_spin_lock> helper(*this, is_safe_lock);
		return helper;
	}

	//ロック取得
	void lock(const int spin_count = DEFAULT_SPIN_COUNT);
	//ロックガード取得
	//※ロック取得を伴う
	inline lock_guard<lw_spin_lock> lock_scoped(const int spin_count = DEFAULT_SPIN_COUNT)
	{
		lock_guard<lw_spin_lock> lock(*this);
		return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
	}
	//ロック取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock()
	{
		bool prev = false;
		return m_lock.compare_exchange_weak(prev, true) == false;
	}
	//ロック解放
	inline void unlock()
	{
		m_lock.store(false);
	}
public:
	//ムーブオペレータ
	lw_spin_lock& operator=(lw_spin_lock&&) = delete;
	//コピーオペレータ
	lw_spin_lock& operator=(const lw_spin_lock&) = delete;
public:
	//ムーブコンストラクタ
	lw_spin_lock(lw_spin_lock&&) = delete;
	//コピーコンストラクタ
	lw_spin_lock(const lw_spin_lock&) = delete;
	//コンストラクタ
	inline lw_spin_lock()
	{
		m_lock.store(false);//ロック用フラグ
	}
	//デストラクタ
	inline ~lw_spin_lock()
	{}
private:
	//フィールド
	std::atomic_bool m_lock;//ロック用フラグ
};

NAMESPACE_GASHA_END//ネームスペース：終了

#endif//__LW_SPIN_LOCK_H_

// End of file

#pragma once
#ifndef __DUMMY_LOCK_H_
#define __DUMMY_LOCK_H_

//--------------------------------------------------------------------------------
// dummy_lock.h
// ダミーロック
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

NAMESPACE_GASHA_BEGIN//ネームスペース：開始

//----------------------------------------
//ダミーロッククラス
//※std::mutex がモデル。
//※ロッククラスのインターフェースのみ実装し、実際には何もしない。
//※コンテナクラスのロック制御を無効化する際などに使用する。
class dummy_lock
{
public:
	//メソッド

	//安全ロック制御取得
	inline unique_lock<dummy_lock> unique(const bool is_safe_lock = true)
	{
		unique_lock<dummy_lock> lock(*this, is_safe_lock);
		return lock;
	}
	
	//ロック取得
	inline void lock(const int dummy_count = DEFAULT_SPIN_COUNT)
	{
		//何もしない
	}
	//ロックガード取得
	inline lock_guard<dummy_lock> lock_scoped(const int dummy_count = DEFAULT_SPIN_COUNT)
	{
		lock_guard<dummy_lock> lock(*this);
		return lock;
	}
	//ロック取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock()
	{
		//何もしない（常に成功）
		return true;
	}
	//ロック解放
	inline void unlock()
	{
		//何もしない
	}
public:
	//ムーブオペレータ
	dummy_lock& operator=(dummy_lock&&) = delete;
	//コピーオペレータ
	dummy_lock& operator=(const dummy_lock&) = delete;
public:
	//ムーブコンストラクタ
	dummy_lock(dummy_lock&&) = delete;
	//コピーコンストラクタ
	dummy_lock(const dummy_lock&) = delete;
	//コンストラクタ
	inline dummy_lock()
	{
		//何もしない
	}
	//デストラクタ
	inline ~dummy_lock()
	{
		//何もしない
	}
};

NAMESPACE_GASHA_END//ネームスペース：終了

#endif//__DUMMY_LOCK_H_

// End of file

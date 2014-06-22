#pragma once
#ifndef __DUMMY_SHARED_LOCK_H_
#define __DUMMY_SHARED_LOCK_H_

//--------------------------------------------------------------------------------
// dummy_shared_lock.h
// ダミー共有ロック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/build_settings.h>//ビルド設定

#include <gasha/lock_common.h>//ロック共通設定

#include <gasha/unique_shared_lock.h>//安全共有ロック制御
#include <gasha/lock_guard.h>//ロックガード

NAMESPACE_GASHA_BEGIN//ネームスペース：開始

//----------------------------------------
//ダミー共有ロッククラス
//※std::shared_mutex がモデル。
//※共有ロッククラスのインターフェースのみ実装し、実際には何もしない。
//※コンテナクラスのロック制御を無効化する際などに使用する。
class dummy_shared_lock
{
public:
	//メソッド

	//共有安全ロック制御取得
	inline unique_shared_lock<dummy_shared_lock> unique(const bool is_safe_lock = true)
	{
		unique_shared_lock<dummy_shared_lock> lock(*this, is_safe_lock);
		return lock;
	}

	//排他ロック（ライトロック）取得
	inline void lock(const int spin_count = DEFAULT_SPIN_COUNT)
	{
		//何もしない
	}
	//排他ロック（ライトロック）用のロックガード取得
	//※排他ロック（ライトロック）取得を伴う
	inline lock_guard<dummy_shared_lock> lock_scoped(const int spin_count = DEFAULT_SPIN_COUNT)
	{
		lock_guard<dummy_shared_lock> lock(*this);
		return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
	}
	//排他ロック（ライトロック）取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock()
	{
		//何もしない（常に成功）
		return true;
	}
	//排他ロック（ライトロック）解放
	inline void unlock()
	{
		//何もしない
	}

	//共有ロック（リードロック）取得
	inline void lock_shared(const int spin_count = DEFAULT_SPIN_COUNT)
	{
		//何もしない
	}
	//共有ロック（リードロック）用のロックガード取得
	//※共有ロック（リードロック）取得を伴う
	inline shared_lock_guard<dummy_shared_lock> lock_shared_scoped(const int spin_count = DEFAULT_SPIN_COUNT)
	{
		shared_lock_guard<dummy_shared_lock> lock(*this);
		return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
	}
	//共有ロック（リードロック）取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock_shared()
	{
		//何もしない（常に成功）
		return true;
	}
	//共有ロック（リードロック）解放
	inline void unlock_shared()
	{
		//何もしない
	}
public:
	//ムーブオペレータ
	dummy_shared_lock& operator=(dummy_shared_lock&&) = delete;
	//コピーオペレータ
	dummy_shared_lock& operator=(const dummy_shared_lock&) = delete;
public:
	//ムーブコンストラクタ
	dummy_shared_lock(dummy_shared_lock&&) = delete;
	//コピーコンストラクタ
	dummy_shared_lock(const dummy_shared_lock&) = delete;
	//コンストラクタ
	inline dummy_shared_lock()
	{
		//何もしない
	}
	//デストラクタ
	inline ~dummy_shared_lock()
	{
		//何もしない
	}
};

NAMESPACE_GASHA_END//ネームスペース：終了

#endif//__DUMMY_SHARED_LOCK_H_

// End of file

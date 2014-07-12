#pragma once
#ifndef GASHA_INCLUDED_SPIN_LOCK_H
#define GASHA_INCLUDED_SPIN_LOCK_H

//--------------------------------------------------------------------------------
// spin_lock.h
// スピンロック【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_common.h>//ロック共通設定

#include <gasha/unique_lock.h>//単一ロック
#include <gasha/lock_guard.h>//ロックガード

#include <atomic>//C++11 std::atomic

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//スピンロッククラス
//※std::mutex がモデル。
//※サイズは4バイト(std::atomic_flag一つ分のサイズ)。
class spinLock
{
public:
	//メソッド

	//単一ロック取得
	inline GASHA_ unique_lock<spinLock> lockUnique();
	inline GASHA_ unique_lock<spinLock> lockUnique(const GASHA_ with_lock_t&);
	inline GASHA_ unique_lock<spinLock> lockUnique(const GASHA_ try_to_lock_t&);
	inline GASHA_ unique_lock<spinLock> lockUnique(const GASHA_ adopt_lock_t&);
	inline GASHA_ unique_lock<spinLock> lockUnique(const GASHA_ defer_lock_t&);

	//ロック取得
	void lock(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT);
	//ロックガード取得
	//※ロック取得を伴う
	inline GASHA_ lock_guard<spinLock> lockScoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT);
	//ロック取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock();
	//ロック解放
	inline void unlock();
public:
	//ムーブオペレータ
	spinLock& operator=(spinLock&&) = delete;
	//コピーオペレータ
	spinLock& operator=(const spinLock&) = delete;
public:
	//ムーブコンストラクタ
	spinLock(spinLock&&) = delete;
	//コピーコンストラクタ
	spinLock(const spinLock&) = delete;
	//コンストラクタ
	inline spinLock();
	//デストラクタ
	inline ~spinLock();
private:
	//フィールド
	std::atomic_flag m_lock;//ロック用フラグ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/spin_lock.inl>

#endif//GASHA_INCLUDED_SPIN_LOCK_H

// End of file

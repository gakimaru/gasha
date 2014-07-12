#pragma once
#ifndef GASHA_INCLUDED_LW_SPIN_LOCK_H
#define GASHA_INCLUDED_LW_SPIN_LOCK_H

//--------------------------------------------------------------------------------
// lw_spin_lock.h
// サイズ軽量スピンロック【宣言部】
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
//サイズ軽量スピンロッククラス
//※std::mutex がモデル。
//※サイズは1バイト(bool型一つ分のサイズ)。
//※spin_lockの方が速い。
class lwSpinLock
{
public:
	//メソッド

	//単一ロック取得
	inline GASHA_ unique_lock<lwSpinLock> lockUnique();
	inline GASHA_ unique_lock<lwSpinLock> lockUnique(const GASHA_ with_lock_t&);
	inline GASHA_ unique_lock<lwSpinLock> lockUnique(const GASHA_ try_to_lock_t&);
	inline GASHA_ unique_lock<lwSpinLock> lockUnique(const GASHA_ adopt_lock_t&);
	inline GASHA_ unique_lock<lwSpinLock> lockUnique(const GASHA_ defer_lock_t&);

	//ロック取得
	void lock(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT);
	//ロックガード取得
	//※ロック取得を伴う
	inline GASHA_ lock_guard<lwSpinLock> lockScoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT);
	//ロック取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock();
	//ロック解放
	inline void unlock();
public:
	//ムーブオペレータ
	lwSpinLock& operator=(lwSpinLock&&) = delete;
	//コピーオペレータ
	lwSpinLock& operator=(const lwSpinLock&) = delete;
public:
	//ムーブコンストラクタ
	lwSpinLock(lwSpinLock&&) = delete;
	//コピーコンストラクタ
	lwSpinLock(const lwSpinLock&) = delete;
	//コンストラクタ
	inline lwSpinLock();
	//デストラクタ
	inline ~lwSpinLock();
private:
	//フィールド
	std::atomic_bool m_lock;//ロック用フラグ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/lw_spin_lock.inl>

#endif//GASHA_INCLUDED_LW_SPIN_LOCK_H

// End of file

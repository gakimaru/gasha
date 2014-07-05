#pragma once
#ifndef GASHA_INCLUDED_DUMMY_LOCK_H
#define GASHA_INCLUDED_DUMMY_LOCK_H

//--------------------------------------------------------------------------------
// dummy_lock.h
// ダミーロック【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_common.h>//ロック共通設定

#include <gasha/unique_lock.h>//単一ロック
#include <gasha/lock_guard.h>//ロックガード

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//ダミーロッククラス
//※std::mutex がモデル。
//※ロッククラスのインターフェースのみ実装し、実際には何もしない。
//※コンテナクラスのロック制御を無効化する際などに使用する。
class dummyLock
{
public:
	//メソッド

	//単一ロック取得
	inline GASHA_ unique_lock<dummyLock> lockUnique();
	inline GASHA_ unique_lock<dummyLock> lockUnique(const GASHA_ with_lock_t);
	inline GASHA_ unique_lock<dummyLock> lockUnique(const GASHA_ try_lock_t);
	inline GASHA_ unique_lock<dummyLock> lockUnique(const GASHA_ adopt_lock_t);
	inline GASHA_ unique_lock<dummyLock> lockUnique(const GASHA_ defer_lock_t);

	//ロック取得
	inline void lock(const int dummy_count = GASHA_ DEFAULT_SPIN_COUNT);
	//ロックガード取得
	inline GASHA_ lock_guard<dummyLock> lockScoped(const int dummy_count = GASHA_ DEFAULT_SPIN_COUNT);
	//ロック取得を試行
	//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
	inline bool try_lock();
	//ロック解放
	inline void unlock();
public:
	//ムーブオペレータ
	dummyLock& operator=(dummyLock&&) = delete;
	//コピーオペレータ
	dummyLock& operator=(const dummyLock&) = delete;
public:
	//ムーブコンストラクタ
	dummyLock(dummyLock&&) = delete;
	//コピーコンストラクタ
	dummyLock(const dummyLock&) = delete;
	//コンストラクタ
	inline dummyLock();
	//デストラクタ
	inline ~dummyLock();
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/dummy_lock.inl>

#endif//GASHA_INCLUDED_DUMMY_LOCK_H

// End of file

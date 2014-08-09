#pragma once
#ifndef GASHA_INCLUDED_UNIQUE_SHARED_LOCK_H
#define GASHA_INCLUDED_UNIQUE_SHARED_LOCK_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// unique_shared_lock.h
// 単一共有ロック【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_common.h>//ロック共通設定

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//単一共有ロッククラス
//※std::unique_shared_lock がモデル。
//※実装を隠ぺいして共有ロックを操作するためのヘルパークラス。
//※デストラクタでロックの解放を確実に行うことができる。
template<class T>
class unique_shared_lock
{
public:
	typedef T lock_type;//ロックオブジェクト型
	enum status_t//ロック取得状態型
	{
		UNLOCKED = 0,//未ロック状態
		LOCKING_EXCLUSIVELY,//排他ロック状態
		LOCKING_SHARED,//共有ロック状態
	};
public:
	//キャストオペレータ
	inline operator bool() const { return owns_any_lock(); }//排他／共有ロック取得中か？
public:
	//アクセッサ
	inline bool owns_any_lock() const { return m_status != UNLOCKED; }//排他／共有ロック取得中か？
	inline bool owns_lock() const { return m_status == LOCKING_EXCLUSIVELY; }//排他ロック取得中か？
	inline bool owns_shared_lock() const { return m_status == LOCKING_SHARED; }//共有ロック取得中か？
public:
	//メソッド

	//排他ロック（ライトロック）取得
	inline void lock();
	//排他ロック（ライトロック）取得を試行
	inline bool try_lock();
	//排他ロック（ライトロック）解放
	inline void unlock();

	//共有ロック（リードロック）取得
	inline void lock_shared();
	//共有ロック（リードロック）取得を試行
	inline bool try_lock_shared();
	//共有ロック（リードロック）解放
	inline void unlock_shared();
	
	//アップグレード
	//※共有ロックから排他ロックにアップグレード
	inline bool upgrade();
	//アップグレードを試行
	inline bool try_upgrade();
	//ダウングレード
	//※排他ロックから共有ロックにダウングレード
	inline bool downgrade();

	//ロックの所有権を放棄する
	lock_type* release();
	//ロックの所有権を交換する
	inline void swap(unique_shared_lock& obj);
public:
	//ムーブオペレータ
	inline unique_shared_lock& operator=(unique_shared_lock&& rhs);
	//コピーオペレータ
	unique_shared_lock& operator=(const unique_shared_lock&) = delete;
public:
	//ムーブコンストラクタ
	inline unique_shared_lock(unique_shared_lock&& obj);
	//コピーコンストラクタ
	unique_shared_lock(const unique_shared_lock&) = delete;
	//コンストラクタ
	inline explicit unique_shared_lock(lock_type& obj);
	inline unique_shared_lock(lock_type& obj, const with_lock_t&);
	inline unique_shared_lock(lock_type& obj, const with_lock_shared_t&);
	inline unique_shared_lock(lock_type& obj, const try_to_lock_t&);
	inline unique_shared_lock(lock_type& obj, const try_to_lock_shared_t&);
	inline unique_shared_lock(lock_type& obj, const adopt_lock_t&);
	inline unique_shared_lock(lock_type& obj, const adopt_shared_lock_t&);
	inline unique_shared_lock(lock_type& obj, const defer_lock_t&);
	//デフォルトコンストラクタ
	inline unique_shared_lock();
	//デストラクタ
	inline ~unique_shared_lock();
private:
	//フィールド
	lock_type* m_lock;//ロックオブジェクトの参照
	status_t m_status;//ロック取得状態
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/unique_shared_lock.inl>

#endif//GASHA_INCLUDED_UNIQUE_SHARED_LOCK_H

// End of file

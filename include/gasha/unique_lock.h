#pragma once
#ifndef GASHA_INCLUDED_UNIQUE_LOCK_H
#define GASHA_INCLUDED_UNIQUE_LOCK_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// unique_lock.h
// 単一ロック【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_common.h>//ロック共通設定

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//単一ロッククラス
//※std::unique_lock がモデル。
//※実装を隠ぺいしてロックを操作するためのヘルパークラス。
//※デストラクタでロックの解放を確実に行うことができる。
template<class T>
class unique_lock
{
public:
	typedef T lock_type;//ロックオブジェクト型
public:
	//キャストオペレータ
	inline operator bool() const { return m_isLocked; }//ロック取得中か？
public:
	//アクセッサ
	inline bool owns_lock() const { return m_isLocked; }//ロック取得中か？
public:
	//メソッド

	//ロック取得
	inline void lock();
	//ロック取得を試行
	inline bool try_lock();
	//ロック解放
	inline void unlock();
	//ロックの所有権を放棄する
	inline lock_type* release();
	//ロックの所有権を交換する
	inline void swap(unique_lock& obj);
public:
	//ムーブオペレータ
	inline unique_lock& operator=(unique_lock&& rhs);
	//コピーオペレータ
	unique_lock& operator=(const unique_lock&) = delete;
public:
	//ムーブコンストラクタ
	inline unique_lock(unique_lock&& obj);
	//コピーコンストラクタ
	unique_lock(const unique_lock&) = delete;
	//コンストラクタ
	inline explicit unique_lock(lock_type& obj);
	inline unique_lock(lock_type& obj, const with_lock_t&);
	inline unique_lock(lock_type& obj, const try_to_lock_t&);
	inline unique_lock(lock_type& obj, const adopt_lock_t&);
	inline unique_lock(lock_type& obj, const defer_lock_t&);
	//デフォルトコンストラクタ
	inline unique_lock();
	//デストラクタ
	inline ~unique_lock();
private:
	//フィールド
	lock_type* m_lock;//ロックオブジェクトの参照
	bool m_isLocked;//ロック取得中フラグ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/unique_lock.inl>

#endif//GASHA_INCLUDED_UNIQUE_LOCK_H

// End of file

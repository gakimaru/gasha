#pragma once
#ifndef GASHA_INCLUDED_SHARED_LOCK_GUARD_H
#define GASHA_INCLUDED_SHARED_LOCK_GUARD_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_lock_guard.h
// 共有ロックガード（スコープロック）【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_common.h>//ロック共通設定

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//共有ロックガードクラス（スコープロック）
//※std::lock_guard がモデル。
//※スコープロックで共有ロック（リードロック）のロック取得と解放を行う
template<class T>
class shared_lock_guard
{
public:
	typedef T lock_type;//ロックオブジェクト型
public:
	//ムーブオペレータ
	shared_lock_guard& operator=(shared_lock_guard&&) = delete;
	//コピーオペレータ
	shared_lock_guard& operator=(const shared_lock_guard&) = delete;
public:
	//アップグレード
	//※共有ロックから排他ロックにアップグレード
	inline bool upgrade();
	//アップグレードを試行
	inline bool try_upgrade();
	//ダウングレード
	//※排他ロックから共有ロックにダウングレード
	inline bool downgrade();
public:
	//ムーブコンストラクタ
	inline shared_lock_guard(shared_lock_guard&& obj);
	//コピーコンストラクタ
	shared_lock_guard(const shared_lock_guard&) = delete;
	//コンストラクタ
	inline explicit shared_lock_guard(lock_type& lock);
	//デストラクタ
	inline ~shared_lock_guard();
private:
	//フィールド
	lock_type& m_lock;//ロックオブジェクトの参照
	bool m_isLocked;//ロック取得中フラグ
	bool m_isUpgraded;//アップグレード状態フラグ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/shared_lock_guard.inl>

#endif//GASHA_INCLUDED_SHARED_LOCK_GUARD_H

// End of file

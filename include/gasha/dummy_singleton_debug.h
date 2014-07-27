#pragma once
#ifndef GASHA_INCLUDED_DUMMY_SINGLETON_DEBUG_H
#define GASHA_INCLUDED_DUMMY_SINGLETON_DEBUG_H

//--------------------------------------------------------------------------------
// dummy_singleton_debug.h
// シングルトンデバッグ用ダミー処理【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dummy_shared_lock.h>//ダミー共有ロック
#include <gasha/unique_shared_lock.h>//単一共有ロック

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シングルトンデバッグ用ダミー処理
//--------------------------------------------------------------------------------

//----------------------------------------
//シングルトンデバッグ用ダミー処理
//※何もしないクラス
class dummySingletonDebug
{
public:
	//型
	struct id_type
	{
		inline bool operator==(id_type&& rhs) const { return true; }
		inline bool operator==(const id_type& rhs) const { return true; }
		inline bool operator!=(id_type&& rhs) const { return false; }
		inline bool operator!=(const id_type& rhs) const { return false; }
	};
public:
	inline id_type invalidId() const { return id_type(); }//不正なIDを取得（初期値用）
	inline bool isInvalidId(const id_type& id) const { return true; }//不正なIDか？
public:
	//シングルトン生成時呼び出し
	inline bool create(const char* procedure_name);
	//シングルトン破棄時呼び出し
	inline bool destroy(const char* procedure_name);
	//シングルトンアクセス開始時呼び出し
	inline id_type enter(const char* procedure_name);
	//シングルトンアクセス終了時呼び出し
	inline bool leave(const id_type id);
	//デバッグ情報作成
	std::size_t debugInfo(char* message, const std::size_t max_size) const;
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/dummy_singleton_debug.inl>

#endif//GASHA_INCLUDED_DUMMY_SINGLETON_DEBUG_H

// End of file

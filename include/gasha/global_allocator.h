#pragma once
#ifndef GASHA_INCLUDED_GLOBAL_ALLOCATOR_H
#define GASHA_INCLUDED_GLOBAL_ALLOCATOR_H

//--------------------------------------------------------------------------------
// global_allocator.h
// グローバルアロケータ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/allocator_common.h>//メモリアロケータ共通設定
#include <gasha/memory.h>//メモリ操作：adjustStaticAlign, adjustAlign()
#include <gasha/allocator_adapter.h>//アロケータアダプター
#include <gasha/dummy_lock.h>//ダミーロック

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//グローバルアロケータクラス
//※内部で _aligned_malloc(), _aligned_free() を使用してメモリを確保。
//※基本的にそのままでスレッドセーフのはずだが、コンパイラによってスレッドセーブでない場合は、ロック型を指定することができる。
template<class LOCK_TYPE = GASHA_ dummyLock>
class globalAllocator
{
public:
	//型
	typedef LOCK_TYPE lock_type;//ロック型
	typedef std::uint32_t size_type;//サイズ型

public:
	//アクセッサ
	inline size_type maxSize() const;//バッファの全体サイズ（バイト数）
	inline size_type size() const;//使用中のサイズ（バイト数）
	inline size_type remain() const;//残りサイズ（バイト数）

public:
	//アロケータアダプター取得
	inline GASHA_ allocatorAdapter<globalAllocator<LOCK_TYPE>> adapter(){ GASHA_ allocatorAdapter<globalAllocator<LOCK_TYPE>> adapter(*this, "globalAllocator"); return adapter; }

public:
	//メソッド

	//メモリ確保
	inline void* alloc(const std::size_t size, const std::size_t align = GASHA_ DEFAULT_ALIGN);

	//メモリ解放
	inline bool free(void* p);

	//メモリ確保とコンストラクタ呼び出し
	template<typename T, typename...Tx>
	T* newObj(Tx&&... args);
	//※配列用
	template<typename T, typename...Tx>
	T* newArray(const std::size_t num, Tx&&... args);

	//メモリ解放とデストラクタ呼び出し
	template<typename T>
	bool deleteObj(T* p);
	//※配列用（要素数の指定が必要な点に注意）
	template<typename T>
	bool deleteArray(T* p, const std::size_t num);
	
	//デバッグ情報作成
	//※十分なサイズのバッファを渡す必要あり。
	//※使用したバッファのサイズを返す。
	//※作成中、ロックを取得する。
	std::size_t debugInfo(char* message);

private:
	//メモリ解放（共通処理）
	//※ロック取得は呼び出し元で行う
	inline bool _free(void* p);

	//ポインタが範囲内か判定
	inline bool isInUsingRange(void* p);

public:
	//コンストラクタ
	inline globalAllocator();
	//デストラクタ
	inline ~globalAllocator();

private:
	//フィールド
	lock_type m_lock;//ロックオブジェクト
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/global_allocator.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_GLOBAL_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H
#include <gasha/global_allocator.cpp.h>
#endif//GASHA_GLOBAL_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_GLOBAL_ALLOCATOR_H

// End of file

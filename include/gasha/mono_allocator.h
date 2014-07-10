#pragma once
#ifndef GASHA_INCLUDED_MONO_ALLOCATOR_H
#define GASHA_INCLUDED_MONO_ALLOCATOR_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// mono_allocator.h
// 単一アロケータ【宣言部】
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
//単一アロケータクラス
//※バッファをコンストラクタで受け渡して使用
template<class LOCK_TYPE = GASHA_ dummyLock>
class monoAllocator
{
public:
	//型
	typedef LOCK_TYPE lock_type;//ロック型
	typedef std::uint32_t size_type;//サイズ型

public:
	//アクセッサ
	const char* name() const { return "monoAllocator"; }
	const char* mode() const { return "-"; }
	inline const void* buff() const { return reinterpret_cast<const void*>(m_buffRef); }//バッファの先頭アドレス
	inline size_type maxSize() const { return m_maxSize; }//バッファの全体サイズ（バイト数）
	inline size_type size() const { return m_size; }//使用中のサイズ（バイト数）
	inline size_type remain() const { return m_maxSize - size(); }//残りサイズ（バイト数）
	inline bool isAllocated() const { return m_size > 0; }//アロケート中か？

public:
	//アロケータアダプター取得
	inline GASHA_ allocatorAdapter<monoAllocator<LOCK_TYPE>> adapter(){ GASHA_ allocatorAdapter<monoAllocator<LOCK_TYPE>> adapter(*this, name(), mode()); return adapter; }

public:
	//メソッド

	//メモリ確保
	void* alloc(const std::size_t size, const std::size_t align = GASHA_ DEFAULT_ALIGN);

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
	bool _free(void* p);

	//ポインタが範囲内か判定
	inline bool isInUsingRange(void* p);

public:
	//コンストラクタ
	inline monoAllocator(void* buff, const std::size_t max_size);
	template<typename T>
	inline monoAllocator(T* buff, const std::size_t num);
	template<typename T, std::size_t N>
	inline monoAllocator(T (&buff)[N]);
	//デストラクタ
	inline ~monoAllocator();

private:
	//フィールド
	char* m_buffRef;//バッファの参照
	const size_type m_maxSize;//バッファの全体サイズ
	size_type m_size;//バッファの使用中サイズ
	lock_type m_lock;//ロックオブジェクト
};

//--------------------------------------------------------------------------------
//バッファ付き単一アロケータクラス
template<std::size_t _MAX_SIZE, class LOCK_TYPE = GASHA_ dummyLock>
class monoAllocator_withBuff : public monoAllocator<LOCK_TYPE>
{
	//定数
	static const std::size_t MAX_SIZE = _MAX_SIZE;//バッファの全体サイズ
public:
	//コンストラクタ
	inline monoAllocator_withBuff();
	//デストラクタ
	inline ~monoAllocator_withBuff();
private:
	char m_buff[MAX_SIZE];//バッファ
};
//----------------------------------------
//※バッファを基本型とその個数で指定
//※アラインメント分余計にバッファを確保するので注意
template<typename T, std::size_t _NUM, class LOCK_TYPE = GASHA_ dummyLock>
class monoAllocator_withType : public monoAllocator<LOCK_TYPE>
{
public:
	//型
	typedef T value_type;//値の型
public:
	//定数
	static const std::size_t VALUE_ALIGN = alignof(value_type);//基本型のアラインメント
	static const std::size_t VALUE_SIZE = sizeof(value_type);//基本型のサイズ
	static const std::size_t VALUE_NUM = _NUM;//基本型の確保可能数
	static const std::size_t MAX_SIZE = VALUE_SIZE * VALUE_NUM + VALUE_ALIGN;//バッファの全体サイズ ※アラインメント分余計に確保する
public:
	//デフォルト型のメモリ確保とコンストラクタ呼び出し
	template<typename... Tx>
	inline value_type* newDefault(Tx&&... args);
	
	//デフォルト型のメモリ解放とデストラクタ呼び出し
	inline bool deleteDefault(value_type*& p);
public:
	//コンストラクタ
	inline monoAllocator_withType();
	//デストラクタ
	inline ~monoAllocator_withType();
private:
	GASHA_ALIGNAS_OF(value_type) char m_buff[MAX_SIZE];//バッファ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/mono_allocator.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_MONO_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H
#include <gasha/mono_allocator.cpp.h>
#endif//GASHA_MONO_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_MONO_ALLOCATOR_H

// End of file

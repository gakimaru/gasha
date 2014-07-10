#pragma once
#ifndef GASHA_INCLUDED_ALLOCATOR_ADAPTER_H
#define GASHA_INCLUDED_ALLOCATOR_ADAPTER_H

//--------------------------------------------------------------------------------
// allocator_adapter.h
// アロケータアダプター【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/allocator_common.h>//メモリアロケータ共通設定

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アロケータアダプタークラス
//※アロケータの実装を隠ぺいして、アロケータを共通利用できるようにするためのインターフェース。
class IAllocatorAdapter
{
public:
	//型
	typedef std::uint32_t size_type;//サイズ型

public:
	//アクセッサ
	virtual size_type maxSize() const = 0;//バッファの全体サイズ（バイト数）
	virtual size_type size() const = 0;//使用中のサイズ（バイト数）
	virtual size_type remain() const = 0;//残りサイズ（バイト数）

public:
	//メソッド
	//メモリ確保
	virtual void* alloc(const std::size_t size, const std::size_t align) = 0;

	//メモリ解放
	virtual bool free(void* p) = 0;

#if 0//※テンプレート関数は仮想化不可（一定のvtableが確定できないため）
	//メモリ確保とコンストラクタ呼び出し
	template<typename T, typename...Tx>
	virtual T* newObj(Tx&&... args) = 0;
	//※配列用
	template<typename T, typename...Tx>
	virtual T* newArray(const std::size_t num, Tx&&... args) = 0;

	//メモリ解放とデストラクタ呼び出し
	template<typename T>
	virtual bool deleteObj(T* p) = 0;
	//※配列用（要素数の指定が必要な点に注意）
	template<typename T>
	virtual bool deleteArray(T* p, const std::size_t num) = 0;
#endif//DELETE

	//デバッグ情報作成
	virtual std::size_t debugInfo(char* message) = 0;
};

//--------------------------------------------------------------------------------
//アロケータアダプタークラス
//※アロケータの実装を隠ぺいして、共通インターフェースでアロケータを利用できるようにする。
//※既存のアロケータをコンストラクタで受け渡して使用する。
template<class ALLOCATOR>
class allocatorAdapter : public IAllocatorAdapter
{
public:
	//型
	typedef ALLOCATOR allocator_type;//アロケータ型
	typedef std::uint32_t size_type;//サイズ型

public:
	//アクセッサ
	inline size_type maxSize() const;//バッファの全体サイズ（バイト数）
	inline size_type size() const;//使用中のサイズ（バイト数）
	inline size_type remain() const;//残りサイズ（バイト数）

public:
	//メソッド
	//メモリ確保
	inline void* alloc(const std::size_t size, const std::size_t align = GASHA_ DEFAULT_ALIGN) override;

	//メモリ解放
	inline bool free(void* p) override;

#if 0//※テンプレート関数は仮想化不可（一定のvtableが確定できないため）
	//メモリ確保とコンストラクタ呼び出し
	template<typename T, typename...Tx>
	inline T* newObj(Tx&&... args) override;
	//※配列用
	template<typename T, typename...Tx>
	inline T* newArray(const std::size_t num, Tx&&... args) override;

	//メモリ解放とデストラクタ呼び出し
	template<typename T>
	inline bool deleteObj(T* p) override;
	//※配列用（要素数の指定が必要な点に注意）
	template<typename T>
	inline bool deleteArray(T* p, const std::size_t num) override;
#endif//DELETE

	//デバッグ情報作成
	inline std::size_t debugInfo(char* message) override;

public:
	//コンストラクタ
	inline allocatorAdapter(allocatorAdapter<ALLOCATOR>&& allocator);
	inline allocatorAdapter(const allocatorAdapter<ALLOCATOR>& allocator);
	inline allocatorAdapter(allocator_type&& allocator);
	inline allocatorAdapter(allocator_type& allocator);
	//デストラクタ
	inline ~allocatorAdapter();
private:
	//フィールド
	allocator_type& m_allocator;//アロケータ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/allocator_adapter.inl>

#endif//GASHA_INCLUDED_ALLOCATOR_ADAPTER_H

// End of file

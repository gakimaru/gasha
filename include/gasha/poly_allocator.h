﻿#pragma once
#ifndef GASHA_INCLUDED_POLY_ALLOCATOR_H
#define GASHA_INCLUDED_POLY_ALLOCATOR_H

//--------------------------------------------------------------------------------
// poly_allocator.h
// 多態アロケータ【宣言部】
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
//多態アロケータクラス
//※アロケータの実装を隠ぺいして、共通インターフェースでアロケータを利用できるようにする。
//※既存のアロケータをコンストラクタで受け渡して使用する。
template<class ALLOCATOR>
class polyAllocator : public IAllocatorAdapter
{
public:
	//型
	typedef ALLOCATOR allocator_type;//アロケータ型
	typedef std::uint32_t size_type;//サイズ型

public:
	//アクセッサ
	inline const char* name() const override;//アロケータ名
	inline size_type maxSize() const override;//バッファの全体サイズ（バイト数）
	inline size_type size() const override;//使用中のサイズ（バイト数）
	inline size_type remain() const override;//残りサイズ（バイト数）

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
	inline polyAllocator(polyAllocator<ALLOCATOR>&& allocator);
	inline polyAllocator(const polyAllocator<ALLOCATOR>& allocator);
	inline polyAllocator(allocator_type&& allocator, const char* name = "(unknown)");
	inline polyAllocator(allocator_type& allocator, const char* name = "(unknown)");
	//デストラクタ
	inline ~polyAllocator();
private:
	//フィールド
	allocator_type& m_allocator;//アロケータ
	const char* m_name;//アロケータ名
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/poly_allocator.inl>

#endif//GASHA_INCLUDED_POLY_ALLOCATOR_H

// End of file

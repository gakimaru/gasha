#pragma once
#ifndef GASHA_INCLUDED_SCOPED_STACK_ALLOCATOR_H
#define GASHA_INCLUDED_SCOPED_STACK_ALLOCATOR_H

//--------------------------------------------------------------------------------
// scoped_stack_allocator.h
// スコープスタックアロケータ【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/allocator_common.h>//メモリアロケータ共通設定
#include <gasha/allocator_adapter.h>//アロケータアダプタ

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//スコープスタックアロケータクラス
//※既存のスタックアロケータをコンストラクタで受け渡して使用する。
//※スコープの範囲内でのみ有効なスタックアロケータ。スコープを抜けると、使用していたメモリを全て解放する。
//※メモリのクリア／リワインドもスコープの範囲内に制限される。
//※【注】マルチスレッド非対応。他のスレッドとアロケータを共有している時には使えない。
//※【注】スコープ内での自動リセット（スマートスタック）には対応しない。（ただし、全体のアロケータ数が0になったら自動リセットが働く）
template<class ALLOCATOR>
class scopedStackAllocator
{
public:
	//型
	typedef ALLOCATOR allocator_type;//アロケータ型
	typedef std::uint32_t size_type;//サイズ型

public:
	//アクセッサ
	inline const char* name() const;//名前
	inline const char* mode() const;//実装モード
	inline size_type maxSize() const;//バッファの全体サイズ（バイト数）
	inline size_type size() const;//使用中のサイズ（バイト数）
	inline size_type remain() const;//残りサイズ（バイト数）
	inline size_type count() const;//アロケート中の数

public:
	//アロケータアダプタ取得
	inline GASHA_ allocatorAdapter<scopedStackAllocator<ALLOCATOR>> adapter(){ GASHA_ allocatorAdapter<scopedStackAllocator<ALLOCATOR>> adapter(*this, name(), mode()); return adapter; }

public:
	//メソッド
	//メモリ確保
	inline void* alloc(const std::size_t size, const std::size_t align = GASHA_ DEFAULT_ALIGN);

	//メモリ解放
	inline bool free(void* p);

	//メモリ確保とコンストラクタ呼び出し
	template<typename T, typename...Tx>
	inline T* newObj(Tx&&... args);
	//※配列用
	template<typename T, typename...Tx>
	inline T* newArray(const std::size_t num, Tx&&... args);

	//メモリ解放とデストラクタ呼び出し
	template<typename T>
	inline bool deleteObj(T* p);
	//※配列用（要素数の指定が必要な点に注意）
	template<typename T>
	inline bool deleteArray(T* p, const std::size_t num);

	//使用中のサイズを指定位置に戻す
	inline bool rewind(const size_type pos);
	//※ポインタ指定版（スコープの範囲内に制限しないので注意）
	inline bool rewind(void* p);

	//メモリクリア
	inline void clear();
	
	//デバッグ情報作成
	std::size_t debugInfo(char* message, const std::size_t max_size) const;

public:
	//コンストラクタ
	inline scopedStackAllocator(allocator_type& allocator);
	//デストラクタ
	inline ~scopedStackAllocator();
private:
	//フィールド
	allocator_type& m_allocator;//アロケータ
	size_type m_initSize;//初期使用サイズ
	size_type m_initCount;//初期アロケート数
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/scoped_stack_allocator.inl>

#endif//GASHA_INCLUDED_SCOPED_STACK_ALLOCATOR_H

// End of file

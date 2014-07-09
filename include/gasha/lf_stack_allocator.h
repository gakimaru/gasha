#pragma once
#ifndef GASHA_INCLUDED_LF_STACK_ALLOCATOR_H
#define GASHA_INCLUDED_LF_STACK_ALLOCATOR_H

//--------------------------------------------------------------------------------
// lf_stack_allocator.h
// ロックフリースタックアロケータ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/allocator_common.h>//メモリアロケータ共通設定
#include <gasha/memory.h>//メモリ操作：adjustStaticAlign, adjustAlign()

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t
#include <atomic>//C++11 std::atomic

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//クラス宣言
template<class AUTO_CLEAR>
class lfStackAllocator;

//--------------------------------------------------------------------------------
//ロックフリースタックアロケータ補助クラス

//----------------------------------------
//スタック自動クリア
class lfStackAllocatorAutoClear
{
public:
	//自動クリア
	template<class AUTO_CLEAR>
	inline void autoClear(lfStackAllocator<AUTO_CLEAR>& allocator);
};

//----------------------------------------
//スタック自動クリア（ダミー）
//※何もしない
class dummyLfStackAllocatorAutoClear
{
public:
	//自動クリア
	template<class AUTO_CLEAR>
	inline void autoClear(lfStackAllocator<AUTO_CLEAR>& allocator);
};

//--------------------------------------------------------------------------------
//ロックフリースタックアロケータクラス
//※スタック用のバッファをコンストラクタで受け渡して使用
//※free()を呼んでも、明示的に clear() または rewind() しない限り、バッファが解放されないので注意。
template<class AUTO_CLEAR = dummyLfStackAllocatorAutoClear>
class lfStackAllocator
{
	friend lfStackAllocatorAutoClear;//スタック自動クリア
public:
	//型
	typedef AUTO_CLEAR auto_clear_type;//スタック自動クリア型
	typedef std::uint32_t size_type;//サイズ型

public:
	//アクセッサ
	inline size_type maxSize() const { return m_maxSize; }//バッファの全体サイズ（バイト数）
	inline size_type size() const { return  m_size.load(); }//使用中のサイズ（バイト数）
	inline size_type remain() const { return m_maxSize - size(); }//残りサイズ（バイト数）
	inline size_type allocatedCount() const { return m_allocatedCount.load(); }//アロケート中の数

public:
	//メソッド
	//メモリ確保
	void* alloc(const std::size_t size, const std::size_t align = GASHA_ DEFAULT_ALIGN);

	//メモリ解放
	//※実際にはメモリを解放しない（できない）ので注意。
	//※アロケート中の数を減らすだけ。
	//※アロケート中の数を見て、バッファを全クリアしても良いかどうかの判断に用いる。
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

	//使用中のサイズを指定位置に戻す
	//※【注意】メモリ確保状態（アロケート中の数）と無関係に実行するので注意
	//※【注意】自動クリア（スマートスタック）使用時には使用禁止
	//　（メモリ解放時のアドレスが不正なアドレスと見なされて、アロケート中の数が正しく更新されなくなり、
	//　　自動クリアが機能しなくなるため）
	//※位置指定版
	inline bool rewind(const size_type pos);
	//※ポインタ指定版
	bool rewind(void* p);

	//メモリクリア
	//※メモリ確保状態（アロケート中の数）と無関係に実行するので注意
	//※初期状態にする
	inline void clear();
	
	//デバッグ情報作成
	//※十分なサイズのバッファを渡す必要あり。
	//※使用したバッファのサイズを返す。
	//※作成中、他のスレッドで操作が発生すると、不整合が生じる可能性がある点に注意
	std::size_t debugInfo(char* message);

private:
	//メモリ解放（共通処理）
	bool _free(void* p);

	//ポインタが範囲内か判定
	inline bool isInUsingRange(void* p);

public:
	//コンストラクタ
	inline lfStackAllocator(void* buff, const std::size_t max_size);
	template<typename T>
	inline lfStackAllocator(T* buff, const std::size_t num);
	template<typename T, std::size_t N>
	inline lfStackAllocator(T (&buff)[N]);
	//デストラクタ
	inline ~lfStackAllocator();

private:
	//フィールド
	char* m_buffRef;//バッファの参照
	const size_type m_maxSize;//バッファの全体サイズ
	std::atomic<size_type> m_size;//バッファの使用中サイズ
	std::atomic<size_type> m_allocatedCount;//アロケート中の数
};

//--------------------------------------------------------------------------------
//バッファ付きロックフリースタックアロケータクラス
template<std::size_t _MAX_SIZE, class AUTO_CLEAR = dummyLfStackAllocatorAutoClear>
class lfStackAllocator_withBuff : public lfStackAllocator<AUTO_CLEAR>
{
	//定数
	static const std::size_t MAX_SIZE = _MAX_SIZE;//バッファの全体サイズ
public:
	//コンストラクタ
	inline lfStackAllocator_withBuff();
	//デストラクタ
	inline ~lfStackAllocator_withBuff();
private:
	char m_buff[MAX_SIZE];//バッファ
};
//----------------------------------------
//※バッファを基本型とその個数で指定
//※アラインメント分余計にバッファを確保するので注意
template<typename T, std::size_t _NUM, class AUTO_CLEAR = dummyLfStackAllocatorAutoClear>
class lfStackAllocator_withType : public lfStackAllocator<AUTO_CLEAR>
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
	inline lfStackAllocator_withType();
	//デストラクタ
	inline ~lfStackAllocator_withType();
private:
	GASHA_ALIGNAS_OF(value_type) char m_buff[MAX_SIZE];//バッファ
};

//----------------------------------------
//ロックフリースタックアロケータ別名定義：スマートロックフリースタックアロケータ
//※明示的にクリアしなくても、参照がなくなった時に自動的にクリアする。

//※スタック用のバッファをコンストラクタで受け渡して使用
using lfSmartStackAllocator = lfStackAllocator<lfStackAllocatorAutoClear>;

//※バッファ付き
template<std::size_t _MAX_SIZE>
using lfSmartStackAllocator_withBuff = lfStackAllocator_withBuff<_MAX_SIZE, lfStackAllocatorAutoClear>;

//※バッファ付き（基本型とその個数で指定）
template<typename T, std::size_t _SIZE>
using lfSmartStackAllocator_withType = lfStackAllocator_withType<T, _SIZE, lfStackAllocatorAutoClear>;

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/lf_stack_allocator.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_LF_STACK_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H
#include <gasha/lf_stack_allocator.cpp.h>
#endif//GASHA_LF_STACK_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_LF_STACK_ALLOCATOR_H

// End of file

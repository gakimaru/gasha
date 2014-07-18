#pragma once
#ifndef GASHA_INCLUDED_STACK_ALLOCATOR_H
#define GASHA_INCLUDED_STACK_ALLOCATOR_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// stack_allocator.h
// スタックアロケータ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/allocator_common.h>//メモリアロケータ共通設定
#include <gasha/memory.h>//メモリ操作：adjustStaticAlign, adjustAlign()
#include <gasha/scoped_stack_allocator.h>//スコープスタックアロケータ
#include <gasha/allocator_adapter.h>//アロケータアダプター
#include <gasha/dummy_lock.h>//ダミーロック

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//スタックアロケータ補助クラス

//クラス宣言
template<class LOCK_TYPE, class AUTO_CLEAR>
class stackAllocator;

//----------------------------------------
//スタック自動クリア
class stackAllocatorAutoClear
{
public:
	//名前
	inline static const char* name(){ return "AutoClear"; }
	//自動クリア
	template<class LOCK_TYPE, class AUTO_CLEAR>
	inline void autoClear(stackAllocator<LOCK_TYPE, AUTO_CLEAR>& allocator);
};

//----------------------------------------
//スタック自動クリア（ダミー）
//※何もしない
class dummyStackAllocatorAutoClear
{
public:
	//名前
	inline static const char* name(){ return "ManualClear"; }
	//自動クリア
	template<class LOCK_TYPE, class AUTO_CLEAR>
	inline void autoClear(stackAllocator<LOCK_TYPE, AUTO_CLEAR>& allocator);
};

//--------------------------------------------------------------------------------
//スタックアロケータクラス
//※スタック用のバッファをコンストラクタで受け渡して使用
//※free()を呼んでも、明示的に clear() または rewind() しない限り、バッファが解放されないので注意。
template<class LOCK_TYPE = GASHA_ dummyLock, class AUTO_CLEAR = dummyStackAllocatorAutoClear>
class stackAllocator
{
	friend stackAllocatorAutoClear;//スタック自動クリア
public:
	//型
	typedef LOCK_TYPE lock_type;//ロック型
	typedef AUTO_CLEAR auto_clear_type;//スタック自動クリア型
	typedef std::uint32_t size_type;//サイズ型

public:
	//アクセッサ
	const char* name() const { return "stackAllocator"; }
	const char* mode() const { return auto_clear_type::name(); }
	inline const void* buff() const { return reinterpret_cast<const void*>(m_buffRef); }//バッファの先頭アドレス
	inline size_type maxSize() const { return m_maxSize; }//バッファの全体サイズ（バイト数）
	inline size_type size() const { return m_size; }//使用中のサイズ（バイト数）
	inline size_type remain() const { return m_maxSize - size(); }//残りサイズ（バイト数）
	inline size_type count() const { return m_count; }//アロケート中の数

public:
	//スコープスタックアロケータ取得
	inline GASHA_ scopedStackAllocator<stackAllocator<LOCK_TYPE, AUTO_CLEAR>> scopedAllocator(){ GASHA_ scopedStackAllocator<stackAllocator<LOCK_TYPE, AUTO_CLEAR>> allocator(*this); return allocator; }

public:
	//アロケータアダプター取得
	inline GASHA_ allocatorAdapter<stackAllocator<LOCK_TYPE, AUTO_CLEAR>> adapter(){ GASHA_ allocatorAdapter<stackAllocator<LOCK_TYPE, AUTO_CLEAR>> adapter(*this, name(), mode()); return adapter; }

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
	//※【注意】自動クリア時（スマートスタックアロケータ使用時）は、このメソッドの使用禁止
	//　（メモリ解放時のアドレスが不正なアドレスと見なされて、アロケート中の数が正しく更新されなくなり、
	//　　自動クリアが機能しなくなるため）
	//※位置指定版
	inline bool rewind(const size_type pos);
	//※ポインタ指定版
	bool rewind(void* p);

	//メモリクリア
	//※初期状態にする
	//※【注意】メモリ確保状態（アロケート中の数）と無関係に実行するので注意
	inline void clear();
	
	//デバッグ情報作成
	//※十分なサイズのバッファを渡す必要あり。
	//※使用したバッファのサイズを返す。
	//※作成中、ロックを取得する。
	std::size_t debugInfo(char* message, const std::size_t max_size) const;

	//使用中のサイズと数を取得
	//※スコープスタックアロケータで使用されるメソッド
	void getSizeAndCount(size_type& size, size_type& count);

	//使用中のサイズと数をリセット
	//※スコープスタックアロケータで使用されるメソッド
	//※【注意】現在のサイズと数より小さい数でなければならない
	bool resetSizeAndCount(const size_type size, const size_type count);

private:
	//メモリ解放（共通処理）
	//※ロック取得は呼び出し元で行う
	bool _free(void* p);

	//ポインタが範囲内か判定
	inline bool isInUsingRange(void* p);

public:
	//コンストラクタ
	inline stackAllocator(void* buff, const std::size_t max_size);
	template<typename T>
	inline stackAllocator(T* buff, const std::size_t num);
	template<typename T, std::size_t N>
	inline stackAllocator(T (&buff)[N]);
	//デストラクタ
	inline ~stackAllocator();

private:
	//フィールド
	char* m_buffRef;//バッファの参照
	const size_type m_maxSize;//バッファの全体サイズ
	size_type m_size;//バッファの使用中サイズ
	size_type m_count;//アロケート中の数
	mutable lock_type m_lock;//ロックオブジェクト
};

//--------------------------------------------------------------------------------
//バッファ付きスタックアロケータクラス
template<std::size_t _MAX_SIZE, class LOCK_TYPE = GASHA_ dummyLock, class AUTO_CLEAR = dummyStackAllocatorAutoClear>
class stackAllocator_withBuff : public stackAllocator<LOCK_TYPE, AUTO_CLEAR>
{
	//定数
	static const std::size_t MAX_SIZE = _MAX_SIZE;//バッファの全体サイズ
public:
	//コンストラクタ
	inline stackAllocator_withBuff();
	//デストラクタ
	inline ~stackAllocator_withBuff();
private:
	char m_buff[MAX_SIZE];//バッファ
};
//----------------------------------------
//※バッファを基本型とその個数で指定
//※アラインメント分余計にバッファを確保するので注意
template<typename T, std::size_t _NUM, class LOCK_TYPE = GASHA_ dummyLock, class AUTO_CLEAR = dummyStackAllocatorAutoClear>
class stackAllocator_withType : public stackAllocator<LOCK_TYPE, AUTO_CLEAR>
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
	inline stackAllocator_withType();
	//デストラクタ
	inline ~stackAllocator_withType();
private:
	GASHA_ALIGNAS_OF(value_type) char m_buff[MAX_SIZE];//バッファ
};

//----------------------------------------
//スタックアロケータ別名定義：スマートスタックアロケータ
//※明示的にクリアしなくても、参照がなくなった時に自動的にクリアする。

//※スタック用のバッファをコンストラクタで受け渡して使用
template<class LOCK_TYPE = GASHA_ dummyLock>
using smartStackAllocator = stackAllocator<LOCK_TYPE, stackAllocatorAutoClear>;

//※バッファ付き
template<std::size_t _MAX_SIZE, class LOCK_TYPE = GASHA_ dummyLock>
using smartStackAllocator_withBuff = stackAllocator_withBuff<_MAX_SIZE, LOCK_TYPE, stackAllocatorAutoClear>;

//※バッファ付き（基本型とその個数で指定）
template<typename T, std::size_t _SIZE, class LOCK_TYPE = GASHA_ dummyLock>
using smartStackAllocator_withType = stackAllocator_withType<T, _SIZE, LOCK_TYPE, stackAllocatorAutoClear>;

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/stack_allocator.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_STACK_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H
#include <gasha/stack_allocator.cpp.h>
#endif//GASHA_STACK_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_STACK_ALLOCATOR_H

// End of file

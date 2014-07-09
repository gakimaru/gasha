#pragma once
#ifndef GASHA_INCLUDED_LF_DUAL_STACK_ALLOCATOR_H
#define GASHA_INCLUDED_LF_DUAL_STACK_ALLOCATOR_H

//--------------------------------------------------------------------------------
// lf_dual_stack_allocator.h
// ロックフリー双方向スタックアロケータ【宣言部】
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
class lfDualStackAllocator;

//--------------------------------------------------------------------------------
//ロックフリー双方向スタックアロケータ補助クラス

//----------------------------------------
//ロックフリー双方向スタック自動クリア
class lfDualStackAllocatorAutoClear
{
public:
	//正順方向の自動クリア
	template<class AUTO_CLEAR>
	void autoClearAsc(lfDualStackAllocator<AUTO_CLEAR>& allocator);
	//逆順方向の自動クリア
	template<class AUTO_CLEAR>
	void autoClearDesc(lfDualStackAllocator<AUTO_CLEAR>& allocator);
};

//----------------------------------------
//ロックフリー双方向スタック自動クリア（ダミー）
//※何もしない
class dummyLfDualStackAllocatorAutoClear
{
public:
	//正順方向の自動クリア
	template<class AUTO_CLEAR>
	inline void autoClearAsc(lfDualStackAllocator<AUTO_CLEAR>& allocator);
	//逆順方向の自動クリア
	template<class AUTO_CLEAR>
	inline void autoClearDesc(lfDualStackAllocator<AUTO_CLEAR>& allocator);
};

//--------------------------------------------------------------------------------
//ロックフリー双方向スタックアロケータクラス
//※ロックフリー双方向スタック用のバッファをコンストラクタで受け渡して使用
//※free()を呼んでも、明示的に clear() または rewind() しない限り、バッファが解放されないので注意。
template<class AUTO_CLEAR = dummyLfDualStackAllocatorAutoClear>
class lfDualStackAllocator
{
	friend lfDualStackAllocatorAutoClear;//ロックフリー双方向スタック自動クリア
public:
	//型
	typedef AUTO_CLEAR auto_clear_type;//ロックフリー双方向スタック自動クリア型
	typedef std::uint32_t size_type;//サイズ型
	typedef std::uint64_t size2_type;//二種サイズ混成型

public:
	//アクセッサ
	inline size_type maxSize() const { return m_maxSize; }//バッファの全体サイズ（バイト数）
	inline size2_type size2(const size_type size_asc, const size_type size_desc) const;//使用中のサイズ（バイト数） ※正順サイズと逆順サイズから取得
	inline size_type sizeAsc(const size2_type size) const;//正順で使用中のサイズ（バイト数） ※サイズから取得
	inline size_type sizeDesc(const size2_type size) const;//逆順で使用中のサイズ（バイト数） ※サイズから取得
	inline size_type size() const;//使用中のサイズ（バイト数）
	inline size_type sizeAsc() const;//正順で使用中のサイズ（バイト数）
	inline size_type sizeDesc() const;//逆順で使用中のサイズ（バイト数）
	inline size_type remain() const;//残りサイズ（バイト数）
	inline size_type allocatedCountAsc() const { return m_allocatedCountAsc.load(); }//正順でアロケート中の数
	inline size_type allocatedCountDesc() const { return m_allocatedCountDesc.load(); }//逆順でアロケート中の数
	inline size_type allocatedCount() const { return m_allocatedCountAsc.load() + m_allocatedCountDesc.load(); }//アロケート中の数
	inline allocateOrder_t allocateOrder() const { return m_allocateOrder.load(); }//現在のアロケート方向
	inline void setAllocateOrder(const allocateOrder_t order){ m_allocateOrder.store(order); }//現在のアロケート方向を変更
	inline void reversewAllocateOrder(){ m_allocateOrder.store(m_allocateOrder.load() == ALLOC_ASC ? ALLOC_DESC : ALLOC_ASC); }//現在のアロケート方向を逆にする

public:
	//メソッド
	//メモリ確保
	inline void* alloc(const std::size_t size, const std::size_t align = GASHA_ DEFAULT_ALIGN);
	//※アロケート方向指定版
	inline  void* allocOrdinal(const allocateOrder_t order, const std::size_t size, const std::size_t align = GASHA_ DEFAULT_ALIGN);

	//メモリ解放
	//※実際にはメモリを解放しない（できない）ので注意。
	//※アロケート中の数を減らすだけ。
	//※アロケート中の数を見て、正順か逆順のバッファをクリアしても良いかどうかの判断に用いる。
	inline bool free(void* p);

	//メモリ確保とコンストラクタ呼び出し
	template<typename T, typename...Tx>
	inline T* newObj(Tx&&... args);
	//※アロケート方向指定版
	template<typename T, typename...Tx>
	T* newObjOrdinal(const allocateOrder_t order, Tx&&... args);
	//※配列用
	template<typename T, typename...Tx>
	inline T* newArray(const std::size_t num, Tx&&... args);
	//※配列用アロケート方向指定版
	template<typename T, typename...Tx>
	T* newArrayOrdinal(const allocateOrder_t order, const std::size_t num, Tx&&... args);

	//メモリ解放とデストラクタ呼び出し
	template<typename T>
	bool deleteObj(T* p);
	//※配列用（要素数の指定が必要な点に注意）
	template<typename T>
	bool deleteArray(T* p, const std::size_t num);

	//使用中のサイズを指定位置に戻す
	//※【注意】メモリ確保状態（アロケート中の数）と無関係に実行するので注意
	//※【注意】自動クリア（スマートロックフリー双方向スタック）使用時には使用禁止
	//　（メモリ解放時のアドレスが不正なアドレスと見なされて、アロケート中の数が正しく更新されなくなり、
	//　　自動クリアが機能しなくなるため）
	//※位置指定版
	inline bool rewind(const size_type pos);
	//※位置指定とアロケート方向指定版
	inline bool rewindOrdinal(const allocateOrder_t order, const size_type pos);
	//※ポインタ指定版
	inline bool rewind(void* p);

	//メモリクリア
	//※メモリ確保状態（アロケート中の数）と無関係に実行するので注意
	//※全て初期状態にする
	inline void clearAll();
	//※現在のアロケート方向のみ
	inline void clear();
	//※アロケート方向指定
	inline void clearOrdinal(const allocateOrder_t order);
	
	//デバッグ情報作成
	//※十分なサイズのバッファを渡す必要あり。
	//※使用したバッファのサイズを返す。
	//※作成中、ロックを取得する。
	std::size_t debugInfo(char* message);

private:
	//正順メモリ確保（共通処理）
	void* _allocAsc(const std::size_t size, const std::size_t align);
	//逆順メモリ確保（共通処理）
	void* _allocDesc(const std::size_t size, const std::size_t align);
	
	//正順メモリ解放（共通処理）
	//※ロック取得は呼び出し元で行う
	bool _freeAsc(void* p);
	//逆順メモリ解放（共通処理）
	//※ロック取得は呼び出し元で行う
	bool _freeDesc(void* p);

	//正順に使用中のサイズを指定位置に戻す（共通処理）
	bool _rewindAsc(void* p);
	//逆順に使用中のサイズを指定位置に戻す（共通処理）
	bool _rewindDesc(void* p);

	//メモリクリア（共通処理）
	//※ロック取得は呼び出し元で行う
	void _clearAsc();
	void _clearDesc();

	//ポインタが範囲内か判定
	//※範囲に含む方向を返す。どこにも含まれない場合は ALLOC_UNKNOWN_ORDER を返す。
	inline allocateOrder_t isInUsingRange(void* p);

public:
	//コンストラクタ
	inline lfDualStackAllocator(void* buff, const std::size_t max_size);
	template<typename T>
	inline lfDualStackAllocator(T* buff, const std::size_t num);
	template<typename T, std::size_t N>
	inline lfDualStackAllocator(T (&buff)[N]);
	//デストラクタ
	inline ~lfDualStackAllocator();

private:
	//フィールド
	char* m_buffRef;//バッファの参照
	const size_type m_maxSize;//バッファの全体サイズ
	std::atomic<size2_type> m_size;//バッファの使用中サイズ（正順＋逆準）
	std::atomic<size_type> m_allocatedCountAsc;//アロケート中の数(正順)
	std::atomic<size_type> m_allocatedCountDesc;//アロケート中の数(逆順)
	std::atomic<allocateOrder_t> m_allocateOrder;//現在のアロケート方向
};

//--------------------------------------------------------------------------------
//バッファ付きロックフリー双方向スタックアロケータクラス
template<std::size_t _MAX_SIZE, class AUTO_CLEAR = dummyLfDualStackAllocatorAutoClear>
class lfDualStackAllocator_withBuff : public lfDualStackAllocator<AUTO_CLEAR>
{
	//定数
	static const std::size_t MAX_SIZE = _MAX_SIZE;//バッファの全体サイズ
public:
	//コンストラクタ
	inline lfDualStackAllocator_withBuff();
	//デストラクタ
	inline ~lfDualStackAllocator_withBuff();
private:
	char m_buff[MAX_SIZE];//バッファ
};
//----------------------------------------
//※バッファを基本型とその個数で指定
//※アラインメント分余計にバッファを確保するので注意
template<typename T, std::size_t _NUM, class AUTO_CLEAR = dummyLfDualStackAllocatorAutoClear>
class lfDualStackAllocator_withType : public lfDualStackAllocator<AUTO_CLEAR>
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
	template<typename... Tx>
	inline value_type* newDefaultOrdinal(const allocateOrder_t order, Tx&&... args);
	
	//デフォルト型のメモリ解放とデストラクタ呼び出し
	inline bool deleteDefault(value_type*& p);
public:
	//コンストラクタ
	inline lfDualStackAllocator_withType();
	//デストラクタ
	inline ~lfDualStackAllocator_withType();
private:
	GASHA_ALIGNAS_OF(value_type) char m_buff[MAX_SIZE];//バッファ
};

//----------------------------------------
//ロックフリー双方向スタックアロケータ別名定義：スマートロックフリー双方向スタックアロケータ
//※明示的にクリアしなくても、参照がなくなった時に自動的にクリアする。

//※ロックフリー双方向スタック用のバッファをコンストラクタで受け渡して使用
template<class LOCK_TYPE = GASHA_ dummyLock>
using lfSmartDualStackAllocator = lfDualStackAllocator<lfDualStackAllocatorAutoClear>;

//※バッファ付き
template<std::size_t _MAX_SIZE, class LOCK_TYPE = GASHA_ dummyLock>
using lfSmartDualStackAllocator_withBuff = lfDualStackAllocator_withBuff<_MAX_SIZE, lfDualStackAllocatorAutoClear>;

//※バッファ付き（基本型とその個数で指定）
template<typename T, std::size_t _SIZE, class LOCK_TYPE = GASHA_ dummyLock>
using lfSmartDualStackAllocator_withType = lfDualStackAllocator_withType<T, _SIZE, lfDualStackAllocatorAutoClear>;

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/lf_dual_stack_allocator.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_LF_DUAL_STACK_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H
#include <gasha/lf_dual_stack_allocator.cpp.h>
#endif//GASHA_LF_DUAL_STACK_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_LF_DUAL_STACK_ALLOCATOR_H

// End of file

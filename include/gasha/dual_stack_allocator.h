#pragma once
#ifndef GASHA_INCLUDED_DUAL_STACK_ALLOCATOR_H
#define GASHA_INCLUDED_DUAL_STACK_ALLOCATOR_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// dual_stack_allocator.h
// 双方向スタックアロケータ【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/allocator_common.h>//メモリアロケータ共通設定
#include <gasha/memory.h>//メモリ操作：adjustStaticAlign, adjustAlign()
#include <gasha/scoped_stack_allocator.h>//スコープスタックアロケータ
#include <gasha/scoped_dual_stack_allocator.h>//双方向スコープスタックアロケータ
#include <gasha/allocator_adapter.h>//アロケータアダプター
#include <gasha/dummy_lock.h>//ダミーロック

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//双方向スタックアロケータ補助クラス

//クラス宣言
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
class dualStackAllocator;

//----------------------------------------
//双方向スタック自動クリア
class dualStackAllocatorAutoClear
{
public:
	//名前
	static const char* name(){ return "AutoClear"; }
	//正順方向の自動クリア
	template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
	inline void autoClearAsc(dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>& allocator);
	//逆順方向の自動クリア
	template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
	inline void autoClearDesc(dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>& allocator);
};

//----------------------------------------
//双方向スタック自動クリア（ダミー）
//※何もしない
class dummyDualStackAllocatorAutoClear
{
public:
	//名前
	static const char* name(){ return "ManualClear"; }
	//正順方向の自動クリア
	template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
	inline void autoClearAsc(dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>& allocator);
	//逆順方向の自動クリア
	template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
	inline void autoClearDesc(dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>& allocator);
};

//--------------------------------------------------------------------------------
//双方向スタックアロケータクラス
//※双方向スタック用のバッファをコンストラクタで受け渡して使用
//※free()を呼んでも、明示的に clear() または rewind() しない限り、バッファが解放されないので注意。
template<class LOCK_POLICY = GASHA_ dummyLock, class AUTO_CLEAR_POLICY = dummyDualStackAllocatorAutoClear>
class dualStackAllocator
{
	friend dualStackAllocatorAutoClear;//双方向スタック自動クリア
public:
	//型
	typedef LOCK_POLICY lock_type;//ロック型
	typedef AUTO_CLEAR_POLICY auto_clear_type;//双方向スタック自動クリア型
	typedef std::uint32_t size_type;//サイズ型

public:
	//アクセッサ
	const char* name() const { return "dualStackAllocator"; }//アロケータ名
	const char* mode() const { return auto_clear_type::name(); }//実装モード名
	inline const void* buff() const { return reinterpret_cast<const void*>(m_buffRef); }//バッファの先頭アドレス
	inline size_type maxSize() const { return m_maxSize; }//バッファの全体サイズ（バイト数）
	inline size_type size() const { return m_sizeAsc + m_sizeDesc; }//使用中のサイズ（バイト数）
	inline size_type sizeAsc() const { return m_sizeAsc; }//正順で使用中のサイズ（バイト数）
	inline size_type sizeDesc() const { return m_sizeDesc; }//逆順で使用中のサイズ（バイト数）
	inline size_type remain() const { return m_maxSize - size(); }//残りサイズ（バイト数）
	inline size_type count() const { return m_countAsc + m_countDesc; }//アロケート中の数
	inline size_type countAsc() const { return m_countAsc; }//正順でアロケート中の数
	inline size_type countDesc() const { return m_countDesc; }//逆順でアロケート中の数
	inline allocationOrder_t allocationOrder() const { return m_allocateOrder; }//現在のアロケート方向
	inline void setAllocateOrder(const allocationOrder_t order){ m_allocateOrder = order; }//現在のアロケート方向を変更
	inline void reversewAllocateOrder(){ m_allocateOrder = m_allocateOrder == ALLOC_ASC ? ALLOC_DESC : ALLOC_ASC; }//現在のアロケート方向を逆にする

public:
	//スコープスタックアロケータ取得
	inline GASHA_ scopedStackAllocator<dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>> scopedAllocator(){ GASHA_ scopedStackAllocator<dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>> allocator(*this); return allocator; }
	inline GASHA_ scopedDualStackAllocator<dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>> scopedDualAllocator(){ GASHA_ scopedDualStackAllocator<dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>> allocator(*this); return allocator; }

public:
	//アロケータアダプター取得
	inline GASHA_ allocatorAdapter<dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>> adapter(){ GASHA_ allocatorAdapter<dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>> adapter(*this, name(), mode()); return adapter; }

public:
	//メソッド

	//メモリ確保
	inline void* alloc(const std::size_t size, const std::size_t align = GASHA_ DEFAULT_ALIGN);
	//※アロケート方向指定版
	inline  void* allocOrd(const allocationOrder_t order, const std::size_t size, const std::size_t align = GASHA_ DEFAULT_ALIGN);

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
	T* newObjOrd(const allocationOrder_t order, Tx&&... args);
	//※配列用
	template<typename T, typename...Tx>
	inline T* newArray(const std::size_t num, Tx&&... args);
	//※配列用アロケート方向指定版
	template<typename T, typename...Tx>
	T* newArrayOrd(const allocationOrder_t order, const std::size_t num, Tx&&... args);

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
	//※位置指定とアロケート方向指定版
	inline bool rewindOrd(const allocationOrder_t order, const size_type pos);
	//※ポインタ指定版
	inline bool rewind(void* p);

	//メモリクリア
	//※初期状態にする
	//※【注意】メモリ確保状態（アロケート中の数）と無関係に実行するので注意
	inline void clearAll();
	//※現在のアロケート方向のみ
	inline void clear();
	//※アロケート方向指定
	inline void clearOrd(const allocationOrder_t order);
	
	//デバッグ情報作成
	//※十分なサイズのバッファを渡す必要あり。
	//※使用したバッファのサイズを返す。
	//※作成中、ロックを取得する。
	std::size_t debugInfo(char* message, const std::size_t max_size) const;

	//使用中のサイズと数を取得
	//※スコープスタックアロケータで使用されるメソッド
	void getSizeAndCount(size_type& size, size_type& count);
	void getSizeAndCount(allocationOrder_t& order, size_type& size_asc, size_type& size_desc, size_type& count_asc, size_type& count_desc);

	//使用中のサイズと数をリセット
	//※スコープスタックアロケータで使用されるメソッド
	//※【注意】現在のサイズと数より小さい数でなければならない
	inline bool resetSizeAndCount(const size_type size, const size_type count);
	bool resetSizeAndCount(const allocationOrder_t order, const size_type size_asc, const size_type size_desc, const size_type count_asc, const size_type count_desc);
	bool resetSizeAndCount(const allocationOrder_t order, const size_type size, const size_type count);

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
	inline void _clearAsc();
	inline void _clearDesc();

	//ポインタが範囲内か判定
	//※範囲に含む方向を返す。どこにも含まれない場合は ALLOC_UNKNOWN_ORDER を返す。
	inline allocationOrder_t isInUsingRange(void* p);

public:
	//コンストラクタ
	inline dualStackAllocator(void* buff, const std::size_t max_size);
	template<typename T>
	inline dualStackAllocator(T* buff, const std::size_t num);
	template<typename T, std::size_t N>
	inline dualStackAllocator(T (&buff)[N]);
	//デストラクタ
	inline ~dualStackAllocator();

private:
	//フィールド
	char* m_buffRef;//バッファの参照
	const size_type m_maxSize;//バッファの全体サイズ
	size_type m_sizeAsc;//バッファの使用中サイズ（正順）
	size_type m_sizeDesc;//バッファの使用中サイズ（逆順）
	size_type m_countAsc;//アロケート中の数(正順)
	size_type m_countDesc;//アロケート中の数(逆順)
	allocationOrder_t m_allocateOrder;//現在のアロケート方向
	mutable lock_type m_lock;//ロックオブジェクト
};

//--------------------------------------------------------------------------------
//バッファ付き双方向スタックアロケータクラス
template<std::size_t _MAX_SIZE, class LOCK_POLICY = GASHA_ dummyLock, class AUTO_CLEAR_POLICY = dummyDualStackAllocatorAutoClear>
class dualStackAllocator_withBuff : public dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>
{
	//定数
	static const std::size_t MAX_SIZE = _MAX_SIZE;//バッファの全体サイズ
public:
	//コンストラクタ
	inline dualStackAllocator_withBuff();
	//デストラクタ
	inline ~dualStackAllocator_withBuff();
private:
	char m_buff[MAX_SIZE];//バッファ
};
//----------------------------------------
//※バッファを基本型とその個数で指定
//※アラインメント分余計にバッファを確保するので注意
template<typename T, std::size_t _NUM, class LOCK_POLICY = GASHA_ dummyLock, class AUTO_CLEAR_POLICY = dummyDualStackAllocatorAutoClear>
class dualStackAllocator_withType : public dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>
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
	inline value_type* newDefaultOrd(const allocationOrder_t order, Tx&&... args);
	
	//デフォルト型のメモリ解放とデストラクタ呼び出し
	inline bool deleteDefault(value_type*& p);
public:
	//コンストラクタ
	inline dualStackAllocator_withType();
	//デストラクタ
	inline ~dualStackAllocator_withType();
private:
	GASHA_ALIGNAS_OF(value_type) char m_buff[MAX_SIZE];//バッファ
};

//----------------------------------------
//双方向スタックアロケータ別名定義：スマート双方向スタックアロケータ
//※明示的にクリアしなくても、参照がなくなった時に自動的にクリアする。

//※双方向スタック用のバッファをコンストラクタで受け渡して使用
template<class LOCK_POLICY = GASHA_ dummyLock>
using smartDualStackAllocator = dualStackAllocator<LOCK_POLICY, dualStackAllocatorAutoClear>;

//※バッファ付き
template<std::size_t _MAX_SIZE, class LOCK_POLICY = GASHA_ dummyLock>
using smartDualStackAllocator_withBuff = dualStackAllocator_withBuff<_MAX_SIZE, LOCK_POLICY, dualStackAllocatorAutoClear>;

//※バッファ付き（基本型とその個数で指定）
template<typename T, std::size_t _SIZE, class LOCK_POLICY = GASHA_ dummyLock>
using smartDualStackAllocator_withType = dualStackAllocator_withType<T, _SIZE, LOCK_POLICY, dualStackAllocatorAutoClear>;

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/dual_stack_allocator.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_DUAL_STACK_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H
#include <gasha/dual_stack_allocator.cpp.h>
#endif//GASHA_DUAL_STACK_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_DUAL_STACK_ALLOCATOR_H

// End of file

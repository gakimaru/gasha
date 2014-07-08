#pragma once
#ifndef GASHA_INCLUDED_STACK_ALLOCATOR_H
#define GASHA_INCLUDED_STACK_ALLOCATOR_H

//--------------------------------------------------------------------------------
//stack_allocator.h
// スタックアロケータ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/allocator_common.h>//メモリアロケータ共通設定
#include <gasha/memory.h>//メモリ操作：adjustStaticAlign, adjustAlign()
#include <gasha/dummy_lock.h>//ダミーロック

#include <cstddef>//std::size_t

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <functional> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <functional>//std::function

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//クラス宣言
template<class LOCK_TYPE, class AUTO_CLEAR>
class stackAllocator;

//--------------------------------------------------------------------------------
//スタックアロケータ補助クラス

//----------------------------------------
//スタック自動クリア
class stackAllocatorAutoClear
{
public:
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
	//自動クリア
	template<class LOCK_TYPE, class AUTO_CLEAR>
	inline void autoClear(stackAllocator<LOCK_TYPE, AUTO_CLEAR>& allocator);
};

//--------------------------------------------------------------------------------
//スタックアロケータクラス
template<class LOCK_TYPE = GASHA_ dummyLock, class AUTO_CLEAR = dummyStackAllocatorAutoClear>
class stackAllocator
{
	friend stackAllocatorAutoClear;//スタック自動クリア
public:
	//型
	typedef LOCK_TYPE lock_type;//ロック型
	typedef AUTO_CLEAR auto_clear_type;//スタック自動クリア型

public:
	//アクセッサ
	inline std::size_t maxSize() const { return m_maxSize; }//バッファの全体サイズ（バイト数）
	inline std::size_t size() const { return  m_size; }//使用中のサイズ（バイト数）
	inline std::size_t remain() const { return m_maxSize - size(); }//残りサイズ（バイト数）
	inline std::size_t allocatedCount() const { return m_allocatedCount; }//アロケート中の数
	inline const void* prevPtr() const { return  reinterpret_cast<void*>(reinterpret_cast<char*>(m_buffRef)+m_prevSize); }//直近のアロケート時のポインタ
	inline void* prevPtr(){ return  reinterpret_cast<void*>(reinterpret_cast<char*>(m_buffRef)+m_prevSize); }//直近のアロケート時のポインタ
	inline std::size_t prevSize() const { return  m_prevSize; }//直近のアロケート前のサイズ

public:
	//メソッド
	//メモリ確保
	void* alloc(const std::size_t size, const std::size_t align = GASHA_ DEFAULT_ALIGN);

	//メモリ解放
	bool free(void* p);

	//メモリを以前の位置に戻す
	//※マーカー指定版
	void back(const std::size_t pos);
	//※ポインタ指定版
	void back(const void* p);

	//メモリクリア
	//※初期状態にする
	void clear();

	//メモリ確保とコンストラクタ呼び出し
	template<typename T, typename...Tx>
	T* newObj(Tx&&... args);
	//※配列用
	template<typename T, typename...Tx>
	T* newArray(const std::size_t num, Tx&&... args);

	//メモリ解放とデストラクタ呼び出し
	template<typename T>
	bool deleteObj(T*& p);
	//※配列用（要素数の指定が必要な点に注意）
	template<typename T>
	bool deleteArray(T*& p, const std::size_t num);

	//デバッグ情報作成
	//※十分なサイズのバッファを渡す必要あり。
	//※使用したバッファのサイズを返す。
	//※作成中、ロックを取得する。
	template<typename T, class FUNC = std::function<std::size_t(char* messdage, const T& value)>>
	std::size_t debugInfo(char* message, FUNC print_node);
public:
	//コンストラクタ
	inline stackAllocator(const char* buff, const std::size_t max_size, const std::size_t bock_size, const std::size_t block_align = GASHA_ DEFAULT_ALIGN);
	template<typename T>
	inline stackAllocator(const T* buff, const std::size_t max_size);
	template<typename T, std::size_t N>
	inline stackAllocator(const T (&buff)[N]);
	//デストラクタ
	inline ~stackAllocator();

private:
	//フィールド
	char* m_buffRef;//プールバッファの参照
	const std::size_t m_maxSize;//プールバッファの全体サイズ
	const std::size_t m_size;//プールバッファの使用中サイズ
	const std::size_t m_prevSize;//直近のアロケート前のサイズ（直近のアロケート時のポインタを指す）
	std::size_t m_allocatedCount;//アロケート中の数
	lock_type m_lock;//ロックオブジェクト
};

//--------------------------------------------------------------------------------
//バッファ付きスタックアロケータクラス
//※アラインメント分余計にバッファを確保するため、場合によっては指定の _POOL_SIZE よりも多くなることがある。
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
	char m_buff[MAX_SIZE];//プールバッファ
};
//----------------------------------------
//※バッファを基本型とその個数で指定
template<typename T, std::size_t _SIZE, class LOCK_TYPE = GASHA_ dummyLock, class AUTO_CLEAR = dummyStackAllocatorAutoClear>
class stackAllocator_withType : public stackAllocator<LOCK_TYPE, AUTO_CLEAR>
{
public:
	//型
	typedef T value_type;//値の型
public:
	//定数
	static const std::size_t TYPE_ALIGN = alignof(value_type);//基本型のアラインメント
	static const std::size_t TYPE_SIZE = sizeof(value_type);//基本型のサイズ
	static const std::size_t SIZE = _SIZE;//基本型の確保可能数
	static const std::size_t MAX_SIZE = TYPE_SIZE * SIZE + TYPE_SIZE;//バッファの全体サイズ ※アラインメント分余計に確保する
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
	GASHA_ALIGNAS_OF(value_type) char m_buff[MAX_SIZE];//プールバッファ
};

//----------------------------------------
//スタックアロケータ別名

//※自動クリア版
template<class LOCK_TYPE = GASHA_ dummyLock >
using smartStackAllocator = stackAllocator<LOCK_TYPE, stackAllocatorAutoClear>;
//※バッファ付き＋自動クリア版
template<std::size_t _MAX_SIZE, class LOCK_TYPE = GASHA_ dummyLock >
using smartStackAllocator_withBuff = stackAllocator_withBuff<_MAX_SIZE, LOCK_TYPE, stackAllocatorAutoClear>;
//※バッファ付き（基本型とその個数で指定）＋自動クリア版
template<typename T, std::size_t _SIZE, class LOCK_TYPE = GASHA_ dummyLock >
using smartStackAllocator_withType = stackAllocator_withType<T, _SIZE, LOCK_TYPE, stackAllocatorAutoClear>;

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/stack_allocator.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_STACK_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H
#include <gasha/stack_allocator.cpp.h>
#endif//GASHA_STACK_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_STACK_ALLOCATOR_H

// End of file

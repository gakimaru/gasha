﻿#pragma once
#ifndef GASHA_INCLUDED_POOL_ALLOCATOR_H
#define GASHA_INCLUDED_POOL_ALLOCATOR_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// pool_allocator.h
// プールアロケータ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/allocator_common.h>//メモリアロケータ共通設定
#include <gasha/basic_math.h>//基本算術：adjustStaticAlign
#include <gasha/dummy_lock.h>//ダミーロック

#include <cstddef>//std::size_t

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <functional> <bitset> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <functional>//std::function
#include <bitset>//std::bitset

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//プールアロケータクラス
//※プールバッファをコンストラクタで受け渡して使用
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE = GASHA_ dummyLock>
class poolAllocator
{
public:
	//型
	typedef LOCK_TYPE lock_type;//ロック型

	//再利用プール型
	struct recycable_t
	{
		std::size_t m_next_index;//再利用プール連結インデックス
	};

public:
	//定数
	static const std::size_t MAX_POOL_SIZE = _MAX_POOL_SIZE;//最大プール数
	                                                        //※実際のプール数はコンストラクタで渡されたバッファサイズに基づく。
	                                                        //※最大プール数は、それと同じかそれ以上の値を指定する必要がある。
	static const std::size_t INVALID_INDEX = 0xffffffff;//無効なインデックス
	static const std::size_t DIRTY_INDEX = 0xfefefefe;//再利用プール連結インデックス削除用

public:
	//アクセッサ
	inline std::size_t offset() const { return m_offset; }//プールバッファのオフセット（アラインメント調整用）
	inline std::size_t maxSize() const { return m_maxSize; }//プールバッファの全体サイズ（バイト数）
	inline std::size_t blockSize() const { return m_blockSize; }//ブロックサイズ
	inline std::size_t blockAlign() const { return m_blockAlign; }//ブロックのアライメント
	inline std::size_t poolSize() const { return m_poolSize; }//プール数
	inline std::size_t usingPoolSize() const { return m_usingCount; }//使用中のプール数
	inline std::size_t size() const { return  m_usingCount * m_blockSize; }//使用中のサイズ（バイト数）
	inline std::size_t remain() const { return m_maxSize - size(); }//残りサイズ（プール数）

public:
	//メソッド

	//メモリ確保
	void* alloc();

	//メモリ解放
	bool free(void* p);
	
	//メモリ確保とコンストラクタ呼び出し
	template<typename T, typename...Tx>
	T* newObj(Tx&&... args);
	
	//メモリ解放とデストラクタ呼び出し
	template<typename T>
	bool deleteObj(T*& p);

	//デバッグ情報作成
	//※十分なサイズのバッファを渡す必要あり。
	//※使用したバッファのサイズを返す。
	//※作成中、ロックを取得する。
	template<typename T, class FUNC = std::function<std::size_t(char* messdage, const T& value)>>
	std::size_t debugInfo(char* message, FUNC print_node);

private:
	//メモリ解放（共通処理）
	//※ロック取得は呼び出し元で行う
	bool free(void* p, const std::size_t index);
	
	//ポインタをインデックスに変換
	inline std::size_t ptrToIndex(void* p);

	//インデックスに対応するバッファのポインタを取得
	inline const void* refBuff(const std::size_t index) const;
	inline void* refBuff(const std::size_t index);

public:
	//コンストラクタ
	inline poolAllocator(void* buff, const std::size_t max_size, const std::size_t bock_size, const std::size_t block_align = DEFAULT_ALIGN);
	template<typename T>
	inline poolAllocator(T* buff, const std::size_t max_size);
	template<typename T, std::size_t N>
	inline poolAllocator(T (&buff)[N]);
	//デストラクタ
	inline ~poolAllocator();

private:
	//フィールド
	char* m_buffRef;//プールバッファの参照
	const std::size_t m_offset;//プールバッファのオフセット（アラインメント調整用）
	const std::size_t m_maxSize;//プールバッファの全体サイズ
	const std::size_t m_blockSize;//ブロックサイズ
	const std::size_t m_blockAlign;//ブロックのアライメント
	const std::size_t m_poolSize;//プール数
	std::size_t m_vacantHead;//空きプールの先頭インデックス
	std::size_t m_recyclableHead;//再利用プールの先頭インデックス
	std::bitset<MAX_POOL_SIZE> m_using;//使用中インデックス（二重解放判定用）
	std::size_t m_usingCount;//使用中の数（デバッグ用）※制御に必要な情報ではない
	lock_type m_lock;//ロックオブジェクト
};

//--------------------------------------------------------------------------------
//バッファ付きプールアロケータクラス
//※アラインメント分余計にバッファを確保するため、場合によっては指定の _POOL_SIZE よりも多くなることがある。
template<std::size_t _BLOCK_SIZE, std::size_t _POOL_SIZE, std::size_t _BLOCK_ALIGN = DEFAULT_ALIGN, class LOCK_TYPE = GASHA_ dummyLock>
class poolAllocator_withBuff : public poolAllocator<_POOL_SIZE, LOCK_TYPE>
{
	//静的アサーション
	static_assert(isValidStaticAlign<_BLOCK_ALIGN>::value == true, "poolAllocator_withBuff: _BLOCK_ALIGN is invalid. ");
public:
	//定数
	static const std::size_t BLOCK_ALIGN = _BLOCK_ALIGN;//ブロックのアラインメント
	static const std::size_t BLOCK_SIZE = adjustStaticAlign<_BLOCK_SIZE, BLOCK_ALIGN>::value;//ブロックサイズ
	static const std::size_t POOL_SIZE = _POOL_SIZE;//プール数
	static const std::size_t MAX_SIZE = BLOCK_SIZE * POOL_SIZE + BLOCK_ALIGN;//プールバッファの全体サイズ ※アラインメント分余計に確保する
public:
	//コンストラクタ
	inline poolAllocator_withBuff();
	//デストラクタ
	inline ~poolAllocator_withBuff();
private:
	char m_buff[MAX_SIZE];//プールバッファ
};
//※型指定版
template<typename T, std::size_t _POOL_SIZE, class LOCK_TYPE = GASHA_ dummyLock>
class poolAllocator_withType : public poolAllocator<_POOL_SIZE, LOCK_TYPE>
{
public:
	//型
	typedef T block_type;//ブロックの型
public:
	//定数
	static const std::size_t BLOCK_ALIGN = alignof(block_type);//ブロックのアラインメント
	static const std::size_t BLOCK_SIZE = sizeof(block_type);//ブロックサイズ
	static const std::size_t POOL_SIZE = _POOL_SIZE;//プール数
	static const std::size_t MAX_SIZE = BLOCK_SIZE * POOL_SIZE + BLOCK_ALIGN;//プールバッファの全体サイズ ※アラインメント分余計に確保する
public:
	//デフォルト型のメモリ確保とコンストラクタ呼び出し
	template<typename... Tx>
	inline T* newDefault(Tx&&... args);
	
	//デフォルト型のメモリ解放とデストラクタ呼び出し
	inline bool deleteDefault(T*& p);
public:
	//コンストラクタ
	inline poolAllocator_withType();
	//デストラクタ
	inline ~poolAllocator_withType();
private:
	GASHA_ALIGNAS_OF(block_type) char m_buff[MAX_SIZE];//プールバッファ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/pool_allocator.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_POOL_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H
#include <gasha/pool_allocator.cpp.h>
#endif//GASHA_POOL_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_POOL_ALLOCATOR_H

// End of file
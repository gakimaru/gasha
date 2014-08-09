#pragma once
#ifndef GASHA_INCLUDED_POOL_ALLOCATOR_H
#define GASHA_INCLUDED_POOL_ALLOCATOR_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// pool_allocator.h
// プールアロケータ【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/allocator_common.h>//メモリアロケータ共通設定
#include <gasha/memory.h>//メモリ操作：adjustStaticAlign, adjustAlign()
#include <gasha/allocator_adapter.h>//アロケータアダプタ
#include <gasha/dummy_lock.h>//ダミーロック

#include <cstddef>//std::size_t

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <functional>//C++11 std::function
#include <bitset>//std::bitset
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//プールアロケータクラス
//※プールバッファをコンストラクタで受け渡して使用
template<std::size_t _MAX_POOL_SIZE, class LOCK_POLICY = GASHA_ dummyLock>
class poolAllocator
{
public:
	//型
	typedef LOCK_POLICY lock_type;//ロック型
	typedef std::uint32_t index_type;//インデックス型
	typedef std::uint32_t size_type;//サイズ型

	//再利用プール型
	struct recycable_t
	{
		index_type m_next_index;//再利用プール連結インデックス
	};

public:
	//定数
	static const size_type MAX_POOL_SIZE = _MAX_POOL_SIZE;//最大プール数
	                                                      //※実際のプール数はコンストラクタで渡されたバッファサイズに基づく。
	                                                      //※最大プール数は、それと同じかそれ以上の値を指定する必要がある。
	static const index_type INVALID_INDEX = ~static_cast<index_type>(0);//無効なインデックス
	static const index_type DIRTY_INDEX = 0xfefefefe;//再利用プール連結インデックス削除用

public:
	//アクセッサ
	const char* name() const { return "poolAllocator"; }
	const char* mode() const { return "-"; }
	inline const void* buff() const { return reinterpret_cast<const void*>(m_buffRef); }//バッファの先頭アドレス
	inline size_type offset() const { return m_offset; }//プールバッファのオフセット（アラインメント調整用）
	inline size_type maxSize() const { return m_maxSize; }//プールバッファの全体サイズ（バイト数）
	inline size_type blockSize() const { return m_blockSize; }//ブロックサイズ
	inline size_type blockAlign() const { return m_blockAlign; }//ブロックのアライメント
	inline size_type poolSize() const { return m_poolSize; }//プール数（最大）
	inline size_type size() const { return m_usingPoolSize * m_blockSize; }//使用中のサイズ（バイト数）
	inline size_type remain() const { return m_maxSize - size(); }//残りサイズ（バイト数）
	inline size_type usingPoolSize() const { return m_usingPoolSize; }//使用中のプール数
	inline size_type poolRemain() const { return m_poolSize - m_usingPoolSize; }//残りのプール数

public:
	//アロケータアダプタ取得
	inline GASHA_ allocatorAdapter<poolAllocator<_MAX_POOL_SIZE, LOCK_POLICY>> adapter(){ GASHA_ allocatorAdapter<poolAllocator<_MAX_POOL_SIZE, LOCK_POLICY>> adapter(*this, name(), mode()); return adapter; }

public:
	//メソッド

	//メモリ確保
	//※最低限必要なサイズとアラインメントを指定可能。
	//※ブロックサイズを超える場合は確保不可。
	void* alloc(const std::size_t size = 0, const std::size_t align = 0);

	//メモリ解放
	bool free(void* p);
	
	//メモリ確保とコンストラクタ呼び出し
	template<typename T, typename...Tx>
	T* newObj(Tx&&... args);
	//※配列用（一つのプールに収まる配列を扱う点に注意。連続したブロックを確保するのではない。）
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
	template<typename T, class FUNC = std::function<std::size_t(char* messdage, const std::size_t max_size, std::size_t& size, const T& value)>>
	std::size_t debugInfo(char* message, const std::size_t max_size, const bool with_detail, FUNC print_node) const;
	std::size_t debugInfo(char* message, const std::size_t max_size, const bool with_detail) const;
	inline std::size_t debugInfo(char* message, const std::size_t max_size) const;

	//強制クリア
	//※【要注意】強制的に未アロケート状態にする
	inline void clear();

private:
	//メモリ解放（共通処理）
	//※ロック取得は呼び出し元で行う
	bool free(void* p, const index_type index);
	
	//ポインタをインデックスに変換
	inline index_type ptrToIndex(void* p);

	//インデックスに対応するバッファのポインタを取得
	inline const void* refBuff(const index_type index) const;
	inline void* refBuff(const index_type index);

public:
	//コンストラクタ
	inline poolAllocator(void* buff, const std::size_t buff_size, const std::size_t bock_size, const std::size_t block_align = GASHA_ DEFAULT_ALIGN);
	template<typename T>
	inline poolAllocator(T* buff, const std::size_t pool_size);
	template<typename T, std::size_t N>
	inline poolAllocator(T (&buff)[N]);
	//デストラクタ
	inline ~poolAllocator();

private:
	//フィールド
	char* m_buffRef;//プールバッファの参照
	const size_type m_offset;//プールバッファのオフセット（アラインメント調整用）
	const size_type m_maxSize;//プールバッファの全体サイズ
	const size_type m_blockSize;//ブロックサイズ
	const size_type m_blockAlign;//ブロックのアライメント
	const size_type m_poolSize;//プール数
	index_type m_vacantHead;//空きプールの先頭インデックス
	index_type m_recyclableHead;//再利用プールの先頭インデックス
	std::bitset<MAX_POOL_SIZE> m_using;//使用中インデックス（二重解放判定用）
	size_type m_usingPoolSize;//使用中の数（デバッグ用）※制御に必要な情報ではない
	mutable lock_type m_lock;//ロックオブジェクト
};

//--------------------------------------------------------------------------------
//バッファ付きプールアロケータクラス
//※アラインメント分余計にバッファを確保するので注意
template<std::size_t _BLOCK_SIZE, std::size_t _POOL_SIZE, std::size_t _BLOCK_ALIGN = GASHA_ DEFAULT_ALIGN, class LOCK_POLICY = GASHA_ dummyLock>
class poolAllocator_withBuff : public poolAllocator<_POOL_SIZE, LOCK_POLICY>
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
//----------------------------------------
//※バッファを型で指定
//※アラインメント分余計にバッファを確保するので注意
template<typename T, std::size_t _POOL_SIZE, class LOCK_POLICY = GASHA_ dummyLock>
class poolAllocator_withType : public poolAllocator<_POOL_SIZE, LOCK_POLICY>
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
	inline block_type* newDefault(Tx&&... args);
	
	//デフォルト型のメモリ解放とデストラクタ呼び出し
	inline bool deleteDefault(block_type*& p);
public:
	//コンストラクタ
	inline poolAllocator_withType();
	//デストラクタ
	inline ~poolAllocator_withType();
private:
	GASHA_ALIGNAS_OF(block_type) char m_buff[MAX_SIZE];//プールバッファ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/pool_allocator.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_POOL_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H
#include <gasha/pool_allocator.cpp.h>
#endif//GASHA_POOL_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_POOL_ALLOCATOR_H

// End of file

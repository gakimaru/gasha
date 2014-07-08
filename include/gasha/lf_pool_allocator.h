#pragma once
#ifndef GASHA_INCLUDED_LOCKFREE_POOL_ALLOCATOR_H
#define GASHA_INCLUDED_LOCKFREE_POOL_ALLOCATOR_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_pool_allocator.h
// ロックフリープールアロケータ【宣言部】
//
// ※クラスをインスタンス化する際は、別途 .cpp.h ファイルをインクルードする必要あり。
// ※明示的なインスタンス化を避けたい場合は、ヘッダーファイルと共にインクルード。
// 　（この場合、実際に使用するメンバー関数しかインスタンス化されないので、対象クラスに不要なインターフェースを実装しなくても良い）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/allocator_common.h>//メモリアロケータ共通設定
#include <gasha/memory.h>//メモリ操作：adjustStaticAlign, adjustAlign()

#include <cstddef>//std::size_t
#include <cstdint>//++11 std::uint32_t
#include <atomic>//C++11 std::atomic

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <functional> <bitset> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <functional>//std::function

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリープールアロケータクラス
//※プールバッファをコンストラクタで受け渡して使用
//※ABA問題対策あり（プールの再利用インデックスに8ビットのタグを付けて管理）
//※プール可能な要素数の最大は2^(32-8)-2 = 16,777,214個（2^(32-8)-1だと、値がINVALID_INDEXになる可能性があるのでNG）
template<std::size_t _MAX_POOL_SIZE>
class lfPoolAllocator
{
	static_assert(_MAX_POOL_SIZE < 0x00ffffff, "POOL is too large.");

public:
	//型
	typedef std::uint32_t index_type;//インデックス型
	typedef std::uint32_t size_type;//サイズ型

	//再利用プール型
	struct recycable_t
	{
		std::atomic<index_type> m_next_index;//再利用プール連結インデックス
	};

public:
	//定数
	static const size_type MAX_POOL_SIZE = static_cast<size_type>(_MAX_POOL_SIZE);//最大プール数
	                                                        //※実際のプール数はコンストラクタで渡されたバッファサイズに基づく。
	                                                        //※最大プール数は、それと同じかそれ以上の値を指定する必要がある。
	static const index_type INVALID_INDEX = 0xffffffff;//無効なインデックス
	static const index_type DIRTY_INDEX = 0xfefefefe;//再利用プール連結インデックス削除用

public:
	//アクセッサ
	inline size_type offset() const { return m_offset; }//プールバッファのオフセット（アラインメント調整用）
	inline size_type maxSize() const { return m_maxSize; }//プールバッファの全体サイズ（バイト数）
	inline size_type blockSize() const { return m_blockSize; }//ブロックサイズ
	inline size_type blockAlign() const { return m_blockAlign; }//ブロックのアライメント
	inline size_type poolSize() const { return m_poolSize; }//プール数（最大）
	inline size_type size() const { return  m_usingPoolSize.load() * m_blockSize; }//使用中のサイズ（バイト数）
	inline size_type remain() const { return m_maxSize - size(); }//残りサイズ（バイト数）
	inline size_type usingPoolSize() const { return m_usingPoolSize.load(); }//使用中のプール数
	inline size_type poolRemain() const { return m_poolSize - m_usingPoolSize; }//残りのプール数

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
	bool deleteArray(T*& p, const std::size_t num);

	//デバッグ情報作成
	//※十分なサイズのバッファを渡す必要あり。
	//※使用したバッファのサイズを返す。
	//※作成中、他のスレッドで操作が発生すると、不整合が生じる可能性がある点に注意
	template<typename T, class FUNC = std::function<std::size_t(char* message, const T& value)>>
	std::size_t debugInfo(char* message, FUNC print_node);
	std::size_t debugInfo(char* message);

private:
	//メモリ解放（共通処理）
	bool free(void* p, const index_type index);
	
	//ポインタをインデックスに変換
	inline index_type ptrToIndex(void* p);

	//インデックスに対応するバッファのポインタを取得
	inline const void* refBuff(const index_type index) const;
	inline void* refBuff(const index_type index);

public:
	//コンストラクタ
	inline lfPoolAllocator(void* buff, const std::size_t max_size, const std::size_t bock_size, const std::size_t block_align = GASHA_ DEFAULT_ALIGN);
	template<typename T>
	inline lfPoolAllocator(T* buff, const std::size_t num);
	template<typename T, std::size_t N>
	inline lfPoolAllocator(T (&buff)[N]);
	//デストラクタ
	inline ~lfPoolAllocator();

private:
	//フィールド
	char* m_buffRef;//プールバッファの参照
	const size_type m_offset;//プールバッファのオフセット（アラインメント調整用）
	const size_type m_maxSize;//プールバッファの全体サイズ
	const size_type m_blockSize;//ブロックサイズ
	const size_type m_blockAlign;//ブロックのアライメント
	const size_type m_poolSize;//プール数
	std::atomic<index_type> m_vacantHead;//空きプールの先頭インデックス
	std::atomic<index_type> m_recyclableHead;//再利用プールの先頭インデックス
	std::atomic<unsigned char> m_tag;//ABA問題対策用のタグ
	std::atomic<char> m_using[MAX_POOL_SIZE];//使用中インデックス（二重解放判定＆保険の排他制御用）  ※std::bitset使用不可
	std::atomic<size_type> m_usingPoolSize;//使用中の数（デバッグ用）※制御に必要な情報ではない
	//std::atomic<size_type> m_allocCount[MAX_POOL_SIZE];//アロケート回数（デバッグ用）※制御に必要な情報ではない
	//std::atomic<size_type> m_freeCount[MAX_POOL_SIZE];//フリー回数（デバッグ用）※制御に必要な情報ではない
};

//--------------------------------------------------------------------------------
//バッファ付きプールアロケータクラス
//※アラインメント分余計にバッファを確保するので注意
template<std::size_t _BLOCK_SIZE, std::size_t _POOL_SIZE, std::size_t _BLOCK_ALIGN = GASHA_ DEFAULT_ALIGN>
class lfPoolAllocator_withBuff : public lfPoolAllocator<_POOL_SIZE>
{
	//静的アサーション
	static_assert(isValidStaticAlign<_BLOCK_ALIGN>::value == true, "lfPoolAllocator_withBuff: _BLOCK_ALIGN is invalid. ");
public:
	//定数
	static const std::size_t BLOCK_ALIGN = _BLOCK_ALIGN;//ブロックのアラインメント
	static const std::size_t BLOCK_SIZE = adjustStaticAlign<_BLOCK_SIZE, BLOCK_ALIGN>::value;//ブロックサイズ
	static const std::size_t POOL_SIZE = _POOL_SIZE;//プール数
	static const std::size_t MAX_SIZE = BLOCK_SIZE * POOL_SIZE + BLOCK_ALIGN;//プールバッファの全体サイズ ※アラインメント分余計に確保する
public:
	//コンストラクタ
	inline lfPoolAllocator_withBuff();
	//デストラクタ
	inline ~lfPoolAllocator_withBuff();
private:
	char m_buff[MAX_SIZE];//プールバッファ
};
//----------------------------------------
//※バッファを型で指定
//※アラインメント分余計にバッファを確保するので注意
template<typename T, std::size_t _POOL_SIZE>
class lfPoolAllocator_withType : public lfPoolAllocator<_POOL_SIZE>
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
	inline lfPoolAllocator_withType();
	//デストラクタ
	inline ~lfPoolAllocator_withType();
private:
	GASHA_ALIGNAS_OF(block_type) char m_buff[MAX_SIZE];//プールバッファ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルードする場合
#include <gasha/lf_pool_allocator.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_LF_POOL_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H
#include <gasha/lf_pool_allocator.cpp.h>
#endif//GASHA_LF_POOL_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_LOCKFREE_POOL_ALLOCATOR_H

// End of file

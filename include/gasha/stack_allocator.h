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

#include <gasha/dummy_lock.h>//ダミーロック

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

#if 0

//--------------------------------------------------------------------------------
//スタックアロケータクラス
template<class LOCK_TYPE = GASHA_ dummyLock>
class stackAllocator
{
public:
	//定数
	static const std::size_t STACK_SIZE = _STACK_SIZE;//スタックサイズ

public:
	//アクセッサ
	inline std::size_t maxSize() const { return m_maxSize; }//バッファの全体サイズ（バイト数）
	inline std::size_t size() const { return  m_size; }//使用中のサイズ（バイト数）
	inline std::size_t remain() const { return m_maxSize - size(); }//残りサイズ（プール数）

public:
	//メソッド
	//メモリ確保
	void* alloc(const std::size_t size, const std::size_t align = DEFAULT_ALIGN)
	{
		const uintptr_t now_ptr = reinterpret_cast<uintptr_t>(m_buffPtr) + m_size;//現在のポインタ位置算出
		const uintptr_t align_diff = align > 0 ? now_ptr % align == 0 ? 0 : align - now_ptr % align : 0;//アラインメント計算
		const marker_t next_used = m_size + align_diff + size;//次のマーカー算出
		if (next_used > m_maxSize)//メモリオーバーチェック（符号なしなので、範囲チェックは大判定のみでOK）
		{
			//printf("stack overflow!(size=%d+align=%d, remain=%d)\n", size, align_diff, m_maxSize - m_size);
			return nullptr;
		}
		const uintptr_t alloc_ptr = now_ptr + align_diff;//メモリ確保アドレス算出
		m_size = next_used;//マーカー更新
		return reinterpret_cast<void*>(alloc_ptr);
	}
	//メモリを以前のマーカーに戻す
	//※マーカー指定版
	void back(const marker_t marker)
	{
		if (marker >= m_size)//符号なしなので、範囲チェックは大判定のみでOK
			return;
		m_size = marker;
	}
	//メモリを以前のマーカーに戻す
	//※ポインタ指定版
	void back(const void* p)
	{
		const marker_t marker = reinterpret_cast<uintptr_t>(p)-reinterpret_cast<uintptr_t>(m_buffPtr);
		back(marker);
	}
	//メモリ破棄
	void clear()
	{
		m_size = 0;//マーカーをリセット
	}
public:
	//コンストラクタ
	//コンストラクタ
	inline stackAllocator(const char* buff, const std::size_t max_size, const std::size_t bock_size, const std::size_t block_align = DEFAULT_ALIGN);
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
	std::size_t m_usingCount;//使用中の数
	lock_type m_lock;//ロックオブジェクト
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/stack_allocator.inl>

#endif

#endif//GASHA_INCLUDED_STACK_ALLOCATOR_H

// End of file

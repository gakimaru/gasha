#pragma once
#ifndef GASHA_INCLUDED_LOCKFREE_POOL_ALLOCATOR_INL
#define GASHA_INCLUDED_LOCKFREE_POOL_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_pool_allocator.inl
// ロックフリープールアロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_pool_allocator.h>//ロックフリープールアロケータ【宣言部】

#include <utility>//C++11 std::forward
#include <stdio.h>//sprintf()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

//【VC++】sprintf を使用すると、error C4996 が発生する
//  error C4996: 'sprintf': This function or variable may be unsafe. Consider using strncpy_fast_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリープールアロケータクラス

//メモリ確保とコンストラクタ呼び出し
template<std::size_t _MAX_POOL_SIZE>
template<typename T, typename...Tx>
T* lfPoolAllocator<_MAX_POOL_SIZE>::newObj(Tx&&... args)
{
	assert(sizeof(T) <= m_blockSize);
	void* p = alloc();
	if (!p)
		return nullptr;
	return new(p)T(std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<std::size_t _MAX_POOL_SIZE>
template<typename T>
bool lfPoolAllocator<_MAX_POOL_SIZE>::deleteObj(T* p)
{
	const index_type index = ptrToIndex(p);//ポインタをインデックスに変換
	if (index == INVALID_INDEX)
		return false;
	p->~T();//デストラクタ呼び出し
	operator delete(p, p);//（作法として）deleteオペレータ呼び出し
	return free(p, index);
}

//デバッグ情報削除
template<std::size_t _MAX_POOL_SIZE>
template<typename T, class FUNC>
std::size_t lfPoolAllocator<_MAX_POOL_SIZE>::debugInfo(char* message, FUNC print_node)
{
#ifdef GASHA_HAS_DEBUG_FEATURE
	std::size_t size = 0;
	size += sprintf(message + size, "----- Debug Info for lfPoolAllocator -----\n");
	size += sprintf(message + size, "buffRef=%p, offset=%d, maxSize=%d, blockSize=%d, blockAlign=%d, poolSize=%d, usingPoolSize=%d, size=%d, remain=%d, vacantHead=%d\n", m_buffRef, offset(), maxSize(), blockSize(), blockAlign(), poolSize(), usingPoolSize(), this->size(), remain(), m_vacantHead.load());

	size += sprintf(message + size, "Using:\n");
	for (std::size_t index = 0; index < m_poolSize; ++index)
	{
		if (m_using[index].load() != 0)
		{
			size += sprintf(message + size, "[%d]", index);
			if (m_using[index].load() != 1)
				size += sprintf(message + size, "(using=%d)", m_using[index].load());
			//size += sprintf(message + size, "(leak=%d)", static_cast<int>(m_allocCount[index].load() - m_freeCount[index].load()));
			T* value = reinterpret_cast<T*>(refBuff(index));
			size += print_node(message + size, *value);
			size += sprintf(message + size, "\n");
		}
		//else
		//{
		//	if (m_allocCount[index].load() != m_freeCount[index].load())
		//		size += sprintf(message + size, "[%d](leak=%d)\n", index, static_cast<int>(m_allocCount[index].load() - m_freeCount[index].load()));
		//}
	}
	size += sprintf(message + size, "Recycable pool:\n");
	index_type recycable_index_and_tag = m_recyclableHead;
	while (recycable_index_and_tag != INVALID_INDEX)
	{
		index_type recycable_index = recycable_index_and_tag & 0x00ffffff;
		index_type tag = recycable_index_and_tag >> 24;
		size += sprintf(message + size, " [%d(tag=%d)]", recycable_index, tag);
		recycable_t* recycable_pool = reinterpret_cast<recycable_t*>(refBuff(recycable_index));
		recycable_index_and_tag = recycable_pool->m_next_index.load();
	}
	size += sprintf(message + size, "\n");
	size += sprintf(message + size, "----------\n");
	return size;
#else//GASHA_HAS_DEBUG_FEATURE
	message[0] = '\0';
	return 0;
#endif//GASHA_HAS_DEBUG_FEATURE
}

//ポインタをインデックスに変換
template<std::size_t _MAX_POOL_SIZE>
inline typename lfPoolAllocator<_MAX_POOL_SIZE>::index_type lfPoolAllocator<_MAX_POOL_SIZE>::ptrToIndex(void* p)
{
	const index_type index = static_cast<index_type>((reinterpret_cast<char*>(p) - reinterpret_cast<char*>(m_buffRef)) / m_blockSize);
	if (index >= m_poolSize)//範囲外のインデックスなら終了
	{
	#ifdef GASHA_ASSERTION_IS_ENABLED
		static const bool IS_INVALID_POINTER_OF_POOL = false;
		assert(IS_INVALID_POINTER_OF_POOL);
	#endif//GASHA_ASSERTION_IS_ENABLED
		return INVALID_INDEX;
	}
	if (m_using[index].load() == 0)//インデックスが既に未使用状態なら終了
	{
	#ifdef GASHA_ASSERTION_IS_ENABLED
		static const bool IS_ALREADY_DELETE_POINTER = false;
		assert(IS_ALREADY_DELETE_POINTER);
	#endif//GASHA_ASSERTION_IS_ENABLED
		return INVALID_INDEX;
	}
	return index;
}

//インデックスに対応するバッファのポインタを取得
template<std::size_t _MAX_POOL_SIZE>
inline const void* lfPoolAllocator<_MAX_POOL_SIZE>::refBuff(const std::size_t index) const
{
	return reinterpret_cast<char*>(m_buffRef) + (index * m_blockSize);
}
template<std::size_t _MAX_POOL_SIZE>
inline void* lfPoolAllocator<_MAX_POOL_SIZE>::refBuff(const std::size_t index)
{
	return reinterpret_cast<char*>(m_buffRef) + (index * m_blockSize);
}

//コンストラクタ
template<std::size_t _MAX_POOL_SIZE>
inline lfPoolAllocator<_MAX_POOL_SIZE>::lfPoolAllocator(void* buff, const std::size_t max_size, const std::size_t block_size, const std::size_t block_align) :
	m_buffRef(reinterpret_cast<char*>(adjustAlign(buff, block_align))),
	m_offset(m_buffRef - reinterpret_cast<char*>(buff)),
	m_maxSize(max_size - m_offset),
	m_blockSize(block_size),
	m_blockAlign(block_align),
	m_poolSize(m_maxSize / m_blockSize),
	m_vacantHead(0),
	m_recyclableHead(INVALID_INDEX),
	m_usingCount(0)
{
	assert(m_poolSize <= MAX_POOL_SIZE);
	assert(m_maxSize > 0);
	assert(m_poolSize > 0);
	
	for (std::size_t i = 0; i < m_poolSize; ++i)
	{
		m_using[i].store(0);
		//m_allocCount[i].store(0);
		//m_freeCount[i].store(0);
	}
}
template<std::size_t _MAX_POOL_SIZE>
template<typename T>
inline lfPoolAllocator<_MAX_POOL_SIZE>::lfPoolAllocator(T* buff, const std::size_t max_size) :
	lfPoolAllocator(reinterpret_cast<void*>(buff), max_size, sizeof(T), alignof(T))//C++11 委譲コンストラクタ
{}
template<std::size_t _MAX_POOL_SIZE>
template<typename T, std::size_t N>
inline lfPoolAllocator<_MAX_POOL_SIZE>::lfPoolAllocator(T(&buff)[N]) :
lfPoolAllocator(reinterpret_cast<void*>(buff), sizeof(buff), sizeof(T), alignof(T))//C++11 委譲コンストラクタ
{}

//デストラクタ
template<std::size_t _MAX_POOL_SIZE>
inline lfPoolAllocator<_MAX_POOL_SIZE>::~lfPoolAllocator()
{}

//--------------------------------------------------------------------------------
//バッファ付きプールアロケータクラス

//コンストラクタ
template<std::size_t _BLOCK_SIZE, std::size_t _POOL_SIZE, std::size_t _BLOCK_ALIGN>
inline lfPoolAllocator_withBuff<_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN>::lfPoolAllocator_withBuff() :
lfPoolAllocator<_POOL_SIZE>(m_buff, MAX_SIZE, BLOCK_SIZE, BLOCK_ALIGN)
{}

//デストラクタ
template<std::size_t _BLOCK_SIZE, std::size_t _POOL_SIZE, std::size_t _BLOCK_ALIGN>
inline lfPoolAllocator_withBuff<_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN>::~lfPoolAllocator_withBuff()
{}

//--------------------------------------------------------------------------------
//バッファ付きプールアロケータクラス
//※型指定版

//メモリ確保とコンストラクタ呼び出し
template<typename T, std::size_t _POOL_SIZE>
template<typename... Tx>
inline T* lfPoolAllocator_withType<T, _POOL_SIZE>::newDefault(Tx&&... args)
{
	return this->template newObj<block_type>(std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<typename T, std::size_t _POOL_SIZE>
inline bool lfPoolAllocator_withType<T, _POOL_SIZE>::deleteDefault(T*& p)
{
	return this->template deleteObj<block_type>(p);
}

//コンストラクタ
template<typename T, std::size_t _POOL_SIZE>
inline lfPoolAllocator_withType<T, _POOL_SIZE>::lfPoolAllocator_withType() :
	lfPoolAllocator<_POOL_SIZE>(reinterpret_cast<void*>(m_buff), MAX_SIZE, BLOCK_SIZE, BLOCK_ALIGN)
{}

//デストラクタ
template<typename T, std::size_t _POOL_SIZE>
inline lfPoolAllocator_withType<T, _POOL_SIZE>::~lfPoolAllocator_withType()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_LOCKFREE_POOL_ALLOCATOR_INL

// End of file

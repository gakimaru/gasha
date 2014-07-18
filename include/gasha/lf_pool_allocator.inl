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

#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し
#include <gasha/utility.h>//汎用ユーティリティ：min()
#include <gasha/string.h>//文字列処理：spprintf()

#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリープールアロケータクラス

//メモリ確保とコンストラクタ呼び出し
template<std::size_t _MAX_POOL_SIZE>
template<typename T, typename...Tx>
T* lfPoolAllocator<_MAX_POOL_SIZE>::newObj(Tx&&... args)
{
	void* p = alloc(sizeof(T), alignof(T));
	if (!p)
		return nullptr;
	return GASHA_ callConstructor<T>(p, std::forward<Tx>(args)...);
}
//※配列用
template<std::size_t _MAX_POOL_SIZE>
template<typename T, typename...Tx>
T* lfPoolAllocator<_MAX_POOL_SIZE>::newArray(const std::size_t num, Tx&&... args)
{
	void* p = alloc(sizeof(T) * num, alignof(T));
	if (!p)
		return nullptr;
	T* top_obj = nullptr;
	for (std::size_t i = 0; i < num; ++i)
	{
		T* obj = GASHA_ callConstructor<T>(p, std::forward<Tx>(args)...);
		if (!top_obj)
			top_obj = obj;
		p = reinterpret_cast<void*>(reinterpret_cast<char*>(p) + sizeof(T));
	}
	return top_obj;
}

//メモリ解放とデストラクタ呼び出し
template<std::size_t _MAX_POOL_SIZE>
template<typename T>
bool lfPoolAllocator<_MAX_POOL_SIZE>::deleteObj(T* p)
{
	const index_type index = ptrToIndex(p);//ポインタをインデックスに変換
	if (index == INVALID_INDEX)
		return false;
	GASHA_ callDestructor(p);//デストラクタ呼び出し
	return free(p, index);
}
//※配列用
template<std::size_t _MAX_POOL_SIZE>
template<typename T>
bool lfPoolAllocator<_MAX_POOL_SIZE>::deleteArray(T* p, const std::size_t num)
{
	const index_type index = ptrToIndex(p);//ポインタをインデックスに変換
	if (index == INVALID_INDEX)
		return false;
	T* obj = p;
	for (std::size_t i = 0; i < num; ++i, ++obj)
	{
		GASHA_ callDestructor(obj);//デストラクタ呼び出し
	}
	return free(p, index);
}

//デバッグ情報作成
template<std::size_t _MAX_POOL_SIZE>
template<typename T, class FUNC>
std::size_t lfPoolAllocator<_MAX_POOL_SIZE>::debugInfo(char* message, const std::size_t max_size, const bool with_detail, FUNC print_node) const
{
	std::size_t size = 0;
	GASHA_ spprintf(message, max_size, size, "----- Debug-info for lfPoolAllocator -----\n");
	GASHA_ spprintf(message, max_size, size, "buff=%p, offset=%d, maxSize=%d, blockSize=%d, blockAlign=%d, poolSize=%d, usingPoolSize=%d, poolRemain=%d, size=%d, remain=%d, vacantHead=%d\n", m_buffRef, offset(), maxSize(), blockSize(), blockAlign(), poolSize(), usingPoolSize(), poolRemain(), this->size(), remain(), m_vacantHead.load());

	if (with_detail)
	{
		GASHA_ spprintf(message, max_size, size, "Using:\n");
		std::size_t num = 0;
		for (index_type index = 0; index < m_poolSize; ++index)
		{
			if (m_using[index].load() != 0)
			{
				++num;
				GASHA_ spprintf(message, max_size, size, "[%d] ", index);
				if (m_using[index].load() != 1)
					GASHA_ spprintf(message, max_size, size, "(using=%d)", m_using[index].load());
				//GASHA_ spprintf(message, max_size, size, "(leak=%d)", static_cast<int>(m_allocCount[index].load() - m_freeCount[index].load()));
				const T* value = reinterpret_cast<const T*>(refBuff(index));
				print_node(message, max_size, size, *value);
				GASHA_ spprintf(message, max_size, size, "\n");
			}
			//else
			//{
			//	if (m_allocCount[index].load() != m_freeCount[index].load())
			//		GASHA_ spprintf(message, max_size, size, "[%d](leak=%d)\n", index, static_cast<int>(m_allocCount[index].load() - m_freeCount[index].load()));
			//}
		}
		GASHA_ spprintf(message, max_size, size, "(num=%d)\n", num);
		GASHA_ spprintf(message, max_size, size, "Recycable pool:\n");
		num = 0;
		index_type recycable_index_and_tag = m_recyclableHead;
		while (recycable_index_and_tag != INVALID_INDEX)
		{
			++num;
			index_type recycable_index = recycable_index_and_tag & 0x00ffffff;
			index_type tag = recycable_index_and_tag >> 24;
			GASHA_ spprintf(message, max_size, size, " [%d(tag=%d)]", recycable_index, tag);
			const recycable_t* recycable_pool = reinterpret_cast<const recycable_t*>(refBuff(recycable_index));
			recycable_index_and_tag = recycable_pool->m_next_index.load();
		}
		GASHA_ spprintf(message, max_size, size, "\n");
		GASHA_ spprintf(message, max_size, size, "(num=%d)\n", num);
	}
	GASHA_ spprintf(message, max_size, size, "------------------------------------------\n");
	return size;
}
template<std::size_t _MAX_POOL_SIZE>
inline std::size_t lfPoolAllocator<_MAX_POOL_SIZE>::debugInfo(char* message, const std::size_t max_size) const
{
	return debugInfo(message, max_size, false);
}

//強制クリア
template<std::size_t _MAX_POOL_SIZE>
inline void lfPoolAllocator<_MAX_POOL_SIZE>::clear()
{
	m_vacantHead.store(0);
	m_recyclableHead.store(INVALID_INDEX);
	m_usingPoolSize.store(0);
	for (std::size_t i = 0; i < m_poolSize; ++i)
	{
		m_using[i].store(0);
		//m_allocCount[i].store(0);
		//m_freeCount[i].store(0);
	}
}

//ポインタをインデックスに変換
template<std::size_t _MAX_POOL_SIZE>
inline typename lfPoolAllocator<_MAX_POOL_SIZE>::index_type lfPoolAllocator<_MAX_POOL_SIZE>::ptrToIndex(void* p)
{
	const index_type index = static_cast<index_type>((reinterpret_cast<char*>(p) - reinterpret_cast<char*>(m_buffRef)) / m_blockSize);
	if (index >= m_poolSize)//範囲外のインデックスなら終了
	{
	#ifdef GASHA_LF_POOL_ALLOCATOR_ENABLE_ASSERTION
		static const bool IS_INVALID_POINTER = false;
		assert(IS_INVALID_POINTER);
	#endif//GASHA_LF_POOL_ALLOCATOR_ENABLE_ASSERTION
		return INVALID_INDEX;
	}
	if (m_using[index].load() == 0)//インデックスが既に未使用状態なら終了
	{
	#ifdef GASHA_LF_POOL_ALLOCATOR_ENABLE_ASSERTION
		static const bool IS_ALREADY_DELETE_POINTER = false;
		assert(IS_ALREADY_DELETE_POINTER);
	#endif//GASHA_LF_POOL_ALLOCATOR_ENABLE_ASSERTION
		return INVALID_INDEX;
	}
	return index;
}

//インデックスに対応するバッファのポインタを取得
template<std::size_t _MAX_POOL_SIZE>
inline const void* lfPoolAllocator<_MAX_POOL_SIZE>::refBuff(const typename lfPoolAllocator<_MAX_POOL_SIZE>::index_type index) const
{
	return reinterpret_cast<char*>(m_buffRef) + (index * m_blockSize);
}
template<std::size_t _MAX_POOL_SIZE>
inline void* lfPoolAllocator<_MAX_POOL_SIZE>::refBuff(const typename lfPoolAllocator<_MAX_POOL_SIZE>::index_type index)
{
	return reinterpret_cast<char*>(m_buffRef) + (index * m_blockSize);
}

//コンストラクタ
template<std::size_t _MAX_POOL_SIZE>
inline lfPoolAllocator<_MAX_POOL_SIZE>::lfPoolAllocator(void* buff, const std::size_t buff_size, const std::size_t block_size, const std::size_t block_align) :
	m_buffRef(reinterpret_cast<char*>(adjustAlign(buff, block_align))),
	m_offset(static_cast<size_type>(m_buffRef - reinterpret_cast<char*>(buff))),
	m_maxSize(static_cast<size_type>(buff_size - m_offset)),
	m_blockSize(static_cast<size_type>(block_size)),
	m_blockAlign(static_cast<size_type>(block_align)),
	m_poolSize(static_cast<size_type>(GASHA_ min(m_maxSize / m_blockSize, _MAX_POOL_SIZE))),
	m_vacantHead(0),
	m_recyclableHead(INVALID_INDEX),
	m_usingPoolSize(0)
{
	assert(m_buffRef != nullptr);
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
inline lfPoolAllocator<_MAX_POOL_SIZE>::lfPoolAllocator(T* buff, const std::size_t pool_size) :
lfPoolAllocator(reinterpret_cast<void*>(buff), sizeof(T)* pool_size, sizeof(T), alignof(T))//C++11 委譲コンストラクタ
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
inline typename lfPoolAllocator_withType<T, _POOL_SIZE>::block_type* lfPoolAllocator_withType<T, _POOL_SIZE>::newDefault(Tx&&... args)
{
	return this->template newObj<block_type>(std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<typename T, std::size_t _POOL_SIZE>
inline bool lfPoolAllocator_withType<T, _POOL_SIZE>::deleteDefault(typename lfPoolAllocator_withType<T, _POOL_SIZE>::block_type*& p)
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

#endif//GASHA_INCLUDED_LOCKFREE_POOL_ALLOCATOR_INL

// End of file

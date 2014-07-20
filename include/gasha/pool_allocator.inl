#pragma once
#ifndef GASHA_INCLUDED_POOL_ALLOCATOR_INL
#define GASHA_INCLUDED_POOL_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// pool_allocator.inl
// プールアロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/pool_allocator.h>//プールアロケータ【宣言部】

#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し
#include <gasha/utility.h>//汎用ユーティリティ：min()
#include <gasha/string.h>//文字列処理：spprintf()

#include <utility>//C++11 std::forward
#include <cassert>//assert()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//プールアロケータクラス
//メモリ確保とコンストラクタ呼び出し
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
template<typename T, typename...Tx>
T* poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::newObj(Tx&&... args)
{
	void* p = alloc(sizeof(T), alignof(T));
	if (!p)
		return nullptr;
	return GASHA_ callConstructor<T>(p, std::forward<Tx>(args)...);
}
//※配列用
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
template<typename T, typename...Tx>
T* poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::newArray(const std::size_t num, Tx&&... args)
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
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
template<typename T>
bool poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::deleteObj(T* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	const index_type index = ptrToIndex(p);//ポインタをインデックスに変換
	if (index == INVALID_INDEX)
		return false;
	GASHA_ callDestructor(p);//デストラクタ呼び出し
	return free(p, index);
}
//※配列用
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
template<typename T>
bool poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::deleteArray(T* p, const std::size_t num)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
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
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
template<typename T, class FUNC>
std::size_t poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::debugInfo(char* message, const std::size_t max_size, const bool with_detail, FUNC print_node) const
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	std::size_t message_len = 0;
	GASHA_ spprintf(message, max_size, message_len, "----- Debug-info for poolAllocator -----\n");
	GASHA_ spprintf(message, max_size, message_len, "buff=%p, offset=%d, maxSize=%d, blockSize=%d, blockAlign=%d, poolSize=%d, usingPoolSize=%d, poolRemain=%d, size=%d, remain=%d, vacantHead=%d\n", m_buffRef, offset(), maxSize(), blockSize(), blockAlign(), poolSize(), usingPoolSize(), poolRemain(), this->size(), remain(), m_vacantHead);

	if (with_detail)
	{
		GASHA_ spprintf(message, max_size, message_len, "Using:\n");
		std::size_t num = 0;
		for (index_type index = 0; index < m_poolSize; ++index)
		{
			if (m_using[index])
			{
				++num;
				GASHA_ spprintf(message, max_size, message_len, "[%d] ", index);
				const T* value = reinterpret_cast<const T*>(refBuff(index));
				print_node(message, max_size, message_len, *value);
				GASHA_ spprintf(message, max_size, message_len, "\n");
			}
		}
		GASHA_ spprintf(message, max_size, message_len, "(num=%d)\n", num);
		GASHA_ spprintf(message, max_size, message_len, "Recycable pool:\n");
		num = 0;
		index_type recycable_index = m_recyclableHead;
		while (recycable_index != INVALID_INDEX)
		{
			++num;
			GASHA_ spprintf(message, max_size, message_len, " [%d]", recycable_index);
			const recycable_t* recycable_pool = reinterpret_cast<const recycable_t*>(refBuff(recycable_index));
			recycable_index = recycable_pool->m_next_index;
		}
		GASHA_ spprintf(message, max_size, message_len, "\n");
		GASHA_ spprintf(message, max_size, message_len, "(num=%d)\n", num);
	}
	GASHA_ spprintf(message, max_size, message_len, "----------------------------------------");//最終行改行なし
	return message_len;
}
//デバッグ情報作成
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
inline std::size_t poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::debugInfo(char* message, const std::size_t max_size) const
{
	return debugInfo(message, max_size, false);
}

//強制クリア
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
inline void poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::clear()
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	m_vacantHead = 0;
	m_recyclableHead = INVALID_INDEX;
	m_usingPoolSize = 0;
	m_using.reset();
}

//ポインタをインデックスに変換
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
inline typename poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::index_type poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::ptrToIndex(void* p)
{
	const index_type index = static_cast<index_type>((reinterpret_cast<char*>(p)-reinterpret_cast<char*>(m_buffRef)) / m_blockSize);
	if (index >= m_poolSize)//範囲外のインデックスなら終了
	{
	#ifdef GASHA_POOL_ALLOCATOR_ENABLE_ASSERTION
		static const bool IS_INVALID_POINTER = false;
		assert(IS_INVALID_POINTER);
	#endif//GASHA_POOL_ALLOCATOR_ENABLE_ASSERTION
		return INVALID_INDEX;
	}
	if (!m_using[index])//インデックスが既に未使用状態なら終了
	{
	#ifdef GASHA_POOL_ALLOCATOR_ENABLE_ASSERTION
		static const bool IS_ALREADY_DELETE_POINTER = false;
		assert(IS_ALREADY_DELETE_POINTER);
	#endif//GASHA_POOL_ALLOCATOR_ENABLE_ASSERTION
		return INVALID_INDEX;
	}
	return index;
}

//インデックスに対応するバッファのポインタを取得
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
inline const void* poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::refBuff(const typename poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::index_type index) const
{
	return reinterpret_cast<char*>(m_buffRef) + (index * m_blockSize);
}
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
inline void* poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::refBuff(const typename poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::index_type index)
{
	return reinterpret_cast<char*>(m_buffRef) + (index * m_blockSize);
}

//コンストラクタ
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
inline poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::poolAllocator(void* buff, const std::size_t buff_size, const std::size_t block_size, const std::size_t block_align) :
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
#ifdef GASHA_POOL_ALLOCATOR_ENABLE_ASSERTION
	assert(m_buffRef != nullptr);
	assert(m_maxSize > 0);
	assert(m_poolSize > 0);
#endif//GASHA_POOL_ALLOCATOR_ENABLE_ASSERTION
}
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
template<typename T>
inline poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::poolAllocator(T* buff, const std::size_t pool_size) :
poolAllocator(reinterpret_cast<void*>(buff), sizeof(T)* pool_size, sizeof(T), alignof(T))//C++11 委譲コンストラクタ
{}
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
template<typename T, std::size_t N>
inline poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::poolAllocator(T(&buff)[N]) :
poolAllocator(reinterpret_cast<void*>(buff), sizeof(buff), sizeof(T), alignof(T))//C++11 委譲コンストラクタ
{}

//デストラクタ
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
inline poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::~poolAllocator()
{}

//--------------------------------------------------------------------------------
//バッファ付きプールアロケータクラス

//コンストラクタ
template<std::size_t _BLOCK_SIZE, std::size_t _POOL_SIZE, std::size_t _BLOCK_ALIGN, class LOCK_TYPE>
inline poolAllocator_withBuff<_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN, LOCK_TYPE>::poolAllocator_withBuff() :
poolAllocator<_POOL_SIZE, LOCK_TYPE>(m_buff, MAX_SIZE, BLOCK_SIZE, BLOCK_ALIGN)
{}

//デストラクタ
template<std::size_t _BLOCK_SIZE, std::size_t _POOL_SIZE, std::size_t _BLOCK_ALIGN, class LOCK_TYPE>
inline poolAllocator_withBuff<_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN, LOCK_TYPE>::~poolAllocator_withBuff()
{}

//--------------------------------------------------------------------------------
//バッファ付きプールアロケータクラス
//※型指定版

//メモリ確保とコンストラクタ呼び出し
template<typename T, std::size_t _POOL_SIZE, class LOCK_TYPE>
template<typename... Tx>
inline typename poolAllocator_withType<T, _POOL_SIZE, LOCK_TYPE>::block_type* poolAllocator_withType<T, _POOL_SIZE, LOCK_TYPE>::newDefault(Tx&&... args)
{
	return this->template newObj<block_type>(std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<typename T, std::size_t _POOL_SIZE, class LOCK_TYPE>
inline bool poolAllocator_withType<T, _POOL_SIZE, LOCK_TYPE>::deleteDefault(typename poolAllocator_withType<T, _POOL_SIZE, LOCK_TYPE>::block_type*& p)
{
	return this->template deleteObj<block_type>(p);
}

//コンストラクタ
template<typename T, std::size_t _POOL_SIZE, class LOCK_TYPE>
inline poolAllocator_withType<T, _POOL_SIZE, LOCK_TYPE>::poolAllocator_withType() :
	poolAllocator<_POOL_SIZE, LOCK_TYPE>(reinterpret_cast<void*>(m_buff), MAX_SIZE, BLOCK_SIZE, BLOCK_ALIGN)
{}

//デストラクタ
template<typename T, std::size_t _POOL_SIZE, class LOCK_TYPE>
inline poolAllocator_withType<T, _POOL_SIZE, LOCK_TYPE>::~poolAllocator_withType()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_POOL_ALLOCATOR_INL

// End of file

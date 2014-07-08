#pragma once
#ifndef GASHA_INCLUDED_STACK_ALLOCATOR_INL
#define GASHA_INCLUDED_STACK_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// stack_allocator.inl
// スタックアロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/stack_allocator.h>//スタックアロケータ【宣言部】

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

//メモリ確保とコンストラクタ呼び出し
template<std::size_t _MAX_STACK_SIZE, class LOCK_TYPE>
template<typename T, typename...Tx>
T* stackAllocator<_MAX_STACK_SIZE, LOCK_TYPE>::newObj(Tx&&... args)
{
	assert(sizeof(T) <= m_blockSize);
	void* p = alloc();
	if (!p)
		return nullptr;
	return new(p)T(std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<std::size_t _MAX_STACK_SIZE, class LOCK_TYPE>
template<typename T>
bool stackAllocator<_MAX_STACK_SIZE, LOCK_TYPE>::deleteObj(T*& p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	const std::size_t index = ptrToIndex(p);//ポインタをインデックスに変換
	if (index == INVALID_INDEX)
		return false;
	p->~T();//デストラクタ呼び出し
	//operator delete(p, p);//（作法として）deleteオペレータ呼び出し
	return free(p, index);
}

//デバッグ情報作成
template<std::size_t _MAX_STACK_SIZE, class LOCK_TYPE>
template<typename T, class FUNC>
std::size_t stackAllocator<_MAX_STACK_SIZE, LOCK_TYPE>::debugInfo(char* message, FUNC print_node)
{
#ifdef GASHA_HAS_DEBUG_FEATURE
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	std::size_t size = 0;
	size += sprintf(message + size, "----- Debug Info for stackAllocator -----\n");
	size += sprintf(message + size, "buffRef=%p, offset=%d, maxSize=%d, blockSize=%d, blockAlign=%d, stackSize=%d, usingStackSize=%d, stackRemain=%d, size=%d, remain=%d, vacantHead=%d\n", m_buffRef, offset(), maxSize(), blockSize(), blockAlign(), stackSize(), usingStackSize(), stackRemain(), this->size(), remain(), m_vacantHead);

	size += sprintf(message + size, "Using:\n");
	for (std::size_t index = 0; index < m_stackSize; ++index)
	{
		if (m_using[index])
		{
			size += sprintf(message + size, "[%d] ", index);
			T* value = static_cast<T*>(refBuff(index));
			size += print_node(message + size, *value);
			size += sprintf(message + size, "\n");
		}
	}
	size += sprintf(message + size, "Recycable stack:\n");
	std::size_t recycable_index = m_recyclableHead;
	while (recycable_index != INVALID_INDEX)
	{
		size += sprintf(message + size, " [%d]", recycable_index);
		recycable_t* recycable_stack = static_cast<recycable_t*>(refBuff(recycable_index));
		recycable_index = recycable_stack->m_next_index;
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
template<std::size_t _MAX_STACK_SIZE, class LOCK_TYPE>
inline std::size_t stackAllocator<_MAX_STACK_SIZE, LOCK_TYPE>::ptrToIndex(void* p)
{
	const std::size_t index = (reinterpret_cast<char*>(p) - reinterpret_cast<char*>(m_buffRef)) / m_blockSize;
	if (index >= m_stackSize)//範囲外のインデックスなら終了
	{
	#ifdef GASHA_ASSERTION_IS_ENABLED
		static const bool IS_INVALID_POINTER_OF_STACK = false;
		assert(IS_INVALID_POINTER_OF_STACK);
	#endif//GASHA_ASSERTION_IS_ENABLED
		return INVALID_INDEX;
	}
	if (!m_using[index])//インデックスが既に未使用状態なら終了
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
template<std::size_t _MAX_STACK_SIZE, class LOCK_TYPE>
inline const void* stackAllocator<_MAX_STACK_SIZE, LOCK_TYPE>::refBuff(const std::size_t index) const
{
	return reinterpret_cast<char*>(m_buffRef) + (index * m_blockSize);
}
template<std::size_t _MAX_STACK_SIZE, class LOCK_TYPE>
inline void* stackAllocator<_MAX_STACK_SIZE, LOCK_TYPE>::refBuff(const std::size_t index)
{
	return reinterpret_cast<char*>(m_buffRef) + (index * m_blockSize);
}

//コンストラクタ
template<std::size_t _MAX_STACK_SIZE, class LOCK_TYPE>
inline stackAllocator<_MAX_STACK_SIZE, LOCK_TYPE>::stackAllocator(void* buff, const std::size_t max_size, const std::size_t block_size, const std::size_t block_align) :
	m_buffRef(reinterpret_cast<char*>(adjustAlign(buff, block_align))),
	m_offset(m_buffRef - reinterpret_cast<char*>(buff)),
	m_maxSize(max_size - m_offset),
	m_blockSize(block_size),
	m_blockAlign(block_align),
	m_stackSize(m_maxSize / m_blockSize),
	m_vacantHead(0),
	m_recyclableHead(INVALID_INDEX),
	m_usingStackSize(0)
{
	assert(m_stackSize <= MAX_STACK_SIZE);
	assert(m_maxSize > 0);
	assert(m_stackSize > 0);
}
template<std::size_t _MAX_STACK_SIZE, class LOCK_TYPE>
template<typename T>
inline stackAllocator<_MAX_STACK_SIZE, LOCK_TYPE>::stackAllocator(T* buff, const std::size_t max_size) :
	stackAllocator(reinterpret_cast<void*>(buff), max_size, sizeof(T), alignof(T))//C++11 委譲コンストラクタ
{}
template<std::size_t _MAX_STACK_SIZE, class LOCK_TYPE>
template<typename T, std::size_t N>
inline stackAllocator<_MAX_STACK_SIZE, LOCK_TYPE>::stackAllocator(T(&buff)[N]) :
stackAllocator(reinterpret_cast<void*>(buff), sizeof(buff), sizeof(T), alignof(T))//C++11 委譲コンストラクタ
{}

//デストラクタ
template<std::size_t _MAX_STACK_SIZE, class LOCK_TYPE>
inline stackAllocator<_MAX_STACK_SIZE, LOCK_TYPE>::~stackAllocator()
{}

//--------------------------------------------------------------------------------
//バッファ付きスタックアロケータクラス

//コンストラクタ
template<std::size_t _BLOCK_SIZE, std::size_t _STACK_SIZE, std::size_t _BLOCK_ALIGN, class LOCK_TYPE>
inline stackAllocator_withBuff<_BLOCK_SIZE, _STACK_SIZE, _BLOCK_ALIGN, LOCK_TYPE>::stackAllocator_withBuff() :
stackAllocator<_STACK_SIZE, LOCK_TYPE>(m_buff, MAX_SIZE, BLOCK_SIZE, BLOCK_ALIGN)
{}

//デストラクタ
template<std::size_t _BLOCK_SIZE, std::size_t _STACK_SIZE, std::size_t _BLOCK_ALIGN, class LOCK_TYPE>
inline stackAllocator_withBuff<_BLOCK_SIZE, _STACK_SIZE, _BLOCK_ALIGN, LOCK_TYPE>::~stackAllocator_withBuff()
{}

//--------------------------------------------------------------------------------
//バッファ付きスタックアロケータクラス
//※型指定版

//メモリ確保とコンストラクタ呼び出し
template<typename T, std::size_t _STACK_SIZE, class LOCK_TYPE>
template<typename... Tx>
inline T* stackAllocator_withType<T, _STACK_SIZE, LOCK_TYPE>::newDefault(Tx&&... args)
{
	return this->template newObj<block_type>(std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<typename T, std::size_t _STACK_SIZE, class LOCK_TYPE>
inline bool stackAllocator_withType<T, _STACK_SIZE, LOCK_TYPE>::deleteDefault(T*& p)
{
	return this->template deleteObj<block_type>(p);
}

//コンストラクタ
template<typename T, std::size_t _STACK_SIZE, class LOCK_TYPE>
inline stackAllocator_withType<T, _STACK_SIZE, LOCK_TYPE>::stackAllocator_withType() :
	stackAllocator<_STACK_SIZE, LOCK_TYPE>(reinterpret_cast<void*>(m_buff), MAX_SIZE, BLOCK_SIZE, BLOCK_ALIGN)
{}

//デストラクタ
template<typename T, std::size_t _STACK_SIZE, class LOCK_TYPE>
inline stackAllocator_withType<T, _STACK_SIZE, LOCK_TYPE>::~stackAllocator_withType()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_STACK_ALLOCATOR_INL

// End of file

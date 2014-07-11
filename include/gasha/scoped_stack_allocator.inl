#pragma once
#ifndef GASHA_INCLUDED_SCOPED_STACK_ALLOCATOR_INL
#define GASHA_INCLUDED_SCOPED_STACK_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// scoped_stack_allocator.inl
// スコープスタックアロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/scoped_stack_allocator.h>//スコープスタックアロケータ【宣言部】

#include <utility>//C++11 std::forward
#include <stdio.h>//sprintf()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】sprintf を使用すると、error C4996 が発生する
//  error C4996: 'sprintf': This function or variable may be unsafe. Consider using strncpy_fast_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//スコープスタックアロケータクラス

//名前
template<class ALLOCATOR>
inline const char* scopedStackAllocator<ALLOCATOR>::name() const
{
	return m_allocator.name();
}

//実装モード
template<class ALLOCATOR>
inline const char* scopedStackAllocator<ALLOCATOR>::mode() const
{
	return m_allocator.mode();
}

//バッファの全体サイズ（バイト数）
template<class ALLOCATOR>
inline typename scopedStackAllocator<ALLOCATOR>::size_type scopedStackAllocator<ALLOCATOR>::maxSize() const
{
	return m_allocator.maxSize() - m_initSize;
}

//使用中のサイズ（バイト数）
template<class ALLOCATOR>
inline typename scopedStackAllocator<ALLOCATOR>::size_type scopedStackAllocator<ALLOCATOR>::size() const
{
	return m_allocator.size() - m_initSize;
}

//残りサイズ（バイト数）
template<class ALLOCATOR>
inline typename scopedStackAllocator<ALLOCATOR>::size_type scopedStackAllocator<ALLOCATOR>::remain() const
{
	return maxSize() - size();
}

//アロケート中の数
template<class ALLOCATOR>
inline typename scopedStackAllocator<ALLOCATOR>::size_type scopedStackAllocator<ALLOCATOR>::count() const
{
	return m_allocator.count() - m_initCount;
}

//メモリ確保
template<class ALLOCATOR>
inline void* scopedStackAllocator<ALLOCATOR>::alloc(const std::size_t size, const std::size_t align)
{
	return m_allocator.alloc(size, align);
}

//メモリ解放
template<class ALLOCATOR>
inline bool scopedStackAllocator<ALLOCATOR>::free(void* p)
{
	return m_allocator.free(p);
}

//メモリ確保とコンストラクタ呼び出し
template<class ALLOCATOR>
template<typename T, typename...Tx>
T* scopedStackAllocator<ALLOCATOR>::newObj(Tx&&... args)
{
	return m_allocator.template newObj<T>(std::forward<Tx>(args)...);
}
//※配列用
template<class ALLOCATOR>
template<typename T, typename...Tx>
T* scopedStackAllocator<ALLOCATOR>::newArray(const std::size_t num, Tx&&... args)
{
	return m_allocator.template newArray<T>(num, std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<class ALLOCATOR>
template<typename T>
bool scopedStackAllocator<ALLOCATOR>::deleteObj(T* p)
{
	return m_allocator.deleteObj(p);
}
//※配列用
template<class ALLOCATOR>
template<typename T>
bool scopedStackAllocator<ALLOCATOR>::deleteArray(T* p, const std::size_t num)
{
	return m_allocator.deleteArray(p, num);
}

//使用中のサイズを指定位置に戻す
//※位置指定版
template<class ALLOCATOR>
inline bool  scopedStackAllocator<ALLOCATOR>::rewind(const size_type pos)
{
	return m_allocator.rewind(m_initSize + pos);
}
//※ポインタ指定版
template<class ALLOCATOR>
inline bool  scopedStackAllocator<ALLOCATOR>::rewind(void* p)
{
	return m_allocator.rewind(p);
}

//メモリクリア
template<class ALLOCATOR>
inline void scopedStackAllocator<ALLOCATOR>::clear()
{
	m_allocator.resetSizeAndCount(m_initSize, m_initCount);
}

//デバッグ情報作成
template<class ALLOCATOR>
std::size_t scopedStackAllocator<ALLOCATOR>::debugInfo(char* message)
{
	std::size_t size = 0;
	size += sprintf(message + size, "----- Debug Info for scopedStackAllocator -----\n");
	size += sprintf(message + size, "maxSize=%d, size=%d, remain=%d, count=%d (INIT: buff=%p, size=%d, count=%d)\n", maxSize(), this->size(), remain(), count(), m_allocator.buff(), m_initSize, m_initCount);
	size += sprintf(message + size, "-----------------------------------------------\n");
	return size;
}

//コンストラクタ
template<class ALLOCATOR>
inline scopedStackAllocator<ALLOCATOR>::scopedStackAllocator(allocator_type& allocator) :
	m_allocator(allocator)
{
	m_allocator.getSizeAndCount(m_initSize, m_initCount);
}

//デストラクタ
template<class ALLOCATOR>
inline scopedStackAllocator<ALLOCATOR>::~scopedStackAllocator()
{
	clear();
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_SCOPED_STACK_ALLOCATOR_INL

// End of file

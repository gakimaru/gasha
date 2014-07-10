#pragma once
#ifndef GASHA_INCLUDED_SCOPED_DUAL_STACK_ALLOCATOR_INL
#define GASHA_INCLUDED_SCOPED_DUAL_STACK_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// scoped_dual_stack_allocator.inl
// スコープ双方向スタックアロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/scoped_dual_stack_allocator.h>//スコープ双方向スタックアロケータ【宣言部】

#include <utility>//C++11 std::forward
#include <stdio.h>//sprintf()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】sprintf を使用すると、error C4996 が発生する
//  error C4996: 'sprintf': This function or variable may be unsafe. Consider using strncpy_fast_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//スコープ双方向スタックアロケータクラス

//バッファの全体サイズ（バイト数）
template<class ALLOCATOR>
inline typename scopedDualStackAllocator<ALLOCATOR>::size_type scopedDualStackAllocator<ALLOCATOR>::maxSize() const
{
	return m_allocator.maxSize() - m_initSizeAsc - m_initSizeDesc;
}

//使用中のサイズ（バイト数）
template<class ALLOCATOR>
inline typename scopedDualStackAllocator<ALLOCATOR>::size_type scopedDualStackAllocator<ALLOCATOR>::size() const
{
	return m_allocator.size() - m_initSizeAsc - m_initSizeDesc;
}
//正順で使用中のサイズ（バイト数）
template<class ALLOCATOR>
inline typename scopedDualStackAllocator<ALLOCATOR>::size_type scopedDualStackAllocator<ALLOCATOR>::sizeAsc() const
{
	return m_allocator.sizeAsc() - m_initSizeAsc;
}
//逆順で使用中のサイズ（バイト数）
template<class ALLOCATOR>
inline typename scopedDualStackAllocator<ALLOCATOR>::size_type scopedDualStackAllocator<ALLOCATOR>::sizeDesc() const
{
	return m_allocator.sizeDesc() - m_initSizeDesc;
}

//残りサイズ（バイト数）
template<class ALLOCATOR>
inline typename scopedDualStackAllocator<ALLOCATOR>::size_type scopedDualStackAllocator<ALLOCATOR>::remain() const
{
	return maxSize() - size();
}

//アロケート中の数
template<class ALLOCATOR>
inline typename scopedDualStackAllocator<ALLOCATOR>::size_type scopedDualStackAllocator<ALLOCATOR>::count() const
{
	return m_allocator.count() - m_initCountAsc - m_initCountDesc;
}

//正順でアロケート中の数
template<class ALLOCATOR>
inline typename scopedDualStackAllocator<ALLOCATOR>::size_type scopedDualStackAllocator<ALLOCATOR>::countAsc() const
{
	return m_allocator.countAsc() - m_initCountAsc;
}

//逆順でアロケート中の数
template<class ALLOCATOR>
inline typename scopedDualStackAllocator<ALLOCATOR>::size_type scopedDualStackAllocator<ALLOCATOR>::countDesc() const
{
	return m_allocator.countDesc() - m_initCountDesc;
}

//現在のアロケート方向
template<class ALLOCATOR>
inline allocateOrder_t scopedDualStackAllocator<ALLOCATOR>::allocateOrder() const
{
	return m_allocator.allocateOrder();
}

//現在のアロケート方向を変更
template<class ALLOCATOR>
inline void scopedDualStackAllocator<ALLOCATOR>::setAllocateOrder(const allocateOrder_t order)
{
	return m_allocator.setAllocateOrder(order);
}

//現在のアロケート方向を逆にする
template<class ALLOCATOR>
inline void scopedDualStackAllocator<ALLOCATOR>::reversewAllocateOrder()
{
	m_allocator.reversewAllocateOrder();
}

//メモリ確保
template<class ALLOCATOR>
inline void* scopedDualStackAllocator<ALLOCATOR>::alloc(const std::size_t size, const std::size_t align)
{
	return m_allocator.alloc(size, align);
}
//※アロケート方向指定版
template<class ALLOCATOR>
inline void* scopedDualStackAllocator<ALLOCATOR>::allocOrdinal(const allocateOrder_t order, const std::size_t size, const std::size_t align)
{
	return m_allocator.allocOrdinal(order, size, align);
}

//メモリ解放
template<class ALLOCATOR>
inline bool scopedDualStackAllocator<ALLOCATOR>::free(void* p)
{
	return m_allocator.free(p);
}

//メモリ確保とコンストラクタ呼び出し
template<class ALLOCATOR>
template<typename T, typename...Tx>
T* scopedDualStackAllocator<ALLOCATOR>::newObj(Tx&&... args)
{
	return m_allocator.template newObj<T>(std::forward<Tx>(args)...);
}
//※配列用
template<class ALLOCATOR>
template<typename T, typename...Tx>
T* scopedDualStackAllocator<ALLOCATOR>::newArray(const std::size_t num, Tx&&... args)
{
	return m_allocator.template newArray<T>(std::forward<Tx>(args)...);
}
//※アロケート方向指定版
template<class ALLOCATOR>
template<typename T, typename...Tx>
T* scopedDualStackAllocator<ALLOCATOR>::newObjOrdinal(const allocateOrder_t order, Tx&&... args)
{
	return m_allocator.template newObjOrdinal<T>(order, std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<class ALLOCATOR>
template<typename T>
bool scopedDualStackAllocator<ALLOCATOR>::deleteObj(T* p)
{
	return m_allocator.deleteObj(p);
}
//※配列用
template<class ALLOCATOR>
template<typename T>
bool scopedDualStackAllocator<ALLOCATOR>::deleteArray(T* p, const std::size_t num)
{
	return m_allocator.deleteArray(p, num);
}
//※配列用アロケート方向指定版
template<class ALLOCATOR>
template<typename T, typename...Tx>
T* scopedDualStackAllocator<ALLOCATOR>::newArrayOrdinal(const allocateOrder_t order, const std::size_t num, Tx&&... args)
{
	return m_allocator.template newArrayOrdinal<T>(order, num, std::forward<Tx>(args)...);
}

//使用中のサイズを指定位置に戻す
//※位置指定版
template<class ALLOCATOR>
inline bool scopedDualStackAllocator<ALLOCATOR>::rewind(const typename scopedDualStackAllocator<ALLOCATOR>::size_type pos)
{
	if (m_initAllocateOrder == ALLOC_ASC)
		return m_allocator.rewind(m_initSizeAsc + pos);
	else//if(m_initAllocateOrder == ALLOC_DESC)
		return m_allocator.rewind(m_initSizeDesc + pos);
}
//※位置指定とアロケート方向指定版
template<class ALLOCATOR>
inline bool scopedDualStackAllocator<ALLOCATOR>::rewindOrdinal(const allocateOrder_t order, const typename scopedDualStackAllocator<ALLOCATOR>::size_type pos)
{
	if(order == ALLOC_ASC)
		return m_allocator.rewindOrdinal(order, m_initSizeAsc + pos);
	else//if(order == ALLOC_DESC)
		return m_allocator.rewindOrdinal(order, m_initSizeDesc + pos);
}
//※ポインタ指定版
template<class ALLOCATOR>
inline bool  scopedDualStackAllocator<ALLOCATOR>::rewind(void* p)
{
	return m_allocator.rewind(p);
}

//メモリクリア
//※全て初期状態にする
template<class ALLOCATOR>
inline void scopedDualStackAllocator<ALLOCATOR>::clearAll()
{
	m_allocator.resetSizeAndCount(m_initAllocateOrder, m_initSizeAsc, m_initSizeDesc, m_initCountAsc, m_initCountDesc);
}
//※現在のアロケート方向のみ
template<class ALLOCATOR>
inline void scopedDualStackAllocator<ALLOCATOR>::clear()
{
	if(allocateOrder() == ALLOC_ASC)
		m_allocator.resetSizeAndCount(ALLOC_ASC, m_initSizeAsc, m_initCountAsc);
	else//if(allocateOrder() == ALLOC_DESC)
		m_allocator.resetSizeAndCount(ALLOC_DESC, m_initSizeDesc, m_initCountDesc);
}
//※アロケート方向指定
template<class ALLOCATOR>
inline void scopedDualStackAllocator<ALLOCATOR>::clearOrdinal(const allocateOrder_t order)
{
	if(order == ALLOC_ASC)
		m_allocator.resetSizeAndCount(ALLOC_ASC, m_initSizeAsc, m_initCountAsc);
	else//if(order == ALLOC_DESC)
		m_allocator.resetSizeAndCount(ALLOC_DESC, m_initSizeDesc, m_initCountDesc);
}

//デバッグ情報作成
template<class ALLOCATOR>
inline std::size_t scopedDualStackAllocator<ALLOCATOR>::debugInfo(char* message)
{
	std::size_t size = 0;
	size += sprintf(message + size, "----- Debug Info for scopedDualStackAllocator -----\n");
	size += sprintf(message + size, "maxSize=%d, size=%d, sizeAsc=%d, sizeDesc=%d, remain=%d, count=%d, countAsc=%d, countDesc=%d, allocate-order=%s (buff=%p, initAllocateOrder=%s, initSizeAsc=%d, initSizeDesc=%d, initCountAsc=%d, initCountDesc=%d)\n", maxSize(), this->size(), sizeAsc(), sizeDesc(), remain(), count(), countAsc(), countDesc(), allocateOrder() == ALLOC_ASC ? "ASC" : "DESC", m_allocator.buff(), m_initAllocateOrder == ALLOC_ASC ? "ASC" : "DESC", m_initSizeAsc, m_initSizeDesc, m_initCountAsc, m_initCountDesc);
	size += sprintf(message + size, "----------\n");
	return size;
}

//コンストラクタ
template<class ALLOCATOR>
inline scopedDualStackAllocator<ALLOCATOR>::scopedDualStackAllocator(allocator_type& allocator) :
	m_allocator(allocator)
{
	m_allocator.getSizeAndCount(m_initAllocateOrder, m_initSizeAsc, m_initSizeDesc, m_initCountAsc, m_initCountDesc);
}

//デストラクタ
template<class ALLOCATOR>
inline scopedDualStackAllocator<ALLOCATOR>::~scopedDualStackAllocator()
{
	clearAll();
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_SCOPED_DUAL_STACK_ALLOCATOR_INL

// End of file

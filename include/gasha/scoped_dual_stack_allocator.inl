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

#include <gasha/string.h>//文字列処理：spprintf()

#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//スコープ双方向スタックアロケータクラス

//名前
template<class ALLOCATOR>
inline const char* scopedDualStackAllocator<ALLOCATOR>::name() const
{
	return m_allocator.name();
}

//実装モード
template<class ALLOCATOR>
inline const char* scopedDualStackAllocator<ALLOCATOR>::mode() const
{
	return m_allocator.mode();
}

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
inline allocationOrder_t scopedDualStackAllocator<ALLOCATOR>::allocationOrder() const
{
	return m_allocator.allocationOrder();
}

//現在のアロケート方向を変更
template<class ALLOCATOR>
inline void scopedDualStackAllocator<ALLOCATOR>::setAllocateOrder(const allocationOrder_t order)
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
inline void* scopedDualStackAllocator<ALLOCATOR>::allocOrd(const allocationOrder_t order, const std::size_t size, const std::size_t align)
{
	return m_allocator.allocOrd(order, size, align);
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
	return m_allocator.template newArray<T>(num, std::forward<Tx>(args)...);
}
//※アロケート方向指定版
template<class ALLOCATOR>
template<typename T, typename...Tx>
T* scopedDualStackAllocator<ALLOCATOR>::newObjOrd(const allocationOrder_t order, Tx&&... args)
{
	return m_allocator.template newObjOrd<T>(order, std::forward<Tx>(args)...);
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
T* scopedDualStackAllocator<ALLOCATOR>::newArrayOrd(const allocationOrder_t order, const std::size_t num, Tx&&... args)
{
	return m_allocator.template newArrayOrd<T>(order, num, std::forward<Tx>(args)...);
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
inline bool scopedDualStackAllocator<ALLOCATOR>::rewindOrd(const allocationOrder_t order, const typename scopedDualStackAllocator<ALLOCATOR>::size_type pos)
{
	if(order == ALLOC_ASC)
		return m_allocator.rewindOrd(order, m_initSizeAsc + pos);
	else//if(order == ALLOC_DESC)
		return m_allocator.rewindOrd(order, m_initSizeDesc + pos);
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
	m_allocator.resetSizeAndCount(allocationOrder(), m_initSizeAsc, m_initSizeDesc, m_initCountAsc, m_initCountDesc);
}
//※現在のアロケート方向のみ
template<class ALLOCATOR>
inline void scopedDualStackAllocator<ALLOCATOR>::clear()
{
	if(allocationOrder() == ALLOC_ASC)
		m_allocator.resetSizeAndCount(ALLOC_ASC, m_initSizeAsc, m_initCountAsc);
	else//if(allocationOrder() == ALLOC_DESC)
		m_allocator.resetSizeAndCount(ALLOC_DESC, m_initSizeDesc, m_initCountDesc);
}
//※アロケート方向指定
template<class ALLOCATOR>
inline void scopedDualStackAllocator<ALLOCATOR>::clearOrd(const allocationOrder_t order)
{
	if(order == ALLOC_ASC)
		m_allocator.resetSizeAndCount(ALLOC_ASC, m_initSizeAsc, m_initCountAsc);
	else//if(order == ALLOC_DESC)
		m_allocator.resetSizeAndCount(ALLOC_DESC, m_initSizeDesc, m_initCountDesc);
}

//デバッグ情報作成
template<class ALLOCATOR>
std::size_t scopedDualStackAllocator<ALLOCATOR>::debugInfo(char* message, const std::size_t max_size) const
{
	std::size_t message_len = 0;
	GASHA_ spprintf(message, max_size, message_len, "----- Debug-info for scopedDualStackAllocator -----\n");
	GASHA_ spprintf(message, max_size, message_len, "maxSize=%d, size=%d(ASC=%d,DESC=%d), remain=%d, count=%d(ASC=%d,DESC=%d), order=%s (INIT: buff=%p, order=%s, size=%d(ASC=%d,DESC=%d), count=%d(ASC=%d,DESC=%d))\n", maxSize(), this->size(), sizeAsc(), sizeDesc(), remain(), count(), countAsc(), countDesc(), allocationOrder() == ALLOC_ASC ? "ASC" : "DESC", m_allocator.buff(), m_initAllocateOrder == ALLOC_ASC ? "ASC" : "DESC", m_initSizeAsc + m_initSizeDesc, m_initSizeAsc, m_initSizeDesc, m_initCountAsc + m_initCountDesc, m_initCountAsc, m_initCountDesc);
	GASHA_ spprintf(message, max_size, message_len, "---------------------------------------------------");//最終行改行なし
	return message_len;
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
	m_allocator.resetSizeAndCount(m_initAllocateOrder, m_initSizeAsc, m_initSizeDesc, m_initCountAsc, m_initCountDesc);
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SCOPED_DUAL_STACK_ALLOCATOR_INL

// End of file

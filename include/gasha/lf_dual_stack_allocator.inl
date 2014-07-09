#pragma once
#ifndef GASHA_INCLUDED_LF_DUAL_STACK_ALLOCATOR_INL
#define GASHA_INCLUDED_LF_DUAL_STACK_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lf_dual_stack_allocator.inl
// ロックフリー双方向スタックアロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_dual_stack_allocator.h>//ロックフリー双方向スタックアロケータ【宣言部】

#include <utility>//C++11 std::forward
#include <stdio.h>//sprintf()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリー双方向スタックアロケータ補助クラス

//----------------------------------------
//ロックフリー双方向スタック自動クリア

//正順方向の自動クリア
template<class AUTO_CLEAR>
void lfDualStackAllocatorAutoClear::autoClearAsc(lfDualStackAllocator<AUTO_CLEAR>& allocator)
{
	while(true)
	{
		typename lfDualStackAllocator<AUTO_CLEAR>::size2_type now_size2 = allocator.m_size.load();
		if (allocator.m_allocatedCountAsc.load() > 0)
			return;
		typename lfDualStackAllocator<AUTO_CLEAR>::size_type now_size = allocator.sizeAsc(now_size2);
		const typename lfDualStackAllocator<AUTO_CLEAR>::size_type now_size_desc = allocator.sizeDesc(now_size2);
		const typename lfDualStackAllocator<AUTO_CLEAR>::size_type new_size = 0;
		const typename lfDualStackAllocator<AUTO_CLEAR>::size2_type new_size2 = allocator.size2(new_size, now_size_desc);
		if (allocator.m_size.compare_exchange_weak(now_size2, new_size2))//サイズのCAS
			return;
	}
}

//逆順方向の自動クリア
template<class AUTO_CLEAR>
void lfDualStackAllocatorAutoClear::autoClearDesc(lfDualStackAllocator<AUTO_CLEAR>& allocator)
{
	while(true)
	{
		typename lfDualStackAllocator<AUTO_CLEAR>::size2_type now_size2 = allocator.m_size.load();
		if (allocator.m_allocatedCountDesc.load() > 0)
			return;
		typename lfDualStackAllocator<AUTO_CLEAR>::size_type now_size = allocator.sizeDesc(now_size2);
		const typename lfDualStackAllocator<AUTO_CLEAR>::size_type now_size_asc = allocator.sizeAsc(now_size2);
		const typename lfDualStackAllocator<AUTO_CLEAR>::size_type new_size = 0;
		const typename lfDualStackAllocator<AUTO_CLEAR>::size2_type new_size2 = allocator.size2(now_size_asc, new_size);
		if (allocator.m_size.compare_exchange_weak(now_size2, new_size2))//サイズのCAS
			return;
	}
}

//----------------------------------------
//ロックフリー双方向スタック自動クリア（ダミー）

//正順方向の自動クリア
template<class AUTO_CLEAR>
inline void dummyLfDualStackAllocatorAutoClear::autoClearAsc(lfDualStackAllocator<AUTO_CLEAR>& allocator)
{
	//何もしない
}

//逆方向の自動クリア
template<class AUTO_CLEAR>
inline void dummyLfDualStackAllocatorAutoClear::autoClearDesc(lfDualStackAllocator<AUTO_CLEAR>& allocator)
{
	//何もしない
}

//--------------------------------------------------------------------------------
//ロックフリー双方向スタックアロケータクラス

//使用中のサイズ（バイト数） ※正順サイズと逆順サイズから取得
template<class AUTO_CLEAR>
inline typename lfDualStackAllocator<AUTO_CLEAR>::size2_type lfDualStackAllocator<AUTO_CLEAR>::size2(const typename lfDualStackAllocator<AUTO_CLEAR>::size_type size_asc, const typename lfDualStackAllocator<AUTO_CLEAR>::size_type size_desc) const
{
	return static_cast<size2_type>(size_desc) << 32 | static_cast<size2_type>(size_asc);
}

//正順で使用中のサイズ（バイト数） ※サイズから取得
template<class AUTO_CLEAR>
inline typename lfDualStackAllocator<AUTO_CLEAR>::size_type lfDualStackAllocator<AUTO_CLEAR>::sizeAsc(const typename lfDualStackAllocator<AUTO_CLEAR>::size2_type size) const
{
	return static_cast<size_type>(size & 0xffffffffllu);
}

//逆順で使用中のサイズ（バイト数） ※サイズから取得
template<class AUTO_CLEAR>
inline typename lfDualStackAllocator<AUTO_CLEAR>::size_type lfDualStackAllocator<AUTO_CLEAR>::sizeDesc(const typename lfDualStackAllocator<AUTO_CLEAR>::size2_type size) const
{
	return static_cast<size_type>((size >> 32)& 0xffffffffllu);
}

//使用中のサイズ（バイト数）
template<class AUTO_CLEAR>
inline typename lfDualStackAllocator<AUTO_CLEAR>::size_type lfDualStackAllocator<AUTO_CLEAR>::size() const
{
	const size2_type size = m_size.load();
	return sizeAsc(size) + sizeDesc(size);
}

//正順で使用中のサイズ（バイト数）
template<class AUTO_CLEAR>
inline typename lfDualStackAllocator<AUTO_CLEAR>::size_type lfDualStackAllocator<AUTO_CLEAR>::sizeAsc() const
{
	const size2_type size = m_size.load();
	return sizeAsc(size);
}

//逆順で使用中のサイズ（バイト数）
template<class AUTO_CLEAR>
inline typename lfDualStackAllocator<AUTO_CLEAR>::size_type lfDualStackAllocator<AUTO_CLEAR>::sizeDesc() const
{
	const size2_type size = m_size.load();
	return sizeDesc(size);
}
//残りサイズ（バイト数）
template<class AUTO_CLEAR>
inline typename lfDualStackAllocator<AUTO_CLEAR>::size_type lfDualStackAllocator<AUTO_CLEAR>::remain() const
{
	return m_maxSize - size();
}

//メモリ確保
template<class AUTO_CLEAR>
inline void* lfDualStackAllocator<AUTO_CLEAR>::alloc(const std::size_t size, const std::size_t align)
{
	return allocOrdinal(m_allocateOrder.load(), size, align);
}

//※アロケート方向指定版
template<class AUTO_CLEAR>
inline  void* lfDualStackAllocator<AUTO_CLEAR>::allocOrdinal(const allocateOrder_t order, const std::size_t size, const std::size_t align)
{
	if(order == ALLOC_ASC)
		return _allocAsc(size, align);
	else//if(order == ALLOC_DESC)
		return _allocDesc(size, align);
}

//メモリ解放
template<class AUTO_CLEAR>
inline bool lfDualStackAllocator<AUTO_CLEAR>::free(void* p)
{
	const allocateOrder_t order = isInUsingRange(p);//ポインタの所属を判定
	if(order == ALLOC_ASC)
		return _freeAsc(p);
	else if(order == ALLOC_DESC)
		return _freeDesc(p);

	return false;
}

//メモリ確保とコンストラクタ呼び出し
template<class AUTO_CLEAR>
template<typename T, typename...Tx>
inline T* lfDualStackAllocator<AUTO_CLEAR>::newObj(Tx&&... args)
{
	return this->template newObjOrdinal<T>(m_allocateOrder.load(), std::forward<Tx>(args)...);
}
//※アロケート方向指定版
template<class AUTO_CLEAR>
template<typename T, typename...Tx>
inline T* lfDualStackAllocator<AUTO_CLEAR>::newObjOrdinal(const allocateOrder_t order, Tx&&... args)
{
	void* p = allocOrdinal(order, sizeof(T), alignof(T));
	if (!p)
		return nullptr;
	return new(p)T(std::forward<Tx>(args)...);
}
//※配列用
template<class AUTO_CLEAR>
template<typename T, typename...Tx>
inline T* lfDualStackAllocator<AUTO_CLEAR>::newArray(const std::size_t num, Tx&&... args)
{
	return this->template newArrayOrdinal<T>(m_allocateOrder.load(), num, std::forward<Tx>(args)...);
}
//※配列用アロケート方向指定版
template<class AUTO_CLEAR>
template<typename T, typename...Tx>
T* lfDualStackAllocator<AUTO_CLEAR>::newArrayOrdinal(const allocateOrder_t order, const std::size_t num, Tx&&... args)
{
	void* p = allocOrdinal(order, sizeof(T) * num, alignof(T));
	if (!p)
		return nullptr;
	T* top_obj = nullptr;
	for (std::size_t i = 0; i < num; ++i)
	{
		T* obj = new(p)T(std::forward<Tx>(args)...);
		if (!top_obj)
			top_obj = obj;
		p = reinterpret_cast<void*>(reinterpret_cast<char*>(p) + sizeof(T));
	}
	return top_obj;
}

//メモリ解放とデストラクタ呼び出し
template<class AUTO_CLEAR>
template<typename T>
bool lfDualStackAllocator<AUTO_CLEAR>::deleteObj(T* p)
{
	const allocateOrder_t order = isInUsingRange(p);//ポインタの所属を判定
	if(order == ALLOC_UNKNOWN_ORDER)
		return false;
	p->~T();//デストラクタ呼び出し
	//operator delete(p, p);//（作法として）deleteオペレータ呼び出し
	if(order == ALLOC_ASC)
		return _freeAsc(p);
	else//if(order == ALLOC_DESC)
		return _freeDesc(p);
}
//※配列用
template<class AUTO_CLEAR>
template<typename T>
bool lfDualStackAllocator<AUTO_CLEAR>::deleteArray(T* p, const std::size_t num)
{
	const allocateOrder_t order = isInUsingRange(p);
	if(order == ALLOC_UNKNOWN_ORDER)//ポインタの所属を判定
		return false;
	T* obj = p;
	for (std::size_t i = 0; i < num; ++i, ++obj)
	{
		obj->~T();//デストラクタ呼び出し
		//operator delete(p, p);//（作法として）deleteオペレータ呼び出し
	}
	if(order == ALLOC_ASC)
		return _freeAsc(p);
	else//if(order == ALLOC_DESC)
		return _freeDesc(p);
}

//使用中のサイズを指定位置に戻す
//※位置指定版
template<class AUTO_CLEAR>
inline bool lfDualStackAllocator<AUTO_CLEAR>::rewind(const size_type pos)
{
	return rewindOrdinal(m_allocateOrder.load(), pos);
}
//※位置指定とアロケート方向指定版
template<class AUTO_CLEAR>
inline bool lfDualStackAllocator<AUTO_CLEAR>::rewindOrdinal(const allocateOrder_t order, const size_type pos)
{
	if(order == ALLOC_ASC)
		return _rewindAsc(m_buffRef + pos);
	else if(order == ALLOC_DESC)
		return _rewindDesc(m_buffRef + m_maxSize - pos);
	return false;
}
//※ポインタ指定版
template<class AUTO_CLEAR>
inline bool lfDualStackAllocator<AUTO_CLEAR>::rewind(void* p)
{
	const allocateOrder_t order = isInUsingRange(p);
	if(order == ALLOC_ASC)
		return _rewindAsc(p);
	else if(order == ALLOC_DESC)
		return _rewindDesc(p);
	return false;
}

//メモリクリア
template<class AUTO_CLEAR>
inline void lfDualStackAllocator<AUTO_CLEAR>::clearAll()
{
	_clearAsc();
	_clearDesc();
}
//※現在のアロケート方向のみ
template<class AUTO_CLEAR>
inline void lfDualStackAllocator<AUTO_CLEAR>::clear()
{
	return clearOrdinal(m_allocateOrder.load());
}
//※アロケート方向指定
template<class AUTO_CLEAR>
inline void lfDualStackAllocator<AUTO_CLEAR>::clearOrdinal(const allocateOrder_t order)
{
	if(order == ALLOC_ASC)
		return _clearAsc();
	else if(order == ALLOC_DESC)
		return _clearDesc();
}

//ポインタが範囲内か判定
template<class AUTO_CLEAR>
inline allocateOrder_t lfDualStackAllocator<AUTO_CLEAR>::isInUsingRange(void* p)
{
	if (p)
	{
		const size2_type size = m_size.load();
		const size_type size_asc = sizeAsc(size);
		const size_type size_desc = sizeDesc(size);
		if (p >= m_buffRef && p < m_buffRef + size_asc)//正順の範囲内
			return ALLOC_ASC;
		else if (p >= m_buffRef + m_maxSize - size_desc && p < m_buffRef + m_maxSize)//逆順の範囲内
			return ALLOC_DESC;
printf("!!!!! p=%p(%d, %d), size=(%u,%u), buff=%p, max_size=%d\n", p, reinterpret_cast<char*>(p)-m_buffRef, m_buffRef + m_maxSize - reinterpret_cast<char*>(p), size_asc, size_desc, m_buffRef, m_maxSize);
	}
	//範囲外のポインタ
#ifdef GASHA_LF_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
	static const bool IS_INVALID_POINTER = false;
	assert(IS_INVALID_POINTER);
#endif//GASHA_LF_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
	return ALLOC_UNKNOWN_ORDER;
}

//コンストラクタ
template<class AUTO_CLEAR>
inline lfDualStackAllocator<AUTO_CLEAR>::lfDualStackAllocator(void* buff, const std::size_t max_size) :
	m_buffRef(reinterpret_cast<char*>(buff)),
	m_maxSize(static_cast<size_type>(max_size)),
	m_size(0),
	m_allocatedCountAsc(0),
	m_allocatedCountDesc(0)
{
	assert(m_buffRef != nullptr);
	assert(m_maxSize > 0);
}
template<class AUTO_CLEAR>
template<typename T>
inline lfDualStackAllocator<AUTO_CLEAR>::lfDualStackAllocator(T* buff, const std::size_t num) :
	lfDualStackAllocator(reinterpret_cast<void*>(buff), sizeof(T) * num)//C++11 委譲コンストラクタ
{}
template<class AUTO_CLEAR>
template<typename T, std::size_t N>
inline lfDualStackAllocator<AUTO_CLEAR>::lfDualStackAllocator(T(&buff)[N]) :
lfDualStackAllocator(reinterpret_cast<void*>(buff), sizeof(buff))//C++11 委譲コンストラクタ
{}

//デストラクタ
template<class AUTO_CLEAR>
inline lfDualStackAllocator<AUTO_CLEAR>::~lfDualStackAllocator()
{}

//--------------------------------------------------------------------------------
//バッファ付きロックフリー双方向スタックアロケータクラス

//コンストラクタ
template<std::size_t _MAX_SIZE, class AUTO_CLEAR>
inline lfDualStackAllocator_withBuff<_MAX_SIZE, AUTO_CLEAR>::lfDualStackAllocator_withBuff() :
lfDualStackAllocator<AUTO_CLEAR>(m_buff, MAX_SIZE)
{}

//デストラクタ
template<std::size_t _MAX_SIZE, class AUTO_CLEAR>
inline lfDualStackAllocator_withBuff<_MAX_SIZE, AUTO_CLEAR>::~lfDualStackAllocator_withBuff()
{}

//--------------------------------------------------------------------------------
//バッファ付きロックフリー双方向スタックアロケータクラス
//※型指定版

//メモリ確保とコンストラクタ呼び出し
template<typename T, std::size_t _NUM, class AUTO_CLEAR>
template<typename... Tx>
inline typename lfDualStackAllocator_withType<T, _NUM, AUTO_CLEAR>::value_type* lfDualStackAllocator_withType<T, _NUM, AUTO_CLEAR>::newDefault(Tx&&... args)
{
	return this->template newObj<value_type>(std::forward<Tx>(args)...);
}
//※アロケート方向指定版
template<typename T, std::size_t _NUM, class AUTO_CLEAR>
template<typename... Tx>
inline typename lfDualStackAllocator_withType<T, _NUM, AUTO_CLEAR>::value_type* lfDualStackAllocator_withType<T, _NUM, AUTO_CLEAR>::newDefaultOrdinal(const allocateOrder_t order, Tx&&... args)
{
	return this->template newObjOrdinal<value_type>(order, std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<typename T, std::size_t _NUM, class AUTO_CLEAR>
inline bool lfDualStackAllocator_withType<T, _NUM, AUTO_CLEAR>::deleteDefault(typename lfDualStackAllocator_withType<T, _NUM, AUTO_CLEAR>::value_type*& p)
{
	return this->template deleteObj<value_type>(p);
}

//コンストラクタ
template<typename T, std::size_t _NUM, class AUTO_CLEAR>
inline lfDualStackAllocator_withType<T, _NUM, AUTO_CLEAR>::lfDualStackAllocator_withType() :
	lfDualStackAllocator<AUTO_CLEAR>(reinterpret_cast<void*>(m_buff), MAX_SIZE)
{}

//デストラクタ
template<typename T, std::size_t _NUM, class AUTO_CLEAR>
inline lfDualStackAllocator_withType<T, _NUM, AUTO_CLEAR>::~lfDualStackAllocator_withType()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_LF_DUAL_STACK_ALLOCATOR_INL

// End of file

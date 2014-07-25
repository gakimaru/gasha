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
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_dual_stack_allocator.h>//ロックフリー双方向スタックアロケータ【宣言部】

#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し
#include <gasha/simple_assert.h>//シンプルアサーション

#include <utility>//C++11 std::forward

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
		if (allocator.m_countAsc.load() > 0)
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
		if (allocator.m_countDesc.load() > 0)
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
	return allocOrd(m_allocateOrder.load(), size, align);
}

//※アロケート方向指定版
template<class AUTO_CLEAR>
inline  void* lfDualStackAllocator<AUTO_CLEAR>::allocOrd(const allocationOrder_t order, const std::size_t size, const std::size_t align)
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
	if (!p)//nullptrの解放は常に成功扱い
		return true;
	const allocationOrder_t order = isInUsingRange(p);//ポインタの所属を判定
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
	return this->template newObjOrd<T>(m_allocateOrder.load(), std::forward<Tx>(args)...);
}
//※アロケート方向指定版
template<class AUTO_CLEAR>
template<typename T, typename...Tx>
inline T* lfDualStackAllocator<AUTO_CLEAR>::newObjOrd(const allocationOrder_t order, Tx&&... args)
{
	void* p = allocOrd(order, sizeof(T), alignof(T));
	if (!p)
		return nullptr;
	return GASHA_ callConstructor<T>(p, std::forward<Tx>(args)...);
}
//※配列用
template<class AUTO_CLEAR>
template<typename T, typename...Tx>
inline T* lfDualStackAllocator<AUTO_CLEAR>::newArray(const std::size_t num, Tx&&... args)
{
	return this->template newArrayOrd<T>(m_allocateOrder.load(), num, std::forward<Tx>(args)...);
}
//※配列用アロケート方向指定版
template<class AUTO_CLEAR>
template<typename T, typename...Tx>
T* lfDualStackAllocator<AUTO_CLEAR>::newArrayOrd(const allocationOrder_t order, const std::size_t num, Tx&&... args)
{
	void* p = allocOrd(order, sizeof(T) * num, alignof(T));
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
template<class AUTO_CLEAR>
template<typename T>
bool lfDualStackAllocator<AUTO_CLEAR>::deleteObj(T* p)
{
	if (!p)//nullptrの解放は常に成功扱い
		return true;
	const allocationOrder_t order = isInUsingRange(p);//ポインタの所属を判定
	if(order == ALLOC_UNKNOWN_ORDER)
		return false;
	GASHA_ callDestructor(p);//デストラクタ呼び出し
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
	if (!p)//nullptrの解放は常に成功扱い
		return true;
	const allocationOrder_t order = isInUsingRange(p);
	if(order == ALLOC_UNKNOWN_ORDER)//ポインタの所属を判定
		return false;
	T* obj = p;
	for (std::size_t i = 0; i < num; ++i, ++obj)
	{
		GASHA_ callDestructor(obj);//デストラクタ呼び出し
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
	return rewindOrd(m_allocateOrder.load(), pos);
}
//※位置指定とアロケート方向指定版
template<class AUTO_CLEAR>
inline bool lfDualStackAllocator<AUTO_CLEAR>::rewindOrd(const allocationOrder_t order, const size_type pos)
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
	const allocationOrder_t order = isInUsingRange(p);
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
	return clearOrd(m_allocateOrder.load());
}
//※アロケート方向指定
template<class AUTO_CLEAR>
inline void lfDualStackAllocator<AUTO_CLEAR>::clearOrd(const allocationOrder_t order)
{
	if(order == ALLOC_ASC)
		return _clearAsc();
	else if(order == ALLOC_DESC)
		return _clearDesc();
}
//使用中のサイズと数をリセット
template<class AUTO_CLEAR>
bool lfDualStackAllocator<AUTO_CLEAR>::resetSizeAndCount(const typename lfDualStackAllocator<AUTO_CLEAR>::size_type size, const typename lfDualStackAllocator<AUTO_CLEAR>::size_type count)
{
	return resetSizeAndCount(m_allocateOrder.load(), size, count);
}

//ポインタが範囲内か判定
template<class AUTO_CLEAR>
inline allocationOrder_t lfDualStackAllocator<AUTO_CLEAR>::isInUsingRange(void* p)
{
#ifdef GASHA_LF_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
	GASHA_SIMPLE_ASSERT(p != nullptr, "Pointer is nullptr.");
#endif//GASHA_LF_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
	if (p)
	{
		const size2_type size = m_size.load();
		const size_type size_asc = sizeAsc(size);
		const size_type size_desc = sizeDesc(size);
		if (p >= m_buffRef && p < m_buffRef + size_asc)//正順の範囲内
			return ALLOC_ASC;
		else if (p >= m_buffRef + m_maxSize - size_desc && p < m_buffRef + m_maxSize)//逆順の範囲内
			return ALLOC_DESC;
	}
	//範囲外のポインタ
	return ALLOC_UNKNOWN_ORDER;
}

//コンストラクタ
template<class AUTO_CLEAR>
inline lfDualStackAllocator<AUTO_CLEAR>::lfDualStackAllocator(void* buff, const std::size_t max_size) :
	m_buffRef(reinterpret_cast<char*>(buff)),
	m_maxSize(static_cast<size_type>(max_size)),
	m_size(0),
	m_countAsc(0),
	m_countDesc(0),
	m_allocateOrder(ALLOC_ASC)
{
#ifdef GASHA_LF_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
	GASHA_SIMPLE_ASSERT(m_buffRef != nullptr, "buff is nullptr.");
	GASHA_SIMPLE_ASSERT(m_maxSize > 0, "max_size is zero.");
#endif//GASHA_LF_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
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
inline typename lfDualStackAllocator_withType<T, _NUM, AUTO_CLEAR>::value_type* lfDualStackAllocator_withType<T, _NUM, AUTO_CLEAR>::newDefaultOrd(const allocationOrder_t order, Tx&&... args)
{
	return this->template newObjOrd<value_type>(order, std::forward<Tx>(args)...);
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

#endif//GASHA_INCLUDED_LF_DUAL_STACK_ALLOCATOR_INL

// End of file

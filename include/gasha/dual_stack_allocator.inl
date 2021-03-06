﻿#pragma once
#ifndef GASHA_INCLUDED_DUAL_STACK_ALLOCATOR_INL
#define GASHA_INCLUDED_DUAL_STACK_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// dual_stack_allocator.inl
// 双方向スタックアロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dual_stack_allocator.h>//双方向スタックアロケータ【宣言部】

#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し
#include <gasha/simple_assert.h>//シンプルアサーション

#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//双方向スタックアロケータ補助クラス

//----------------------------------------
//双方向スタック自動クリア

//正順方向の自動クリア
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline void dualStackAllocatorAutoClear::autoClearAsc(dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>& allocator)
{
	if (allocator.m_countAsc == 0)
		allocator.m_sizeAsc = 0;
}

//逆順方向の自動クリア
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline void dualStackAllocatorAutoClear::autoClearDesc(dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>& allocator)
{
	if (allocator.m_countDesc == 0)
		allocator.m_sizeDesc = 0;
}

//----------------------------------------
//双方向スタック自動クリア（ダミー）

//正順方向の自動クリア
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline void dummyDualStackAllocatorAutoClear::autoClearAsc(dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>& allocator)
{
	//何もしない
}

//逆方向の自動クリア
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline void dummyDualStackAllocatorAutoClear::autoClearDesc(dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>& allocator)
{
	//何もしない
}

//--------------------------------------------------------------------------------
//双方向スタックアロケータクラス

//メモリ確保
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline void* dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::alloc(const std::size_t size, const std::size_t align)
{
	return allocOrd(m_allocateOrder, size, align);
}

//※アロケート方向指定版
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline  void* dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::allocOrd(const allocationOrder_t order, const std::size_t size, const std::size_t align)
{
	if(order == ALLOC_ASC)
		return _allocAsc(size, align);
	else//if(order == ALLOC_DESC)
		return _allocDesc(size, align);
}

//メモリ解放
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline bool dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::free(void* p)
{
	if (!p)//nullptrの解放は常に成功扱い
		return true;
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	const allocationOrder_t order = isInUsingRange(p);//ポインタの所属を判定
	if(order == ALLOC_ASC)
		return _freeAsc(p);
	else if(order == ALLOC_DESC)
		return _freeDesc(p);
	else
		return false;
}

//メモリ確保とコンストラクタ呼び出し
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
template<typename T, typename...Tx>
inline T* dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::newObj(Tx&&... args)
{
	return this->template newObjOrd<T>(m_allocateOrder, std::forward<Tx>(args)...);
}
//※アロケート方向指定版
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
template<typename T, typename...Tx>
inline T* dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::newObjOrd(const allocationOrder_t order, Tx&&... args)
{
	void* p = allocOrd(order, sizeof(T), alignof(T));
	if (!p)
		return nullptr;
	return GASHA_ callConstructor<T>(p, std::forward<Tx>(args)...);
}
//※配列用
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
template<typename T, typename...Tx>
inline T* dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::newArray(const std::size_t num, Tx&&... args)
{
	return this->template newArrayOrd<T>(m_allocateOrder, num, std::forward<Tx>(args)...);
}
//※配列用アロケート方向指定版
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
template<typename T, typename...Tx>
T* dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::newArrayOrd(const allocationOrder_t order, const std::size_t num, Tx&&... args)
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
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
template<typename T>
bool dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::deleteObj(T* p)
{
	if (!p)//nullptrの解放は常に成功扱い
		return true;
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
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
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
template<typename T>
bool dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::deleteArray(T* p, const std::size_t num)
{
	if (!p)//nullptrの解放は常に成功扱い
		return true;
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
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
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline bool dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::rewind(const size_type pos)
{
	return rewindOrd(m_allocateOrder, pos);
}
//※位置指定とアロケート方向指定版
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline bool dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::rewindOrd(const allocationOrder_t order, const size_type pos)
{
	if(order == ALLOC_ASC)
		return _rewindAsc(m_buffRef + pos);
	else if(order == ALLOC_DESC)
		return _rewindDesc(m_buffRef + m_maxSize - pos);
	else
		return false;
}
//※ポインタ指定版
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline bool dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::rewind(void* p)
{
	const allocationOrder_t order = isInUsingRange(p);
	if(order == ALLOC_ASC)
		return _rewindAsc(p);
	else if(order == ALLOC_DESC)
		return _rewindDesc(p);
	else
		return false;
}

//メモリクリア
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline void dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::clearAll()
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	//使用中のサイズとメモリ確保数を更新
	m_sizeAsc = 0;
	m_sizeDesc = 0;
	m_countAsc = 0;
	m_countDesc = 0;
}
//※現在のアロケート方向のみ
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline void dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::clear()
{
	return clearOrd(m_allocateOrder);
}
//※アロケート方向指定
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline void dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::clearOrd(const allocationOrder_t order)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if(order == ALLOC_ASC)
		return _clearAsc();
	else if(order == ALLOC_DESC)
		return _clearDesc();
}

//使用中のサイズと数をリセット
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline bool dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::resetSizeAndCount(const typename dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::size_type size, const typename dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::size_type count)
{
	return resetSizeAndCount(m_allocateOrder, size, count);
}

//正順のメモリクリア
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline void dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::_clearAsc()
{
	//使用中のサイズとメモリ確保数を更新
	m_sizeAsc = 0;
	m_countAsc = 0;
}
//逆順のメモリクリア
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline void dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::_clearDesc()
{
	//使用中のサイズとメモリ確保数を更新
	m_sizeDesc = 0;
	m_countDesc = 0;
}

//ポインタが範囲内か判定
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline allocationOrder_t dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::isInUsingRange(void* p)
{
#ifdef GASHA_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
	GASHA_SIMPLE_ASSERT(p != nullptr, "Pointer is nullptr.");
#endif//GASHA_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
	if (p)
	{
		if (p >= m_buffRef && p < m_buffRef + m_sizeAsc)//正順の範囲内
			return ALLOC_ASC;
		else if (p >= m_buffRef + m_maxSize - m_sizeDesc && p < m_buffRef + m_maxSize)//逆順の範囲内
			return ALLOC_DESC;
	}
	//範囲外のポインタ
	return ALLOC_UNKNOWN_ORDER;
}

//コンストラクタ
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::dualStackAllocator(void* buff, const std::size_t max_size) :
	m_buffRef(reinterpret_cast<char*>(buff)),
	m_maxSize(static_cast<size_type>(max_size)),
	m_sizeAsc(0),
	m_sizeDesc(0),
	m_countAsc(0),
	m_countDesc(0)
{
#ifdef GASHA_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
	GASHA_SIMPLE_ASSERT(m_buffRef != nullptr, "buff is nullptr.");
	GASHA_SIMPLE_ASSERT(m_maxSize > 0, "max_size is zero.");
#endif//GASHA_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
}
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
template<typename T>
inline dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::dualStackAllocator(T* buff, const std::size_t num) :
	dualStackAllocator(reinterpret_cast<void*>(buff), sizeof(T) * num)//C++11 委譲コンストラクタ
{}
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
template<typename T, std::size_t N>
inline dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::dualStackAllocator(T(&buff)[N]) :
dualStackAllocator(reinterpret_cast<void*>(buff), sizeof(buff))//C++11 委譲コンストラクタ
{}

//デストラクタ
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::~dualStackAllocator()
{}

//--------------------------------------------------------------------------------
//バッファ付き双方向スタックアロケータクラス

//コンストラクタ
template<std::size_t _MAX_SIZE, class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline dualStackAllocator_withBuff<_MAX_SIZE, LOCK_POLICY, AUTO_CLEAR_POLICY>::dualStackAllocator_withBuff() :
dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>(m_buff, MAX_SIZE)
{}

//デストラクタ
template<std::size_t _MAX_SIZE, class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline dualStackAllocator_withBuff<_MAX_SIZE, LOCK_POLICY, AUTO_CLEAR_POLICY>::~dualStackAllocator_withBuff()
{}

//--------------------------------------------------------------------------------
//バッファ付き双方向スタックアロケータクラス
//※型指定版

//メモリ確保とコンストラクタ呼び出し
template<typename T, std::size_t _NUM, class LOCK_POLICY, class AUTO_CLEAR_POLICY>
template<typename... Tx>
inline typename dualStackAllocator_withType<T, _NUM, LOCK_POLICY, AUTO_CLEAR_POLICY>::value_type* dualStackAllocator_withType<T, _NUM, LOCK_POLICY, AUTO_CLEAR_POLICY>::newDefault(Tx&&... args)
{
	return this->template newObj<value_type>(std::forward<Tx>(args)...);
}
//※アロケート方向指定版
template<typename T, std::size_t _NUM, class LOCK_POLICY, class AUTO_CLEAR_POLICY>
template<typename... Tx>
inline typename dualStackAllocator_withType<T, _NUM, LOCK_POLICY, AUTO_CLEAR_POLICY>::value_type* dualStackAllocator_withType<T, _NUM, LOCK_POLICY, AUTO_CLEAR_POLICY>::newDefaultOrd(const allocationOrder_t order, Tx&&... args)
{
	return this->template newObjOrd<value_type>(order, std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<typename T, std::size_t _NUM, class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline bool dualStackAllocator_withType<T, _NUM, LOCK_POLICY, AUTO_CLEAR_POLICY>::deleteDefault(typename dualStackAllocator_withType<T, _NUM, LOCK_POLICY, AUTO_CLEAR_POLICY>::value_type*& p)
{
	return this->template deleteObj<value_type>(p);
}

//コンストラクタ
template<typename T, std::size_t _NUM, class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline dualStackAllocator_withType<T, _NUM, LOCK_POLICY, AUTO_CLEAR_POLICY>::dualStackAllocator_withType() :
	dualStackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>(reinterpret_cast<void*>(m_buff), MAX_SIZE)
{}

//デストラクタ
template<typename T, std::size_t _NUM, class LOCK_POLICY, class AUTO_CLEAR_POLICY>
inline dualStackAllocator_withType<T, _NUM, LOCK_POLICY, AUTO_CLEAR_POLICY>::~dualStackAllocator_withType()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_DUAL_STACK_ALLOCATOR_INL

// End of file

#pragma once
#ifndef GASHA_INCLUDED_DUAL_STACK_ALLOCATOR_INL
#define GASHA_INCLUDED_DUAL_STACK_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// dual_stack_allocator.inl
// 双方向スタックアロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dual_stack_allocator.h>//双方向スタックアロケータ【宣言部】

#include <utility>//C++11 std::forward

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//双方向スタックアロケータ補助クラス

//----------------------------------------
//双方向スタック自動クリア

//正順方向の自動クリア
template<class LOCK_TYPE, class AUTO_CLEAR>
inline void dualStackAllocatorAutoClear::autoClearAsc(dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>& allocator)
{
	if (allocator.m_countAsc == 0)
	{
		allocator.m_size -= allocator.m_sizeAsc;
		allocator.m_sizeAsc = 0;
	}
}

//逆順方向の自動クリア
template<class LOCK_TYPE, class AUTO_CLEAR>
inline void dualStackAllocatorAutoClear::autoClearDesc(dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>& allocator)
{
	if (allocator.m_countDesc == 0)
	{
		allocator.m_size -= allocator.m_sizeDesc;
		allocator.m_sizeDesc = 0;
	}
}

//----------------------------------------
//双方向スタック自動クリア（ダミー）

//正順方向の自動クリア
template<class LOCK_TYPE, class AUTO_CLEAR>
inline void dummyDualStackAllocatorAutoClear::autoClearAsc(dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>& allocator)
{
	//何もしない
}

//逆方向の自動クリア
template<class LOCK_TYPE, class AUTO_CLEAR>
inline void dummyDualStackAllocatorAutoClear::autoClearDesc(dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>& allocator)
{
	//何もしない
}

//--------------------------------------------------------------------------------
//双方向スタックアロケータクラス

//メモリ確保
template<class LOCK_TYPE, class AUTO_CLEAR>
inline void* dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::alloc(const std::size_t size, const std::size_t align)
{
	return allocOrdinal(m_allocateOrder, size, align);
}

//※アロケート方向指定版
template<class LOCK_TYPE, class AUTO_CLEAR>
inline  void* dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::allocOrdinal(const allocateOrder_t order, const std::size_t size, const std::size_t align)
{
	if(order == ALLOC_ASC)
		return _allocAsc(size, align);
	else//if(order == ALLOC_DESC)
		return _allocDesc(size, align);
}

//メモリ解放
template<class LOCK_TYPE, class AUTO_CLEAR>
inline bool dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::free(void* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	const allocateOrder_t order = isInUsingRange(p);//ポインタの所属を判定
	if(order == ALLOC_ASC)
		return _freeAsc(p);
	else if(order == ALLOC_DESC)
		return _freeDesc(p);
	else
		return false;
}

//メモリ確保とコンストラクタ呼び出し
template<class LOCK_TYPE, class AUTO_CLEAR>
template<typename T, typename...Tx>
inline T* dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::newObj(Tx&&... args)
{
	return this->template newObjOrdinal<T>(m_allocateOrder, std::forward<Tx>(args)...);
}
//※アロケート方向指定版
template<class LOCK_TYPE, class AUTO_CLEAR>
template<typename T, typename...Tx>
inline T* dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::newObjOrdinal(const allocateOrder_t order, Tx&&... args)
{
	void* p = allocOrdinal(order, sizeof(T), alignof(T));
	if (!p)
		return nullptr;
	return new(p)T(std::forward<Tx>(args)...);
}
//※配列用
template<class LOCK_TYPE, class AUTO_CLEAR>
template<typename T, typename...Tx>
inline T* dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::newArray(const std::size_t num, Tx&&... args)
{
	return this->template newArrayOrdinal<T>(m_allocateOrder, num, std::forward<Tx>(args)...);
}
//※配列用アロケート方向指定版
template<class LOCK_TYPE, class AUTO_CLEAR>
template<typename T, typename...Tx>
T* dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::newArrayOrdinal(const allocateOrder_t order, const std::size_t num, Tx&&... args)
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
template<class LOCK_TYPE, class AUTO_CLEAR>
template<typename T>
bool dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::deleteObj(T* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
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
template<class LOCK_TYPE, class AUTO_CLEAR>
template<typename T>
bool dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::deleteArray(T* p, const std::size_t num)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
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
template<class LOCK_TYPE, class AUTO_CLEAR>
inline bool dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::rewind(const size_type pos)
{
	return rewindOrdinal(m_allocateOrder, pos);
}
//※位置指定とアロケート方向指定版
template<class LOCK_TYPE, class AUTO_CLEAR>
inline bool dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::rewindOrdinal(const allocateOrder_t order, const size_type pos)
{
	if(order == ALLOC_ASC)
		return _rewindAsc(m_buffRef + pos);
	else if(order == ALLOC_DESC)
		return _rewindDesc(m_buffRef + m_maxSize - pos);
	else
		return false;
}
//※ポインタ指定版
template<class LOCK_TYPE, class AUTO_CLEAR>
inline bool dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::rewind(void* p)
{
	const allocateOrder_t order = isInUsingRange(p);
	if(order == ALLOC_ASC)
		return _rewindAsc(p);
	else if(order == ALLOC_DESC)
		return _rewindDesc(p);
	else
		return false;
}

//メモリクリア
template<class LOCK_TYPE, class AUTO_CLEAR>
inline void dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::clearAll()
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	//使用中のサイズとメモリ確保数を更新
	m_size = 0;
	m_sizeAsc = 0;
	m_sizeDesc = 0;
	m_countAsc = 0;
	m_countDesc = 0;
}
//※現在のアロケート方向のみ
template<class LOCK_TYPE, class AUTO_CLEAR>
inline void dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::clear()
{
	return clearOrdinal(m_allocateOrder);
}
//※アロケート方向指定
template<class LOCK_TYPE, class AUTO_CLEAR>
inline void dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::clearOrdinal(const allocateOrder_t order)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if(order == ALLOC_ASC)
		return _clearAsc();
	else if(order == ALLOC_DESC)
		return _clearDesc();
}

//使用中のサイズと数をリセット
template<class LOCK_TYPE, class AUTO_CLEAR>
inline bool dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::resetSizeAndCount(const typename dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::size_type size, const typename dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::size_type count)
{
	return resetSizeAndCount(m_allocateOrder, size, count);
}

//正順のメモリクリア
template<class LOCK_TYPE, class AUTO_CLEAR>
inline void dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::_clearAsc()
{
	//使用中のサイズとメモリ確保数を更新
	m_size -= m_sizeAsc;
	m_sizeAsc = 0;
	m_countAsc = 0;
}
//逆順のメモリクリア
template<class LOCK_TYPE, class AUTO_CLEAR>
inline void dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::_clearDesc()
{
	//使用中のサイズとメモリ確保数を更新
	m_size -= m_sizeDesc;
	m_sizeDesc = 0;
	m_countDesc = 0;
}

//ポインタが範囲内か判定
template<class LOCK_TYPE, class AUTO_CLEAR>
inline allocateOrder_t dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::isInUsingRange(void* p)
{
	if (p)
	{
		if (p >= m_buffRef && p < m_buffRef + m_sizeAsc)//正順の範囲内
			return ALLOC_ASC;
		else if (p >= m_buffRef + m_maxSize - m_sizeDesc && p < m_buffRef + m_maxSize)//逆順の範囲内
			return ALLOC_DESC;
	}
	//範囲外のポインタ
#ifdef GASHA_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
	static const bool IS_INVALID_POINTER = false;
	assert(IS_INVALID_POINTER);
#endif//GASHA_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
	return ALLOC_UNKNOWN_ORDER;
}

//コンストラクタ
template<class LOCK_TYPE, class AUTO_CLEAR>
inline dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::dualStackAllocator(void* buff, const std::size_t max_size) :
	m_buffRef(reinterpret_cast<char*>(buff)),
	m_maxSize(static_cast<size_type>(max_size)),
	m_size(0),
	m_sizeAsc(0),
	m_sizeDesc(0),
	m_countAsc(0),
	m_countDesc(0)
{
	assert(m_buffRef != nullptr);
	assert(m_maxSize > 0);
}
template<class LOCK_TYPE, class AUTO_CLEAR>
template<typename T>
inline dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::dualStackAllocator(T* buff, const std::size_t num) :
	dualStackAllocator(reinterpret_cast<void*>(buff), sizeof(T) * num)//C++11 委譲コンストラクタ
{}
template<class LOCK_TYPE, class AUTO_CLEAR>
template<typename T, std::size_t N>
inline dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::dualStackAllocator(T(&buff)[N]) :
dualStackAllocator(reinterpret_cast<void*>(buff), sizeof(buff))//C++11 委譲コンストラクタ
{}

//デストラクタ
template<class LOCK_TYPE, class AUTO_CLEAR>
inline dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::~dualStackAllocator()
{}

//--------------------------------------------------------------------------------
//バッファ付き双方向スタックアロケータクラス

//コンストラクタ
template<std::size_t _MAX_SIZE, class LOCK_TYPE, class AUTO_CLEAR>
inline dualStackAllocator_withBuff<_MAX_SIZE, LOCK_TYPE, AUTO_CLEAR>::dualStackAllocator_withBuff() :
dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>(m_buff, MAX_SIZE)
{}

//デストラクタ
template<std::size_t _MAX_SIZE, class LOCK_TYPE, class AUTO_CLEAR>
inline dualStackAllocator_withBuff<_MAX_SIZE, LOCK_TYPE, AUTO_CLEAR>::~dualStackAllocator_withBuff()
{}

//--------------------------------------------------------------------------------
//バッファ付き双方向スタックアロケータクラス
//※型指定版

//メモリ確保とコンストラクタ呼び出し
template<typename T, std::size_t _NUM, class LOCK_TYPE, class AUTO_CLEAR>
template<typename... Tx>
inline typename dualStackAllocator_withType<T, _NUM, LOCK_TYPE, AUTO_CLEAR>::value_type* dualStackAllocator_withType<T, _NUM, LOCK_TYPE, AUTO_CLEAR>::newDefault(Tx&&... args)
{
	return this->template newObj<value_type>(std::forward<Tx>(args)...);
}
//※アロケート方向指定版
template<typename T, std::size_t _NUM, class LOCK_TYPE, class AUTO_CLEAR>
template<typename... Tx>
inline typename dualStackAllocator_withType<T, _NUM, LOCK_TYPE, AUTO_CLEAR>::value_type* dualStackAllocator_withType<T, _NUM, LOCK_TYPE, AUTO_CLEAR>::newDefaultOrdinal(const allocateOrder_t order, Tx&&... args)
{
	return this->template newObjOrdinal<value_type>(order, std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<typename T, std::size_t _NUM, class LOCK_TYPE, class AUTO_CLEAR>
inline bool dualStackAllocator_withType<T, _NUM, LOCK_TYPE, AUTO_CLEAR>::deleteDefault(typename dualStackAllocator_withType<T, _NUM, LOCK_TYPE, AUTO_CLEAR>::value_type*& p)
{
	return this->template deleteObj<value_type>(p);
}

//コンストラクタ
template<typename T, std::size_t _NUM, class LOCK_TYPE, class AUTO_CLEAR>
inline dualStackAllocator_withType<T, _NUM, LOCK_TYPE, AUTO_CLEAR>::dualStackAllocator_withType() :
	dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>(reinterpret_cast<void*>(m_buff), MAX_SIZE)
{}

//デストラクタ
template<typename T, std::size_t _NUM, class LOCK_TYPE, class AUTO_CLEAR>
inline dualStackAllocator_withType<T, _NUM, LOCK_TYPE, AUTO_CLEAR>::~dualStackAllocator_withType()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_DUAL_STACK_ALLOCATOR_INL

// End of file

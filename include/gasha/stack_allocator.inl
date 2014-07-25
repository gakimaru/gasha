#pragma once
#ifndef GASHA_INCLUDED_STACK_ALLOCATOR_INL
#define GASHA_INCLUDED_STACK_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// stack_allocator.inl
// スタックアロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/stack_allocator.h>//スタックアロケータ【宣言部】

#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し
#include <gasha/simple_assert.h>//シンプルアサーション

#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//スタックアロケータ補助クラス

//----------------------------------------
//スタック自動クリア

//自動クリア
template<class LOCK_TYPE, class AUTO_CLEAR>
inline void stackAllocatorAutoClear::autoClear(stackAllocator<LOCK_TYPE, AUTO_CLEAR>& allocator)
{
	if (allocator.m_count == 0)
		allocator.m_size = 0;
}

//----------------------------------------
//スタック自動クリア（ダミー）

//自動クリア
template<class LOCK_TYPE, class AUTO_CLEAR>
inline void dummyStackAllocatorAutoClear::autoClear(stackAllocator<LOCK_TYPE, AUTO_CLEAR>& allocator)
{
	//何もしない
}

//--------------------------------------------------------------------------------
//スタックアロケータクラス

//メモリ解放
template<class LOCK_TYPE, class AUTO_CLEAR>
inline bool stackAllocator<LOCK_TYPE, AUTO_CLEAR>::free(void* p)
{
	if (!p)//nullptrの解放は常に成功扱い
		return true;
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	return _free(p);
}

//メモリ確保とコンストラクタ呼び出し
template<class LOCK_TYPE, class AUTO_CLEAR>
template<typename T, typename...Tx>
T* stackAllocator<LOCK_TYPE, AUTO_CLEAR>::newObj(Tx&&... args)
{
	void* p = alloc(sizeof(T), alignof(T));
	if (!p)
		return nullptr;
	return GASHA_ callConstructor<T>(p, std::forward<Tx>(args)...);
}
//※配列用
template<class LOCK_TYPE, class AUTO_CLEAR>
template<typename T, typename...Tx>
T* stackAllocator<LOCK_TYPE, AUTO_CLEAR>::newArray(const std::size_t num, Tx&&... args)
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
template<class LOCK_TYPE, class AUTO_CLEAR>
template<typename T>
bool stackAllocator<LOCK_TYPE, AUTO_CLEAR>::deleteObj(T* p)
{
	if (!p)//nullptrの解放は常に成功扱い
		return true;
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	GASHA_ callDestructor(p);//デストラクタ呼び出し
	return _free(p);
}
//※配列用
template<class LOCK_TYPE, class AUTO_CLEAR>
template<typename T>
bool stackAllocator<LOCK_TYPE, AUTO_CLEAR>::deleteArray(T* p, const std::size_t num)
{
	if (!p)//nullptrの解放は常に成功扱い
		return true;
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	T* obj = p;
	for (std::size_t i = 0; i < num; ++i, ++obj)
	{
		GASHA_ callDestructor(obj);//デストラクタ呼び出し
	}
	return _free(p);
}

//使用中のサイズを指定位置に戻す
//※位置指定版
template<class LOCK_TYPE, class AUTO_CLEAR>
inline bool  stackAllocator<LOCK_TYPE, AUTO_CLEAR>::rewind(const size_type pos)
{
	return rewind(m_buffRef + pos);
}

//メモリクリア
template<class LOCK_TYPE, class AUTO_CLEAR>
inline void stackAllocator<LOCK_TYPE, AUTO_CLEAR>::clear()
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	//使用中のサイズとメモリ確保数を更新
	m_size = 0;
	m_count = 0;
}

//ポインタが範囲内か判定
template<class LOCK_TYPE, class AUTO_CLEAR>
inline bool stackAllocator<LOCK_TYPE, AUTO_CLEAR>::isInUsingRange(void* p)
{
#ifdef GASHA_STACK_ALLOCATOR_ENABLE_ASSERTION
	GASHA_SIMPLE_ASSERT(p >= m_buffRef && p < m_buffRef + m_size, "Pointer is not in range.");
#endif//GASHA_STACK_ALLOCATOR_ENABLE_ASSERTION
	if (p >= m_buffRef && p < m_buffRef + m_size)//範囲内
		return true;
	//範囲外のポインタ
	return false;
}

//コンストラクタ
template<class LOCK_TYPE, class AUTO_CLEAR>
inline stackAllocator<LOCK_TYPE, AUTO_CLEAR>::stackAllocator(void* buff, const std::size_t max_size) :
	m_buffRef(reinterpret_cast<char*>(buff)),
	m_maxSize(static_cast<size_type>(max_size)),
	m_size(0),
	m_count(0)
{
#ifdef GASHA_STACK_ALLOCATOR_ENABLE_ASSERTION
	GASHA_SIMPLE_ASSERT(m_buffRef != nullptr, "buff is nullptr.");
	GASHA_SIMPLE_ASSERT(m_maxSize > 0, "max_size is zero.");
#endif//GASHA_STACK_ALLOCATOR_ENABLE_ASSERTION
}
template<class LOCK_TYPE, class AUTO_CLEAR>
template<typename T>
inline stackAllocator<LOCK_TYPE, AUTO_CLEAR>::stackAllocator(T* buff, const std::size_t num) :
	stackAllocator(reinterpret_cast<void*>(buff), sizeof(T) * num)//C++11 委譲コンストラクタ
{}
template<class LOCK_TYPE, class AUTO_CLEAR>
template<typename T, std::size_t N>
inline stackAllocator<LOCK_TYPE, AUTO_CLEAR>::stackAllocator(T(&buff)[N]) :
stackAllocator(reinterpret_cast<void*>(buff), sizeof(buff))//C++11 委譲コンストラクタ
{}

//デストラクタ
template<class LOCK_TYPE, class AUTO_CLEAR>
inline stackAllocator<LOCK_TYPE, AUTO_CLEAR>::~stackAllocator()
{}

//--------------------------------------------------------------------------------
//バッファ付きスタックアロケータクラス

//コンストラクタ
template<std::size_t _MAX_SIZE, class LOCK_TYPE, class AUTO_CLEAR>
inline stackAllocator_withBuff<_MAX_SIZE, LOCK_TYPE, AUTO_CLEAR>::stackAllocator_withBuff() :
stackAllocator<LOCK_TYPE, AUTO_CLEAR>(m_buff, MAX_SIZE)
{}

//デストラクタ
template<std::size_t _MAX_SIZE, class LOCK_TYPE, class AUTO_CLEAR>
inline stackAllocator_withBuff<_MAX_SIZE, LOCK_TYPE, AUTO_CLEAR>::~stackAllocator_withBuff()
{}

//--------------------------------------------------------------------------------
//バッファ付きスタックアロケータクラス
//※型指定版

//メモリ確保とコンストラクタ呼び出し
template<typename T, std::size_t _NUM, class LOCK_TYPE, class AUTO_CLEAR>
template<typename... Tx>
inline typename stackAllocator_withType<T, _NUM, LOCK_TYPE, AUTO_CLEAR>::value_type* stackAllocator_withType<T, _NUM, LOCK_TYPE, AUTO_CLEAR>::newDefault(Tx&&... args)
{
	return this->template newObj<value_type>(std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<typename T, std::size_t _NUM, class LOCK_TYPE, class AUTO_CLEAR>
inline bool stackAllocator_withType<T, _NUM, LOCK_TYPE, AUTO_CLEAR>::deleteDefault(typename stackAllocator_withType<T, _NUM, LOCK_TYPE, AUTO_CLEAR>::value_type*& p)
{
	return this->template deleteObj<value_type>(p);
}

//コンストラクタ
template<typename T, std::size_t _NUM, class LOCK_TYPE, class AUTO_CLEAR>
inline stackAllocator_withType<T, _NUM, LOCK_TYPE, AUTO_CLEAR>::stackAllocator_withType() :
	stackAllocator<LOCK_TYPE, AUTO_CLEAR>(reinterpret_cast<void*>(m_buff), MAX_SIZE)
{}

//デストラクタ
template<typename T, std::size_t _NUM, class LOCK_TYPE, class AUTO_CLEAR>
inline stackAllocator_withType<T, _NUM, LOCK_TYPE, AUTO_CLEAR>::~stackAllocator_withType()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_STACK_ALLOCATOR_INL

// End of file

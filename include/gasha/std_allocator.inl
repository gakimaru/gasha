#pragma once
#ifndef GASHA_INCLUDED_STD_ALLOCATOR_INL
#define GASHA_INCLUDED_STD_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// std_allocator.inl
// 標準アロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/std_allocator.h>//標準アロケータ【宣言部】

#include <gasha/new.h>//new/delete操作

#include <utility>//C++11 std::forward

#ifdef GASHA_HAS_MALLINFO
#include <malloc.h>
#endif//GASHA_HAS_MALLINFO

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準アロケータクラス

//バッファの全体サイズ（バイト数）
template<class LOCK_TYPE>
inline typename stdAllocator<LOCK_TYPE>::size_type stdAllocator<LOCK_TYPE>::maxSize() const
{
#ifdef GASHA_HAS_MALLINFO
	//mallinfoが使える場合
	struct mallinfo  mi = mallinfo();
	return mi.arena;
#else//GASHA_HAS_MALLINFO
	return 0;//正確な値を返せない
#endif//GASHA_HAS_MALLINFO
}

//使用中のサイズ（バイト数）
template<class LOCK_TYPE>
inline typename stdAllocator<LOCK_TYPE>::size_type stdAllocator<LOCK_TYPE>::size() const
{
#ifdef GASHA_HAS_MALLINFO
	//mallinfoが使える場合
	struct mallinfo  mi = mallinfo();
	return mi.uordblks;
#else//GASHA_HAS_MALLINFO
	return 0;//正確な値を返せない
#endif//GASHA_HAS_MALLINFO
}

//残りサイズ（バイト数）
template<class LOCK_TYPE>
inline typename stdAllocator<LOCK_TYPE>::size_type stdAllocator<LOCK_TYPE>::remain() const
{
#ifdef GASHA_HAS_MALLINFO
	//mallinfoが使える場合
	struct mallinfo  mi = mallinfo();
	return mi.fordblks;
#else//GASHA_HAS_MALLINFO
	return 0;//正確な値を返せない
#endif//GASHA_HAS_MALLINFO
}

//メモリ確保
template<class LOCK_TYPE>
inline void* stdAllocator<LOCK_TYPE>::alloc(const std::size_t size, const std::size_t align)
{
	void* p = _aligned_malloc(size, align);
	if (!p)
	{
	#ifdef GASHA_STD_ALLOCATOR_ENABLE_ASSERTION
		static const bool NOT_ENOUGH_SPACE = false;
		assert(NOT_ENOUGH_SPACE);
	#endif//GASHA_STD_ALLOCATOR_ENABLE_ASSERTION
		return nullptr;
	}
	return p;
}

//メモリ解放
template<class LOCK_TYPE>
inline bool stdAllocator<LOCK_TYPE>::free(void* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	return _free(p);
}

//メモリ確保とコンストラクタ呼び出し
template<class LOCK_TYPE>
template<typename T, typename...Tx>
T* stdAllocator<LOCK_TYPE>::newObj(Tx&&... args)
{
	void* p = alloc(sizeof(T), alignof(T));
	if (!p)
		return nullptr;
	return GASHA_ callConstructor<T>(p, std::forward<Tx>(args)...);
}
//※配列用
template<class LOCK_TYPE>
template<typename T, typename...Tx>
T* stdAllocator<LOCK_TYPE>::newArray(const std::size_t num, Tx&&... args)
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
template<class LOCK_TYPE>
template<typename T>
bool stdAllocator<LOCK_TYPE>::deleteObj(T* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	GASHA_ callDestructor(p);//デストラクタ呼び出し
	return _free(p);
}
//※配列用
template<class LOCK_TYPE>
template<typename T>
bool stdAllocator<LOCK_TYPE>::deleteArray(T* p, const std::size_t num)
{
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

//メモリ解放（共通処理）
template<class LOCK_TYPE>
inline bool stdAllocator<LOCK_TYPE>::_free(void* p)
{
	_aligned_free(p);
	return true;
}

//ポインタが範囲内か判定
template<class LOCK_TYPE>
inline bool stdAllocator<LOCK_TYPE>::isInUsingRange(void* p)
{
	return true;//判定できないので常に true を返す

	//※コンパイラによって判定可能なら処理を実装する

//	//範囲外のポインタ
//#ifdef GASHA_MONO_ALLOCATOR_ENABLE_ASSERTION
//	static const bool IS_INVALID_POINTER = false;
//	assert(IS_INVALID_POINTER);
//#endif//GASHA_MONO_ALLOCATOR_ENABLE_ASSERTION
//	return false;
}

//コンストラクタ
template<class LOCK_TYPE>
inline stdAllocator<LOCK_TYPE>::stdAllocator()
{}

//デストラクタ
template<class LOCK_TYPE>
inline stdAllocator<LOCK_TYPE>::~stdAllocator()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_STD_ALLOCATOR_INL

// End of file

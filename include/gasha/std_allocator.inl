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
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/std_allocator.h>//標準アロケータ【宣言部】

#include <gasha/memory.h>//メモリ操作：adjustStaticAlign, adjustAlign(), _aligned_malloc(), _aligned_free()
#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し
#include <gasha/simple_assert.h>//シンプルアサーション

#include <utility>//C++11 std::forward
#include <cstdlib>//malloc(), free()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準アロケータ実装部クラス

//----------------------------------------
//アラインメント非対応版（デフォルト）

//メモリ確保
inline void* stdAllocatorImpl_NoAlign::alloc(const std::size_t size, const std::size_t align)
{
	return std::malloc(size);
}
//メモリ解放
inline void stdAllocatorImpl_NoAlign::free(void* p)
{
	return std::free(p);
}

//----------------------------------------
//アライメント対応版

//メモリ確保
inline void* stdAllocatorImpl_Align::alloc(const std::size_t size, const std::size_t align)
{
	return _aligned_malloc(size, align);
}
//メモリ解放
inline void stdAllocatorImpl_Align::free(void* p)
{
	return _aligned_free(p);
}

//--------------------------------------------------------------------------------
//標準アロケータクラス

//バッファの全体サイズ（バイト数）
template<class LOCK_POLICY, class IMPLEMENT_POLICY>
inline typename stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::size_type stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::maxSize() const
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
template<class LOCK_POLICY, class IMPLEMENT_POLICY>
inline typename stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::size_type stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::size() const
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
template<class LOCK_POLICY, class IMPLEMENT_POLICY>
inline typename stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::size_type stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::remain() const
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
template<class LOCK_POLICY, class IMPLEMENT_POLICY>
inline void* stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::alloc(const std::size_t size, const std::size_t align)
{
	void* p = implemnt_type::alloc(size, align);
#ifdef GASHA_STD_ALLOCATOR_ENABLE_ASSERTION
	GASHA_SIMPLE_ASSERT(p != nullptr, "stdAllocator is failed allocation.");
#endif//GASHA_STD_ALLOCATOR_ENABLE_ASSERTION
	if (!p)
		return nullptr;
	return p;
}

//メモリ解放
template<class LOCK_POLICY, class IMPLEMENT_POLICY>
inline bool stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::free(void* p)
{
	if (!p)//nullptrの解放は常に成功扱い
		return true;
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	return _free(p);
}

//メモリ確保とコンストラクタ呼び出し
template<class LOCK_POLICY, class IMPLEMENT_POLICY>
template<typename T, typename...Tx>
T* stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::newObj(Tx&&... args)
{
	void* p = alloc(sizeof(T), alignof(T));
	if (!p)
		return nullptr;
	return GASHA_ callConstructor<T>(p, std::forward<Tx>(args)...);
}
//※配列用
template<class LOCK_POLICY, class IMPLEMENT_POLICY>
template<typename T, typename...Tx>
T* stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::newArray(const std::size_t num, Tx&&... args)
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
template<class LOCK_POLICY, class IMPLEMENT_POLICY>
template<typename T>
bool stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::deleteObj(T* p)
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
template<class LOCK_POLICY, class IMPLEMENT_POLICY>
template<typename T>
bool stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::deleteArray(T* p, const std::size_t num)
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

//メモリ解放（共通処理）
template<class LOCK_POLICY, class IMPLEMENT_POLICY>
inline bool stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::_free(void* p)
{
	if (!p)//nullptrの解放は常に成功扱い
		return true;
	implemnt_type::free(p);
	return true;
}

//ポインタが範囲内か判定
template<class LOCK_POLICY, class IMPLEMENT_POLICY>
inline bool stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::isInUsingRange(void* p)
{
	return true;//判定できないので常に true を返す

	//※コンパイラによって判定可能なら処理を実装する

//	//範囲外のポインタ
//#ifdef GASHA_STD_ALLOCATOR_ENABLE_ASSERTION
//	GASHA_SIMPLE_ASSERT(???, "Pointer is not in range.");
//#endif//GASHA_STD_ALLOCATOR_ENABLE_ASSERTION
//	return false;
}

//コンストラクタ
template<class LOCK_POLICY, class IMPLEMENT_POLICY>
inline stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::stdAllocator()
{}

//デストラクタ
template<class LOCK_POLICY, class IMPLEMENT_POLICY>
inline stdAllocator<LOCK_POLICY, IMPLEMENT_POLICY>::~stdAllocator()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_STD_ALLOCATOR_INL

// End of file

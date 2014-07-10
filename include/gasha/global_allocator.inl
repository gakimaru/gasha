#pragma once
#ifndef GASHA_INCLUDED_GLOBAL_ALLOCATOR_INL
#define GASHA_INCLUDED_GLOBAL_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// global_allocator.inl
// グローバルアロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/global_allocator.h>//グローバルアロケータ【宣言部】

#include <utility>//C++11 std::forward

#ifdef GASHA_HAS_MALLINFO
#include <malloc.h>
#endif//GASHA_HAS_MALLINFO

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//グローバルアロケータクラス

//バッファの全体サイズ（バイト数）
template<class LOCK_TYPE>
inline typename globalAllocator<LOCK_TYPE>::size_type globalAllocator<LOCK_TYPE>::maxSize() const
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
inline typename globalAllocator<LOCK_TYPE>::size_type globalAllocator<LOCK_TYPE>::size() const
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
inline typename globalAllocator<LOCK_TYPE>::size_type globalAllocator<LOCK_TYPE>::remain() const
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
inline void* globalAllocator<LOCK_TYPE>::alloc(const std::size_t size, const std::size_t align)
{
	void* p = _aligned_malloc(size, align);
	if (!p)
	{
	#ifdef GASHA_GLOBAL_ALLOCATOR_ENABLE_ASSERTION
		static const bool NOT_ENOUGH_SPACE = false;
		assert(NOT_ENOUGH_SPACE);
	#endif//GASHA_GLOBAL_ALLOCATOR_ENABLE_ASSERTION
		return nullptr;
	}
	return p;
}

//メモリ解放
template<class LOCK_TYPE>
inline bool globalAllocator<LOCK_TYPE>::free(void* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	return _free(p);
}

//メモリ確保とコンストラクタ呼び出し
template<class LOCK_TYPE>
template<typename T, typename...Tx>
T* globalAllocator<LOCK_TYPE>::newObj(Tx&&... args)
{
	void* p = alloc(sizeof(T), alignof(T));
	if (!p)
		return nullptr;
	return new(p)T(std::forward<Tx>(args)...);
}
//※配列用
template<class LOCK_TYPE>
template<typename T, typename...Tx>
T* globalAllocator<LOCK_TYPE>::newArray(const std::size_t num, Tx&&... args)
{
	void* p = alloc(sizeof(T) * num, alignof(T));
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
template<class LOCK_TYPE>
template<typename T>
bool globalAllocator<LOCK_TYPE>::deleteObj(T* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	p->~T();//デストラクタ呼び出し
	//operator delete(p, p);//（作法として）deleteオペレータ呼び出し
	return _free(p);
}
//※配列用
template<class LOCK_TYPE>
template<typename T>
bool globalAllocator<LOCK_TYPE>::deleteArray(T* p, const std::size_t num)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	T* obj = p;
	for (std::size_t i = 0; i < num; ++i, ++obj)
	{
		obj->~T();//デストラクタ呼び出し
		//operator delete(p, p);//（作法として）deleteオペレータ呼び出し
	}
	return _free(p);
}

//メモリ解放（共通処理）
template<class LOCK_TYPE>
inline bool globalAllocator<LOCK_TYPE>::_free(void* p)
{
	_aligned_free(p);
	return true;
}

//ポインタが範囲内か判定
template<class LOCK_TYPE>
inline bool globalAllocator<LOCK_TYPE>::isInUsingRange(void* p)
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
inline globalAllocator<LOCK_TYPE>::globalAllocator()
{}

//デストラクタ
template<class LOCK_TYPE>
inline globalAllocator<LOCK_TYPE>::~globalAllocator()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_GLOBAL_ALLOCATOR_INL

// End of file

#pragma once
#ifndef GASHA_INCLUDED_MONO_ALLOCATOR_INL
#define GASHA_INCLUDED_MONO_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// mono_allocator.inl
// 単一アロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/mono_allocator.h>//単一アロケータ【宣言部】

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
//単一アロケータクラス

//メモリ解放
template<class LOCK_TYPE>
inline bool monoAllocator<LOCK_TYPE>::free(void* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	return _free(p);
}

//メモリ確保とコンストラクタ呼び出し
template<class LOCK_TYPE>
template<typename T, typename...Tx>
T* monoAllocator<LOCK_TYPE>::newObj(Tx&&... args)
{
	void* p = alloc(sizeof(T), alignof(T));
	if (!p)
		return nullptr;
	return new(p)T(std::forward<Tx>(args)...);
}
//※配列用
template<class LOCK_TYPE>
template<typename T, typename...Tx>
T* monoAllocator<LOCK_TYPE>::newArray(const std::size_t num, Tx&&... args)
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
bool monoAllocator<LOCK_TYPE>::deleteObj(T* p)
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
bool monoAllocator<LOCK_TYPE>::deleteArray(T* p, const std::size_t num)
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

//ポインタが範囲内か判定
template<class LOCK_TYPE>
inline bool monoAllocator<LOCK_TYPE>::isInUsingRange(void* p)
{
	if (p >= m_buffRef && p < m_buffRef + m_size)//範囲内
		return true;
	//範囲外のポインタ
#ifdef GASHA_MONO_ALLOCATOR_ENABLE_ASSERTION
	static const bool IS_INVALID_POINTER = false;
	assert(IS_INVALID_POINTER);
#endif//GASHA_MONO_ALLOCATOR_ENABLE_ASSERTION
	return false;
}

//コンストラクタ
template<class LOCK_TYPE>
inline monoAllocator<LOCK_TYPE>::monoAllocator(void* buff, const std::size_t max_size) :
	m_buffRef(reinterpret_cast<char*>(buff)),
	m_maxSize(static_cast<size_type>(max_size)),
	m_size(0)
{
	assert(m_buffRef != nullptr);
	assert(m_maxSize > 0);
}
template<class LOCK_TYPE>
template<typename T>
inline monoAllocator<LOCK_TYPE>::monoAllocator(T* buff, const std::size_t num) :
	monoAllocator(reinterpret_cast<void*>(buff), sizeof(T) * num)//C++11 委譲コンストラクタ
{}
template<class LOCK_TYPE>
template<typename T, std::size_t N>
inline monoAllocator<LOCK_TYPE>::monoAllocator(T(&buff)[N]) :
monoAllocator(reinterpret_cast<void*>(buff), sizeof(buff))//C++11 委譲コンストラクタ
{}

//デストラクタ
template<class LOCK_TYPE>
inline monoAllocator<LOCK_TYPE>::~monoAllocator()
{}

//--------------------------------------------------------------------------------
//バッファ付き単一アロケータクラス

//コンストラクタ
template<std::size_t _MAX_SIZE, class LOCK_TYPE>
inline monoAllocator_withBuff<_MAX_SIZE, LOCK_TYPE>::monoAllocator_withBuff() :
monoAllocator<LOCK_TYPE>(m_buff, MAX_SIZE)
{}

//デストラクタ
template<std::size_t _MAX_SIZE, class LOCK_TYPE>
inline monoAllocator_withBuff<_MAX_SIZE, LOCK_TYPE>::~monoAllocator_withBuff()
{}

//--------------------------------------------------------------------------------
//バッファ付き単一アロケータクラス
//※型指定版

//メモリ確保とコンストラクタ呼び出し
template<typename T, std::size_t _NUM, class LOCK_TYPE>
template<typename... Tx>
inline typename monoAllocator_withType<T, _NUM, LOCK_TYPE>::value_type* monoAllocator_withType<T, _NUM, LOCK_TYPE>::newDefault(Tx&&... args)
{
	return this->template newObj<value_type>(std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<typename T, std::size_t _NUM, class LOCK_TYPE>
inline bool monoAllocator_withType<T, _NUM, LOCK_TYPE>::deleteDefault(typename monoAllocator_withType<T, _NUM, LOCK_TYPE>::value_type*& p)
{
	return this->template deleteObj<value_type>(p);
}

//コンストラクタ
template<typename T, std::size_t _NUM, class LOCK_TYPE>
inline monoAllocator_withType<T, _NUM, LOCK_TYPE>::monoAllocator_withType() :
	monoAllocator<LOCK_TYPE>(reinterpret_cast<void*>(m_buff), MAX_SIZE)
{}

//デストラクタ
template<typename T, std::size_t _NUM, class LOCK_TYPE>
inline monoAllocator_withType<T, _NUM, LOCK_TYPE>::~monoAllocator_withType()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_MONO_ALLOCATOR_INL

// End of file

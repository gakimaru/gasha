#pragma once
#ifndef GASHA_INCLUDED_LF_STACK_ALLOCATOR_INL
#define GASHA_INCLUDED_LF_STACK_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lf_stack_allocator.inl
// ロックフリースタックアロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_stack_allocator.h>//ロックフリースタックアロケータ【宣言部】

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
//ロックフリースタックアロケータ補助クラス

//----------------------------------------
//スタック自動クリア

//自動クリア
template<class AUTO_CLEAR>
void lfStackAllocatorAutoClear::autoClear(lfStackAllocator<AUTO_CLEAR>& allocator)
{
	while(true)
	{
		typename lfStackAllocator<AUTO_CLEAR>::size_type now_size = allocator.m_size.load();
		const typename lfStackAllocator<AUTO_CLEAR>::size_type new_size = 0;
		if (allocator.m_allocatedCount.load() > 0)
			return;
		if (allocator.m_size.compare_exchange_weak(now_size, new_size))//サイズのCAS
			return;
	}
}

//----------------------------------------
//スタック自動クリア（ダミー）

//自動クリア
template<class AUTO_CLEAR>
inline void dummyLfStackAllocatorAutoClear::autoClear(lfStackAllocator<AUTO_CLEAR>& allocator)
{
	//何もしない
}

//--------------------------------------------------------------------------------
//ロックフリースタックアロケータクラス

//メモリ解放
template<class AUTO_CLEAR>
inline bool lfStackAllocator<AUTO_CLEAR>::free(void* p)
{
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	return _free(p);
}

//メモリ確保とコンストラクタ呼び出し
template<class AUTO_CLEAR>
template<typename T, typename...Tx>
T* lfStackAllocator<AUTO_CLEAR>::newObj(Tx&&... args)
{
	void* p = alloc(sizeof(T), alignof(T));
	if (!p)
		return nullptr;
	return new(p)T(std::forward<Tx>(args)...);
}
//※配列用
template<class AUTO_CLEAR>
template<typename T, typename...Tx>
T* lfStackAllocator<AUTO_CLEAR>::newArray(const std::size_t num, Tx&&... args)
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
template<class AUTO_CLEAR>
template<typename T>
bool lfStackAllocator<AUTO_CLEAR>::deleteObj(T* p)
{
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	p->~T();//デストラクタ呼び出し
	//operator delete(p, p);//（作法として）deleteオペレータ呼び出し
	return _free(p);
}
//※配列用
template<class AUTO_CLEAR>
template<typename T>
bool lfStackAllocator<AUTO_CLEAR>::deleteArray(T* p, const std::size_t num)
{
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

//使用中のサイズを指定位置に戻す
//※位置指定版
template<class AUTO_CLEAR>
inline bool lfStackAllocator<AUTO_CLEAR>::rewind(const size_type pos)
{
	return rewind(m_buffRef + pos);
}

//ポインタが範囲内か判定
template<class AUTO_CLEAR>
inline bool lfStackAllocator<AUTO_CLEAR>::isInUsingRange(void* p)
{
	if (p >= m_buffRef && p < m_buffRef + m_size)//範囲内
		return true;
	//範囲外のポインタ
#ifdef GASHA_LF_STACK_ALLOCATOR_ENABLE_ASSERTION
	static const bool IS_INVALID_POINTER = false;
	assert(IS_INVALID_POINTER);
#endif//GASHA_LF_STACK_ALLOCATOR_ENABLE_ASSERTION
	return false;
}

//コンストラクタ
template<class AUTO_CLEAR>
inline lfStackAllocator<AUTO_CLEAR>::lfStackAllocator(void* buff, const std::size_t max_size) :
	m_buffRef(reinterpret_cast<char*>(buff)),
	m_maxSize(static_cast<size_type>(max_size)),
	m_size(0),
	m_allocatedCount(0)
{
	assert(m_buffRef != nullptr);
	assert(m_maxSize > 0);
}
template<class AUTO_CLEAR>
template<typename T>
inline lfStackAllocator<AUTO_CLEAR>::lfStackAllocator(T* buff, const std::size_t num) :
	lfStackAllocator(reinterpret_cast<void*>(buff), sizeof(T) * num)//C++11 委譲コンストラクタ
{}
template<class AUTO_CLEAR>
template<typename T, std::size_t N>
inline lfStackAllocator<AUTO_CLEAR>::lfStackAllocator(T(&buff)[N]) :
lfStackAllocator(reinterpret_cast<void*>(buff), sizeof(buff))//C++11 委譲コンストラクタ
{}

//デストラクタ
template<class AUTO_CLEAR>
inline lfStackAllocator<AUTO_CLEAR>::~lfStackAllocator()
{}

//--------------------------------------------------------------------------------
//バッファ付きロックフリースタックアロケータクラス

//コンストラクタ
template<std::size_t _MAX_SIZE, class AUTO_CLEAR>
inline lfStackAllocator_withBuff<_MAX_SIZE, AUTO_CLEAR>::lfStackAllocator_withBuff() :
lfStackAllocator<AUTO_CLEAR>(m_buff, MAX_SIZE)
{}

//デストラクタ
template<std::size_t _MAX_SIZE, class AUTO_CLEAR>
inline lfStackAllocator_withBuff<_MAX_SIZE, AUTO_CLEAR>::~lfStackAllocator_withBuff()
{}

//--------------------------------------------------------------------------------
//バッファ付きロックフリースタックアロケータクラス
//※型指定版

//メモリ確保とコンストラクタ呼び出し
template<typename T, std::size_t _NUM, class AUTO_CLEAR>
template<typename... Tx>
inline typename lfStackAllocator_withType<T, _NUM, AUTO_CLEAR>::value_type* lfStackAllocator_withType<T, _NUM, AUTO_CLEAR>::newDefault(Tx&&... args)
{
	return this->template newObj<value_type>(std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<typename T, std::size_t _NUM, class AUTO_CLEAR>
inline bool lfStackAllocator_withType<T, _NUM, AUTO_CLEAR>::deleteDefault(typename lfStackAllocator_withType<T, _NUM, AUTO_CLEAR>::value_type*& p)
{
	return this->template deleteObj<value_type>(p);
}

//コンストラクタ
template<typename T, std::size_t _NUM, class AUTO_CLEAR>
inline lfStackAllocator_withType<T, _NUM, AUTO_CLEAR>::lfStackAllocator_withType() :
	lfStackAllocator<AUTO_CLEAR>(reinterpret_cast<void*>(m_buff), MAX_SIZE)
{}

//デストラクタ
template<typename T, std::size_t _NUM, class AUTO_CLEAR>
inline lfStackAllocator_withType<T, _NUM, AUTO_CLEAR>::~lfStackAllocator_withType()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_LF_STACK_ALLOCATOR_INL

// End of file

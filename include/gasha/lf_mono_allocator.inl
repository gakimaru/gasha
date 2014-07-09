#pragma once
#ifndef GASHA_INCLUDED_LF_MONO_ALLOCATOR_INL
#define GASHA_INCLUDED_LF_MONO_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lf_mono_allocator.inl
// ロックフリー単一アロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_mono_allocator.h>//ロックフリー単一アロケータ【宣言部】

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
//ロックフリー単一アロケータクラス

//メモリ解放
inline bool lfMonoAllocator::free(void* p)
{
	if (!inUsingRange(p))//正しいポインタか判定
		return false;
	return _free(p);
}

//メモリ確保とコンストラクタ呼び出し
template<typename T, typename...Tx>
T* lfMonoAllocator::newObj(Tx&&... args)
{
	void* p = alloc(sizeof(T), alignof(T));
	if (!p)
		return nullptr;
	return new(p)T(std::forward<Tx>(args)...);
}
//※配列用
template<typename T, typename...Tx>
T* lfMonoAllocator::newArray(const std::size_t num, Tx&&... args)
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
template<typename T>
bool lfMonoAllocator::deleteObj(T* p)
{
	if (!inUsingRange(p))//正しいポインタか判定
		return false;
	p->~T();//デストラクタ呼び出し
	//operator delete(p, p);//（作法として）deleteオペレータ呼び出し
	return _free(p);
}
//※配列用
template<typename T>
bool lfMonoAllocator::deleteArray(T* p, const std::size_t num)
{
	if (!inUsingRange(p))//正しいポインタか判定
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
inline bool lfMonoAllocator::inUsingRange(void* p)
{
	if (p < m_buffRef || p >= m_buffRef + m_size)//範囲外のポインタなら終了
	{
	#ifdef GASHA_LF_MONO_ALLOCATOR_ENABLE_ASSERTION
		static const bool IS_INVALID_POINTER = false;
		assert(IS_INVALID_POINTER);
	#endif//GASHA_LF_MONO_ALLOCATOR_ENABLE_ASSERTION
		return false;
	}
	return true;
}

//コンストラクタ
inline lfMonoAllocator::lfMonoAllocator(void* buff, const std::size_t max_size) :
	m_buffRef(reinterpret_cast<char*>(buff)),
	m_maxSize(static_cast<size_type>(max_size)),
	m_size(0)
{
	assert(m_buffRef != nullptr);
	assert(m_maxSize > 0);
}
template<typename T>
inline lfMonoAllocator::lfMonoAllocator(T* buff, const std::size_t num) :
	lfMonoAllocator(reinterpret_cast<void*>(buff), sizeof(T) * num)//C++11 委譲コンストラクタ
{}
template<typename T, std::size_t N>
inline lfMonoAllocator::lfMonoAllocator(T(&buff)[N]) :
lfMonoAllocator(reinterpret_cast<void*>(buff), sizeof(buff))//C++11 委譲コンストラクタ
{}

//デストラクタ
inline lfMonoAllocator::~lfMonoAllocator()
{}

//--------------------------------------------------------------------------------
//バッファ付きロックフリー単一アロケータクラス

//コンストラクタ
template<std::size_t _MAX_SIZE>
inline lfMonoAllocator_withBuff<_MAX_SIZE>::lfMonoAllocator_withBuff() :
lfMonoAllocator(m_buff, MAX_SIZE)
{}

//デストラクタ
template<std::size_t _MAX_SIZE>
inline lfMonoAllocator_withBuff<_MAX_SIZE>::~lfMonoAllocator_withBuff()
{}

//--------------------------------------------------------------------------------
//バッファ付きロックフリー単一アロケータクラス
//※型指定版

//メモリ確保とコンストラクタ呼び出し
template<typename T, std::size_t _NUM>
template<typename... Tx>
inline typename lfMonoAllocator_withType<T, _NUM>::value_type* lfMonoAllocator_withType<T, _NUM>::newDefault(Tx&&... args)
{
	return this->template newObj<value_type>(std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<typename T, std::size_t _NUM>
inline bool lfMonoAllocator_withType<T, _NUM>::deleteDefault(typename lfMonoAllocator_withType<T, _NUM>::value_type*& p)
{
	return this->template deleteObj<value_type>(p);
}

//コンストラクタ
template<typename T, std::size_t _NUM>
inline lfMonoAllocator_withType<T, _NUM>::lfMonoAllocator_withType() :
	lfMonoAllocator(reinterpret_cast<void*>(m_buff), MAX_SIZE)
{}

//デストラクタ
template<typename T, std::size_t _NUM>
inline lfMonoAllocator_withType<T, _NUM>::~lfMonoAllocator_withType()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_LF_MONO_ALLOCATOR_INL

// End of file

#pragma once
#ifndef GASHA_INCLUDED_LF_MONO_ALLOCATOR_INL
#define GASHA_INCLUDED_LF_MONO_ALLOCATOR_INL

//--------------------------------------------------------------------------------
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

#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し

#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリー単一アロケータクラス

//メモリ解放
inline bool lfMonoAllocator::free(void* p)
{
	if (!isInUsingRange(p))//正しいポインタか判定
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
	return GASHA_ callConstructor<T>(p, std::forward<Tx>(args)...);
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
		T* obj = GASHA_ callConstructor<T>(p, std::forward<Tx>(args)...);
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
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	GASHA_ callDestructor(p);//デストラクタ呼び出し
	return _free(p);
}
//※配列用
template<typename T>
bool lfMonoAllocator::deleteArray(T* p, const std::size_t num)
{
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	T* obj = p;
	for (std::size_t i = 0; i < num; ++i, ++obj)
	{
		GASHA_ callDestructor(obj);//デストラクタ呼び出し
	}
	return _free(p);
}

//強制クリア
inline void lfMonoAllocator::clear()
{
	m_size.store(0);
}

//ポインタが範囲内か判定
inline bool lfMonoAllocator::isInUsingRange(void* p)
{
	if (p >= m_buffRef && p < m_buffRef + m_size)//範囲内
		return true;
	//範囲外のポインタ
#ifdef GASHA_LF_MONO_ALLOCATOR_ENABLE_ASSERTION
	static const bool IS_INVALID_POINTER = false;
	assert(IS_INVALID_POINTER);
#endif//GASHA_LF_MONO_ALLOCATOR_ENABLE_ASSERTION
	return false;
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

#endif//GASHA_INCLUDED_LF_MONO_ALLOCATOR_INL

// End of file

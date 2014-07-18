#pragma once
#ifndef GASHA_INCLUDED_ALLOCATOR_ADAPTER_INL
#define GASHA_INCLUDED_ALLOCATOR_ADAPTER_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// allocator_adapter.inl
// アロケータアダプター【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/allocator_adapter.h>//アロケータアダプター【宣言部】

#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アロケータアダプタークラス

//アロケータ名
template<class ALLOCATOR>
inline const char* allocatorAdapter<ALLOCATOR>::name() const
{
	return m_name;
}

//アロケータの実装モード名
template<class ALLOCATOR>
inline const char* allocatorAdapter<ALLOCATOR>::mode() const
{
	return m_mode;
}

//バッファの全体サイズ（バイト数）
template<class ALLOCATOR>
inline typename allocatorAdapter<ALLOCATOR>::size_type allocatorAdapter<ALLOCATOR>::maxSize() const
{
	return m_allocator.maxSize();
}

//使用中のサイズ（バイト数）
template<class ALLOCATOR>
inline typename allocatorAdapter<ALLOCATOR>::size_type allocatorAdapter<ALLOCATOR>::size() const
{
	return m_allocator.size();
}

//残りサイズ（バイト数）
template<class ALLOCATOR>
inline typename allocatorAdapter<ALLOCATOR>::size_type allocatorAdapter<ALLOCATOR>::remain() const
{
	return maxSize();
}

//メモリ確保
template<class ALLOCATOR>
inline void* allocatorAdapter<ALLOCATOR>::alloc(const std::size_t size, const std::size_t align)
{
	return m_allocator.alloc(size, align);
}

//メモリ解放
template<class ALLOCATOR>
inline bool allocatorAdapter<ALLOCATOR>::free(void* p)
{
	return m_allocator.free(p);
}

#if 0//※テンプレート関数は仮想化不可（一定のvtableが確定できないため）
//メモリ確保とコンストラクタ呼び出し
template<class ALLOCATOR>
template<typename T, typename...Tx>
T* allocatorAdapter<ALLOCATOR>::newObj(Tx&&... args)
{
	return m_allocator.template newObj<T>(std::forward<Tx>(args)...);
}
//※配列用
template<class ALLOCATOR>
template<typename T, typename...Tx>
T* allocatorAdapter<ALLOCATOR>::newArray(const std::size_t num, Tx&&... args)
{
	return m_allocator.template newArray<T>(std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<class ALLOCATOR>
template<typename T>
bool allocatorAdapter<ALLOCATOR>::deleteObj(T* p)
{
	return m_allocator.deleteObj(p);
}
//※配列用
template<class ALLOCATOR>
template<typename T>
bool allocatorAdapter<ALLOCATOR>::deleteArray(T* p, const std::size_t num)
{
	return m_allocator.deleteArray(p, num);
}
#endif//DELETE

//デバッグ情報作成
template<class ALLOCATOR>
inline std::size_t allocatorAdapter<ALLOCATOR>::debugInfo(char* message, const std::size_t max_size) const
{
	return m_allocator.debugInfo(message, max_size);
}

//コンストラクタ
template<class ALLOCATOR>
inline allocatorAdapter<ALLOCATOR>::allocatorAdapter(allocatorAdapter<ALLOCATOR>&& adapter) :
	m_allocator(adapter.m_allocator),
	m_name(adapter.m_name),
	m_mode(adapter.m_mode)
{}
template<class ALLOCATOR>
inline allocatorAdapter<ALLOCATOR>::allocatorAdapter(const allocatorAdapter<ALLOCATOR>& adapter) :
	m_allocator(adapter.m_allocator),
	m_name(adapter.m_name),
	m_mode(adapter.m_mode)
{}
template<class ALLOCATOR>
inline allocatorAdapter<ALLOCATOR>::allocatorAdapter(allocator_type&& allocator, const char* name, const char* mode) :
	m_allocator(allocator),
	m_name(name),
	m_mode(mode)
{}
template<class ALLOCATOR>
inline allocatorAdapter<ALLOCATOR>::allocatorAdapter(allocator_type& allocator, const char* name, const char* mode) :
	m_allocator(allocator),
	m_name(name),
	m_mode(mode)
{}

//デストラクタ
template<class ALLOCATOR>
inline allocatorAdapter<ALLOCATOR>::~allocatorAdapter()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ALLOCATOR_ADAPTER_INL

// End of file

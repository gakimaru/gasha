#pragma once
#ifndef GASHA_INCLUDED_POLY_ALLOCATOR_INL
#define GASHA_INCLUDED_POLY_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// poly_allocator.inl
// 多態アロケータ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/poly_allocator.h>//多態アロケータ【宣言部】

#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------
//アロケート用デバッグ情報

//コンストラクタ
#ifdef GASHA_HAS_DEBUG_FEATURE
inline debugAllocationInfo::debugAllocationInfo(const char* file_name, const char* func_name, const char* call_point_name, const double time, const char* type_name, const std::size_t type_size, const std::size_t array_num) :
m_fileName(file_name),
m_funcName(func_name),
m_callPointName(call_point_name),
m_time(time),
m_typeName(type_name),
m_typeSize(type_size),
m_arrayNum(array_num)
{}
#else//GASHA_HAS_DEBUG_FEATURE
inline debugAllocationInfo::debugAllocationInfo(const char* file_name, const char* func_name, const char* call_point_name, const double time, const char* type_name, const std::size_t type_size, const std::size_t array_num)
{}
#endif//GASHA_HAS_DEBUG_FEATURE

//--------------------
//デバッグ用メモリアロケート観察者

//コンストラクタ
debugAllocationObserver::debugAllocationObserver():
	m_atNew(nullptr),
	m_atDelete(nullptr),
	m_atChangeAllocator(nullptr),
	m_atReturnAllocator(nullptr)
{}

//--------------------
//多態アロケータクラス

#ifdef GASHA_ENABLE_POLY_ALLOCATOR

//アクセッサ
inline const char* polyAllocator::name() const{ return m_adapter->name(); };//アロケータ名
inline const char* polyAllocator::mode() const{ return m_adapter->mode(); };//アロケータの実装モード名
inline const GASHA_ IAllocatorAdapter* polyAllocator::adapter() const{ return m_adapter; };//アダプター
inline GASHA_ IAllocatorAdapter* polyAllocator::adapter(){ return m_adapter; };//アダプター

//オペレータ
inline const GASHA_ IAllocatorAdapter& polyAllocator::operator*() const { return *m_adapter; }
inline GASHA_ IAllocatorAdapter& polyAllocator::operator*(){ return *m_adapter; }
inline const GASHA_ IAllocatorAdapter* polyAllocator::operator->() const { return m_adapter; }
inline GASHA_ IAllocatorAdapter* polyAllocator::operator->(){ return m_adapter; }

//キャストオペレータ
inline polyAllocator::operator const GASHA_ IAllocatorAdapter&() const { return *m_adapter; }
inline polyAllocator::operator GASHA_ IAllocatorAdapter&() { return *m_adapter; }

#else//GASHA_ENABLE_POLY_ALLOCATOR

//アクセッサ
inline const char* polyAllocator::name() const{ return nullptr; };//アロケータ名
inline const char* polyAllocator::mode() const{ return nullptr; };//アロケータの実装モード名
inline const GASHA_ IAllocatorAdapter* polyAllocator::adapter() const{ return nullptr; };//アダプター
inline GASHA_ IAllocatorAdapter* polyAllocator::adapter(){ return nullptr; };//アダプター

//オペレータ
inline const GASHA_ IAllocatorAdapter& polyAllocator::operator*() const { return *m_dummyAdapter; }
inline GASHA_ IAllocatorAdapter& polyAllocator::operator*(){ return *m_dummyAdapter; }
inline const GASHA_ IAllocatorAdapter* polyAllocator::operator->() const { return nullptr; }
inline GASHA_ IAllocatorAdapter* polyAllocator::operator->(){ return nullptr; }

//キャストオペレータ
inline polyAllocator::operator const GASHA_ IAllocatorAdapter&() const { return *m_dummyAdapter; }
inline polyAllocator::operator GASHA_ IAllocatorAdapter&() { return *m_dummyAdapter; }

#endif//GASHA_ENABLE_POLY_ALLOCATOR

//デバッグ観察者を変更
inline const GASHA_ debugAllocationObserver* polyAllocator::debugObserver() const
{
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
	return m_observer;
#else//GASHA_ENABLE_POLY_ALLOCATOR
	return nullptr;
#endif//GASHA_ENABLE_POLY_ALLOCATOR
}

//デバッグ観察者を更新
inline void polyAllocator::setDebugObserver(const GASHA_ debugAllocationObserver& observer) const
{
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
	m_observer = &observer;
#endif//GASHA_ENABLE_POLY_ALLOCATOR
}
inline void polyAllocator::resetDebugObserver() const
{
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
	m_observer = nullptr;
#endif//GASHA_ENABLE_POLY_ALLOCATOR
}

//アライメントサイズを取得
inline std::size_t polyAllocator::align()
{
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
	return m_align;
#else//GASHA_ENABLE_POLY_ALLOCATOR
	return 0;
#endif//GASHA_ENABLE_POLY_ALLOCATOR
}

//アライメントサイズを変更
inline void polyAllocator::setAlign(const std::size_t align)
{
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
	m_align = align;
#endif//GASHA_ENABLE_POLY_ALLOCATOR
}
inline void polyAllocator::resetAlign()
{
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
	m_align = DEFAULT_ALIGN;
#endif//GASHA_ENABLE_POLY_ALLOCATOR
}

//デバッグ情報取得
inline const debugAllocationInfo* polyAllocator::debugInfo()
{
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
	return m_debugInfo;
#else//GASHA_ENABLE_POLY_ALLOCATOR
	return nullptr;
#endif//GASHA_ENABLE_POLY_ALLOCATOR
}

//デバッグ情報を変更
inline void polyAllocator::setDebugInfo(const GASHA_ debugAllocationInfo* info)
{
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
	m_debugInfo = info;
#else//GASHA_ENABLE_POLY_ALLOCATOR
#endif//GASHA_ENABLE_POLY_ALLOCATOR
}
inline void polyAllocator::resetDebugInfo()
{
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
	m_debugInfo = nullptr;
#else//GASHA_ENABLE_POLY_ALLOCATOR
#endif//GASHA_ENABLE_POLY_ALLOCATOR
}

//コンストラクタ
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
inline polyAllocator::polyAllocator(GASHA_ IAllocatorAdapter& adapter) :
	m_prevAdapter(m_adapter),
	m_prevObserver(m_observer)
{
	callbackAtChangeAllocator(*m_adapter, adapter);
	m_adapter = &adapter;
	m_observer = nullptr;
}
#else//GASHA_ENABLE_POLY_ALLOCATOR
inline polyAllocator::polyAllocator(GASHA_ IAllocatorAdapter& adapter)
{}
#endif//GASHA_ENABLE_POLY_ALLOCATOR

//デフォルトコンストラクタ
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
inline polyAllocator::polyAllocator() :
	m_prevAdapter(nullptr),
	m_prevObserver(nullptr)
{
	if (!m_adapter)
		initlaizeStdAllocatorAdapter();//強制初期化
}
#else//GASHA_ENABLE_POLY_ALLOCATOR
inline polyAllocator::polyAllocator()
{}
#endif//GASHA_ENABLE_POLY_ALLOCATOR

//デストラクタ
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
inline polyAllocator::~polyAllocator()
{
	if (m_prevAdapter)
	{
		callbackAtReturnAllocator(*m_prevAdapter, *m_adapter);
		m_adapter = m_prevAdapter;
		m_observer = m_prevObserver;
	}
}
#else//GASHA_ENABLE_POLY_ALLOCATOR
inline polyAllocator::~polyAllocator()
{}
#endif//GASHA_ENABLE_POLY_ALLOCATOR

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_POLY_ALLOCATOR_INL

// End of file

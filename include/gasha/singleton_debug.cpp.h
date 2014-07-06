#pragma once
#ifndef GASHA_INCLUDED_SINGLETON_DEBUG_CPP_H
#define GASHA_INCLUDED_SINGLETON_DEBUG_CPP_H

//--------------------------------------------------------------------------------
// singleton_debug.cpp.h
// シングルトンデバッグ用処理【関数定義部】
//
// ※シングルトンの実体化が必要な場所でインクルード。
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/singleton_debug.inl>//シングルトンデバッグ用処理【インライン関数／テンプレート関数定義部】

#include <gasha/lf_pool_allocator.cpp.h>//ロックフリープールアロケータ【関数定義部】
#include <gasha/linked_list.cpp.h>//双方向連結リスト【関数定義部】

#include <gasha/utility.h>//汎用ユーティリティ：getSysElapsedTime()

//--------------------------------------------------------------------------------
//シングルトンデバッグ用処理
//--------------------------------------------------------------------------------

#ifdef GASHA_SINGLETON_DEBUG_ENABLED//シングルトンデバッグ用処理有効時

//----------------------------------------
//シングルトンデバッグ用ダミー処理

//シングルトン生成時呼び出し
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
bool singletonDebug<_MAX_RECORDS, LOCK_TYPE>::create(const char* procedure_name)
{
	m_createdProcedureName = procedure_name;
	m_createdSysTime = nowSysElapsedTime();
	m_destroyedProcedureName = nullptr;
	m_destroyedSysTime = 0.;
	return true;
}

//シングルトン破棄時呼び出し
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
bool singletonDebug<_MAX_RECORDS, LOCK_TYPE>::destroy(const char* procedure_name)
{
	m_destroyedProcedureName = procedure_name;
	m_destroyedSysTime = nowSysElapsedTime();
	return true;
}

//シングルトンアクセス開始時呼び出し
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
typename singletonDebug<_MAX_RECORDS, LOCK_TYPE>::id_type singletonDebug<_MAX_RECORDS, LOCK_TYPE>::enter(const char* procedure_name)
{
	std::size_t seq_no = 0xffffffff;
	accessInfo* info = m_pool.newObj();
	if (info)
	{
		seq_no = m_seqNo.fetch_add(1);
		info->m_seqNo = seq_no;
		info->m_procedureName = procedure_name;
		info->m_sysTime = nowSysElapsedTime();
		{
			auto lock = m_list.lockScoped();
			m_list.push_back(*info);
		}
	}
	m_accessCount.fetch_add(1);
	return seq_no;
}

//シングルトンアクセス終了時呼び出し
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
bool singletonDebug<_MAX_RECORDS, LOCK_TYPE>::leave(const typename singletonDebug<_MAX_RECORDS, LOCK_TYPE>::id_type id)
{
	auto lock = m_list.lockScoped();
	accessInfo* info = m_list.findValue(id);
	if (info)
		m_list.remove(*info);
	m_accessCount.fetch_sub(1);
	return true;
}

//コンストラクタ
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
singletonDebug<_MAX_RECORDS, LOCK_TYPE>::singletonDebug() :
	m_pool(),
	m_list(),
	m_createdProcedureName(nullptr),
	m_destroyedProcedureName(nullptr),
	m_createdSysTime(0.),
	m_destroyedSysTime(0.),
	m_accessCount(0),
	m_seqNo(0)
{}

//デストラクタ
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
singletonDebug<_MAX_RECORDS, LOCK_TYPE>::~singletonDebug()
{}

#endif//GASHA_SINGLETON_DEBUG_ENABLED

#ifdef GASHA_SINGLETON_DEBUG_ENABLED//シングルトンデバッグ用処理有効時

//シングルトンデバッグ用処理の明示的なインスタンス化用マクロ
//※ロック型省略版
#define GASHA_INSTANCING_singletonDebug(_MAX_RECORDS) \
	template class linked_list::container<typename singletonDebug<_MAX_RECORDS>::listOpe>; \
	template class singletonDebug<_MAX_RECORDS>;
//※ロック型指定版
#define GASHA_INSTANCING_singletonDebug_withLock(_MAX_RECORDS, LOCK_TYPE) \
	template class linked_list::container<typename singletonDebug<_MAX_RECORDS, LOCK_TYPE>::listOpe>; \
	template class singletonDebug<_MAX_RECORDS, LOCK_TYPE>;

#else//GASHA_SINGLETON_DEBUG_ENABLED//シングルトンデバッグ用処理無効時

//シングルトンデバッグ用処理の明示的なインスタンス化用マクロ
//※ロック型省略版
#define GASHA_INSTANCING_singletonDebug(_MAX_RECORDS) \
	template class singletonDebug<_MAX_RECORDS>;
//※ロック型指定版
#define GASHA_INSTANCING_singletonDebug_withLock(_MAX_RECORDS, LOCK_TYPE) \
	template class singletonDebug<_MAX_RECORDS, LOCK_TYPE>;

#endif//GASHA_SINGLETON_DEBUG_ENABLED

#endif//GASHA_INCLUDED_SINGLETON_DEBUG_CPP_H

// End of file

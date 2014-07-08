#pragma once
#ifndef GASHA_INCLUDED_SINGLETON_DEBUG_CPP_H
#define GASHA_INCLUDED_SINGLETON_DEBUG_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// singleton_debug.cpp.h
// シングルトンデバッグ用処理【関数／実体定義部】
//
// ※クラスのインスタンス化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。
// 　（コンパイル・リンク時間への影響を気にしないならOK）
// ※明示的なインスタンス化を避けたい場合は、ヘッダーファイルと共にインクルード。
// 　（この場合、実際に使用するメンバー関数しかインスタンス化されないので、対象クラスに不要なインターフェースを実装しなくても良い）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/singleton_debug.inl>//シングルトンデバッグ用処理【インライン関数／テンプレート関数定義部】

#include <gasha/lf_pool_allocator.cpp.h>//ロックフリープールアロケータ【関数／実体定義部】
#include <gasha/linked_list.cpp.h>//双方向連結リスト【関数／実体定義部】

#include <gasha/chrono.h>//時間系ユーティリティ：getSysElapsedTime()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】sprintf を使用すると、error C4996 が発生する
//  error C4996: 'sprintf': This function or variable may be unsafe. Consider using strncpy_fast_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

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
	m_createdSysTime = nowElapsedTime();
	m_destroyedProcedureName = nullptr;
	m_destroyedSysTime = 0.;
	return true;
}

//シングルトン破棄時呼び出し
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
bool singletonDebug<_MAX_RECORDS, LOCK_TYPE>::destroy(const char* procedure_name)
{
	m_destroyedProcedureName = procedure_name;
	m_destroyedSysTime = nowElapsedTime();
	return true;
}

//シングルトンアクセス開始時呼び出し
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
typename singletonDebug<_MAX_RECORDS, LOCK_TYPE>::id_type singletonDebug<_MAX_RECORDS, LOCK_TYPE>::enter(const char* procedure_name)
{
	std::size_t seq_no = 0xffffffff;
	accessInfo* info = m_allocator.newDefault();
	if (info)
	{
		seq_no = m_seqNo.fetch_add(1);
		info->m_seqNo = seq_no;
		info->m_procedureName = procedure_name;
		info->m_sysTime = nowElapsedTime();
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
	{
		m_list.remove(*info);
		m_allocator.deleteDefault(info);
	}
	m_accessCount.fetch_sub(1);
	return true;
}

//デバッグ情報作成
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
std::size_t singletonDebug<_MAX_RECORDS, LOCK_TYPE>::debugInfo(char* message)
{
	std::size_t size = 0;
	size += sprintf(message + size, "----- Debug Info for singletonDebug -----\n");
	size += sprintf(message + size, "Accessing Count: %d\n", m_accessCount.load());
	size += sprintf(message + size, "Created:         %.9lf sec, \"%s\"\n", m_createdSysTime, m_createdProcedureName);
	size += sprintf(message + size, "Destroyed:       %.9lf sec, \"%s\"\n", m_destroyedSysTime, m_destroyedProcedureName);
	{
		auto lock = m_list.lockSharedScoped();
		size += sprintf(message + size, "Access Info: (Count=%d)\n", m_list.size());
		for (auto& info : m_list)
		{
			size += sprintf(message + size, "  - [%d] %.9lf sec, \"%s\": thread=\"%s\"(0x%08x)\n", info.m_seqNo, info.m_sysTime, info.m_procedureName, info.m_threadId.name(), info.m_threadId.id());
		}
	}
	size += sprintf(message + size, "----------\n");
	return size;
}

//コンストラクタ
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
singletonDebug<_MAX_RECORDS, LOCK_TYPE>::singletonDebug() :
	m_allocator(),
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

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

#ifdef GASHA_SINGLETON_DEBUG_ENABLED//シングルトンデバッグ用処理有効時

//シングルトンデバッグ用処理の明示的なインスタンス化用マクロ
//※ロック型省略版
#define GASHA_INSTANCING_singletonDebug(_MAX_RECORDS) \
	template class singletonDebug<_MAX_RECORDS>; \
	template class linked_list::container<typename singletonDebug<_MAX_RECORDS>::listOpe>; \
	template class lfPoolAllocator_withType<typename singletonDebug<_MAX_RECORDS>::accessInfo, _MAX_RECORDS>;
//※ロック型指定版
#define GASHA_INSTANCING_singletonDebug_withLock(_MAX_RECORDS, LOCK_TYPE) \
	template class singletonDebug<_MAX_RECORDS, LOCK_TYPE>; \
	template class linked_list::container<typename singletonDebug<_MAX_RECORDS, LOCK_TYPE>::listOpe>; \
	template class lfPoolAllocator_withType<typename singletonDebug<_MAX_RECORDS>::accessInfo, _MAX_RECORDS>;

#else//GASHA_SINGLETON_DEBUG_ENABLED//シングルトンデバッグ用処理無効時

//シングルトンデバッグ用処理の明示的なインスタンス化用マクロ
//※ロック型省略版
#define GASHA_INSTANCING_singletonDebug(_MAX_RECORDS) \
	template class singletonDebug<_MAX_RECORDS>;
//※ロック型指定版
#define GASHA_INSTANCING_singletonDebug_withLock(_MAX_RECORDS, LOCK_TYPE) \
	template class singletonDebug<_MAX_RECORDS, LOCK_TYPE>;

#endif//GASHA_SINGLETON_DEBUG_ENABLED

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_SINGLETON_DEBUG_CPP_H

// End of file

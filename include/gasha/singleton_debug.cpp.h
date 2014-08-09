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
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/singleton_debug.inl>//シングルトンデバッグ用処理【インライン関数／テンプレート関数定義部】

#include <gasha/lf_pool_allocator.cpp.h>//ロックフリープールアロケータ【関数／実体定義部】
#include <gasha/linked_list.cpp.h>//双方向連結リスト【関数／実体定義部】

#include <gasha/chrono.h>//時間処理ユーティリティ：nowElapsedTime()
#include <gasha/string.h>//文字列処理：spprintf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シングルトンデバッグ用処理
//--------------------------------------------------------------------------------

#ifdef GASHA_SINGLETON_DEBUG_ENABLED//シングルトンデバッグ用処理有効時

//----------------------------------------
//シングルトンデバッグ用ダミー処理

//シングルトン生成時呼び出し
template<std::size_t _MAX_RECORDS, class LOCK_POLICY>
bool singletonDebug<_MAX_RECORDS, LOCK_POLICY>::create(const char* procedure_name)
{
	//m_accessCount.store(0);//アクセスカウントは初期化しない
	m_createdProcedureName = procedure_name;
	m_createdSysTime = nowElapsedTime();
	m_destroyedProcedureName = nullptr;
	m_destroyedSysTime = 0.;
	return true;
}

//シングルトン破棄時呼び出し
template<std::size_t _MAX_RECORDS, class LOCK_POLICY>
bool singletonDebug<_MAX_RECORDS, LOCK_POLICY>::destroy(const char* procedure_name)
{
	m_destroyedProcedureName = procedure_name;
	m_destroyedSysTime = nowElapsedTime();
	return true;
}

//シングルトンアクセス開始時呼び出し
template<std::size_t _MAX_RECORDS, class LOCK_POLICY>
typename singletonDebug<_MAX_RECORDS, LOCK_POLICY>::id_type singletonDebug<_MAX_RECORDS, LOCK_POLICY>::enter(const char* procedure_name)
{
	id_type seq_no = invalidId();
	accessInfo* info = m_allocator.newDefault();
	if (info)
	{
		seq_no = m_seqNo.fetch_add(1);
		info->m_seqNo = seq_no;
		info->m_procedureName = procedure_name;
		info->m_sysTime = nowElapsedTime();
		{
			auto lock = m_list.lockScoped();//排他ロック
			m_list.push_back(*info);
		}
	}
	m_accessCount.fetch_add(1);
	return seq_no;
}

//シングルトンアクセス終了時呼び出し
template<std::size_t _MAX_RECORDS, class LOCK_POLICY>
bool singletonDebug<_MAX_RECORDS, LOCK_POLICY>::leave(const typename singletonDebug<_MAX_RECORDS, LOCK_POLICY>::id_type id)
{
	if (isInvalidId(id))
		return false;
	auto lock = m_list.lockScoped();//排他ロック
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
template<std::size_t _MAX_RECORDS, class LOCK_POLICY>
std::size_t singletonDebug<_MAX_RECORDS, LOCK_POLICY>::debugInfo(char* message, const std::size_t max_size) const
{
	std::size_t message_len = 0;
	GASHA_ spprintf(message, max_size, message_len, "----- Debug-info for singletonDebug -----\n");
	GASHA_ spprintf(message, max_size, message_len, "Accessing Count: %d\n", m_accessCount.load());
	GASHA_ spprintf(message, max_size, message_len, "Created:         %.9lf sec, \"%s\"\n", m_createdSysTime, m_createdProcedureName);
	GASHA_ spprintf(message, max_size, message_len, "Destroyed:       %.9lf sec, \"%s\"\n", m_destroyedSysTime, m_destroyedProcedureName);
	{
		auto lock = m_list.lockSharedScoped();//排他ロック
		GASHA_ spprintf(message, max_size, message_len, "Access Info: (Count=%d)\n", m_list.size());
		for (auto& info : m_list)
		{
			GASHA_ spprintf(message, max_size, message_len, "  - [%lld] %.9lf sec, \"%s\": thread=\"%s\"(0x%08x)\n", info.m_seqNo, info.m_sysTime, info.m_procedureName, info.m_threadId.name(), info.m_threadId.id());
		}
	}
	GASHA_ spprintf(message, max_size, message_len, "-----------------------------------------");//最終行改行なし
	return message_len;
}

//コンストラクタ
template<std::size_t _MAX_RECORDS, class LOCK_POLICY>
singletonDebug<_MAX_RECORDS, LOCK_POLICY>::singletonDebug() :
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
template<std::size_t _MAX_RECORDS, class LOCK_POLICY>
singletonDebug<_MAX_RECORDS, LOCK_POLICY>::~singletonDebug()
{}

#endif//GASHA_SINGLETON_DEBUG_ENABLED

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化：シングルトンデバッグ用処理有効時
#ifdef GASHA_SINGLETON_DEBUG_ENABLED

//シングルトンデバッグ用処理の明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_singletonDebug(_MAX_RECORDS) \
	template class GASHA_ singletonDebug<_MAX_RECORDS>; \
	template class GASHA_ linked_list::container<typename GASHA_ singletonDebug<_MAX_RECORDS>::listOpe>;
//※ロック指定版
#define GASHA_INSTANCING_singletonDebug_withLock(_MAX_RECORDS, LOCK_POLICY) \
	template class GASHA_ singletonDebug<_MAX_RECORDS, LOCK_POLICY>; \
	template class GASHA_ linked_list::container<typename GASHA_ singletonDebug<_MAX_RECORDS, LOCK_POLICY>::listOpe>;

//※別途、必要に応じてロックフリープールアロケータの明示的なインスタンス化も必要
//　　GASHA_INSTANCING_lfPoolAllocator(_MAX_RECORDS);//※ロックなし版
//　　GASHA_INSTANCING_lfPoolAllocator_withLock(_MAX_RECORDS, LOCK_POLICY);//※ロック指定版

//----------------------------------------
//明示的なインスタンス化：シングルトンデバッグ用処理無効時
#else//GASHA_SINGLETON_DEBUG_ENABLED

//シングルトンデバッグ用処理の明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_singletonDebug(_MAX_RECORDS) \
	template class GASHA_ singletonDebug<_MAX_RECORDS>;
//※ロック指定版
#define GASHA_INSTANCING_singletonDebug_withLock(_MAX_RECORDS, LOCK_POLICY) \
	template class GASHA_ singletonDebug<_MAX_RECORDS, LOCK_POLICY>;

#endif//GASHA_SINGLETON_DEBUG_ENABLED

//--------------------------------------------------------------------------------
//【注】明示的インスタンス化に失敗する場合
// ※このコメントは、「明示的なインスタンス化マクロ」が定義されている全てのソースコードに
// 　同じ内容のものをコピーしています。
//--------------------------------------------------------------------------------
//【原因①】
// 　対象クラスに必要なインターフェースが実装されていない。
//
// 　例えば、ソート処理に必要な「bool operator<(const value_type&) const」か「friend bool operator<(const value_type&, const value_type&)」や、
// 　探索処理に必要な「bool operator==(const key_type&) const」か「friend bool operator==(const value_type&, const key_type&)」。
//
// 　明示的なインスタンス化を行う場合、実際に使用しない関数のためのインターフェースも確実に実装する必要がある。
// 　逆に言えば、明示的なインスタンス化を行わない場合、使用しない関数のためのインターフェースを実装する必要がない。
//
//【対策１】
// 　インターフェースをきちんと実装する。
// 　（無難だが、手間がかかる。）
//
//【対策２】
// 　明示的なインスタンス化を行わずに、.cpp.h をテンプレート使用前にインクルードする。
// 　（手間がかからないが、コンパイル時の依存ファイルが増えるので、コンパイルが遅くなる可能性がある。）
//
//--------------------------------------------------------------------------------
//【原因②】
// 　同じ型のインスタンスが複数作成されている。
//
// 　通常、テンプレートクラス／関数の同じ型のインスタンスが複数作られても、リンク時に一つにまとめられるため問題がない。
// 　しかし、一つのソースファイルの中で複数のインスタンスが生成されると、コンパイラによってはエラーになる。
//   GCCの場合のエラーメッセージ例：（VC++ではエラーにならない）
// 　  source_file.cpp.h:114:17: エラー: duplicate explicit instantiation of ‘class templateClass<>’ [-fpermissive]
//
//【対策１】
// 　別のファイルに分けてインスタンス化する。
// 　（コンパイルへの影響が少なく、良い方法だが、無駄にファイル数が増える可能性がある。）
//
//【対策２】
// 　明示的なインスタンス化を行わずに、.cpp.h をテンプレート使用前にインクルードする。
// 　（手間がかからないが、コンパイル時の依存ファイルが増えるので、コンパイルが遅くなる可能性がある。）
//
//【対策３】
// 　GCCのコンパイラオプションに、 -fpermissive を指定し、エラーを警告に格下げする。
// 　（最も手間がかからないが、常時多数の警告が出る状態になりかねないので注意。）
//--------------------------------------------------------------------------------

#endif//GASHA_INCLUDED_SINGLETON_DEBUG_CPP_H

// End of file

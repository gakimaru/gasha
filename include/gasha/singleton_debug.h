#pragma once
#ifndef GASHA_INCLUDED_SINGLETON_DEBUG_H
#define GASHA_INCLUDED_SINGLETON_DEBUG_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ】
// singleton_debug.h
// シングルトンデバッグ用処理【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dummy_shared_lock.h>//ダミー共有ロック
#include <gasha/unique_shared_lock.h>//単一共有ロック

#include <gasha/thread_id.h>//スレッドID
#include <gasha/lf_pool_allocator.h>//ロックフリープールアロケータ【宣言部】
#include <gasha/linked_list.h>//双方向連結リスト【宣言部】
#include <gasha/chrono.h>//時間処理ユーティリティ

#include <cstddef>//std::size_t
#include <atomic>//C++11 st::atomic

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シングルトンデバッグ用処理
//--------------------------------------------------------------------------------

#ifdef GASHA_SINGLETON_DEBUG_ENABLED//シングルトンデバッグ用処理有効時

//----------------------------------------
//シングルトンデバッグ用処理
template<std::size_t _MAX_RECORDS, class LOCK_TYPE = GASHA_ dummySharedLock>
class singletonDebug
{
public:
	//型
	typedef std::size_t id_type;//ID型（アクセス中情報のポインタ）
	struct accessInfo//シングルトンアクセス中処理情報
	{
		mutable const accessInfo* m_next;//次の連結ノード
		mutable const accessInfo* m_prev;//前の連結ノード
		std::size_t m_seqNo;//シーケンス番号
		const char* m_procedureName;//手続き名
		threadId m_threadId;//スレッドID
		GASHA_ time_type m_sysTime;//システム時間
		inline bool operator<(const accessInfo& rhs) const { return m_seqNo < rhs.m_seqNo; }
		inline bool operator==(const id_type id) const { return m_seqNo == id; }
	};
	struct listOpe : public linked_list::baseOpe<listOpe, accessInfo>//連結リスト操作型
	{
		inline static const accessInfo* getNext(const accessInfo& node){ return node.m_next; }
		inline static const accessInfo* getPrev(const accessInfo& node){ return node.m_prev; }
		inline static void setNext(accessInfo& node, const accessInfo* next){ node.m_next = next; }
		inline static void setPrev(accessInfo& node, const accessInfo* prev){ node.m_prev = prev; }
		typedef LOCK_TYPE lock_type;//ロックオブジェクト型
	};
	typedef linked_list::container<listOpe> list_type;//双方向連結リスト型
public:
	//定数
	static const std::size_t MAX_RECORDS = _MAX_RECORDS;//記録数
public:
	//キャストオペレータ
	inline operator list_type&(){ return m_list; }//アクセス情報の連結情報
	inline operator const list_type&() const { return m_list; }//アクセス情報の連結情報
public:
	//アクセッサ
	inline list_type& list(){ return m_list; }//アクセス情報の連結情報
	inline const list_type& list() const { return m_list; }//アクセス情報の連結情報
	const char* createdProcedureName() const { return m_createdProcedureName; }//インスタンス生成手続き名
	const char* destroyedProcedureName() const { return m_destroyedProcedureName; }//インスタンス破棄手続き名
	GASHA_ time_type createdSysTime() const { return m_createdSysTime; }//インスタンス生成時システム時間
	GASHA_ time_type destroyedSysTime() const { return m_destroyedSysTime; }//インスタンス破棄時システム時間
	std::size_t accessCount() const { return m_accessCount.load(); }//アクセス中のカウント数
public:
	//シングルトン生成時呼び出し
	bool create(const char* procedure_name);
	//シングルトン破棄時呼び出し
	bool destroy(const char* procedure_name);
	//シングルトンアクセス開始時呼び出し
	id_type enter(const char* procedure_name);
	//シングルトンアクセス終了時呼び出し
	bool leave(const id_type id);
	//デバッグ情報作成
	//※十分なサイズのバッファを渡す必要あり。
	//※使用したバッファのサイズを返す。
	//※作成中、ロックを取得する。
	std::size_t debugInfo(char* message) const;
public:
	//コンストラクタ
	singletonDebug();
	//デストラクタ
	~singletonDebug();
private:
	//フィールド
	lfPoolAllocator_withType<accessInfo, MAX_RECORDS> m_allocator;//アクセス情報のプール情報
	list_type m_list;//アクセス情報の連結情報
	const char* m_createdProcedureName;//インスタンス生成手続き名
	const char* m_destroyedProcedureName;//インスタンス破棄手続き名
	GASHA_ time_type m_createdSysTime;//インスタンス生成時システム時間
	GASHA_ time_type m_destroyedSysTime;//インスタンス破棄時システム時間
	std::atomic<std::size_t> m_accessCount;//アクセス中のカウント数
	std::atomic<std::size_t> m_seqNo;//シーケンス番号
};

#else//GASHA_SINGLETON_DEBUG_ENABLED//シングルトンデバッグ用処理無効時

//----------------------------------------
//シングルトンデバッグ用処理
template<std::size_t MAX_RECORDS, class LOCK_TYPE = GASHA_ dummySharedLock>
class singletonDebug
{
public:
	//型
	struct id_type {};
public:
	//シングルトン生成時呼び出し
	inline bool create(const char* procedure_name);
	//シングルトン破棄時呼び出し
	inline bool destroy(const char* procedure_name);
	//シングルトンアクセス開始時呼び出し
	inline id_type enter(const char* procedure_name);
	//シングルトンアクセス終了時呼び出し
	inline bool leave(const id_type id);
	//デバッグ情報作成
	inline std::size_t debugInfo(char* message) const;
};

#endif//GASHA_SINGLETON_DEBUG_ENABLED

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/singleton_debug.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_SINGLETON_DEBUG_ALLWAYS_TOGETHER_CPP_H
#include <gasha/singleton_debug.cpp.h>
#endif//GASHA_SINGLETON_DEBUG_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_SINGLETON_DEBUG_H

// End of file

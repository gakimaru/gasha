#pragma once
#ifndef GASHA_INCLUDED_LOG_QUEUE_INL
#define GASHA_INCLUDED_LOG_QUEUE_INL

//--------------------------------------------------------------------------------
// log_queue.inl
// ログキュー【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_queue.h>//ログキュー【宣言部】

#include <gasha/lock_common.h>//ロック共通設定
#include <gasha/lf_pool_allocator.h>//ロックフリープールアロケータ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログキュー
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//キューイング予約
inline logQueue::id_type logQueue::reserve(const int num)
{
	id_type id = m_id.fetch_add(num);
	if (id == 0)//ID=0は許されないので、もし 0 だったら再発行する ※範囲オーバーで値が巡回する可能性は考慮しない
		id = m_id.fetch_add(num);
	return id;
}

//デキュー
inline bool logQueue::dequeue(GASHA_ logPrintInfo& info)
{
	return m_queue.popCopying(info);
}

//リリース
inline void logQueue::release(const char* message)
{
	m_messageBuff.free(const_cast<char*>(message));
}
inline void logQueue::release(GASHA_ logPrintInfo& info)
{
	release(info.m_message);
}

//中断
inline void logQueue::abort()
{
	m_abort.store(true);//中断
}

//一時停止
inline void logQueue::pause()
{
	m_pause.store(true);//一時停止
}

//一時停止から再開
inline void logQueue::resume()
{
	m_pause.store(true);//一時停止解除
}

//明示的な初期化用コンストラクタ
inline logQueue::logQueue(const explicitInitialize_t&)
{
	initializeOnce();//コンテナ初期化
	auto dummy = [](){};
	std::call_once(m_initialized, dummy);//強制的に初期化済みにする
}

//デフォルトコンストラクタ
inline logQueue::logQueue()
{
#ifdef GASHA_LOG_QUEUE_SECURE_INITIALIZE
	std::call_once(m_initialized, initializeOnce);//コンテナ初期化（一回限り）
#endif//GASHA_LOG_QUEUE_SECURE_INITIALIZE
}

//デストラクタ
inline logQueue::~logQueue()
{}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_QUEUE_INL

// End of file

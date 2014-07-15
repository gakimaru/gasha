#pragma once
#ifndef GASHA_INCLUDED_LOG_QUEUE_MONITOR_INL
#define GASHA_INCLUDED_LOG_QUEUE_MONITOR_INL

//--------------------------------------------------------------------------------
// log_queue_monitor.inl
// ログキューモニター【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_queue_monitor.h>//ログキューモニター【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログキューモニター
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//中断
inline void logQueueMonitor::abort()
{
	//すでに中断時は即終了
	if (m_abort.load())
		return;

	//中断
	m_abort.store(true);

	//通知
	m_cond.notify_one();
}

//リセット
inline void logQueueMonitor::reset()
{
	//次のIDをリセット
	m_nextId.store(INIT_ID);
}

//明示的な初期化用コンストラクタ
inline logQueueMonitor::logQueueMonitor(const explicitInitialize_t&)
{
	initializeOnce();//コンテナ初期化
	auto dummy = [](){};
	std::call_once(m_initialized, dummy);//強制的に初期化済みにする
}

//デフォルトコンストラクタ
inline logQueueMonitor::logQueueMonitor()
{
#ifdef GASHA_LOG_QUEUE_MONITOR_SECURE_INITIALIZE
	std::call_once(m_initialized, initializeOnce);//コンテナ初期化（一回限り）
#endif//GASHA_LOG_QUEUE_MONITOR_SECURE_INITIALIZE
}

//デストラクタ
inline logQueueMonitor::~logQueueMonitor()
{}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_QUEUE_MONITOR_INL

// End of file

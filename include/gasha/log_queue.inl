﻿#pragma once
#ifndef GASHA_INCLUDED_LO_QUEUE_INL
#define GASHA_INCLUDED_LO_QUEUE_INL

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

//デキュー
inline bool logQueue::dequeue(node_type& node)
{
	return m_queue.popCopying(node);
}

//リリース
inline void logQueue::release(const char* message)
{
	m_messageBuff.free(const_cast<char*>(message));
}
inline void logQueue::release(node_type& node)
{
	release(node.m_message);
}

//中断
inline void logQueue::abort()
{
	m_abort.store(true);//中断
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
#ifdef GASHA_LO_QUEUE_SECURE_INITIALIZE
	std::call_once(m_initialized, initializeOnce);//コンテナ初期化（一回限り）
#endif//GASHA_LO_QUEUE_SECURE_INITIALIZE
}

//デストラクタ
inline logQueue::~logQueue()
{}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LO_QUEUE_INL

// End of file

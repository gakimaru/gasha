#pragma once
#ifndef GASHA_INCLUDED_LOG_WORK_BUFF_INL
#define GASHA_INCLUDED_LOG_WORK_BUFF_INL

//--------------------------------------------------------------------------------
// log_work_buff.inl
// ログワークバッファ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_work_buff.h>//ログワークバッファ【宣言部】

#include <gasha/lock_common.h>//ロック共通設定
#include <gasha/lf_pool_allocator.h>//ロックフリープールアロケータ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログワークバッファ
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//ワークバッファの解放
inline void logWorkBuff::free(char* p)
{
	m_workBuff.free(p);
}

//中断
inline void logWorkBuff::abort()
{
	m_abort.store(true);//中断
}

//明示的な初期化用コンストラクタ
inline logWorkBuff::logWorkBuff(const explicitInitialize_t&)
{
	initializeOnce();//コンテナ初期化
	auto dummy = [](){};
	std::call_once(m_initialized, dummy);//強制的に初期化済みにする
}

//デフォルトコンストラクタ
inline logWorkBuff::logWorkBuff()
{
#ifdef GASHA_LOG_WORK_BUFF_SECURE_INITIALIZE
	std::call_once(m_initialized, initializeOnce);//コンテナ初期化（一回限り）
#endif//GASHA_LOG_WORK_BUFF_SECURE_INITIALIZE
}

//デストラクタ
inline logWorkBuff::~logWorkBuff()
{}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_WORK_BUFF_INL

// End of file

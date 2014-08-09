#pragma once
#ifndef GASHA_INCLUDED_LOG_WORK_BUFF_INL
#define GASHA_INCLUDED_LOG_WORK_BUFF_INL

//--------------------------------------------------------------------------------
// log_work_buff.inl
// ログワークバッファ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_work_buff.h>//ログワークバッファ【宣言部】

#include <gasha/lf_pool_allocator.h>//ロックフリープールアロケータ【インライン関数／テンプレート関数定義部】

#include <gasha/lock_common.h>//ロック共通設定
#include <gasha/fast_string.h>//高速文字列処理

#include <cstring>//std::memcpy()
#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログワークバッファ
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

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

//一時停止
inline void logWorkBuff::pause()
{
	m_pause.store(true);//一時停止
}

//一時停止から再開
inline void logWorkBuff::resume()
{
	m_pause.store(false);//一時停止解除
}

//spprintf
template<typename... Tx>
inline std::size_t logWorkBuff::spprintf(char* message, std::size_t& pos, const char* fmt, Tx&&... args)
{
	return GASHA_ spprintf(message, MAX_MESSAGE_SIZE, pos, fmt, std::forward<Tx>(args)...);
}

//spprintf ※バッファサイズ半分版
template<typename... Tx>
inline std::size_t logWorkBuff::spprintf_halfSized(char* message, std::size_t& pos, const char* fmt, Tx&&... args)
{
	return GASHA_ spprintf(message, HALF_MESSAGE_SIZE, pos, fmt, std::forward<Tx>(args)...);
}

//明示的な初期化用コンストラクタ
inline logWorkBuff::logWorkBuff(const explicitInit_tag&)
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

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_WORK_BUFF_INL

// End of file

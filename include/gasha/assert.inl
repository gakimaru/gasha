#pragma once
#ifndef GASHA_INCLUDED_ASSERT_INL
#define GASHA_INCLUDED_ASSERT_INL

//--------------------------------------------------------------------------------
// assert.inl
// アサーション／ブレークポイント／ウォッチポイント【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/assert.h>//アサーション／ブレークポイント／ウォッチポイント【宣言部】

#include <gasha/std_debug_pause.h>//標準デバッグポーズ
#include <gasha/log_mask.h>//ログレベルマスク

#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アサーション／ブレークポイント／ウォッチポイント
//--------------------------------------------------------------------------------

#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

//ブレークポイント
template<typename... Tx>
bool breakPoint::operator()(const bool conditon, const GASHA_ log::level_type level, const GASHA_ log::category_type category, const char* expr, const char* file_name, const char* func_name, const char* message, Tx&&... args)
{
	//条件判定式が false なら何もしない
	if (!conditon)
		return false;

	//ログレベルマスク（ログ出力用）判定
	logMask mask;
	if (!mask.isEnableLevel(ofLog, level, category))
		return false;

	//ログ出力用の追加メッセージ関数
	//※ユーザー指定のメッセージの後に、条件判定式の文字列、ソースファイル情報を付加する
	auto add_message_func = [&expr, &file_name, &func_name](char* message, const std::size_t max_size, std::size_t& pos) -> std::size_t
	{
		return GASHA_ spprintf(message, max_size, pos, "\n%sfunc: %s\nfile: %s\n", expr, func_name, file_name);
	};

	GASHA_ log log;

#ifdef GASHA_LOG_PRINT_USE_QUEUE
	
	//ログキュー使用版
	log.print(log::addCPStack, add_message_func, level, category, message, std::forward<Tx>(args)...);
	
	//ログキュー一時停止
	//※以後、他のスレッドがログ出力しようとしても待機状態になる
	log.pause();

	//ログ出力モニターフラッシュ
	//※溜まっているログが全て出力されるのを待つ
	log.flush();

	//ブレーク処理呼び出し
	if (m_tlsDebugPause)
		m_tlsDebugPause->breakPoint();

	//ポーズ処理呼び出し
	if (m_tlsDebugPause)
		m_tlsDebugPause->pause();

	//ログ一時停止解除
	log.resume();

#else//GASHA_LOG_PRINT_USE_QUEUE
	
	//直接出力版
	log.printDirect(log::addCPStack, GASHA_ stdLogPrint(), add_message_func, level, category, message, std::forward<Tx>(args)...);

	//ブレーク処理呼び出し
	if (m_tlsDebugPause)
		m_tlsDebugPause->breakPoint();

	//ポーズ処理呼び出し
	if (m_tlsDebugPause)
		m_tlsDebugPause->pause();

#endif//GASHA_LOG_PRINT_USE_QUEUE
	return true;
}

//デバッグポーズ処理の取得
inline GASHA_ iDebugPause* breakPoint::debugPause()
{
	return m_tlsDebugPause;
}

//デバッグポーズ処理の変更
inline GASHA_ iDebugPause* breakPoint::changeDebugPause(GASHA_ iDebugPause* new_debug_pause)
{
	GASHA_ iDebugPause* old_debug_pause = m_tlsDebugPause;
	m_tlsDebugPause = new_debug_pause;
	return old_debug_pause;
}

//デバッグポーズ処理の変更
inline GASHA_ iDebugPause* breakPoint::changeDebugPause(GASHA_ iDebugPause& new_debug_pause)
{
	return changeDebugPause(&new_debug_pause);
}

//デバッグポーズ処理を標準処理に変更
inline GASHA_ iDebugPause* breakPoint::changeStdDebugPause()
{
	return changeDebugPause(GASHA_ stdDebugPause::instance());
}


//デフォルトコンストラクタ
inline breakPoint::breakPoint()
{
	if (!m_tlsDebugPause)//nullptrなら標準デバッグブレークをセット
		m_tlsDebugPause = &stdDebugPause::instance();
}

//デストラクタ
inline breakPoint::~breakPoint()
{}

#endif//GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ASSERT_INL

// End of file

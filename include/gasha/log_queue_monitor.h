﻿#pragma once
#ifndef GASHA_INCLUDED_LOG_QUEUE_MONITOR_H
#define GASHA_INCLUDED_LOG_QUEUE_MONITOR_H

//--------------------------------------------------------------------------------
// log_queue_monitor.h
// ログキューモニター【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_queue.h>//ログキュー

#include <gasha/i_console.h>//コンソールインターフェース
#include <gasha/console_color.h>//コンソールカラー

#include <gasha/log_purpose.h>//ログ用途
#include <gasha/log_level.h>//ログレベル
#include <gasha/log_category.h>//ログカテゴリ

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::int32_t
#include <atomic>//C++11 std::atomic
#include <condition_variable>//C++11 std::conditio_variable

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <mutex> <functional> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <mutex>//C++11 std::mutex, std::call_once
#include <functional>//C++11 std::function

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログキューモニター
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログキューモニター
class logQueueMonitor
{
public:
	//型
	typedef GASHA_ logPurpose::purpose_type purpose_type;//ログ用途の値
	typedef GASHA_ logCategory::category_type category_type;//ログカテゴリの値
	typedef GASHA_ logLevel::level_type level_type;//ログレベルの値
	typedef GASHA_ logQueue::id_type id_type;//キューIDの型
public:
	//定数
	static const purpose_type PURPOSE_NUM = GASHA_ logPurpose::NUM;//ログ用途の数
	static const int MAX_RETRY_COUNT = GASHA_LOG_QUEUE_MONITOR_MAX_RETRY_COUNT;//次のIDのキューが来ない時にリトライ（待機）する最大回数
	static const id_type INIT_ID = GASHA_ logQueue::INIT_ID;//初期ID

public:
	//型
	struct explicitInitialize_t{};//明示的な初期化用構造体

public:
	//モニター時の出力用デフォルト関数
	static void defaultOutput(GASHA_ logQueue::node_type& node);

	//モニター
	//※マルチスレッド専用。実行するとループ処理に入る。
	static void monitor(std::function<void(GASHA_ logQueue::node_type&)> output = defaultOutput);
	
	//キューイングを通知
	//※確実にキューイングが成功した時にだけ呼ぶこと
	//　（キューの数と通知の数に不整合があると、スレッドが終了できなくなる）
	bool notify();

	//フラッシュ
	//※すべての出力が完了するのを待つ
	//※中断時は即終了する
	bool flush();

	//中断
	//※モニターを中断する。
	//※一度中断したら、モニターを開始できなくなるため、
	//　再度使用するには明示的な初期化が必要
	inline void abort();

	//リセット
	//※キューが中断／再初期化された際などに呼び出し、「次のID」をリセットする
	inline void reset();

public:
	//明示的な初期化用コンストラクタ
	inline logQueueMonitor(const explicitInitialize_t&);
	//デフォルトコンストラクタ
	inline logQueueMonitor();
	//デストラクタ
	inline ~logQueueMonitor();

private:
	//初期化メソッド（一回限り）
	static void initializeOnce();

public:
	//静的フィールド
	static const explicitInitialize_t explicitInitialize;//明示的な初期化指定用
private:
	//静的フィールド
	static std::once_flag m_initialized;//初期化済み
	static std::atomic<bool> m_abort;//中断
	static std::atomic<std::int32_t> m_request;//要求数
	static std::atomic<std::int32_t> m_flush;//フラッシュ要求数
	static std::atomic<id_type> m_nextId;//次のID（先に大きなIDが来たら、次のIDが来るまで待つ）
	static std::mutex m_mutex;//ミューテックス
	static std::condition_variable m_cond;//条件変数
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_queue_monitor.inl>

#endif//GASHA_INCLUDED_LOG_QUEUE_MONITOR_H

// End of file

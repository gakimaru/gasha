#pragma once
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
#include <gasha/log_attr.h>//ログ属性
#include <gasha/log_print_info.h>//ログ出力情報

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::int32_t
#include <atomic>//C++11 std::atomic
#include <condition_variable>//C++11 std::condition_variable

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <mutex>//C++11 std::mutex, std::call_once
#include <functional>//C++11 std::function
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログキューモニター
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログキューモニター
class logQueueMonitor
{
public:
	//型
	typedef GASHA_ logPrintInfo::id_type id_type;//IDの型
	typedef GASHA_ logPurpose::purpose_type purpose_type;//ログ用途の値
	typedef GASHA_ logCategory::category_type category_type;//ログカテゴリの値
	typedef GASHA_ logLevel::level_type level_type;//ログレベルの値
public:
	//定数
	static const purpose_type PURPOSE_NUM = GASHA_ logPurpose::NUM;//ログ用途の数
	static const int MAX_RETRY_COUNT = GASHA_LOG_QUEUE_MONITOR_MAX_RETRY_COUNT;//次のIDのキューが来ない時にリトライ（待機）する最大回数
	static const id_type INIT_ID = GASHA_ logQueue::INIT_ID;//初期ID

public:
	//型
	struct explicitInit_type{};//明示的な初期化用構造体

public:
	//モニター
	//※マルチスレッド専用。
	//※実行するとループ処理に入る。
	//※終了するには、他のスレッドから abort() を呼び出す。
	//※ログ表示用関数を指定する。
	//　PRINT_FUNC のプロトタイプ：void print_func(GASHA_ logPrintInfo& info)
	template<class PRINT_FUNC>
	void monitor(PRINT_FUNC print_func);
	inline void monitor();
	
	//キューイングを通知
	//※確実にキューイングが成功した時にだけ呼ぶこと
	//　（キューの数と通知の数に不整合があると、スレッドが終了できなくなる）
	bool notify();

	//ID欠番通知
	//※飛ばして良いIDを指定する。
	//※指定のIDの次のIDが「次のID」としてセットされる。
	//※既に「次のID」が進行していた場合は何もせず、false を返す。
	bool skip(const id_type skip_id);

	//フラッシュ
	//※すべての出力が完了するのを待つ
	//※中断時は即終了する
	bool flush();

	//中断
	//※モニターを中断する。
	//※一度中断したら、モニターを開始できなくなるため、
	//　再度使用するには明示的な初期化が必要
	void abort();

	//リセット
	//※キューが中断／再初期化された際などに呼び出し、「次のID」をリセットする
	inline void reset();

public:
	//明示的な初期化用コンストラクタ
	inline logQueueMonitor(const explicitInit_type&);
	//デフォルトコンストラクタ
	inline logQueueMonitor();
	//デストラクタ
	inline ~logQueueMonitor();

private:
	//初期化メソッド（一回限り）
	static void initializeOnce();

private:
	//静的フィールド
	static std::once_flag m_initialized;//初期化済み
	static std::atomic<bool> m_abort;//中断
	static std::atomic<bool> m_isEnd;//終了済み
	static std::atomic<std::int32_t> m_request;//要求数
	static std::atomic<std::int32_t> m_flush;//フラッシュ要求数
	static std::atomic<id_type> m_nextId;//次のID（先に大きなIDが来たら、次のIDが来るまで待つ）
	static std::mutex m_mutex;//ミューテックス
	static std::condition_variable m_cond;//条件変数
public:
	//静的フィールド
	static const explicitInit_type explicitInit;//明示的な初期化指定用
};

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_queue_monitor.inl>

#endif//GASHA_INCLUDED_LOG_QUEUE_MONITOR_H

// End of file

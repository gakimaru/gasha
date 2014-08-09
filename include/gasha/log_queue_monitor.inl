#pragma once
#ifndef GASHA_INCLUDED_LOG_QUEUE_MONITOR_INL
#define GASHA_INCLUDED_LOG_QUEUE_MONITOR_INL

//--------------------------------------------------------------------------------
// log_queue_monitor.inl
// ログキューモニター【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_queue_monitor.h>//ログキューモニター【宣言部】

#include <gasha/std_log_print.h>//標準ログ出力

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログキューモニター
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//モニター
template<class PRINT_FUNC>
void logQueueMonitor::monitor(PRINT_FUNC print_func)
{
	m_isEnd.store(false);//終了状態を初期化
	m_nextId.store(INIT_ID);//次のID初期化
	int retry_count = MAX_RETRY_COUNT;//次のIDが来るまでの最大リトライカウント
	while (true)
	{
		//キューイング待ち受け
		{
			//キューイング状態を判定するプリディケート関数
			auto pred = []() -> bool
			{
				return m_request.load() > 0 || m_flush.load() > 0 || m_abort.load() || m_isEnd.load();
			};

			//条件変数による待ち受け
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cond.wait(lock, pred);
		}

		//フラッシュ要求判定
		if (m_flush.load() > 0)
			m_flush.fetch_sub(1);

		//中断判定
		if (m_abort.load())
			break;

		//キューからメッセージを取得
		GASHA_ logPrintInfo print_info;
		GASHA_ logQueue queue;
		const id_type id = queue.top();
		if (id > 0)
		{
			//次のID（以下のID）が来なければコンテキストスイッチを発生させて待つ
			//※一定回数待っても次のIDが来なければ強制的に
			if (id <= m_nextId.load() || retry_count == 0)
			{
				//デキュー
				queue.dequeue(print_info);

				//出力
				print_func(print_info);

				//キューを解放
				queue.release(print_info);

				//要求数をカウントダウン
				m_request.fetch_sub(1);

				//次のIDを更新
				m_nextId.store(print_info.id());
				m_nextId.fetch_add(1);
				retry_count = MAX_RETRY_COUNT;//リトライカウントをリセット
			}
			else
			{
				//リトライ
				GASHA_ defaultContextSwitch();//コンテキストスイッチ
				--retry_count;
			}
		}
	}
	m_isEnd.store(true);//終了状態にする
}
inline void logQueueMonitor::monitor()
{
	monitor(GASHA_ stdLogPrint());
}

//リセット
inline void logQueueMonitor::reset()
{
	//次のIDをリセット
	m_nextId.store(INIT_ID);
}

//明示的な初期化用コンストラクタ
inline logQueueMonitor::logQueueMonitor(const explicitInit_tag&)
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

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_QUEUE_MONITOR_INL

// End of file

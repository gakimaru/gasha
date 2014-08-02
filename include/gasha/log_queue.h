#pragma once
#ifndef GASHA_INCLUDED_LOG_QUEUE_H
#define GASHA_INCLUDED_LOG_QUEUE_H

//--------------------------------------------------------------------------------
// log_queue.h
// ログキュー【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース
#include <gasha/console_color.h>//コンソールカラー

#include <gasha/log_purpose.h>//ログ用途
#include <gasha/log_level.h>//ログレベル
#include <gasha/log_category.h>//ログカテゴリ
#include <gasha/log_attr.h>//ログ属性
#include <gasha/log_print_info.h>//ログ出力情報

#include <gasha/lf_stack_allocator.h>//ロックフリースタックアロケータ
#include <gasha/binary_heap.h>//二分ヒープ ※ユニークなIDを扱うので、優先度付きキューを使う必要がない
#include <gasha/spin_lock.h>//スピンロック

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t
#include <atomic>//C++11 std::atomic

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <mutex>//C++11 std::call_once
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログキュー
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログキュー
class logQueue
{
public:
	//型
	typedef GASHA_ logPrintInfo::id_type id_type;//ID型（シーケンス番号を扱う）
	typedef GASHA_ logPurpose::purpose_type purpose_type;//ログ用途の値
	typedef GASHA_ logCategory::category_type category_type;//ログカテゴリの値
	typedef GASHA_ logLevel::level_type level_type;//ログレベルの値
public:
	//定数
	static const purpose_type PURPOSE_NUM = GASHA_ logPurpose::NUM;//ログ用途の数
	static const std::size_t MESSAGE_BUFF_SIZE = GASHA_LOG_QUEUE_MESSAGE_STACK_SIZE;//メッセージバッファサイズ
	static const std::size_t QUEUE_SIZE = GASHA_LOG_QUEUE_NODE_SIZE;//最大キュー数
	static const id_type INIT_ID = 1;//IDの初期値
#ifdef GASHA_LOG_QUEUE_NOWAIT
	static const bool IS_NO_WAIT_MODE = true;
#else//GASHA_LOG_QUEUE_NOWAIT
	static const bool IS_NO_WAIT_MODE = false;
#endif//GASHA_LOG_QUEUE_NOWAIT
public:
	//バイナリヒープ操作型
	struct queueOpe : public GASHA_ binary_heap::baseOpe<queueOpe, QUEUE_SIZE, GASHA_ logPrintInfo>
	{
		//ロック型
		typedef GASHA_ spinLock lock_type;//ロックオブジェクト型
	};
public:
	//型
	struct explicitInit_type{};//明示的な初期化用構造体

public:
	//エンキュー
	//※エンキューに成功したら true を返す。
	//※通常はエンキューが成功するまでリトライを繰り返す。
	//※GASHA_LOG_QUEUE_NOWAIT が指定されている場合、キュー用のバッファがいっぱいだとエンキューに失敗する。
	//※エンキュー待ち時に中断が指定されたら失敗する。
	//※引数 print_info.m_mesage_size に 0 を指定すると、print_info.m_message の長さを計測する。print_info.m_message には終端を含めたサイズを指定する。
	//※引数 print_info.m_id には予約したIDを指定する。通常は 0 で、0 を指定すると、キューイングの際にIDを自動発番する。
	bool enqueue(const logPrintInfo& print_info);

	//エンキュー予約
	//※予約数を指定する
	id_type reserve(const int num);

	//先頭キューのIDを取得
	//※キューの順序を確認して、次のキューを待つために使用。
	//※キューがなければ 0 を返す。※ID = 0 のキューはない
	id_type top();

	//デキュー
	//※キューのノードを渡して情報を受け取る。
	//※キューを取得できた場合、true を返す。
	inline bool dequeue(GASHA_ logPrintInfo& info);

	//リリース
	//※メッセージをバッファから解放する。
	inline void release(const char* message);
	inline void release(logPrintInfo& info);

	//中断
	//※エンキュー待ちがあったら中断させる
	//※一度中断したら、エンキューを許可しなくなるため、
	//　再度使用するには明示的な初期化が必要
	inline void abort();

	//一時停止
	inline void pause();

	//一時停止から再開
	inline void resume();

public:
	//明示的な初期化用コンストラクタ
	inline logQueue(const explicitInit_type&);
	//デフォルトコンストラクタ
	inline logQueue();
	//デストラクタ
	inline ~logQueue();

private:
	//初期化メソッド（一回限り）
	static void initializeOnce();

private:
	//静的フィールド
	static std::once_flag m_initialized;//初期化済み
	static std::atomic<bool> m_abort;//中断
	static std::atomic<bool> m_pause;//一時停止
	static std::atomic<id_type> m_id;//キューID発番用
	static GASHA_ lfSmartStackAllocator_withBuff<MESSAGE_BUFF_SIZE> m_messageBuff;//メッセージバッファ
	static GASHA_ binary_heap::container<queueOpe> m_queue;//ログキュー
public:
	//静的フィールド
	static const explicitInit_type explicitInit;//明示的な初期化指定用
};

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_queue.inl>

#endif//GASHA_INCLUDED_LOG_QUEUE_H

// End of file

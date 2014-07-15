#pragma once
#ifndef GASHA_INCLUDED_LO_QUEUE_H
#define GASHA_INCLUDED_LO_QUEUE_H

//--------------------------------------------------------------------------------
// log_queue.h
// ログキュー【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース
#include <gasha/console_color.h>//コンソールカラー
#include <gasha/log_purpose.h>//ログ用途
#include <gasha/log_level.h>//ログレベル
#include <gasha/log_category.h>//ログカテゴリ

#include <gasha/lf_stack_allocator.h>//ロックフリースタックアロケータ
#include <gasha/binary_heap.h>//二分ヒープ ※ユニークなIDを扱うので、優先度付きキューを使う必要がない
#include <gasha/spin_lock.h>//スピンロック

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t
#include <atomic>//C++11 std::atomic

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <mutex> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <mutex>//C++11 std::call_once

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログキュー
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログキュー
class logQueue
{
public:
	//型
	typedef std::uint64_t id_type;//キューID型（シーケンス番号）
	typedef GASHA_ logPurpose::purpose_type purpose_type;//ログ用途の値
	typedef GASHA_ logCategory::category_type category_type;//ログカテゴリの値
	typedef GASHA_ logLevel::level_type level_type;//ログレベルの値
public:
	//定数
	static const purpose_type PURPOSE_NUM = GASHA_ logPurpose::NUM;//ログ用途の数
public:
	//キューノード型
	struct node_type
	{
		id_type m_id;//キューID
		const char* m_message;//メッセージ
		level_type m_level;//ログレベル
		category_type m_category;//ログカテゴリ
		GASHA_ IConsole* m_consoles[PURPOSE_NUM];//ログ出力先
		const GASHA_ consoleColor* m_colors[PURPOSE_NUM];//ログ出力カラー

		//比較演算子
		inline bool operator<(const node_type& rhs) const;

		//コンストラクタ
		inline node_type(const id_type id, const char* message, level_type level, category_type category, GASHA_ IConsole* (&consoles)[PURPOSE_NUM], const GASHA_ consoleColor* (&colors)[PURPOSE_NUM]);
		
		//デフォルトコンストラクタ
		inline node_type();

		//デストラクタ
		inline ~node_type();
	};
	//バイナリヒープ操作型
	struct queueOpe : public GASHA_ binary_heap::baseOpe<queueOpe, node_type>
	{
		//ロック型
		typedef spinLock lock_type;//ロックオブジェクト型
	};
public:
	//型
	struct explicitInitialize_t{};//明示的な初期化用構造体
public:
	//定数
	static const std::size_t MESSAGE_BUFF_SIZE = GASHA_LOG_QUEUE_MESSAGE_STACK_SIZE;//メッセージバッファサイズ
	static const std::size_t QUEUE_SIZE = GASHA_LOG_QUEUE_NODE_SIZE;//最大キュー数
#ifdef GASHA_LOG_QUEUE_NOWAIT
	static const bool IS_NO_WAIT_MODE = true;
#else//GASHA_LOG_QUEUE_NOWAIT
	static const bool IS_NO_WAIT_MODE = false;
#endif//GASHA_LOG_QUEUE_NOWAIT

public:
	//キューイング
	//※キューイングに成功したら true を返す。
	//※通常はキューイングが成功するまでリトライを繰り返す。
	//※GASHA_LOG_QUEUE_NOWAIT が指定されている場合、キュー用のバッファがいっぱいだとキューイングせずに失敗する。
	//※キューイング待ち時に中断が指定されたら失敗する。
	//※引数 mesage_size に 0 を指定すると、message の長さを計測する。message には終端を含めたサイズを指定する。
	//※引数 id に 0 を指定すると、id を自動発番する。
	bool enqueue(const char* message, const level_type level, const category_type category, GASHA_ IConsole* (&consoles)[PURPOSE_NUM], const GASHA_ consoleColor* (&colors)[PURPOSE_NUM], const std::size_t message_size = 0, const id_type id = 0);

	//デキュー
	//※キューのノードを渡して情報を受け取る。
	//※キューを取得できた場合、true を返す。
	inline bool dequeue(node_type& node);

	//リリース
	//※メッセージをバッファから解放する。
	inline void release(const char* message);
	inline void release(node_type& node);

	//中断
	//※キューイング待ちがあったら中断させる
	//※一度中断したら、キューイングを許可しなくなるため、
	//　再度使用するには明示的な初期化が必要
	inline void abort();

public:
	//明示的な初期化用コンストラクタ
	inline logQueue(const explicitInitialize_t&);
	//デフォルトコンストラクタ
	inline logQueue();
	//デストラクタ
	inline ~logQueue();

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
	static std::atomic<id_type> m_id;//キューID発番用
	static GASHA_ lfSmartStackAllocator_withBuff<MESSAGE_BUFF_SIZE> m_messageBuff;//メッセージバッファ
	static GASHA_ binary_heap::container<queueOpe, QUEUE_SIZE> m_queue;//ログキュー
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_queue.inl>

#endif//GASHA_INCLUDED_LO_QUEUE_H

// End of file

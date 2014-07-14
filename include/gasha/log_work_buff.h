#pragma once
#ifndef GASHA_INCLUDED_LOG_WORK_BUFF_H
#define GASHA_INCLUDED_LOG_WORK_BUFF_H

//--------------------------------------------------------------------------------
// log_work_buff.h
// ログワークバッファ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_pool_allocator.h>//ロックフリープールアロケータ

#include <atomic>//C++11 std::atomic

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <mutex> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <mutex>//C++11 std::call_once

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログワークバッファ
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログワークバッファ
class logWorkBuff
{
public:
	//型
	struct explicitInitialize_t{};//明示的な初期化用構造体

public:
	//ワークバッファの取得
	//※確保が成功するまでリトライを続ける
	//※確保できなければ無制限にリトライを続けるが、中断が呼び出されたら nullptr を返す
	char* alloc();

	//ワークバッファの解放
	inline void free(char* p);

	//中断
	//※ワークバッファ取得待ちがあったら中断させる
	//※一度中断したら、ワークバッファの取得を許可しなくなるため、
	//　再度使用するには明示的な初期化が必要
	inline void abort();

public:
	//明示的な初期化用コンストラクタ
	inline logWorkBuff(const explicitInitialize_t&);
	//デフォルトコンストラクタ
	inline logWorkBuff();
	//デストラクタ
	inline ~logWorkBuff();

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
	static GASHA_ lfPoolAllocator_withBuff<GASHA_LOG_WORK_BUFF_BLOCK_SIZE, GASHA_LOG_WORK_BUFF_POOL_SIZE> m_workBuff;//ワークバッファ
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_work_buff.inl>

#endif//GASHA_INCLUDED_LOG_WORK_BUFF_H

// End of file

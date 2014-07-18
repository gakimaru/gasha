#pragma once
#ifndef GASHA_INCLUDED_SHARED_QUEUE_H
#define GASHA_INCLUDED_SHARED_QUEUE_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_queue.h
// マルチスレッド共有キュー【宣言部】
//
// ※クラスをインスタンス化する際は、別途 .cpp.h ファイルをインクルードする必要あり。
// ※明示的なインスタンス化を避けたい場合は、ヘッダーファイルと共にインクルード。
// 　（この場合、実際に使用するメンバー関数しかインスタンス化されないので、対象クラスに不要なインターフェースを実装しなくても良い）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/pool_allocator.h>//プールアロケータ
#include <gasha/spin_lock.h>//スピンロック
#include <gasha/dummy_lock.h>//ダミーロック

#include <cstddef>//std::size_t

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <functional> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <functional>//std::function

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//マルチスレッド共有キュークラス
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE = GASHA_ spinLock>
class sharedQueue
{
public:
	//型
	typedef T value_type;//値型
	typedef LOCK_TYPE lock_type;//ロック型

	//キュー型
	struct queue_t
	{
		queue_t* m_next;//次のノード
		value_type m_value;//値
		//コンストラクタ
		inline queue_t(value_type&& value) :
			m_value(std::move(value))
		{}
		inline queue_t(value_type& value) :
			m_value(value)
		{}
		//デフォルトコンストラクタ（ダミー生成用）
		inline queue_t()
		{}
	};

public:
	//メソッド

	//エンキュー
private:
	inline bool _enqueue(queue_t* new_node);
public:
	bool enqueue(value_type&& value);//※ムーブ版
	bool enqueue(value_type& value);//※コピー版

	//デキュー
	bool dequeue(value_type& value);

	//デバッグ情報作成
	//※十分なサイズのバッファを渡す必要あり。
	//※使用したバッファのサイズを返す。
	//※作成中、ロックを取得する。
	std::size_t debugInfo(char* message, const bool with_detail, std::function<std::size_t(char* message, const value_type& value)> print_node) const;

private:
	//初期化
	void initialize();
	//終了
	void finalize();

public:
	//コンストラクタ
	sharedQueue();
	//デストラクタ
	~sharedQueue();
private:
	//フィールド
	poolAllocator_withType<queue_t, POOL_SIZE, GASHA_ dummyLock> m_allocator;//プールアロケータ（プールアロケータ自体はロック制御しない）
	queue_t* m_head;//キューの先頭
	queue_t* m_tail;//キューの末尾
	mutable lock_type m_lock;//ロックオブジェクト
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/shared_queue.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_SHARED_QUEUE_ALLWAYS_TOGETHER_CPP_H
#include <gasha/shared_queue.cpp.h>
#endif//GASHA_SHARED_QUEUE_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_SHARED_QUEUE_H

// End of file

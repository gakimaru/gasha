﻿#pragma once
#ifndef __SHARED_QUEUE_H_
#define __SHARED_QUEUE_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_queue.h
// マルチスレッド共有キュー【宣言部】
//
// ※クラスをインスタンス化する際は、別途下記のファイルをインクルードする必要あり
//
//   ・shared_queue.inl   ... 【インライン関数／テンプレート関数実装部】
//                            クラスの操作が必要な場所でインクルード。
//   ・shared_queue.cpp.h ... 【関数実装部】
//                            クラスの実体化が必要な場所でインクルード。
//
// ※面倒なら三つまとめてインクルードして使用しても良いが、分けた方が、
// 　コンパイルへの影響やコンパイル速度を抑えることができる。
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shared_pool_allocator.h>//マルチスレッド共有プールアロケータ
#include <gasha/spin_lock.h>//スピンロック
#include <gasha/dummy_lock.h>//ダミーロック

#include <cstddef>//std::size_t

//例外を無効化した状態で <functional> をインクルードすると、warning C4530 が発生する
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

	//デバッグ情報表示
	void printDebugInfo(std::function<void(const value_type& value)> print_node);

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
	sharedPoolAllocator<queue_t, POOL_SIZE, GASHA_ dummyLock> m_allocator;//プールアロケータ（プールアロケータ自体はロック制御しない）
	queue_t* m_head;//キューの先頭
	queue_t* m_tail;//キューの末尾
	lock_type m_lock;//ロックオブジェクト
};

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__SHARED_QUEUE_H_

// End of file

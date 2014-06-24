#pragma once
#ifndef __SHARED_QUEUE_H_
#define __SHARED_QUEUE_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_queue.h
// マルチスレッド共有キュー
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/spin_lock.h>//スピンロック
#include <gasha/dummy_lock.h>//ダミーロック
#include <gasha/shared_pool_allocator.h>//マルチスレッド共有プールアロケータ

#include <cstddef>//std::size_t
#include <stdio.h>//printf()

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
		//コンストラクタ ※値の受け渡しはムーブコンストラクタを使用
		queue_t(value_type&& value) :
			m_value(std::move(value))
		{}
		//デフォルトコンストラクタ（ダミー生成用）
		queue_t()
		{}
	};

public:
	//メソッド

	//エンキュー
	bool enqueue(value_type&& value)
	{
		GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
		void* p = m_allocator.alloc();//新規ノードのメモリを確保
		if (!p)//メモリ確保失敗
			return false;//エンキュー失敗
		queue_t* new_node = new(p)queue_t(std::move(value));//新規ノードのコンストラクタ呼び出し
		new_node->m_next = nullptr;//新規ノードの次ノードを初期化
		m_tail->m_next = new_node;//末尾ノードの次ノードを新規ノードにする
		m_tail = new_node;//末尾ノードを新規ノードにする
		return true;//エンキュー成功
	}
	bool enqueue(value_type& value)
	{
		return enqueue(std::move(value));//常にムーブ処理を適用
	}

	//デキュー
	bool dequeue(value_type& value)
	{
		GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
		if (m_head != m_tail)
		{
			queue_t* top = m_head->m_next;//次ノード（有効なキューの先頭）を取得
			m_allocator.deleteObj(m_head);//先頭ノード（ダミーノード）を削除
			m_head = top;//先頭ノードを次ノードに変更（これより次ノードがダミーノード扱いになる）
			value = std::move(top->m_value);//値を取得
			return true;//デキュー成功
		}
		return false;//デキュー失敗
	}

	//デバッグ情報表示
	void printDebugInfo(std::function<void(const value_type& value)> print_node)
	{
		printf("----- Debug Info for queue -----\n");
		printf("Queue:\n");
		int no = 0;
		const queue_t* node = m_head;
		while (node)
		{
			printf("[%d](%p) ", no++, node);
			print_node(node->m_value);
			printf("\n");
			node = node->m_next;
		}
		printf("[tail](%p)", m_tail);
		print_node(m_tail->m_value);
		printf("\n");
		printf("----------\n");
		auto print_allocator_node = [&print_node](const queue_t& info)
		{
			print_node(info.m_value);
		};
		m_allocator.printDebugInfo(print_allocator_node);
	}

private:
	//初期化
	void initialize()
	{
		queue_t* dummy_node = m_allocator.newObj();//ダミーノードを生成
		dummy_node->m_next = nullptr;//ダミーノードの次ノードを初期化
		m_head = dummy_node;//先頭ノードにダミーノードをセット
		m_tail = dummy_node;//末尾ノードにダミーノードをセット
	}
	//終了
	void finalize()
	{
		//空になるまでデキュー
		value_type value;
		while (dequeue(value));
		//ダミーノードを削除
		m_allocator.deleteObj(m_head);
	}

public:
	//コンストラクタ
	sharedQueue()
	{
		initialize();
	}
	//デストラクタ
	~sharedQueue()
	{
		finalize();
	}
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

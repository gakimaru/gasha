#pragma once
#ifndef GASHA_INCLUDED_SHARED_QUEUE_CPP_H
#define GASHA_INCLUDED_SHARED_QUEUE_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_queue.h
// マルチスレッド共有キュー【関数定義部】
//
// ※クラスの実体化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shared_queue.inl>//共有キュー【インライン関数／テンプレート関数定義部】

#include <gasha/shared_pool_allocator.cpp.h>//マルチスレッド共有プールアロケータ【関数定義部】

#include <utility>//C++11 std::move
#include <stdio.h>//printf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//マルチスレッド共有キュークラス

//エンキュー
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
inline bool sharedQueue<T, POOL_SIZE, LOCK_TYPE>::_enqueue(typename sharedQueue<T, POOL_SIZE, LOCK_TYPE>::queue_t* new_node)
{
	new_node->m_next = nullptr;//新規ノードの次ノードを初期化
	m_tail->m_next = new_node;//末尾ノードの次ノードを新規ノードにする
	m_tail = new_node;//末尾ノードを新規ノードにする
	return true;//エンキュー成功
}
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
bool sharedQueue<T, POOL_SIZE, LOCK_TYPE>::enqueue(typename sharedQueue<T, POOL_SIZE, LOCK_TYPE>::value_type&& value)//※ムーブ版
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	void* p = m_allocator.alloc();//新規ノードのメモリを確保
	if (!p)//メモリ確保失敗
		return false;//エンキュー失敗
	queue_t* new_node = new(p)queue_t(std::move(value));//新規ノードのコンストラクタ呼び出し
	return _enqueue(new_node);
}
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
bool sharedQueue<T, POOL_SIZE, LOCK_TYPE>::enqueue(typename sharedQueue<T, POOL_SIZE, LOCK_TYPE>::value_type& value)//※コピー版
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	void* p = m_allocator.alloc();//新規ノードのメモリを確保
	if (!p)//メモリ確保失敗
		return false;//エンキュー失敗
	queue_t* new_node = new(p)queue_t(value);//新規ノードのコンストラクタ呼び出し
	return _enqueue(new_node);
}

//デキュー
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
bool sharedQueue<T, POOL_SIZE, LOCK_TYPE>::dequeue(typename sharedQueue<T, POOL_SIZE, LOCK_TYPE>::value_type& value)
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
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
void sharedQueue<T, POOL_SIZE, LOCK_TYPE>::printDebugInfo(std::function<void(const typename sharedQueue<T, POOL_SIZE, LOCK_TYPE>::value_type& value)> print_node)
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

//初期化
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
void sharedQueue<T, POOL_SIZE, LOCK_TYPE>::initialize()
{
	queue_t* dummy_node = m_allocator.newObj();//ダミーノードを生成
	dummy_node->m_next = nullptr;//ダミーノードの次ノードを初期化
	m_head = dummy_node;//先頭ノードにダミーノードをセット
	m_tail = dummy_node;//末尾ノードにダミーノードをセット
}

//終了
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
void sharedQueue<T, POOL_SIZE, LOCK_TYPE>::finalize()
{
	//空になるまでデキュー
	value_type value;
	while (dequeue(value));
	//ダミーノードを削除
	m_allocator.deleteObj(m_head);
}

//コンストラクタ
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
sharedQueue<T, POOL_SIZE, LOCK_TYPE>::sharedQueue()
{
	initialize();
}

//デストラクタ
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
sharedQueue<T, POOL_SIZE, LOCK_TYPE>::~sharedQueue()
{
	finalize();
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SHARED_QUEUE_CPP_H

// End of file

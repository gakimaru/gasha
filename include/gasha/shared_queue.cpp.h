#pragma once
#ifndef GASHA_INCLUDED_SHARED_QUEUE_CPP_H
#define GASHA_INCLUDED_SHARED_QUEUE_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_queue.cpp.h
// マルチスレッド共有キュー【関数／実体定義部】
//
// ※クラスのインスタンス化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。
// 　（コンパイル・リンク時間への影響を気にしないならOK）
// ※明示的なインスタンス化を避けたい場合は、ヘッダーファイルと共にインクルード。
// 　（この場合、実際に使用するメンバー関数しかインスタンス化されないので、対象クラスに不要なインターフェースを実装しなくても良い）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shared_queue.inl>//共有キュー【インライン関数／テンプレート関数定義部】

#include <gasha/pool_allocator.cpp.h>//プールアロケータ【関数／実体定義部】

#include <utility>//C++11 std::move
#include <stdio.h>//sprintf()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】sprintf を使用すると、error C4996 が発生する
//  error C4996: 'sprintf': This function or variable may be unsafe. Consider using strncpy_fast_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

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

//デバッグ情報作成
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
std::size_t sharedQueue<T, POOL_SIZE, LOCK_TYPE>::debugInfo(char* message, std::function<std::size_t(char* buff, const typename sharedQueue<T, POOL_SIZE, LOCK_TYPE>::value_type& value)> print_node)
{
#ifdef GASHA_HAS_DEBUG_FEATURE
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	std::size_t size = 0;
	size += sprintf(message + size, "----- Debug Info for queue -----\n");
	size += sprintf(message + size, "Queue:\n");
	int no = 0;
	const queue_t* node = m_head;
	while (node)
	{
		size += sprintf(message + size, "[%d](%p) ", no++, node);
		size += print_node(message + size, node->m_value);
		size += sprintf(message + size, "\n");
		node = node->m_next;
	}
	size += sprintf(message + size, "[tail](%p)", m_tail);
	size += print_node(message + size, m_tail->m_value);
	size += sprintf(message + size, "\n");
	size += sprintf(message + size, "----------\n");
	auto print_allocator_node = [&print_node](char* message, const queue_t& info) -> std::size_t
	{
		return print_node(message, info.m_value);
	};
	size += m_allocator.template debugInfo<queue_t>(message + size, print_allocator_node);
	return size;
#else//GASHA_HAS_DEBUG_FEATURE
	message[0] = '\0';
	return 0;
#endif//GASHA_HAS_DEBUG_FEATURE
}

//初期化
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
void sharedQueue<T, POOL_SIZE, LOCK_TYPE>::initialize()
{
	queue_t* dummy_node = m_allocator.newDefault();//ダミーノードを生成
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

//----------------------------------------
//明示的なインスタンス化

//明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_sharedQueue(T, _POOL_SIZE) \
	template class sharedQueue<T, _POOL_SIZE>; \
	template class poolAllocator_withType<typename sharedQueue<T, _POOL_SIZE>::queue_t, _POOL_SIZE, GASHA_ dummyLock>;
#define GASHA_INSTANCING_sharedQueue_withLock(T, _POOL_SIZE, LOCK_TYPE) \
	template class sharedQueue<T, _POOL_SIZE, LOCK_TYPE>; \
	template class poolAllocator_withType<typename sharedQueue<T, _POOL_SIZE, LOCK_TYPE>::queue_t, _POOL_SIZE, GASHA_ dummyLock>;

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_SHARED_QUEUE_CPP_H

// End of file

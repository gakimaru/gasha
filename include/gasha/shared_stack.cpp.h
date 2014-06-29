﻿#pragma once
#ifndef __SHARED_STACK_CPP__H_
#define __SHARED_STACK_CPP__H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_stack.h
// マルチスレッド共有スタック【関数定義部】
//
// ※クラスの実体化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shared_stack.inl>//共有スタック【インライン関数／テンプレート関数定義部】

#include <gasha/shared_pool_allocator.cpp.h>//マルチスレッド共有プールアロケータ【関数定義部】

#include <stdio.h>//printf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//マルチスレッド共有スタッククラス

//プッシュ
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
inline bool sharedStack<T, POOL_SIZE, LOCK_TYPE>::_push(typename sharedStack<T, POOL_SIZE, LOCK_TYPE>::stack_t* new_node)
{
	new_node->m_next = m_head;//新規ノードの次ノードに現在の先頭ノードをセット
	m_head = new_node;//先頭ノードを新規ノードにする
	return true;//プッシュ成功
}
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
bool sharedStack<T, POOL_SIZE, LOCK_TYPE>::push(typename sharedStack<T, POOL_SIZE, LOCK_TYPE>::value_type&& value)//※ムーブ版
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	void* p = m_allocator.alloc();//新規ノードのメモリを確保
	if (!p)//メモリ確保失敗
		return false;//プッシュ失敗
	stack_t* new_node = new(p)stack_t(std::move(value));//新規ノードのコンストラクタ呼び出し
	return _push(new_node);
}
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
bool sharedStack<T, POOL_SIZE, LOCK_TYPE>::push(typename sharedStack<T, POOL_SIZE, LOCK_TYPE>::value_type& value)//※コピー版
{
	//return push(std::move(value));//常にムーブ処理を適用
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	void* p = m_allocator.alloc();//新規ノードのメモリを確保
	if (!p)//メモリ確保失敗
		return false;//プッシュ失敗
	stack_t* new_node = new(p)stack_t(value);//新規ノードのコンストラクタ呼び出し
	return _push(new_node);
}

//ポップ
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
bool sharedStack<T, POOL_SIZE, LOCK_TYPE>::pop(typename sharedStack<T, POOL_SIZE, LOCK_TYPE>::value_type& value)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	stack_t* head = m_head;//先頭ノードを取得
	if (head)//先頭ノードが存在していた場合
	{
		m_head = head->m_next;//先頭ノードを次ノードに変更
		value = std::move(head->m_value);//値を取得
		m_allocator.deleteObj(head);//先頭ノードを削除
		return true;//ポップ成功
	}
	return false;//ポップ失敗
}

//デバッグ情報表示
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
void sharedStack<T, POOL_SIZE, LOCK_TYPE>::printDebugInfo(std::function<void(const typename sharedStack<T, POOL_SIZE, LOCK_TYPE>::value_type& value)> print_node)
{
	printf("----- Debug Info for stack -----\n");
	printf("Stack:\n");
	int no = 0;
	stack_t* node = m_head;
	while (node)
	{
		printf("[%d](%p) ", no++, node);
		print_node(node->m_value);
		printf("\n");
		node = node->m_next;
	}
	printf("----------\n");
	auto print_allocator_node = [&print_node](const stack_t& info)
	{
		print_node(info.m_value);
	};
	m_allocator.printDebugInfo(print_allocator_node);
}

//初期化
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
void sharedStack<T, POOL_SIZE, LOCK_TYPE>::initialize()
{
	m_head = nullptr;//先頭ノードの初期値はnullptr
}

//終了
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
void sharedStack<T, POOL_SIZE, LOCK_TYPE>::finalize()
{
	//空になるまでポップ
	value_type value;
	while (pop(value));
}

//コンストラクタ
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
sharedStack<T, POOL_SIZE, LOCK_TYPE>::sharedStack()
{
	initialize();
}

//デストラクタ
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE>
sharedStack<T, POOL_SIZE, LOCK_TYPE>::~sharedStack()
{
	finalize();
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__SHARED_STACK_CPP__H_

// End of file
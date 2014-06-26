#pragma once
#ifndef __LOCKFREE_STACK_CPP_H_
#define __LOCKFREE_STACK_CPP_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_stack.h
// ロックフリースタック【関実装部】
//
// ※クラスの実体化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_stack.inl>//ロックフリースタック【インライン関数／テンプレート関数定義部】

#include <gasha/lf_pool_allocator.cpp.h>//ロックフリープールアロケータ【関数定義部】

#include <stdio.h>//printf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリースタッククラス

//プッシュ
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
inline bool lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::_push(typename lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::stack_t* new_node)
{
	new_node->m_next.store(m_head.load());//新規ノードの次ノードに現在の先頭ノードをセット
	stack_ptr_t new_node_tag_ptr;
	new_node_tag_ptr.set(new_node, m_tag.fetch_add(1));//タグ付きポインタ生成
	while (true)
	{
		stack_ptr_t next_tag_ptr = new_node->m_next.load();//新規ノードの次ノードを取得

		//CAS操作①
		if (m_head.compare_exchange_weak(next_tag_ptr, new_node_tag_ptr))//CAS操作
			//【CAS操作の内容】
			//    if(m_head == next_tag_ptr)//先頭ノードが新規ノードの次ノードか？
			//        m_head = new_node_tag_ptr;//先頭ノードを新規ノード＆タグに置き換えて完了（プッシュ成功）
			//    else
			//        next_tag_ptr = m_head;//新規ノードの次ノードを現在の先頭ノードにする
		{
			return true;//プッシュ成功
		}

		new_node->m_next.store(next_tag_ptr);//先頭ノードを再取得
	}
	return false;//ダミー
}
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
bool lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::push(typename lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::value_type&& value)//※ムーブ版
{
	void* p = m_allocator.alloc();//新規ノードのメモリを確保
	if (!p)//メモリ確保失敗
		return nullptr;//プッシュ失敗
	stack_t* new_node = new(p)stack_t(std::move(value));//新規ノードのコンストラクタ呼び出し
	return _push(new_node);
}
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
bool lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::push(typename lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::value_type& value)//※コピー版
{
	void* p = m_allocator.alloc();//新規ノードのメモリを確保
	if (!p)//メモリ確保失敗
		return nullptr;//プッシュ失敗
	stack_t* new_node = new(p)stack_t(value);//新規ノードのコンストラクタ呼び出し
	return _push(new_node);
}

//ポップ
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
bool lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::pop(typename lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::value_type& value)
{
	stack_ptr_t head_tag_ptr = m_head.load();//先頭ノードを取得
	while (head_tag_ptr.isNotNull())
	{
		stack_t* head = head_tag_ptr;//タグ付きポインタからポインタを取得
		stack_ptr_t next_tag_ptr = head->m_next;//次のノードを取得

		//CAS操作②
		if (m_head.compare_exchange_weak(head_tag_ptr, next_tag_ptr))//CAS操作
		//【CAS操作の内容】
		//    if(m_head == head_tag_ptr)//先頭ノードを他のスレッドが書き換えていないか？
		//        m_head = next_tag_ptr;//先頭ノードを先頭ノードの次ノードに置き換えて完了（ポップ成功）
		//    else
		//        head_tag_ptr = m_head;//先頭ノードを再取得
		{
			value = std::move(head->m_value);//値を取得
			m_allocator.deleteObj(head);//先頭ノードを削除
			return true;//ポップ成功
		}
	}
	return false;//ポップ失敗
}

//デバッグ情報表示
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
void lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::printDebugInfo(std::function<void(const typename lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::value_type& value)> print_node)
{
	printf("----- Debug Info for lfStack -----\n");
	printf("Stack:\n");
	int no = 0;
	stack_ptr_t node_tag_ptr = m_head.load();
	while (node_tag_ptr.isNotNull())
	{
		const stack_t* node = node_tag_ptr;
		printf("[%d(tag=%d)](%p) ", no++, node_tag_ptr.tag(), node);
		print_node(node->m_value);
		printf("\n");
		node_tag_ptr = node->m_next;
	}
	printf("----------\n");
	auto print_allocator_node = [&print_node](const stack_t& info)
	{
		print_node(info.m_value);
	};
	m_allocator.printDebugInfo(print_allocator_node);
}

//初期化
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
void lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::initialize()
{
	stack_ptr_t null_tag_ptr;
	null_tag_ptr.set(nullptr, 0);//タグ付きヌルポインタ
	m_head.store(null_tag_ptr);//先頭ノードの初期値はnull
	m_tag.store(0);//タグを初期化
}

//終了
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
void lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::finalize()
{
	//空になるまでポップ
	value_type value;
	while (pop(value));
}

//コンストラクタ
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::lfStack()
{
	initialize();
}

//デストラクタ
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::~lfStack()
{
	finalize();
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__LOCKFREE_STACK_CPP_H_

// End of file

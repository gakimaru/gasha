#pragma once
#ifndef __LOCKFREE_QUEUE_CPP_H_
#define __LOCKFREE_QUEUE_CPP_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_queue.h
// ロックフリーキュー【関数定義部】
//
// ※クラスの実体化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_queue.inl>//ロックフリーキュー【インライン関数／テンプレート関数定義部】

#include <gasha/lf_pool_allocator.cpp.h>//ロックフリープールアロケータ【関数定義部】

#include <utility>//C++11 std::move
#include <stdio.h>//printf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリーキュークラス

//※論文に基づいていて実装: http://www.cs.rochester.edu/u/scott/papers/1996_PODC_queues.pdf
//　論文から変更している点多し。
//    ⇒ 変更点①：enqueu:E9:   if CAS(&tail.ptr->next, next, <node, ...>)                     ←【NG】他のスレッドの処理が進んで、一気に tail まで削除されてしまうと、tailがキューの連結から外れていることに気がつかず、キューが消失する
//                        E10:      break;
//                                      ↓
//                        E9:   if tail == Q->Tail && CAS(&tail.ptr->next, next, <node, ...>)  ←【NG】これでもまだ「&&」の処理中に上記の問題が起こり得る
//                        E10:      break;
//                                      ↓
//                        E8+:  if CAS(&Q->Next, next, <node, ...>)                            ←【OK】「Q->Next」を追加して対処。Q->next は常にNULL。新規ノード追加の最中のみ値が入る。（実質的なロック制御...）
//                        E9:       if tail == Q->Tail && CAS(&tail.ptr->next, next, <node, ...>)
//                        E9+:          Q->next = NULL
//                        E10:          break;
//                        E11:      endif
//                        E11+:     Q->next = NULL
//       変更点②：enqueu:E7:   if tail == Q->Tail
//                        D15:  endif
//                                ↓
//                               削除
//       変更点③：dequeu:D5:   if head == Q->Head
//                        D17:  endif
//                                ↓
//                               削除
//       変更点③：（全般的なCAS操作）ポインタへのタグ付けは新規ノード生成時のみ適用

//エンキュー
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
inline bool lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::_enqueue(typename lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::queue_t* new_node)
{
	queue_ptr_t new_node_tag_ptr;
	new_node_tag_ptr.set(new_node, m_tag.fetch_add(1));//タグ付きポインタ生成
	queue_ptr_t null_tag_ptr;
	null_tag_ptr.set(nullptr, 0);//タグ付きヌルポインタ
	new_node->m_next.store(null_tag_ptr);//新規ノードの次ノードを初期化
	queue_ptr_t tail_tag_ptr = null_tag_ptr;
	while (true)
	{
		tail_tag_ptr = m_tail.load();//末尾ノードを取得
		queue_t* tail = tail_tag_ptr;
		std::atomic<queue_ptr_t>& next = tail->m_next;
		queue_ptr_t next_tag_ptr = next.load();//末尾ノードの次ノードを取得
		//if (tail_tag_ptr == m_tail.load())//このタイミングで他のスレッドが末尾を書き換えていないか？　←削除（E7,E15）
		{
			if (next_tag_ptr.isNull())//末尾ノードの次ノードが末端（nullptr）か？
			{
				//CAS操作(0)
				//※問題解消のための追加処理。m_nextを設けて、先着処理を保証する。
				//※本質的にロック操作（try_lock）と同等なのが残念...
				//※このような形にするのであれば、m_next のCAS操作をエンキュー処理の起点にして、
				//　単純なnextとtailの更新処理に変更しても良いのだが（実際その方がシンプルで
				//　高速になるが）、少しでもロック状態を短縮するために、論文の構造を踏襲し、
				//　つじつま合わせ処理（CAS操作③&④）を生かす方針にする。
				if (m_next.compare_exchange_weak(next_tag_ptr, new_node_tag_ptr))//CAS操作
				//【CAS操作の内容】
				//    if(m_next == next_ready_tag_ptr)//末尾ノードの次ノード（予約）を他のスレッドが書き換えていないか？
				//        m_next = new_node_tag_ptr;//末尾ノードの次ノード（予約）に新規ノードをセット（エンキュー準備成功）
				//    else
				//        next_tag_ptr = m_next;//他のスレッドが処理中のノード（予約）を取得
				{
					//CAS操作①
					//※論文通りのこの処理だけだとアトミック性を保証できない問題があったので、前後に対策処理を追加
					if(tail_tag_ptr == m_tail.load() && next.compare_exchange_weak(next_tag_ptr, new_node_tag_ptr))//CAS操作
					//【CAS操作の内容】
					//    if(next == next_tag_ptr)//末尾ノードの次ノードを他のスレッドが書き換えていないか？
					//        next = new_node_tag_ptr;//末尾ノードの次ノードに新規ノードをセット（エンキュー成功）
					//    else
					//        next_tag_ptr = next;//他のスレッドが書き換えた末尾ノードの次ノードを取得
					{
						m_next.store(next_tag_ptr);//※問題解消のための追加処理（本質的にロック解除操作と同等）

						//CAS操作②
						m_tail.compare_exchange_weak(tail_tag_ptr, new_node_tag_ptr);//CAS操作
						//【CAS操作の内容】
						//    if(m_tail == tail_tag_ptr)//末尾ノードを他のスレッドが書き換えていないか？
						//        m_tail = new_node_tag_ptr;//末尾ノードを新規ノードに変更

						return true;//エンキュー成功
					}
					
					//エンキューに失敗したので元の状態に戻す
					m_next.store(null_tag_ptr);//※問題解消のための追加処理（本質的にロック解除操作と同等）
				}
			}
			else//if(next_tag_ptr.isNotNull())//末尾ノードの次ノードが末端ではない（他のスレッドの処理が割り込んでいた場合）
			{
				//CAS操作③：つじつま合わせ処理 ※CAS操作②が失敗する可能性があるため、この処理が必要
				m_tail.compare_exchange_weak(tail_tag_ptr, next_tag_ptr);//CAS操作
				//【CAS操作の内容】
				//    if(m_tail == tail_tag_ptr)//末尾ノードを他のスレッドが書き換えていないか？
				//        m_tail = next_tag_ptr;//末尾ノードを次ノードに変更
			}
		}
	}
	return false;//ダミー
}
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
bool lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::enqueue(typename lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::value_type&& value)//※ムーブ版
{
	void* p = m_allocator.alloc();//新規ノードのメモリを確保
	if (!p)//メモリ確保失敗
		return false;//エンキュー失敗
	queue_t* new_node = new(p)queue_t(std::move(value));//新規ノードのコンストラクタ呼び出し
	return _enqueue(new_node);
}
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
bool lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::enqueue(typename lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::value_type& value)//※コピー版
{
	void* p = m_allocator.alloc();//新規ノードのメモリを確保
	if (!p)//メモリ確保失敗
		return false;//エンキュー失敗
	queue_t* new_node = new(p)queue_t(value);//新規ノードのコンストラクタ呼び出し
	return _enqueue(new_node);
}

//デキュー
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
bool lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::dequeue(typename lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::value_type& value)
{
	queue_ptr_t null_tag_ptr;
	null_tag_ptr.set(nullptr, 0);//タグ付きヌルポインタ
	queue_ptr_t head_tag_ptr = null_tag_ptr;
	queue_ptr_t tail_tag_ptr = null_tag_ptr;
	queue_ptr_t top_tag_ptr = null_tag_ptr;
	while (true)
	{
		head_tag_ptr = m_head.load();//先頭ノードを取得
		tail_tag_ptr = m_tail.load();//末尾ノードを取得
		queue_t* head = head_tag_ptr;
		queue_ptr_t next_tag_ptr = head->m_next.load();//先頭ノードの次ノード（有効なキューの先頭）を取得
		//if (head_tag_ptr == m_head.load())//このタイミングで他のスレッドが先頭を書き換えていないか？　←削除（D5,D17）
		{
			if (head_tag_ptr == tail_tag_ptr)//先頭ノードと末尾ノードが同じか？（一つもキューイングされていない状態か？）
			{
				if (next_tag_ptr.isNull())//本当にキューがないか？（tail取得とnext取得の間に、他のスレッドがキューイングしている可能性がある）
					return false;//デキュー失敗
				//CAS操作④：つじつま合わせ処理 ※エンキューのCAS操作②が失敗する可能性があるため、この処理が必要
				m_tail.compare_exchange_weak(tail_tag_ptr, next_tag_ptr);//CAS操作
				//【CAS操作の内容】
				//    if(m_tail == tail_tag_ptr)//末尾ノードを他のスレッドが書き換えていないか？
				//        m_tail = next_tag_ptr;//末尾ノードを次ノードに変更
			}
			else
			{
				//CAS操作⑤
				if (m_head.compare_exchange_weak(head_tag_ptr, next_tag_ptr))//CAS操作
				//【CAS操作の内容】
				//    if(m_head == head_tag_ptr)//先頭ノードの次ノードを他のスレッドが書き換えていないか？
				//        m_head = next_tag_ptr;//先頭ノードを次ノードに変更（これより次ノードがダミーノード扱いになる）（デキュー成功）
				{
					queue_t* top = next_tag_ptr;//新しい先頭ノードから値を取り出す
					value = std::move(top->m_value);//値を取得
					m_allocator.deleteObj(head);//先頭ノード（ダミーノード）を削除
					return true;//デキュー成功
				}
			}
		}
	}
	return false;//ダミー
}

//デバッグ情報表示
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
void lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::printDebugInfo(std::function<void(const typename lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::value_type& value)> print_node)
{
	printf("----- Debug Info for lfQueue -----\n");
	printf("Queue:\n");
	int no = 0;
	queue_ptr_t node_tag_ptr = m_head;
	while (node_tag_ptr.isNotNull())
	{
		queue_t* node = node_tag_ptr;
		printf("[%d(tag=%d)](%p) ", no++, node_tag_ptr.tag(), node);
		print_node(node->m_value);
		printf("\n");
		node_tag_ptr = node->m_next.load();
	}
	queue_ptr_t tail_tag_ptr = m_tail.load();
	queue_t* tail = tail_tag_ptr;
	printf("[tail(tag=%d)](%p)", tail_tag_ptr.tag(), tail);
	print_node(tail->m_value);
	printf("\n");
	printf("----------\n");
	auto print_allocator_node = [&print_node](const queue_t& info)
	{
		print_node(info.m_value);
	};
	m_allocator.printDebugInfo(print_allocator_node);
}

//初期化
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
void lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::initialize()
{
	queue_t* dummy_node = m_allocator.newObj();//ダミーノードを生成
	queue_ptr_t null_tag_ptr;
	null_tag_ptr.set(nullptr, 0);//タグ付きヌルポインタ
	dummy_node->m_next.store(null_tag_ptr);//ダミーノードの次ノードを初期化
	queue_ptr_t dummy_node_tag_ptr;
	dummy_node_tag_ptr.set(dummy_node, 0);
	m_head.store(dummy_node_tag_ptr);//先頭ノードにダミーノードをセット
	m_tail.store(dummy_node_tag_ptr);//末尾ノードにダミーノードをセット
	m_next.store(null_tag_ptr);//末尾ノードの次を初期化
	m_tag.store(0);//タグを初期化
}

//終了
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
void lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::finalize()
{
	//空になるまでデキュー
	value_type value;
	while (dequeue(value));
	//ダミーノードを削除
	queue_ptr_t head_tag_ptr = m_head.load();
	queue_t* head = head_tag_ptr;
	m_allocator.deleteObj(head);
}

//コンストラクタ
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::lfQueue()
{
	initialize();
}

//デストラクタ
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::~lfQueue()
{
	finalize();
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__LOCKFREE_QUEUE_CPP_H_

// End of file

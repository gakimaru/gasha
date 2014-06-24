#pragma once
#ifndef __LOCKFREE_QUEUE_H_
#define __LOCKFREE_QUEUE_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_queue.h
// ロックフリーキュー
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lockfree_pool_allocator.h>//ロックフリープールアロケータ
#include <gasha/tagged_ptr.h>//タグ付きポインタ
#include <gasha/basic_math.h>//基本算術：calcStaticMSB()

#include <cstddef>//std::size_t
#include <cstdint>//std::uint32_t
#include <atomic>//C++11 std::atomic
#include <stdio.h>//printf()

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリーキュークラス
//※論文に基づいていて実装: http://www.cs.rochester.edu/u/scott/papers/1996_PODC_queues.pdf
//　論文から変更している点多し。
//    ⇒ 変更点①：enqueu:E9:   if CAS(&tail.ptr->next, next, <node, ...>)                     ←【NG】他のスレッドの処理が進んで一気に削除されるとtailがキューの連結から外れていることに気がつかず、キューが消失する
//                        E10:      break;
//                                      ↓
//                              if CAS(tail == Q->Tail && &tail.ptr->next, next, <node, ...>)  ←【NG】「&&」の時に他のスレッドの処理が進んで一気に削除されると結局キューが消失する...
//                                  break;
//                                      ↓
//                        E4+:  next = NULL                                                    ←【OK】「Q->Next」を追加して対処。Q->next は常にNULL。新規ノード追加の最中のみ値が入る。
//                              if CAS(&Q->Next, next, <node, ...>)
//                                  break;
//       変更点②：enqueu:E5:   tail = Q->Tail
//                                ～
//                        E15:  endif
//                                ↓
//                               まるごと削除
//       変更点③：enqueu:E17:  CAS(&Q->Tail, tail, <node, ...>)
//                                     ↓
//                              Q->Tail.ptr->next = Q->Next
//                              Q->Tail = Q->Next
//                              Q->Next = NULL
//       変更点⑤：dequeu:D5:   if head == Q->Head
//                        D17:  endif
//                                ↓
//                               削除
//       変更点⑤：enqueu:D10:  CAS(&Q->Tail, tail, <next.ptr, tail, <next.ptr, ...>)
//                                ↓
//                               削除 ※（変更点①）の影響により、つじつま合わせ処理が不要になる
//       変更点⑥：（全般的なCAS操作）ポインタへのタグ付けは新規ノード生成時のみ適用
//※ABA問題対策あり（タグ付きポインタ型使用）
//
//【テンプレート引数の説明】
//・T ... キューのデータ型
//・_POOL_SIZE ... 同時にキューイング可能なデータの最大個数
//※以下、ABA問題対策のためのタグ付きポインタのパラメータ（詳しい説明は tagged_ptr.h 参照）
//・_TAGGED_PTR_TAG_BITS  ... taggedPtr クラスのテンプレート引数 _TAG_BITS　※0でデータ型 T のアラインメントサイズに合わせる（デフォルト）
//・_TAGGED_PTR_TAG_SHIFT ... taggedPtr クラスのテンプレート引数 _TAG_SHIFT
//・TAGGED_PTR_VALUE_TYPE ... taggedPtr クラスのテンプレート引数 VALUE_TYPE
//・TAGGED_PTR_TAG_TYPE   ... taggedPtr クラスのテンプレート引数 TAG_TYPE
//※デフォルトでは、データ型 T のアライメントの隙間にタグを挿入するので、タグのサイズは2ビット程度になる。
//　スレッドが混み合う場面では、ABA問題の対策不十分で、データ破壊が起こる可能性がある。（さほど混み合わないなら十分）
//　推奨設定は tagged_ptr.h 参照。
//
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS = 0, int _TAGGED_PTR_TAG_SHIFT = 0, typename TAGGED_PTR_VALUE_TYPE = std::uint64_t, typename TAGGED_PTR_TAG_TYPE = std::uint32_t>
class lfQueue
{
public:
	//定数
	static const std::size_t POOL_SIZE = _POOL_SIZE;//プールアロケータのプールサイズ（プールする個数）
	static const std::size_t TAGGED_PTR_TAG_BITS = _TAGGED_PTR_TAG_BITS == 0 ? GASHA_ calcStaticMSB<alignof(T)>::value : _TAGGED_PTR_TAG_BITS;//タグ付きポインタのタグのビット長
	static const int TAGGED_PTR_TAG_SHIFT = _TAGGED_PTR_TAG_SHIFT;//タグ付きポインタのタグの位置

public:
	//型
	typedef T value_type;//値型

	//キュー型用タグ付きポインタ型
	struct queue_t;
	typedef TAGGED_PTR_VALUE_TYPE tagged_ptr_value_type;//タグ付きポインタの値の型
	typedef TAGGED_PTR_TAG_TYPE tagged_ptr_tag_type;//タグ付きポインタのタグの型
	typedef GASHA_ taggedPtr<queue_t, TAGGED_PTR_TAG_BITS, TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE> queue_ptr_t;

	//アロケータ型
	typedef GASHA_ lfPoolAllocator<queue_t, POOL_SIZE> allocator_type;//ロックフリープールアロケータ

	//キュー型
	struct queue_t
	{
		value_type m_value;//値
		std::atomic<queue_ptr_t> m_next;//次のノード
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
		void* p = m_allocator.alloc();//新規ノードのメモリを確保
		if (!p)//メモリ確保失敗
			return false;//エンキュー失敗
		queue_t* new_node = new(p)queue_t(std::move(value));//新規ノードのコンストラクタ呼び出し
		queue_ptr_t new_node_tag_ptr;
		new_node_tag_ptr.set(new_node, m_tag.fetch_add(1));//タグ付きポインタ生成
		queue_ptr_t null_tag_ptr;
		null_tag_ptr.set(nullptr, 0);//タグ付きヌルポインタ
		new_node->m_next.store(null_tag_ptr);//新規ノードの次ノードを初期化
		queue_ptr_t tail_tag_ptr = null_tag_ptr;
		while (true)
		{
	#if 0//【大幅修正】上記の説明にあるとおり、処理に問題があるので、m_next を追加してもっとシンプルに対処
			tail_tag_ptr = m_tail.load();//末尾ノードを取得
			queue_t* tail = tail_tag_ptr;
			std::atomic<queue_ptr_t>& next = tail->m_next;
			queue_ptr_t next_tag_ptr = next.load();//末尾ノードの次ノードを取得
			//if (tail_tag_ptr == m_tail.load())//このタイミングで他のスレッドが末尾を書き換えていないか？　←削除（E7,E15）
			{
				if (next_tag_ptr.isNull())//末尾ノードの次ノードが末端（nullptr）か？
				{
					//CAS操作①
					//※この処理には問題あり
					if (tail_tag_ptr == m_tail.load() && next.compare_exchange_weak(next_tag_ptr, new_node_tag_ptr))//CAS操作
					//【CAS操作の内容】
					//    if(tail_tag_ptr->m_next == next_tag_ptr)//末尾ノードの次ノードを他のスレッドが書き換えていないか？
					//        tail_tag_ptr->m_next = new_node_tag_ptr;//末尾ノードの次ノードに新規ノードをセット（エンキュー成功）
					{
						//CAS操作②
						m_tail.compare_exchange_strong(tail_tag_ptr, new_node_tag_ptr);//CAS操作
						//【CAS操作の内容】
						//    if(m_tail == tail_tag_ptr)//末尾ノードを他のスレッドが書き換えていないか？
						//        m_tail = new_node_tag_ptr;//末尾ノードを新規ノードに変更

						return true;//エンキュー成功
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
	#else//【大幅修正】
			//CAS操作①
			queue_ptr_t next_tag_ptr = null_tag_ptr;
			if (m_next.compare_exchange_weak(next_tag_ptr, new_node_tag_ptr))//CAS操作
			//【CAS操作の内容】
			//    if(m_next == next_tag_ptr)//末尾ノードの次ノードを他のスレッドが書き換えていないか？
			//        m_next = new_node_tag_ptr;//末尾ノードの次ノードに新規ノードをセット（エンキュー成功）
			{
				m_tail.load().ptr()->m_next.store(new_node_tag_ptr);
				m_tail.store(new_node_tag_ptr);
				m_next.store(null_tag_ptr);
				return true;//エンキュー成功
			}
	#endif//【大幅修正】
		}
		return false;//ダミー
	}
	bool enqueue(value_type& value)
	{
		return enqueue(std::move(value));//常にムーブ処理を適用
	}

	//デキュー
	bool dequeue(value_type& value)
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
	#if 0//【大幅修正】
					//CAS操作④：つじつま合わせ処理 ※エンキューのCAS操作②が失敗する可能性があるため、この処理が必要
					m_tail.compare_exchange_weak(tail_tag_ptr, next_tag_ptr);//CAS操作
					//【CAS操作の内容】
					//    if(m_tail == tail_tag_ptr)//末尾ノードを他のスレッドが書き換えていないか？
					//        m_tail = next_tag_ptr;//末尾ノードを次ノードに変更
	#endif//【大幅修正】
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
	void printDebugInfo(std::function<void(const value_type& value)> print_node)
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

private:
	//初期化
	void initialize()
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
	void finalize()
	{
		//空になるまでデキュー
		value_type value;
		while (dequeue(value));
		//ダミーノードを削除
		queue_ptr_t head_tag_ptr = m_head.load();
		queue_t* head = head_tag_ptr;
		m_allocator.deleteObj(head);
	}

public:
	//コンストラクタ
	lfQueue()
	{
		initialize();
	}
	//デストラクタ
	~lfQueue()
	{
		finalize();
	}
private:
	//フィールド
	allocator_type m_allocator;//アロケータ
	std::atomic<queue_ptr_t> m_head;//キューの先頭
	std::atomic<queue_ptr_t> m_tail;//キューの末尾
	std::atomic<queue_ptr_t> m_next;//キューの末尾の次
	std::atomic<typename queue_ptr_t::tag_type> m_tag;//ABA問題対策用のタグ
};

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__LOCKFREE_QUEUE_H_

// End of file

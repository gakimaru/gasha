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

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリーキュークラス
//※論文に基づいていて実装: http://www.cs.rochester.edu/u/scott/papers/1996_PODC_queues.pdf
//　一部論文通りの実装と異なる
//    ⇒ 変更点①：enqueu:E9:  if CAS(&tail.ptr->next, next, <node, ...>)
//                                     ↓
//                             if CAS(&Q->Tail.ptr->next, next, <node, ...>)
//       変更点②：enqueu:E7:  if tail == Q->Tail
//                        E15: endif
//                               ↓
//                              削除
//       変更点③：dequeu:D5:  if head == Q->Head
//                        D17: endif
//                               ↓
//                              削除
//       変更点④：ポインタへのタグ付けは新規ノード生成時のみ適用
//                 ※32bitタグ＋32bitポインタによる64bitのタグ付きポインタとすることで、
//                   タグが巡回して競合する危険性がほとんどない状態に
//※ABA問題対策あり（タグ付きポインタ型使用）
#ifdef USE_LF_POOL_ALLOCATOR
template<class T, std::size_t POOL_SIZE>
#else//USE_LF_POOL_ALLOCATOR
template<class T>
#endif//USE_LF_POOL_ALLOCATOR
class lf_queue
{
public:
	//型
	typedef T value_type;//値型

	//キュー型用タグ付きポインタ型
	struct queue_t;
	typedef tag_ptr<queue_t> queue_ptr_t;

	//キュー型
	struct queue_t
	{
		std::atomic<queue_ptr_t> m_next;//次のノード
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
	#ifdef USE_LF_POOL_ALLOCATOR
		void* p = m_allocator.alloc();//新規ノードのメモリを確保
	#else//USE_LF_POOL_ALLOCATOR
		void* p = _aligned_malloc(sizeof(queue_t), 16);//新規ノードのメモリを確保
	#endif//USE_LF_POOL_ALLOCATOR
		if (!p)//メモリ確保失敗
			return false;//エンキュー失敗
		queue_t* new_node = new(p)queue_t(std::move(value));//新規ノードのコンストラクタ呼び出し
		queue_ptr_t new_node_tag_ptr;
		new_node_tag_ptr.setTagPtr(new_node, m_tag.fetch_add(1));//タグ付きポインタ生成
		queue_ptr_t null_tag_ptr;
		null_tag_ptr.setTagPtr(nullptr, 0);//タグ付きヌルポインタ
		new_node->m_next.store(null_tag_ptr);//新規ノードの次ノードを初期化
		queue_ptr_t tail_tag_ptr = null_tag_ptr;
		while (true)
		{
			tail_tag_ptr = m_tail.load();//末尾ノードを取得
			queue_t* tail = tail_tag_ptr;
			queue_ptr_t next_tag_ptr = tail->m_next.load();//末尾ノードの次ノードを取得
			//if (tail_tag_ptr == m_tail.load())//このタイミングで他のスレッドが末尾を書き換えていないか？　←削除（E7,E15）
			{
				if (next_tag_ptr.isNull())//末尾ノードの次ノードが末端（nullptr）か？
				{
					//CAS操作①
				#ifdef USE_SAFE_CAS_LF_QUEUE//【安全対策】※スピンロックでCAS操作を保護　←変更（E9）
					m_lock.lock();//ロック取得
					const bool result = m_tail.load().ptr()->m_next.compare_exchange_weak(next_tag_ptr, new_node_tag_ptr);//CAS操作
					m_lock.unlock();//ロック解除
					if (result)
				#else//USE_SAFE_CAS_LF_QUEUE
					if (m_tail.load().ptr()->m_next.compare_exchange_weak(next_tag_ptr, new_node_tag_ptr))//CAS操作
				#endif//USE_SAFE_CAS_LF_QUEUE
					//【CAS操作の内容】
					//    if(tail_tag_ptr->m_next == next_tag_ptr)//末尾ノードの次ノードを他のスレッドが書き換えていないか？
					//        tail_tag_ptr->m_next = new_node_tag_ptr;//末尾ノードの次ノードに新規ノードをセット（エンキュー成功）
					{
						//CAS操作②
					#ifdef USE_SAFE_CAS_LF_QUEUE//【安全対策】※スピンロックでCAS操作を保護
						m_lock.lock();//ロック取得
						m_tail.compare_exchange_strong(tail_tag_ptr, new_node_tag_ptr);//CAS操作
						m_lock.unlock();//ロック解除
					#else//USE_SAFE_CAS_LF_QUEUE
						m_tail.compare_exchange_strong(tail_tag_ptr, new_node_tag_ptr);//CAS操作
					#endif//USE_SAFE_CAS_LF_QUEUE
						//【CAS操作の内容】
						//    if(m_tail == tail_tag_ptr)//末尾ノードを他のスレッドが書き換えていないか？
						//        tail_tag_ptr = new_node_tag_ptr;//末尾ノードを新規ノードに変更

						return true;//エンキュー成功
					}
				}
				else//if(next_tag_ptr.isNotNull())//末尾ノードの次ノードが末端ではない（他のスレッドの処理が割り込んでいた場合）
				{
					//CAS操作③：つじつま合わせ処理 ※CAS操作②が失敗する可能性があるため、この処理が必要
				#ifdef USE_SAFE_CAS_LF_QUEUE//【安全対策】※スピンロックでCAS操作を保護
					m_lock.lock();//ロック取得
					m_tail.compare_exchange_weak(tail_tag_ptr, next_tag_ptr);//CAS操作
					m_lock.unlock();//ロック解除
				#else//USE_SAFE_CAS_LF_QUEUE
					m_tail.compare_exchange_weak(tail_tag_ptr, next_tag_ptr);//CAS操作
				#endif//USE_SAFE_CAS_LF_QUEUE
					//【CAS操作の内容】
					//    if(m_tail == tail_tag_ptr)//末尾ノードを他のスレッドが書き換えていないか？
					//        m_tail = next_tag_ptr;//末尾ノードを次ノードに変更
				}
			}
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
		null_tag_ptr.setTagPtr(nullptr, 0);//タグ付きヌルポインタ
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
				#ifdef USE_SAFE_CAS_LF_QUEUE//【安全対策】※スピンロックでCAS操作を保護
					m_lock.lock();//ロック取得
					m_tail.compare_exchange_weak(tail_tag_ptr, next_tag_ptr);//CAS操作
					m_lock.unlock();//ロック解除
				#else//USE_SAFE_CAS_LF_QUEUE
					m_tail.compare_exchange_weak(tail_tag_ptr, next_tag_ptr);//CAS操作
				#endif//USE_SAFE_CAS_LF_QUEUE
					//【CAS操作の内容】
					//    if(m_tail == tail_tag_ptr)//末尾ノードを他のスレッドが書き換えていないか？
					//        m_tail = next_tag_ptr;//末尾ノードを次ノードに変更
				}
				else
				{
					//CAS操作⑤
				#ifdef USE_SAFE_CAS_LF_QUEUE//【安全対策】※スピンロックでCAS操作を保護
					m_lock.lock();//ロック取得
					const bool result = m_head.compare_exchange_weak(head_tag_ptr, next_tag_ptr);//CAS操作
					m_lock.unlock();//ロック解除
					if (result)
				#else//USE_SAFE_CAS_LF_QUEUE
					if (m_head.compare_exchange_weak(head_tag_ptr, next_tag_ptr))//CAS操作
				#endif//USE_SAFE_CAS_LF_QUEUE
					//【CAS操作の内容】
					//    if(m_head == head_tag_ptr)//先頭ノードの次ノードを他のスレッドが書き換えていないか？
					//        m_head = next_tag_ptr;//先頭ノードを次ノードに変更（これより次ノードがダミーノード扱いになる）（デキュー成功）
					{
						queue_t* top = next_tag_ptr;//新しい先頭ノードから値を取り出す
						value = std::move(top->m_value);//値を取得
					#ifdef USE_LF_POOL_ALLOCATOR
						m_allocator.deleteObj(head);//先頭ノード（ダミーノード）を削除
					#else//USE_LF_POOL_ALLOCATOR
						head->~queue_t();
						//operator delete(head, head);
						_aligned_free(head);//先頭ノード（ダミーノード）を削除
					#endif//USE_LF_POOL_ALLOCATOR
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
		printf("----- Debug Info for lf_queue -----\n");
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
	#ifdef USE_LF_POOL_ALLOCATOR
		auto print_allocator_node = [&print_node](const queue_t& info)
		{
			print_node(info.m_value);
		};
		m_allocator.printDebugInfo(print_allocator_node);
	#endif//USE_LF_POOL_ALLOCATOR
	}

private:
	//初期化
	void initialize()
	{
	#ifdef USE_LF_POOL_ALLOCATOR
		queue_t* dummy_node = m_allocator.newObj();//ダミーノードを生成
	#else//USE_LF_POOL_ALLOCATOR
		queue_t* dummy_node = new(_aligned_malloc(sizeof(queue_t), 16)) queue_t();//ダミーノードを生成
	#endif//USE_LF_POOL_ALLOCATOR
		queue_ptr_t null_tag_ptr;
		null_tag_ptr.setTagPtr(nullptr, 0);//タグ付きヌルポインタ
		dummy_node->m_next.store(null_tag_ptr);//ダミーノードの次ノードを初期化
		queue_ptr_t dummy_node_tag_ptr;
		dummy_node_tag_ptr.setTagPtr(dummy_node, 0);
		m_head.store(dummy_node_tag_ptr);//先頭ノードにダミーノードをセット
		m_tail.store(dummy_node_tag_ptr);//末尾ノードにダミーノードをセット
		m_tag.store(0);//タグを初期化
		//if (dummy_node->m_next.is_lock_free())
		//	printf("dummy_node->m_next is lock free!\n");
		//if (m_head.is_lock_free())
		//	printf("m_head is lock free!\n");
		//if (m_tail.is_lock_free())
		//	printf("m_tail is lock free!\n");
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
	#ifdef USE_LF_POOL_ALLOCATOR
		m_allocator.deleteObj(head);
	#else//USE_LF_POOL_ALLOCATOR
		head->~queue_t();
		_aligned_free(head);//先頭ノード（ダミーノード）を削除
	#endif//USE_LF_POOL_ALLOCATOR
	}

public:
	//コンストラクタ
	lf_queue()
	{
		initialize();
	}
	//デストラクタ
	~lf_queue()
	{
		finalize();
	}
private:
	//フィールド
#ifdef USE_LF_POOL_ALLOCATOR
	lf_pool_allocator<queue_t, POOL_SIZE> m_allocator;//プールアロケータ
#endif//USE_LF_POOL_ALLOCATOR
	std::atomic<queue_ptr_t> m_head;//キューの先頭
	std::atomic<queue_ptr_t> m_tail;//キューの末尾
	std::atomic<std::size_t> m_tag;//ABA問題対策用のタグ
#ifdef USE_SAFE_CAS_LF_QUEUE
	spin_lock m_lock;//CAS操作保護用のスピンロック
#endif//USE_SAFE_CAS_LF_QUEUE
};

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__LOCKFREE_QUEUE_H_

// End of file

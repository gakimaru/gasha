#pragma once
#ifndef GASHA_INCLUDED_LOCKFREE_QUEUE_CPP_H
#define GASHA_INCLUDED_LOCKFREE_QUEUE_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_queue.cpp.h
// ロックフリーキュー【関数／実体定義部】
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

#include <gasha/lf_queue.inl>//ロックフリーキュー【インライン関数／テンプレート関数定義部】

#include <gasha/lf_pool_allocator.cpp.h>//ロックフリープールアロケータ【関数／実体定義部】

#include <utility>//C++11 std::move
#include <stdio.h>//sprintf()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】sprintf を使用すると、error C4996 が発生する
//  error C4996: 'sprintf': This function or variable may be unsafe. Consider using strncpy_fast_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

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

//デバッグ情報作成
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
std::size_t lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::debugInfo(char* message, const bool with_detail, std::function<std::size_t(char* message, const typename lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::value_type& value)> print_node)
{
#ifdef GASHA_HAS_DEBUG_FEATURE
	std::size_t size = 0;
	size += sprintf(message + size, "----- Debug Info for lfQueue -----\n");
	size += sprintf(message + size, "Queue:\n");
	int no = 0;
	queue_ptr_t node_tag_ptr = m_head;
	while (node_tag_ptr.isNotNull())
	{
		queue_t* node = node_tag_ptr;
		size += sprintf(message + size, "[%d(tag=%d)](%p) ", no++, node_tag_ptr.tag(), node);
		size += print_node(message + size, node->m_value);
		size += sprintf(message + size, "\n");
		node_tag_ptr = node->m_next.load();
	}
	queue_ptr_t tail_tag_ptr = m_tail.load();
	queue_t* tail = tail_tag_ptr;
	size += sprintf(message + size, "[tail(tag=%d)](%p)", tail_tag_ptr.tag(), tail);
	size += print_node(message + size, tail->m_value);
	size += sprintf(message + size, "\n");
	size += sprintf(message + size, "----------\n");
	auto print_allocator_node = [&print_node](char* message, const queue_t& info) -> std::size_t
	{
		return print_node(message, info.m_value);
	};
	size += m_allocator.template debugInfo<queue_t>(message + size, with_detail, print_allocator_node);
	return size;
#else//GASHA_HAS_DEBUG_FEATURE
	message[0] = '\0';
	return 0;
#endif//GASHA_HAS_DEBUG_FEATURE
}

//初期化
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
void lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::initialize()
{
	queue_t* dummy_node = m_allocator.newDefault();//ダミーノードを生成
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

//----------------------------------------
//明示的なインスタンス化

//ロックフリーキューの明示的なインスタンス化用マクロ
//※デフォルトのタグ付きポインタを使用
#define GASHA_INSTANCING_lfQueue(T, _POOL_SIZE) \
	template class lfQueue<T, _POOL_SIZE, TEST_TAGGED_PTR_TAG_SIZE, TEST_TAGGED_PTR_TAG_SHIFT>;
//※タグ付きポインタの仕様を簡易指定
#define GASHA_INSTANCING_lfQueue_withTag(T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT) \
	template class lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT>;
//※タグ付きポインタの仕様を完全指定
#define GASHA_INSTANCING_lfQueue_withTagDetail(T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE) \
	template class lfQueue<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>;

//※別途、必要に応じてロックフリープールアロケータの明示的なインスタンス化も必要
//　　GASHA_INSTANCING_lfPoolAllocator(_POOL_SIZE);

//--------------------------------------------------------------------------------
//【注】明示的インスタンス化に失敗する場合
//--------------------------------------------------------------------------------
//【原因①】
// 　対象クラスに必要なインターフェースが実装されていない。
//
// 　例えば、ソート処理に必要な「bool operator<(const value_type&) const」か「friend bool operator<(const value_type&, const value_type&)」や、
// 　探索処理に必要な「bool operator==(const key_type&) const」か「friend bool operator==(const value_type&, const key_type&)」。
//
// 　明示的なインスタンス化を行う場合、実際に使用しない関数のためのインターフェースも確実に実装する必要がある。
// 　逆に言えば、明示的なインスタンス化を行わない場合、使用しない関数のためのインターフェースを実装する必要がない。
//
//【対策１】
// 　インターフェースをきちんと実装する。
// 　（無難だが、手間がかかる。）
//
//【対策２】
// 　明示的なインスタンス化を行わずに、.cpp.h をテンプレート使用前にインクルードする。
// 　（手間がかからないが、コンパイル時の依存ファイルが増えるので、コンパイルが遅くなる可能性がある。）
//
//--------------------------------------------------------------------------------
//【原因②】
// 　同じ型のインスタンスが複数作成されている。
//
// 　通常、テンプレートクラス／関数の同じ型のインスタンスが複数作られても、リンク時に一つにまとめられるため問題がない。
// 　しかし、一つのソースファイルの中で複数のインスタンスが生成されると、コンパイラによってはエラーになる。
//   GCCの場合のエラーメッセージ例：（VC++ではエラーにならない）
// 　  source_file.cpp.h:114:17: エラー: duplicate explicit instantiation of ‘class gasha::templateClass<>’ [-fpermissive]
//
//【対策１】
// 　別のファイルに分けてインスタンス化する。
// 　（コンパイルへの影響が少なく、良い方法だが、無駄にファイル数が増える可能性がある。）
//
//【対策２】
// 　明示的なインスタンス化を行わずに、.cpp.h をテンプレート使用前にインクルードする。
// 　（手間がかからないが、コンパイル時の依存ファイルが増えるので、コンパイルが遅くなる可能性がある。）
//
//【対策３】
// 　GCCのコンパイラオプションに、 -fpermissive を指定し、エラーを警告に格下げする。
// 　（最も手間がかからないが、常時多数の警告が出る状態になりかねないので注意。）
//--------------------------------------------------------------------------------
// ※このコメントは、「明示的なインスタンス化マクロ」が定義されている全てのソースコードに
// 　同じ内容のものをコピーしています。

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_LOCKFREE_QUEUE_CPP_H

// End of file

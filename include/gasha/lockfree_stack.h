#pragma once
#ifndef __LOCKFREE_STACK_H_
#define __LOCKFREE_STACK_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_stack.h
// ロックフリースタック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリースタッククラス
//※ABA問題対策あり（タグ付きポインタ型使用）
#ifdef USE_LF_POOL_ALLOCATOR
template<class T, std::size_t POOL_SIZE>
#else//USE_LF_POOL_ALLOCATOR
template<class T>
#endif//USE_LF_POOL_ALLOCATOR
class lf_stack
{
public:
	//型
	typedef T value_type;//値型

	//スタック型用タグ付きポインタ型
	struct stack_t;
	typedef tag_ptr<stack_t> stack_ptr_t;
	
	//スタック型
	struct stack_t
	{
		std::atomic<stack_ptr_t> m_next;//次のノード
		value_type m_value;//値
		//コンストラクタ ※値の受け渡しはムーブコンストラクタを使用
		stack_t(value_type&& value) :
			m_value(std::move(value))
		{}
	};

public:
	//メソッド

	//プッシュ
	bool push(value_type&& value)
	{
	#ifdef USE_LF_POOL_ALLOCATOR
		void* p = m_allocator.alloc();//新規ノードのメモリを確保
	#else//USE_LF_POOL_ALLOCATOR
		void* p = _aligned_malloc(sizeof(stack_t), 16);//新規ノードのメモリを確保
	#endif//USE_LF_POOL_ALLOCATOR
		if (!p)//メモリ確保失敗
			return nullptr;//プッシュ失敗
		stack_t* new_node = new(p)stack_t(std::move(value));//新規ノードのコンストラクタ呼び出し
		new_node->m_next.store(m_head.load());//新規ノードの次ノードに現在の先頭ノードをセット
		stack_ptr_t new_node_tag_ptr;
		new_node_tag_ptr.setTagPtr(new_node, m_tag.fetch_add(1));//タグ付きポインタ生成
		while (true)
		{
			stack_ptr_t next_tag_ptr = new_node->m_next.load();//新規ノードの次ノードを取得

			//CAS操作①
		#ifdef USE_SAFE_CAS_LF_STACK//【安全対策】※スピンロックでCAS操作を保護
			m_lock.lock();//ロック取得
			const bool result = m_head.compare_exchange_weak(next_tag_ptr, new_node_tag_ptr);//CAS操作
			m_lock.unlock();//ロック解除
			if (result)
		#else//USE_SAFE_CAS_LF_STACK
			if (m_head.compare_exchange_weak(next_tag_ptr, new_node_tag_ptr))//CAS操作
		#endif//USE_SAFE_CAS_LF_STACK
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
	bool push(value_type& value)
	{
		return push(std::move(value));//常にムーブ処理を適用
	}

	//ポップ
	bool pop(value_type& value)
	{
		stack_ptr_t head_tag_ptr = m_head.load();//先頭ノードを取得
		while (head_tag_ptr.isNotNull())
		{
			stack_t* head = head_tag_ptr;//タグ付きポインタからポインタを取得
			stack_ptr_t next_tag_ptr = head->m_next;//次のノードを取得

			//CAS操作②
		#ifdef USE_SAFE_CAS_LF_STACK//【安全対策】※スピンロックでCAS操作を保護
			m_lock.lock();//ロック取得
			const bool result = m_head.compare_exchange_weak(head_tag_ptr, next_tag_ptr);//CAS操作
			m_lock.unlock();//ロック解除
			if (result)
		#else//USE_SAFE_CAS_LF_STACK
			if (m_head.compare_exchange_weak(head_tag_ptr, next_tag_ptr))//CAS操作
		#endif//USE_SAFE_CAS_LF_STACK
			//【CAS操作の内容】
			//    if(m_head == head_tag_ptr)//先頭ノードを他のスレッドが書き換えていないか？
			//        m_head = next_tag_ptr;//先頭ノードを先頭ノードの次ノードに置き換えて完了（ポップ成功）
			//    else
			//        head_tag_ptr = m_head;//先頭ノードを再取得
			{
				value = std::move(head->m_value);//値を取得
			#ifdef USE_LF_POOL_ALLOCATOR
				m_allocator.deleteObj(head);//先頭ノードを削除
			#else//USE_LF_POOL_ALLOCATOR
				head->~stack_t();
				//operator delete(head, head);
				_aligned_free(head);//先頭ノードを削除
			#endif//USE_LF_POOL_ALLOCATOR
				return true;//ポップ成功
			}
		}
		return false;//ポップ失敗
	}

	//デバッグ情報表示
	void printDebugInfo(std::function<void(const value_type& value)> print_node)
	{
		printf("----- Debug Info for lf_stack -----\n");
		printf("Stack:\n");
		int no = 0;
		stack_ptr_t node_tag_ptr = m_head.load();
		while (node_tag_ptr.isNotNull())
		{
			const stack_t* node = node_tag_ptr;
			printf("[%d(tag=%d)](%p) ", no++, node, node_tag_ptr.tag(), node);
			print_node(node->m_value);
			printf("\n");
			node_tag_ptr = node->m_next;
		}
		printf("----------\n");
	#ifdef USE_LF_POOL_ALLOCATOR
		auto print_allocator_node = [&print_node](const stack_t& info)
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
		stack_ptr_t null_tag_ptr;
		null_tag_ptr.setTagPtr(nullptr, 0);//タグ付きヌルポインタ
		m_head.store(null_tag_ptr);//先頭ノードの初期値はnull
		m_tag.store(0);//タグを初期化
		//if (m_head.is_lock_free())
		//	printf("m_head is lock free!\n");
	}
	//終了
	void finalize()
	{
		//空になるまでポップ
		value_type value;
		while (pop(value));
	}

public:
	//コンストラクタ
	lf_stack()
	{
		initialize();
	}
	//デストラクタ
	~lf_stack()
	{
		finalize();
	}
private:
	//フィールド
#ifdef USE_LF_POOL_ALLOCATOR
	lf_pool_allocator<stack_t, POOL_SIZE> m_allocator;//プールアロケータ
#endif//USE_LF_POOL_ALLOCATOR
	std::atomic<stack_ptr_t> m_head;//スタックの先頭　※タグ付きポインタ
	std::atomic<std::size_t> m_tag;//ABA問題対策用のタグ
#ifdef USE_SAFE_CAS_LF_STACK
	spin_lock m_lock;//CAS操作保護用のスピンロック
#endif//USE_SAFE_CAS_LF_STACK
};

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__LOCKFREE_STACK_H_

// End of file

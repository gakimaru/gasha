#pragma once
#ifndef __SHARED_STACK_H_
#define __SHARED_STACK_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_stack.h
// マルチスレッド同期型スタック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//共有スタッククラス
#ifdef USE_POOL_ALLOCATOR
template<class T, std::size_t POOL_SIZE>
#else//USE_POOL_ALLOCATOR
template<class T>
#endif//USE_POOL_ALLOCATOR
class sharedStack
{
public:
	//型
	typedef T value_type;//値型

	//スタック型
	struct stack_t
	{
		stack_t* m_next;//次のノード
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
	#ifdef USE_POOL_ALLOCATOR
		void* p = m_allocator.alloc();//新規ノードのメモリを確保
	#else//USE_POOL_ALLOCATOR
		void* p = _aligned_malloc(sizeof(stack_t), 16);//新規ノードのメモリを確保
	#endif//USE_POOL_ALLOCATOR
		if (!p)//メモリ確保失敗
			return false;//プッシュ失敗
		stack_t* new_node = new(p)stack_t(std::move(value));//新規ノードのコンストラクタ呼び出し
		std::lock_guard<normal_lock> lock(m_lock);//ロック（スコープロック）
		new_node->m_next = m_head;//新規ノードの次ノードに現在の先頭ノードをセット
		m_head = new_node;//先頭ノードを新規ノードにする
		return true;//プッシュ成功
	}
	bool push(value_type& value)
	{
		return push(std::move(value));//常にムーブ処理を適用
	}

	//ポップ
	bool pop(value_type& value)
	{
		std::lock_guard<normal_lock> lock(m_lock);//ロック（スコープロック）
		stack_t* head = m_head;//先頭ノードを取得
		if (head)//先頭ノードが存在していた場合
		{
			m_head = head->m_next;//先頭ノードを次ノードに変更
			value = std::move(head->m_value);//値を取得
		#ifdef USE_POOL_ALLOCATOR
			m_allocator.deleteObj(head);//先頭ノードを削除
		#else//USE_POOL_ALLOCATOR
			delete head;//先頭ノードを削除
		#endif//USE_POOL_ALLOCATOR
			return true;//ポップ成功
		}
		return false;//ポップ失敗
	}

	//デバッグ情報表示
	void printDebugInfo(std::function<void(const value_type& value)> print_node)
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
	#ifdef USE_POOL_ALLOCATOR
		auto print_allocator_node = [&print_node](const stack_t& info)
		{
			print_node(info.m_value);
		};
		m_allocator.printDebugInfo(print_allocator_node);
	#endif//USE_POOL_ALLOCATOR
	}

private:
	//初期化
	void initialize()
	{
		m_head = nullptr;//先頭ノードの初期値はnullptr
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
	sharedStack()
	{
		initialize();
	}
	//デストラクタ
	~sharedStack()
	{
		finalize();
	}
private:
	//フィールド
#ifdef USE_POOL_ALLOCATOR
	pool_allocator<stack_t, POOL_SIZE> m_allocator;//プールアロケータ
#endif//USE_POOL_ALLOCATOR
	stack_t* m_head;//スタックの先頭
	normal_lock m_lock;//ロックオブジェクト（ミューテックスorスピンロック）
};

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__SHARED_STACK_H_

// End of file

#pragma once
#ifndef __SHARED_STACK_H_
#define __SHARED_STACK_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_stack.h
// マルチスレッド共有スタック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/spin_lock.h>//スピンロック
#include <gasha/dummy_lock.h>//ダミーロック
#include <gasha/shared_pool_allocator.h>//マルチスレッド共有プールアロケータ

#include <cstddef>//std::size_t
#include <stdio.h>//printf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//マルチスレッド共有スタッククラス
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE = GASHA_ spinLock>
class sharedStack
{
public:
	//型
	typedef T value_type;//値型
	typedef LOCK_TYPE lock_type;//ロック型

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
		GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
		void* p = m_allocator.alloc();//新規ノードのメモリを確保
		if (!p)//メモリ確保失敗
			return false;//プッシュ失敗
		stack_t* new_node = new(p)stack_t(std::move(value));//新規ノードのコンストラクタ呼び出し
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
		auto print_allocator_node = [&print_node](const stack_t& info)
		{
			print_node(info.m_value);
		};
		m_allocator.printDebugInfo(print_allocator_node);
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
	sharedPoolAllocator<stack_t, POOL_SIZE, GASHA_ dummyLock> m_allocator;//プールアロケータ（プールアロケータ自体はロック制御しない）
	stack_t* m_head;//スタックの先頭
	lock_type m_lock;//ロックオブジェクト
};

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__SHARED_STACK_H_

// End of file

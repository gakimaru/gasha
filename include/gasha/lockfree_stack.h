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

#include <gasha/lockfree_pool_allocator.h>//ロックフリープールアロケータ
#include <gasha/tagged_ptr.h>//タグ付きポインタ
#include <gasha/basic_math.h>//基本算術：calcStaticMSB()

#include <cstddef>//std::size_t
#include <cstdint>//std::uint32_t
#include <atomic>//C++11 std::atomic
#include <stdio.h>//printf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリースタッククラス
//※ABA問題対策あり（タグ付きポインタ型使用）
//
//【テンプレート引数の説明】
//・T ... スタックのデータ型
//・_POOL_SIZE ... 同時にスタック可能なデータの最大個数
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
class lfStack
{
public:
	//定数
	static const std::size_t POOL_SIZE = _POOL_SIZE;//プールアロケータのプールサイズ（プールする個数）
	static const std::size_t TAGGED_PTR_TAG_BITS = _TAGGED_PTR_TAG_BITS == 0 ? GASHA_ calcStaticMSB<alignof(T)>::value : _TAGGED_PTR_TAG_BITS;//タグ付きポインタのタグのビット長
	static const int TAGGED_PTR_TAG_SHIFT = _TAGGED_PTR_TAG_SHIFT;//タグ付きポインタのタグの位置

public:
	//型
	typedef T value_type;//値型

	//スタック型用タグ付きポインタ型
	struct stack_t;
	typedef TAGGED_PTR_VALUE_TYPE tagged_ptr_value_type;//タグ付きポインタの値の型
	typedef TAGGED_PTR_TAG_TYPE tagged_ptr_tag_type;//タグ付きポインタのタグの型
	typedef GASHA_ taggedPtr<stack_t, TAGGED_PTR_TAG_BITS, TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE> stack_ptr_t;
	
	//アロケータ型
	typedef GASHA_ lfPoolAllocator<stack_t, POOL_SIZE> allocator_type;//ロックフリープールアロケータ

	//スタック型
	struct stack_t
	{
		value_type m_value;//値
		std::atomic<stack_ptr_t> m_next;//次のノード
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
		void* p = m_allocator.alloc();//新規ノードのメモリを確保
		if (!p)//メモリ確保失敗
			return nullptr;//プッシュ失敗
		stack_t* new_node = new(p)stack_t(std::move(value));//新規ノードのコンストラクタ呼び出し
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
	void printDebugInfo(std::function<void(const value_type& value)> print_node)
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

private:
	//初期化
	void initialize()
	{
		stack_ptr_t null_tag_ptr;
		null_tag_ptr.set(nullptr, 0);//タグ付きヌルポインタ
		m_head.store(null_tag_ptr);//先頭ノードの初期値はnull
		m_tag.store(0);//タグを初期化
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
	lfStack()
	{
		initialize();
	}
	//デストラクタ
	~lfStack()
	{
		finalize();
	}
private:
	//フィールド
	allocator_type m_allocator;//アロケータ
	std::atomic<stack_ptr_t> m_head;//スタックの先頭　※タグ付きポインタ
	std::atomic<typename stack_ptr_t::tag_type> m_tag;//ABA問題対策用のタグ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__LOCKFREE_STACK_H_

// End of file

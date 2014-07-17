#pragma once
#ifndef GASHA_INCLUDED_LOCKFREE_STACK_CPP_H
#define GASHA_INCLUDED_LOCKFREE_STACK_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_stack.cpp.h
// ロックフリースタック【関数／実体定義部】
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

#include <gasha/lf_stack.inl>//ロックフリースタック【インライン関数／テンプレート関数定義部】

#include <gasha/lf_pool_allocator.cpp.h>//ロックフリープールアロケータ【関数／実体定義部】
#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し
#include <gasha/string.h>//文字列処理：spprintf()

#include <utility>//C++11 std::move

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリースタッククラス

//プッシュ
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
inline bool lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::_push(typename lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::stack_t* new_node)
{
	new_node->m_next.store(m_head.load());//新規ノードの次ノードに現在の先頭ノードをセット
	stack_ptr_type new_node_tag_ptr;
	new_node_tag_ptr.set(new_node, m_tag.fetch_add(1));//タグ付きポインタ生成
	while (true)
	{
		stack_ptr_type next_tag_ptr = new_node->m_next.load();//新規ノードの次ノードを取得

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
	stack_t* new_node = GASHA_ callConstructor<stack_t>(p, std::move(value));//新規ノードのコンストラクタ呼び出し
	return _push(new_node);
}
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
bool lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::push(typename lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::value_type& value)//※コピー版
{
	void* p = m_allocator.alloc();//新規ノードのメモリを確保
	if (!p)//メモリ確保失敗
		return nullptr;//プッシュ失敗
	stack_t* new_node = GASHA_ callConstructor<stack_t>(p, value);//新規ノードのコンストラクタ呼び出し
	return _push(new_node);
}

//ポップ
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
bool lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::pop(typename lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::value_type& value)
{
	stack_ptr_type head_tag_ptr = m_head.load();//先頭ノードを取得
	while (head_tag_ptr.isNotNull())
	{
		stack_t* head = head_tag_ptr;//タグ付きポインタからポインタを取得
		stack_ptr_type next_tag_ptr = head->m_next;//次のノードを取得

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

//デバッグ情報作成
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
std::size_t lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::debugInfo(char* message, const bool with_detail, std::function<std::size_t(char* message, const typename lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::value_type& value)> print_node)
{
	std::size_t size = 0;
	GASHA_ spprintf(message, size, "----- Debug Info for lfStack -----\n");
	GASHA_ spprintf(message, size, "Stack:\n");
	int no = 0;
	stack_ptr_type node_tag_ptr = m_head.load();
	while (node_tag_ptr.isNotNull())
	{
		const stack_t* node = node_tag_ptr;
		GASHA_ spprintf(message, size, "[%d(tag=%d)](%p) ", no++, node_tag_ptr.tag(), node);
		size += print_node(message + size, node->m_value);
		GASHA_ spprintf(message, size, "\n");
		node_tag_ptr = node->m_next;
	}
	GASHA_ spprintf(message, size, "----------------------------------\n");
	auto print_allocator_node = [&print_node](char* message, const stack_t& info) -> std::size_t
	{
		return print_node(message, info.m_value);
	};
	size += m_allocator.template debugInfo<stack_t>(message + size, with_detail, print_allocator_node);
	return size;
}

//初期化
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS, int _TAGGED_PTR_TAG_SHIFT, typename TAGGED_PTR_VALUE_TYPE, typename TAGGED_PTR_TAG_TYPE>
void lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>::initialize()
{
	stack_ptr_type null_tag_ptr;
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

//----------------------------------------
//明示的なインスタンス化

//ロックフリースタックの明示的なインスタンス化用マクロ
//※デフォルトのタグ付きポインタを使用
#define GASHA_INSTANCING_lfStack(T, _POOL_SIZE) \
	template class GASHA_ lfStack<T, _POOL_SIZE>;
//※タグ付きポインタの仕様を簡易指定
#define GASHA_INSTANCING_lfStack_withTag(T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT) \
	template class GASHA_ lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT>;
//※タグ付きポインタの仕様を完全指定
#define GASHA_INSTANCING_lfStack_withTagDetail(T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE) \
	template class GASHA_ lfStack<T, _POOL_SIZE, _TAGGED_PTR_TAG_BITS, _TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE>;

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

#endif//GASHA_INCLUDED_LOCKFREE_STACK_CPP_H

// End of file

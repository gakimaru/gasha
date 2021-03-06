﻿#pragma once
#ifndef GASHA_INCLUDED_SHARED_STACK_CPP_H
#define GASHA_INCLUDED_SHARED_STACK_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_stack.cpp.h
// マルチスレッド共有スタック【関数／実体定義部】
//
// ※クラスのインスタンス化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。
// 　（コンパイル・リンク時間への影響を気にしないならOK）
// ※明示的なインスタンス化を避けたい場合は、ヘッダーファイルと共にインクルード。
// 　（この場合、実際に使用するメンバー関数しかインスタンス化されないので、対象クラスに不要なインターフェースを実装しなくても良い）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shared_stack.inl>//共有スタック【インライン関数／テンプレート関数定義部】

#include <gasha/pool_allocator.cpp.h>//プールアロケータ【関数／実体定義部】
#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し
#include <gasha/string.h>//文字列処理：spprintf()

#include <utility>//C++11 std::move

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//マルチスレッド共有スタッククラス

//プッシュ
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
inline bool sharedStack<T, POOL_SIZE, LOCK_POLICY>::_push(typename sharedStack<T, POOL_SIZE, LOCK_POLICY>::stack_t* new_node)
{
	new_node->m_next = m_head;//新規ノードの次ノードに現在の先頭ノードをセット
	m_head = new_node;//先頭ノードを新規ノードにする
	return true;//プッシュ成功
}
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
bool sharedStack<T, POOL_SIZE, LOCK_POLICY>::push(typename sharedStack<T, POOL_SIZE, LOCK_POLICY>::value_type&& value)//※ムーブ版
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	void* p = m_allocator.alloc();//新規ノードのメモリを確保
	if (!p)//メモリ確保失敗
		return false;//プッシュ失敗
	stack_t* new_node = GASHA_ callConstructor<stack_t>(p, std::move(value));//新規ノードのコンストラクタ呼び出し
	return _push(new_node);
}
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
bool sharedStack<T, POOL_SIZE, LOCK_POLICY>::push(typename sharedStack<T, POOL_SIZE, LOCK_POLICY>::value_type& value)//※コピー版
{
	//return push(std::move(value));//常にムーブ処理を適用
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	void* p = m_allocator.alloc();//新規ノードのメモリを確保
	if (!p)//メモリ確保失敗
		return false;//プッシュ失敗
	stack_t* new_node = GASHA_ callConstructor<stack_t>(p, value);//新規ノードのコンストラクタ呼び出し
	return _push(new_node);
}

//ポップ
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
bool sharedStack<T, POOL_SIZE, LOCK_POLICY>::pop(typename sharedStack<T, POOL_SIZE, LOCK_POLICY>::value_type& value)
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

//デバッグ情報作成
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
std::size_t sharedStack<T, POOL_SIZE, LOCK_POLICY>::debugInfo(char* message, const std::size_t max_size, const bool with_detail, std::function<std::size_t(char* message, const std::size_t max_size, std::size_t& size, const typename sharedStack<T, POOL_SIZE, LOCK_POLICY>::value_type& value)> print_node) const
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	std::size_t message_len = 0;
	GASHA_ spprintf(message, max_size, message_len, "----- Debug-info for stack -----\n");
	GASHA_ spprintf(message, max_size, message_len, "Stack:\n");
	int no = 0;
	stack_t* node = m_head;
	while (node)
	{
		GASHA_ spprintf(message, max_size, message_len, "[%d](%p) ", no++, node);
		print_node(message, max_size, message_len, node->m_value);
		GASHA_ spprintf(message, max_size, message_len, "\n");
		node = node->m_next;
	}
	GASHA_ spprintf(message, max_size, message_len, "--------------------------------\n");
	auto print_allocator_node = [&print_node](char* message, const std::size_t max_size, std::size_t& message_len, const stack_t& info) -> std::size_t
	{
		return print_node(message, max_size, message_len, info.m_value);
	};
	message_len += m_allocator.template debugInfo<stack_t>(message + message_len, max_size - message_len, with_detail, print_allocator_node);
	return message_len;
}

//初期化
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
void sharedStack<T, POOL_SIZE, LOCK_POLICY>::initialize()
{
	m_head = nullptr;//先頭ノードの初期値はnullptr
}

//終了
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
void sharedStack<T, POOL_SIZE, LOCK_POLICY>::finalize()
{
	//空になるまでポップ
	value_type value;
	while (pop(value));
}

//コンストラクタ
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
sharedStack<T, POOL_SIZE, LOCK_POLICY>::sharedStack()
{
	initialize();
}

//デストラクタ
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
sharedStack<T, POOL_SIZE, LOCK_POLICY>::~sharedStack()
{
	finalize();
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//マルチスレッド共有スタックの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_sharedStack(T, _POOL_SIZE) \
	template class GASHA_ sharedStack<T, _POOL_SIZE>; \
	template class GASHA_ poolAllocator_withType<typename GASHA_ sharedStack<T, _POOL_SIZE>::stack_t, _POOL_SIZE, GASHA_ dummyLock>;
//※ロック指定版
#define GASHA_INSTANCING_sharedStack_withLock(T, _POOL_SIZE, LOCK_POLICY) \
	template class GASHA_ sharedStack<T, _POOL_SIZE, LOCK_POLICY>; \
	template class GASHA_ poolAllocator_withType<typename GASHA_ sharedStack<T, _POOL_SIZE, LOCK_POLICY>::stack_t, _POOL_SIZE, GASHA_ dummyLock>;

//※別途、必要に応じてプールアロケータの明示的なインスタンス化も必要（ロックなし版のみ使用）
//　　GASHA_INSTANCING_poolAllocator(_POOL_SIZE);//※ロックなし版

//--------------------------------------------------------------------------------
//【注】明示的インスタンス化に失敗する場合
// ※このコメントは、「明示的なインスタンス化マクロ」が定義されている全てのソースコードに
// 　同じ内容のものをコピーしています。
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
// 　  source_file.cpp.h:114:17: エラー: duplicate explicit instantiation of ‘class templateClass<>’ [-fpermissive]
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

#endif//GASHA_INCLUDED_SHARED_STACK_CPP_H

// End of file

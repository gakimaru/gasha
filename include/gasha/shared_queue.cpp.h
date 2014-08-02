#pragma once
#ifndef GASHA_INCLUDED_SHARED_QUEUE_CPP_H
#define GASHA_INCLUDED_SHARED_QUEUE_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_queue.cpp.h
// マルチスレッド共有キュー【関数／実体定義部】
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

#include <gasha/shared_queue.inl>//共有キュー【インライン関数／テンプレート関数定義部】

#include <gasha/pool_allocator.cpp.h>//プールアロケータ【関数／実体定義部】
#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し
#include <gasha/string.h>//文字列処理：spprintf()

#include <utility>//C++11 std::move

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//マルチスレッド共有キュークラス

//エンキュー
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
inline bool sharedQueue<T, POOL_SIZE, LOCK_POLICY>::_enqueue(typename sharedQueue<T, POOL_SIZE, LOCK_POLICY>::queue_t* new_node)
{
	new_node->m_next = nullptr;//新規ノードの次ノードを初期化
	m_tail->m_next = new_node;//末尾ノードの次ノードを新規ノードにする
	m_tail = new_node;//末尾ノードを新規ノードにする
	return true;//エンキュー成功
}
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
bool sharedQueue<T, POOL_SIZE, LOCK_POLICY>::enqueue(typename sharedQueue<T, POOL_SIZE, LOCK_POLICY>::value_type&& value)//※ムーブ版
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	void* p = m_allocator.alloc();//新規ノードのメモリを確保
	if (!p)//メモリ確保失敗
		return false;//エンキュー失敗
	queue_t* new_node = GASHA_ callConstructor<queue_t>(p, std::move(value));//新規ノードのコンストラクタ呼び出し
	return _enqueue(new_node);
}
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
bool sharedQueue<T, POOL_SIZE, LOCK_POLICY>::enqueue(typename sharedQueue<T, POOL_SIZE, LOCK_POLICY>::value_type& value)//※コピー版
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	void* p = m_allocator.alloc();//新規ノードのメモリを確保
	if (!p)//メモリ確保失敗
		return false;//エンキュー失敗
	queue_t* new_node = GASHA_ callConstructor<queue_t>(p, value);//新規ノードのコンストラクタ呼び出し
	return _enqueue(new_node);
}

//デキュー
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
bool sharedQueue<T, POOL_SIZE, LOCK_POLICY>::dequeue(typename sharedQueue<T, POOL_SIZE, LOCK_POLICY>::value_type& value)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (m_head != m_tail)
	{
		queue_t* top = m_head->m_next;//次ノード（有効なキューの先頭）を取得
		m_allocator.deleteObj(m_head);//先頭ノード（ダミーノード）を削除
		m_head = top;//先頭ノードを次ノードに変更（これより次ノードがダミーノード扱いになる）
		value = std::move(top->m_value);//値を取得
		return true;//デキュー成功
	}
	return false;//デキュー失敗
}

//デバッグ情報作成
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
std::size_t sharedQueue<T, POOL_SIZE, LOCK_POLICY>::debugInfo(char* message, const std::size_t max_size, const bool with_detail, std::function<std::size_t(char* message, const std::size_t max_size, std::size_t& size, const typename sharedQueue<T, POOL_SIZE, LOCK_POLICY>::value_type& value)> print_node) const
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	std::size_t message_len = 0;
	GASHA_ spprintf(message, max_size, message_len, "----- Debug-info for queue -----\n");
	GASHA_ spprintf(message, max_size, message_len, "Queue:\n");
	int no = 0;
	const queue_t* node = m_head;
	while (node)
	{
		GASHA_ spprintf(message, max_size, message_len, "[%d](%p) ", no++, node);
		print_node(message, max_size, message_len, node->m_value);
		GASHA_ spprintf(message, max_size, message_len, "\n");
		node = node->m_next;
	}
	GASHA_ spprintf(message, max_size, message_len, "[tail](%p)", m_tail);
	print_node(message, max_size, message_len, m_tail->m_value);
	GASHA_ spprintf(message, max_size, message_len, "\n");
	GASHA_ spprintf(message, max_size, message_len, "--------------------------------\n");
	auto print_allocator_node = [&print_node](char* message, const std::size_t max_size, std::size_t& message_len, const queue_t& info) -> std::size_t
	{
		return print_node(message, max_size, message_len, info.m_value);
	};
	message_len += m_allocator.template debugInfo<queue_t>(message + message_len, max_size - message_len, with_detail, print_allocator_node);
	return message_len;
}

//初期化
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
void sharedQueue<T, POOL_SIZE, LOCK_POLICY>::initialize()
{
	queue_t* dummy_node = m_allocator.newDefault();//ダミーノードを生成
	dummy_node->m_next = nullptr;//ダミーノードの次ノードを初期化
	m_head = dummy_node;//先頭ノードにダミーノードをセット
	m_tail = dummy_node;//末尾ノードにダミーノードをセット
}

//終了
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
void sharedQueue<T, POOL_SIZE, LOCK_POLICY>::finalize()
{
	//空になるまでデキュー
	value_type value;
	while (dequeue(value));
	//ダミーノードを削除
	m_allocator.deleteObj(m_head);
}

//コンストラクタ
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
sharedQueue<T, POOL_SIZE, LOCK_POLICY>::sharedQueue()
{
	initialize();
}

//デストラクタ
template<class T, std::size_t POOL_SIZE, class LOCK_POLICY>
sharedQueue<T, POOL_SIZE, LOCK_POLICY>::~sharedQueue()
{
	finalize();
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//マルチスレッド共有キューの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_sharedQueue(T, _POOL_SIZE) \
	template class GASHA_ sharedQueue<T, _POOL_SIZE>;
//※ロック指定版
#define GASHA_INSTANCING_sharedQueue_withLock(T, _POOL_SIZE, LOCK_POLICY) \
	template class GASHA_ sharedQueue<T, _POOL_SIZE, LOCK_POLICY>;

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

#endif//GASHA_INCLUDED_SHARED_QUEUE_CPP_H

// End of file

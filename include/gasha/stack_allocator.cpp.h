﻿#pragma once
#ifndef GASHA_INCLUDED_STACK_ALLOCATOR_CPP_H
#define GASHA_INCLUDED_STACK_ALLOCATOR_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// stack_allocator.cpp.h
// スタックアロケータ【関数定義部】
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

#include <gasha/stack_allocator.inl>//スタックアロケータ【インライン関数／テンプレート関数定義部】

#include <gasha/string.h>//文字列処理：spprintf()
#include <gasha/simple_assert.h>//シンプルアサーション

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//スタックアロケータクラス

//メモリ確保
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
void* stackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::alloc(const std::size_t size, const std::size_t align)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	//サイズが0バイトならサイズを1に、アラインメントを0にする
	//※要求サイズが0でも必ずメモリを割り当てる点に注意（ただし、アラインメントは守らない）
	const std::size_t _size = size == 0 ? 1 : size;
	const std::size_t _align = size == 0 ? 0 : align;
	//サイズとアラインメントをチェック
	size_type now_size = m_size;
	char* now_ptr = m_buffRef + now_size;
	char* new_ptr = adjustAlign(now_ptr, _align);
	const std::ptrdiff_t padding_size = new_ptr - now_ptr;
	const size_type alloc_size = static_cast<size_type>(padding_size + _size);
	const size_type new_size = now_size + alloc_size;
#ifdef GASHA_STACK_ALLOCATOR_ENABLE_ASSERTION
	GASHA_SIMPLE_ASSERT(new_size <= m_maxSize, "stackAllocator is not enough memory.");
#endif//GASHA_STACK_ALLOCATOR_ENABLE_ASSERTION
	if (new_size > m_maxSize)
		return nullptr;
	
	//使用中のサイズとメモリ確保数を更新
	m_size = new_size;
	++m_count;

	//空き領域を確保
	return reinterpret_cast<void*>(new_ptr);
}

//使用中のサイズを指定位置に戻す
//※ポインタ指定版
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
bool stackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::rewind(void* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	m_size = static_cast<size_type>(reinterpret_cast<char*>(p) - m_buffRef);
	return true;
}

//デバッグ情報作成
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
std::size_t stackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::debugInfo(char* message, const std::size_t max_size) const
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	std::size_t message_len = 0;
	GASHA_ spprintf(message, max_size, message_len, "----- Debug-info for stackAllocator -----\n");
	GASHA_ spprintf(message, max_size, message_len, "buff=%p, maxSize=%d, size=%d, remain=%d, count=%d\n", m_buffRef, maxSize(), this->size(), remain(), count());
	GASHA_ spprintf(message, max_size, message_len, "-----------------------------------------");//最終行改行なし
	return message_len;
}

//使用中のサイズと数を取得
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
void stackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::getSizeAndCount(typename stackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::size_type& size, typename stackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::size_type& count)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	//使用中のサイズとメモリ確保数を取得
	size = m_size;
	count = m_count;
}

//使用中のサイズと数をリセット
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
bool stackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::resetSizeAndCount(const typename stackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::size_type size, const typename stackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::size_type count)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (m_size < size || m_count < count)//現在の値の方が小さい場合は失敗
		return false;
	//使用中のサイズとメモリ確保数を更新
	m_size = size;
	m_count = count;
	return true;
}

//メモリ解放（共通処理）
template<class LOCK_POLICY, class AUTO_CLEAR_POLICY>
bool stackAllocator<LOCK_POLICY, AUTO_CLEAR_POLICY>::_free(void* p)
{
	//メモリ確保数を更新
	--m_count;
	//自動クリア呼び出し
	AUTO_CLEAR_POLICY auto_clear;
	auto_clear.autoClear(*this);
	return true;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_stackAllocator() \
	template class GASHA_ stackAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_stackAllocator_withLock(LOCK_POLICY) \
	template class GASHA_ stackAllocator<LOCK_POLICY>;

//スマートスタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_smartStackAllocator() \
	template class GASHA_ stackAllocator<GASHA_ dummyLock, GASHA_ stackAllocatorAutoClear>;
//※ロック指定版
#define GASHA_INSTANCING_smartStackAllocator_withLock(LOCK_POLICY) \
	template class GASHA_ stackAllocator<LOCK_POLICY, GASHA_ stackAllocatorAutoClear>;

#if 0//不要
//バッファ付きスタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_stackAllocator_withBuff(_MAX_SIZE) \
	template class GASHA_ stackAllocator_withBuff<_MAX_SIZE>; \
	template class GASHA_ stackAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_stackAllocator_withBuff_withLock(_MAX_SIZE, LOCK_POLICY) \
	template class GASHA_ stackAllocator_withBuff<_MAX_SIZE, LOCK_POLICY>; \
	template class GASHA_ stackAllocator<LOCK_POLICY>;

//バッファ付きスマートスタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_smartStackAllocator_withBuff(_MAX_SIZE) \
	template class GASHA_ stackAllocator_withBuff<_MAX_SIZE, GASHA_ dummyLock, GASHA_ stackAllocatorAutoClear>; \
	template class GASHA_ stackAllocator<GASHA_ dummyLock, GASHA_ stackAllocatorAutoClear>;
//※ロック指定版
#define GASHA_INSTANCING_smartStackAllocator_withBuff_withLock(_MAX_SIZE, LOCK_POLICY) \
	template class GASHA_ stackAllocator_withBuff<_MAX_SIZE, LOCK_POLICY, GASHA_ stackAllocatorAutoClear>; \
	template class GASHA_ stackAllocator<LOCK_POLICY, GASHA_ stackAllocatorAutoClear>;

//型指定バッファ付きスタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_stackAllocator_withType(T, _NUM) \
	template class GASHA_ stackAllocator_withType<T, _NUM>; \
	template class GASHA_ stackAllocator_withBuff<sizeof(T) * _NUM>; \
	template class GASHA_ stackAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_stackAllocator_withType_withLock(T, _NUM, LOCK_POLICY) \
	template class GASHA_ stackAllocator_withType<T, _NUM, LOCK_POLICY>; \
	template class GASHA_ stackAllocator_withBuff<sizeof(T)* _NUM, LOCK_POLICY>; \
	template class GASHA_ stackAllocator<LOCK_POLICY>;

//型指定バッファ付きスマートスタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_smartStackAllocator_withType(T, _NUM) \
	template class GASHA_ stackAllocator_withType<T, _NUM, GASHA_ dummyLock, GASHA_ stackAllocatorAutoClear>; \
	template class GASHA_ stackAllocator_withBuff<sizeof(T)* _NUM, GASHA_ dummyLock, GASHA_ stackAllocatorAutoClear>; \
	template class GASHA_ stackAllocator<GASHA_ dummyLock, stackAllocatorAutoClear>;
//※ロック指定版
#define GASHA_INSTANCING_smartStackAllocator_withType_withLock(T, _NUM, LOCK_POLICY) \
	template class GASHA_ stackAllocator_withType<T, _NUM, LOCK_POLICY, GASHA_ stackAllocatorAutoClear>; \
	template class GASHA_ stackAllocator_withBuff<sizeof(T)* _NUM, LOCK_POLICY, GASHA_ stackAllocatorAutoClear>; \
	template class GASHA_ stackAllocator<LOCK_POLICY, stackAllocatorAutoClear>;
#endif

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

#endif//GASHA_INCLUDED_STACK_ALLOCATOR_CPP_H

// End of file

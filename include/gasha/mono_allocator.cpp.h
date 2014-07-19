#pragma once
#ifndef GASHA_INCLUDED_MONO_ALLOCATOR_CPP_H
#define GASHA_INCLUDED_MONO_ALLOCATOR_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// mono_allocator.cpp.h
// 単一アロケータ【関数定義部】
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

#include <gasha/mono_allocator.inl>//単一アロケータ【インライン関数／テンプレート関数定義部】

#include <gasha/type_traits.inl>//型特性ユーティリティ：toStr()
#include <gasha/string.h>//文字列処理：spprintf()

#include <cassert>//assert()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//単一アロケータクラス

//メモリ確保
template<class LOCK_TYPE>
void* monoAllocator<LOCK_TYPE>::alloc(const std::size_t size, const std::size_t align)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (m_size > 0)//既にメモリ確保中なら失敗
		return nullptr;
	//サイズが0バイトならサイズを1に、アラインメントを0にする
	//※要求サイズが0でも必ずメモリを割り当てる点に注意（ただし、アラインメントは守らない）
	const std::size_t _size = size == 0 ? 1 : size;
	const std::size_t _align = size == 0 ? 0 : align;
	//サイズとアラインメントをチェック
	char* new_ptr = adjustAlign(m_buffRef, _align);
	const std::size_t padding_size = new_ptr - m_buffRef;
	const size_type alloc_size = static_cast<size_type>(padding_size + _size);
	if (alloc_size > m_maxSize)
	{
	#ifdef GASHA_MONO_ALLOCATOR_ENABLE_ASSERTION
		static const bool NOT_ENOUGH_SPACE = false;
		assert(NOT_ENOUGH_SPACE);
	#endif//GASHA_MONO_ALLOCATOR_ENABLE_ASSERTION
		return nullptr;
	}
	
	//使用中のサイズを更新
	m_size = alloc_size;

	//空き領域を確保
	return reinterpret_cast<void*>(new_ptr);
}

//デバッグ情報作成
template<class LOCK_TYPE>
std::size_t monoAllocator<LOCK_TYPE>::debugInfo(char* message, const std::size_t max_size) const
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	std::size_t message_len = 0;
	GASHA_ spprintf(message, max_size, message_len, "----- Debug-info for monoAllocator -----\n");
	GASHA_ spprintf(message, max_size, message_len, "buff=%p, maxSize=%d, size=%d, remain=%d, isAllocated=%s\n", m_buffRef, maxSize(), this->size(), remain(), toStr(isAllocated()));
	GASHA_ spprintf(message, max_size, message_len, "----------------------------------------");//最終行改行なし
	return message_len;
}

//メモリ解放（共通処理）
template<class LOCK_TYPE>
bool monoAllocator<LOCK_TYPE>::_free(void* p)
{
	//バッファの使用中サイズを更新
	m_size = 0;
	return true;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//単一アロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_monoAllocator() \
	template class GASHA_ monoAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_monoAllocator_withLock(LOCK_TYPE) \
	template class GASHA_ monoAllocator<LOCK_TYPE>;

#if 0//不要
//バッファ付き単一アロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_monoAllocator_withBuff(_MAX_SIZE) \
	template class GASHA_ monoAllocator_withBuff<_MAX_SIZE>; \
	template class GASHA_ monoAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_monoAllocator_withBuff_withLock(_MAX_SIZE, LOCK_TYPE) \
	template class GASHA_ monoAllocator_withBuff<_MAX_SIZE, LOCK_TYPE>; \
	template class GASHA_ monoAllocator<LOCK_TYPE>;

//型指定バッファ付き単一アロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_monoAllocator_withType(T, _NUM) \
	template class GASHA_ monoAllocator_withType<T, _NUM>; \
	template class GASHA_ monoAllocator_withBuff<sizeof(T) * _NUM>; \
	template class GASHA_ monoAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_monoAllocator_withType_withLock(T, _NUM, LOCK_TYPE) \
	template class GASHA_ monoAllocator_withType<T, _NUM, LOCK_TYPE>; \
	template class GASHA_ monoAllocator_withBuff<sizeof(T)* _NUM, LOCK_TYPE>; \
	template class GASHA_ monoAllocator<LOCK_TYPE>;
#endif

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
// ※このコメントは、「明示的なインスタンス化マクロ」が定義されている全てのソースコードに
// 　同じ内容のものをコピーしています。

#endif//GASHA_INCLUDED_MONO_ALLOCATOR_CPP_H

// End of file

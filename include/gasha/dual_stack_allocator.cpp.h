﻿#pragma once
#ifndef GASHA_INCLUDED_DUAL_STACK_ALLOCATOR_CPP_H
#define GASHA_INCLUDED_DUAL_STACK_ALLOCATOR_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// dual_stack_allocator.cpp.h
// 双方向スタックアロケータ【関数定義部】
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

#include <gasha/dual_stack_allocator.inl>//双方向スタックアロケータ【インライン関数／テンプレート関数定義部】

#include <assert.h>//assert()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

//【VC++】sprintf を使用すると、error C4996 が発生する
//  error C4996: 'sprintf': This function or variable may be unsafe. Consider using strncpy_fast_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//双方向スタックアロケータクラス

//デバッグ情報作成
template<class LOCK_TYPE, class AUTO_CLEAR>
std::size_t dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::debugInfo(char* message)
{
#ifdef GASHA_HAS_DEBUG_FEATURE
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	std::size_t size = 0;
	size += sprintf(message + size, "----- Debug Info for dualStackAllocator -----\n");
	size += sprintf(message + size, "buffRef=%p, maxSize=%d, size=%d, sizeAsc=%d, sizeDesc=%d, remain=%d, allocatedCountAsc=%d, allocatedCountDesc=%d, allocatedCount=%d\n", m_buffRef, maxSize(), this->size(), sizeAsc(), sizeDesc(), remain(), allocatedCountAsc(), allocatedCountDesc(), allocatedCount());
	size += sprintf(message + size, "----------\n");
	return size;
#else//GASHA_HAS_DEBUG_FEATURE
	message[0] = '\0';
	return 0;
#endif//GASHA_HAS_DEBUG_FEATURE
}

//正順メモリ確保
template<class LOCK_TYPE, class AUTO_CLEAR>
void* dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::_allocAsc(const std::size_t size, const std::size_t align)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	//サイズが0バイトならサイズを1に、アラインメントを0にする
	//※要求サイズが0でも必ずメモリを割り当てる点に注意（ただし、アラインメントは守らない）
	const std::size_t _size = size == 0 ? 1 : size;
	const std::size_t _align = size == 0 ? 0 : align;
	//サイズとアラインメントをチェック
	size_type now_size = m_sizeAsc;
	char* now_ptr = m_buffRef + now_size;
	char* new_ptr = adjustAlign(now_ptr, _align);
	const std::size_t padding_size = new_ptr - now_ptr;
	const size_type alloc_size = static_cast<size_type>(padding_size + _size);
	const size_type new_size = now_size + alloc_size;
	const size_type new_size_all = new_size + m_sizeDesc;
	if (new_size_all > m_maxSize)
	{
	#ifdef GASHA_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
		static const bool NOT_ENOUGH_SPACE = false;
		assert(NOT_ENOUGH_SPACE);
	#endif//GASHA_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
		return nullptr;
	}
	
	//使用中のサイズとメモリ確保数を更新
	m_sizeAsc = new_size;
	m_size += alloc_size;
	++m_allocatedCountAsc;

	//空き領域を確保
	return reinterpret_cast<void*>(new_ptr);
}

//逆順メモリ確保
template<class LOCK_TYPE, class AUTO_CLEAR>
void* dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::_allocDesc(const std::size_t size, const std::size_t align)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	//サイズが0バイトならサイズを1に、アラインメントを0にする
	//※要求サイズが0でも必ずメモリを割り当てる点に注意（ただし、アラインメントは守らない）
	const std::size_t _size = size == 0 ? 1 : size;
	const std::size_t _align = size == 0 ? 0 : align;
	//サイズとアラインメントをチェック
	size_type now_size = m_sizeDesc;
	char* now_ptr = m_buffRef + m_maxSize - now_size;
	char* new_ptr = adjustAlign(now_ptr - _size - _align + (_align == 0 ? 0 : 1), _align);
	const size_type alloc_size = static_cast<size_type>(now_ptr - new_ptr);
	const size_type new_size = now_size + alloc_size;
	const size_type new_size_all = new_size + m_sizeAsc;
	if (new_size_all > m_maxSize)
	{
	#ifdef GASHA_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
		static const bool NOT_ENOUGH_SPACE = false;
		assert(NOT_ENOUGH_SPACE);
	#endif//GASHA_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
		return nullptr;
	}
	
	//使用中のサイズとメモリ確保数を更新
	m_sizeDesc = new_size;
	m_size += alloc_size;
	++m_allocatedCountDesc;

	//空き領域を確保
	return reinterpret_cast<void*>(new_ptr);
}

//正順メモリ解放（共通処理）
template<class LOCK_TYPE, class AUTO_CLEAR>
bool dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::_freeAsc(void* p)
{
	//メモリ確保数を更新
	--m_allocatedCountAsc;
	//自動クリア呼び出し
	AUTO_CLEAR auto_clear;
	auto_clear.autoClearAsc(*this);
	return true;
}

//逆順メモリ解放（共通処理）
template<class LOCK_TYPE, class AUTO_CLEAR>
bool dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::_freeDesc(void* p)
{
	//メモリ確保数を更新
	--m_allocatedCountDesc;
	//自動クリア呼び出し
	AUTO_CLEAR auto_clear;
	auto_clear.autoClearDesc(*this);
	return true;
}

//正順で使用中のサイズを指定位置に戻す
//※ポインタ指定版
template<class LOCK_TYPE, class AUTO_CLEAR>
bool dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::_rewindAsc(void* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	const size_type new_size = static_cast<size_type>(reinterpret_cast<char*>(p) - m_buffRef);
	const size_type rewind_size = m_sizeAsc - new_size;
	m_sizeAsc = new_size;
	m_size -= rewind_size;
	return true;
}

//逆順で使用中のサイズを指定位置に戻す
//※ポインタ指定版
template<class LOCK_TYPE, class AUTO_CLEAR>
bool dualStackAllocator<LOCK_TYPE, AUTO_CLEAR>::_rewindDesc(void* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (!isInUsingRange(p))//正しいポインタか判定
		return false;
	const size_type new_size = static_cast<size_type>(m_buffRef + m_maxSize - reinterpret_cast<char*>(p));
	const size_type rewind_size = m_sizeDesc - new_size;
	m_sizeDesc = new_size;
	m_size -= new_size;
	return true;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_dualStackAllocator() \
	template class dualStackAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_dualStackAllocator_withLock(LOCK_TYPE) \
	template class dualStackAllocator<LOCK_TYPE>;

//スマート双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_smartDualStackAllocator() \
	template class dualStackAllocator<GASHA_ dummyLock, dualStackAllocatorAutoClear>;
//※ロック指定版
#define GASHA_INSTANCING_smartDualStackAllocator_withLock(LOCK_TYPE) \
	template class dualStackAllocator<LOCK_TYPE, dualStackAllocatorAutoClear>;

#if 0//不要
//バッファ付き双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_dualStackAllocator_withBuff(_MAX_SIZE) \
	template class dualStackAllocator_withBuff<_MAX_SIZE>; \
	template class dualStackAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_dualStackAllocator_withBuff_withLock(_MAX_SIZE, LOCK_TYPE) \
	template class dualStackAllocator_withBuff<_MAX_SIZE, LOCK_TYPE>; \
	template class dualStackAllocator<LOCK_TYPE>;

//バッファ付きスマート双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_smartDualStackAllocator_withBuff(_MAX_SIZE) \
	template class dualStackAllocator_withBuff<_MAX_SIZE, GASHA_ dummyLock, dualStackAllocatorAutoClear>; \
	template class dualStackAllocator<GASHA_ dummyLock, dualStackAllocatorAutoClear>;
//※ロック指定版
#define GASHA_INSTANCING_smartDualStackAllocator_withBuff_withLock(_MAX_SIZE, LOCK_TYPE) \
	template class dualStackAllocator_withBuff<_MAX_SIZE, LOCK_TYPE, dualStackAllocatorAutoClear>; \
	template class dualStackAllocator<LOCK_TYPE, dualStackAllocatorAutoClear>;

//型指定バッファ付き双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_dualStackAllocator_withType(T, _NUM) \
	template class dualStackAllocator_withType<T, _NUM>; \
	template class dualStackAllocator_withBuff<sizeof(T) * _NUM>; \
	template class dualStackAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_dualStackAllocator_withType_withLock(T, _NUM, LOCK_TYPE) \
	template class dualStackAllocator_withType<T, _NUM, LOCK_TYPE>; \
	template class dualStackAllocator_withBuff<sizeof(T)* _NUM, LOCK_TYPE>; \
	template class dualStackAllocator<LOCK_TYPE>;

//型指定バッファ付きスマート双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_smartDualStackAllocator_withType(T, _NUM) \
	template class dualStackAllocator_withType<T, _NUM, GASHA_ dummyLock, dualStackAllocatorAutoClear>; \
	template class dualStackAllocator_withBuff<sizeof(T)* _NUM, GASHA_ dummyLock, dualStackAllocatorAutoClear>; \
	template class dualStackAllocator<GASHA_ dummyLock, dualStackAllocatorAutoClear>;
//※ロック指定版
#define GASHA_INSTANCING_smartDualStackAllocator_withType_withLock(T, _NUM, LOCK_TYPE) \
	template class dualStackAllocator_withType<T, _NUM, LOCK_TYPE, dualStackAllocatorAutoClear>; \
	template class dualStackAllocator_withBuff<sizeof(T)* _NUM, LOCK_TYPE, dualStackAllocatorAutoClear>; \
	template class dualStackAllocator<LOCK_TYPE, dualStackAllocatorAutoClear>;
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

#endif//GASHA_INCLUDED_DUAL_STACK_ALLOCATOR_CPP_H

// End of file
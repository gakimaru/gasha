#pragma once
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
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/stack_allocator.inl>//スタックアロケータ【インライン関数／テンプレート関数定義部】

#include <utility>//C++11 std::move

#include <assert.h>//assert()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//スタックアロケータクラス

//メモリ確保
template<class LOCK_TYPE, class AUTO_CLEAR>
void* stackAllocator<LOCK_TYPE, AUTO_CLEAR>::alloc(const std::size_t size, const std::size_t align)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	//サイズとアラインメントをチェック
	void* new_ptr = reinterpret_cast<void*>(adjustAlign(m_buffRef + m_size, align));
	const size_type new_size = static_cast<size_type>((reinterpret_cast<char*>(new_ptr) - m_buffRef) + size);
	if (new_size > remain())
	{
	#ifdef GASHA_STACK_ALLOCATOR_ENABLE_ASSERTION
		static const bool NOT_ENOUGH_SPACE = false;
		assert(NOT_ENOUGH_SPACE);
	#endif//GASHA_STACK_ALLOCATOR_ENABLE_ASSERTION
		return nullptr;
	}
	
	//使用中のサイズとメモリ確保数を更新
	m_size += new_size;
	++m_allocatedCount;

	//空き領域を確保
	return new_ptr;
}

//メモリ解放（共通処理）
//※ロック取得は呼び出し元で行う
template<class LOCK_TYPE, class AUTO_CLEAR>
bool stackAllocator<LOCK_TYPE, AUTO_CLEAR>::_free(void* p)
{
	//メモリ確保数を更新
	--m_allocatedCount;
	//自動クリア呼び出し
	AUTO_CLEAR auto_clear;
	auto_clear.autoClear(*this);
	return true;
}

//メモリを以前の位置に戻す
//※ポインタ指定版
template<class LOCK_TYPE, class AUTO_CLEAR>
bool  stackAllocator<LOCK_TYPE, AUTO_CLEAR>::back(void* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	if (!inUsingRange(p))//正しいポインタか判定
		return false;
	m_size = static_cast<size_type>(reinterpret_cast<char*>(p) - m_buffRef);
	return true;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//明示的なインスタンス化用マクロ

//基本形
#define GASHA_INSTANCING_stackAllocator() \
	template class stackAllocator<>;
#define GASHA_INSTANCING_smartStackAllocator() \
	template class stackAllocator<GASHA_ dummyLock, stackAllocatorAutoClear>;

//基本形＋ロック指定
#define GASHA_INSTANCING_stackAllocator_withLock(LOCK_TYPE) \
	template class stackAllocator<LOCK_TYPE>;
#define GASHA_INSTANCING_smartStackAllocator_withLock(LOCK_TYPE) \
	template class stackAllocator<LOCK_TYPE, stackAllocatorAutoClear>;

//バッファ付き
#define GASHA_INSTANCING_stackAllocator_withBuff(_MAX_SIZE) \
	template class stackAllocator_withBuff<_MAX_SIZE>; \
	template class stackAllocator<>;
#define GASHA_INSTANCING_smartStackAllocator_withBuff(_MAX_SIZE) \
	template class stackAllocator_withBuff<_MAX_SIZE, GASHA_ dummyLock, stackAllocatorAutoClear>; \
	template class stackAllocator<GASHA_ dummyLock, stackAllocatorAutoClear>;

//バッファ付き＋ロック指定
#define GASHA_INSTANCING_stackAllocator_withBuff_withLock(_MAX_SIZE, LOCK_TYPE) \
	template class stackAllocator_withBuff<_MAX_SIZE, LOCK_TYPE>; \
	template class stackAllocator<LOCK_TYPE>;
#define GASHA_INSTANCING_smartStackAllocator_withBuff_withLock(_MAX_SIZE, LOCK_TYPE) \
	template class stackAllocator_withBuff<_MAX_SIZE, LOCK_TYPE, stackAllocatorAutoClear>; \
	template class stackAllocator<LOCK_TYPE, stackAllocatorAutoClear>;

//型指定バッファ付き
#define GASHA_INSTANCING_stackAllocator_withType(T, _NUM) \
	template class stackAllocator_withType<T, _NUM>; \
	template class stackAllocator_withBuff<sizeof(T) * _NUM>; \
	template class stackAllocator<>;
#define GASHA_INSTANCING_smartStackAllocator_withType(T, _NUM) \
	template class stackAllocator_withType<T, _NUM, GASHA_ dummyLock, stackAllocatorAutoClear>; \
	template class stackAllocator_withBuff<sizeof(T)* _NUM, GASHA_ dummyLock, stackAllocatorAutoClear>; \
	template class stackAllocator<GASHA_ dummyLock, stackAllocatorAutoClear>;

//型指定バッファ付き＋ロック指定
#define GASHA_INSTANCING_stackAllocator_withType_withLock(T, _NUM, LOCK_TYPE) \
	template class stackAllocator_withType<T, _NUM, LOCK_TYPE>; \
	template class stackAllocator_withBuff<sizeof(T) * _NUM, LOCK_TYPE>; \
	template class stackAllocator<LOCK_TYPE>;
#define GASHA_INSTANCING_smartStackAllocator_withType_withLock(T, _NUM, LOCK_TYPE) \
	template class stackAllocator_withType<T, _NUM, LOCK_TYPE, stackAllocatorAutoClear>; \
	template class stackAllocator_withBuff<sizeof(T)* _NUM, LOCK_TYPE, stackAllocatorAutoClear>; \
	template class stackAllocator<LOCK_TYPE, stackAllocatorAutoClear>;

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_STACK_ALLOCATOR_CPP_H

// End of file

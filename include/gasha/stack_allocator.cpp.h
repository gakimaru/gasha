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

#if 0

	//メモリ確保
	void* alloc(const std::size_t size, const std::size_t align = GASHA_ DEFAULT_ALIGN)
	{
		const uintptr_t now_ptr = reinterpret_cast<uintptr_t>(m_buffPtr) + m_size;//現在のポインタ位置算出
		const uintptr_t align_diff = align > 0 ? now_ptr % align == 0 ? 0 : align - now_ptr % align : 0;//アラインメント計算
		const marker_t next_used = m_size + align_diff + size;//次のマーカー算出
		if (next_used > m_maxSize)//メモリオーバーチェック（符号なしなので、範囲チェックは大判定のみでOK）
		{
			//printf("stack overflow!(size=%d+align=%d, remain=%d)\n", size, align_diff, m_maxSize - m_size);
			return nullptr;
		}
		const uintptr_t alloc_ptr = now_ptr + align_diff;//メモリ確保アドレス算出
		m_size = next_used;//マーカー更新
		return reinterpret_cast<void*>(alloc_ptr);
	}

	//メモリ解放
	bool free(void* p);
	
	//メモリを以前のマーカーに戻す
	//※マーカー指定版
	void back(const marker_t marker)
	{
		if (marker >= m_size)//符号なしなので、範囲チェックは大判定のみでOK
			return;
		m_size = marker;
	}
	//メモリを以前のマーカーに戻す
	//※ポインタ指定版
	void back(const void* p)
	{
		const marker_t marker = reinterpret_cast<uintptr_t>(p)-reinterpret_cast<uintptr_t>(m_buffPtr);
		back(marker);
	}
	//メモリ破棄
	void clear()
	{
		m_size = 0;//マーカーをリセット
	}
#endif

//メモリ確保
template<std::size_t _MAX_STACK_SIZE, class LOCK_TYPE>
void* stackAllocator<_MAX_STACK_SIZE, LOCK_TYPE>::alloc()
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	//空きスタックを確保
	if (m_vacantHead < m_stackSize)//空きスタックの先頭インデックスがスタックサイズ未満なら空きスタックを利用する
	{
		const std::size_t vacant_index = m_vacantHead++;//空きスタックの先頭インでックスを取得＆インクリメント
		m_using[vacant_index] = true;//インデックスを使用中にする
		//++m_usingPoolSize;//使用中の数を増やす（デバッグ用）
		return refBuff(vacant_index);//メモリ確保成功
	}
	//再利用スタックの先頭インデックスが無効ならメモリ確保失敗（再利用スタックが無い）
	if (m_recyclableHead == INVALID_INDEX)
		return nullptr;//メモリ確保失敗
	//再利用スタックを確保
	{
		const std::size_t recyclable_index = m_recyclableHead;//再利用スタックの先頭インデックスを取得
		recycable_t* recyclable_stack = static_cast<recycable_t*>(refBuff(recyclable_index));//再利用スタックの先頭を割り当て
		m_recyclableHead = recyclable_stack->m_next_index;//再利用スタックの先頭インデックスを次の再利用スタックに変更
		recyclable_stack->m_next_index = DIRTY_INDEX;//再利用スタックの連結インデックスを削除
		m_using[recyclable_index] = true;//インデックスを使用中にする
		//++m_usingPoolSize;//使用中の数を増やす（デバッグ用）
		return recyclable_stack;//メモリ確保成功
	}
}

//メモリ解放（共通処理）
//※ロック取得は呼び出し元で行う
template<std::size_t _MAX_STACK_SIZE, class LOCK_TYPE>
bool stackAllocator<_MAX_STACK_SIZE, LOCK_TYPE>::free(void* p, const std::size_t index)
{
	recycable_t* deleted_stack = static_cast<recycable_t*>(refBuff(index));//解放されたメモリを参照
	deleted_stack->m_next_index = m_recyclableHead;//次の再利用スタックのインデックスを保存
	m_recyclableHead = index;//再利用スタックの先頭インデックスを変更
	m_using[index] = false;//インデックスを未使用状態にする
	//--m_usingPoolSize;//使用中の数を減らす（デバッグ用）
	return true;
}

//メモリ解放
template<std::size_t _MAX_STACK_SIZE, class LOCK_TYPE>
bool stackAllocator<_MAX_STACK_SIZE, LOCK_TYPE>::free(void* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	const std::size_t index = ptrToIndex(p);//ポインタをインデックスに変換
	if (index == INVALID_INDEX)
		return false;
	return free(p, index);
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//明示的なインスタンス化用マクロ

//基本形
#define GASHA_INSTANCING_stackAllocator(_MAX_STACK_SIZE) \
	template class stackAllocator<_MAX_STACK_SIZE>;

//基本形＋ロック指定
#define GASHA_INSTANCING_stackAllocator_withLock(_MAX_STACK_SIZE, LOCK_TYPE) \
	template class stackAllocator<_MAX_STACK_SIZE, LOCK_TYPE>;

//バッファ付き
#define GASHA_INSTANCING_stackAllocator_withBuff(_BLOCK_SIZE, _STACK_SIZE, _BLOCK_ALIGN) \
	template class stackAllocator_withBuff<_BLOCK_SIZE, _STACK_SIZE, _BLOCK_ALIGN>; \
	template class stackAllocator<_STACK_SIZE>;

//バッファ付き＋ロック指定
#define GASHA_INSTANCING_stackAllocator_withBuff_withLock(_BLOCK_SIZE, _STACK_SIZE, _BLOCK_ALIGN, LOCK_TYPE) \
	template class stackAllocator_withBuff<_BLOCK_SIZE, _STACK_SIZE, _BLOCK_ALIGN, LOCK_TYPE>; \
	template class stackAllocator<_STACK_SIZE, LOCK_TYPE>;

//型指定バッファ付き
#define GASHA_INSTANCING_stackAllocator_withType(T, _STACK_SIZE) \
	template class stackAllocator_withType<T, _STACK_SIZE>; \
	template class stackAllocator_withBuff<sizeof(T), _STACK_SIZE, alignof(T)>; \
	template class stackAllocator<_STACK_SIZE>;

//型指定バッファ付き＋ロック指定
#define GASHA_INSTANCING_stackAllocator_withType_withLock(T, _STACK_SIZE, LOCK_TYPE) \
	template class stackAllocator_withType<T, _STACK_SIZE, LOCK_TYPE>; \
	template class stackAllocator_withBuff<sizeof(T), _STACK_SIZE, alignof(T), LOCK_TYPE>; \
	template class stackAllocator<_STACK_SIZE, LOCK_TYPE>;

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_STACK_ALLOCATOR_CPP_H

// End of file

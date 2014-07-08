#pragma once
#ifndef GASHA_INCLUDED_POOL_ALLOCATOR_CPP_H
#define GASHA_INCLUDED_POOL_ALLOCATOR_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// pool_allocator.cpp.h
// プールアロケータ【関数定義部】
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

#include <gasha/pool_allocator.inl>//プールアロケータ【インライン関数／テンプレート関数定義部】

#include <gasha/type_traits.h>//型特性ユーティリティ

#include <utility>//C++11 std::move
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
//プールアロケータクラス

//メモリ確保
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
void* poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::alloc(const std::size_t size, const std::size_t align)
{
	//サイズとアラインメントをチェック
	const std::size_t _align = align == m_blockAlign ? 0 : align;
	if (adjustAlign(m_blockAlign, _align) - m_blockAlign + size > m_blockSize)
	{
	#ifdef GASHA_POOL_ALLOCATOR_ENABLE_ASSERTION
		static const bool IS_INVALID_SIZE_OR_ALIGNMENT = false;
		assert(IS_INVALID_SIZE_OR_ALIGNMENT);
	#endif//GASHA_POOL_ALLOCATOR_ENABLE_ASSERTION
		return nullptr;
	}

	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	//空きプールを確保
	if (m_vacantHead < m_poolSize)//空きプールの先頭インデックスがプールサイズ未満なら空きプールを利用する
	{
		const index_type vacant_index = m_vacantHead++;//空きプールの先頭インでックスを取得＆インクリメント
		m_using[vacant_index] = true;//インデックスを使用中にする
		++m_usingPoolSize;//使用中の数を増やす（デバッグ用）
		void* ptr = refBuff(vacant_index);//メモリ確保成功
		return adjustAlign(ptr, _align);//アラインメント調整して返す
	}
	//再利用プールの先頭インデックスが無効ならメモリ確保失敗（再利用プールが無い）
	if (m_recyclableHead == INVALID_INDEX)
		return nullptr;//メモリ確保失敗
	//再利用プールを確保
	{
		const index_type recyclable_index = m_recyclableHead;//再利用プールの先頭インデックスを取得
		recycable_t* recyclable_pool = static_cast<recycable_t*>(refBuff(recyclable_index));//再利用プールの先頭を割り当て
		m_recyclableHead = recyclable_pool->m_next_index;//再利用プールの先頭インデックスを次の再利用プールに変更
		recyclable_pool->m_next_index = DIRTY_INDEX;//再利用プールの連結インデックスを削除
		m_using[recyclable_index] = true;//インデックスを使用中にする
		++m_usingPoolSize;//使用中の数を増やす（デバッグ用）
		void* ptr = reinterpret_cast<void*>(recyclable_pool);//メモリ確保成功
		return adjustAlign(ptr, _align);//アラインメント調整して返す
	}
}

//メモリ解放（共通処理）
//※ロック取得は呼び出し元で行う
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
bool poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::free(void* p, const typename poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::index_type index)
{
	recycable_t* deleted_pool = static_cast<recycable_t*>(refBuff(index));//解放されたメモリを参照
	deleted_pool->m_next_index = m_recyclableHead;//次の再利用プールのインデックスを保存
	m_recyclableHead = index;//再利用プールの先頭インデックスを変更
	m_using[index] = false;//インデックスを未使用状態にする
	--m_usingPoolSize;//使用中の数を減らす（デバッグ用）
	return true;
}

//メモリ解放
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
bool poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::free(void* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	const index_type index = ptrToIndex(p);//ポインタをインデックスに変換
	if (index == INVALID_INDEX)
		return false;
	return free(p, index);
}

//デバッグ情報作成
template<std::size_t _MAX_POOL_SIZE, class LOCK_TYPE>
std::size_t poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::debugInfo(char* message)
{
	auto print_node = [](char* message, std::uint32_t& data) -> std::size_t
	{
		unionTypes uni(data);
		return sprintf(message, "image=[0x%02x,0x%02x,0x%02x,0x%02x]", uni.m_uchar[0], uni.m_uchar[1], uni.m_uchar[2], uni.m_uchar[3]);
	};
	return this->template debugInfo<std::uint32_t>(message, print_node);
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//明示的なインスタンス化用マクロ

//基本形
#define GASHA_INSTANCING_poolAllocator(_MAX_POOL_SIZE) \
	template class poolAllocator<_MAX_POOL_SIZE>;

//基本形＋ロック指定
#define GASHA_INSTANCING_poolAllocator_withLock(_MAX_POOL_SIZE, LOCK_TYPE) \
	template class poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>;

//バッファ付き
#define GASHA_INSTANCING_poolAllocator_withBuff(_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN) \
	template class poolAllocator_withBuff<_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN>; \
	template class poolAllocator<_POOL_SIZE>;

//バッファ付き＋ロック指定
#define GASHA_INSTANCING_poolAllocator_withBuff_withLock(_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN, LOCK_TYPE) \
	template class poolAllocator_withBuff<_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN, LOCK_TYPE>; \
	template class poolAllocator<_POOL_SIZE, LOCK_TYPE>;

//型指定バッファ付き
#define GASHA_INSTANCING_poolAllocator_withType(T, _POOL_SIZE) \
	template class poolAllocator_withType<T, _POOL_SIZE>; \
	template class poolAllocator_withBuff<sizeof(T), _POOL_SIZE, alignof(T)>; \
	template class poolAllocator<_POOL_SIZE>;

//型指定バッファ付き＋ロック指定
#define GASHA_INSTANCING_poolAllocator_withType_withLock(T, _POOL_SIZE, LOCK_TYPE) \
	template class poolAllocator_withType<T, _POOL_SIZE, LOCK_TYPE>; \
	template class poolAllocator_withBuff<sizeof(T), _POOL_SIZE, alignof(T), LOCK_TYPE>; \
	template class poolAllocator<_POOL_SIZE, LOCK_TYPE>;

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_POOL_ALLOCATOR_CPP_H

// End of file

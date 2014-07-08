#pragma once
#ifndef GASHA_INCLUDED_LOCKFREE_POOL_ALLOCATOR_CPP_H
#define GASHA_INCLUDED_LOCKFREE_POOL_ALLOCATOR_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_pool_allocator.cpp.h
// ロックフリープールアロケータ【関数／実体定義部】
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

#include <gasha/lf_pool_allocator.inl>//ロックフリープールアロケータ【インライン関数／テンプレート関数定義部】

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
//ロックフリープールアロケータクラス

//メモリ確保
template<std::size_t _MAX_POOL_SIZE>
void* lfPoolAllocator<_MAX_POOL_SIZE>::alloc(const std::size_t size, std::size_t align)
{
	//サイズとアラインメントをチェック
	const std::size_t _align = align == m_blockAlign ? 0 : align;
	if (adjustAlign(m_blockAlign, _align) - m_blockAlign + size > m_blockSize)
	{
	#ifdef GASHA_LF_POOL_ALLOCATOR_ENABLE_ASSERTION
		static const bool IS_INVALID_SIZE_OR_ALIGNMENT = false;
		assert(IS_INVALID_SIZE_OR_ALIGNMENT);
	#endif//GASHA_LF_POOL_ALLOCATOR_ENABLE_ASSERTION
		return nullptr;
	}

	//空きプールを確保
	if (m_vacantHead.load() < m_poolSize)//空きプールの先頭インデックスがプールサイズ未満なら空きプールを利用
	{
		const index_type vacant_index = m_vacantHead.fetch_add(1);//空きプールの先頭インデックスを取得してインクリメント
		if (vacant_index < m_poolSize)//プールサイズ未満なら確保成功
		{
			m_using[vacant_index].fetch_add(1);//インデックスを使用中状態にする
			//m_usingPoolSize.fetch_add(1);//使用中の数を増やす（デバッグ用）
			//m_allocCount[vacant_index].fetch_add(1);//アロケート回数をカウントアップ（デバッグ用）
			void* ptr = refBuff(vacant_index);//メモリ確保成功
			return adjustAlign(ptr, _align);//アラインメント調整して返す
		}
		if (vacant_index > m_poolSize)//インクリメントでオーバーしたインデックスを元に戻す
			m_vacantHead.store(static_cast<index_type>(m_poolSize));
	}
	//再利用プールを確保
	{
		index_type recycable_index_and_tag = m_recyclableHead.load();//再利用プールの先頭インデックスを取得
		while (true)
		{
			if (recycable_index_and_tag == INVALID_INDEX)//再利用プールの先頭インデックスが無効ならメモリ確保失敗（再利用プールが無い）
				return nullptr;//メモリ確保失敗
			const index_type recyclable_index = recycable_index_and_tag & 0x00ffffff;//タグ削除
			if (recyclable_index >= m_poolSize)//再利用プールの先頭インデックス範囲外ならリトライ
			{
				recycable_index_and_tag = m_recyclableHead.load();//再利用プールの先頭インデックスを再取得
				continue;//リトライ
			}
			recycable_t* recyclable_pool = reinterpret_cast<recycable_t*>(refBuff(recyclable_index));//再利用プールの先頭を割り当て
			const index_type next_index_and_tag = recyclable_pool->m_next_index.load();//次の再利用プールのインデックスを取得

			//CAS操作①
			if (m_recyclableHead.compare_exchange_weak(recycable_index_and_tag, next_index_and_tag))//CAS操作
			//【CAS操作の内容】
			//    if(m_recyclableHead == recyclable_index)//再利用プールの先頭インデックスを他のスレッドが書き換えていないか？
			//        m_recyclableHead = next;//再利用プールの先頭インデックスを次の再利用インデックスに変更（メモリ確保成功）
			//    else
			//        recycable_index_and_tag = m_recyclableHead;//再利用プールの先頭インデックスを再取得
			{
				recyclable_pool->m_next_index.store(DIRTY_INDEX);//再利用プールの連結インデックスを削除
				m_using[recyclable_index].fetch_add(1);//インデックスを使用中状態にする
				//m_usingPoolSize.fetch_add(1);//使用中の数を増やす（デバッグ用）
				//m_allocCount[recyclable_index].fetch_add(1);//アロケート回数をカウントアップ（デバッグ用）
				void* ptr = reinterpret_cast<void*>(recyclable_pool);//メモリ確保成功
				return adjustAlign(ptr, _align);//アラインメント調整して返す
			}
		}
		return nullptr;//ダミー
	}
}

//メモリ解放（共通処理）
template<std::size_t _MAX_POOL_SIZE>
bool lfPoolAllocator<_MAX_POOL_SIZE>::free(void* p, const typename lfPoolAllocator<_MAX_POOL_SIZE>::index_type index)
{
	const index_type tag = static_cast<index_type>(m_tag.fetch_add(1));//タグ取得
	const index_type index_and_tag = index | (tag << 24);//タグ付きインデックス作成
	index_type recycable_index_and_tag = m_recyclableHead.load();//再利用プールの先頭インデックスを取得
	while (true)
	{
		recycable_t* deleted_pool = reinterpret_cast<recycable_t*>(refBuff(index));//解放されたメモリを参照
		deleted_pool->m_next_index.store(recycable_index_and_tag);//次の再利用プールのインデックスを保存
		
		//CAS操作②
		if (m_recyclableHead.compare_exchange_weak(recycable_index_and_tag, index_and_tag))//CAS操作
		//【CAS操作の内容】
		//    if(m_recyclableHead == recycable_index_and_tag)//再利用プールの先頭インデックスを他のスレッドが書き換えていないか？
		//        m_recyclableHead = index_and_tag;//再利用プールの先頭インデックスを次のインデックスに変更（メモリ解放成功）
		//    else
		//        recycable_index_and_tag = m_recyclableHead;//再利用プールの先頭インデックスを再取得
		{
			m_using[index].fetch_sub(1);//インデックスを未使用状態にする
			//m_usingPoolSize.fetch_sub(1);//使用中の数を減らす（デバッグ用）
			//m_freeCount[index].fetch_add(1);//フリー回数をカウントアップ（デバッグ用）
			return true;//メモリ解放成功
		}
	}
	return false;//ダミー
}

//メモリ解放
template<std::size_t _MAX_POOL_SIZE>
bool lfPoolAllocator<_MAX_POOL_SIZE>::free(void* p)
{
	const index_type index = ptrToIndex(p);//ポインタをインデックスに変換
	if (index == INVALID_INDEX)
		return false;
	return free(p, index);
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//明示的なインスタンス化用マクロ

//基本形
#define GASHA_INSTANCING_lfPoolAllocator(_MAX_POOL_SIZE) \
	template class lfPoolAllocator<_MAX_POOL_SIZE>;

//バッファ付き
#define GASHA_INSTANCING_lfPoolAllocator_withBuff(_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN) \
	template class lfPoolAllocator_withBuff<_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN>; \
	template class lfPoolAllocator<_POOL_SIZE>;

//型指定バッファ付き
#define GASHA_INSTANCING_lfPoolAllocator_withType(T, _POOL_SIZE) \
	template class lfPoolAllocator_withType<T, _POOL_SIZE>; \
	template class lfPoolAllocator_withBuff<sizeof(T), _POOL_SIZE, alignof(T)>; \
	template class lfPoolAllocator<_POOL_SIZE>;


//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_LOCKFREE_POOL_ALLOCATOR_CPP_H

// End of file

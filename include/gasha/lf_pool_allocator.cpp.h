#pragma once
#ifndef __LOCKFREE_POOL_ALLOCATOR_CPP_H_
#define __LOCKFREE_POOL_ALLOCATOR_CPP_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_pool_allocator.h
// ロックフリープールアロケータ【関数定義部】
//
// ※クラスの実体化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_pool_allocator.inl>//ロックフリープールアロケータ【インライン関数／テンプレート関数定義部】

#include <utility>//C++11 std::move
#include <assert.h>//assert()
#include <stdio.h>//printf()

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
template<class T, std::size_t _POOL_SIZE>
void* lfPoolAllocator<T, _POOL_SIZE>::alloc()
{
	//空きプールを確保
	if (m_vacantHead.load() < POOL_SIZE)//空きプールの先頭インデックスがプールサイズ未満なら空きプールを利用
	{
		const index_type vacant_index = m_vacantHead.fetch_add(1);//空きプールの先頭インデックスを取得してインクリメント
		if (vacant_index < POOL_SIZE)//プールサイズ未満なら確保成功
		{
			m_using[vacant_index].fetch_add(1);//インデックスを使用中状態にする
			//m_usingCount.fetch_add(1);//使用中の数を増やす（デバッグ用）
			//m_allocCount[vacant_index].fetch_add(1);//アロケート回数をカウントアップ（デバッグ用）
			return m_pool[vacant_index];//メモリ確保成功
		}
		if (vacant_index > POOL_SIZE)//インクリメントでオーバーしたインデックスを元に戻す
			m_vacantHead.store(POOL_SIZE);
	}
	//再利用プールを確保
	{
		index_type recycable_index_and_tag = m_recyclableHead.load();//再利用プールの先頭インデックスを取得
		while (true)
		{
			if (recycable_index_and_tag == INVALID_INDEX)//再利用プールの先頭インデックスが無効ならメモリ確保失敗（再利用プールが無い）
				return nullptr;//メモリ確保失敗
			const index_type recyclable_index = recycable_index_and_tag & 0x00ffffff;//タグ削除
			if (recyclable_index >= POOL_SIZE)//再利用プールの先頭インデックス範囲外ならリトライ
			{
				recycable_index_and_tag = m_recyclableHead.load();//再利用プールの先頭インデックスを再取得
				continue;//リトライ
			}
			recycable_t* recyclable_pool = reinterpret_cast<recycable_t*>(m_pool[recyclable_index]);//再利用プールの先頭を割り当て
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
				//m_usingCount.fetch_add(1);//使用中の数を増やす（デバッグ用）
				//m_allocCount[recyclable_index].fetch_add(1);//アロケート回数をカウントアップ（デバッグ用）
				return recyclable_pool;//メモリ確保成功
			}
		}
		return nullptr;//ダミー
	}
}

//メモリ解放（共通処理）
template<class T, std::size_t _POOL_SIZE>
bool lfPoolAllocator<T, _POOL_SIZE>::free(void* p, const typename lfPoolAllocator<T, _POOL_SIZE>::index_type index)
{
	const index_type tag = static_cast<index_type>(m_tag.fetch_add(1));//タグ取得
	const index_type index_and_tag = index | (tag << 24);//タグ付きインデックス作成
	index_type recycable_index_and_tag = m_recyclableHead.load();//再利用プールの先頭インデックスを取得
	while (true)
	{
		recycable_t* deleted_pool = reinterpret_cast<recycable_t*>(m_pool[index]);//解放されたメモリを参照
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
			//m_usingCount.fetch_sub(1);//使用中の数を減らす（デバッグ用）
			//m_freeCount[index].fetch_add(1);//フリー回数をカウントアップ（デバッグ用）
			return true;//メモリ解放成功
		}
	}
	return false;//ダミー
}

//ポインタをインデックスに変換
template<class T, std::size_t _POOL_SIZE>
typename lfPoolAllocator<T, _POOL_SIZE>::index_type lfPoolAllocator<T, _POOL_SIZE>::ptrToIndex(void* p)
{
	const index_type index = static_cast<index_type>((reinterpret_cast<char*>(p) - reinterpret_cast<char*>(m_pool)) / VALUE_SIZE);
	if (index >= POOL_SIZE)//範囲外のインデックスなら終了
	{
	#ifdef GASHA_ASSERTION_ENABLED
		static const bool IS_INVALID_POINTER_OF_POOL = false;
		assert(IS_INVALID_POINTER_OF_POOL);
	#endif//GASHA_ASSERTION_ENABLED
		return INVALID_INDEX;
	}
	if (m_using[index].load() == 0)//インデックスが既に未使用状態なら終了
	{
	#ifdef GASHA_ASSERTION_ENABLED
		static const bool IS_ALREADY_DELETE_POINTER = false;
		assert(IS_ALREADY_DELETE_POINTER);
	#endif//GASHA_ASSERTION_ENABLED
		return INVALID_INDEX;
	}
	return index;
}

//メモリ解放
template<class T, std::size_t _POOL_SIZE>
bool lfPoolAllocator<T, _POOL_SIZE>::free(void* p)
{
	const index_type index = ptrToIndex(p);//ポインタをインデックスに変換
	if (index == INVALID_INDEX)
		return false;
	return free(p, index);
}

//メモリ確保とコンストラクタ呼び出し
template<class T, std::size_t _POOL_SIZE>
typename lfPoolAllocator<T, _POOL_SIZE>::value_type* lfPoolAllocator<T, _POOL_SIZE>::newMoveObj(typename lfPoolAllocator<T, _POOL_SIZE>::value_type&& org)//※ムーブ版
{
	void* p = alloc();
	if (!p)
		return nullptr;
	return new(p)value_type(std::move(org));
}
template<class T, std::size_t _POOL_SIZE>
typename lfPoolAllocator<T, _POOL_SIZE>::value_type* lfPoolAllocator<T, _POOL_SIZE>::newCopyObj(typename lfPoolAllocator<T, _POOL_SIZE>::value_type& org)//※コピー版
{
	void* p = alloc();
	if (!p)
		return nullptr;
	return new(p)value_type(org);
}

//デバッグ情報表示
template<class T, std::size_t _POOL_SIZE>
void lfPoolAllocator<T, _POOL_SIZE>::printDebugInfo(std::function<void(const typename lfPoolAllocator<T, _POOL_SIZE>::value_type& value)> print_node)
{
	printf("----- Debug Info for lfPoolAllocator -----\n");
	//printf("POOL_SIZE=%d, VALUE_SIZE=%d, emptyHead=%d, usingCount=%d\n", POOL_SIZE, VALUE_SIZE, m_vacantHead.load(), m_usingCount.load());
	printf("POOL_SIZE=%d, VALUE_SIZE=%d, emptyHead=%d\n", POOL_SIZE, VALUE_SIZE, m_vacantHead.load());
	printf("Using:\n");
	for (int index = 0; index < POOL_SIZE; ++index)
	{
		if (m_using[index].load() != 0)
		{
			printf("[%d]", index);
			if (m_using[index].load() != 1)
				printf("(using=%d)", m_using[index].load());
			//printf("(leak=%d)", static_cast<int>(m_allocCount[index].load() - m_freeCount[index].load()));
			value_type* value = reinterpret_cast<value_type*>(m_pool[index]);
			print_node(*value);
			printf("\n");
		}
		//else
		//{
		//	if (m_allocCount[index].load() != m_freeCount[index].load())
		//		printf("[%d](leak=%d)\n", index, static_cast<int>(m_allocCount[index].load() - m_freeCount[index].load()));
		//}
	}
	printf("Recycable pool:\n");
	index_type recycable_index_and_tag = m_recyclableHead;
	while (recycable_index_and_tag != INVALID_INDEX)
	{
		index_type recycable_index = recycable_index_and_tag & 0x00ffffff;
		index_type tag = recycable_index_and_tag >> 24;
		printf(" [%d(tag=%d)]", recycable_index, tag);
		recycable_t* recycable_pool = reinterpret_cast<recycable_t*>(m_pool[recycable_index]);
		recycable_index_and_tag = recycable_pool->m_next_index.load();
	}
	printf("\n");
	printf("----------\n");
}

//コンストラクタ
template<class T, std::size_t _POOL_SIZE>
lfPoolAllocator<T, _POOL_SIZE>::lfPoolAllocator()
{
	m_vacantHead.store(0);
	m_recyclableHead.store(INVALID_INDEX);
	//m_usingCount.store(0);
	for (int i = 0; i < POOL_SIZE; ++i)
	{
		m_using[i].store(0);
		//m_allocCount[i].store(0);
		//m_freeCount[i].store(0);
	}
}

//デストラクタ
template<class T, std::size_t _POOL_SIZE>
lfPoolAllocator<T, _POOL_SIZE>::~lfPoolAllocator()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//__LOCKFREE_POOL_ALLOCATOR_CPP_H_

// End of file

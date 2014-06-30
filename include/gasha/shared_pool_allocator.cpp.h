#pragma once
#ifndef __SHARED_POOL_ALLOCATOR_CPP_H_
#define __SHARED_POOL_ALLOCATOR_CPP_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_allocator.h
// マルチスレッド共有プールアロケータ【関数定義部】
//
// ※クラスの実体化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shared_pool_allocator.inl>//共有プールアロケータ【インライン関数／テンプレート関数定義部】

#include <utility>//C++11 std::move
#include <stdio.h>//printf()

#ifdef GASHA_ASSERTION_IS_ENABLED
#include <assert.h>//assert()
#endif//GASHA_ASSERTION_IS_ENABLED

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//マルチスレッド共有プールアロケータクラス

//メモリ確保
template<class T, std::size_t _POOL_SIZE, class LOCK_TYPE>
void* sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::alloc()
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	//空きプールを確保
	if (m_vacantHead < POOL_SIZE)//空きプールの先頭インデックスがプールサイズ未満なら空きプールを利用する
	{
		const std::size_t vacant_index = m_vacantHead++;//空きプールの先頭インでックスを取得＆インクリメント
		m_using[vacant_index] = true;//インデックスを使用中にする
		//++m_usingCount;//使用中の数を増やす（デバッグ用）
		return  m_pool[vacant_index];//メモリ確保成功
	}
	//再利用プールの先頭インデックスが無効ならメモリ確保失敗（再利用プールが無い）
	if (m_recyclableHead == INVALID_INDEX)
		return nullptr;//メモリ確保失敗
	//再利用プールを確保
	{
		const std::size_t recyclable_index = m_recyclableHead;//再利用プールの先頭インデックスを取得
		recycable_t* recyclable_pool = reinterpret_cast<recycable_t*>(m_pool[recyclable_index]);//再利用プールの先頭を割り当て
		m_recyclableHead = recyclable_pool->m_next_index;//再利用プールの先頭インデックスを次の再利用プールに変更
		recyclable_pool->m_next_index = DIRTY_INDEX;//再利用プールの連結インデックスを削除
		m_using[recyclable_index] = true;//インデックスを使用中にする
		//++m_usingCount;//使用中の数を増やす（デバッグ用）
		return recyclable_pool;//メモリ確保成功
	}
}

//メモリ解放（共通処理）
//※ロック取得は呼び出し元で行う
template<class T, std::size_t _POOL_SIZE, class LOCK_TYPE>
bool sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::free(void* p, const std::size_t index)
{
	recycable_t* deleted_pool = reinterpret_cast<recycable_t*>(m_pool[index]);//解放されたメモリを参照
	deleted_pool->m_next_index = m_recyclableHead;//次の再利用プールのインデックスを保存
	m_recyclableHead = index;//再利用プールの先頭インデックスを変更
	m_using[index] = false;//インデックスを未使用状態にする
	//--m_usingCount;//使用中の数を減らす（デバッグ用）
	return true;
}

//ポインタをインデックスに変換
template<class T, std::size_t _POOL_SIZE, class LOCK_TYPE>
std::size_t sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::ptrToIndex(void* p)
{
	const std::size_t index = (reinterpret_cast<char*>(p) - reinterpret_cast<char*>(m_pool)) / VALUE_SIZE;
	if (index >= POOL_SIZE)//範囲外のインデックスなら終了
	{
	#ifdef GASHA_ASSERTION_IS_ENABLED
		static const bool IS_INVALID_POINTER_OF_POOL = false;
		assert(IS_INVALID_POINTER_OF_POOL);
	#endif//GASHA_ASSERTION_IS_ENABLED
		return INVALID_INDEX;
	}
	if (!m_using[index])//インデックスが既に未使用状態なら終了
	{
	#ifdef GASHA_ASSERTION_IS_ENABLED
		static const bool IS_ALREADY_DELETE_POINTER = false;
		assert(IS_ALREADY_DELETE_POINTER);
	#endif//GASHA_ASSERTION_IS_ENABLED
		return INVALID_INDEX;
	}
	return index;
}

//メモリ解放
template<class T, std::size_t _POOL_SIZE, class LOCK_TYPE>
bool sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::free(void* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	const std::size_t index = ptrToIndex(p);//ポインタをインデックスに変換
	if (index == INVALID_INDEX)
		return false;
	return free(p, index);
}

//メモリ確保とコンストラクタ呼び出し
template<class T, std::size_t _POOL_SIZE, class LOCK_TYPE>
typename sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::value_type* sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::newMoveObj(typename sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::value_type&& org)//※ムーブ版
{
	void* p = alloc();
	if (!p)
		return nullptr;
	return new(p)value_type(std::move(org));
}
template<class T, std::size_t _POOL_SIZE, class LOCK_TYPE>
typename sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::value_type* sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::newCopyObj(const typename sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::value_type& org)//※コピー版
{
	void* p = alloc();
	if (!p)
		return nullptr;
	return new(p)value_type(org);
}

//デバッグ情報表示
template<class T, std::size_t _POOL_SIZE, class LOCK_TYPE>
void sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::printDebugInfo(std::function<void(const typename sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::value_type& value)> print_node)
{
	printf("----- Debug Info for simplePoolAllocator -----\n");
	//printf("POOL_SIZE=%d, VALUE_SIZE=%d, emptyHead=%d, usingCount=%d\n", POOL_SIZE, VALUE_SIZE, m_vacantHead, m_usingCount);
	printf("POOL_SIZE=%d, VALUE_SIZE=%d, emptyHead=%d\n", POOL_SIZE, VALUE_SIZE, m_vacantHead);
	printf("Using:\n");
	for (int index = 0; index < POOL_SIZE; ++index)
	{
		if (m_using[index])
		{
			printf("[%d] ", index);
			value_type* value = reinterpret_cast<value_type*>(m_pool[index]);
			print_node(*value);
			printf("\n");
		}
	}
	printf("Recycable pool:\n");
	std::size_t recycable_index = m_recyclableHead;
	while (recycable_index != INVALID_INDEX)
	{
		printf(" [%d]", recycable_index);
		recycable_t* recycable_pool = reinterpret_cast<recycable_t*>(m_pool[recycable_index]);
		recycable_index = recycable_pool->m_next_index;
	}
	printf("\n");
	printf("----------\n");
}

//コンストラクタ
template<class T, std::size_t _POOL_SIZE, class LOCK_TYPE>
sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::sharedPoolAllocator() :
	m_vacantHead(0),
	m_recyclableHead(INVALID_INDEX)
{}

//デストラクタ
template<class T, std::size_t _POOL_SIZE, class LOCK_TYPE>
sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::~sharedPoolAllocator()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//__SHARED_POOL_ALLOCATOR_CPP_H_

// End of file

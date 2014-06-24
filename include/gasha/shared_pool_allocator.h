#pragma once
#ifndef __SHARED_POOL_ALLOCATOR_H_
#define __SHARED_POOL_ALLOCATOR_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_allocator.h
// マルチスレッド共有プールアロケータ
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/spin_lock.h>//スピンロック

//例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <cstddef>//std::size_t
#include <new>//new(void*), delete(void*, void*)
#include <bitset>//std::bitset
#include <assert.h>//assert()
#include <functional>//std::function
#include <stdio.h>//printf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//マルチスレッド共有プールアロケータクラス
//※デフォルトでは排他制御しないので、必要に応じてテンプレートパラメータに
//　spinLock や std::mutex などの適切な同期オブジェクト型の指定が必要。
//※排他制御が不要か滅多に必要がない場合は、ロックフリー版よりも速い。
template<class T, std::size_t _POOL_SIZE, class LOCK_TYPE = GASHA_ spinLock>
class alignas(4) sharedPoolAllocator
{
	static_assert(sizeof(T) >= 4, "sizeof(T) is too small.");

public:
	//型
	typedef T value_type;//値型
	typedef LOCK_TYPE lock_type;//ロック型

	//再利用プール型
	struct recycable_t
	{
		std::size_t m_next_index;//再利用プール連結インデックス
	};

public:
	//定数
	static const std::size_t POOL_SIZE = _POOL_SIZE;//プールサイズ（プールする個数）
	static const std::size_t VALUE_SIZE = sizeof(value_type);//値のサイズ
	static const std::size_t INVALID_INDEX = 0xffffffff;//無効なインデックス
	static const std::size_t DIRTY_INDEX = 0xfefefefe;//再利用プール連結インデックス削除用

public:
	//メソッド

	//メモリ確保
	void* alloc()
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

private:
	//メモリ解放（共通処理）
	//※ロック取得は呼び出し元で行う
	bool free(void* p, const std::size_t index)
	{
		recycable_t* deleted_pool = reinterpret_cast<recycable_t*>(m_pool[index]);//解放されたメモリを参照
		deleted_pool->m_next_index = m_recyclableHead;//次の再利用プールのインデックスを保存
		m_recyclableHead = index;//再利用プールの先頭インデックスを変更
		m_using[index] = false;//インデックスを未使用状態にする
		//--m_usingCount;//使用中の数を減らす（デバッグ用）
		return true;
	}
	
	//ポインタをインデックスに変換
	std::size_t ptrToIndex(void* p)
	{
		const std::size_t index = (reinterpret_cast<char*>(p) - reinterpret_cast<char*>(m_pool)) / VALUE_SIZE;
		if (index >= POOL_SIZE)//範囲外のインデックスなら終了
		{
		#ifdef _DEBUG
			static const bool IS_INVALID_POINTER_OF_POOL = false;
			assert(IS_INVALID_POINTER_OF_POOL);
		#endif//_DEBUG
			return INVALID_INDEX;
		}
		if (!m_using[index])//インデックスが既に未使用状態なら終了
		{
		#ifdef _DEBUG
			static const bool IS_ALREADY_DELETE_POINTER = false;
			assert(IS_ALREADY_DELETE_POINTER);
		#endif//_DEBUG
			return INVALID_INDEX;
		}
		return index;
	}

public:
	//メモリ解放
	bool free(void* p)
	{
		GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
		const std::size_t index = ptrToIndex(p);//ポインタをインデックスに変換
		if (index == INVALID_INDEX)
			return false;
		return free(p, index);
	}
	
	//メモリ確保とコンストラクタ呼び出し
	template<typename...Tx>
	value_type* newObj(Tx... args)
	{
		void* p = alloc();
		if (!p)
			return nullptr;
		return new(p)value_type(args...);
	}
	template<typename ObjType, typename...Tx>
	ObjType* newObj(Tx... args)
	{
		static_assert(sizeof(ObjType) <= VALUE_SIZE, "sizeof(ObjType) is too large.");
		void* p = alloc();
		if (!p)
			return nullptr;
		return new(p)ObjType(args...);
	}
	value_type* newCopyObj(value_type& org)
	{
		void* p = alloc();
		if (!p)
			return nullptr;
		return new(p)value_type(org);
	}
	value_type* newMoveObj(value_type&& org)
	{
		void* p = alloc();
		if (!p)
			return nullptr;
		return new(p)value_type(std::move(org));
	}

	//メモリ解放とデストラクタ呼び出し
	template<typename ObjType>
	bool deleteObj(ObjType* p)
	{
		GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
		const std::size_t index = ptrToIndex(p);//ポインタをインデックスに変換
		if (index == INVALID_INDEX)
			return false;
		p->~ObjType();//デストラクタ呼び出し
		//operator delete(p, p);
		return free(p, index);
	}

	//デバッグ情報表示
	void printDebugInfo(std::function<void(const value_type& value)> print_node)
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

public:
	//コンストラクタ
	sharedPoolAllocator() :
		m_vacantHead(0),
		m_recyclableHead(INVALID_INDEX)
	{}
	//デストラクタ
	~sharedPoolAllocator()
	{}

private:
	//フィールド
	char m_pool[POOL_SIZE][VALUE_SIZE];//プールバッファ ※先頭に配置してクラスのアライメントと一致させる
	std::size_t m_vacantHead;//空きプールの先頭インデックス
	std::size_t m_recyclableHead;//再利用プールの先頭インデックス
	std::bitset<POOL_SIZE> m_using;//使用中インデックス（二重解放判定用）
	//std::size_t m_usingCount;//使用中の数（デバッグ用）※必須の情報ではない
	lock_type m_lock;//ロックオブジェクト
};

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__SHARED_POOL_ALLOCATOR_H_

// End of file

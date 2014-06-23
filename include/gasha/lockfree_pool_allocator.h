#pragma once
#ifndef __LOCKFREE_POOL_ALLOCATOR_H_
#define __LOCKFREE_POOL_ALLOCATOR_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_pool_allocator.h
// ロックフリープールアロケータ
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <new>//new(void*), delete(void*, void*)
#include <bitset>//std::bitset
#include <thread>//C++11 std::this_thread

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリープールアロケータクラス
//※ABA問題対策あり（プールの再利用インデックスに8ビットのタグを付けて管理）
//※プール可能な要素数の最大は2^(32-8)-2 = 16,777,214個（2^(32-8)-1だと、値がINVALID_INDEXになる可能性があるのでNG）
template<class T, std::size_t POOL>
class lf_pool_allocator
{
	static_assert(sizeof(T) >= 4, "sizeof(T) is too small.");
	static_assert(POOL < 0x00ffffff, "POOL is too large.");

public:
	//型
	typedef T value_type;//値型

	//再利用プール型
	struct recycable_t
	{
		std::atomic<std::size_t> m_next_index;//再利用プール連結インデックス
	};

public:
	//定数
	static const std::size_t POOL_SIZE = POOL;//プールサイズ
	static const std::size_t VALUE_SIZE = sizeof(value_type);//値のサイズ
	static const std::size_t INVALID_INDEX = 0xffffffff;//無効なインデックス
	static const std::size_t DIRTY_INDEX = 0xfefefefe;//再利用プール連結インデックス削除用

public:
	//メソッド
	
	//メモリ確保
	void* alloc()
	{
		//空きプールを確保
		if (m_vacantHead.load() < POOL_SIZE)//空きプールの先頭インデックスがプールサイズ未満なら空きプールを利用
		{
			const std::size_t vacant_index = m_vacantHead.fetch_add(1);//空きプールの先頭インデックスを取得してインクリメント
			if (vacant_index < POOL_SIZE)//プールサイズ未満なら確保成功
			{
				m_using[vacant_index].fetch_add(1);//インデックスを使用中状態にする
			//	m_usingCount.fetch_add(1);//使用中の数を増やす（デバッグ用）
			//	m_allocCount[vacant_index].fetch_add(1);//アロケート回数をカウントアップ（デバッグ用）
				return m_pool[vacant_index];//メモリ確保成功
			}
			if (vacant_index > POOL_SIZE)//インクリメントでオーバーしたインデックスを元に戻す
				m_vacantHead.store(POOL_SIZE);
		}
		//再利用プールを確保
		{
			std::size_t recycable_index_and_tag = m_recyclableHead.load();//再利用プールの先頭インデックスを取得
		#ifdef USE_SAFE_ALLOC_LF_POOL_ALLOCATOR//【安全対策】※同じインデックスのアロケートを排他制御
			static const int retry_max = 32;
			int retry_count = retry_max;//リトライ回数
		#endif//USE_SAFE_ALLOC_LF_POOL_ALLOCATOR
			while (true)
			{
				if (recycable_index_and_tag == INVALID_INDEX)//再利用プールの先頭インデックスが無効ならメモリ確保失敗（再利用プールが無い）
					return nullptr;//メモリ確保失敗
				const std::size_t recyclable_index = recycable_index_and_tag & 0x00ffffff;//タグ削除
				if (recyclable_index >= POOL_SIZE)//再利用プールの先頭インデックス範囲外ならリトライ
				{
					recycable_index_and_tag = m_recyclableHead.load();//再利用プールの先頭インデックスを再取得
					continue;//リトライ
				}
			#ifdef USE_SAFE_ALLOC_LF_POOL_ALLOCATOR//【安全対策】※同じインデックスのアロケートを排他制御
				const std::size_t index_using = m_using[recyclable_index].fetch_add(1);//インデックスを使用中状態にする
				if (index_using != 0)//他のスレッドがインデックスを処理中ならリトライ
				{
					m_using[recyclable_index].fetch_sub(1);//インデックスの使用中状態を戻す
					--retry_count;
					if (retry_count == 0)//一定数のリトライごとにコンテキストスイッチ
					{
						retry_count = retry_max;//リトライ回数を初期状態に戻す
						std::this_thread::sleep_for(std::chrono::nanoseconds(0));//コンテキストスイッチ（ゼロスリープ）
					}
					recycable_index_and_tag = m_recyclableHead.load();//再利用プールのインデックスを再取得
					continue;//リトライ
				}
			#endif//USE_SAFE_ALLOC_LF_POOL_ALLOCATOR
				recycable_t* recyclable_pool = reinterpret_cast<recycable_t*>(m_pool[recyclable_index]);//再利用プールの先頭を割り当て
				const std::size_t next_index_and_tag = recyclable_pool->m_next_index.load();//次の再利用プールのインデックスを取得

				//CAS操作①
			#ifdef USE_SAFE_CAS_LF_POOL_ALLOCATOR//【安全対策】※スピンロックでCAS操作を保護
				m_lock.lock();//ロック取得
				const bool result = m_recyclableHead.compare_exchange_weak(recycable_index_and_tag, next_index_and_tag);//CAS操作
				m_lock.unlock();//ロック解除
				if (result)
			#else//USE_SAFE_CAS_LF_POOL_ALLOCATOR
				if (m_recyclableHead.compare_exchange_weak(recycable_index_and_tag, next_index_and_tag))//CAS操作
			#endif//USE_SAFE_CAS_LF_POOL_ALLOCATOR
				//【CAS操作の内容】
				//    if(m_recyclableHead == recyclable_index)//再利用プールの先頭インデックスを他のスレッドが書き換えていないか？
				//        m_recyclableHead = next;//再利用プールの先頭インデックスを次の再利用インデックスに変更（メモリ確保成功）
				//    else
				//        recycable_index_and_tag = m_recyclableHead;//再利用プールの先頭インデックスを再取得
				{
					recyclable_pool->m_next_index.store(DIRTY_INDEX);//再利用プールの連結インデックスを削除
				#ifndef USE_SAFE_ALLOC_LF_POOL_ALLOCATOR
					m_using[recyclable_index].fetch_sub(1);//インデックスを使用中状態にする
				#endif//USE_SAFE_ALLOC_LF_POOL_ALLOCATOR
				//	m_usingCount.fetch_add(1);//使用中の数を増やす（デバッグ用）
				//	m_allocCount[recyclable_index].fetch_add(1);//アロケート回数をカウントアップ（デバッグ用）
					return recyclable_pool;//メモリ確保成功
				}
				
		#ifdef USE_SAFE_ALLOC_LF_POOL_ALLOCATOR//【安全対策】※同じインデックスのアロケートを排他制御
				m_using[recyclable_index].fetch_sub(1);//インデックスの使用中状態を戻してリトライ
		#endif//USE_SAFE_ALLOC_LF_POOL_ALLOCATOR
			}
			return nullptr;//ダミー
		}
	}

private:
	//メモリ解放（共通処理）
	bool free(void* p, const std::size_t index)
	{
		const std::size_t tag = static_cast<std::size_t>(m_tag.fetch_add(1));//タグ取得
		const std::size_t index_and_tag = index | (tag << 24);//タグ付きインデックス作成
		std::size_t recycable_index_and_tag = m_recyclableHead.load();//再利用プールの先頭インデックスを取得
		while (true)
		{
			recycable_t* deleted_pool = reinterpret_cast<recycable_t*>(m_pool[index]);//解放されたメモリを参照
			deleted_pool->m_next_index.store(recycable_index_and_tag);//次の再利用プールのインデックスを保存
			
			//CAS操作②
		#ifdef USE_SAFE_CAS_LF_POOL_ALLOCATOR//【安全対策】※スピンロックでCAS操作を保護
			m_lock.lock();//ロック取得
			const bool result = m_recyclableHead.compare_exchange_weak(recycable_index_and_tag, index_and_tag);//CAS操作
			m_lock.unlock();//ロック解除
			if (result)
		#else//USE_SAFE_CAS_LF_POOL_ALLOCATOR
			if (m_recyclableHead.compare_exchange_weak(recycable_index_and_tag, index_and_tag))//CAS操作
		#endif//USE_SAFE_CAS_LF_POOL_ALLOCATOR
			//【CAS操作の内容】
			//    if(m_recyclableHead == recycable_index_and_tag)//再利用プールの先頭インデックスを他のスレッドが書き換えていないか？
			//        m_recyclableHead = index_and_tag;//再利用プールの先頭インデックスを次のインデックスに変更（メモリ解放成功）
			//    else
			//        recycable_index_and_tag = m_recyclableHead;//再利用プールの先頭インデックスを再取得
			{
				m_using[index].fetch_sub(1);//インデックスを未使用状態にする
			//	m_usingCount.fetch_sub(1);//使用中の数を減らす（デバッグ用）
			//	m_freeCount[index].fetch_add(1);//フリー回数をカウントアップ（デバッグ用）
				return true;//メモリ解放成功
			}
		}
		return false;//ダミー
	}
	
	//ポインタをインデックスに変換
	std::size_t ptrToIndex(void* p)
	{
		const std::size_t index = (reinterpret_cast<char*>(p) - reinterpret_cast<char*>(m_pool)) / VALUE_SIZE;
		if (index >= POOL_SIZE)//範囲外のインデックスなら終了
		{
			static const bool IS_INVALID_POINTER_OF_POOL = false;
			assert(IS_INVALID_POINTER_OF_POOL);
			return INVALID_INDEX;
		}
		if (m_using[index].load() == 0)//インデックスが既に未使用状態なら終了
		{
			static const bool IS_ALREADY_DELETE_POINTER = false;
			assert(IS_ALREADY_DELETE_POINTER);
			return INVALID_INDEX;
		}
		return index;
	}

public:
	//メモリ解放
	bool free(void* p)
	{
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
		printf("----- Debug Info for lf_pool_allocator -----\n");
	//	printf("POOL_SIZE=%d, VALUE_SIZE=%d, emptyHead=%d, usingCount=%d\n", POOL_SIZE, VALUE_SIZE, m_vacantHead.load(), m_usingCount.load());
		printf("POOL_SIZE=%d, VALUE_SIZE=%d, emptyHead=%d\n", POOL_SIZE, VALUE_SIZE, m_vacantHead.load());
		printf("Using:\n");
		for (int index = 0; index < POOL_SIZE; ++index)
		{
			if (m_using[index].load() != 0)
			{
				printf("[%d]", index);
				if (m_using[index].load() != 1)
					printf("(using=%d)", m_using[index].load());
			//	printf("(leak=%d)", static_cast<int>(m_allocCount[index].load() - m_freeCount[index].load()));
				print_node(*reinterpret_cast<const value_type*>(m_pool[index]));
				printf("\n");
			}
		//	else
		//	{
		//		if (m_allocCount[index].load() != m_freeCount[index].load())
		//			printf("[%d](leak=%d)\n", index, static_cast<int>(m_allocCount[index].load() - m_freeCount[index].load()));
		//	}
		}
		printf("Recycable pool:\n");
		std::size_t recycable_index_and_tag = m_recyclableHead;
		while (recycable_index_and_tag != INVALID_INDEX)
		{
			const std::size_t recycable_index = recycable_index_and_tag & 0x00ffffff;
			const std::size_t tag = recycable_index_and_tag >> 24;
			printf(" [%d(tag=%d)]", recycable_index, tag);
			recycable_t* recycable_pool = reinterpret_cast<recycable_t*>(m_pool[recycable_index]);
			recycable_index_and_tag = recycable_pool->m_next_index.load();
		}
		printf("\n");
		printf("----------\n");
	}

public:
	//コンストラクタ
	lf_pool_allocator()
	{
		m_vacantHead.store(0);
		m_recyclableHead.store(INVALID_INDEX);
	//	m_usingCount.store(0);
		for (int i = 0; i < POOL_SIZE; ++i)
		{
			m_using[i].store(0);
		//	m_allocCount[i].store(0);
		//	m_freeCount[i].store(0);
		}
	}
	//デストラクタ
	~lf_pool_allocator()
	{}

private:
	//フィールド
	char m_pool[POOL_SIZE][VALUE_SIZE];//プールバッファ ※先頭に配置してクラスのアライメントと一致させる
	std::atomic<std::size_t> m_vacantHead;//空きプールの先頭インデックス
	std::atomic<std::size_t> m_recyclableHead;//再利用プールの先頭インデックス
	std::atomic<unsigned char> m_tag;//ABA問題対策用のタグ
	std::atomic<char> m_using[POOL_SIZE];//使用中インデックス（二重解放判定＆保険の排他制御用）  ※std::bitset使用不可
//	std::atomic<std::size_t> m_usingCount;//使用中の数（デバッグ用）※必須の情報ではない
//	std::atomic<std::size_t> m_allocCount[POOL_SIZE];//アロケート回数（デバッグ用）※必須の情報ではない
//	std::atomic<std::size_t> m_freeCount[POOL_SIZE];//フリー回数（デバッグ用）※必須の情報ではない
#ifdef USE_SAFE_CAS_LF_POOL_ALLOCATOR
	spin_lock m_lock;//CAS操作保護用のスピンロック
#endif//USE_SAFE_CAS_LF_POOL_ALLOCATOR
};

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__LOCKFREE_POOL_ALLOCATOR_H_

// End of file

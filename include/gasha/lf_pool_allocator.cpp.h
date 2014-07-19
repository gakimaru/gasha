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

#include <gasha/type_traits.h>//型特性ユーティリティ
#include <gasha/string.h>//文字列処理：spprintf()

#include <cassert>//assert()

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
			m_usingPoolSize.fetch_add(1);//使用中の数を増やす（デバッグ用）
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
				m_usingPoolSize.fetch_add(1);//使用中の数を増やす（デバッグ用）
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
			m_usingPoolSize.fetch_sub(1);//使用中の数を減らす（デバッグ用）
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

//デバッグ情報作成
template<std::size_t _MAX_POOL_SIZE>
std::size_t lfPoolAllocator<_MAX_POOL_SIZE>::debugInfo(char* message, const std::size_t max_size, const bool with_detail) const
{
	auto print_node = [](char* message, const std::size_t max_size, std::size_t& message_len, const std::uint32_t& data) -> std::size_t
	{
		unionTypes uni(data);
		return GASHA_ spprintf(message, max_size, message_len, "image=[0x%02x,0x%02x,0x%02x,0x%02x]", uni.m_uchar[0], uni.m_uchar[1], uni.m_uchar[2], uni.m_uchar[3]);
	};
	return this->template debugInfo<std::uint32_t>(message, max_size, with_detail, print_node);
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//ロックフリープールアロケータの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_lfPoolAllocator(_MAX_POOL_SIZE) \
	template class GASHA_ lfPoolAllocator<_MAX_POOL_SIZE>;

#if 0//不要
//バッファ付きロックフリープールアロケータの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_lfPoolAllocator_withBuff(_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN) \
	template class GASHA_ lfPoolAllocator_withBuff<_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN>; \
	template class GASHA_ lfPoolAllocator<_POOL_SIZE>;

//型指定バッファ付きロックフリープールアロケータの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_lfPoolAllocator_withType(T, _POOL_SIZE) \
	template class GASHA_ lfPoolAllocator_withType<T, _POOL_SIZE>; \
	template class GASHA_ lfPoolAllocator_withBuff<sizeof(T), _POOL_SIZE, alignof(T)>; \
	template class GASHA_ lfPoolAllocator<_POOL_SIZE>;
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

#endif//GASHA_INCLUDED_LOCKFREE_POOL_ALLOCATOR_CPP_H

// End of file

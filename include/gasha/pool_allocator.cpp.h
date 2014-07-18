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
#include <gasha/string.h>//文字列処理：spprintf

#include <cassert>//assert()

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
		recycable_t* recyclable_pool = reinterpret_cast<recycable_t*>(refBuff(recyclable_index));//再利用プールの先頭を割り当て
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
	recycable_t* deleted_pool = reinterpret_cast<recycable_t*>(refBuff(index));//解放されたメモリを参照
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
std::size_t poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>::debugInfo(char* message, const bool with_detail) const
{
	auto print_node = [](char* message, const std::uint32_t& data) -> std::size_t
	{
		unionTypes uni(data);
		return GASHA_ spprintf(message, "image=[0x%02x,0x%02x,0x%02x,0x%02x]", uni.m_uchar[0], uni.m_uchar[1], uni.m_uchar[2], uni.m_uchar[3]);
	};
	return this->template debugInfo<std::uint32_t>(message, with_detail, print_node);
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//プールアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_poolAllocator(_MAX_POOL_SIZE) \
	template class GASHA_ poolAllocator<_MAX_POOL_SIZE>;
//※ロック指定版
#define GASHA_INSTANCING_poolAllocator_withLock(_MAX_POOL_SIZE, LOCK_TYPE) \
	template class GASHA_ poolAllocator<_MAX_POOL_SIZE, LOCK_TYPE>;

#if 0//不要
//バッファ付きプールアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_poolAllocator_withBuff(_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN) \
	template class GASHA_ poolAllocator_withBuff<_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN>; \
	template class GASHA_ poolAllocator<_POOL_SIZE>;
//※ロック指定版
#define GASHA_INSTANCING_poolAllocator_withBuff_withLock(_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN, LOCK_TYPE) \
	template class GASHA_ poolAllocator_withBuff<_BLOCK_SIZE, _POOL_SIZE, _BLOCK_ALIGN, LOCK_TYPE>; \
	template class GASHA_ poolAllocator<_POOL_SIZE, LOCK_TYPE>;

//型指定バッファ付きプールアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_poolAllocator_withType(T, _POOL_SIZE) \
	template class GASHA_ poolAllocator_withType<T, _POOL_SIZE>; \
	template class GASHA_ poolAllocator_withBuff<sizeof(T), _POOL_SIZE, alignof(T)>; \
	template class GASHA_ poolAllocator<_POOL_SIZE>;
//※ロック指定版
#define GASHA_INSTANCING_poolAllocator_withType_withLock(T, _POOL_SIZE, LOCK_TYPE) \
	template class GASHA_ poolAllocator_withType<T, _POOL_SIZE, LOCK_TYPE>; \
	template class GASHA_ poolAllocator_withBuff<sizeof(T), _POOL_SIZE, alignof(T), LOCK_TYPE>; \
	template class GASHA_ poolAllocator<_POOL_SIZE, LOCK_TYPE>;
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

#endif//GASHA_INCLUDED_POOL_ALLOCATOR_CPP_H

// End of file

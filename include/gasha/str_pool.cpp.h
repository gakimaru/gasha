#pragma once
#ifndef GASHA_INCLUDED_STR_POOL_CPP_H
#define GASHA_INCLUDED_STR_POOL_CPP_H

//--------------------------------------------------------------------------------
// str_pool.cpp.h
// 文字列プール【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/str_pool.inl>//文字列プール【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//文字列プール
//--------------------------------------------------------------------------------

//----------------------------------------
//文字列プール

//文字列プール登録
template<std::size_t _STR_POOL_BUFF_SIZE, std::size_t _STR_POOL_TABLE_SIZE, class LOCK_POLICY>
const char* strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY>::regist(GASHA_ crc32_t key, const char* str)
{
	//文字列プール情報を取得
	const char* pooled_str = (*this)(key);
	if (pooled_str)
	{
		//既に登録済みのため、文字列を照合
		const int compare = GASHA_ strcmp_fast(str, pooled_str);
		if (compare == 0)
			return pooled_str;//一致
		GASHA_SIMPLE_ASSERT(compare == 0, "key:0x%08x is already registered.(str=\"%s\", registered-str=\"%s\")", key, str, pooled_str);
		return nullptr;
	}
	return _registCommon(key, str);
}

//※【高速版（安全保証なし版）】
template<std::size_t _STR_POOL_BUFF_SIZE, std::size_t _STR_POOL_TABLE_SIZE, class LOCK_POLICY>
const char* strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY>::regist(const unsafe_t&, GASHA_ crc32_t key, const char* str)
{
	//文字列プール情報を取得
	const char* pooled_str = (*this)(key);
	if (pooled_str)
	{
		//既に登録済み
		return pooled_str;
	}
	return _registCommon(key, str);
}

//【共通処理】
template<std::size_t _STR_POOL_BUFF_SIZE, std::size_t _STR_POOL_TABLE_SIZE, class LOCK_POLICY>
const char* strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY>::_registCommon(GASHA_ crc32_t key, const char* str)
{
	//文字列プールからメモリ確保
	const std::size_t str_len = GASHA_ strlen_fast(str);
	const std::size_t str_size = str_len + 1;
	char* str_buff = m_strPoolBuff.template newArray<char>(str_size);
	GASHA_SIMPLE_ASSERT(str_buff != nullptr, "m_strPoolBuff is not enough memory.");
	if (!str_buff)
		return nullptr;
	//文字列プール情報を登録
	GASHA_ strcpy_fast(str_buff, str);
	const pool* str_info = m_strPoolTable.emplace(key, str_buff);//内部で排他ロック
	if (!str_info)//スレッド間で登録が競合した可能性があるので、文字列プール情報を再取得
	{
		//auto lock = m_strPoolTable.lockScoped();//共有ロック ※排他ロックで登録しているので、この時点で他方の処理が完了していないことはない（ロック不要）
		str_info = m_strPoolTable.at(key);
	}
	if (str_info)
		return str_info->m_str;
	GASHA_SIMPLE_ASSERT(str_info != nullptr, "m_strPoolTable is not enough memory.");
	return nullptr;
}

//静的フィールド
template<std::size_t _STR_POOL_BUFF_SIZE, std::size_t _STR_POOL_TABLE_SIZE, class LOCK_POLICY>
typename strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY>::unsafe_t strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY>::unsafe;//高速登録（安全保証なし版）指定用

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

#include <gasha/stack_allocator.cpp.h>//スタックアロケータ【関数／実体定義部】
#include <gasha/lf_stack_allocator.cpp.h>//ロックフリースタックアロケータ【関数／実体定義部】
#include <gasha/hash_table.cpp.h>//開番地法ハッシュテーブル【関数／実体定義部】

//スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_strPool(_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE) \
	template class GASHA_ strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE>; \
	template class GASHA_ stackAllocator<>; \
	template class GASHA_ hash_table::container<typename GASHA_ strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE>::strPoolOpe>;

//※ロック指定版
#define GASHA_INSTANCING_strPool_withLock(_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY) \
	template class GASHA_ strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY>; \
	template class GASHA_ lfStackAllocator<>; \
	template class GASHA_ hash_table::container<typename GASHA_ strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY>::strPoolOpe>;

#endif//GASHA_INCLUDED_STR_POOL_CPP_H

// End of file

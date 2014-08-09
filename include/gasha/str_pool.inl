#pragma once
#ifndef GASHA_INCLUDED_STR_POOL_INL
#define GASHA_INCLUDED_STR_POOL_INL

//--------------------------------------------------------------------------------
// str_pool.inl
// 文字列プール【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/str_pool.h>//文字列プール【宣言部】

#include <gasha/fast_string.h>//高速文字列処理
#include <gasha/simple_assert.h>//シンプルアサーション

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//文字列プール
//--------------------------------------------------------------------------------

//----------------------------------------
//文字列プール情報

//コンストラクタ
template<std::size_t _STR_POOL_BUFF_SIZE, std::size_t _STR_POOL_TABLE_SIZE, class LOCK_POLICY>
inline strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY>::pool::pool(const char* str) :
	m_str(str)
{}

//----------------------------------------
//文字列プールクラス

//関数オペレータ
//※文字列参照
template<std::size_t _STR_POOL_BUFF_SIZE, std::size_t _STR_POOL_TABLE_SIZE, class LOCK_POLICY>
inline const char* strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY>::operator()(const GASHA_ crc32_t key)
{
	auto lock = m_strPoolTable.lockScoped();//共有ロック ※登録中の情報にアクセスする可能性があるため、ロックを取得する
	const pool* str_info = m_strPoolTable.at(key);
	if(!str_info)
		return  nullptr;
	return str_info->m_str;
}

//文字列プール登録
template<std::size_t _STR_POOL_BUFF_SIZE, std::size_t _STR_POOL_TABLE_SIZE, class LOCK_POLICY>
const char* strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY>::regist(const char* str)
{
	GASHA_ crc32_t key = GASHA_ calcCRC32(str);
	return regist(key, str);
}

//※【高速版（安全保証なし版）】
template<std::size_t _STR_POOL_BUFF_SIZE, std::size_t _STR_POOL_TABLE_SIZE, class LOCK_POLICY>
inline const char* strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY>::regist(const unsafe_t&, const char* str)
{
	GASHA_ crc32_t key = GASHA_ calcCRC32(str);
	return regist(unsafe, key, str);
}

//【使用注意】クリア
template<std::size_t _STR_POOL_BUFF_SIZE, std::size_t _STR_POOL_TABLE_SIZE, class LOCK_POLICY>
inline void strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY>::clear()
{
	m_strPoolTable.clear();
	m_strPoolBuff.clear();
}

//コンストラクタ
template<std::size_t _STR_POOL_BUFF_SIZE, std::size_t _STR_POOL_TABLE_SIZE, class LOCK_POLICY>
inline strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY>::strPool() :
	m_strPoolBuff(),
	m_strPoolTable()
{}

//デストラクタ
template<std::size_t _STR_POOL_BUFF_SIZE, std::size_t _STR_POOL_TABLE_SIZE, class LOCK_POLICY>
inline strPool<_STR_POOL_BUFF_SIZE, _STR_POOL_TABLE_SIZE, LOCK_POLICY>::~strPool()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_STR_POOL_INL

// End of file

#pragma once
#ifndef GASHA_INCLUDED_SINGLETON_DEBUG_INL
#define GASHA_INCLUDED_SINGLETON_DEBUG_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ】
// singleton_debug.inl
// シングルトンデバッグ用処理【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/singleton_debug.h>//シングルトンデバッグ用処理【宣言部】

#include <gasha/lf_pool_allocator.inl>//ロックフリープールアロケータ【インライン関数／テンプレート関数定義部】
#include <gasha/linked_list.inl>//双方向連結リスト【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シングルトンデバッグ用処理
//--------------------------------------------------------------------------------

#ifdef GASHA_SINGLETON_DEBUG_ENABLED//シングルトンデバッグ用処理有効時

//なし

#else//GASHA_SINGLETON_DEBUG_ENABLED//シングルトンデバッグ用処理無向時

//----------------------------------------
//シングルトンデバッグ用ダミー処理

//シングルトン生成時呼び出し
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
inline bool singletonDebug<_MAX_RECORDS, LOCK_TYPE>::create(const char* procedure_name)
{
	//何もしない
	return true;
}

//シングルトン破棄時呼び出し
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
inline bool singletonDebug<_MAX_RECORDS, LOCK_TYPE>::destroy(const char* procedure_name)
{
	//何もしない
	return true;
}

//シングルトンアクセス開始時呼び出し
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
inline typename singletonDebug<_MAX_RECORDS, LOCK_TYPE>::id_type singletonDebug<_MAX_RECORDS, LOCK_TYPE>::enter(const char* procedure_name)
{
	//何もしない
	return id_type();
}

//シングルトンアクセス終了時呼び出し
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
inline bool singletonDebug<_MAX_RECORDS, LOCK_TYPE>::leave(const typename singletonDebug<_MAX_RECORDS, LOCK_TYPE>::id_type id)
{
	//何もしない
	return true;
}

//デバッグ情報作成
template<std::size_t _MAX_RECORDS, class LOCK_TYPE>
inline std::size_t  singletonDebug<_MAX_RECORDS, LOCK_TYPE>::debugInfo(char* message, const std::size_t max_size) const
{
	//何もしない
	message[0] = '\0';
	return 0;
}

#endif//GASHA_SINGLETON_DEBUG_ENABLED//シングルトンデバッグ用処理有効時

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SINGLETON_DEBUG_INL

// End of file

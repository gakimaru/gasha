#pragma once
#ifndef GASHA_INCLUDED_DUMMY_SINGLETON_DEBUG_INL
#define GASHA_INCLUDED_DUMMY_SINGLETON_DEBUG_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ】
// dummy_singleton_debug.inl
// シングルトンデバッグ用ダミー処理【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dummy_singleton_debug.h>//シングルトンデバッグ用ダミー処理【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シングルトンデバッグ用ダミー処理
//--------------------------------------------------------------------------------

//----------------------------------------
//シングルトンデバッグ用ダミー処理

//シングルトン生成時呼び出し
inline bool dummySingletonDebug::create(const char* procedure_name)
{
	//何もしない
	return true;
}

//シングルトン破棄時呼び出し
inline bool dummySingletonDebug::destroy(const char* procedure_name)
{
	//何もしない
	return true;
}

//シングルトンアクセス開始時呼び出し
inline dummySingletonDebug::id_type dummySingletonDebug::enter(const char* procedure_name)
{
	//何もしない
	return id_type();
}

//シングルトンアクセス終了時呼び出し
inline bool dummySingletonDebug::leave(const dummySingletonDebug::id_type)
{
	//何もしない
	return true;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_DUMMY_SINGLETON_DEBUG_INL

// End of file

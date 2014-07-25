#pragma once
#ifndef GASHA_INCLUDED_ALLOCATOR_COMMON_INL
#define GASHA_INCLUDED_ALLOCATOR_COMMON_INL

//--------------------------------------------------------------------------------
// allocator_common.inl
// メモリアロケータ共通設定・処理【インライン関数／テンプレート関数定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/allocator_common.h>//メモリアロケータ共通設定【宣言部】

#include <utility>//C++11 std::forward

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <new>//配置new/配置delete用
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//明示的なコンストラクタ／デストラクタ呼び出し
//--------------------------------------------------------------------------------

//明示的なコンストラクタ呼び出し
template<class T, typename... Tx>
inline T* callConstructor(void* buff, Tx&&... args) GASHA_NOEXCEPT
{
	return new(buff)T(std::forward<Tx>(args)...);
}
template<class T, typename... Tx>
inline T* callConstructor(T* buff, Tx&&... args) GASHA_NOEXCEPT
{
	return new(buff)T(std::forward<Tx>(args)...);
}

//明示的なデストラクタ呼び出し
template<class T>
inline void callDestructor(T* obj) GASHA_NOEXCEPT
{
	if (!obj)
	return;
	obj->~T();
	//operator delete (obj, obj);//（作法として）deleteオペレータ呼び出し
}
template<class T>
inline void callDestructor(void* obj) GASHA_NOEXCEPT
{
	if (!obj)
	return;
	reinterpret_cast<T*>(obj)->~T();
	//operator delete (obj, obj);//（作法として）deleteオペレータ呼び出し
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ALLOCATOR_COMMON_INL

// End of file

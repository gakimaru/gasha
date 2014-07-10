#pragma once
#ifndef GASHA_INCLUDED_NEW_H
#define GASHA_INCLUDED_NEW_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// new.h
// new/delete操作【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//明示的なコンストラクタ／デストラクタ呼び出し
//--------------------------------------------------------------------------------

//明示的なコンストラクタ呼び出し
template<class T, typename... Tx>
inline T* callConstructor(void* buff, Tx&&... args) GASHA_NOEXCEPT;
template<class T, typename... Tx>
inline T* callConstructor(T* buff, Tx&&... args) GASHA_NOEXCEPT;

//明示的なデストラクタ呼び出し
template<class T>
inline void callDestructor(T* obj) GASHA_NOEXCEPT;
template<class T>
inline void callDestructor(void* obj) GASHA_NOEXCEPT;

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/new.inl>

#endif//GASHA_INCLUDED_NEW_H

// End of file

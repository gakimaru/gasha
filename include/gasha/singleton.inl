#pragma once
#ifndef GASHA_INCLUDED_SINGLETON_INL
#define GASHA_INCLUDED_SINGLETON_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ】
// singleton.inl
// シングルトン【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/singleton.h>//シングルトン【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シングルトン
//--------------------------------------------------------------------------------


GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/singleton.inl>

#endif//GASHA_INCLUDED_SINGLETON_NIL

// End of file

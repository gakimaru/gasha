#pragma once
#ifndef GASHA_INCLUDED_LOCKFREE_STACK_INL
#define GASHA_INCLUDED_LOCKFREE_STACK_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_stack.inl
// ロックフリースタック【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_stack.h>//ロックフリースタック【宣言部】

#include <gasha/lf_pool_allocator.inl>//ロックフリープールアロケータ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリースタッククラス

//※なし

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOCKFREE_STACK_INL

// End of file

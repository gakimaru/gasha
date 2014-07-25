#pragma once
#ifndef GASHA_INCLUDED_LOCKFREE_QUEUE_INL
#define GASHA_INCLUDED_LOCKFREE_QUEUE_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_queue.inl
// ロックフリーキュー【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_queue.h>//ロックフリーキュー【宣言部】

#include <gasha/lf_pool_allocator.inl>//ロックフリープールアロケータ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリーキュークラス

//※なし

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOCKFREE_QUEUE_INL

// End of file

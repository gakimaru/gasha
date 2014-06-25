#pragma once
#ifndef __LOCKFREE_QUEUE_INL_
#define __LOCKFREE_QUEUE_INL_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_queue.h
// ロックフリーキュー【インライン関数／テンプレート関数実装部】
//
// ※クラスの操作が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_queue.h>//ロックフリーキュー【宣言部】

#include <gasha/lf_pool_allocator.inl>//ロックフリープールアロケータ【インライン関数／テンプレート関数実装部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリーキュークラス

//※なし

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__LOCKFREE_QUEUE_INL_

// End of file

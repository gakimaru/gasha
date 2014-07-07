#pragma once
#ifndef GASHA_INCLUDED_SHARED_QUEUE_INL
#define GASHA_INCLUDED_SHARED_QUEUE_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_queue.inl
// マルチスレッド共有キュー【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shared_queue.h>//マルチスレッド共有キュー【宣言部】

#include <gasha/shared_stack.inl>//マルチスレッド共有スタック【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//マルチスレッド共有キュークラス

//※なし

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SHARED_QUEUE_INL

// End of file

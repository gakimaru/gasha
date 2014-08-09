#pragma once
#ifndef GASHA_INCLUDED_I_ALLOCATOR_ADAPTER_INL
#define GASHA_INCLUDED_I_ALLOCATOR_ADAPTER_INL

//--------------------------------------------------------------------------------
// i_allocator_adapter.inl
// アロケータアダプタインターフェース【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_allocator_adapter.h>//メモリアロケータインターフェース【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アロケータアダプタインターフェースクラス

//デフォルトコンストラクタ
inline iAllocatorAdapter::iAllocatorAdapter() :
	m_isInstanced(true)
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_I_ALLOCATOR_ADAPTER_H

// End of file

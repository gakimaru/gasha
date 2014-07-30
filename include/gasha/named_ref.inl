#pragma	 once
#ifndef GASHA_INCLUDED_NAMED_REF_INL
#define GASHA_INCLUDED_NAMED_REF_INL

//--------------------------------------------------------------------------------
// named_ref.inl
// 名前付きデータ参照【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/named_ref.h>//名前付きデータ参照【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//名前付きデータ参照
//--------------------------------------------------------------------------------

//----------------------------------------
//名前付きデータ参照クラス

//デフォルトコンストラクタ
inline namedRef::namedRef()
{}

//デストラクタ
inline namedRef::~namedRef()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_NAMED_REF_INL

// End of file

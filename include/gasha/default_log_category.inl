#pragma once
#ifndef GASHA_INCLUDED_DEFAULT_LOG_CATEGORY_INL
#define GASHA_INCLUDED_DEFAULT_LOG_CATEGORY_INL

//--------------------------------------------------------------------------------
// default_log_category.inl
// 既定のログカテゴリ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/default_log_category.h>//既定のログカテゴリ【宣言部】

#include <gasha/log_category.inl>//ログカテゴリ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//既定のログカテゴリ
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//既定のログカテゴリを登録
void registDefaultLogCategory();

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_DEFAULT_LOG_CATEGORY_INL

// End of file

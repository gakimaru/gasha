#pragma once
#ifndef GASHA_INCLUDED_STRCONV_INL
#define GASHA_INCLUDED_STRCONV_INL

//--------------------------------------------------------------------------------
// strconv.inl
// 文字列変換【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/strconv.h>//文字列変換【宣言部】

#include <gasha/utility.h>//汎用ユーティリティ：min()

#include <cstring>//std::memcpy()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//ダミー文字列変換（関数オブジェクト）
inline  std::size_t dummyStrConv::operator()(char* dst, const std::size_t max_dst_size, const char* src, const std::size_t src_len) const
{
	if (!dst || max_dst_size == 0)
		return 0;
	const std::size_t minimum_len = GASHA_ min(max_dst_size, src_len + 1) - 1;
	if (minimum_len > 0)
		std::memcpy(dst, src, minimum_len);
	dst[minimum_len] = '\0';
	return minimum_len;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_STRCONV_INL

// End of file

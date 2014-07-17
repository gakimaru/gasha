#pragma once
#ifndef GASHA_INCLUDED_STRCONV_H
#define GASHA_INCLUDED_STRCONV_H

//--------------------------------------------------------------------------------
// strconv.h
// 文字列変換【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//ダミー文字列変換（関数オブジェクト）
struct dummyStrConv {
	inline  std::size_t operator()(char* dst, const std::size_t max_dst_size, const char* src, const std::size_t src_len) const;
};

//----------------------------------------
//大文字変換関数
std::size_t upperCaseConv(char* dst, const std::size_t max_dst_size, const char* src, const std::size_t src_len);

//----------------------------------------
//小文字変換関数
std::size_t lowerCaseConv(char* dst, const std::size_t max_dst_size, const char* src, const std::size_t src_len);

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/strconv.inl>

#endif//GASHA_INCLUDED_STRCONV_H

// End of file

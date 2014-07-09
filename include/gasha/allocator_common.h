#pragma once
#ifndef GASHA_INCLUDED_ALLOCATOR_COMMON_H
#define GASHA_INCLUDED_ALLOCATOR_COMMON_H

//--------------------------------------------------------------------------------
// allocator_common.h
// メモリアロケータ共通設定【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstddef>//C++11 std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//メモリアロケータ共通設定

static const int DEFAULT_ALIGN = sizeof(int);//デフォルトのメモリアライメント

//アロケート方向
enum allocateOrder_t
{
	ALLOC_ASC  = 0,//正順アロケート
	ALLOC_DESC = 1,//逆順アロケート
	ALLOC_UNKNOWN_ORDER = -1,//アロケート方向不明
};

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ALLOCATOR_COMMON_H

// End of file

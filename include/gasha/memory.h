#pragma once
#ifndef GASHA_INCLUDED_MEMORY_H
#define GASHA_INCLUDED_MEMORY_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// memory.h
// メモリ操作【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/basic_math.h>//基本算術

#include <cstddef>//std::size_t
#include <malloc.h>//_aligned_malloc(), _aligned_free()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アラインメント調整
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】アラインメント調整
template<std::size_t ALIGN>              inline std::size_t adjustAlign(const std::size_t value);
template<>                               inline std::size_t adjustAlign<1>(const std::size_t value){ return value; }
template<>                               inline std::size_t adjustAlign<0>(const std::size_t value){ return value; }
template <typename T, std::size_t ALIGN> inline T*          adjustAlign(const T* p);
                                         inline std::size_t adjustAlign(const std::size_t value, const std::size_t align);
template <typename T>                    inline T*          adjustAlign(const T* p, const std::size_t align);
inline bool isValidAlign(const std::size_t align);//アラインメントとして適正な値か判定

//----------------------------------------
//【メタプログラミング版】アラインメント調整
namespace _private
{
	template<std::size_t VAL, std::size_t ALIGN_1, std::size_t VAL_OR> struct adjustStaticAlign{ static const std::size_t value = (VAL + ALIGN_1) & ~ALIGN_1; };
	template<std::size_t VAL, std::size_t ALIGN_1>                     struct adjustStaticAlign<VAL, ALIGN_1, 0xffffffff>{ static const std::size_t value = 0; };//VAL & ~ALIGN_1; };
}//namespace _private
template<std::size_t VAL, std::size_t ALIGN> struct adjustStaticAlign{ static const std::size_t ALIGN_1 = ALIGN - 1;  static const std::size_t value = _private::adjustStaticAlign<VAL, ALIGN_1, VAL | ALIGN_1>::value; };
template<std::size_t VAL>                    struct adjustStaticAlign<VAL, 1>{ static const std::size_t value = VAL; };
template<std::size_t VAL>                    struct adjustStaticAlign<VAL, 0>{ static const std::size_t value = VAL; };
template<std::size_t ALIGN> struct isValidStaticAlign{    static const bool value = (countStaticBits<ALIGN>::value == 1); };//アラインメントとして適正な値か判定
template<>                  struct isValidStaticAlign<0>{ static const bool value = true; };                                //アラインメントとして適正な値か判定

GASHA_NAMESPACE_END;//ネームスペース：終了

//--------------------------------------------------------------------------------
//アラインメント付きメモリ確保
//※【注】グローバルネームスペースの関数として定義
//--------------------------------------------------------------------------------

//--------------------
//アラインメント指定付きメモリ確保
//※VC++仕様に合わせて共通化
#ifdef GASHA_HASCAMOUFLAGE_ALIGNED_MALLOC
#ifdef GASHA_IS_GCC
inline void* _aligned_malloc(const std::size_t size, const std::size_t alignment);
#endif//GASHA_IS_GCC
#endif//GASHA_HASCAMOUFLAGE_ALIGNED_MALLOC

//--------------------
//アラインメント指定付きメモリ解放
//※VC++仕様に合わせて共通化
#ifdef GASHA_HASCAMOUFLAGE_ALIGNED_FREE
#ifdef GASHA_IS_GCC
inline void _aligned_free(void* p);
#endif//GASHA_IS_GCC
#endif//GASHA_HASCAMOUFLAGE_ALIGNED_FREE

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/memory.inl>

#endif//GASHA_INCLUDED_MEMORY_H

// End of file

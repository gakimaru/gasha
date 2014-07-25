#pragma once
#ifndef GASHA_INCLUDED_MEMORY_INL
#define GASHA_INCLUDED_MEMORY_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// memory.inl
// メモリ操作【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/memory.h>//メモリ操作【宣言部】
#include <gasha/simple_assert.h>//シンプルアサーション

//#include <memory>//C++11 std::align
#include <cstdint>//std::uintptr_t

#ifdef GASHA_IS_GCC
#include <stdlib.h>//posix_memalign(), free()
#endif//GASHA_IS_GCC

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アラインメント調整
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】アラインメント調整
template<std::size_t ALIGN>
inline std::size_t adjustAlign(const std::size_t value)
{
	static_assert(countStaticBits<ALIGN>::value == 1, "ALIGN is only supported power of 2.");//2のべき乗（=ビット数が1の値）でなければNG
	static const std::size_t ALIGN_1 = ALIGN - 1;
	//if ((value | ALIGN_1) == 0xffffffff)
	//	return 0;//value & ~ALIGN_1;
	return (value + ALIGN_1) & ~ALIGN_1;
}

//※ポインタ版
template<typename T, std::size_t ALIGN>
inline T* adjustAlign(const T* p)
{
	return reinterpret_cast<T*>(adjustAlign<ALIGN>(reinterpret_cast<std::uintptr_t>(p)));
}

//※アライメントの引数指定版
inline std::size_t adjustAlign(const std::size_t value, const std::size_t align)
{
	if (align == 0)
		return value;
	GASHA_SIMPLE_ASSERT(countBits(static_cast<unsigned int>(align)) == 1, "align is not a power of 2.");
	const std::size_t align_1 = align - 1;
	return (value + align_1) & ~align_1;
}
//※ポインタ版＋アライメントの引数指定版
template <typename T>
inline T* adjustAlign(const T* p, const std::size_t align)
{
	return reinterpret_cast<T*>(adjustAlign(reinterpret_cast<std::uintptr_t>(p), align));
}

//----------------------------------------
//【ランタイム版】アラインメントとして適正な値か判定
inline bool isValidAlign(const std::size_t align)
{
	if (align == 0)
		return true;
	return (countBits(static_cast<unsigned int>(align)) == 1);
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//--------------------------------------------------------------------------------
//アラインメント付きメモリ確保
//※【注】グローバルネームスペースの関数として定義
//--------------------------------------------------------------------------------

//--------------------
//アラインメント指定付きメモリ確保
//※VC++仕様に合わせて共通化
#ifdef GASHA_HAS_CAMOUFLAGE_ALIGNED_MALLOC
#ifdef GASHA_IS_GCC
inline void* _aligned_malloc(const std::size_t size, const std::size_t alignment)
{
	void *p;
	int ret = posix_memalign(&p, alignment, size);
	return (ret == 0) ? p : 0;
}
#endif//GASHA_IS_GCC
#endif//GASHA_HAS_CAMOUFLAGE_ALIGNED_MALLOC

//--------------------
//アラインメント指定付きメモリ解放
//※VC++仕様に合わせて共通化
#ifdef GASHA_HAS_CAMOUFLAGE_ALIGNED_FREE
#ifdef GASHA_IS_GCC
inline void _aligned_free(void* p)
{
	free(p);
}
#endif//GASHA_IS_GCC
#endif//GASHA_HAS_CAMOUFLAGE_ALIGNED_FREE

#endif//GASHA_INCLUDED_MEMORY_INL

// End of file

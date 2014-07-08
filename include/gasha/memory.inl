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
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/memory.h>//メモリ操作【宣言部】

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
	return reinterpret_cast<T*>(adjustAlign<ALIGN>(reinterpret_cast<uintptr_t>(p)));
}

//※アライメントの引数指定版
inline std::size_t adjustAlign(const std::size_t value, const std::size_t align)
{
	if (align == 0)
		return value;
	assert(countBits(static_cast<unsigned int>(align)) == 1);
	const std::size_t align_1 = align - 1;
	return (value + align_1) & ~align_1;
}
//※ポインタ版＋アライメントの引数指定版
template <typename T>
inline T* adjustAlign(const T* p, const std::size_t align)
{
	return reinterpret_cast<T*>(adjustAlign(reinterpret_cast<uintptr_t>(p), align));
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

#endif//GASHA_INCLUDED_MEMORY_INL

// End of file

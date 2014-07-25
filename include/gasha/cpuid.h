#pragma once
#ifndef GASHA_INCLUDED_CPUID_H
#define GASHA_INCLUDED_CPUID_H

//--------------------------------------------------------------------------------
// cpuid.h
// CPU情報（x86系CPU用）【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstdint>//C++11 std::uint64_t, std::uint32_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

#ifdef GASHA_HAS_CAMOUFLAGE_CPUID
#ifdef GASHA_IS_X86
#ifdef GASHA_IS_VC
#include <intrin.h>//__cpuid(), _xgetbv()
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
void __cpuid(int cpu_info[4], const int type);
std::uint64_t _xgetbv(const std::uint32_t xcr);
#define _XCR_XFEATURE_ENABLED_MASK 0
#endif//GASHA_IS_GCC
#endif//GASHA_IS_X86
#endif//GASHA_HAS_CAMOUFLAGE_CPUID

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_CPUID_H

// End of file

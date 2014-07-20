#pragma once
#ifndef GASHA_INCLUDED_SIMPLE_ASSERT_H
#define GASHA_INCLUDED_SIMPLE_ASSERT_H

//--------------------------------------------------------------------------------
// simple_assert.h
// シンプルアサーション【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/debugger_break.h>//デバッガ用ブレークポイント割り込み

#include <cstdio>//std::printf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シンプルアサーション
//--------------------------------------------------------------------------------

//（なし）

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//シンプルアサーション／ブレークポイント／ウォッチポイント用マクロ

#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション無効時は無効化

#define GASHA_SIMPLE_ASSERT(expr, ...) ((!(expr)) && (std::fprintf(GASHA_SIMPLE_ASSERT_STDOUT, __VA_ARGS__) != 0) && (std::fprintf(GASHA_SIMPLE_ASSERT_STDOUT, "\nASSERTION FAILURE!: %s\nfile: %s\nfunc: %s\n", #expr, GASHA_SRC_FILE_LINE_TIME(), GASHA_FUNC_NAME()) != 0) && GASHA_ debuggerBreak())
#define GASHA_SIMPLE_BREAKPOINT(...) (true && (std::fprintf(GASHA_SIMPLE_ASSERT_STDOUT, __VA_ARGS__) != 0) && (std::fprintf(GASHA_SIMPLE_ASSERT_STDOUT, "\nfile: %s\nfunc: %s\n", GASHA_SRC_FILE_LINE_TIME(), GASHA_FUNC_NAME()) != 0) && GASHA_ debuggerBreak())
#define GASHA_SIMPLE_WATCHPOINT(expr, ...) ((expr) && (std::fprintf(GASHA_SIMPLE_ASSERT_STDOUT, __VA_ARGS__) != 0) && (std::fprintf(GASHA_SIMPLE_ASSERT_STDOUT, "\nHIT!: %s\nfile: %s\nfunc: %s\n", #expr, GASHA_SRC_FILE_LINE_TIME(), GASHA_FUNC_NAME()) != 0) && GASHA_ debuggerBreak())

#else//GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

#define GASHA_SIMPLE_ASSERT(expr, ...)
#define GASHA_SIMPLE_BREAKPOINT(...)
#define GASHA_SIMPLE_WATCHPOINT(expr, ...)

#endif//GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/simple_assert.inl>

#endif//GASHA_INCLUDED_SIMPLE_ASSERT_H

// End of file

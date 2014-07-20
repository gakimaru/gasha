#pragma once
#ifndef GASHA_INCLUDED_PRINT_H
#define GASHA_INCLUDED_PRINT_H

//--------------------------------------------------------------------------------
// print.h
// ログ出力操作【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/debug_log.h>//ログ操作

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ出力操作
//--------------------------------------------------------------------------------
//【注意】UTF-16, UTF-32（通常文字の文字コードに'\0'が含まれるもの）には非対応
//【注意】文字列中の改行コード変換は行わない（'\r', '\n' はそのまま出力する）
//【注意】文字コード変換処理を指定した場合、かつ、書式付き出力時は、ワークバッファ（logWorkBuff）の半分のサイズの文字列しか扱えない
//--------------------------------------------------------------------------------

//----------------------------------------
//ログ出力操作

//※指定のログレベルとログカテゴリに応じて、マスクされていない時だけ実際にコンソールに出力される。
//※ログ出力時はログ出力属性が適用され、ログレベル名やログカテゴリ名などを付加することができる。
//※ログレベルマスクを変更したい場合は、別途 logMask を使用する。
//※ログ出力属性を変更したい場合は、別途 logAttr を使用する。

//※ビルド設定 GASHA_LOG_PRINT_USE_QUEUE が指定された場合、この関数はログキューを使用した出力を行う。（未指定時は直接出力する）
//※キューを使用した出力の場合、キューからコンソールに出力するために、logQueueMonitor を専用スレッドで動作させておく必要がある。
//※キューを使用すると、マルチスレッドで出力が干渉することがない。
//※書式付き出力時は専用のワークバッファを使用するため、スタックサイズの小さなスレッドからでも問題なく出力可能。

template<typename... Tx>
inline bool print(const GASHA_ debugLog::level_type level, const GASHA_ debugLog::category_type category, const char* fmt, Tx&&... args);//書式付き出力
inline bool put(const GASHA_ debugLog::level_type level, const GASHA_ debugLog::category_type category, const char* str);//書式なし出力

//※文字コード変換処理指定版
//　CONVERTER のプロトタイプ：std::size_t converter(char* dst, const std::size_t dst_size, const char* src, const std::size_t src_size)
template<class CONVERTER_FUNC, typename... Tx>
inline bool convPrint(CONVERTER_FUNC converter_func, const GASHA_ debugLog::level_type level, const GASHA_ debugLog::category_type category, const char* fmt, Tx&&... args);//書式付き出力
template<class CONVERTER_FUNC>
inline bool convPut(CONVERTER_FUNC converter_func, const GASHA_ debugLog::level_type level, const GASHA_ debugLog::category_type category, const char* str);//書式なし出力

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/print.inl>

#endif//GASHA_INCLUDED_PRINT_H

// End of file

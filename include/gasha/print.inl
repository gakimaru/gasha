#pragma once
#ifndef GASHA_INCLUDED_PRINT_INL
#define GASHA_INCLUDED_PRINT_INL

//--------------------------------------------------------------------------------
// print.inl
// ログ出力操作【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/print.h>//ログ出力操作【宣言部】

#include <gasha/std_log_print.h>//標準ログ出力

#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ出力操作
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログ出力操作

//書式付きログ出力
template<typename... Tx>
inline bool print(const GASHA_ debugLog::level_type level, const GASHA_ debugLog::category_type category, const char* fmt, Tx&&... args)
{
	GASHA_ debugLog log;
#ifdef GASHA_LOG_PRINT_USE_QUEUE
	return log.print(level, category, fmt, std::forward<Tx>(args)...);
#else//GASHA_LOG_PRINT_USE_QUEUE
	return log.printDirect(level, category, fmt, std::forward<Tx>(args)...);
#endif//GASHA_LOG_PRINT_USE_QUEUE
}

//書式なしログ出力
inline bool put(const GASHA_ debugLog::level_type level, const GASHA_ debugLog::category_type category, const char* str)
{
	GASHA_ debugLog log;
#ifdef GASHA_LOG_PRINT_USE_QUEUE
	return log.put(level, category, str);
#else//GASHA_LOG_PRINT_USE_QUEUE
	return log.putDirect(level, category, str);
#endif//GASHA_LOG_PRINT_USE_QUEUE
}

//書式付きログ出力
//※文字コード変換処理指定版
template<class CONVERTER_FUNC, typename... Tx>
inline bool convPrint(CONVERTER_FUNC converter_func, const GASHA_ debugLog::level_type level, const GASHA_ debugLog::category_type category, const char* fmt, Tx&&... args)
{
	GASHA_ debugLog log;
#ifdef GASHA_LOG_PRINT_USE_QUEUE
	return log.convPrint(converter_func, level, category, fmt, std::forward<Tx>(args)...);
#else//GASHA_LOG_PRINT_USE_QUEUE
	return log.convPrintDirect(converter_func, level, category, fmt, std::forward<Tx>(args)...);
#endif//GASHA_LOG_PRINT_USE_QUEUE
}

//書式なしログ出力
//※文字コード変換処理指定版
template<class CONVERTER_FUNC>
inline bool convPut(CONVERTER_FUNC converter_func, const GASHA_ debugLog::level_type level, const GASHA_ debugLog::category_type category, const char* str)
{
	GASHA_ debugLog log;
#ifdef GASHA_LOG_PRINT_USE_QUEUE
	return log.convPut(converter_func, level, category, str);
#else//GASHA_LOG_PRINT_USE_QUEUE
	return log.convPutDirect(converter_func, level, category, str);
#endif//GASHA_LOG_PRINT_USE_QUEUE
}

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログ出力操作

//書式付きログ出力
template<typename... Tx>
inline bool print(const GASHA_ debugLog::level_type level, const GASHA_ debugLog::category_type category, const char* fmt, Tx&&... args)
{
	//なにもしない
	return true;//常に成功扱い
}

//書式なしログ出力
inline bool put(const GASHA_ debugLog::level_type level, const GASHA_ debugLog::category_type category, const char* str)
{
	//なにもしない
	return true;//常に成功扱い
}

//書式付きログ出力
//※文字コード変換処理指定版
template<class CONVERTER_FUNC, typename... Tx>
inline bool convPrint(CONVERTER_FUNC converter_func, const GASHA_ debugLog::level_type level, const GASHA_ debugLog::category_type category, const char* fmt, Tx&&... args)
{
	//なにもしない
	return true;//常に成功扱い
}

//書式なしログ出力
//※文字コード変換処理指定版
template<class CONVERTER_FUNC>
inline bool convPut(CONVERTER_FUNC converter_func, const GASHA_ debugLog::level_type level, const GASHA_ debugLog::category_type category, const char* str)
{
	//なにもしない
	return true;//常に成功扱い
}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_PRINT_INL

// End of file

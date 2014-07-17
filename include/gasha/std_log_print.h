#pragma once
#ifndef GASHA_INCLUDED_STD_LOG_PRINT_H
#define GASHA_INCLUDED_STD_LOG_PRINT_H

//--------------------------------------------------------------------------------
// std_log_print.h
// 標準ログ出力【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_print_info.h>//ログ出力情報

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準ログ出力
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//標準ログ出力
//※関数オブジェクト
struct stdLogPrint
{
public:
	//標準ログ出力
	//※長い処理だが、ログキューモニターのテンプレート関数内でインライン展開する事を意図して inline 宣言を付加
	inline void operator()(GASHA_ logPrintInfo& info);
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/std_log_print.inl>

#endif//GASHA_INCLUDED_STD_LOG_PRINT_H

// End of file

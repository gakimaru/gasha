﻿#pragma once
#ifndef GASHA_INCLUDED_STDIN_DEBUG_PAUSE_H
#define GASHA_INCLUDED_STDIN_DEBUG_PAUSE_H

//--------------------------------------------------------------------------------
// stdin_debug_pause.h
// 標準入力デバッグポーズ【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_debug_pause.h>//デバッグポーズインターフェース

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準入力デバッグポーズ
//--------------------------------------------------------------------------------

//----------------------------------------
//標準出力デバッグポーズ
class stdinDebugPause : public GASHA_ iDebugPause
{
#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

public:
	//メソッド

	//デバッガ用ブレークポイント割り込み
	void breakPoint() override;

	//ポーズ処理呼び出し
	void pause() override;

public:
	//デストラクタ
	~stdinDebugPause() override
	{}

#else// GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

public:
	inline void breakPoint(){}//デバッガ用ブレークポイント割り込み
	inline void pause(){}//ポーズ処理呼び出し
public:
	inline ~stdinDebugPause(){}//デストラクタ

#endif// GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化
};

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_STDIN_DEBUG_PAUSE_H

// End of file

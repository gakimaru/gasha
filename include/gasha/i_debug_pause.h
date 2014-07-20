#pragma once
#ifndef GASHA_INCLUDED_I_DEBUG_PAUSE_H
#define GASHA_INCLUDED_I_DEBUG_PAUSE_H

//--------------------------------------------------------------------------------
// i_debug_pause.h
// デバッグポーズインターフェース【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//デバッグポーズインターフェース
//--------------------------------------------------------------------------------

//----------------------------------------
//デバッグポーズインターフェースクラス
class iDebugPause
{
#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//メソッド

	//デバッガ用ブレークポイント割り込み
	virtual void breakPoint() = 0;

	//ポーズ処理呼び出し
	virtual void pause() = 0;

public:
	//デストラクタ
	virtual ~iDebugPause()
	{}

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//メソッド
	inline void breakPoint(){}//デバッガ用ブレークポイント割り込み
	inline void pause(){}//ポーズ処理呼び出し
public:
	inline ~iDebugPause(){}//デストラクタ

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/i_debug_pause.inl>

#endif//GASHA_INCLUDED_I_DEBUG_PAUSE_H

// End of file

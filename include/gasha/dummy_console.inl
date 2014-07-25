#pragma once
#ifndef GASHA_INCLUDED_DUMMY_CONSOLE_INL
#define GASHA_INCLUDED_DUMMY_CONSOLE_INL

//--------------------------------------------------------------------------------
// dummy_console.inl
// ダミーコンソール【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dummy_console.h>//ダミーコンソール【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ダミーコンソール
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ダミーコンソールクラス

//カラー変更
inline void dummyConsole::changeColor(const GASHA_ consoleColor& color)
{
	//なにもしない
}

//コンストラクタ
inline dummyConsole::dummyConsole(const char* name) :
	m_name(name)
{}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_DUMMY_CONSOLE_INL

// End of file

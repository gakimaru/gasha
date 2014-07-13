#pragma once
#ifndef GASHA_INCLUDED_CONSOLE_STD_H
#define GASHA_INCLUDED_CONSOLE_STD_H

//--------------------------------------------------------------------------------
// console_stds.h
// 標準コンソール【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース

#ifdef GASHA_USE_WINDOWS_CONSOLE
#include <gasha/console_win.h>//Windowsコマンドプロンプト
#else//GASHA_USE_WINDOWS_CONSOLE
#include <gasha/console_tty.h>//TTY端末
#endif//GASHA_USE_WINDOWS_CONSOLE

#include <stdio.h>//stdout, stderr

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準コンソール
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

#ifdef GASHA_USE_WINDOWS_CONSOLE

//----------------------------------------
//標準コンソール
//※Windowsコマンドプロンプトに委譲
class consoleStd : public consoleWin
{
public:
	//コンストラクタ
	consoleStd():
		consoleWin(stdout)
	{}
	//デストラクタ
	~consoleStd() override
	{}
};

//----------------------------------------
//画面通知用標準コンソール
//※Windowsコマンドプロンプトに委譲
class consoleStdForNotice : public consoleWin
{
public:
	//コンストラクタ
	consoleStdForNotice():
		consoleWin(stderr)
	{}
	//デストラクタ
	~consoleStdForNotice() override
	{}
};

#else//GASHA_USE_WINDOWS_CONSOLE

//----------------------------------------
//標準コンソール
//TTY端末に委譲
class consoleStd : public consoleTTY
{
public:
	//コンストラクタ
	consoleStd():
		consoleTTY(stdout)
	{}
	//デストラクタ
	~consoleStd() override
	{}
};

//----------------------------------------
//画面通知用標準コンソール
//TTY端末に委譲
class consoleStdForNotice : public consoleTTY
{
public:
	//コンストラクタ
	consoleStdForNotice():
		consoleTTY(stderr)
	{}
	//デストラクタ
	~consoleStdForNotice() override
	{}
};

#endif//GASHA_USE_WINDOWS_CONSOLE

//----------------------------------------
//標準コンソールのインスタンス
extern consoleStd g_consoleStd;//標準コンソール
extern consoleStdForNotice g_consoleStdForNotice;//画面通知用標準コンソール

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_CONSOLE_STD_H

// End of file

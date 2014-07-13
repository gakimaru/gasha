#pragma once
#ifndef GASHA_INCLUDED_STD_CONSOLE_H
#define GASHA_INCLUDED_STD_CONSOLE_H

//--------------------------------------------------------------------------------
// std_console.h
// 標準コンソール【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース

#ifdef GASHA_USE_WINDOWS_CONSOLE
#include <gasha/win_console.h>//Windowsコマンドプロンプト
#else//GASHA_USE_WINDOWS_CONSOLE
#include <gasha/tty_console.h>//TTY端末
#endif//GASHA_USE_WINDOWS_CONSOLE

#include <cstdio>//stdout, stderr

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準コンソール
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

#ifdef GASHA_USE_WINDOWS_CONSOLE

//----------------------------------------
//標準コンソール
//※Windowsコマンドプロンプトに委譲
class stdConsole : public winConsole
{
public:
	//静的アクセッサ
	inline static stdConsole& instance(){ return s_instance; }

public:
	//コンストラクタ
	stdConsole():
		winConsole(stdout)
	{}
	//デストラクタ
	~stdConsole() override
	{}

private:
	//静的フィールド
	static stdConsole s_instance;
};

//----------------------------------------
//画面通知用標準コンソール
//※Windowsコマンドプロンプトに委譲
class stdConsoleForNotice : public winConsole
{
public:
	//静的アクセッサ
	inline static stdConsoleForNotice& instance(){ return s_instance; }

public:
	//コンストラクタ
	stdConsoleForNotice():
		winConsole(stderr)
	{}
	//デストラクタ
	~stdConsoleForNotice() override
	{}

private:
	//静的フィールド
	static stdConsoleForNotice s_instance;
};

#else//GASHA_USE_WINDOWS_CONSOLE

//----------------------------------------
//標準コンソール
//※TTY端末に委譲
class stdConsole : public ttyConsole
{
public:
	//静的アクセッサ
	inline static stdConsole& instance(){ return s_instance; }

public:
	//コンストラクタ
	stdConsole():
		ttyConsole(stdout)
	{}
	//デストラクタ
	~stdConsole() override
	{}

private:
	//静的フィールド
	static stdConsole s_instance;
};

//----------------------------------------
//画面通知用標準コンソール
//※TTY端末に委譲
class stdConsoleForNotice : public ttyConsole
{
public:
	//静的アクセッサ
	inline static stdConsoleForNotice& instance(){ return s_instance; }

public:
	//コンストラクタ
	stdConsoleForNotice():
		ttyConsole(stderr)
	{}
	//デストラクタ
	~stdConsoleForNotice() override
	{}

private:
	//静的フィールド
	static stdConsoleForNotice s_instance;
};

#endif//GASHA_USE_WINDOWS_CONSOLE

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_STD_CONSOLE_H

// End of file

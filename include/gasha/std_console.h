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
//標準出力コンソール
//※Windowsコマンドプロンプトに委譲
class stdOutConsole : public winConsole
{
public:
	//静的アクセッサ
	inline static stdOutConsole& instance(){ return s_instance; }

public:
	//コンストラクタ
	stdOutConsole() :
		winConsole(stdout, "Win(stdout)")
	{}
	//デストラクタ
	~stdOutConsole() override
	{}

private:
	//静的フィールド
	static stdOutConsole s_instance;
};

//----------------------------------------
//標準エラーコンソール
//※Windowsコマンドプロンプトに委譲
class stdErrConsole : public winConsole
{
public:
	//静的アクセッサ
	inline static stdErrConsole& instance(){ return s_instance; }

public:
	//コンストラクタ
	stdErrConsole() :
		winConsole(stderr, "Win(stderr)")
	{}
	//デストラクタ
	~stdErrConsole() override
	{}

private:
	//静的フィールド
	static stdErrConsole s_instance;
};

//----------------------------------------
//画面通知用標準コンソール
//※Windowsコマンドプロンプトに委譲
class stdConsoleOfNotice : public winConsole
{
public:
	//静的アクセッサ
	inline static stdConsoleOfNotice& instance(){ return s_instance; }

public:
	//コンストラクタ
	stdConsoleOfNotice():
		winConsole(stderr, "Win(stderr)")
	{}
	//デストラクタ
	~stdConsoleOfNotice() override
	{}

private:
	//静的フィールド
	static stdConsoleOfNotice s_instance;
};

#else//GASHA_USE_WINDOWS_CONSOLE

//----------------------------------------
//標準出力コンソール
//※TTY端末に委譲
class stdOutConsole : public ttyConsole
{
public:
	//静的アクセッサ
	inline static stdOutConsole& instance(){ return s_instance; }

public:
	//コンストラクタ
	stdOutConsole():
		ttyConsole(stdout, "TTY(stdout)")
	{}
	//デストラクタ
	~stdOutConsole() override
	{}

private:
	//静的フィールド
	static stdOutConsole s_instance;
};

//----------------------------------------
//標準エラーコンソール
//※TTY端末に委譲
class stdErrConsole : public ttyConsole
{
public:
	//静的アクセッサ
	inline static stdErrConsole& instance(){ return s_instance; }

public:
	//コンストラクタ
	stdErrConsole():
		ttyConsole(stderr, "TTY(stderr)")
	{}
	//デストラクタ
	~stdErrConsole() override
	{}

private:
	//静的フィールド
	static stdErrConsole s_instance;
};

//----------------------------------------
//画面通知用標準コンソール
//※TTY端末に委譲
class stdConsoleOfNotice : public ttyConsole
{
public:
	//静的アクセッサ
	inline static stdConsoleOfNotice& instance(){ return s_instance; }

public:
	//コンストラクタ
	stdConsoleOfNotice():
		ttyConsole(stderr, "TTY(stderr)")
	{}
	//デストラクタ
	~stdConsoleOfNotice() override
	{}

private:
	//静的フィールド
	static stdConsoleOfNotice s_instance;
};

#endif//GASHA_USE_WINDOWS_CONSOLE

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_STD_CONSOLE_H

// End of file

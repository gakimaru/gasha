#pragma once
#ifndef GASHA_INCLUDED_VS_CONSOLE_H
#define GASHA_INCLUDED_VS_CONSOLE_H

//--------------------------------------------------------------------------------
// vs_console.h
// Visual Studio出力ウインドウ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース

#ifndef GASHA_USE_VS_CONSOLE
#ifdef GASHA_USE_WINDOWS_CONSOLE
#include <gasha/win_console.h>//Windowsコマンドプロンプト
#else//GASHA_USE_WINDOWS_CONSOLE
#include <gasha/tty_console.h>//TTY端末
#endif//GASHA_USE_WINDOWS_CONSOLE
#include <stdio.h>//stdout
#endif//GASHA_USE_VS_CONSOLE

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//Visual Studio出力ウインドウ
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//Visual Studio出力ウインドウクラス

#ifdef GASHA_USE_VS_CONSOLE
//Visual Studio出力ウインドウクラス有効時
class vsConsole : public GASHA_ IConsole
{
public:
	//アクセッサ
	const char* name() const override { return m_name; }

public:
	//メソッド

	//出力開始
	void beginOutput() override;

	//出力終了
	//※フラッシュ可能な状態
	void endOutput() override;

	//出力
	void output(const char* str) override;

	//カラー変更
	void changeColor(GASHA_ consoleColor&& color) override;
	inline void changeColor(const GASHA_ consoleColor& color);
	
	//カラーリセット
	void resetColor() override;

public:
	//コンストラクタ
	inline vsConsole(const char* name = "VS-console");
	//デストラクタ
	~vsConsole() override;

private:
	//フィールド
	const char* m_name;//名前
};

#else//GASHA_USE_VS_CONSOLE
//Visual Studio出力ウインドウクラス無効時

#ifdef GASHA_USE_WINDOWS_CONSOLE
//Windowsコマンドプロンプトに委譲
class vsConsole : public winConsole
{
public:
	//コンストラクタ
	vsConsole(const char* name = "Win-console"):
		winConsole(stdout, name)
	{}
	//デストラクタ
	~vsConsole() override
	{}
};

#else//GASHA_USE_WINDOWS_CONSOLE
//TTY端末に委譲
class vsConsole : public ttyConsole
{
public:
	//コンストラクタ
	vsConsole(const char* name = "TTY-console") :
		ttyConsole(stdout, name)
	{}
	//デストラクタ
	~vsConsole() override
	{}
};

#endif//GASHA_USE_WINDOWS_CONSOLE

#endif//GASHA_USE_VS_CONSOLE

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/vs_console.inl>

#endif//GASHA_INCLUDED_VS_CONSOLE_H

// End of file

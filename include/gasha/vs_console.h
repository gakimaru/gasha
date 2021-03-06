﻿#pragma once
#ifndef GASHA_INCLUDED_VS_CONSOLE_H
#define GASHA_INCLUDED_VS_CONSOLE_H

//--------------------------------------------------------------------------------
// vs_console.h
// Visual Studio出力ウインドウ【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
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
#include <cstdio>//stdout
#endif//GASHA_USE_VS_CONSOLE

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//Visual Studio出力ウインドウ
//--------------------------------------------------------------------------------
//【注意】UTF-16, UTF-32（通常文字の文字コードに'\0'が含まれるもの）には非対応
//【注意】文字列中の改行コード変換は行わない（'\r', '\n' はそのまま出力する）
//--------------------------------------------------------------------------------

//----------------------------------------
//Visual Studio出力ウインドウクラス

#ifdef GASHA_USE_VS_CONSOLE
//Visual Studio出力ウインドウクラス有効時
class vsConsole : public GASHA_ iConsole
{
#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//アクセッサ
	const char* name() const override { return m_name; }

public:
	//メソッド

	//出力開始
	void begin() override;

	//出力終了
	//※フラッシュ可能な状態
	void end() override;

	//出力
	void put(const char* str) override;

	//改行出力
	void putCr() override;

	//カラー変更
	void changeColor(GASHA_ consoleColor&& color) override;
	inline void changeColor(const GASHA_ consoleColor& color);
	
	//カラーリセット
	void resetColor() override;

	//出力先が同じか判定
	bool isSame(const iConsole* rhs) const override;

public:
	//コンストラクタ
	inline vsConsole(const char* name = "VS-console");
	//デストラクタ
	~vsConsole() override;

private:
	//フィールド
	const char* m_name;//名前

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//アクセッサ
	inline const char* name() const { return ""; }
public:
	//メソッド
	inline void begin(){}//出力開始
	inline void end(){}//出力終了
	inline void put(const char* str){}//出力
	inline void putCr(){}//改行出力
	inline void changeColor(GASHA_ consoleColor&& color){}//カラー変更
	inline void changeColor(const GASHA_ consoleColor& color){}//カラー変更
	inline void resetColor(){}//カラーリセット
	inline bool isSame(const iConsole* rhs) const{ return true; }//出力先が同じか判定
public:
	inline vsConsole(const char* name = nullptr){}//コンストラクタ
	inline ~vsConsole(){}//デストラクタ

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
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
};

#else//GASHA_USE_WINDOWS_CONSOLE
//TTY端末に委譲
class vsConsole : public ttyConsole
{
public:
	//コンストラクタ
	inline vsConsole(const char* name = "TTY-console") :
		ttyConsole(stdout, name)
	{}
};

#endif//GASHA_USE_WINDOWS_CONSOLE

#endif//GASHA_USE_VS_CONSOLE

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/vs_console.inl>

#endif//GASHA_INCLUDED_VS_CONSOLE_H

// End of file

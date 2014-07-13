﻿#pragma once
#ifndef GASHA_INCLUDED_CONSOLE_WIN_H
#define GASHA_INCLUDED_CONSOLE_WIN_H

//--------------------------------------------------------------------------------
// console_win.h
// Windowsコマンドプロントプト【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース

#ifndef GASHA_USE_WINDOWS_CONSOLE
#include <gasha/console_tty.h>//TTY端末
#endif//GASHA_USE_WINDOWS_CONSOLE

#include <stdio.h>//FILE, stdout, stderr

#ifdef GASHA_USE_WINDOWS_CONSOLE
#include <Windows.h>//HANDLE, CONSOLE_SCREEN_BUFFER_INFO
#endif//GASHA_USE_WINDOWS_CONSOLE

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//Windowsコマンドプロントプト
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//Windowsコマンドプロントプトクラス

#ifdef GASHA_USE_WINDOWS_CONSOLE
//Windowsコマンドプロンプトクラス有効時
class consoleWin : public GASHA_ IConsole
{
public:
	//アクセッサ
	const char* name() const override { return "Win"; }
	inline FILE* handle() const { return m_handle; }//ハンドル
	inline HANDLE hWin() const { return m_hWin; }//Windowsハンドル
	inline const CONSOLE_SCREEN_BUFFER_INFO& screenBuffer() const { return m_screenBuffer; }//スクリーンバッファ

public:
	//メソッド

	//出力開始
	void beginOutput() override;

	//出力終了
	//※フラッシュ可能な状態
	void endOutput() override;

	//出力
	void output(const char* str) override;

	//書式付き出力
	template<typename... Tx>
	inline int printf(const char* fmt, Tx&&... args) const;
	
	//カラー変更
	void changeColor(const GASHA_ consoleColor& color) override;
	
	//カラーリセット
	void resetColor() override;

public:
	//コンストラクタ
	consoleWin(FILE* handle);
	//デストラクタ
	~consoleWin() override;
private:
	//フィールド
	FILE* m_handle;//ハンドル
	HANDLE m_hWin;//Windowsハンドル
	CONSOLE_SCREEN_BUFFER_INFO m_screenBuffer;//スクリーンバッファ
};

#else//GASHA_USE_WINDOWS_CONSOLE
//Windowsコマンドプロンプトクラス無効時

//TTY端末に委譲
class consoleWin : public consoleTTY
{
public:
	//コンストラクタ
	consoleWin(FILE* handle) :
		consoleTTY(handle)
	{}
	//デストラクタ
	~consoleWin() override
	{}
};

#endif//GASHA_USE_WINDOWS_CONSOLE

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/console_win.inl>

#endif//GASHA_INCLUDED_CONSOLE_WIN_H

// End of file

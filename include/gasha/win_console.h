#pragma once
#ifndef GASHA_INCLUDED_WIN_CONSOLE_H
#define GASHA_INCLUDED_WIN_CONSOLE_H

//--------------------------------------------------------------------------------
// win_console.h
// Windowsコマンドプロントプト【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース
#include <gasha/console_color.h>//コンソールカラー

#ifndef GASHA_USE_WINDOWS_CONSOLE
#include <gasha/tty_console.h>//TTY端末
#endif//GASHA_USE_WINDOWS_CONSOLE

#include <cstdio>//FILE, stdout, stderr

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
#ifdef GASHA_USE_WINDOWS_CONSOLE
#include <Windows.h>//HANDLE, CONSOLE_SCREEN_BUFFER_INFO
//Windows.h のインクルードによる min, max を無効化する
#ifdef min
#undef min
#endif//min
#ifdef max
#undef max
#endif//max
#endif//GASHA_USE_WINDOWS_CONSOLE
#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//Windowsコマンドプロントプト
//--------------------------------------------------------------------------------
//【注意】UTF-16, UTF-32（通常文字の文字コードに'\0'が含まれるもの）には非対応
//【注意】文字列中の改行コード変換は行わない（'\r', '\n' はそのまま出力する）
//--------------------------------------------------------------------------------

//----------------------------------------
//Windowsコマンドプロントプトクラス

#ifdef GASHA_USE_WINDOWS_CONSOLE
//Windowsコマンドプロンプトクラス有効時
class winConsole : public GASHA_ IConsole
{
#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//アクセッサ
	const char* name() const override { return m_name; }
	inline std::FILE* handle() const { return m_handle; }//ハンドル
	inline HANDLE hWin() const { return m_hWin; }//Windowsハンドル
	inline const CONSOLE_SCREEN_BUFFER_INFO& screenBuffer() const { return m_screenBuffer; }//スクリーンバッファ

public:
	//メソッド

	//出力開始
	void begin() override;

	//出力終了
	//※フラッシュ可能な状態
	void end() override;

	//出力
	void put(const char* str) override;

	//書式付き出力
	template<typename... Tx>
	inline int printf(const char* fmt, Tx&&... args) const;

	//改行出力
	//※改行前にカラーのリセットも行う
	void putCr() override;

	//カラー変更
	void changeColor(GASHA_ consoleColor&& color) override;
	inline void changeColor(const GASHA_ consoleColor& color);
	
	//カラーリセット
	void resetColor() override;

	//出力先が同じか判定
	bool isSame(const IConsole* rhs) const override;

public:
	//コンストラクタ
	winConsole(std::FILE* handle, const char* name = "Win-console");
	//デストラクタ
	~winConsole() override;

private:
	//フィールド
	const char* m_name;//名前
	std::FILE* m_handle;//ハンドル
	HANDLE m_hWin;//Windowsハンドル
	CONSOLE_SCREEN_BUFFER_INFO m_screenBuffer;//スクリーンバッファ
	consoleColor m_currColor;//現在のカラー

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//アクセッサ
	inline const char* name() const { return ""; }
	inline std::FILE* handle() const { return nullptr; }//ハンドル
	inline int hWin() const { return 0; }//Windowsハンドル
	//inline const CONSOLE_SCREEN_BUFFER_INFO& screenBuffer() const { return m_screenBuffer; }//スクリーンバッファ
public:
	//メソッド
	inline void begin(){}//出力開始
	inline void end(){}//出力終了
	inline void put(const char* str){}//出力
	template<typename... Tx>
	inline int printf(const char* fmt, Tx&&... args) const{ return 0; }//書式付き出力
	inline void putCr(){}//改行出力
	inline void changeColor(GASHA_ consoleColor&& color){}//カラー変更
	inline void changeColor(const GASHA_ consoleColor& color){}//カラー変更
	inline void resetColor(){}//カラーリセット
	inline bool isSame(const IConsole* rhs) const{ return true; }//出力先が同じか判定
public:
	inline winConsole(std::FILE* handle, const char* name = nullptr){}//コンストラクタ
	inline ~winConsole(){}//デストラクタ

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
};

//----------------------------------------
//カラー出力なしWindowsコマンドプロンプトクラス
class monoWinConsole : public winConsole
{

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//カラー変更
	void changeColor(GASHA_ consoleColor&& color) override;
	inline void changeColor(const GASHA_ consoleColor& color){}//なにもしない
public:
	//コンストラクタ
	inline monoWinConsole(std::FILE* handle, const char* name = "Mono-Win-console");
	//デストラクタ
	~monoWinConsole() override;

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//カラー変更
	inline void changeColor(GASHA_ consoleColor&& color){}
	inline void changeColor(const GASHA_ consoleColor& color){}
public:
	inline monoWinConsole(std::FILE* handle, const char* name = nullptr) ://コンストラクタ
		winConsole(handle, name)
	{}
	inline ~monoWinConsole(){}//デストラクタ

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
};

#else//GASHA_USE_WINDOWS_CONSOLE
//Windowsコマンドプロンプトクラス無効時

//TTY端末に委譲
class winConsole : public ttyConsole
{
public:
	//コンストラクタ
	inline winConsole(std::FILE* handle, const char* name = "TTY-console") :
		ttyConsole(handle, name)
	{}
};

//TTY端末に委譲
class monoWinConsole : public monoTtyConsole
{
public:
	//コンストラクタ
	inline monoWinConsole(std::FILE* handle, const char* name = "Mono-TTY-console") :
		monoTtyConsole(handle, name)
	{}
};

#endif//GASHA_USE_WINDOWS_CONSOLE

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/win_console.inl>

#endif//GASHA_INCLUDED_WIN_CONSOLE_H

// End of file

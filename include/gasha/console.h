#pragma once
#ifndef GASHA_INCLUDED_CONSOLE_H
#define GASHA_INCLUDED_CONSOLE_H

//--------------------------------------------------------------------------------
// console.h
// コンソール【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <stdio.h>//FILE, stdout, stderr

#ifdef GASHA_USE_WINDOWS_CONSOLE
#include <Windows.h>//HANDLE, GetConsoleScreenBufferInfo()
#endif//GASHA_USE_WINDOWS_CONSOLE

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//コンソール
//--------------------------------------------------------------------------------

//----------------------------------------
//コンソールクラス
class console
{
public:
	//種別
	struct TTY_t{};//TTY端末
	struct WIN_t{};//Windowsコマンドプロンプト
	struct VS_t{};//Visual Studioの出力ウインドウ
public:
	//定数
	//コンソール種別
	enum console_type
	{
		TTY,//TTY端末
		WIN,//Windowsコマンドプロンプト
		VS,//Visual Studioの出力Window
	};
public:
	//アクセッサ
	inline console_type type() const { return m_type; }//コンソール種別
	inline FILE* handleTTY() const { return m_handleTTY; }//ハンドル：TTY端末用
#ifdef GASHA_USE_WINDOWS_CONSOLE
	inline HANDLE handleWin() const { return m_handleWin; }//ハンドル：Windowsコマンドプロンプト用
	inline const CONSOLE_SCREEN_BUFFER_INFO& screenBuffer() const { return m_screenBuffer; }//Windowsコマンドプロンプト用スクリーンバッファ
#endif//GASHA_USE_WINDOWS_CONSOLE

public:
	//判定オペレータ
	inline const bool operator==(const console& rhs) const { return m_type == rhs.m_type; }
	inline const bool operator!=(const console& rhs) const { return m_type != rhs.m_type; }
	inline const bool operator==(const console_type rhs) const { return m_type == rhs; }
	inline const bool operator!=(const console_type rhs) const { return m_type != rhs; }
	inline const bool operator==(const TTY_t&) const { return m_type == TTY; }
	inline const bool operator!=(const TTY_t&) const { return m_type != TTY; }
	inline const bool operator==(const WIN_t&) const { return m_type == WIN; }
	inline const bool operator!=(const WIN_t&) const { return m_type != WIN; }
	inline const bool operator==(const VS_t&) const { return m_type == VS; }
	inline const bool operator!=(const VS_t&) const { return m_type != VS; }

public:
	//メソッド
	
	//プリント
	template<typename... Tx>
	inline int printf(const char* fmt, Tx&&... args) const;

public:
	//コンストラクタ
	inline console(const TTY_t&, FILE* handle);
	console(const WIN_t&, FILE* handle);
	inline console(const VS_t&);
	//デストラクタ
	inline ~console();
private:
	//フィールド
	const console_type m_type;//コンソール種別
	FILE* m_handleTTY;//TTY用ハンドル
#ifdef GASHA_USE_WINDOWS_CONSOLE
	HANDLE m_handleWin;//Windowsコマンドプロンプト用ハンドル
	CONSOLE_SCREEN_BUFFER_INFO m_screenBuffer;//Windowsコマンドプロンプト用スクリーンバッファ
#endif//GASHA_USE_WINDOWS_CONSOLE
public:
};

//定数
extern const console::TTY_t console_TTY;//TTY端末
extern const console::WIN_t console_WIN;//Windowsコマンドプロンプト
extern const console::VS_t console_VS;//Visual Studioの出力ウインドウ

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/console.inl>

#endif//GASHA_INCLUDED_CONSOLE_H

// End of file

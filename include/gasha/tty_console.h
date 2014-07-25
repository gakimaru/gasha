#pragma once
#ifndef GASHA_INCLUDED_TTY_CONSOLE_H
#define GASHA_INCLUDED_TTY_CONSOLE_H

//--------------------------------------------------------------------------------
// tty_console.h
// TTY端末【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース
#include <gasha/console_color.h>//コンソールカラー

#include <cstdio>//FILE, stdout, stderr

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//TTY端末
//--------------------------------------------------------------------------------
//【注意】UTF-16, UTF-32（通常文字の文字コードに'\0'が含まれるもの）には非対応
//【注意】文字列中の改行コード変換は行わない（'\r', '\n' はそのまま出力する）
//--------------------------------------------------------------------------------

//----------------------------------------
//TTY端末クラス
class ttyConsole : public GASHA_ iConsole
{
#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//アクセッサ
	const char* name() const override { return m_name; }
	inline std::FILE* handle() const { return m_handle; }//ハンドル

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
	bool isSame(const iConsole* rhs) const override;

public:
	//コンストラクタ
	inline ttyConsole(std::FILE* handle, const char* name = "TTY-console");
	//デストラクタ
	~ttyConsole() override;

protected:
	//フィールド
	const char* m_name;//名前
	std::FILE* m_handle;//ハンドル
	consoleColor m_currColor;//現在のカラー

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//アクセッサ
	inline const char* name() const { return ""; }
	inline std::FILE* handle() const { return nullptr; }//ハンドル
public:
	//メソッド
	inline void begin(){}//出力開始
	inline void end(){}//出力終了
	inline void put(const char* str){}//出力
	template<typename... Tx>
	inline int printf(const char* fmt, Tx&&... args){ return 0; }//書式付き出力
	inline void putCr(){}//改行出力
	inline void changeColor(GASHA_ consoleColor&& color){}//カラー変更
	inline void changeColor(const GASHA_ consoleColor& color){}//カラー変更
	inline void resetColor(){}//カラーリセット
	inline bool isSame(const iConsole* rhs) const{ return true; }//出力先が同じか判定
public:
	inline ttyConsole(std::FILE* handle, const char* name = nullptr){}//コンストラクタ
	inline ~ttyConsole(){}//デストラクタ

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
};

//----------------------------------------
//カラー出力なしTTY端末クラス
class monoTtyConsole : public ttyConsole
{

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//カラー変更
	void changeColor(GASHA_ consoleColor&& color) override;
	inline void changeColor(const GASHA_ consoleColor& color){}//なにもしない
public:
	//コンストラクタ
	inline monoTtyConsole(std::FILE* handle, const char* name = "Mono-TTY-console");
	//デストラクタ
	~monoTtyConsole() override;

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//カラー変更
	inline void changeColor(GASHA_ consoleColor&& color){}
	inline void changeColor(const GASHA_ consoleColor& color){}
public:
	inline monoTtyConsole(std::FILE* handle, const char* name = nullptr) ://コンストラクタ
		ttyConsole(handle, name)
	{}
	inline ~monoTtyConsole(){}//デストラクタ

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/tty_console.inl>

#endif//GASHA_INCLUDED_TTY_CONSOLE_H

// End of file

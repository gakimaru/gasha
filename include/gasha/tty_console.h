#pragma once
#ifndef GASHA_INCLUDED_TTY_CONSOLE_H
#define GASHA_INCLUDED_TTY_CONSOLE_H

//--------------------------------------------------------------------------------
// tty_console.h
// TTY端末【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース

#include <cstdio>//FILE, stdout, stderr

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//TTY端末
//--------------------------------------------------------------------------------
//【注意】UTF-16, UTF-32（通常文字の文字コードに'\0'が含まれるもの）には非対応
//【注意】文字列中の改行コード変換は行わない（'\r', '\n' はそのまま出力する）
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//TTY端末クラス
class ttyConsole : public GASHA_ IConsole
{
public:
	//アクセッサ
	const char* name() const override { return m_name; }
	inline std::FILE* handle() const { return m_handle; }//ハンドル

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
	void changeColor(GASHA_ consoleColor&& color) override;
	inline void changeColor(const GASHA_ consoleColor& color);
	
	//カラーリセット
	void resetColor() override;

	//出力先が同じか判定
	bool isSame(const IConsole* rhs) const override;

public:
	//コンストラクタ
	inline ttyConsole(std::FILE* handle, const char* name = "TTY-console");
	//デストラクタ
	~ttyConsole() override;

private:
	//フィールド
	const char* m_name;//名前
	std::FILE* m_handle;//ハンドル
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/tty_console.inl>

#endif//GASHA_INCLUDED_TTY_CONSOLE_H

// End of file

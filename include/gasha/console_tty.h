#pragma once
#ifndef GASHA_INCLUDED_CONSOLE_TTY_H
#define GASHA_INCLUDED_CONSOLE_TTY_H

//--------------------------------------------------------------------------------
// console_tty.h
// TTY端末【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース

#include <stdio.h>//FILE, stdout, stderr

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//TTY端末
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//TTY端末クラス
class consoleTTY : public GASHA_ IConsole
{
public:
	//アクセッサ
	const char* name() const override { return "TTY"; }
	inline FILE* handle() const { return m_handle; }//ハンドル

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
	inline consoleTTY(FILE* handle);
	//デストラクタ
	~consoleTTY() override;
private:
	//フィールド
	FILE* m_handle;//ハンドル
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/console_tty.inl>

#endif//GASHA_INCLUDED_CONSOLE_TTY_H

// End of file

#pragma once
#ifndef GASHA_INCLUDED_DUMMY_CONSOLE_H
#define GASHA_INCLUDED_DUMMY_CONSOLE_H

//--------------------------------------------------------------------------------
// dummy_console.h
// ダミーコンソール【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ダミーコンソール
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ダミーコンソールクラス

class dummyConsole : public GASHA_ IConsole
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

	//出力先が同じか判定
	bool isSame(const IConsole* rhs) const override;

public:
	//コンストラクタ
	inline dummyConsole(const char* name = "Dummy-console");
	//デストラクタ
	~dummyConsole() override;

private:
	//フィールド
	const char* m_name;//名前
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/dummy_console.inl>

#endif//GASHA_INCLUDED_DUMMY_CONSOLE_H

// End of file

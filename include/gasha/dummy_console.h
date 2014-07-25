#pragma once
#ifndef GASHA_INCLUDED_DUMMY_CONSOLE_H
#define GASHA_INCLUDED_DUMMY_CONSOLE_H

//--------------------------------------------------------------------------------
// dummy_console.h
// ダミーコンソール【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ダミーコンソール
//--------------------------------------------------------------------------------

//----------------------------------------
//ダミーコンソールクラス
class dummyConsole : public GASHA_ iConsole
{
#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

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
	inline dummyConsole(const char* name = "Dummy-console");
	//デストラクタ
	~dummyConsole() override;

private:
	//フィールド
	const char* m_name;//名前

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

public:
	//アクセッサ
	inline const char* name() const{ return ""; }
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
	inline dummyConsole(const char* name = nullptr){}//コンストラクタ
	inline ~dummyConsole(){}//デストラクタ

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/dummy_console.inl>

#endif//GASHA_INCLUDED_DUMMY_CONSOLE_H

// End of file

#pragma once
#ifndef GASHA_INCLUDED_CONSOLE_COLOR_H
#define GASHA_INCLUDED_CONSOLE_COLOR_H

//--------------------------------------------------------------------------------
// console_color.h
// コンソールカラー【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/console.h>//コンソール

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//コンソールカラー
//--------------------------------------------------------------------------------

//----------------------------------------
//コンソールカラークラス
class consoleColor
{
public:
	//定数
	//カラー値
	enum color_t : unsigned char
	{
		standard = 0x10,//標準カラー

		R = 0x01,//Red:赤
		G = 0x02,//Green:緑
		B = 0x04,//Blue:青
		I = 0x08,//high Intensity:鮮やか

		RGB = 0x07,//RGBマスク
		RGBI = 0x0f,//RGB+Iマスク

		//カラー
		black = 0,//黒
		blue = B,//青
		red = R,//赤
		magenta = R | B,//紫
		green = G,//緑
		cyan = G | B,//水
		yellow = G | R,//黄
		white = G | R | B,//白

		//鮮やかなカラー
		iBlack = I | black,//黒
		iBlue = I | blue,//青
		iRed = I | red,//赤
		iMagenta = I | magenta,//紫
		iGreen = I | green,//緑
		iCyan = I | cyan,//水
		iYellow = I | yellow ,//黄
		iWhite = I | white,//白
	};
public:
	//アクセッサ
	inline color_t fore() const { return m_fore; }//前景色
	inline color_t& fore(){ return m_fore; }//前景色
	inline color_t back() const { return m_back; }//背景色
	inline color_t& back(){ return m_back; }//背景色
	inline bool bold() const { return m_bold; }//強調
	inline bool& bold(){ return m_bold; }//強調
	inline bool underline() const { return m_underline; }//アンダーライン
	inline bool& underline(){ return m_underline; }//アンダーライン

public:
	//メソッド
	
	//カラー変更
	inline void changeColor();//カラー変更
	inline void resetColor();//カラーリセット
	inline void changeColor(const color_t fore, const color_t back = standard, const bool bold = false, const bool underline = false);//カラー変更
private:
	void changeColorTTY(const color_t fore, const color_t back, const bool bold, const bool underline);//TTY用のカラー変更
	void changeColorWIN(const color_t fore, const color_t back, const bool bold, const bool underline);//Windowsコマンドプロンプト用のカラー変更
	void changeColorVS(const color_t fore, const color_t back, const bool bold, const bool underline);//Visual Studio 出力ウインドウ用のカラー変更

public:
	//ムーブオペレータ
	inline consoleColor& operator=(consoleColor&& rhs);
	//コピーオペレータ
	inline consoleColor& operator=(const consoleColor& rhs);
public:
	//ムーブコンストラクタ
	inline consoleColor(consoleColor&& obj);
	//コピーコンストラクタ
	inline consoleColor(const consoleColor& obj) ;
	//デフォルトコンストラクタ
	inline consoleColor();
	//コンストラクタ
	inline consoleColor(const GASHA_ console& _console, const color_t fore = standard, const color_t back = standard, const bool bold = false, const bool underline = false);
	//デストラクタ
	inline ~consoleColor();
private:
	//フィールド
	const GASHA_ console* m_console;//コンソール
	color_t m_fore;//前景色
	color_t m_back;//背景色
	bool m_bold;//強調
	bool m_underline;//アンダーライン
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/console_color.inl>

#endif//GASHA_INCLUDED_CONSOLE_COLOR_H

// End of file

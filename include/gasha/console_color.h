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

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//コンソールカラー
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//コンソールカラーリセット用構造体
struct stdConsoleColor_t
{};
extern const stdConsoleColor_t stdConsoleColor;

//----------------------------------------
//コンソールカラークラス
class consoleColor
{
public:
	//定数
	//カラー値
	enum color_t : unsigned char
	{
		STANDARD = 0x10,//標準カラー

		R = 0x01,//Red:赤
		G = 0x02,//Green:緑
		B = 0x04,//Blue:青
		I = 0x08,//high Intensity:鮮やか

		RGB = 0x07,//RGBマスク
		RGBI = 0x0f,//RGB+Iマスク

		//カラー
		BLACK = 0,//黒
		BLUE = B,//青
		RED = R,//赤
		MAGENTA = R | B,//紫
		GREEN = G,//緑
		CYAN= G | B,//水
		YELLOW = G | R,//黄
		WHITE = G | R | B,//白

		//鮮やかなカラー
		iBLACK = I | BLACK,//黒
		iBLUE = I | BLUE,//青
		iRED = I | RED,//赤
		iMAGENTA = I | MAGENTA,//紫
		iGREEN = I | GREEN,//緑
		iCYAN = I | CYAN,//水
		iYELLOW = I | YELLOW,//黄
		iWHITE = I | WHITE,//白
	};
	
	//属性
	enum attr_t : unsigned char
	{
		NOATTR = 0x00,//属性なし

		BOLD = 0x01,//強調 ※エスケープシーケンス時のみ対応
		UNDERLINE = 0x02,//下線 ※エスケープシーケンス時のみ対応
		REVERSE = 0x04,//反転 ※エスケープシーケンス時のみ対応

		BOLD_UNDERLINE = BOLD | UNDERLINE,//強調＋下線
		BOLD_REVERSE = BOLD | REVERSE,//強調＋反転
		UNDERLINE_REVERSE = UNDERLINE | REVERSE,//下線＋反転
		BOLD_UNDERLINE_REVERSE = BOLD | UNDERLINE | REVERSE,//強調＋下線＋反転
	};

public:
	//アクセッサ
	inline bool isStandard() const;//標準カラーか？
	inline color_t fore() const { return m_fore; }//前景色
	inline color_t& fore(){ return m_fore; }//前景色
	inline color_t back() const { return m_back; }//背景色
	inline color_t& back(){ return m_back; }//背景色
	inline attr_t attr() const { return m_attr; }//属性

public:
	//文字列作成
	const char* toStr(const color_t color) const;
	const char* toStr(char* str) const;

public:
	//初期状態にする
	inline void reset();

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
	//コンストラクタ
	inline consoleColor(const color_t fore, const color_t back = STANDARD, const attr_t attr = NOATTR);
	//リセット用コンストラクタ
	inline consoleColor(const stdConsoleColor_t&);
	//デフォルトコンストラクタ
	inline consoleColor();
	//デストラクタ
	inline ~consoleColor();
private:
	//フィールド
	color_t m_fore;//前景色
	color_t m_back;//背景色
	attr_t m_attr;//属性
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/console_color.inl>

#endif//GASHA_INCLUDED_CONSOLE_COLOR_H

// End of file

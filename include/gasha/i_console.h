#pragma once
#ifndef GASHA_INCLUDED_I_CONSOLE_H
#define GASHA_INCLUDED_I_CONSOLE_H

//--------------------------------------------------------------------------------
// i_console.h
// コンソールインターフェース【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/console_color.h>//コンソールカラー

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//コンソールインターフェース
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//コンソールインターフェースクラス
class IConsole
{
public:
	//アクセッサ
	virtual const char* name() const = 0;

public:
	//判定オペレータ
	inline bool operator==(IConsole&& rhs) const { return isSame(&rhs); }
	inline bool operator==(const IConsole& rhs) const { return isSame(&rhs); }
	inline bool operator==(const IConsole* rhs) const { return isSame(rhs); }
	inline bool operator!=(IConsole&& rhs) const { return !isSame(&rhs); }
	inline bool operator!=(const IConsole& rhs) const { return !isSame(&rhs); }
	inline bool operator!=(const IConsole* rhs) const { return !isSame(rhs); }

public:
	//メソッド

	//出力開始
	virtual void beginOutput() = 0;

	//出力終了
	//※フラッシュ可能な状態
	virtual void endOutput() = 0;

	//出力
	virtual void output(const char* str) = 0;

	//書式付き出力
	//※十分なサイズのワークバッファを受け渡す必要あり
	template<typename... Tx>
	inline int printf(char* work_buff, const char* fmt, Tx&&... args);
	template<typename... Tx>
	inline int printf(char* work_buff, const std::size_t work_buff_size, const char* fmt, Tx&&... args);

	//改行出力
	//※改行前にカラーリセットも行う
	inline void outputCr(const bool with_reset_color = true);

	//カラー変更
	virtual void changeColor(GASHA_ consoleColor&& color) = 0;
	inline void changeColor(const GASHA_ consoleColor& color);

	//カラーリセット
	virtual void resetColor() = 0;

	//出力先が同じか判定
	virtual bool isSame(const IConsole* rhs) const = 0;

public:
	//デストラクタ
	virtual ~IConsole()
	{}
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/i_console.inl>

#endif//GASHA_INCLUDED_I_CONSOLE_H

// End of file

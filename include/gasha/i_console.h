#pragma once
#ifndef GASHA_INCLUDED_I_CONSOLE_H
#define GASHA_INCLUDED_I_CONSOLE_H

//--------------------------------------------------------------------------------
// i_console.h
// コンソールインターフェース【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
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

//----------------------------------------
//コンソールインターフェースクラス
class iConsole
{
#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//アクセッサ
	virtual const char* name() const = 0;

public:
	//判定オペレータ
	inline bool operator==(iConsole&& rhs) const { return isSame(&rhs); }
	inline bool operator==(const iConsole& rhs) const { return isSame(&rhs); }
	inline bool operator==(const iConsole* rhs) const { return isSame(rhs); }
	inline bool operator!=(iConsole&& rhs) const { return !isSame(&rhs); }
	inline bool operator!=(const iConsole& rhs) const { return !isSame(&rhs); }
	inline bool operator!=(const iConsole* rhs) const { return !isSame(rhs); }

public:
	//メソッド

	//出力開始
	virtual void begin() = 0;

	//出力終了
	//※フラッシュ可能な状態
	virtual void end() = 0;

	//出力
	virtual void put(const char* str) = 0;

	//改行出力
	//※改行前にカラーのリセットも行う
	virtual void putCr() = 0;

	//書式付き出力
	//※十分なサイズのワークバッファを受け渡す必要あり
	template<typename... Tx>
	inline int printf(char* work_buff, const char* fmt, Tx&&... args);
	template<typename... Tx>
	inline int printf(char* work_buff, const std::size_t work_buff_size, const char* fmt, Tx&&... args);

	//カラー変更
	virtual void changeColor(GASHA_ consoleColor&& color) = 0;
	inline void changeColor(const GASHA_ consoleColor& color);

	//カラーリセット
	virtual void resetColor() = 0;

	//出力先が同じか判定
	virtual bool isSame(const iConsole* rhs) const = 0;

public:
	//デストラクタ
	virtual ~iConsole()
	{}

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//アクセッサ
	inline const char* name() const { return ""; }
public:
	//判定オペレータ
	inline bool operator==(iConsole&& rhs) const { return true; }
	inline bool operator==(const iConsole& rhs) const { return true; }
	inline bool operator==(const iConsole* rhs) const { return true; }
	inline bool operator!=(iConsole&& rhs) const { return false; }
	inline bool operator!=(const iConsole& rhs) const { return false; }
	inline bool operator!=(const iConsole* rhs) const { return false; }
public:
	//メソッド
	inline void begin(){}//出力開始
	inline void end(){}//出力終了
	inline void put(const char* str){}//出力
	inline void putCr(){}//改行出力
	template<typename... Tx>
	inline int printf(char* work_buff, const char* fmt, Tx&&... args){ return 0; }//書式付き出力
	template<typename... Tx>
	inline int printf(char* work_buff, const std::size_t work_buff_size, const char* fmt, Tx&&... args){ return 0; }//書式付き出力
	inline void changeColor(GASHA_ consoleColor&& color){}//カラー変更
	inline void changeColor(const GASHA_ consoleColor& color){}//カラー変更
	inline void resetColor(){}//カラーリセット
	inline bool isSame(const iConsole* rhs) const{ return true; }//出力先が同じか判定
public:
	inline ~iConsole(){}//デストラクタ

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/i_console.inl>

#endif//GASHA_INCLUDED_I_CONSOLE_H

// End of file

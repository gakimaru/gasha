#pragma once
#ifndef GASHA_INCLUDED_STD_CONSOLE_H
#define GASHA_INCLUDED_STD_CONSOLE_H

//--------------------------------------------------------------------------------
// std_console.h
// 標準コンソール【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース

#ifdef GASHA_USE_WINDOWS_CONSOLE
#include <gasha/win_console.h>//Windowsコマンドプロンプト
#else//GASHA_USE_WINDOWS_CONSOLE
#include <gasha/tty_console.h>//TTY端末
#endif//GASHA_USE_WINDOWS_CONSOLE
#include <gasha/mem_console.h>//メモリコンソール
#include <gasha/dummy_console.h>//ダミーコンソール

#include <cstdio>//stdout, stderr

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準コンソール
//--------------------------------------------------------------------------------

#ifdef GASHA_USE_WINDOWS_CONSOLE

//----------------------------------------
//標準出力コンソール
//※Windowsコマンドプロンプトに委譲
class stdOutConsole : public GASHA_ winConsole
{
public:
	//静的アクセッサ
	inline static stdOutConsole& instance(){ return m_instance; }

public:
	//コンストラクタ
	inline stdOutConsole() :
		winConsole(stdout, "Win(stdout)")
	{}

private:
	//静的フィールド
	static stdOutConsole m_instance;
};

//----------------------------------------
//標準エラーコンソール
//※Windowsコマンドプロンプトに委譲
class stdErrConsole : public GASHA_ winConsole
{
public:
	//静的アクセッサ
	inline static stdErrConsole& instance(){ return m_instance; }

public:
	//コンストラクタ
	inline stdErrConsole() :
		winConsole(stderr, "Win(stderr)")
	{}

private:
	//静的フィールド
	static stdErrConsole m_instance;
};

//----------------------------------------
//画面通知用標準コンソール
//※内部でWindowsコマンドプロンプトを使用（そのため、標準エラーのWindowsコマンドプロンプトと比較判定すると一致することに注意）
//※改行までしか出力しない。
class stdNoticeConsole : public GASHA_ winConsole
{
public:
	//静的アクセッサ
	inline static stdNoticeConsole& instance(){ return m_instance; }

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

public:
	//出力開始
	void begin() override;
	//出力終了
	void end() override;
	//出力
	void put(const char* str) override;
	//改行出力
	void putCr() override;
	//カラー変更
	void changeColor(GASHA_ consoleColor&& color) override;
public:
	//コンストラクタ
	inline stdNoticeConsole() :
		winConsole(stderr, "Win(stderr)"),
		m_isAlreadyCr(false)
	{}
private:
	bool m_isAlreadyCr;//改行出力済み

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

public:
	//コンストラクタ
	inline stdNoticeConsole() :
		winConsole(stderr, "Win(stderr)")
	{}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

private:
	//静的フィールド
	static stdNoticeConsole m_instance;
};

#else//GASHA_USE_WINDOWS_CONSOLE

//----------------------------------------
//標準出力コンソール
//※TTY端末に委譲
class stdOutConsole : public GASHA_ ttyConsole
{
public:
	//静的アクセッサ
	inline static stdOutConsole& instance(){ return m_instance; }

public:
	//コンストラクタ
	inline stdOutConsole():
		ttyConsole(stdout, "TTY(stdout)")
	{}

private:
	//静的フィールド
	static stdOutConsole m_instance;
};

//----------------------------------------
//標準エラーコンソール
//※TTY端末に委譲
class stdErrConsole : public GASHA_ ttyConsole
{
public:
	//静的アクセッサ
	inline static stdErrConsole& instance(){ return m_instance; }

public:
	//コンストラクタ
	inline stdErrConsole():
		ttyConsole(stderr, "TTY(stderr)")
	{}

private:
	//静的フィールド
	static stdErrConsole m_instance;
};

//----------------------------------------
//画面通知用標準コンソール
//※内部でTTY端末を使用（そのため、標準エラーのTTY端末と比較判定すると一致することに注意）
//※改行までしか出力しない。
class stdNoticeConsole : public GASHA_ ttyConsole
{
public:
	//静的アクセッサ
	inline static stdNoticeConsole& instance(){ return m_instance; }

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

public:
	//出力開始
	void begin() override;
	//出力終了
	void end() override;
	//出力
	void put(const char* str) override;
	//改行出力
	void putCr() override;
	//カラー変更
	void changeColor(GASHA_ consoleColor&& color) override;
public:
	//コンストラクタ
	inline stdNoticeConsole():
		ttyConsole(stderr, "TTY(stderr)"),
		m_isAlreadyCr(false)
	{}

private:
	bool m_isAlreadyCr;//改行出力済み

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

public:
	//コンストラクタ
	inline stdNoticeConsole():
		ttyConsole(stderr, "TTY(stderr)")
	{}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

private:
	//静的フィールド
	static stdNoticeConsole m_instance;
};

#endif//GASHA_USE_WINDOWS_CONSOLE

//----------------------------------------
//標準メモリコンソール
//※ビルド設定で設定されたバッファサイズ、かつ、ロック制御なしで設定する
class stdMemConsole : public GASHA_ memConsole<GASHA_STD_MEM_CONSOLE_BUFF_SIZE>
{
public:
	//静的アクセッサ
	inline static stdMemConsole& instance(){ return m_instance; }

public:
	//コンストラクタ
	inline stdMemConsole() :
		memConsole("Memory")
	{}

private:
	//静的フィールド
	static stdMemConsole m_instance;
};

//----------------------------------------
//標準ダミーコンソール
class stdDummyConsole : public GASHA_ dummyConsole
{
public:
	//静的アクセッサ
	inline static stdDummyConsole& instance(){ return m_instance; }

public:
	//コンストラクタ
	inline stdDummyConsole() :
		dummyConsole("Dummy")
	{}

private:
	//静的フィールド
	static stdDummyConsole m_instance;
};

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_STD_CONSOLE_H

// End of file

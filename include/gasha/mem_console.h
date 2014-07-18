#pragma once
#ifndef GASHA_INCLUDED_MEM_CONSOLE_H
#define GASHA_INCLUDED_MEM_CONSOLE_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// mem_console.h
// メモリコンソール【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース

#include <gasha/ring_buffer.h>//リングバッファ【宣言部】
#include <gasha/dummy_shared_lock.h>//ダミー共有ロック

#include <cstddef>//std::size_t
#include <cstdio>//FILE

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//メモリコンソール
//--------------------------------------------------------------------------------
//【注意】UTF-16, UTF-32（通常文字の文字コードに'\0'が含まれるもの）には非対応
//【注意】文字列中の改行コード変換は行わない（'\r', '\n' はそのまま出力する）
//--------------------------------------------------------------------------------

//----------------------------------------
//メモリコンソールクラス

template<std::size_t _BUFF_SIZE, class LOCK_TYPE = GASHA_ dummySharedLock>
class memConsole : public GASHA_ IConsole
{
public:
	//型
	typedef LOCK_TYPE lock_type;//ロック型
	typedef int block_type;//ブロック型
public:
	//定数
	static const std::size_t BUFF_SIZE = _BUFF_SIZE;//バッファサイズ
	static const std::size_t BLOCK_SIZE = sizeof(block_type);//ブロックサイズ
	static const std::size_t POOL_SIZE = BUFF_SIZE / BLOCK_SIZE;//プールサイズ
	static_assert(BUFF_SIZE >= BLOCK_SIZE, "BUFF_SIZE is too small.");
	static_assert(BUFF_SIZE % BLOCK_SIZE == 0, "BUFF_SIZE is not a multiple of BLOCK_SIZE(sizeof(int)).");

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//リングバッファ操作型
	struct buffOpe : public GASHA_ ring_buffer::baseOpe<buffOpe, block_type>
	{
		typedef typename memConsole::lock_type lock_type;//ロック型
	};
	//リングバッファ型
	typedef GASHA_ ring_buffer::container<buffOpe> ring_buff_type;

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
	bool isSame(const IConsole* rhs) const override;

	//バッファをクリア
	void clear();

	//現在バッファリングされているサイズを取得
	inline std::size_t size();

	//バッファをコピー
	//※コピー先のバッファサイズを渡す。
	//※コピーしたサイズを返す。（終端の分はサイズに含まない）
	//※コピー先のバッファサイズが足りない場合、コピーしたサイズ（コピー先のバッファサイズと同じ）を返す。
	//※コピー先のバッファサイズが足りなくても終端は書き込む。
	std::size_t copy(char* dst, const std::size_t max_size);

	//バッファの内容を画面出力に出力
	//※出力先のデフォルトは標準出力。
	//※標準エラーなどに変更することも可能。
	void printScreen(std::FILE* fp = stdout);

public:
	//コンストラクタ
	inline memConsole(const char* name = "Mem-console");
	//デストラクタ
	~memConsole() override;

private:
	//フィールド
	const char* m_name;//名前
	ring_buff_type m_ringBuff;//リングバッファ
	std::size_t m_posInBlock;//ブロック内のコピー位置
	block_type m_mem[POOL_SIZE];//リングバッファに渡すバッファ部 ※直接操作しない

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

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
	inline bool isSame(const IConsole* rhs) const{ return true; }//出力先が同じか判定
	inline void clear(){}//バッファをクリア
	inline std::size_t size(){ return 0; }//現在バッファリングされているサイズを取得
	inline std::size_t copy(char* dst, const std::size_t max_size){ return 0; }//バッファをコピー
	inline void printScreen(std::FILE* fp = stdout){}//バッファの内容を画面出力に出力
public:
	inline memConsole(const char* name = nullptr){}//コンストラクタ
	inline ~memConsole(){}//デストラクタ

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/mem_console.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_MEM_CONSOLE_ALLWAYS_TOGETHER_CPP_H
#include <gasha/mem_console.cpp.h>
#endif//GASHA_MEM_CONSOLE_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_MEM_CONSOLE_H

// End of file

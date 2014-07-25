#pragma once
#ifndef GASHA_INCLUDED_MEM_CONSOLE_INL
#define GASHA_INCLUDED_MEM_CONSOLE_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// mem_console.inl
// メモリコンソール【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/mem_console.h>//メモリコンソール【宣言部】

#include <gasha/ring_buffer.inl>//リングバッファ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//メモリコンソール
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//メモリコンソールクラス

//改行出力
template<std::size_t _BUFF_SIZE, class LOCK_TYPE>
inline void memConsole<_BUFF_SIZE, LOCK_TYPE>::putCr()
{
	put("\n");
}

//カラー変更
template<std::size_t _BUFF_SIZE, class LOCK_TYPE>
inline void memConsole<_BUFF_SIZE, LOCK_TYPE>::changeColor(const GASHA_ consoleColor& color)
{
	//なにもしない
}

//現在バッファリングされているサイズを取得
template<std::size_t _BUFF_SIZE, class LOCK_TYPE>
inline std::size_t memConsole<_BUFF_SIZE, LOCK_TYPE>::size()
{
	std::size_t size = m_ringBuff.size();
	if (size == 0)
		return 0;
	return size * BLOCK_SIZE - (BLOCK_SIZE - (m_posInBlock + 1));
}

//コンストラクタ
template<std::size_t _BUFF_SIZE, class LOCK_TYPE>
inline memConsole<_BUFF_SIZE, LOCK_TYPE>::memConsole(const char* name) :
	m_name(name),
	m_ringBuff(m_mem),
	m_posInBlock(0)
{}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_MEM_CONSOLE_INL

// End of file

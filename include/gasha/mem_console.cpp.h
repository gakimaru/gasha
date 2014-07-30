#pragma once
#ifndef GASHA_INCLUDED_MEM_CONSOLE_CPP_H
#define GASHA_INCLUDED_MEM_CONSOLE_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// mem_console.cpp.h
// メモリコンソール【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/mem_console.inl>//メモリコンソール【インライン関数／テンプレート関数定義部】

#include <gasha/ring_buffer.cpp.h>//リングバッファ【関数／実体定義部】

#include <gasha/fast_string.h>//高速文字列処理：strlen_fast()
#include <gasha/type_traits.h>//型特性ユーティリティ：toStr()
#include <gasha/utility.h>//汎用ユーティリティ：min()

#include <cstring>//std::memcpy()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//メモリコンソール
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//メモリコンソールクラス

//出力開始
template<std::size_t _BUFF_SIZE, class LOCK_POLICY>
void memConsole<_BUFF_SIZE, LOCK_POLICY>::begin()
{
	//何もしない
}

//出力終了
template<std::size_t _BUFF_SIZE, class LOCK_POLICY>
void memConsole<_BUFF_SIZE, LOCK_POLICY>::end()
{
	//何もしない
}

//出力
template<std::size_t _BUFF_SIZE, class LOCK_POLICY>
void memConsole<_BUFF_SIZE, LOCK_POLICY>::put(const char* str)
{
	auto lock = m_ringBuff.lockScoped();//スコープロック
	if (!str)
		str = GASHA_ toStr(nullptr);
	std::size_t remain = GASHA_ strlen_fast(str) + 1;
	const char* p = str;
	{
		block_type* block = m_ringBuff.refBack();
		if (block)
		{
			const std::size_t block_remain = BLOCK_SIZE - m_posInBlock;
			const std::size_t copy_size = GASHA_ min(remain, block_remain);
			std::memcpy(reinterpret_cast<char*>(block) + m_posInBlock, p, copy_size);
			p += copy_size;
			remain -= copy_size;
			if (remain == 0)
				m_posInBlock += (copy_size - 1);
			else
				m_posInBlock = 0;
		}
		else
			m_posInBlock = 0;
	}
	while (remain > 0)
	{
		block_type block;
		const std::size_t copy_size = GASHA_ min(remain, BLOCK_SIZE);
		std::memcpy(reinterpret_cast<char*>(&block), p, copy_size);
		p += copy_size;
		remain -= copy_size;
		if (remain == 0)
			m_posInBlock = copy_size - 1;
		block_type* assigned = m_ringBuff.push_back(block);
		if (!assigned)
		{
			m_ringBuff.pop_front();
			m_ringBuff.push_back(block);
		}
	}
}

//カラー変更
template<std::size_t _BUFF_SIZE, class LOCK_POLICY>
void memConsole<_BUFF_SIZE, LOCK_POLICY>::changeColor(GASHA_ consoleColor&& color)
{
	//何もしない
}

//カラーリセット
template<std::size_t _BUFF_SIZE, class LOCK_POLICY>
void memConsole<_BUFF_SIZE, LOCK_POLICY>::resetColor()
{
	//何もしない
}

//出力先が同じか判定
template<std::size_t _BUFF_SIZE, class LOCK_POLICY>
bool memConsole<_BUFF_SIZE, LOCK_POLICY>::isSame(const iConsole* rhs) const
{
	const memConsole* _rhs = dynamic_cast<const memConsole*>(rhs);
	if (!_rhs)
		return false;
	return true;
}

//バッファをクリア
template<std::size_t _BUFF_SIZE, class LOCK_POLICY>
void memConsole<_BUFF_SIZE, LOCK_POLICY>::clear()
{
	auto lock = m_ringBuff.lockScoped();//スコープロック
	m_ringBuff.clear();
	m_posInBlock = 0;
}

//バッファをコピー
template<std::size_t _BUFF_SIZE, class LOCK_POLICY>
std::size_t memConsole<_BUFF_SIZE, LOCK_POLICY>::copy(char* dst, const std::size_t max_size)
{
	if (max_size == 0)
		return 0;
	auto lock = m_ringBuff.lockSharedScoped();//スコープ共有ロック
	std::size_t src_remain = size();
	if (src_remain == 0)
	{
		dst[0] = '\0';
		return 0;
	}
	std::size_t dst_remain = max_size - 1;
	std::size_t copy_size = 0;
	char* p = dst;
	int index = 0;
	while(src_remain > 0 && dst_remain > 0)
	{
		const block_type* block = m_ringBuff.at(index);
		if (!block)
			break;
		std::size_t block_copy_size = GASHA_ min(src_remain, dst_remain, BLOCK_SIZE);
		std::memcpy(p, reinterpret_cast<const char*>(block), block_copy_size);
		src_remain -= block_copy_size;
		dst_remain -= block_copy_size;
		p += block_copy_size;
		copy_size += block_copy_size;
		++index;
	}
	if (copy_size == 0)
	{
		dst[0] = '\0';
		return 0;
	}
	if (dst[copy_size - 1] == '\0')
	{
		return copy_size - 1;
	}
	dst[copy_size] = '\0';
	return copy_size;
}

//バッファの内容を画面出力に出力
template<std::size_t _BUFF_SIZE, class LOCK_POLICY>
void memConsole<_BUFF_SIZE, LOCK_POLICY>::printScreen(std::FILE* fp)
{
	auto lock = m_ringBuff.lockSharedScoped();//スコープ共有ロック
	GASHA_ forEach(m_ringBuff, [&fp](const block_type& block)
		{
			char temp[BLOCK_SIZE + 1];
			std::memcpy(temp, reinterpret_cast<const char*>(&block), BLOCK_SIZE);
			temp[BLOCK_SIZE] = '\0';
			fprintf(fp, temp);
		}
	);
}

//デストラクタ
template<std::size_t _BUFF_SIZE, class LOCK_POLICY>
memConsole<_BUFF_SIZE, LOCK_POLICY>::~memConsole()
{}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//メモリコンソールの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_memConsole(_BUFF_SIZE) \
	template class GASHA_ memConsole<_BUFF_SIZE>; \
	template class GASHA_ ring_buffer::container<typename GASHA_ memConsole<_BUFF_SIZE>::buffOpe>;
//※ロック指定版
#define GASHA_INSTANCING_memConsole_withLock(_BUFF_SIZE, LOCK_POLICY) \
	template class GASHA_ memConsole<_BUFF_SIZE, LOCK_POLICY>; \
	template class GASHA_ ring_buffer::container<typename GASHA_ memConsole<_BUFF_SIZE, LOCK_POLICY>::buffOpe>;

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//メモリコンソールの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_memConsole(_BUFF_SIZE) \
	template class GASHA_ memConsole<_BUFF_SIZE>;
//※ロック指定版
#define GASHA_INSTANCING_memConsole_withLock(_BUFF_SIZE, LOCK_POLICY) \
	template class GASHA_ memConsole<_BUFF_SIZE, LOCK_POLICY>;

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//--------------------------------------------------------------------------------
//【注】明示的インスタンス化に失敗する場合
//--------------------------------------------------------------------------------
//【原因①】
// 　対象クラスに必要なインターフェースが実装されていない。
//
// 　例えば、ソート処理に必要な「bool operator<(const value_type&) const」か「friend bool operator<(const value_type&, const value_type&)」や、
// 　探索処理に必要な「bool operator==(const key_type&) const」か「friend bool operator==(const value_type&, const key_type&)」。
//
// 　明示的なインスタンス化を行う場合、実際に使用しない関数のためのインターフェースも確実に実装する必要がある。
// 　逆に言えば、明示的なインスタンス化を行わない場合、使用しない関数のためのインターフェースを実装する必要がない。
//
//【対策１】
// 　インターフェースをきちんと実装する。
// 　（無難だが、手間がかかる。）
//
//【対策２】
// 　明示的なインスタンス化を行わずに、.cpp.h をテンプレート使用前にインクルードする。
// 　（手間がかからないが、コンパイル時の依存ファイルが増えるので、コンパイルが遅くなる可能性がある。）
//
//--------------------------------------------------------------------------------
//【原因②】
// 　同じ型のインスタンスが複数作成されている。
//
// 　通常、テンプレートクラス／関数の同じ型のインスタンスが複数作られても、リンク時に一つにまとめられるため問題がない。
// 　しかし、一つのソースファイルの中で複数のインスタンスが生成されると、コンパイラによってはエラーになる。
//   GCCの場合のエラーメッセージ例：（VC++ではエラーにならない）
// 　  source_file.cpp.h:114:17: エラー: duplicate explicit instantiation of ‘class templateClass<>’ [-fpermissive]
//
//【対策１】
// 　別のファイルに分けてインスタンス化する。
// 　（コンパイルへの影響が少なく、良い方法だが、無駄にファイル数が増える可能性がある。）
//
//【対策２】
// 　明示的なインスタンス化を行わずに、.cpp.h をテンプレート使用前にインクルードする。
// 　（手間がかからないが、コンパイル時の依存ファイルが増えるので、コンパイルが遅くなる可能性がある。）
//
//【対策３】
// 　GCCのコンパイラオプションに、 -fpermissive を指定し、エラーを警告に格下げする。
// 　（最も手間がかからないが、常時多数の警告が出る状態になりかねないので注意。）
//--------------------------------------------------------------------------------
// ※このコメントは、「明示的なインスタンス化マクロ」が定義されている全てのソースコードに
// 　同じ内容のものをコピーしています。

#endif//GASHA_INCLUDED_MEM_CONSOLE_CPP_H

// End of file

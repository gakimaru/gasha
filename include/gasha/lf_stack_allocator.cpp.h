#pragma once
#ifndef GASHA_INCLUDED_LF_STACK_ALLOCATOR_CPP_H
#define GASHA_INCLUDED_LF_STACK_ALLOCATOR_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lf_stack_allocator.cpp.h
// ロックフリースタックアロケータ【関数定義部】
//
// ※クラスのインスタンス化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。
// 　（コンパイル・リンク時間への影響を気にしないならOK）
// ※明示的なインスタンス化を避けたい場合は、ヘッダーファイルと共にインクルード。
// 　（この場合、実際に使用するメンバー関数しかインスタンス化されないので、対象クラスに不要なインターフェースを実装しなくても良い）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_stack_allocator.inl>//ロックフリースタックアロケータ【インライン関数／テンプレート関数定義部】
#include <gasha/string.h>//文字列処理：spprintf()

#include <cassert>//assert()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリースタックアロケータクラス

//メモリ確保
template<class AUTO_CLEAR>
void* lfStackAllocator<AUTO_CLEAR>::alloc(const std::size_t size, const std::size_t align)
{
	//サイズが0バイトならサイズを1に、アラインメントを0にする
	//※要求サイズが0でも必ずメモリを割り当てる点に注意（ただし、アラインメントは守らない）
	const std::size_t _size = size == 0 ? 1 : size;
	const std::size_t _align = size == 0 ? 0 : align;
	while (true)
	{
		//サイズとアラインメントをチェック
		size_type now_size = m_size.load();
		char* now_ptr = m_buffRef + now_size;
		char* new_ptr = adjustAlign(now_ptr, _align);
		const std::ptrdiff_t padding_size = new_ptr - now_ptr;
		const size_type alloc_size = static_cast<size_type>(padding_size + _size);
		const size_type new_size = now_size + alloc_size;
		if (new_size > m_maxSize)
		{
		#ifdef GASHA_LF_STACK_ALLOCATOR_ENABLE_ASSERTION
			static const bool NOT_ENOUGH_SPACE = false;
			assert(NOT_ENOUGH_SPACE);
		#endif//GASHA_LF_STACK_ALLOCATOR_ENABLE_ASSERTION
			return nullptr;
		}

		//使用中のサイズとメモリ確保数を更新
		m_count.fetch_add(1);//アロケート数
		if (m_size.compare_exchange_weak(now_size, new_size))//サイズのCAS
		{
			//空き領域を確保
			return reinterpret_cast<void*>(new_ptr);
		}
		//リトライのためにアロケート数を戻す
		m_count.fetch_sub(1);//アロケート数
	}
	return nullptr;//ダミー
}

//メモリ解放（共通処理）
template<class AUTO_CLEAR>
bool lfStackAllocator<AUTO_CLEAR>::_free(void* p)
{
	//メモリ確保数を更新
	m_count.fetch_sub(1);
	//自動クリア呼び出し
	AUTO_CLEAR auto_clear;
	auto_clear.autoClear(*this);
	return true;
}

//使用中のサイズを指定位置に戻す
//※ポインタ指定版
template<class AUTO_CLEAR>
bool  lfStackAllocator<AUTO_CLEAR>::rewind(void* p)
{
	while(true)
	{
		size_type now_size = m_size.load();
		const size_type new_size = static_cast<size_type>(reinterpret_cast<char*>(p) - m_buffRef);
		if(now_size < new_size)
			return false;
		if(m_size.compare_exchange_weak(now_size, new_size))//サイズのCAS
			return true;
	}
	return false;//ダミー
}

//メモリクリア
template<class AUTO_CLEAR>
void lfStackAllocator<AUTO_CLEAR>::clear()
{
	while(true)
	{
		size_type now_size = m_size.load();
		size_type now_count = m_count.load();
		if (now_count == 0 && now_size == 0)
			return;
		const size_type new_size = 0;
		const size_type new_count = 0;

		if (m_count.compare_exchange_weak(now_count, new_count))//アロケート数のCAS
		{
			if (m_size.compare_exchange_weak(now_size, new_size))//サイズのCAS
				return;
			//リトライのためにアロケート数を戻す
			m_count.fetch_add(now_count);//アロケート数
		}
	}
	return;//ダミー
}

//デバッグ情報作成
template<class AUTO_CLEAR>
std::size_t lfStackAllocator<AUTO_CLEAR>::debugInfo(char* message, const std::size_t max_size) const
{
	std::size_t message_len = 0;
	GASHA_ spprintf(message, max_size, message_len, "----- Debug-info for lfStackAllocator -----\n");
	GASHA_ spprintf(message, max_size, message_len, "buff=%p, maxSize=%d, size=%d, remain=%d, count=%d\n", m_buffRef, maxSize(), this->size(), remain(), count());
	GASHA_ spprintf(message, max_size, message_len, "-------------------------------------------");//最終行改行なし
	return message_len;
}

//使用中のサイズと数を取得
template<class LOCK_TYPE>
void lfStackAllocator<LOCK_TYPE>::getSizeAndCount(typename lfStackAllocator<LOCK_TYPE>::size_type& size, typename lfStackAllocator<LOCK_TYPE>::size_type& count)
{
	//使用中のサイズとメモリ確保数を取得
	while (true)
	{
		size = m_size.load();
		count = m_count.load();
		if (size == m_size.load() && count == m_count.load())//他のスレッドの割り込みがなく、取得した値に不整合が無いと見なす
			break;
	}
}

//使用中のサイズと数をリセット
template<class LOCK_TYPE>
bool lfStackAllocator<LOCK_TYPE>::resetSizeAndCount(const typename lfStackAllocator<LOCK_TYPE>::size_type size, const typename lfStackAllocator<LOCK_TYPE>::size_type count)
{
	while (true)
	{
		size_type now_size = m_size.load();
		size_type now_count = m_count.load();
		if (now_size == size && now_count == count)//現在の値と一致するなら終了
			return true;
		if (now_size < size || now_count < count)//現在の値の方が小さい場合は失敗
			return false;
		//使用中のサイズとメモリ確保数を更新
		if (m_count.compare_exchange_weak(now_count, count))//アロケート数のCAS
		{
			if (m_size.compare_exchange_weak(now_size, size))//サイズのCAS
				return true;
			//リトライのためにアロケート数を戻す
			m_count.fetch_add(now_count - count);//アロケート数
		}
	}
	return false;//ダミー
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//ロックフリースタックアロケータの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_lfStackAllocator() \
	template class GASHA_ lfStackAllocator<>;

//スマートロックフリースタックアロケータの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_lfSmartStackAllocator() \
	template class GASHA_ lfStackAllocator<GASHA_ lfStackAllocatorAutoClear>;

#if 0//不要
//バッファ付きロックフリースタックアロケータの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_lfStackAllocator_withBuff(_MAX_SIZE) \
	template class GASHA_ lfStackAllocator_withBuff<_MAX_SIZE>; \
	template class GASHA_ lfStackAllocator<>;

//バッファ付きスマートロックフリースタックアロケータの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_lfSmartStackAllocator_withBuff(_MAX_SIZE) \
	template class GASHA_ lfStackAllocator_withBuff<_MAX_SIZE, GASHA_ lfStackAllocatorAutoClear>; \
	template class GASHA_ lfStackAllocator<GASHA_ lfStackAllocatorAutoClear>;

//型指定バッファ付きロックフリースタックアロケータの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_lfStackAllocator_withType(T, _NUM) \
	template class GASHA_ lfStackAllocator_withType<T, _NUM>; \
	template class GASHA_ lfStackAllocator_withBuff<sizeof(T) * _NUM>; \
	template class GASHA_ lfStackAllocator<>;

//型指定バッファ付きスマートロックフリースタックアロケータの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_lfSmartStackAllocator_withType(T, _NUM) \
	template class GASHA_ lfStackAllocator_withType<T, _NUM, GASHA_ lfStackAllocatorAutoClear>; \
	template class GASHA_ lfStackAllocator_withBuff<sizeof(T)* _NUM, GASHA_ lfStackAllocatorAutoClear>; \
	template class GASHA_ lfStackAllocator<lfStackAllocatorAutoClear>;
#endif

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

#endif//GASHA_INCLUDED_LF_STACK_ALLOCATOR_CPP_H

// End of file

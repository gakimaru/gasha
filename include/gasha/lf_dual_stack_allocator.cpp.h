#pragma once
#ifndef GASHA_INCLUDED_LF_DUAL_STACK_ALLOCATOR_CPP_H
#define GASHA_INCLUDED_LF_DUAL_STACK_ALLOCATOR_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lf_dual_stack_allocator.cpp.h
// ロックフリー双方向スタックアロケータ【関数定義部】
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

#include <gasha/lf_dual_stack_allocator.inl>//ロックフリー双方向スタックアロケータ【インライン関数／テンプレート関数定義部】

#include <assert.h>//assert()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

//【VC++】sprintf を使用すると、error C4996 が発生する
//  error C4996: 'sprintf': This function or variable may be unsafe. Consider using strncpy_fast_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリー双方向スタックアロケータクラス

//デバッグ情報作成
template<class AUTO_CLEAR>
std::size_t lfDualStackAllocator<AUTO_CLEAR>::debugInfo(char* message)
{
	std::size_t size = 0;
	size += sprintf(message + size, "----- Debug Info for lfDualStackAllocator -----\n");
	size += sprintf(message + size, "buffRef=%p, maxSize=%d, size=%d, sizeAsc=%d, sizeDesc=%d, remain=%d, allocatedCountAsc=%d, allocatedCountDesc=%d, allocatedCount=%d\n", m_buffRef, maxSize(), this->size(), sizeAsc(), sizeDesc(), remain(), allocatedCountAsc(), allocatedCountDesc(), allocatedCount());
	size += sprintf(message + size, "----------\n");
	return size;
}

//正順メモリ確保
template<class AUTO_CLEAR>
void* lfDualStackAllocator<AUTO_CLEAR>::_allocAsc(const std::size_t size, const std::size_t align)
{
	//サイズが0バイトならサイズを1に、アラインメントを0にする
	//※要求サイズが0でも必ずメモリを割り当てる点に注意（ただし、アラインメントは守らない）
	const std::size_t _size = size == 0 ? 1 : size;
	const std::size_t _align = size == 0 ? 0 : align;
	while(true)
	{
		//サイズとアラインメントをチェック
		size2_type now_size2 = m_size.load();
		size_type now_size = sizeAsc(now_size2);
		const size_type now_size_desc = sizeDesc(now_size2);
		const size_type now_size_all = now_size + now_size_desc;
		char* now_ptr = m_buffRef + now_size;
		char* new_ptr = adjustAlign(now_ptr, _align);
		const std::ptrdiff_t padding_size = new_ptr - now_ptr;
		const size_type alloc_size = static_cast<size_type>(padding_size + _size);
		const size_type new_size_all = now_size_all + alloc_size;
		const size_type new_size = now_size + alloc_size;
		if (new_size_all > m_maxSize)
		{
		#ifdef GASHA_LF_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
			static const bool NOT_ENOUGH_SPACE = false;
			assert(NOT_ENOUGH_SPACE);
		#endif//GASHA_LF_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
			return nullptr;
		}
		
		//使用中のサイズとメモリ確保数を更新
		m_allocatedCountAsc.fetch_add(1);//正順アロケート数
		const size2_type new_size2 = size2(new_size, now_size_desc);
		if (m_size.compare_exchange_weak(now_size2, new_size2))//サイズのCAS
		{
			//空き領域を確保
			return reinterpret_cast<void*>(new_ptr);
		}
		//リトライのためにアロケート数を戻す
		m_allocatedCountAsc.fetch_sub(1);//正順アロケート数
	}
	return nullptr;//ダミー
}

//逆順メモリ確保
template<class AUTO_CLEAR>
void* lfDualStackAllocator<AUTO_CLEAR>::_allocDesc(const std::size_t size, const std::size_t align)
{
	//サイズが0バイトならサイズを1に、アラインメントを0にする
	//※要求サイズが0でも必ずメモリを割り当てる点に注意（ただし、アラインメントは守らない）
	const std::size_t _size = size == 0 ? 1 : size;
	const std::size_t _align = size == 0 ? 0 : align;
	while(true)
	{
		//サイズとアラインメントをチェック
		size2_type now_size2 = m_size.load();
		size_type now_size = sizeDesc(now_size2);
		const size_type now_size_asc = sizeAsc(now_size2);
		const size_type now_size_all = now_size + now_size_asc;
		char* now_ptr = m_buffRef + m_maxSize - now_size;
		char* new_ptr = adjustAlign(now_ptr - _size - _align + (_align == 0 ? 0 : 1), _align);
		const size_type alloc_size = static_cast<size_type>(now_ptr - new_ptr);
		const size_type new_size_all = now_size_all + alloc_size;
		const size_type new_size = now_size + alloc_size;
		if (new_size_all > m_maxSize)
		{
		#ifdef GASHA_LF_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
			static const bool NOT_ENOUGH_SPACE = false;
			assert(NOT_ENOUGH_SPACE);
		#endif//GASHA_LF_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
			return nullptr;
		}
		
		//使用中のサイズとメモリ確保数を更新
		m_allocatedCountDesc.fetch_add(1);//逆順アロケート数
		const size2_type new_size2 = size2(now_size_asc, new_size);
		if (m_size.compare_exchange_weak(now_size2, new_size2))//サイズのCAS
		{
			//空き領域を確保
			return reinterpret_cast<void*>(new_ptr);
		}
		//リトライのためにアロケート数を戻す
		m_allocatedCountDesc.fetch_sub(1);//逆順アロケート数
	}
	return nullptr;//ダミー
}

//正順メモリ解放（共通処理）
template<class AUTO_CLEAR>
bool lfDualStackAllocator<AUTO_CLEAR>::_freeAsc(void* p)
{
	//メモリ確保数を更新
	m_allocatedCountAsc.fetch_sub(1);
	//自動クリア呼び出し
	AUTO_CLEAR auto_clear;
	auto_clear.autoClearAsc(*this);
	return true;
}

//逆順メモリ解放（共通処理）
template<class AUTO_CLEAR>
bool lfDualStackAllocator<AUTO_CLEAR>::_freeDesc(void* p)
{
	//メモリ確保数を更新
	m_allocatedCountDesc.fetch_sub(1);
	//自動クリア呼び出し
	AUTO_CLEAR auto_clear;
	auto_clear.autoClearDesc(*this);
	return true;
}

//正順で使用中のサイズを指定位置に戻す
//※ポインタ指定版
template<class AUTO_CLEAR>
bool lfDualStackAllocator<AUTO_CLEAR>::_rewindAsc(void* p)
{
	while(true)
	{
		size2_type now_size2 = m_size.load();
		size_type now_size = sizeAsc(now_size2);
		const size_type now_size_desc = sizeDesc(now_size2);
		const size_type now_size_all = now_size + now_size_desc;
		const size_type new_size = static_cast<size_type>(reinterpret_cast<char*>(p) - m_buffRef);
		if(now_size < new_size)
			return false;
		const size_type rewind_size = now_size - new_size;
		const size_type new_size_all = now_size_all - rewind_size;
		
		//使用中のサイズとメモリ確保数を更新
		const size2_type new_size2 = size2(new_size, now_size_desc);
		if(m_size.compare_exchange_weak(now_size2, new_size2))//サイズのCAS
			return true;
	}
	return false;//ダミー
}

//逆順で使用中のサイズを指定位置に戻す
//※ポインタ指定版
template<class AUTO_CLEAR>
bool lfDualStackAllocator<AUTO_CLEAR>::_rewindDesc(void* p)
{
	while(true)
	{
		size2_type now_size2 = m_size.load();
		size_type now_size = sizeDesc(now_size2);
		const size_type now_size_asc = sizeAsc(now_size2);
		const size_type now_size_all = now_size + now_size_asc;
		const size_type new_size = static_cast<size_type>(m_buffRef + m_maxSize - reinterpret_cast<char*>(p));
		if(now_size < new_size)
			return false;
		const size_type rewind_size = now_size - new_size;
		const size_type new_size_all = now_size_all - rewind_size;
		
		//使用中のサイズとメモリ確保数を更新
		const size2_type new_size2 = size2(now_size_asc, new_size);
		if(m_size.compare_exchange_weak(now_size2, new_size2))//サイズのCAS
			return true;
	}
	return false;//ダミー
}

//正順のメモリクリア
template<class AUTO_CLEAR>
void lfDualStackAllocator<AUTO_CLEAR>::_clearAsc()
{
	while(true)
	{
		const size_type now_count = m_allocatedCountAsc.load();
		size2_type now_size2 = m_size.load();
		size_type now_size = sizeAsc(now_size2);
		const size_type now_size_desc = sizeDesc(now_size2);
		const size_type now_size_all = now_size + now_size_desc;
		const size_type new_size_all = now_size_all - now_size;
		const size_type new_size = 0;
		
		//使用中のサイズとメモリ確保数を更新
		const size2_type new_size2 = size2(new_size, now_size_desc);
		if(m_size.compare_exchange_weak(now_size2, new_size2))//サイズのCAS
		{
			m_allocatedCountAsc.fetch_sub(now_count);//正順アロケート数
			return;
		}
	}
	return;//ダミー
}
//逆順のメモリクリア
template<class AUTO_CLEAR>
void lfDualStackAllocator<AUTO_CLEAR>::_clearDesc()
{
	while(true)
	{
		const size_type now_count = m_allocatedCountDesc.load();
		size2_type now_size2 = m_size.load();
		size_type now_size = sizeDesc(now_size2);
		const size_type now_size_asc = sizeAsc(now_size2);
		const size_type now_size_all = now_size + now_size_asc;
		const size_type new_size_all = now_size_all - now_size;
		const size_type new_size = 0;
		
		//使用中のサイズとメモリ確保数を更新
		const size2_type new_size2 = size2(now_size_asc, new_size);
		if(m_size.compare_exchange_weak(now_size2, new_size2))//サイズのCAS
		{
			m_allocatedCountDesc.fetch_sub(now_count);//逆順アロケート数
			return;
		}
	}
	return;//ダミー
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//ロックフリー双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_lfDualStackAllocator() \
	template class lfDualStackAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_lfDualStackAllocator_withLock(LOCK_TYPE) \
	template class lfDualStackAllocator<LOCK_TYPE>;

//スマートロックフリー双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_lfSmartDualStackAllocator() \
	template class lfDualStackAllocator<lfDualStackAllocatorAutoClear>;
//※ロック指定版
#define GASHA_INSTANCING_lfSmartDualStackAllocator_withLock(LOCK_TYPE) \
	template class lfDualStackAllocator<lfDualStackAllocatorAutoClear>;

#if 0//不要
//バッファ付きロックフリー双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_lfDualStackAllocator_withBuff(_MAX_SIZE) \
	template class lfDualStackAllocator_withBuff<_MAX_SIZE>; \
	template class lfDualStackAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_lfDualStackAllocator_withBuff_withLock(_MAX_SIZE, LOCK_TYPE) \
	template class lfDualStackAllocator_withBuff<_MAX_SIZE, LOCK_TYPE>; \
	template class lfDualStackAllocator<LOCK_TYPE>;

//バッファ付きスマートロックフリー双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_lfSmartDualStackAllocator_withBuff(_MAX_SIZE) \
	template class lfDualStackAllocator_withBuff<_MAX_SIZE, lfDualStackAllocatorAutoClear>; \
	template class lfDualStackAllocator<lfDualStackAllocatorAutoClear>;
//※ロック指定版
#define GASHA_INSTANCING_lfSmartDualStackAllocator_withBuff_withLock(_MAX_SIZE, LOCK_TYPE) \
	template class lfDualStackAllocator_withBuff<_MAX_SIZE, lfDualStackAllocatorAutoClear>; \
	template class lfDualStackAllocator<lfDualStackAllocatorAutoClear>;

//型指定バッファ付きロックフリー双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_lfDualStackAllocator_withType(T, _NUM) \
	template class lfDualStackAllocator_withType<T, _NUM>; \
	template class lfDualStackAllocator_withBuff<sizeof(T) * _NUM>; \
	template class lfDualStackAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_lfDualStackAllocator_withType_withLock(T, _NUM, LOCK_TYPE) \
	template class lfDualStackAllocator_withType<T, _NUM, LOCK_TYPE>; \
	template class lfDualStackAllocator_withBuff<sizeof(T)* _NUM, LOCK_TYPE>; \
	template class lfDualStackAllocator<LOCK_TYPE>;

//型指定バッファ付きスマートロックフリー双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_lfSmartDualStackAllocator_withType(T, _NUM) \
	template class lfDualStackAllocator_withType<T, _NUM, lfDualStackAllocatorAutoClear>; \
	template class lfDualStackAllocator_withBuff<sizeof(T)* _NUM, lfDualStackAllocatorAutoClear>; \
	template class lfDualStackAllocator<lfDualStackAllocatorAutoClear>;
//※ロック指定版
#define GASHA_INSTANCING_lfSmartDualStackAllocator_withType_withLock(T, _NUM, LOCK_TYPE) \
	template class lfDualStackAllocator_withType<T, _NUM, lfDualStackAllocatorAutoClear>; \
	template class lfDualStackAllocator_withBuff<sizeof(T)* _NUM, lfDualStackAllocatorAutoClear>; \
	template class lfDualStackAllocator<lfDualStackAllocatorAutoClear>;
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
// 　  source_file.cpp.h:114:17: エラー: duplicate explicit instantiation of ‘class gasha::templateClass<>’ [-fpermissive]
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

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_LF_DUAL_STACK_ALLOCATOR_CPP_H

// End of file

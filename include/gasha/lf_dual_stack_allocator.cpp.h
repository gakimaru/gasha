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

#include <gasha/string.h>//文字列処理：spprintf()

#include <cassert>//assert()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリー双方向スタックアロケータクラス

//デバッグ情報作成
template<class AUTO_CLEAR>
std::size_t lfDualStackAllocator<AUTO_CLEAR>::debugInfo(char* message) const
{
	std::size_t size = 0;
	GASHA_ spprintf(message, size, "----- Debug-info for lfDualStackAllocator -----\n");
	GASHA_ spprintf(message, size, "buff=%p, maxSize=%d, size=%d(ASC=%d,DESC=%d), remain=%d, count%d(ASC=%d,DESC=%d), order=%s\n", m_buffRef, maxSize(), this->size(), sizeAsc(), sizeDesc(), remain(), count(), countAsc(), countDesc(), allocationOrder() == ALLOC_ASC ? "ASC" : "DESC");
	GASHA_ spprintf(message, size, "-----------------------------------------------\n");
	return size;
}

//使用中のサイズと数を取得
template<class AUTO_CLEAR>
void lfDualStackAllocator<AUTO_CLEAR>::getSizeAndCount(typename lfDualStackAllocator<AUTO_CLEAR>::size_type& size, typename lfDualStackAllocator<AUTO_CLEAR>::size_type& count)
{
	while (true)
	{
		//使用中のサイズとメモリ確保数を取得
		const size2_type size2 = m_size.load();
		const size_type count_asc = m_countAsc.load();
		const size_type count_desc = m_countDesc.load();
		if (m_allocateOrder.load() == ALLOC_ASC)
		{
			size = sizeAsc(size2);
			count = count_asc;
		}
		else//if (m_allocateOrder.load() == ALLOC_DESC)
		{
			size = sizeDesc(size2);
			count = count_desc;
		}
		if (size2 == m_size.load() && count_asc == m_countAsc.load() && count_desc == m_countDesc.load())//他のスレッドの割り込みがなく、取得した値に不整合が無いと見なす
			break;
	}
}
template<class AUTO_CLEAR>
void lfDualStackAllocator<AUTO_CLEAR>::getSizeAndCount(allocationOrder_t& order, typename lfDualStackAllocator<AUTO_CLEAR>::size_type& size_asc, typename lfDualStackAllocator<AUTO_CLEAR>::size_type& size_desc, typename lfDualStackAllocator<AUTO_CLEAR>::size_type& count_asc, typename lfDualStackAllocator<AUTO_CLEAR>::size_type& count_desc)
{
	while(true)
	{
		//使用中のサイズとメモリ確保数を取得
		const size2_type size2 = m_size.load();
		order = m_allocateOrder.load();
		size_asc = sizeAsc(size2);
		count_asc = m_countAsc.load();
		size_desc = sizeDesc(size2);
		count_desc = m_countDesc.load();
		if (size2 == m_size.load() && order == m_allocateOrder.load() && count_asc == m_countAsc.load() && count_desc == m_countDesc.load())//他のスレッドの割り込みがなく、取得した値に不整合が無いと見なす
			break;
	}
}

//使用中のサイズと数をリセット
template<class AUTO_CLEAR>
bool lfDualStackAllocator<AUTO_CLEAR>::resetSizeAndCount(const allocationOrder_t order, const typename lfDualStackAllocator<AUTO_CLEAR>::size_type size_asc, const typename lfDualStackAllocator<AUTO_CLEAR>::size_type size_desc, const typename lfDualStackAllocator<AUTO_CLEAR>::size_type count_asc, const typename lfDualStackAllocator<AUTO_CLEAR>::size_type count_desc)
{
	while (true)
	{
		size2_type now_size2 = m_size.load();
		size_type now_count_asc = m_countAsc.load();
		size_type now_count_desc = m_countDesc.load();
		const size_type now_size_asc = sizeAsc(now_size2);
		const size_type now_size_desc = sizeDesc(now_size2);
		if (now_size_asc == size_asc && now_size_desc == size_desc && now_count_asc == count_asc && now_count_desc == count_desc)//現在の値と一致するなら終了
			return true;
		if (now_size_asc < size_asc || now_size_desc < size_desc || now_count_asc < count_asc || now_count_desc < count_desc)//現在の値の方が小さい場合は失敗
				return false;
		const size2_type new_size2 = size2(size_asc, size_desc);
		
		//使用中のサイズとメモリ確保数を更新
		if (m_countAsc.compare_exchange_weak(now_count_asc, count_asc))//正順アロケート数のCAS
		{
			if (m_countDesc.compare_exchange_weak(now_count_desc, count_desc))//逆順アロケート数のCAS
			{
				if (m_size.compare_exchange_weak(now_size2, new_size2))//正順・逆順サイズのCAS
				{
					m_allocateOrder.store(order);//アロケート方向
					return true;
				}
				//リトライのためにアロケート数を戻す
				m_countDesc.fetch_add(now_count_desc - count_desc);//逆順アロケート数
			}
			//リトライのためにアロケート数を戻す
			m_countAsc.fetch_add(now_count_asc - count_asc);//正順アロケート数
		}
	}
	return false;//ダミー
}
template<class AUTO_CLEAR>
bool lfDualStackAllocator<AUTO_CLEAR>::resetSizeAndCount(const allocationOrder_t order, const typename lfDualStackAllocator<AUTO_CLEAR>::size_type size, const typename lfDualStackAllocator<AUTO_CLEAR>::size_type count)
{
	while (true)
	{
		size2_type now_size2 = m_size.load();
		const size_type now_size_asc = sizeAsc(now_size2);
		const size_type now_size_desc = sizeDesc(now_size2);
		if (order == ALLOC_ASC)
		{
			size_type now_count = m_countAsc.load();
			if (now_size_asc == size && now_count == count)//現在の値と一致するなら終了
				return true;
			if (now_size_asc < size || now_count < count)//現在の値の方が小さい場合は失敗
				return false;
			const size2_type new_size2 = size2(size, now_size_desc);

			//使用中のサイズとメモリ確保数を更新
			if (m_countAsc.compare_exchange_weak(now_count, count))//正順アロケート数のCAS
			{
				if (m_size.compare_exchange_weak(now_size2, new_size2))//正順サイズのCAS
					return true;
				//リトライのためにアロケート数を戻す
				m_countAsc.fetch_add(now_count - count);//正順アロケート数
			}
		}
		else//if (order == ALLOC_DESC)
		{
			size_type now_count = m_countDesc.load();
			if (now_size_desc == size && now_count == count)//現在の値と一致するなら終了
				return true;
			if (now_size_desc < size || now_count < count)//現在の値の方が小さい場合は失敗
				return false;
			const size2_type new_size2 = size2(now_size_asc, size);

			//使用中のサイズとメモリ確保数を更新
			if (m_countDesc.compare_exchange_weak(now_count, count))//逆順アロケート数のCAS
			{
				if (m_size.compare_exchange_weak(now_size2, new_size2))//逆順サイズのCAS
					return true;
				//リトライのためにアロケート数を戻す
				m_countDesc.fetch_add(now_count - count);//逆順アロケート数
			}
		}
	}
	return false;//ダミー
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
		const size2_type new_size2 = size2(new_size, now_size_desc);

		//使用中のサイズとメモリ確保数を更新
		m_countAsc.fetch_add(1);//正順アロケート数
		if (m_size.compare_exchange_weak(now_size2, new_size2))//サイズのCAS
		{
			//空き領域を確保
			return reinterpret_cast<void*>(new_ptr);
		}
		//リトライのためにアロケート数を戻す
		m_countAsc.fetch_sub(1);//正順アロケート数
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
		const size2_type new_size2 = size2(now_size_asc, new_size);

		//使用中のサイズとメモリ確保数を更新
		m_countDesc.fetch_add(1);//逆順アロケート数
		if (m_size.compare_exchange_weak(now_size2, new_size2))//サイズのCAS
		{
			//空き領域を確保
			return reinterpret_cast<void*>(new_ptr);
		}
		//リトライのためにアロケート数を戻す
		m_countDesc.fetch_sub(1);//逆順アロケート数
	}
	return nullptr;//ダミー
}

//正順メモリ解放（共通処理）
template<class AUTO_CLEAR>
bool lfDualStackAllocator<AUTO_CLEAR>::_freeAsc(void* p)
{
	//メモリ確保数を更新
	m_countAsc.fetch_sub(1);
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
	m_countDesc.fetch_sub(1);
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
		size2_type now_size2 = m_size.load();
		size_type now_count = m_countAsc.load();
		const size_type now_size = sizeAsc(now_size2);
		if (now_count == 0 && now_size == 0)
			return;
		const size_type now_size_desc = sizeDesc(now_size2);
		const size_type now_size_all = now_size + now_size_desc;
		const size_type new_size_all = now_size_all - now_size;
		const size_type new_size = 0;
		const size2_type new_size2 = size2(new_size, now_size_desc);
		const size_type new_count = 0;

		//使用中のサイズとメモリ確保数を更新
		if (m_countAsc.compare_exchange_weak(now_count, new_count))//正順アロケート数のCAS
		{
			if (m_size.compare_exchange_weak(now_size2, new_size2))//サイズのCAS
				return;
			//リトライのためにアロケート数を戻す
			m_countAsc.fetch_add(now_count);//正順アロケート数
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
		size2_type now_size2 = m_size.load();
		size_type now_count = m_countDesc.load();
		const size_type now_size = sizeDesc(now_size2);
		if (now_count == 0 && now_size == 0)
			return;
		const size_type now_size_asc = sizeAsc(now_size2);
		const size_type now_size_all = now_size + now_size_asc;
		const size_type new_size_all = now_size_all - now_size;
		const size_type new_size = 0;
		const size2_type new_size2 = size2(now_size_asc, new_size);
		const size_type new_count = 0;
		
		//使用中のサイズとメモリ確保数を更新
		if (m_countDesc.compare_exchange_weak(now_count, new_count))//逆順アロケート数のCAS
		{
			if (m_size.compare_exchange_weak(now_size2, new_size2))//サイズのCAS
				return;
			//リトライのためにアロケート数を戻す
			m_countDesc.fetch_add(now_count);//逆順アロケート数
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
	template class GASHA_ lfDualStackAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_lfDualStackAllocator_withLock(LOCK_TYPE) \
	template class GASHA_ lfDualStackAllocator<LOCK_TYPE>;

//スマートロックフリー双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_lfSmartDualStackAllocator() \
	template class GASHA_ lfDualStackAllocator<GASHA_ lfDualStackAllocatorAutoClear>;
//※ロック指定版
#define GASHA_INSTANCING_lfSmartDualStackAllocator_withLock(LOCK_TYPE) \
	template class GASHA_ lfDualStackAllocator<GASHA_ lfDualStackAllocatorAutoClear>;

#if 0//不要
//バッファ付きロックフリー双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_lfDualStackAllocator_withBuff(_MAX_SIZE) \
	template class GASHA_ lfDualStackAllocator_withBuff<_MAX_SIZE>; \
	template class GASHA_ lfDualStackAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_lfDualStackAllocator_withBuff_withLock(_MAX_SIZE, LOCK_TYPE) \
	template class GASHA_ lfDualStackAllocator_withBuff<_MAX_SIZE, LOCK_TYPE>; \
	template class GASHA_ lfDualStackAllocator<LOCK_TYPE>;

//バッファ付きスマートロックフリー双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_lfSmartDualStackAllocator_withBuff(_MAX_SIZE) \
	template class GASHA_ lfDualStackAllocator_withBuff<_MAX_SIZE, GASHA_ lfDualStackAllocatorAutoClear>; \
	template class GASHA_ lfDualStackAllocator<GASHA_ lfDualStackAllocatorAutoClear>;
//※ロック指定版
#define GASHA_INSTANCING_lfSmartDualStackAllocator_withBuff_withLock(_MAX_SIZE, LOCK_TYPE) \
	template class GASHA_ lfDualStackAllocator_withBuff<_MAX_SIZE, GASHA_ lfDualStackAllocatorAutoClear>; \
	template class GASHA_ lfDualStackAllocator<GASHA_ lfDualStackAllocatorAutoClear>;

//型指定バッファ付きロックフリー双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_lfDualStackAllocator_withType(T, _NUM) \
	template class GASHA_ lfDualStackAllocator_withType<T, _NUM>; \
	template class GASHA_ lfDualStackAllocator_withBuff<sizeof(T) * _NUM>; \
	template class GASHA_ lfDualStackAllocator<>;
//※ロック指定版
#define GASHA_INSTANCING_lfDualStackAllocator_withType_withLock(T, _NUM, LOCK_TYPE) \
	template class GASHA_ lfDualStackAllocator_withType<T, _NUM, LOCK_TYPE>; \
	template class GASHA_ lfDualStackAllocator_withBuff<sizeof(T)* _NUM, LOCK_TYPE>; \
	template class GASHA_ lfDualStackAllocator<LOCK_TYPE>;

//型指定バッファ付きスマートロックフリー双方向スタックアロケータの明示的なインスタンス化用マクロ
//※ロックなし版
#define GASHA_INSTANCING_lfSmartDualStackAllocator_withType(T, _NUM) \
	template class GASHA_ lfDualStackAllocator_withType<T, _NUM, GASHA_ lfDualStackAllocatorAutoClear>; \
	template class GASHA_ lfDualStackAllocator_withBuff<sizeof(T)* _NUM, GASHA_ lfDualStackAllocatorAutoClear>; \
	template class GASHA_ lfDualStackAllocator<GASHA_ lfDualStackAllocatorAutoClear>;
//※ロック指定版
#define GASHA_INSTANCING_lfSmartDualStackAllocator_withType_withLock(T, _NUM, LOCK_TYPE) \
	template class GASHA_ lfDualStackAllocator_withType<T, _NUM, GASHA_ lfDualStackAllocatorAutoClear>; \
	template class GASHA_ lfDualStackAllocator_withBuff<sizeof(T)* _NUM, GASHA_ lfDualStackAllocatorAutoClear>; \
	template class GASHA_ lfDualStackAllocator<GASHA_ lfDualStackAllocatorAutoClear>;
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

#endif//GASHA_INCLUDED_LF_DUAL_STACK_ALLOCATOR_CPP_H

// End of file

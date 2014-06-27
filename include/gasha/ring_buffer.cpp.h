#pragma once
#ifndef __RING_BUFFER_CPP_H_
#define __RING_BUFFER_CPP_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// ring_buffer.cpp.h
// リングバッファコンテナ【関数定義部】
//
// ※コンテナクラスの実体化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/ring_buffer.inl>//リングバッファコンテナ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace ring_buffer
{
	//----------------------------------------
	//イテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::update(const typename container<OPE_TYPE>::index_type logical_index) const
	{
		//if (logical_index == INVALID_INDEX || logical_index < 0 || logical_index > static_cast<index_type>(m_con->m_size))
		if (logical_index > static_cast<index_type>(m_con->m_size))
		{
			m_logicalIndex = INVALID_INDEX;
			m_value = nullptr;
		}
		else
		{
			m_logicalIndex = logical_index;
			const index_type real_index = m_con->_to_real_index(m_logicalIndex);
			m_value = const_cast<value_type*>(m_con->_ref_real_element(real_index));
		}
	}
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::addIndexAndUpdate(const int add) const
	{
		update(m_logicalIndex + add);
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		update(m_logicalIndex);
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		update(m_logicalIndex);
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(nullptr)
	{
		update(m_logicalIndex);
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(nullptr)
	{
		update(m_logicalIndex);
	}
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const container<OPE_TYPE>& con, const bool is_end) :
		m_con(&con),
		m_logicalIndex(INVALID_INDEX),
		m_value(nullptr)
	{
		if (!is_end)
			update(0);//先頭データ
		else
			update(m_con->m_size);//末尾データ
	}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const container<OPE_TYPE>& con, const typename container<OPE_TYPE>::index_type logical_index) :
		m_con(&con),
		m_logicalIndex(INVALID_INDEX),
		m_value(nullptr)
	{
		update(logical_index);
	}

	//----------------------------------------
	//リバースイテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::update(const typename container<OPE_TYPE>::index_type logical_index) const
	{
		//if (logical_index == INVALID_INDEX || logical_index < 0 || index > static_cast<index_type>(m_con->m_size))
		if (logical_index > static_cast<index_type>(m_con->m_size))
		{
			m_logicalIndex = INVALID_INDEX;
			m_value = nullptr;
		}
		else
		{
			m_logicalIndex = logical_index;
			const index_type real_index = m_con->_to_real_index(m_logicalIndex);
			m_value = real_index == 0 ? const_cast<value_type*>(m_con->_ref_real_element(m_con->m_maxSize - 1)) : const_cast<value_type*>(m_con->_ref_real_element(real_index - 1));
		}
	}
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::addIndexAndUpdate(const int add) const
	{
		update(m_logicalIndex - add);
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		update(m_logicalIndex);
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		update(m_logicalIndex);
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(nullptr)
	{
		update(m_logicalIndex);
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(nullptr)
	{
		update(m_logicalIndex);
	}
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const container<OPE_TYPE>& con, const bool is_end) :
		m_con(&con),
		m_logicalIndex(INVALID_INDEX),
		m_value(nullptr)
	{
		if (!is_end)
			update(m_con->m_size);//末尾データ
		else
			update(0);//先頭データ
	}
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const  container<OPE_TYPE>& con, const typename  container<OPE_TYPE>::index_type logical_index) :
		m_con(&con),
		m_logicalIndex(INVALID_INDEX),
		m_value(nullptr)
	{
		update(logical_index);
	}

	//----------------------------------------
	//コンテナ本体のメソッド

	//配列の再割り当て
	//※ポインタと配列要素数指定版
//	template<class OPE_TYPE>
//	void container<OPE_TYPE>::assignArray(value_type* array, const typename container<OPE_TYPE>::size_type max_size, const int size)
//	{
//	}
	
}//namespace ring_buffer

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__RING_BUFFER_CPP_H_

// End of file

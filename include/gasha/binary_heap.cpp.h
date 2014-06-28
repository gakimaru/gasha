#pragma once
#ifndef __BINARY_HEAP_CPP_H_
#define __BINARY_HEAP_CPP_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// binary_heap.cpp.h
// 二分ヒープコンテナ【関数定義部】
//
// ※コンテナクラスの実体化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/binary_heap.inl>//二分ヒープコンテナ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace binary_heap
{
	//----------------------------------------
	//イテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::iterator::update(const index_type index) const
	{
		//if (index == INVALID_INDEX || index < 0 || index > static_cast<index_type>(m_con->m_used))
		if (index > static_cast<index_type>(m_con->m_used))
		{
			m_index = INVALID_INDEX;
			m_value = nullptr;
		}
		else
		{
			m_index = index;
			m_value = const_cast<value_type*>(m_con->_refNode(m_index));
		}
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::iterator::addIndexAndUpdate(const int add) const
	{
		update(m_index + add);
	}
	//ムーブオペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator=(const iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
	//コピーオペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator=(const iterator& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
	//ムーブコンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
	//コピーコンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
	//コンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(const container& con, const bool is_end) :
		m_con(&con),
		m_index(INVALID_INDEX),
		m_value(nullptr)
	{
		if (!is_end)
			update(0);//先頭データ
		else
			update(m_con->m_used);//末尾データ
	}

	//----------------------------------------
	//リバースイテレータのメソッド

	//メソッド
	//参照を更新
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::update(const index_type index) const
	{
		//if (index == INVALID_INDEX || index < 0 || index > static_cast<index_type>(m_con->m_used))
		if (index > static_cast<index_type>(m_con->m_used))
		{
			m_index = INVALID_INDEX;
			m_value = nullptr;
		}
		else
		{
			m_index = index;
			m_value = const_cast<value_type*>(m_con->_refNode(m_index)) - 1;
		}
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::addIndexAndUpdate(const int add) const
	{
		update(m_index - add);
	}
	//ムーブオペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
	}
	//コピーコンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
	}
	//コンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::reverse_iterator(const container& con, const bool is_end) :
		m_con(&con),
		m_index(INVALID_INDEX),
		m_value(nullptr)
	{
		if (!is_end)
			update(m_con->m_used);//末尾データ
		else
			update(0);//先頭データ
	}

	//----------------------------------------
	//コンテナ本体のメソッド

	//配列の再割り当て
	//※ポインタと配列要素数指定版
//	template<class OPE_TYPE>
//	void container<OPE_TYPE>::assignArray(value_type* array, const typename container<OPE_TYPE>::size_type max_size, const int size)
//	{
//	}
	
}//namespace binary_heap

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__BINARY_HEAP_CPP_H_

// End of file

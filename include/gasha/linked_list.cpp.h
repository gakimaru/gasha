#pragma once
#ifndef GASHA_INCLUDED_LINKED_LIST_CPP_H
#define GASHA_INCLUDED_LINKED_LIST_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// linked_list.cpp.h
// 双方向連結リストコンテナ【関数定義部】
//
// ※コンテナクラスの実体化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/linked_list.inl>//双方向連結リストコンテナ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace linked_list
{
	//----------------------------------------
	//イテレータのメソッド
	
	//演算オペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::difference_type container<OPE_TYPE>::iterator::operator-(const typename container<OPE_TYPE>::iterator& rhs) const
	{
		if (!m_value && !rhs.m_value)
			return 0;
		difference_type diff = 0;
		node_type* value = rhs.m_value;
		if (!rhs.m_isEnd)
		{
			while (value && value != m_value)
			{
				value = const_cast<node_type*>(getNextNode<ope_type>(*value));
				++diff;
			}
			if (value == m_value)
				return diff;
		}
		diff = 0;
		value = m_value;
		while (value && value != rhs.m_value)
		{
			value = const_cast<node_type*>(getNextNode<ope_type>(*value));
			--diff;
		}
		if (value == m_value)
			return diff;
		return 0;
	}
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateNext() const
	{
		node_type* prev = m_value;
		if (m_value)
			m_value = const_cast<node_type*>(getNextNode<ope_type>(*m_value));
		m_isEnd = (prev && !m_value);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updatePrev() const
	{
		if (m_isEnd)
		{
			m_value = const_cast<node_type*>(m_con->m_last);
			m_isEnd = false;
			return;
		}
		if (m_value)
			m_value = const_cast<node_type*>(getPrevNode<ope_type>(*m_value));
		m_isEnd = false;
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateForward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (step == 0)
			return;
		if (step < 0)
			return updateBackward(-step);
		std::size_t _step = static_cast<std::size_t>(step);
		node_type* prev = m_value;
		if (m_value)
			m_value = const_cast<node_type*>(getForwardNode<ope_type>(*m_value, _step));
		m_isEnd = (prev && !m_value && _step == 0);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateBackward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (step <= 0)
			return updateForward(-step);
		std::size_t _step = static_cast<std::size_t>(step);
		if (_step > 0 && m_isEnd)
		{
			m_value = const_cast<node_type*>(m_con->m_last);
			--_step;
		}
		if (m_value)
			m_value = const_cast<node_type*>(getBackwardNode<ope_type>(*m_value, _step));
		m_isEnd = false;
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = false;
		if (m_value)
		{
			++(*this);
		}
		else
		{
			if (rhs.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_first);
		}
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = false;
		if (m_value)
		{
			++(*this);
		}
		else
		{
			if (rhs.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_first);
		}
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(false)
	{
		if (m_value)
		{
			++(*this);
		}
		else
		{
			if (obj.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_first);
		}
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(false)
	{
		if (m_value)
		{
			++(*this);
		}
		else
		{
			if (obj.m_isEnd)
				m_value = const_cast<value_type *>(m_con->m_first);
		}
	}
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const container& con, const bool is_end) :
		m_con(&con),
		m_value(nullptr),
		m_isEnd(is_end)
	{
		if (!is_end)
		{
			m_value = const_cast<value_type*>(con.m_first);
			if (!m_value)
				m_isEnd = true;
		}
	}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const container& con, typename container<OPE_TYPE>::value_type* value, const bool is_end) :
		m_con(&con),
		m_value(value),
		m_isEnd(is_end)
	{}

	//----------------------------------------
	//リバースイテレータのメソッド
	
	//演算オペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::difference_type container<OPE_TYPE>::reverse_iterator::operator-(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		if (!m_value && !rhs.m_value)
			return 0;
		difference_type diff = 0;
		node_type* value = m_value;
		if (!m_isEnd)
		{
			while (value && value != rhs.m_value)
			{
				value = const_cast<node_type*>(getNextNode<ope_type>(*value));
				++diff;
			}
			if (value == m_value)
				return diff;
		}
		diff = 0;
		value = rhs.m_value;
		while (value && value != m_value)
		{
			value = const_cast<node_type*>(getNextNode<ope_type>(*value));
			--diff;
		}
		if (value == m_value)
			return diff;
		return 0;
	}
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateNext() const
	{
		node_type* prev = m_value;
		if (m_value)
			m_value = const_cast<node_type*>(getPrevNode<ope_type>(*m_value));
		m_isEnd = (prev && !m_value);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updatePrev() const
	{
		if (m_isEnd)
		{
			m_value = const_cast<node_type*>(m_con->m_first);
			m_isEnd = false;
			return;
		}
		if (m_value)
			m_value = const_cast<node_type*>(getNextNode<ope_type>(*m_value));
		m_isEnd = false;
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateForward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (step == 0)
			return;
		if (step < 0)
			return updateBackward(-step);
		std::size_t _step = static_cast<std::size_t>(step);
		node_type* prev = m_value;
		if (m_value)
			m_value = const_cast<node_type*>(getBackwardNode<ope_type>(*m_value, _step));
		m_isEnd = (prev && !m_value && _step == 0);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateBackward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (step <= 0)
			return updateForward(-step);
		std::size_t _step = static_cast<std::size_t>(step);
		if (_step > 0 && m_isEnd)
		{
			m_value = const_cast<node_type*>(m_con->m_first);
			--_step;
		}
		if (m_value)
			m_value = const_cast<node_type*>(getForwardNode<ope_type>(*m_value, _step));
		m_isEnd = false;
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = false;
		if (m_value)
		{
			++(*this);
		}
		else
		{
			if (rhs.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_last);
		}
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = false;
		if (m_value)
		{
			++(*this);
		}
		else
		{
			if (rhs.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_last);
		}
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(false)
	{
		if (m_value)
		{
			++(*this);
		}
		else
		{
			if (obj.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_last);
		}
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{
		if (m_value)
		{
			++(*this);
		}
		else
		{
			if (obj.m_isEnd)
				m_value = const_cast<value_type*>(m_con->m_last);
		}
	}
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const container& con, const bool is_end) :
		m_con(&con),
		m_value(nullptr),
		m_isEnd(is_end)
	{
		if (!is_end)
		{
			m_value = const_cast<value_type*>(m_con->m_last);
			if (!m_value)
				m_isEnd = true;
		}
	}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const container& con, typename container<OPE_TYPE>::value_type* value, const bool is_end) :
		m_con(&con),
		m_value(value),
		m_isEnd(is_end)
	{}

	//----------------------------------------
	//コンテナ本体のメソッド

	//全ノードをクリア
	//※先頭ノードを返す
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::clear()
	{
		node_type* first = m_first;
		m_first = nullptr;
		m_last = nullptr;
		return first;
	}

	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::container(container&& con) :
		m_first(con.m_first),
		m_last(con.m_last)
	{}
		
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::container(const container& con) :
		m_first(con.m_first),
		m_last(con.m_last)
	{}

	//デストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::
		~container()
	{}

}//namespace linked_list

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LINKED_LIST_CPP_H

// End of file

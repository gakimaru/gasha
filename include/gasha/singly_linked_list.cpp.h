#pragma once
#ifndef __SINGLY_LINKED_LIST_CPP_H_
#define __SINGLY_LINKED_LIST_CPP_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// singly_linked_list.cpp.h
// 片方向連結リストコンテナ【関数定義部】
//
// ※コンテナクラスの実体化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/singly_linked_list.inl>//片方向連結リストコンテナ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace singly_linked_list
{
	//----------------------------------------
	//イテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateNext() const
	{
		if (m_value == reinterpret_cast<node_type*>(BEFORE_BEGIN))
		{
			m_value = m_con->m_first;
			m_isEnd = (m_con->m_first == nullptr);
			return;
		}
		node_type* prev = m_value;
		if (m_value)
			m_value = const_cast<node_type*>(getNextNode<ope_type>(*m_value));
		m_isEnd = (prev && !m_value);
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updatePrev() const
	{
		if (m_value == reinterpret_cast<node_type*>(BEFORE_BEGIN))
		{
			m_value = nullptr;
			m_isEnd = false;
			return;
		}
		if (m_isEnd)
		{
			m_value = const_cast<node_type*>(m_con->m_last);
			m_isEnd = false;
			return;
		}
		if (m_value)
			m_value = const_cast<node_type*>(getPrevNode<ope_type>(*m_value, m_con->m_first, nullptr));
		m_isEnd = false;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateForward(const std::size_t step) const
	{
		if (step == 0)
			return;
		std::size_t _step = step;
		node_type* prev = m_value;
		if (m_value == reinterpret_cast<node_type*>(BEFORE_BEGIN))
		{
			m_value = m_con->m_first;
			--_step;
		}
		if (m_value)
			m_value = const_cast<node_type*>(getForwardNode<ope_type>(*m_value, _step));
		m_isEnd = (prev && !m_value && _step == 0);
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateBackward(const std::size_t step) const
	{
		if (step == 0)
			return;
		if (m_value == reinterpret_cast<node_type*>(BEFORE_BEGIN))
		{
			m_value = nullptr;
			m_isEnd = false;
			return;
		}
		std::size_t _step = step;
		if (_step > 0 && m_isEnd)
		{
			m_value = const_cast<node_type*>(m_con->m_last);
			--_step;
		}
		if (m_value)
			m_value = const_cast<node_type*>(getBackwardNode<ope_type>(*m_value, _step, m_con->m_first, nullptr));
		m_isEnd = false;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateBeforeBegin() const
	{
		m_value = reinterpret_cast<node_type*>(BEFORE_BEGIN);
		m_isEnd = false;
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator&& rhs)
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
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const iterator& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
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
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator&& obj) :
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
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
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
				m_value = const_cast<value_type*>(m_con->m_first);
		}
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
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

#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	//----------------------------------------
	//リバースイテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateNext() const
	{
		node_type* prev = m_value;
		if (m_value)
			m_value = const_cast<node_type*>(getPrevNode<ope_type>(*m_value, m_con->m_first, nullptr));
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
	void container<OPE_TYPE>::reverse_iterator::updateForward(const std::size_t step) const
	{
		std::size_t _step = step;
		node_type* prev = m_value;
		if (m_value)
			m_value = const_cast<node_type*>(getBackwardNode<ope_type>(*m_value, _step, m_con->m_first, nullptr));
		m_isEnd = (prev && !m_value && _step == 0);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateBackward(const std::size_t step) const
	{
		std::size_t _step = step;
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
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = false;
		if (m_value)
		{
			if (m_value == reinterpret_cast<value_type*>(BEFORE_BEGIN))
			{
				m_value = m_con->m_first;
				if (!m_value)
					m_isEnd = true;
			}
			else
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
			if (m_value == reinterpret_cast<value_type*>(BEFORE_BEGIN))
			{
				m_value = m_con->m_first;
				if (!m_value)
					m_isEnd = true;
			}
			else
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
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(false)
	{
		if (m_value)
		{
			if (m_value == reinterpret_cast<value_type*>(BEFORE_BEGIN))
			{
				m_value = m_con->m_first;
				if (!m_value)
					m_isEnd = true;
			}
			else
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
			if (m_value == reinterpret_cast<value_type*>(BEFORE_BEGIN))
			{
				m_value = m_con->m_first;
				if (!m_value)
					m_isEnd = true;
			}
			else
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
		m_con(con),
		m_value(value),
		m_isEnd(is_end)
	{}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR

	//----------------------------------------
	//コンテナ本体のメソッド
	
	//全ノードをクリア
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
	container<OPE_TYPE>::container(const container&& con) :
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
	container<OPE_TYPE>::~container()
	{}

}//namespace singly_linked_list

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__SINGLY_LINKED_LIST_CPP_H_

// End of file

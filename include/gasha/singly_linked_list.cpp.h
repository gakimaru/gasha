#pragma once
#ifndef GASHA_INCLUDED_SINGLY_LINKED_LIST_CPP_H
#define GASHA_INCLUDED_SINGLY_LINKED_LIST_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// singly_linked_list.cpp.h
// 片方向連結リストコンテナ【関数／実体定義部】
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

#include <gasha/singly_linked_list.inl>//片方向連結リストコンテナ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace singly_linked_list
{
	//----------------------------------------
	//イテレータのメソッド
	
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
	//演算オペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::difference_type container<OPE_TYPE>::iterator::operator-(const typename container<OPE_TYPE>::iterator& rhs) const
	{
		if (!m_value && !rhs.m_value)
			return 0;
		if (!m_value && !m_isEnd)
			return 0;
		if (!rhs.m_value && !rhs.m_isEnd)
			return 0;
		if (m_value == reinterpret_cast<node_type*>(BEFORE_BEGIN) && rhs.m_value == reinterpret_cast<node_type*>(BEFORE_BEGIN))
			return 0;
		if (!rhs.m_isEnd)
		{
			difference_type diff = 0;
			const node_type* value = rhs.m_value;
			const node_type* end = m_value;
			if (value == reinterpret_cast<node_type*>(BEFORE_BEGIN))
			{
				value = m_con->m_first;
				++diff;
			}
			while (value && value != end)
			{
				value = const_cast<node_type*>(getNextNode<ope_type>(*value));
				++diff;
			}
			if (value == end)
				return diff;
		}
		{
			difference_type diff = 0;
			const node_type* value = m_value;
			const node_type* end = rhs.m_value;
			if (value == reinterpret_cast<node_type*>(BEFORE_BEGIN))
			{
				value = m_con->m_first;
				--diff;
			}
			while (value && value != end)
			{
				value = const_cast<node_type*>(getNextNode<ope_type>(*value));
				--diff;
			}
			if (value == end)
				return diff;
		}
		return 0;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE

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
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateForward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (step == 0)
			return;
		if (step < 0)
			return updateBackward(-step);
		std::size_t _step = static_cast<std::size_t>(step);
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
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateBackward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (step <= 0)
			return updateForward(-step);
		if (m_value == reinterpret_cast<node_type*>(BEFORE_BEGIN))
		{
			m_value = nullptr;
			m_isEnd = false;
			return;
		}
		std::size_t _step = static_cast<std::size_t>(step);
		if (_step > 0 && m_isEnd)
		{
			m_value = const_cast<node_type*>(m_con->m_last);
			--_step;
		}
		if (m_value)
			m_value = const_cast<node_type*>(getBackwardNode<ope_type>(*m_value, _step, m_con->m_first, nullptr));
		m_isEnd = false;
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateBeforeBegin() const
	{
		m_value = reinterpret_cast<node_type*>(BEFORE_BEGIN);
		m_isEnd = false;
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_value = rhs.m_value;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
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
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
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
	container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
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
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
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

#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	//----------------------------------------
	//リバースイテレータのメソッド
	
#ifdef GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
	//演算オペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::difference_type container<OPE_TYPE>::reverse_iterator::operator-(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		if (!m_value && !rhs.m_value)
			return 0;
		if (!m_value && !m_isEnd)
			return 0;
		if (!rhs.m_value && !rhs.m_isEnd)
			return 0;
		if (m_value == reinterpret_cast<node_type*>(BEFORE_BEGIN) && rhs.m_value == reinterpret_cast<node_type*>(BEFORE_BEGIN))
			return 0;
		if (!rhs.m_isEnd)
		{
			difference_type diff = 0;
			const node_type* value = m_value;
			const node_type* end = rhs.m_value;
			if (!value || value == reinterpret_cast<node_type*>(BEFORE_BEGIN))
			{
				value = m_con->m_first;
				++diff;
			}
			while (value && value != end)
			{
				value = const_cast<node_type*>(getNextNode<ope_type>(*value));
				++diff;
			}
			if (value == end)
				return diff;
		}
		{
			difference_type diff = 0;
			const node_type* value = rhs.m_value;
			const node_type* end = m_value;
			if (!value || value == reinterpret_cast<node_type*>(BEFORE_BEGIN))
			{
				value = m_con->m_first;
				--diff;
			}
			while (value && value != end)
			{
				value = const_cast<node_type*>(getNextNode<ope_type>(*value));
				--diff;
			}
			if (value == end)
				return diff;
		}
		return 0;
	}
#endif//GASHA_SINGLY_LINKED_LIST_ENABLE_RANDOM_ACCESS_INTERFACE
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
	void container<OPE_TYPE>::reverse_iterator::updateForward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (step == 0)
			return;
		if (step < 0)
			return updateBackward(-step);
		std::size_t _step = static_cast<std::size_t>(step);
		node_type* prev = m_value;
		if (m_value)
			m_value = const_cast<node_type*>(getBackwardNode<ope_type>(*m_value, _step, m_con->m_first, nullptr));
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
		m_con(&con),
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
	container<OPE_TYPE>::~container()
	{}

}//namespace singly_linked_list

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//片方向連結リストコンテナの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_slList(OPE_TYPE) \
	template class singly_linked_list::container<OPE_TYPE>;

//シンプル片方向連結リストコンテナの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_simpleSLList(VALUE_TYPE) \
	template class singly_linked_list::simpleContainer<VALUE_TYPE>; \
	template class singly_linked_list::container<typename singly_linked_list::simpleContainer<VALUE_TYPE>::ope>;

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

#endif//GASHA_INCLUDED_SINGLY_LINKED_LIST_CPP_H

// End of file

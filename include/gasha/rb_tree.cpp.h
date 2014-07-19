#pragma once
#ifndef GASHA_INCLUDED_RB_TREE_CPP_H
#define GASHA_INCLUDED_RB_TREE_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// rb_tree.cpp.h
// 赤黒木コンテナ【関数／実体定義部】
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

#include <gasha/rb_tree.inl>//赤黒木コンテナ【インライン関数／テンプレート関数定義部】

#include <utility>//C++11 std::move
#include <cstring>//std::memcpy()
#include <cassert>//assert()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace rb_tree
{
	//--------------------
	//赤黒木処理用スタッククラス

	//スタックにノード情報を記録
	template<class OPE_TYPE>
	typename stack_t<OPE_TYPE>::info_t* stack_t<OPE_TYPE>::push(const typename OPE_TYPE::node_type& node, const bool is_large)
	{
		assert(m_depth < DEPTH_MAX);
		info_t* stack_node = &m_array[m_depth++];
		stack_node->m_nodeRef = &node;
		stack_node->m_isLarge = is_large;
		return stack_node;
	}
	//スタックからノード情報を取得
	template<class OPE_TYPE>
	typename stack_t<OPE_TYPE>::info_t* stack_t<OPE_TYPE>::pop()
	{
		if (m_depth == 0)
			return nullptr;
		return &m_array[--m_depth];
	}
	//スタックの先頭のノード情報を参照
	//※要素が減らない
	template<class OPE_TYPE>
	typename stack_t<OPE_TYPE>::info_t* stack_t<OPE_TYPE>::top()
	{
		if (m_depth == 0)
			return nullptr;
		return &m_array[m_depth - 1];
	}
	//スタックの現在の深さを更新
	template<class OPE_TYPE>
	void stack_t<OPE_TYPE>::setDepth(const int depth)
	{
		if (depth < 0)
			m_depth = 0;
		else if (depth < m_depth)
			m_depth = depth;
	}
	//スタックの現在の深さをリセット
	template<class OPE_TYPE>
	void stack_t<OPE_TYPE>::reset()
	{
		m_depth = 0;
	}
	//スタックの現在の幅を算出
	//※「幅」＝スタックの現在の深さまでの大小連結の合計値を算出
	//※小側を-1、大側を+1として計算
	template<class OPE_TYPE>
	std::int64_t stack_t<OPE_TYPE>::calcBreadth()
	{
		std::int64_t breadth = 0;
		for (int depth = 0; depth < m_depth; ++depth)
		{
			breadth *= 2ll;
			breadth += (m_array[depth].m_isLarge ? 1ll : 0ll);
		}
		return breadth;
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	stack_t<OPE_TYPE>& stack_t<OPE_TYPE>::operator=(stack_t<OPE_TYPE>&& rhs)
	{
		m_depth = rhs.m_depth;
		if (m_depth > 0)
			std::memcpy(m_array, rhs.m_array, sizeof(info_t)* m_depth);
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	stack_t<OPE_TYPE>& stack_t<OPE_TYPE>::operator=(const stack_t<OPE_TYPE>& rhs)
	{
		//return operator=(std::move(rhs));
		m_depth = rhs.m_depth;
		if (m_depth > 0)
			std::memcpy(m_array, rhs.m_array, sizeof(info_t)* m_depth);
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	stack_t<OPE_TYPE>::stack_t(stack_t<OPE_TYPE>&& obj) :
		m_depth(obj.m_depth)
	{
		if (m_depth > 0)
			std::memcpy(m_array, obj.m_array, sizeof(info_t)* m_depth);
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	stack_t<OPE_TYPE>::stack_t(const stack_t<OPE_TYPE>& obj) :
		m_depth(obj.m_depth)
	{
		if (m_depth > 0)
			std::memcpy(m_array, obj.m_array, sizeof(info_t)* m_depth);
	}
	
	//----------------------------------------
	//イテレータのメソッド
	
	//演算オペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::difference_type container<OPE_TYPE>::iterator::operator-(const iterator& rhs) const
	{
		if (!m_con->m_root)
			return 0;
		if (!m_value && !rhs.m_value)
			return 0;
		if (!m_value && !m_isEnd)
			return 0;
		if (!rhs.m_value && !rhs.m_isEnd)
			return 0;
		stack_t<OPE_TYPE> stack;
		if (!rhs.m_isEnd)
		{
			stack.reset();
			difference_type diff = 0;
			const node_type* value = searchNode<ope_type>(m_con->m_root, *rhs.m_value, stack);
			const node_type* end = m_value;
			while (value && value != end)
			{
				value = const_cast<node_type*>(getNextNode<ope_type>(*value, stack));
				++diff;
			}
			if (value == end)
				return diff;
		}
		{
			stack.reset();
			difference_type diff = 0;
			const node_type* value = searchNode<ope_type>(m_con->m_root, *m_value, stack);
			const node_type* end = rhs.m_value;
			while (value && value != end)
			{
				value = const_cast<node_type*>(getNextNode<ope_type>(*value, stack));
				--diff;
			}
			if (value == end)
				return diff;
		}
		return 0;
	}
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateNext() const
	{
		if (!m_con->m_root)
			return;
		value_type* prev = m_value;
		if (m_value)
			m_value = const_cast<value_type*>(getNextNode<ope_type>(*m_value, m_stack));
		m_isEnd = (prev && !m_value);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updatePrev() const
	{
		if (!m_con->m_root)
			return;
		if (m_isEnd)
		{
			m_stack.reset();
			m_value = const_cast<value_type*>(getLargestNode<ope_type>(m_con->m_root, m_stack));
			m_isEnd = false;
			return;
		}
		if (m_value)
			m_value = const_cast<value_type*>(getPrevNode<ope_type>(*m_value, m_stack));
		m_isEnd = false;
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateForward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (!m_con->m_root || !m_value)
			return;
		if (step == 0)
			return;
		if (step < 0)
			updateBackward(-step);
		difference_type _step = step;
		value_type* prev = m_value;
		while (_step > 0 && m_value)
		{
			m_value = const_cast<value_type*>(getNextNode<ope_type>(*m_value, m_stack));
			--_step;
		}
		m_isEnd = (prev && !m_value && _step == 0);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateBackward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (!m_con->m_root)
			return;
		if (step <= 0)
			updateForward(-step);
		difference_type _step = step;
		if (_step > 0 && m_isEnd)
		{
			m_stack.reset();
			m_value = const_cast<value_type*>(getLargestNode<ope_type>(m_con->m_root, m_stack));
			--_step;
		}
		while (_step > 0 && m_value)
		{
			m_value = const_cast<value_type*>(getPrevNode<ope_type>(*m_value, m_stack));
			--_step;
		}
		m_isEnd = false;
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_stack = std::move(rhs.m_stack);
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
			m_stack = std::move(rhs.m_stack);
			++(*this);
		}
		else
		{
			m_stack.reset();
			if (rhs.m_isEnd && m_con->m_root)
				m_value = const_cast<node_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
		}
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::iterator& rhs)
	{
		m_stack = rhs.m_stack;
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
			m_stack = rhs.m_stack;
			++(*this);
		}
		else
		{
			m_stack.reset();
			if (rhs.m_isEnd && m_con->m_root)
				m_value = const_cast<node_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
		}
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::iterator&& obj) :
		m_stack(std::move(obj.m_stack)),
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_stack(),
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(false)
	{
		if (m_value)
		{
			m_stack = std::move(obj.m_stack);
			++(*this);
		}
		else
		{
			m_stack.reset();
			if (obj.m_isEnd && m_con->m_root)
				m_value = const_cast<node_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
		}
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_stack(obj.m_stack),
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_stack(),
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(false)
	{
		if (m_value)
		{
			m_stack = obj.m_stack;
			++(*this);
		}
		else
		{
			m_stack.reset();
			if (obj.m_isEnd && m_con->m_root)
				m_value = const_cast<node_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
		}
	}
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const container& con, const bool is_end) :
		m_stack(),
		m_con(&con),
		m_value(nullptr),
		m_isEnd(is_end)
	{
		if (!is_end && m_con->m_root)
		{
			m_value = const_cast<value_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
			if (!m_value)
				m_isEnd = true;
		}
	}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(stack_type&& stack, const container& con, typename container<OPE_TYPE>::value_type* value, const bool is_end) :
		m_stack(std::move(stack)),
		m_con(&con),
		m_value(value),
		m_isEnd(is_end)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const stack_type& stack, const container& con, typename container<OPE_TYPE>::value_type* value, const bool is_end) :
		m_stack(stack),
		m_con(&con),
		m_value(value),
		m_isEnd(is_end)
	{}

	//----------------------------------------
	//リバースイテレータのメソッド
	
	//演算オペレータ
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::difference_type container<OPE_TYPE>::reverse_iterator::operator-(const reverse_iterator& rhs)
	{
		if (!m_con->m_root)
			return 0;
		if (!m_value && !rhs.m_value)
			return 0;
		if (!m_value && !m_isEnd)
			return 0;
		if (!rhs.m_value && !rhs.m_isEnd)
			return 0;
		stack_t<OPE_TYPE> stack;
		if (!rhs.m_isEnd)
		{
			stack.reset();
			difference_type diff = 0;
			const node_type* value = searchNode<ope_type>(m_con->m_root, *rhs.m_value, stack);
			const node_type* end = m_value;
			while (value && value != end)
			{
				value = const_cast<node_type*>(getPrevNode<ope_type>(*value, stack));
				++diff;
			}
			if (value == end)
				return diff;
		}
		{
			stack.reset();
			difference_type diff = 0;
			const node_type* value = searchNode<ope_type>(m_con->m_root, *m_value, stack);
			const node_type* end = rhs.m_value;
			while (value && value != end)
			{
				value = const_cast<node_type*>(getPrevNode<ope_type>(*value, stack));
				--diff;
			}
			if (value == end)
				return diff;
		}
		return 0;
	}
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateNext() const
	{
		if (!m_con->m_root)
			return;
		value_type* prev = m_value;
		if (m_value)
			m_value = const_cast<value_type*>(getPrevNode<ope_type>(*m_value, m_stack));
		m_isEnd = (prev && !m_value);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updatePrev() const
	{
		if (!m_con->m_root)
			return;
		if (m_isEnd)
		{
			m_stack.reset();
			m_value = const_cast<value_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
			m_isEnd = false;
			return;
		}
		if (!m_value)
			return;
		m_value = const_cast<value_type*>(getNextNode<ope_type>(*m_value, m_stack));
		m_isEnd = false;
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateForward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (!m_con->m_root || !m_value)
			return;
		if (step == 0)
			return;
		if (step < 0)
			updateBackward(-step);
		difference_type _step = step;
		value_type* prev = m_value;
		while (_step > 0 && m_value)
		{
			m_value = const_cast<value_type*>(getPrevNode<ope_type>(*m_value, m_stack));
			--_step;
		}
		m_isEnd = (prev && !m_value && _step == 0);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateBackward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (!m_con->m_root)
			return;
		if (step <= 0)
			updateForward(-step);
		difference_type _step = step;
		if (_step > 0 && m_isEnd)
		{
			m_stack.reset();
			m_value = const_cast<value_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
			--_step;
		}
		while (_step > 0 && m_value)
		{
			m_value = const_cast<value_type*>(getNextNode<ope_type>(*m_value, m_stack));
			--_step;
		}
		m_isEnd = false;
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_stack = std::move(rhs.m_stack);
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
			m_stack = std::move(rhs.m_stack);
			++(*this);
		}
		else
		{
			m_stack.reset();
			if (rhs.m_isEnd && m_con->m_root)
				m_value = const_cast<value_type*>(getLargestNode<ope_type>(m_con->m_root, m_stack));
		}
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_stack = rhs.m_stack;
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
			m_stack = rhs.m_stack;
			++(*this);
		}
		else
		{
			m_stack.reset();
			if (rhs.m_isEnd && m_con->m_root)
				m_value = const_cast<value_type*>(getLargestNode<ope_type>(m_con->m_root, m_stack));
		}
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_stack(std::move(obj.m_stack)),
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(typename container<OPE_TYPE>::iterator&& obj) :
		m_stack(),
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(false)
	{
		if (m_value)
		{
			m_stack = std::move(obj.m_stack);
			++(*this);
		}
		else
		{
			m_stack.reset();
			if (obj.m_isEnd && m_con->m_root)
				m_value = const_cast<value_type*>(getLargestNode<ope_type>(m_con->m_root, m_stack));
		}
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_stack(obj.m_stack),
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_stack(),
		m_con(obj.m_con),
		m_value(obj.m_value),
		m_isEnd(obj.m_isEnd)
	{
		if (m_value)
		{
			m_stack = obj.m_stack;
			++(*this);
		}
		else
		{
			m_stack.reset();
			if (obj.m_isEnd && m_con->m_root)
				m_value = const_cast<value_type*>(getLargestNode<ope_type>(m_con->m_root, m_stack));
		}
	}
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const container& con, const bool is_end) :
		m_stack(),
		m_con(&con),
		m_value(nullptr),
		m_isEnd(is_end)
	{
		if (!is_end && m_con->m_root)
		{
			m_value = const_cast<value_type*>(getLargestNode<ope_type>(m_con->m_root, m_stack));
			if (!m_value)
				m_isEnd = true;
		}
	}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(stack_type&& stack, const container& con, typename container<OPE_TYPE>::value_type* value, const bool is_end) :
		m_stack(std::move(stack)),
		m_con(&con),
		m_value(value),
		m_isEnd(is_end)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const stack_type& stack, const container& con, typename container<OPE_TYPE>::value_type* value, const bool is_end) :
		m_stack(stack),
		m_con(&con),
		m_value(value),
		m_isEnd(is_end)
	{}

	//----------------------------------------
	//コンテナ本体のメソッド

	//全ノードをクリア
	//※根ノードを返す
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::clear()
	{
		node_type* root = m_root;
		m_root = nullptr;
		return root;
	}

	//キーを探索（共通）
	template<class OPE_TYPE>
	const typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::_findValue(const typename container<OPE_TYPE>::key_type key, const match_type_t type) const
	{
		stack_type stack;
		return searchNode<ope_type>(m_root, key, stack, type);
	}

	//キーを探索（共通）
	template<class OPE_TYPE>
	void container<OPE_TYPE>::_find(typename container<OPE_TYPE>::iterator& ite, const typename container<OPE_TYPE>::key_type key, const match_type_t type) const
	{
		ite.m_value = const_cast<node_type*>(searchNode<ope_type>(m_root, key, ite.m_stack, type));
		ite.m_isEnd = (ite.m_value == nullptr);
	}

	//キーが一致する範囲を返す
	template<class OPE_TYPE>
	void container<OPE_TYPE>::_equal_range(typename container<OPE_TYPE>::iterator& ite, const typename container<OPE_TYPE>::key_type key) const
	{
		ite.m_value = const_cast<node_type*>(searchNode<ope_type>(m_root, key, ite.m_stack, FOR_MATCH));
		ite.m_isEnd = (ite.m_value == nullptr);
		while (ite.m_value && ope_type::getKey(*ite) == key)
			++ite;
	}
	
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::container(container&& con) :
		m_root(con.m_root)
	{}
	
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::container(const container& con) :
		m_root(con.m_root)
	{}
	
	//デストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::~container()
	{}

}//namespace rb_tree

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//赤黒木コンテナの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_rbTree(OPE_TYPE) \
	template class GASHA_ rb_tree::stack_t<OPE_TYPE>; \
	template class GASHA_ rb_tree::container<OPE_TYPE>;

//シンプル赤黒木コンテナの明示的なインスタンス化用マクロ
//※キー型を省略する場合
#define GASHA_INSTANCING_simpleRBTree(VALUE_TYPE) \
	template class GASHA_ rb_tree::simpleContainer<VALUE_TYPE>; \
	template class GASHA_ rb_tree::container<typename GASHA_ rb_tree::simpleContainer<VALUE_TYPE>::ope>; \
	template class GASHA_ rb_tree::stack_t<typename GASHA_ rb_tree::simpleContainer<VALUE_TYPE>::ope>;
//※キー型を指定する場合
#define GASHA_INSTANCING_simpleRBTree_withKey(VALUE_TYPE, KEY_TYPE) \
	template class GASHA_ rb_tree::simpleContainer<VALUE_TYPE, KEY_TYPE>; \
	template class GASHA_ rb_tree::container<typename GASHA_ rb_tree::simpleContainer<VALUE_TYPE, KEY_TYPE>::ope>; \
	template class GASHA_ rb_tree::stack_t<typename GASHA_ rb_tree::simpleContainer<VALUE_TYPE, KEY_TYPE>::ope>;

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

#endif//GASHA_INCLUDED_RB_TREE_CPP_H

// End of file

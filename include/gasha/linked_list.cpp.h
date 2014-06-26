#pragma once
#ifndef __LINKED_LIST_CPP_H_
#define __LINKED_LIST_CPP_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// linked_list.cpp.h
// 双方向連結リストコンテナ【関実装部】
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
	
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateNext() const
	{
		node_type* prev = m_node;
		if (m_node)
			m_node = const_cast<node_type*>(getNextNode<ope_type>(*m_node));
		m_isEnd = (prev && !m_node);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updatePrev() const
	{
		if (m_isEnd)
		{
			m_node = const_cast<node_type*>(m_con->m_last);
			m_isEnd = false;
			return;
		}
		if (m_node)
			m_node = const_cast<node_type*>(getPrevNode<ope_type>(*m_node));
		m_isEnd = false;
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateForward(const std::size_t step) const
	{
		std::size_t _step = step;
		node_type* prev = m_node;
		if (m_node)
			m_node = const_cast<node_type*>(getForwardNode<ope_type>(*m_node, _step));
		m_isEnd = (prev && !m_node && _step == 0);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateBackward(const std::size_t step) const
	{
		std::size_t _step = step;
		if (_step > 0 && m_isEnd)
		{
			m_node = const_cast<node_type*>(m_con->m_last);
			--_step;
		}
		if (m_node)
			m_node = const_cast<node_type*>(getBackwardNode<ope_type>(*m_node, _step));
		m_isEnd = false;
	}

	//----------------------------------------
	//リバースイテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateNext() const
	{
		node_type* prev = m_node;
		if (m_node)
			m_node = const_cast<node_type*>(getPrevNode<ope_type>(*m_node));
		m_isEnd = (prev && !m_node);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updatePrev() const
	{
		if (m_isEnd)
		{
			m_node = const_cast<node_type*>(m_con->m_first);
			m_isEnd = false;
			return;
		}
		if (m_node)
			m_node = const_cast<node_type*>(getNextNode<ope_type>(*m_node));
		m_isEnd = false;
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateForward(const std::size_t step) const
	{
		std::size_t _step = step;
		node_type* prev = m_node;
		if (m_node)
			m_node = const_cast<node_type*>(getBackwardNode<ope_type>(*m_node, _step));
		m_isEnd = (prev && !m_node && _step == 0);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateBackward(const std::size_t step) const
	{
		std::size_t _step = step;
		if (_step > 0 && m_isEnd)
		{
			m_node = const_cast<node_type*>(m_con->m_first);
			--_step;
		}
		if (m_node)
			m_node = const_cast<node_type*>(getForwardNode<ope_type>(*m_node, _step));
		m_isEnd = false;
	}

	//----------------------------------------
	//コンテナ本体のメソッド

	//配列の再割り当て
	//※ポインタと配列要素数指定版
//	template<class OPE_TYPE>
//	void container<OPE_TYPE>::assignArray(value_type* array, const typename container<OPE_TYPE>::size_type max_size, const int size)
//	{
//	}
	
}//namespace linked_list

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__LINKED_LIST_CPP_H_

// End of file

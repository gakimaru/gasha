#pragma once
#ifndef __RB_TREE_CPP_H_
#define __RB_TREE_CPP_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// rb_tree.cpp.h
// 赤黒木コンテナ【関実装部】
//
// ※コンテナクラスの実体化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/rb_tree.inl>//赤黒木コンテナ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace rb_tree
{
	//----------------------------------------
	//イテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateNext() const
	{
		value_type* prev = m_value;
		m_value = const_cast<value_type*>(getNextNode<ope_type>(m_value, m_stack));
		m_isEnd = (prev && !m_value);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updatePrev() const
	{
		if (m_isEnd)
		{
			m_stack.reset();
			m_value = const_cast<value_type*>(getLargestNode<ope_type>(m_con->m_root, m_stack));
			m_isEnd = false;
			return;
		}
		m_value = const_cast<value_type*>(getPrevNode<ope_type>(m_value, m_stack));
		m_isEnd = false;
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateForward(const int step) const
	{
		int _step = step;
		value_type* prev = m_value;
		while (_step > 0 && m_value)
		{
			m_value = const_cast<value_type*>(getNextNode<ope_type>(m_value, m_stack));
			--_step;
		}
		m_isEnd = (prev && !m_value && _step == 0);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateBackward(const int step) const
	{
		int _step = step;
		if (_step > 0 && m_isEnd)
		{
			m_stack.reset();
			m_value = const_cast<value_type*>(getLargestNode<ope_type>(m_root->m_root, m_stack));
			--_step;
		}
		while (_step > 0 && m_value)
		{
			m_value = const_cast<value_type*>(getPrevNode<ope_type>(m_value, m_stack));
			--_step;
		}
		m_isEnd = false;
	}

	//----------------------------------------
	//リバースイテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateNext() const
	{
		value_type* prev = m_value;
		m_value = const_cast<value_type*>(getPrevNode<ope_type>(m_value, m_stack));
		m_isEnd = (prev && !m_value);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updatePrev() const
	{
		if (m_isEnd)
		{
			m_stack.reset();
			m_value = const_cast<value_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
			m_isEnd = false;
			return;
		}
		m_value = const_cast<value_type*>(getNextNode<ope_type>(m_value, m_stack));
		m_isEnd = false;
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateForward(const int step) const
	{
		int _step = step;
		value_type* prev = m_value;
		while (_step > 0 && m_value)
		{
			m_value = const_cast<value_type*>(getPrevNode<ope_type>(m_value, m_stack));
			--_step;
		}
		m_isEnd = (prev && !m_value && _step == 0);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateBackward(const int step) const
	{
		int _step = step;
		if (_step > 0 && m_isEnd)
		{
			m_stack.reset();
			m_value = const_cast<value_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
			--_step;
		}
		while (_step > 0 && m_value)
		{
			m_value = const_cast<value_type*>(getNextNode<ope_type>(m_value, m_stack));
			--_step;
		}
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
	
}//namespace rb_tree

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__RB_TREE_CPP_H_

// End of file

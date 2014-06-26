#pragma once
#ifndef __RING_BUFFER_CPP_H_
#define __RING_BUFFER_CPP_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// ring_buffer.cpp.h
// リングバッファコンテナ【関実装部】
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

#pragma once
#ifndef __RING_BUFFER_INL_
#define __RING_BUFFER_INL_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// ring_buffer.inl
// リングバッファコンテナ【インライン関数／テンプレート関数定義部】
//
// ※コンテナクラスの操作が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/ring_buffer.h>//リングバッファコンテナ【宣言部】

#include <gasha/intro_sort.h>//イントロソート
#include <gasha/insertion_sort.h>//挿入ソート

#include <gasha/linear_search.h>//線形探索
#include <gasha/binary_search.h>//二分探索

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace ring_buffer
{
	//--------------------
	//イテレータのインライン関数
	
	//基本オペレータ
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator[](const int logical_index) const
	{
		iterator ite(*m_con, false);
		ite.update(logical_index);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator[](const int logical_index)
	{
		iterator ite(*m_con, false);
		ite.update(logical_index);
		return ite;
	}
	//比較オペレータ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator==(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_logicalIndex == rhs.m_logicalIndex;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator!=(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_logicalIndex != rhs.m_logicalIndex;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator>(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_logicalIndex > rhs.m_logicalIndex;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator>=(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_logicalIndex >= rhs.m_logicalIndex;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator<(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_logicalIndex < rhs.m_logicalIndex;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator<=(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_logicalIndex <= rhs.m_logicalIndex;
	}
	//演算オペレータ
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator++() const
	{
		addIndexAndUpdate(1);
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator--() const
	{
		addIndexAndUpdate(-1);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator++()
	{
		addIndexAndUpdate(1);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator--()
	{
		addIndexAndUpdate(-1);
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator++(int) const
	{
		iterator ite(*this);
		++(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator--(int) const
	{
		iterator ite(*this);
		--(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator++(int)
	{
		iterator ite(*this);
		++(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator--(int)
	{
		iterator ite(*this);
		--(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator+=(const int rhs) const
	{
		addIndexAndUpdate(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator-=(const int rhs) const
	{
		addIndexAndUpdate(-rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator+=(const int rhs)
	{
		addIndexAndUpdate(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator-=(const int rhs)
	{
		addIndexAndUpdate(-rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator+(const int rhs) const
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator-(const int rhs) const
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator+(const int rhs)
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator-(const int rhs)
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline int container<OPE_TYPE>::iterator::operator-(const typename container<OPE_TYPE>::iterator& rhs)
	{
		if (m_logicalIndex == INVALID_INDEX || rhs.m_logicalIndex == INVALID_INDEX || m_logicalIndex < rhs.m_logicalIndex)
			return 0;
		return static_cast<int>(m_logicalIndex - rhs.m_logicalIndex);
	}
	//アクセッサ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isExist() const
	{
		return m_logicalIndex != INVALID_INDEX && m_logicalIndex < m_con->m_size;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isEnabled() const
	{ 
		return m_logicalIndex != INVALID_INDEX;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isEnd() const//終端か？
	{
		return m_logicalIndex == m_con->m_size;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::iterator::getRealIndex() const//物理インデックス
	{
		return m_logicalIndex == INVALID_INDEX ? INVALID_INDEX : m_con->_toRealIndex(m_logicalIndex);
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::iterator::getIndex() const//論理インデックス
	{
		return m_logicalIndex;
	}
	//メソッド
	template<class OPE_TYPE>
	inline void container<OPE_TYPE>::iterator::addIndexAndUpdate(const int add) const
	{
		update(m_logicalIndex + add);
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		update(m_logicalIndex);
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		update(m_logicalIndex);
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(nullptr)
	{
		update(m_logicalIndex);
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(nullptr)
	{
		update(m_logicalIndex);
	}
	//コンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::iterator::iterator(const container<OPE_TYPE>& con, const bool is_end) :
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
	inline container<OPE_TYPE>::iterator::iterator(const container<OPE_TYPE>& con, const typename container<OPE_TYPE>::index_type logical_index) :
		m_con(&con),
		m_logicalIndex(INVALID_INDEX),
		m_value(nullptr)
	{
		update(logical_index);
	}
	
	//--------------------
	//リバースイテレータのインライン関数
	
	//基本オペレータ
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator[](const int logical_index) const
	{
		reverse_iterator ite(*m_con, false);
		ite.update(m_con->m_size - logical_index);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator[](const int logical_index)
	{
		reverse_iterator ite(*m_con, false);
		ite.update(m_con->m_size - logical_index);
		return ite;
	}
	//比較オペレータ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator==(const reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_logicalIndex == m_logicalIndex;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator!=(const reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_logicalIndex != m_logicalIndex;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator>(const reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_logicalIndex > m_logicalIndex;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator>=(const reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_logicalIndex >= m_logicalIndex;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator<(const reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_logicalIndex < m_logicalIndex;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator<=(const reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_logicalIndex <= m_logicalIndex;
	}
	//演算オペレータ
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator++() const
	{
		addIndexAndUpdate(1);
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator--() const
	{
		addIndexAndUpdate(-1);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator++()
	{
		addIndexAndUpdate(1);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator--()
	{
		addIndexAndUpdate(-1);
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator++(int) const
	{
		reverse_iterator ite(*this);
		++(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator--(int) const
	{
		reverse_iterator ite(*this);
		--(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator++(int)
	{
		reverse_iterator ite(*this);
		++(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator--(int)
	{
		reverse_iterator ite(*this);
		--(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator+=(const int rhs) const
	{
		addIndexAndUpdate(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator-=(const int rhs) const
	{
		addIndexAndUpdate(-rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator+=(const int rhs)
	{
		addIndexAndUpdate(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator-=(const int rhs)
	{
		addIndexAndUpdate(-rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator+(const int rhs) const
	{
		reverse_iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator-(const int rhs) const
	{
		reverse_iterator ite(*this);
		ite -= rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator+(const int rhs)
	{
		reverse_iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator-(const int rhs)
	{
		reverse_iterator ite(*this);
		ite -= rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline int container<OPE_TYPE>::reverse_iterator::operator-(const reverse_iterator& rhs)
	{
		if (m_logicalIndex == INVALID_INDEX || rhs.m_logicalIndex == INVALID_INDEX || rhs.m_logicalIndex < m_logicalIndex)
			return 0;
		return static_cast<int>(rhs.m_logicalIndex - m_logicalIndex);
	}
	//アクセッサ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isExist() const
	{
		return m_logicalIndex != INVALID_INDEX && m_logicalIndex > 0;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isEnabled() const
	{
		return m_logicalIndex != INVALID_INDEX;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isEnd() const//終端か？
	{
		return m_logicalIndex == 0;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::reverse_iterator::getRealIndex() const//物理インデックス
	{
		return m_logicalIndex == INVALID_INDEX ? INVALID_INDEX : m_con->_toRealIndex(m_logicalIndex);
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::reverse_iterator::getIndex() const//論理インデックス
	{
		return m_logicalIndex - 1;
	}
	//メソッド
	template<class OPE_TYPE>
	inline void container<OPE_TYPE>::reverse_iterator::addIndexAndUpdate(const int add) const
	{
		update(m_logicalIndex - add);
	}
	//ベースを取得
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::reverse_iterator::base() const
	{
		iterator ite(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::reverse_iterator::base()
	{
		iterator ite(*this);
		return ite;
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		update(m_logicalIndex);
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		update(m_logicalIndex);
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(nullptr)
	{
		update(m_logicalIndex);
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(nullptr)
	{
		update(m_logicalIndex);
	}
	//コンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::reverse_iterator::reverse_iterator(const container<OPE_TYPE>& con, const bool is_end) :
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
	inline container<OPE_TYPE>::reverse_iterator::reverse_iterator(const  container<OPE_TYPE>& con, const typename  container<OPE_TYPE>::index_type logical_index) :
		m_con(&con),
		m_logicalIndex(INVALID_INDEX),
		m_value(nullptr)
	{
		update(logical_index);
	}
	
	//----------------------------------------
	//コンテナ本体のメソッド

	//メソッド：要素アクセス系
	//※範囲チェックなし

	//論理インデックスを物理インデックスに変換
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::_toRealIndex(const typename container<OPE_TYPE>::index_type logical_index) const
	{
		const index_type real_index = m_offset + logical_index;
		return real_index < m_maxSize ? real_index : real_index - m_maxSize;
	}
	//物理インデックスを論理インデックスに変換
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::_toLogicalIndex(const typename container<OPE_TYPE>::index_type real_index) const
	{
		return real_index > m_offset ? real_index - m_offset : real_index + m_maxSize - m_offset;
	}
	//先頭の新規インデックス
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::size_type container<OPE_TYPE>::_frontNewRealIndex() const
	{
		return m_offset == 0 ? m_maxSize - 1 : m_offset - 1;
	}
	//末尾の新規インデックス
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::size_type container<OPE_TYPE>::_backNewRealIndex() const
	{
		const index_type new_real_index = m_offset + m_size;
		return new_real_index < m_maxSize ? new_real_index : new_real_index - m_maxSize;
	}
	
	//配列の再割り当て
//	template<class OPE_TYPE>
//	template<std::size_t N>
//	inline void container<OPE_TYPE>::assignArray(typename container<OPE_TYPE>::value_type(&array)[N], const int size)
//	{
//	}

}//namespace ring_buffer

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__RING_BUFFER_INL_

// End of file

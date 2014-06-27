#pragma once
#ifndef __HASH_TABLE_INL_
#define __HASH_TABLE_INL_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// hash_table.inl
// ハッシュテーブルコンテナ【インライン関数／テンプレート関数定義部】
//
// ※コンテナクラスの操作が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/hash_table.h>//ハッシュテーブルコンテナ【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace hash_table
{
	//--------------------
	//イテレータのインライン関数
	
#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
	//基本オペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator[](const int index) const
	{
		iterator ite(*m_con, false);
		ite += index;
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator[](const int index)
	{
		iterator ite(*m_con, false);
		ite += index;
		return ite;
	}
#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//比較オペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::operator==(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			m_set.m_index == rhs.index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::operator!=(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? false :
			m_isEnd || rhs.m_isEnd ? true :
			m_set.m_index != rhs.m_set.m_index;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::operator>(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && !rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			m_set.m_index > rhs.m_set.m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::operator>=(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? true :
			m_isEnd && !rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			m_set.m_index >= rhs.m_set.m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::operator<(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			!m_isEnd && rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			m_set.m_index < rhs.m_set.m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::operator<=(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? true :
			!m_isEnd && rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			m_set.m_index <= rhs.m_set.m_index;
	}
#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//演算オペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator++() const
	{
		updateNext();
		return *this;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator--() const
	{
		updatePrev();
		return *this;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator++()
	{
		updateNext();
		return *this;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator--()
	{
		updatePrev();
		return *this;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator++(int) const
	{
		iterator ite(*this);
		++(*this);
		return ite;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator--(int) const
	{
		iterator ite(*this);
		--(*this);
		return ite;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator++(int)
	{
		iterator ite(*this);
		++(*this);
		return ite;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator--(int)
	{
		iterator ite(*this);
		--(*this);
		return ite;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator+=(const int rhs) const
	{
		updateForward(rhs);
		return *this;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-=(const int rhs) const
	{
		updateBackward(rhs);
		return *this;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator+=(const int rhs)
	{
		updateForward(rhs);
		return *this;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-=(const int rhs)
	{
		updateBackward(rhs);
		return *this;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator+(const int rhs) const
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-(const int rhs) const
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator+(const int rhs)
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-(const int rhs)
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	//template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	//inline int container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& rhs) const
	//{
	//	return ???;
	//}
#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//アクセッサ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::isExist() const
	{
		return m_set.m_index != INVALID_INDEX;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::isEnabled() const
	{
		return m_set.m_index != INVALID_INDEX || m_isEnd;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::isEnd() const//終端か？
	{
		return m_isEnd;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::set& container<OPE_TYPE, _TABLE_SIZE>::iterator::getSet() const//現在のセット
	{
		return m_set;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::set& container<OPE_TYPE, _TABLE_SIZE>::iterator::getSet()//現在のセット
	{
		return m_set;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::iterator::getIndex() const//（実際の）インデックス
	{
		return m_set.m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::iterator::getPrimaryIndex() const//本来のインデックス
	{
		return m_set.m_primaryIndex;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::key_type container<OPE_TYPE, _TABLE_SIZE>::iterator::getKey() const//現在のキー
	{
		return m_set.m_key;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::iterator::getValue() const//現在の値
	{
		return m_set.m_value;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::iterator::getValue()//現在の値
	{
		return m_set.m_value;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::isDeleted() const//削除済み
	{
		return m_set.m_isDeleted;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::isPrimaryIndex() const//本来のインデックスか？
	{
		return m_set.isPrimaryIndex();
	}

#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	//--------------------
	//リバースイテレータのインライン関数
	
#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//基本オペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator[](const int index) const
	{
		reverse_iterator ite(*m_con, false);
		ite += index;
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator[](const int index)
	{
		reverse_iterator ite(*m_con, false);
		ite += index;
		return ite;
	}
#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//比較オペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator==(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			rhs.m_set.m_index == m_set.m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator!=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? false :
			rhs.m_isEnd || m_isEnd ? true :
			rhs.m_set.m_index != m_set.m_index;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator>(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && !m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			rhs.m_set.m_index > m_set.m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator>=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? true :
			rhs.m_isEnd && !m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			rhs.m_set.m_index >= m_set.m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator<(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			!rhs.m_isEnd && m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			rhs.m_set.m_index < m_set.m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator<=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs) const
	{
		return !isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? true :
			!rhs.m_isEnd && m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			rhs.m_set.m_index <= m_set.m_index;
	}
#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//演算オペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator++() const
	{
		updateNext();
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator--() const
	{
		updatePrev();
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator++()
	{
		updateNext();
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator--()
	{
		updatePrev();
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator++(int) const
	{
		reverse_iterator ite(*this);
		++(*this);
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator--(int) const
	{
		reverse_iterator ite(*this);
		--(*this);
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator++(int)
	{
		reverse_iterator ite(*this);
		++(*this);
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator--(int)
	{
		reverse_iterator ite(*this);
		--(*this);
		return ite;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator+=(const int rhs) const
	{
		updateForward(rhs);
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator-=(const int rhs) const
	{
		updateBackward(rhs);
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator+=(const int rhs)
	{
		updateForward(rhs);
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator-=(const int rhs)
	{
		updateBackward(rhs);
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator+(const int rhs) const
	{
		reverse_iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator-(const int rhs) const
	{
		reverse_iterator ite(*this);
		ite -= rhs;
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator+(const int rhs)
	{
		reverse_iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator-(const int rhs)
	{
		reverse_iterator ite(*this);
		ite -= rhs;
		return ite;
	}
	//template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	//inline int container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator-(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator rhs) const
	//{
	//	return ???;
	//}
#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//アクセッサ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::isExist() const
	{
		return m_set.m_index != INVALID_INDEX;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::isEnabled() const
	{
		return m_set.m_index != INVALID_INDEX || m_isEnd;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::isEnd() const//終端か？
	{
		return m_isEnd;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::set& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::getSet() const//現在のセット
	{
		return m_set;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::set& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::getSet()//現在のセット
	{
		return m_set;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::getIndex() const//（実際の）インデックス
	{
		return m_set.m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::getPrimaryIndex() const//本来のインデックス
	{
		return m_set.m_primaryIndex;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::key_type container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::getKey() const//現在のキー
	{
		return m_set.m_key;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::getValue() const//現在の値
	{
		return m_set.m_value;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::getValue()//現在の値
	{
		return m_set.m_value;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::isDeleted() const//削除済み
	{
		return m_set.m_isDeleted;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::isPrimaryIndex() const//本来のインデックスか？
	{
		return m_set.isPrimaryIndex();
	}
	//ベースを取得
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::base() const
	{
		iterator ite(*this);
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::base()
	{
		iterator ite(*this);
		return ite;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない

	//----------------------------------------
	//コンテナ本体のメソッド

	//配列の再割り当て
//	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
//	template<std::size_t N>
//	inline void container<OPE_TYPE, _TABLE_SIZE>::assignArray(typename container<OPE_TYPE, _TABLE_SIZE>::value_type(&array)[N], const int size)
//	{
//	}

}//namespace hash_table

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__HASH_TABLE_INL_

// End of file

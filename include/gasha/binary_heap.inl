#pragma once
#ifndef __BINARY_HEAP_INL_
#define __BINARY_HEAP_INL_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// binary_heap.inl
// 二分ヒープコンテナ【インライン関数／テンプレート関数定義部】
//
// ※コンテナクラスの操作が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/binary_heap.h>//二分ヒープコンテナ【宣言部】

#include <gasha/utility.h>//汎用ユーティリティ（値の交換を使用）

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace binary_heap
{
	//--------------------
	//二分ヒープ操作関数：親のインデックス計算
	inline std::size_t calcParent(const std::size_t index)
	{
		return (index - 1) >> 1;
	}
	//--------------------
	//二分ヒープ操作関数：子のインデックス計算
	inline std::size_t calcChildL(const std::size_t index)
	{
		return (index << 1) + 1;
	}
	inline std::size_t calcChildR(const std::size_t index)
	{
		return calcChildL(index) + 1;
	}
	//--------------------
	//二分ヒープ操作関数：アップヒープ
	//※ノードを上方に移動
	template<class OPE_TYPE, class PREDICATE>
	typename OPE_TYPE::node_type* upHeap(typename OPE_TYPE::node_type* top, const std::size_t size, typename OPE_TYPE::node_type* now, PREDICATE less)
	{
		std::size_t index = now - top;
		//if (index < 0 || index >= size)
		if (index >= size)
			return nullptr;
		while (index != 0)
		{
			index = calcParent(index);
			typename OPE_TYPE::node_type* parent = top + index;
			if (!less(*parent, *now))
				break;
			GASHA_ swapValues(*parent, *now);
			now = parent;
		}
		return now;
	}
	//--------------------
	//二分ヒープ操作関数：ダウンヒープ
	//※ノードを下方に移動
	template<class OPE_TYPE, class PREDICATE>
	typename OPE_TYPE::node_type* downHeap(typename OPE_TYPE::node_type* top, const std::size_t size, typename OPE_TYPE::node_type* now, PREDICATE less)
	{
		std::size_t index = now - top;
		//if (index < 0 || index > size)
		if (index >= size)
			return nullptr;
		const std::size_t size_1 = size - 1;
		while (true)
		{
			index = calcChildL(index);
			if (index > size_1)
				break;
			typename OPE_TYPE::node_type* child = top + index;
			const bool l_is_less = less(*child, *now);
			bool is_swap = false;
			if (index == size_1)
			{
				if (!l_is_less)
					is_swap = true;
			}
			else//if (index < size_1)
			{
				typename OPE_TYPE::node_type* child_r = child + 1;
				const bool r_is_less = less(*child_r, *now);
				if (!r_is_less && (l_is_less || !less(*child_r, *child)))
				{
					is_swap = true;
					child = child_r;
					++index;
				}
				else if (!l_is_less)
					is_swap = true;
			}
			if (!is_swap)
				break;
			GASHA_ swapValues(*child, *now);
			now = child;
		}
		return now;
	}
	
	//--------------------
	//イテレータのインライン関数
	
	//基本オペレータ
#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator[](const int index) const
	{
		iterator ite(*m_con, false);
		ite.update(index);
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator[](const int index)
	{
		iterator ite(*m_con, false);
		ite.update(index);
		return ite;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
	//比較オペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::operator==(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_index == rhs.index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::operator!=(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_index != rhs.m_index;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::operator>(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_index > rhs.m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::operator>=(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_index >= rhs.m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::operator<(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_index < rhs.m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::operator<=(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_index <= rhs.m_index;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
	//演算オペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator++() const
	{
		addIndexAndUpdate(1);
		return *this;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator--() const
	{
		addIndexAndUpdate(-1);
		return *this;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator++()
	{
		addIndexAndUpdate(1);
		return *this;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator--()
	{
		addIndexAndUpdate(-1);
		return *this;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator++(int) const
	{
		iterator ite(*this);
		++(*this);
		return ite;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator--(int) const
	{
		iterator ite(*this);
		--(*this);
		return ite;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator++(int)
	{
		iterator ite(*this);
		++(*this);
		return ite;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator--(int)
	{
		iterator ite(*this);
		--(*this);
		return ite;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator+=(const int rhs) const
	{
		addIndexAndUpdate(rhs);
		return *this;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-=(const int rhs) const
	{
		addIndexAndUpdate(-rhs);
		return *this;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator+=(const int rhs)
	{
		addIndexAndUpdate(rhs);
		return *this;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-=(const int rhs)
	{
		addIndexAndUpdate(-rhs);
		return *this;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator+(const int rhs) const
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-(const int rhs) const
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator+(const int rhs)
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-(const int rhs)
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline int container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& rhs) const
	{
		if (m_index == INVALID_INDEX || rhs.m_index == INVALID_INDEX || m_index < rhs.m_index)
			return 0;
		return m_index - rhs.m_index;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
	//アクセッサ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::isExist() const
	{
		return m_index != INVALID_INDEX && m_index < m_con->m_used;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::isEnabled() const
	{
		return m_index != INVALID_INDEX;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::iterator::isEnd() const//終端か？
	{
		return m_index == m_con->m_size;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::iterator::getIndex() const//インデックス
	{
		return m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::iterator::getValue() const//現在の値
	{
		return m_value;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::iterator::getValue()//現在の値
	{
		return m_value;
	}

	//--------------------
	//リバースイテレータのインライン関数

	//基本オペレータ
#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator[](const int index) const
	{
		reverse_iterator ite(*m_con, false);
		ite.update(m_con->m_used - index);
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator[](const int index)
	{
		reverse_iterator ite(*m_con, false);
		ite.update(m_con->m_used - index);
		return ite;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
	//比較オペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator==(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_index == m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator!=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_index != m_index;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator>(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_index > m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator>=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_index >= m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator<(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_index < m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator<=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_index <= m_index;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
	//演算オペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator++() const
	{
		addIndexAndUpdate(1);
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator--() const
	{
		addIndexAndUpdate(-1);
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator++()
	{
		addIndexAndUpdate(1);
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator--()
	{
		addIndexAndUpdate(-1);
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
#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator+=(const int rhs) const
	{
		addIndexAndUpdate(rhs);
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator-=(const int rhs) const
	{
		addIndexAndUpdate(-rhs);
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator+=(const int rhs)
	{
		addIndexAndUpdate(rhs);
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator-=(const int rhs)
	{
		addIndexAndUpdate(-rhs);
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline int container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator-(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs) const
	{
		if (m_index == INVALID_INDEX || rhs.m_index == INVALID_INDEX || rhs.m_index < m_index)
			return 0;
		return rhs.m_index - m_index;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE
	//アクセッサ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::isExist() const
	{
		return m_index != INVALID_INDEX && m_index > 0;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::isEnabled() const
	{
		return m_index != INVALID_INDEX;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::isEnd() const//終端か？
	{
		return m_index == 0;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::getIndex() const//インデックス
	{
		return m_index - 1;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::getValue() const//現在の値
	{
		return m_value;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::getValue()//現在の値
	{
		return m_value;
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

	//----------------------------------------
	//コンテナ本体のメソッド
	
	//メソッド：要素アクセス系（独自拡張版）
	//※範囲チェックなし（非公開）
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::_calcParent(const typename container<OPE_TYPE, _TABLE_SIZE>::index_type index) const//親インデックス計算 ※範囲チェックなし
	{
		return binary_heap::calcParent(index);
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::_calcChildL(const typename container<OPE_TYPE, _TABLE_SIZE>::index_type index) const//左側の子インデックス計算 ※範囲チェックなし
	{
		return binary_heap::calcChildL(index);
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::_calcChildR(const typename container<OPE_TYPE, _TABLE_SIZE>::index_type index) const//右側の子インデックス計算 ※範囲チェックなし
	{
		return binary_heap::calcChildR(index);
	}

	//配列の再割り当て
//	template<class OPE_TYPE>
//	template<std::size_t N>
//	inline void container<OPE_TYPE>::assignArray(typename container<OPE_TYPE>::value_type(&array)[N], const int size)
//	{
//	}

}//namespace binary_heap

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__BINARY_HEAP_INL_

// End of file

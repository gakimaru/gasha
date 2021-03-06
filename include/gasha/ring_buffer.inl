﻿#pragma once
#ifndef GASHA_INCLUDED_RING_BUFFER_INL
#define GASHA_INCLUDED_RING_BUFFER_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// ring_buffer.inl
// リングバッファコンテナ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/ring_buffer.h>//リングバッファコンテナ【宣言部】

#include <gasha/is_ordered.h>//整列状態確認

#include <gasha/intro_sort.h>//イントロソート
#include <gasha/insertion_sort.h>//挿入ソート

#include <gasha/linear_search.h>//線形探索
#include <gasha/binary_search.h>//二分探索
#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し

#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace ring_buffer
{
	//--------------------
	//イテレータのインライン関数
	
	//基本オペレータ
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator[](const int logical_index) const
	{
		//iterator ite(*m_con, false);
		//ite.update(logical_index);
		iterator ite(*this);
		ite.addIndexAndUpdate(logical_index);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator[](const int logical_index)
	{
		//iterator ite(*m_con, false);
		//ite.update(logical_index);
		iterator ite(*this);
		ite.addIndexAndUpdate(logical_index);
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
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator+=(const typename container<OPE_TYPE>::difference_type rhs)
	{
		addIndexAndUpdate(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator-=(const typename container<OPE_TYPE>::difference_type rhs)
	{
		addIndexAndUpdate(-rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator+(const typename container<OPE_TYPE>::difference_type rhs) const
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator-(const typename container<OPE_TYPE>::difference_type rhs) const
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::difference_type container<OPE_TYPE>::iterator::operator-(const typename container<OPE_TYPE>::iterator& rhs) const
	{
		if (m_logicalIndex == INVALID_INDEX || rhs.m_logicalIndex == INVALID_INDEX)
			return 0;
		return static_cast<difference_type>(m_logicalIndex) - static_cast<difference_type>(rhs.m_logicalIndex);
	}
	//アクセッサ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isExist() const
	{
		return m_logicalIndex != INVALID_INDEX && m_logicalIndex < static_cast<index_type>(m_con->m_size);
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isEnabled() const
	{ 
		return m_logicalIndex != INVALID_INDEX;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isEnd() const//終端か？
	{
		return m_logicalIndex == static_cast<index_type>(m_con->m_size);
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
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::iterator::getValue() const//現在の値
	{
		return m_value;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::iterator::getValue()//現在の値
	{
		return m_value;
	}
	
	//--------------------
	//リバースイテレータのインライン関数
	
	//基本オペレータ
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator[](const int logical_index) const
	{
		//reverse_iterator ite(*m_con, false);
		//ite.update(m_con->m_size - logical_index);
		reverse_iterator ite(*this);
		ite.addIndexAndUpdate(logical_index);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator[](const int logical_index)
	{
		//reverse_iterator ite(*m_con, false);
		//ite.update(m_con->m_size - logical_index);
		reverse_iterator ite(*this);
		ite.addIndexAndUpdate(logical_index);
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
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator+=(const typename container<OPE_TYPE>::difference_type rhs)
	{
		addIndexAndUpdate(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator-=(const typename container<OPE_TYPE>::difference_type rhs)
	{
		addIndexAndUpdate(-rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator+(const typename container<OPE_TYPE>::difference_type rhs) const
	{
		reverse_iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator-(const typename container<OPE_TYPE>::difference_type rhs) const
	{
		reverse_iterator ite(*this);
		ite -= rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::difference_type container<OPE_TYPE>::reverse_iterator::operator-(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		if (m_logicalIndex == INVALID_INDEX || rhs.m_logicalIndex == INVALID_INDEX)
			return 0;
		return static_cast<difference_type>(rhs.m_logicalIndex) - static_cast<difference_type>(m_logicalIndex);
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
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::reverse_iterator::getValue() const//現在の値
	{
		return m_value;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::reverse_iterator::getValue()//現在の値
	{
		return m_value;
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
	
	//----------------------------------------
	//コンテナ本体のメソッド

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
	
	//論理インデックスを範囲内に補正
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::_adjLogicalIndex(const typename container<OPE_TYPE>::index_type logical_index) const
	{
		const index_type invalid_index = INVALID_INDEX;//【注】最適化オプションを付けていないと、三項演算子が実体のポインタを参照して直値展開されなくなり、結果リンクエラーになることがある（GCCで確認）
		return static_cast<index_type>(logical_index >= 0 && logical_index < static_cast<index_type>(m_maxSize) ? logical_index : invalid_index);
	}
	//要素を物理インデックスに変換 ※範囲チェックなし
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::_refRealIndex(const typename container<OPE_TYPE>::value_type* node) const
	{
		return static_cast<index_type>(node - _refFront());
	}
	//要素を論理インデックスに変換 ※範囲チェックなし
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::_refLogicalIndex(const typename container<OPE_TYPE>::value_type* node) const
	{
		return _toLogicalIndex(_refRealIndex(node));
	}
	
	//末尾の新規インデックス
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::size_type container<OPE_TYPE>::_backNewRealIndex() const
	{
		const index_type new_real_index = m_offset + m_size;
		return new_real_index < m_maxSize ? new_real_index : new_real_index - m_maxSize;
	}
	
	//配列の再割り当て
	template<class OPE_TYPE>
	template<std::size_t N>
	inline void container<OPE_TYPE>::assignArray(typename container<OPE_TYPE>::value_type(&array)[N], const int size)
	{
		assignArray(array, N, size);
	}
	//※voidポインタとバッファサイズ数指定版
	template<class OPE_TYPE>
	inline void container<OPE_TYPE>::assignArray(void* buff_ptr, const typename container<OPE_TYPE>::size_type buff_size, const int size)
	{
		assignArray(static_cast<value_type*>(buff_ptr), buff_size / sizeof(value_type), size);
	}

	//使用中のサイズを変更（新しいサイズを返す）
	//※コンストラクタ呼び出し版
	template<class OPE_TYPE>
	template<typename... Tx>
	typename container<OPE_TYPE>::size_type container<OPE_TYPE>::resize(const int size, Tx&&... args)
	{
		const size_type _size = size < 0 ? m_maxSize : static_cast<size_type>(size) < m_maxSize ? static_cast<size_type>(size) : m_maxSize;
		if (_size > m_size)
		{
			for (index_type index = m_size; index < _size; ++index)
			{
				value_type* value = _refElement(index);
				GASHA_ callConstructor<value_type>(value, std::forward<Tx>(args)...);//コンストラクタ呼び出し
			}
		}
		else if (_size < m_size)
		{
			for (index_type index = size; index < m_size; ++index)
			{
				value_type* value = _refElement(index);
				ope_type::callDestructor(value);//デストラクタ呼び出し
			}
		}
		m_size = _size;
		return m_size;
	}

	//先頭から指定数の要素にデータを割り当てる
	//※コンストラクタ呼び出し版
	template<class OPE_TYPE>
	template<typename... Tx>
	typename container<OPE_TYPE>::size_type container<OPE_TYPE>::assign(const int size, Tx&&... args)
	{
		const size_type _size = size < 0 ? m_maxSize : static_cast<size_type>(size) < m_maxSize ? static_cast<size_type>(size) : m_maxSize;
		{
			const size_type used_size = _size < m_size ? _size : m_size;
			for (index_type index = 0; index < used_size; ++index)
			{
				value_type* value = _refElement(index);
				ope_type::callDestructor(value);//デストラクタ呼び出し
			}
		}
		{
			for (index_type index = 0; index < _size; ++index)
			{
				value_type* value = _refElement(index);
				GASHA_ callConstructor<value_type>(value, std::forward<Tx>(args)...);//コンストラクタ呼び出し
			}
		}
		if (m_size < _size)
			m_size = _size;
		return m_size;
	}

	//先頭に要素を追加
	//※パラメータ渡し
	template<class OPE_TYPE>
	template<typename... Tx>
	typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::push_front(Tx&&... args)
	{
		value_type* obj = refFrontNew();//サイズチェック含む
		if (!obj)
			return nullptr;
		GASHA_ callConstructor<value_type>(obj, std::forward<Tx>(args)...);//コンストラクタ呼び出し
		++m_size;
		m_offset = m_offset == 0 ? m_maxSize - 1 : m_offset - 1;
		return obj;
	}

	//末尾に要素を追加
	//※パラメータ渡し
	template<class OPE_TYPE>
	template<typename... Tx>
	typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::push_back(Tx&&... args)
	{
		value_type* obj = refBackNew();//サイズチェック含む
		if (!obj)
			return nullptr;
		GASHA_ callConstructor<value_type>(obj, std::forward<Tx>(args)...);//コンストラクタ呼び出し
		++m_size;
		return obj;
	}

	//要素の挿入
	//※コンストラクタ呼び出し版
	template<class OPE_TYPE>
	template<typename... Tx>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::insert(typename container<OPE_TYPE>::iterator pos, const int num, Tx&&... args)
	{
		if (pos.isNotEnabled() || num == 0 || m_size == m_maxSize)
		{
			iterator ite(*this, INVALID_INDEX);
			return ite;
		}
		index_type index = pos.getIndex();
		const size_type remain = m_maxSize - m_size;
		const size_type _num = num < 0 || static_cast<size_type>(num) > remain ? remain : static_cast<size_type>(num);
		const size_type move_num = m_size - index;
		//移動
		moveDesc(index + _num, index, move_num);
		//要素数変更
		m_size += _num;
		//挿入
		index_type _index = index;
		for (size_type i = 0; i < _num; ++i)
		{
			value_type* new_value = _refElement(_index);
			GASHA_ callConstructor<value_type>(new_value, std::forward<Tx>(args)...);
			++_index;
		}
		//終了
		iterator now(*this, index);
		return now;
	}

	//ソート
	template<class OPE_TYPE>
	inline void container<OPE_TYPE>::sort()
	{
		GASHA_ iteratorIntroSort(begin(), end(), typename ope_type::predicateForSort());
	}
	//※プレディケート関数指定版
	template<class OPE_TYPE>
	template<class PREDICATE>
	inline void container<OPE_TYPE>::sort(PREDICATE predicate)
	{
		GASHA_ iteratorIntroSort(begin(), end(), predicate);
	}

	//安定ソート
	template<class OPE_TYPE>
	inline void container<OPE_TYPE>::stableSort()
	{
		GASHA_ iteratorInsertionSort(begin(), end(), typename ope_type::predicateForSort());
	}
	//※プレディケート関数指定版
	template<class OPE_TYPE>
	template<class PREDICATE>
	inline void container<OPE_TYPE>::stableSort(PREDICATE predicate)
	{
		GASHA_ iteratorInsertionSort(begin(), end(), predicate);
	}

	//ソート済み状態チェック
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::isOrdered() const
	{
		return GASHA_ iteratorIsOrdered(begin(), end(), typename ope_type::predicateForSort());
	}
	//※プレディケート関数指定版
	template<class OPE_TYPE>
	template<class PREDICATE>
	inline bool container<OPE_TYPE>::isOrdered(PREDICATE predicate) const
	{
		return GASHA_ iteratorIsOrdered(begin(), end(), predicate);
	}

	//線形探索
	//※探索値指定版
	template<class OPE_TYPE>
	template<typename V>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::findValue(const V& value) const
	{
		iterator found = GASHA_ iteratorLinearSearchValue(begin(), end(), value, typename ope_type::predicateForFind());
		return found;
	}
	//※比較関数＋値指定版
	template<class OPE_TYPE>
	template<typename V, class PREDICATE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::findValue(const V& value, PREDICATE predicate) const
	{
		iterator found = GASHA_ iteratorLinearSearchValue(begin(), end(), value, predicate);
		return found;
	}
	//※比較関数指定版
	template<class OPE_TYPE>
	template<class PREDICATE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::find(PREDICATE predicate) const
	{
		iterator found = GASHA_ iteratorLinearSearch(begin(), end(), predicate);
		return found;
	}

	//二分探索
	//※探索値指定版
	template<class OPE_TYPE>
	template<typename V>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::binarySearchValue(const V& value) const
	{
		iterator found = GASHA_ iteratorBinarySearchValue(begin(), end(), value, typename ope_type::comparisonForSearch());
		return found;
	}
	//※比較関数＋値指定版
	template<class OPE_TYPE>
	template<typename V, class COMPARISON>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::binarySearchValue(const V& value, COMPARISON comparison) const
	{
		iterator found = GASHA_ iteratorBinarySearchValue(begin(), end(), value, comparison);
		return found;
	}
	//※比較関数指定版
	template<class OPE_TYPE>
	template<class COMPARISON>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::binary_search(COMPARISON comparison) const
	{
		iterator found = GASHA_ iteratorBinarySearch(begin(), end(), comparison);
		return found;
	}

	//コンストラクタ
	template<class OPE_TYPE>
	template<std::size_t N>
	container<OPE_TYPE>::container(typename container<OPE_TYPE>::value_type(&array)[N], const int size, const autoClearAttr_t auto_clear_attr) :
		m_array(array),
		m_maxSize(N),
		m_size(size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size)),
		m_offset(0),
		m_autoClearAttr(auto_clear_attr)
	{}

	//デフォルトコンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::container() :
		m_array(nullptr),
		m_maxSize(0),
		m_size(0),
		m_offset(0),
		m_autoClearAttr(NEVER_CLEAR)
	{}

}//namespace ring_buffer

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_RING_BUFFER_INL

// End of file

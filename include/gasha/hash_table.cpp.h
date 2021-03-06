﻿#pragma once
#ifndef GASHA_INCLUDED_HASH_TABLE_CPP_H
#define GASHA_INCLUDED_HASH_TABLE_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// hash_table.cpp.cpp.h
// 開番地法ハッシュテーブルコンテナ【関数／実体定義部】
//
// ※クラスのインスタンス化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。
// 　（コンパイル・リンク時間への影響を気にしないならOK）
// ※明示的なインスタンス化を避けたい場合は、ヘッダーファイルと共にインクルード。
// 　（この場合、実際に使用するメンバー関数しかインスタンス化されないので、対象クラスに不要なインターフェースを実装しなくても良い）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/hash_table.inl>//ハッシュテーブルコンテナ【インライン関数／テンプレート関数定義部】

#include <utility>//C++11 std::move
#include <cstring>//std::memcpy()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace hash_table
{
	//----------------------------------------
	//イテレータ用の型
	
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::set::update(const typename container<OPE_TYPE>::index_type index, const typename container<OPE_TYPE>::index_type primary_index, const typename container<OPE_TYPE>::key_type key, const typename container<OPE_TYPE>::value_type* value, const bool is_deleted)
	{
		m_index = index;
		m_primaryIndex = primary_index;
		m_key = key;
		m_value = const_cast<value_type*>(value);
		m_isDeleted = is_deleted;
	}

	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::set& container<OPE_TYPE>::set::operator=(typename container<OPE_TYPE>::set&& rhs)
	{
		m_index = rhs.m_index;
		m_primaryIndex = rhs.m_primaryIndex;
		m_key = rhs.m_key;
		m_value = rhs.m_value;
		m_isDeleted = rhs.m_isDeleted;
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::set& container<OPE_TYPE>::set::operator=(const typename container<OPE_TYPE>::set& rhs)
	{
		m_index = rhs.m_index;
		m_primaryIndex = rhs.m_primaryIndex;
		m_key = rhs.m_key;
		m_value = rhs.m_value;
		m_isDeleted = rhs.m_isDeleted;
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::set::set(typename container<OPE_TYPE>::set&& obj) :
		m_index(obj.m_index),
		m_primaryIndex(obj.m_primaryIndex),
		m_key(obj.m_key),
		m_value(obj.m_value),
		m_isDeleted(obj.m_isDeleted)
	{}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::set::set(const typename container<OPE_TYPE>::set& obj) :
		m_index(obj.m_index),
		m_primaryIndex(obj.m_primaryIndex),
		m_key(obj.m_key),
		m_value(obj.m_value),
		m_isDeleted(obj.m_isDeleted)
	{}
	//コストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::set::set(const typename container<OPE_TYPE>::index_type index, const typename container<OPE_TYPE>::index_type primary_index, const typename container<OPE_TYPE>::key_type key, const typename container<OPE_TYPE>::value_type* value, const bool is_deleted) :
		m_index(index),
		m_primaryIndex(primary_index),
		m_key(key),
		m_value(const_cast<value_type*>(value)),
		m_isDeleted(is_deleted)
	{}

	//----------------------------------------
	//イテレータのメソッド
	
#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
	//演算オペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::difference_type container<OPE_TYPE>::iterator::operator-(const typename container<OPE_TYPE>::iterator& rhs) const
	{
		if (!m_set.m_value && !rhs.m_set.m_value)
			return 0;
		if (!m_set.m_value && !m_isEnd)
			return 0;
		if (!rhs.m_set.m_value && !rhs.m_isEnd)
			return 0;
		if (!rhs.m_isEnd)
		{
			difference_type diff = 0;
			index_type index = rhs.m_set.m_index;
			const index_type end = m_set.m_index;
			while (index != INVALID_INDEX && index != end)
			{
				index = m_con->getNextIndex(index);
				++diff;
			}
			if (index == end)
				return diff;
		}
		{
			difference_type diff = 0;
			index_type index = m_set.m_index;
			const index_type end = rhs.m_set.m_index;
			while (index != INVALID_INDEX && index != end)
			{
				index = m_con->getNextIndex(index);
				--diff;
			}
			if (index == end)
				return diff;
		}
		return 0;
	}
#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//参照を更新
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::index_type container<OPE_TYPE>::iterator::update(const typename container<OPE_TYPE>::index_type index) const
	{
		//if (index == INVALID_INDEX || index < 0 || index >= static_cast<index_type>(m_con->m_size))
		if (index >= static_cast<index_type>(TABLE_SIZE))
			m_set.update(INVALID_INDEX, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false);
		else
			m_set.update(index, m_con->calcIndex(m_con->m_keyTable[index]), m_con->m_keyTable[index], reinterpret_cast<const value_type*>(m_con->m_table[index]), m_con->m_deleted[index]);
		return m_set.m_index;
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateNext() const
	{
		const index_type prev_index = m_set.m_index;
		const index_type index = update(m_con->getNextIndex(prev_index));
		m_isEnd = (prev_index != INVALID_INDEX && index == INVALID_INDEX);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updatePrev() const
	{
		if (m_isEnd)
		{
			update(m_con->getLastIndex());
			m_isEnd = false;
			return;
		}
		update(m_con->getPrevIndex(m_set.m_index));
		m_isEnd = false;
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateForward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (step == 0)
			return;
		if (step < 0)
			return updateBackward(-step);
		difference_type _step = step;
		const index_type prev_index = m_set.m_index;
		index_type index = prev_index;
		while (_step > 0 && index != INVALID_INDEX)
		{
			index = m_con->getNextIndex(index);
			--_step;
		}
		update(index);
		m_isEnd = (index != INVALID_INDEX && index == INVALID_INDEX && _step == 0);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::updateBackward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (step <= 0)
			return updateForward(-step);
		difference_type _step = step;
		index_type index = m_set.m_index;
		if (_step > 0 && m_isEnd)
		{
			index = m_con->getLastIndex();
			--_step;
		}
		while (_step > 0 && index != INVALID_INDEX)
		{
			index = m_con->getPrevIndex(index);
			--_step;
		}
		update(index);
		m_isEnd = false;
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_set = std::move(rhs.m_set);
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_set = std::move(rhs.m_set);
		m_isEnd = false;
		if (m_set.m_index != INVALID_INDEX)
			++(*this);
		else
		{
			if (rhs.m_isEnd)
				update(m_con->getFirstIndex());//先頭インデックス
		}
		return *this;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_set = rhs.m_set;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_set = rhs.m_set;
		m_isEnd = false;
		if (m_set.m_index != INVALID_INDEX)
			++(*this);
		else
		{
			if (rhs.m_isEnd)
				update(m_con->getFirstIndex());//先頭インデックス
		}
		return *this;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_set(std::move(obj.m_set)),
		m_isEnd(obj.m_isEnd)
	{}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_set(std::move(obj.m_set)),
		m_isEnd(false)
	{
		if (m_set.m_index != INVALID_INDEX)
			++(*this);
		else
		{
			if (obj.m_isEnd)
				update(m_con->getFirstIndex());//先頭インデックス
		}
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_set(obj.m_set),
		m_isEnd(obj.m_isEnd)
	{}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_set(obj.m_set),
		m_isEnd(false)
	{
		if (m_set.m_index != INVALID_INDEX)
			++(*this);
		else
		{
			if (obj.m_isEnd)
				update(m_con->getFirstIndex());//先頭インデックス
		}
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const container& con, const bool is_end) :
		m_con(&con),
		m_set(INVALID_INDEX, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false),
		m_isEnd(is_end)
	{
		if (!is_end)
		{
			update(m_con->getFirstIndex());//先頭インデックス
			if (!m_set.m_value)
				m_isEnd = true;
		}
	}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const container& con, const typename container<OPE_TYPE>::index_type index) :
		m_con(&con),
		m_set(INVALID_INDEX, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false),
		m_isEnd(false)
	{
		update(index);
	}

#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	//----------------------------------------
	//リバースイテレータのメソッド
	
#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
	//演算オペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::difference_type container<OPE_TYPE>::reverse_iterator::operator-(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		if (!m_set.m_value && !rhs.m_set.m_value)
			return 0;
		if (!m_set.m_value && !m_isEnd)
			return 0;
		if (!rhs.m_set.m_value && !rhs.m_isEnd)
			return 0;
		if (!rhs.m_isEnd)
		{
			difference_type diff = 0;
			index_type index = rhs.m_set.m_index;
			const index_type end = m_set.m_index;
			while (index != INVALID_INDEX && index != end)
			{
				index = m_con->getPrevIndex(index);
				++diff;
			}
			if (index == end)
				return diff;
		}
		{
			difference_type diff = 0;
			index_type index = m_set.m_index;
			const index_type end = rhs.m_set.m_index;
			while (index != INVALID_INDEX && index != end)
			{
				index = m_con->getPrevIndex(index);
				--diff;
			}
			if (index == end)
				return diff;
		}
		return 0;
	}
#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//参照を更新
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::index_type container<OPE_TYPE>::reverse_iterator::update(const typename container<OPE_TYPE>::index_type index) const
	{
		//if (index == INVALID_INDEX || index < 0 || index >= static_cast<index_type>(m_con->m_size))
		if (index >= static_cast<index_type>(TABLE_SIZE))
			m_set.update(INVALID_INDEX, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false);
		else
			m_set.update(index, m_con->calcIndex(m_con->m_keyTable[index]), m_con->m_keyTable[index], reinterpret_cast<const value_type*>(m_con->m_table[index]), m_con->m_deleted[index]);
		return m_set.m_index;
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateNext() const
	{
		const index_type prev_index = m_set.m_index;
		const index_type index = update(m_con->getPrevIndex(prev_index));
		m_isEnd = (prev_index != INVALID_INDEX && index == INVALID_INDEX);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updatePrev() const
	{
		if (m_isEnd)
		{
			update(m_con->getFirstIndex());
			m_isEnd = false;
			return;
		}
		update(m_con->getNextIndex(m_set.m_index));
		m_isEnd = false;
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateForward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (step == 0)
			return;
		if (step < 0)
			return updateBackward(-step);
		difference_type _step = step;
		const index_type prev_index = m_set.m_index;
		index_type index = prev_index;
		while (_step > 0 && index != INVALID_INDEX)
		{
			index = m_con->getPrevIndex(index);
			--_step;
		}
		update(index);
		m_isEnd = (index != INVALID_INDEX && index == INVALID_INDEX && _step == 0);
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::updateBackward(const typename container<OPE_TYPE>::difference_type step) const
	{
		if (step <= 0)
			return updateForward(-step);
		difference_type _step = step;
		index_type index = m_set.m_index;
		if (_step > 0 && m_isEnd)
		{
			index = m_con->getFirstIndex();
			--_step;
		}
		while (_step > 0 && index != INVALID_INDEX)
		{
			index = m_con->getNextIndex(index);
			--_step;
		}
		update(index);
		m_isEnd = false;
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_set = std::move(rhs.m_set);
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_set = std::move(rhs.m_set);
		m_isEnd = false;
		if (m_set.m_index != INVALID_INDEX)
			++(*this);
		else
		{
			if (rhs.m_isEnd)
				update(m_con->getLastIndex());//末尾インデックス
		}
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_set = rhs.m_set;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_set = rhs.m_set;
		m_isEnd = false;
		if (m_set.m_index != INVALID_INDEX)
			++(*this);
		else
		{
			if (rhs.m_isEnd)
				update(m_con->getLastIndex());//末尾インデックス
		}
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_set(std::move(obj.m_set)),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_set(std::move(obj.m_set)),
		m_isEnd(false)
	{
		if (m_set.m_index != INVALID_INDEX)
			++(*this);
		else
		{
			if (obj.m_isEnd)
				update(m_con->getLastIndex());//末尾インデックス
		}
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_set(obj.m_set),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_set(obj.m_set),
		m_isEnd(false)
	{
		if (m_set.m_index != INVALID_INDEX)
			++(*this);
		else
		{
			if (obj.m_isEnd)
				update(m_con->getLastIndex());//末尾インデックス
		}
	}
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const container& con, const bool is_end) :
		m_con(&con),
		m_set(INVALID_INDEX, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false),
		m_isEnd(is_end)
	{
		if (!is_end)
		{
			update(m_con->getLastIndex());//末尾インデックス
			if (!m_set.m_value)
				m_isEnd = true;
		}
	}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const container& con, const typename container<OPE_TYPE>::index_type index) :
		m_con(&con),
		m_set(INVALID_INDEX, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false),
		m_isEnd(false)
	{
		update(index);
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない

	//----------------------------------------
	//コンテナ本体のメソッド

	//最適化されていないデータ件数を取得
	template<class OPE_TYPE>
	int container<OPE_TYPE>::getNotOptimizedCount() const
	{
		int count = 0;
		for (index_type index = 0; index < TABLE_SIZE; ++index)
		{
			if (!m_using[index] || m_deleted[index])
				continue;
			if (index != calcIndex(m_keyTable[index]))
				++count;
		}
		return count;
	}

	//メソッド：インデックスを取得
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::index_type container<OPE_TYPE>::getFirstIndex() const
	{
		if (m_usingCount == 0 || m_deletedCount == m_usingCount)
			return INVALID_INDEX;
		for (index_type index = 0; index < TABLE_SIZE; ++index)
		{
			if (m_using[index])
				return index;
		}
		return INVALID_INDEX;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::index_type container<OPE_TYPE>::getLastIndex() const
	{
		if (m_usingCount == 0 || m_deletedCount == m_usingCount)
			return INVALID_INDEX;
		for (index_type index = TABLE_SIZE; index > 0; --index)
		{
			if (m_using[index - 1])
				return index - 1;
		}
		return INVALID_INDEX;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::index_type container<OPE_TYPE>::getNextIndex(const typename container<OPE_TYPE>::index_type index) const
	{
		index_type next_index = index == INVALID_INDEX ? 0 : index + 1;
		if (index < 0 || index >= TABLE_SIZE - 1 || m_usingCount == 0 || m_deletedCount == m_usingCount)
			return INVALID_INDEX;
		for (; next_index < TABLE_SIZE; ++next_index)
		{
			if (m_using[next_index])
				return next_index;
		}
		return INVALID_INDEX;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::index_type container<OPE_TYPE>::getPrevIndex(const typename container<OPE_TYPE>::index_type index) const
	{
		const std::size_t table_size = TABLE_SIZE;//【注】最適化オプションを付けていないと、三項演算子が実体のポインタを参照して直値展開されなくなり、結果リンクエラーになることがある（GCCで確認）
		index_type now_index = index == INVALID_INDEX ? table_size : index;
		if (index <= 0 || index >= TABLE_SIZE || m_usingCount == 0 || m_deletedCount == m_usingCount)
			return INVALID_INDEX;
		for (; now_index > 0; --now_index)
		{
			if (m_using[now_index - 1])
				return now_index - 1;
		}
		return INVALID_INDEX;
	}
	//キーで検索してインデックスを取得（共通）
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::index_type container<OPE_TYPE>::_findIndexCommon(const typename container<OPE_TYPE>::key_type key) const
	{
		if (m_usingCount == 0 || m_deletedCount == m_usingCount)
			return INVALID_INDEX;
		const index_type index_first = calcIndex(key);//キーからインデックス（ハッシュ）を計算
		index_type index = index_first;
		do
		{
			if (!m_using[index])//未使用インデックスなら検索失敗
				break;
			if (!m_deleted[index] && m_keyTable[index] == key)//キーが一致するインデックスなら検索成功
				return index;
			index = calcNextIndex(key, index);//次のインデックスへ
		} while (index != index_first);//最初のインデックスに戻ったら終了（検索失敗）
		return INVALID_INDEX;
	}
	
	//キーで検索して値を取得（本体）
	template<class OPE_TYPE>
	const typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::_findValue(const typename container<OPE_TYPE>::key_type key) const
	{
		const index_type index = _findIndex(key);//検索してインデックスを取得
		if (index == INVALID_INDEX)
			return nullptr;
		return reinterpret_cast<const value_type*>(&m_table[index]);
	}
	
	//キーで検索してイテレータを取得
	template<class OPE_TYPE>
	void container<OPE_TYPE>::_find(iterator& ite, const typename container<OPE_TYPE>::key_type key) const
	{
		const index_type index = _findIndex(key);
		if (index == INVALID_INDEX)
			return;
		ite.update(index);
	}
	
	//キー割り当て（本体）
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::_assign(const typename container<OPE_TYPE>::key_type key)
	{
		if (m_usingCount == static_cast<int>(TABLE_SIZE) && m_deletedCount == 0 && ope_type::REPLACE_ATTR == replaceAttr_t::NEVER_REPLACE)
			return nullptr;
		if ((KEY_MIN != 0 || KEY_MAX != 0) && (key < KEY_MIN || key > KEY_MAX))
			return nullptr;
		index_type index = _findIndexCommon(key);
		if (ope_type::REPLACE_ATTR == replaceAttr_t::NEVER_REPLACE && index != INVALID_INDEX)//同じキーが既に割り当て済みなら割り当て失敗
			return nullptr;
		int find_cycle = 0;
		if (index != INVALID_INDEX)
		{
			//置換
			_eraseByIndex(index);//デストラクタの呼び出しあり
		}
		else
		{
			//新規登録
			const index_type index_first = calcIndex(key);//キーからインデックス（ハッシュ）を計算
			index = index_first;
			do
			{
				++find_cycle;
				if (!m_using[index] || m_deleted[index])//未使用／削除済みインデックスなら割り当て成功
					break;
				if (FINDING_CYCLE_LIMIT > 0 && find_cycle == static_cast<int>(FINDING_CYCLE_LIMIT))//巡回回数が制限に達したら割り当て失敗
					return nullptr;
				index = calcNextIndex(key, index);//次のインデックスへ
			} while (index != index_first);//最初のインデックスに戻ったら終了（割り当て失敗）
		}
		m_keyTable[index] = key;//キーテーブルにキー登録
		if (!m_using[index])//未使用インデックスの割り当てなら使用中数を調整
		{
			m_using[index] = true;//使用中フラグをセット
			++m_usingCount;//使用中数をカウントアップ
		}
		if (m_deleted[index])//削除済みインデックスの再割り当てなら削除数を調整
		{
			m_deleted[index] = false;//削除済みフラグをリセット
			--m_deletedCount;//削除済み数をカウントダウン
		}
		m_maxFindingCycle = m_maxFindingCycle >= find_cycle ? m_maxFindingCycle : find_cycle;//最大巡回回数を更新
		return reinterpret_cast<value_type*>(&m_table[index]);
	}
	
	//キー割り当てして値を挿入（コピー）（本体）
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::_insert(const key_type key, const value_type& value)
	{
		value_type* assigned_value = _assign(key);
		if (!assigned_value)
			return nullptr;
		*assigned_value = value;
		return assigned_value;
	}
	
	//インデックスを指定して削除
	template<class OPE_TYPE>
	void container<OPE_TYPE>::_eraseByIndex(const typename container<OPE_TYPE>::index_type index)
	{
		value_type* data_p = reinterpret_cast<value_type*>(&m_table[index]);
		ope_type::callDestructor(data_p);//デストラクタ呼び出し
		m_deleted[index] = true;//削除済みインデックスを更新
		++m_deletedCount;//削除済み数をカウントアップ
	}

	//キーを削除（本体）
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::_erase(const typename container<OPE_TYPE>::key_type key)
	{
		const index_type index = _findIndex(key);//検索してインデックスを取得
		if (index == INVALID_INDEX)//検索失敗なら削除失敗
			return false;
		_eraseByIndex(index);
		if (m_usingCount == m_deletedCount || m_deletedCount == static_cast<int>(AUTO_REHASH_SIZE))//自動リハッシュ
			_rehash();
		return true;
	}
	
	//リハッシュ（本体）
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::_rehash()
	{
		if (m_usingCount == 0 || m_deletedCount == 0)
			return false;
		if (m_usingCount == m_deletedCount)
		{
			_clear();
			return true;
		}
		m_maxFindingCycle = 1;//最大巡回回数を1にリセット
		//値の移動
		for (index_type index = 0; index < static_cast<index_type>(TABLE_SIZE); ++index)
		{
			if (!m_using[index] || m_deleted[index])//未使用インデックスまたは削除済みインデックスは処理をスキップ
				continue;
			const key_type key = m_keyTable[index];//キーを取得
			if (calcIndex(key) == index)//キーが本来のインデックスと異なる場合、移動を試みる
				continue;
			value_type* value = reinterpret_cast<value_type*>(m_table[index]);//現在の値を記憶
			m_deleted[index] = true;//一旦削除済みにする
			++m_deletedCount;       //（同上）
			value_type* value_new = _assign(key);//改めてキーを割り当て
			if (value_new != value)//インデックスが違っていたなら値を移動
			{
			#if 1
				*value_new = std::move(*value);//ムーブ演算子で移動
			#else
				std::memcpy(value_new, value, sizeof(value_type));//std::memcpyで移動
			#endif
			}
		}
		//削除済みインデックスのクリア
		for (index_type index = 0; index < TABLE_SIZE; ++index)
		{
			if (!m_using[index] || !m_deleted[index])//未使用インデックスまたは未削除インデックスは処理をスキップ
				continue;
			m_deleted[index] = false;//削除済みを解消する
			--m_deletedCount;       //（同上）
			m_using[index] = false;//使用中を解消する
			--m_usingCount;        //（同上）
		}
		return true;
	}
	
	//クリア（本体）
	template<class OPE_TYPE>
	void container<OPE_TYPE>::_clear()
	{
		for (index_type index = 0; index < TABLE_SIZE; ++index)
		{
			if (m_using[index] && !m_deleted[index])//使用中データはデストラクタ呼び出し
				_eraseByIndex(index);
		}
		m_using.reset();
		m_deleted.reset();
		m_usingCount = 0;
		m_deletedCount = 0;
		m_maxFindingCycle = 0;
	}

	//デストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::~container()
	{}

}//namespace hash_table

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//開番地法ハッシュテーブルコンテナの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_hTable(OPE_TYPE) \
	template class GASHA_ hash_table::container<OPE_TYPE>;

//シンプル開番地法ハッシュテーブルコンテナの明示的なインスタンス化用マクロ
//※キー型を省略する場合
#define GASHA_INSTANCING_simpleHTable(VALUE_TYPE, _TABLE_SIZE) \
	template class GASHA_ hash_table::simpleContainer<VALUE_TYPE, _TABLE_SIZE>; \
	template class GASHA_ hash_table::container<typename GASHA_ hash_table::simpleContainer<VALUE_TYPE, _TABLE_SIZE>::ope>;
//※キー型を指定する場合
#define GASHA_INSTANCING_simpleHTable_withKey(VALUE_TYPE, _TABLE_SIZE, KEY_TYPE) \
	template class GASHA_ hash_table::simpleContainer<VALUE_TYPE, _TABLE_SIZE, KEY_TYPE>; \
	template class GASHA_ hash_table::container<typename GASHA_ hash_table::simpleContainer<VALUE_TYPE, _TABLE_SIZE, KEY_TYPE>::ope>;

//--------------------------------------------------------------------------------
//【注】明示的インスタンス化に失敗する場合
// ※このコメントは、「明示的なインスタンス化マクロ」が定義されている全てのソースコードに
// 　同じ内容のものをコピーしています。
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

#endif//GASHA_INCLUDED_HASH_TABLE_CPP_H

// End of file

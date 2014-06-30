#pragma once
#ifndef __HASH_TABLE_CPP_H_
#define __HASH_TABLE_CPP_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// hash_table.cpp.h
// ハッシュテーブルコンテナ【関数定義部】
//
// ※コンテナクラスの実体化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/hash_table.inl>//ハッシュテーブルコンテナ【インライン関数／テンプレート関数定義部】

#include <utility>//C++11 std::move

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace hash_table
{
	//----------------------------------------
	//イテレータ用の型
	
	//参照を更新
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::set::update(const typename container<OPE_TYPE, _TABLE_SIZE>::index_type index, const typename container<OPE_TYPE, _TABLE_SIZE>::index_type primary_index, const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key, const typename container<OPE_TYPE, _TABLE_SIZE>::value_type* value, const bool is_deleted)
	{
		m_index = index;
		m_primaryIndex = primary_index;
		m_key = key;
		m_value = const_cast<value_type*>(value);
		m_isDeleted = is_deleted;
	}

	//ムーブオペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::set& container<OPE_TYPE, _TABLE_SIZE>::set::operator=(typename container<OPE_TYPE, _TABLE_SIZE>::set&& rhs)
	{
		m_index = rhs.m_index;
		m_primaryIndex = rhs.m_primaryIndex;
		m_key = rhs.m_key;
		m_value = rhs.m_value;
		m_isDeleted = rhs.m_isDeleted;
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::set& container<OPE_TYPE, _TABLE_SIZE>::set::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::set& rhs)
	{
		m_index = rhs.m_index;
		m_primaryIndex = rhs.m_primaryIndex;
		m_key = rhs.m_key;
		m_value = rhs.m_value;
		m_isDeleted = rhs.m_isDeleted;
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::set::set(typename container<OPE_TYPE, _TABLE_SIZE>::set&& obj) :
		m_index(obj.m_index),
		m_primaryIndex(obj.m_primaryIndex),
		m_key(obj.m_key),
		m_value(obj.m_value),
		m_isDeleted(obj.m_isDeleted)
	{}
	//コピーコンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::set::set(const typename container<OPE_TYPE, _TABLE_SIZE>::set& obj) :
		m_index(obj.m_index),
		m_primaryIndex(obj.m_primaryIndex),
		m_key(obj.m_key),
		m_value(obj.m_value),
		m_isDeleted(obj.m_isDeleted)
	{}
	//コストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::set::set(const typename container<OPE_TYPE, _TABLE_SIZE>::index_type index, const typename container<OPE_TYPE, _TABLE_SIZE>::index_type primary_index, const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key, const typename container<OPE_TYPE, _TABLE_SIZE>::value_type* value, const bool is_deleted) :
		m_index(index),
		m_primaryIndex(primary_index),
		m_key(key),
		m_value(const_cast<value_type*>(value)),
		m_isDeleted(is_deleted)
	{}

	//----------------------------------------
	//イテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::iterator::update(const typename container<OPE_TYPE, _TABLE_SIZE>::index_type index) const
	{
		//if (index == INVALID_INDEX || index < 0 || index >= static_cast<index_type>(m_con->m_size))
		if (index >= static_cast<index_type>(TABLE_SIZE))
			m_set.update(INVALID_INDEX, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false);
		else
			m_set.update(index, m_con->calcIndex(m_con->m_keyTable[index]), m_con->m_keyTable[index], reinterpret_cast<const value_type*>(m_con->m_table[index]), m_con->m_deleted[index]);
		return m_set.m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::iterator::updateNext() const
	{
		const index_type prev_index = m_set.m_index;
		const index_type index = update(m_con->getNextIndex(prev_index));
		m_isEnd = (prev_index != INVALID_INDEX && index == INVALID_INDEX);
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::iterator::updatePrev() const
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::iterator::updateForward(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type step) const
	{
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::iterator::updateBackward(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type step) const
	{
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator=(typename container<OPE_TYPE, _TABLE_SIZE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_set = std::move(rhs.m_set);
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator=(typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator&& rhs)
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_set = rhs.m_set;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs)
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(typename container<OPE_TYPE, _TABLE_SIZE>::iterator&& obj) :
		m_con(obj.m_con),
		m_set(std::move(obj.m_set)),
		m_isEnd(obj.m_isEnd)
	{}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator&& obj) :
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& obj) :
		m_con(obj.m_con),
		m_set(obj.m_set),
		m_isEnd(obj.m_isEnd)
	{}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& obj) :
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(const container& con, const bool is_end) :
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

#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	//----------------------------------------
	//リバースイテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::update(const typename container<OPE_TYPE, _TABLE_SIZE>::index_type index) const
	{
		//if (index == INVALID_INDEX || index < 0 || index >= static_cast<index_type>(m_con->m_size))
		if (index >= static_cast<index_type>(TABLE_SIZE))
			m_set.update(INVALID_INDEX, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false);
		else
			m_set.update(index, m_con->calcIndex(m_con->m_keyTable[index]), m_con->m_keyTable[index], reinterpret_cast<const value_type*>(m_con->m_table[index]), m_con->m_deleted[index]);
		return m_set.m_index;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::updateNext() const
	{
		const index_type prev_index = m_set.m_index;
		const index_type index = update(m_con->getPrevIndex(prev_index));
		m_isEnd = (prev_index != INVALID_INDEX && index == INVALID_INDEX);
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::updatePrev() const
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::updateForward(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type step) const
	{
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::updateBackward(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type step) const
	{
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator=(typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_set = std::move(rhs.m_set);
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator=(typename container<OPE_TYPE, _TABLE_SIZE>::iterator&& rhs)
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_set = rhs.m_set;
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& rhs)
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::reverse_iterator(typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_set(std::move(obj.m_set)),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::reverse_iterator(typename container<OPE_TYPE, _TABLE_SIZE>::iterator&& obj) :
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_set(obj.m_set),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& obj) :
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::reverse_iterator(const container& con, const bool is_end) :
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
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない

	//----------------------------------------
	//コンテナ本体のメソッド

	//最適化されていないデータ件数を取得
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	int container<OPE_TYPE, _TABLE_SIZE>::getNotOptimizedCount() const
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::getFirstIndex() const
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::getLastIndex() const
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::getNextIndex(const typename container<OPE_TYPE, _TABLE_SIZE>::index_type index) const
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::getPrevIndex(const typename container<OPE_TYPE, _TABLE_SIZE>::index_type index) const
	{
		index_type now_index = index == INVALID_INDEX ? TABLE_SIZE : index;
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::_findIndexCommon(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key) const
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	const typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::_findValue(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key) const
	{
		const index_type index = _findIndex(key);//検索してインデックスを取得
		if (index == INVALID_INDEX)
			return nullptr;
		return reinterpret_cast<const value_type*>(&m_table[index]);
	}
	
	//キーで検索してイテレータを取得
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::find(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key) const
	{
		const index_type index = _findIndex(key);
		if (index == INVALID_INDEX)
			return iterator(*this, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false);
		return iterator(*this, index, m_keyTable[index], reinterpret_cast<const value_type*>(m_table[index]), m_deleted[index]);
	}
	
	//キー割り当て（本体）
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::_assign(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key)
	{
		if (m_usingCount == TABLE_SIZE && m_deletedCount == 0)
			return nullptr;
		index_type index = _findIndexCommon(key);
		if (ope_type::REPLACE_ATTR == ope_type::NEVER_REPLACE && index != INVALID_INDEX)//同じキーが既に割り当て済みなら割り当て失敗
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
				if (FINDING_CYCLE_LIMIT > 0 && find_cycle == FINDING_CYCLE_LIMIT)//巡回回数が制限に達したら割り当て失敗
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::_insert(const key_type key, const value_type& value)
	{
		value_type* assigned_value = _assign(key);
		if (!assigned_value)
			return nullptr;
		*assigned_value = value;
		return assigned_value;
	}
	
	//インデックスを指定して削除
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::_eraseByIndex(const typename container<OPE_TYPE, _TABLE_SIZE>::index_type index)
	{
		value_type* data_p = reinterpret_cast<value_type*>(&m_table[index]);
		ope_type::callDestructor(data_p);//デストラクタ呼び出し
		operator delete(data_p, data_p);//（作法として）deleteオペレータ呼び出し
		m_deleted[index] = true;//削除済みインデックスを更新
		++m_deletedCount;//削除済み数をカウントアップ
	}

	//キーを削除（本体）
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	bool container<OPE_TYPE, _TABLE_SIZE>::_erase(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key)
	{
		const index_type index = _findIndex(key);//検索してインデックスを取得
		if (index == INVALID_INDEX)//検索失敗なら削除失敗
			return false;
		_eraseByIndex(index);
		if (m_usingCount == m_deletedCount || m_deletedCount == AUTO_REHASH_SIZE)//自動リハッシュ
			_rehash();
		return true;
	}
	
	//リハッシュ（本体）
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	bool container<OPE_TYPE, _TABLE_SIZE>::_rehash()
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
		for (index_type index = 0; index < TABLE_SIZE; ++index)
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
				memcpy(value_new, value, sizeof(value_type));//memcpyで移動
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	void container<OPE_TYPE, _TABLE_SIZE>::_clear()
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
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::~container()
	{}

}//namespace hash_table

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__HASH_TABLE_CPP_H_

// End of file

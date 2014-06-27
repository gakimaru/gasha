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

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

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
	typename container<OPE_TYPE, _TABLE_SIZE>::set& container<OPE_TYPE, _TABLE_SIZE>::set::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::set&& rhs)
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
	container<OPE_TYPE, _TABLE_SIZE>::set::set(const typename container<OPE_TYPE, _TABLE_SIZE>::set&& obj) :
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
	void container<OPE_TYPE, _TABLE_SIZE>::iterator::updateForward(const int step) const
	{
		int _step = step;
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
	void container<OPE_TYPE, _TABLE_SIZE>::iterator::updateBackward(const int step) const
	{
		int _step = step;
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
	typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_set = std::move(rhs.m_set);
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator&& rhs)
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
	container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator&& obj) :
		m_con(obj.m_con),
		m_set(std::move(obj.m_set)),
		m_isEnd(obj.m_isEnd)
	{}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator&& obj) :
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
	void container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::updateForward(const int step) const
	{
		int _step = step;
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
	void container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::updateBackward(const int step) const
	{
		int _step = step;
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
	typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_set = std::move(rhs.m_set);
		m_isEnd = rhs.m_isEnd;
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator&& rhs)
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
	container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_set(std::move(obj.m_set)),
		m_isEnd(obj.m_isEnd)
	{}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator&& obj) :
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

	//配列の再割り当て
	//※ポインタと配列要素数指定版
//	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
//	void container<OPE_TYPE, _TABLE_SIZE>::assignArray(value_type* array, const typename container<OPE_TYPE, _TABLE_SIZE>::size_type max_size, const int size)
//	{
//	}
	
}//namespace hash_table

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__HASH_TABLE_CPP_H_

// End of file

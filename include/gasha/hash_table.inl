﻿#pragma once
#ifndef GASHA_INCLUDED_HASH_TABLE_INL
#define GASHA_INCLUDED_HASH_TABLE_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// hash_table.inl
// 開番地法ハッシュテーブルコンテナ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/hash_table.h>//ハッシュテーブルコンテナ【宣言部】

#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し

#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace hash_table
{
	//--------------------
	//イテレータのインライン関数
	
#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
	//基本オペレータ
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator[](const int index) const
	{
		//iterator ite(*m_con, false);
		iterator ite(*this);
		ite += index;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator[](const int index)
	{
		//iterator ite(*m_con, false);
		iterator ite(*this);
		ite += index;
		return ite;
	}
#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//比較オペレータ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator==(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			m_set.m_index == rhs.m_set.m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator!=(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? false :
			m_isEnd || rhs.m_isEnd ? true :
			m_set.m_index != rhs.m_set.m_index;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator>(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && !rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			m_set.m_index > rhs.m_set.m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator>=(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? true :
			m_isEnd && !rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			m_set.m_index >= rhs.m_set.m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator<(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			!m_isEnd && rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			m_set.m_index < rhs.m_set.m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator<=(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? true :
			!m_isEnd && rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			m_set.m_index <= rhs.m_set.m_index;
	}
#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator++()
	{
		updateNext();
		return *this;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator--()
	{
		updatePrev();
		return *this;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator++(int)
	{
		iterator ite(*this);
		++(*this);
		return ite;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator--(int)
	{
		iterator ite(*this);
		--(*this);
		return ite;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator+=(const typename container<OPE_TYPE>::difference_type rhs)
	{
		updateForward(rhs);
		return *this;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator-=(const typename container<OPE_TYPE>::difference_type rhs)
	{
		updateBackward(rhs);
		return *this;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator+(const typename container<OPE_TYPE>::difference_type rhs) const
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator-(const typename container<OPE_TYPE>::difference_type rhs) const
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//アクセッサ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isExist() const
	{
		return m_set.m_index != INVALID_INDEX;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isEnabled() const
	{
		return m_set.m_index != INVALID_INDEX || m_isEnd;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isEnd() const//終端か？
	{
		return m_isEnd;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::set& container<OPE_TYPE>::iterator::getSet() const//現在のセット
	{
		return m_set;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::set& container<OPE_TYPE>::iterator::getSet()//現在のセット
	{
		return m_set;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::iterator::getIndex() const//（実際の）インデックス
	{
		return m_set.m_index;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::iterator::getPrimaryIndex() const//本来のインデックス
	{
		return m_set.m_primaryIndex;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::key_type container<OPE_TYPE>::iterator::getKey() const//現在のキー
	{
		return m_set.m_key;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::iterator::getValue() const//現在の値
	{
		return m_set.m_value;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::iterator::getValue()//現在の値
	{
		return m_set.m_value;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isDeleted() const//削除済み
	{
		return m_set.m_isDeleted;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isPrimaryIndex() const//本来のインデックスか？
	{
		return m_set.isPrimaryIndex();
	}

#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	//--------------------
	//リバースイテレータのインライン関数
	
#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//基本オペレータ
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator[](const int index) const
	{
		//reverse_iterator ite(*m_con, false);
		reverse_iterator ite(*this);
		ite += index;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator[](const int index)
	{
		//reverse_iterator ite(*m_con, false);
		reverse_iterator ite(*this);
		ite += index;
		return ite;
	}
#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//比較オペレータ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator==(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			rhs.m_set.m_index == m_set.m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator!=(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? false :
			rhs.m_isEnd || m_isEnd ? true :
			rhs.m_set.m_index != m_set.m_index;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator>(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && !m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			rhs.m_set.m_index > m_set.m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator>=(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? true :
			rhs.m_isEnd && !m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			rhs.m_set.m_index >= m_set.m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator<(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			!rhs.m_isEnd && m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			rhs.m_set.m_index < m_set.m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator<=(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		return !isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? true :
			!rhs.m_isEnd && m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			rhs.m_set.m_index <= m_set.m_index;
	}
#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//演算オペレータ
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator++()
	{
		updateNext();
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator--()
	{
		updatePrev();
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
#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator+=(const typename container<OPE_TYPE>::difference_type rhs)
	{
		updateForward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator-=(const typename container<OPE_TYPE>::difference_type rhs)
	{
		updateBackward(rhs);
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
#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
	//アクセッサ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isExist() const
	{
		return m_set.m_index != INVALID_INDEX;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isEnabled() const
	{
		return m_set.m_index != INVALID_INDEX || m_isEnd;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isEnd() const//終端か？
	{
		return m_isEnd;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::set& container<OPE_TYPE>::reverse_iterator::getSet() const//現在のセット
	{
		return m_set;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::set& container<OPE_TYPE>::reverse_iterator::getSet()//現在のセット
	{
		return m_set;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::reverse_iterator::getIndex() const//（実際の）インデックス
	{
		return m_set.m_index;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::reverse_iterator::getPrimaryIndex() const//本来のインデックス
	{
		return m_set.m_primaryIndex;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::key_type container<OPE_TYPE>::reverse_iterator::getKey() const//現在のキー
	{
		return m_set.m_key;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::reverse_iterator::getValue() const//現在の値
	{
		return m_set.m_value;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::reverse_iterator::getValue()//現在の値
	{
		return m_set.m_value;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isDeleted() const//削除済み
	{
		return m_set.m_isDeleted;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isPrimaryIndex() const//本来のインデックスか？
	{
		return m_set.isPrimaryIndex();
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
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない

	//----------------------------------------
	//コンテナ本体のメソッド

	//キーからインデックスの歩幅（第二ハッシュ）を計算
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::calcIndexStep(const typename container<OPE_TYPE>::key_type key) const
	{
		return INDEX_STEP_BASE - key % INDEX_STEP_BASE;
	}
	//キーからインデックス（第一ハッシュ）を計算
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::calcIndex(const typename container<OPE_TYPE>::key_type key) const
	{
		typedef calcIndexImpl<(static_cast<key_range_type>(TABLE_SIZE) >= KEY_RANGE && KEY_RANGE > 0), size_type, index_type, key_type, key_range_type, TABLE_SIZE, KEY_MIN, KEY_RANGE> calc_type;
		return calc_type::calc(key);
	}
	//次のインデックスを計算（指定のインデックスに歩幅を加算）
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::calcNextIndex(const typename container<OPE_TYPE>::key_type key, const typename container<OPE_TYPE>::index_type index) const
	{
		return (index + calcIndexStep(key)) % TABLE_SIZE;
	}

	//キーで検索してインデックスを取得
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::_findIndex(const typename container<OPE_TYPE>::key_type key) const
	{
		return _findIndexCommon(key);
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::_findIndex(const char* key) const
	{
		return _findIndexCommon(GASHA_ calcCRC32(key));
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::_findIndex(const std::string& key) const
	{
		return _findIndexCommon(GASHA_ calcCRC32(key.c_str()));
	}

	//キーで検索して値を取得
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::findValue(const typename container<OPE_TYPE>::key_type key) const
	{
		return _findValue(key);
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::findValue(const char* key) const
	{
		return _findValue(GASHA_ calcCRC32(key));
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::findValue(const std::string& key) const
	{
		return _findValue(GASHA_ calcCRC32(key.c_str()));
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::findValue(const typename container<OPE_TYPE>::key_type key)
	{
		return const_cast<value_type*>(_findValue(key));
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::findValue(const char* key)
	{
		return const_cast<value_type*>(_findValue(GASHA_ calcCRC32(key)));
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::findValue(const std::string& key)
	{
		return const_cast<value_type*>(_findValue(GASHA_ calcCRC32(key.c_str())));
	}

	//キーで検索してイテレータを取得
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::find(const typename container<OPE_TYPE>::key_type key) const
	{
		iterator ite(*this, true);
		_find(ite, key);
		return ite;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::find(const char* key) const
	{
		iterator ite(*this, true);
		_find(ite, GASHA_ calcCRC32(key));
		return ite;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::find(const std::string& key) const
	{
		iterator ite(*this, INVALID_INDEX);
		_find(ite, GASHA_ calcCRC32(key.c_str()));
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::find(const typename container<OPE_TYPE>::key_type key)
	{
		iterator ite(*this, INVALID_INDEX);
		_find(ite, key);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::find(const char* key)
	{
		iterator ite(*this, true);
		_find(ite, GASHA_ calcCRC32(key));
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::find(const std::string& key)
	{
		iterator ite(*this, true);
		_find(ite, GASHA_ calcCRC32(key.c_str()));
		return ite;
	}

	//キー割り当て
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::assign(const typename container<OPE_TYPE>::key_type key)
	{
		lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
		return _assign(key);
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::assign(const char* key)
	{
		lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
		return _assign(GASHA_ calcCRC32(key));
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::assign(const std::string& key)
	{
		lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
		return _assign(GASHA_ calcCRC32(key.c_str()));
	}

	//キー割り当てして値を挿入（コピー）
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::insert(const typename container<OPE_TYPE>::key_type key, const typename container<OPE_TYPE>::value_type& value)
	{
		lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
		return _insert(key, value);
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::insert(const char* key, const typename container<OPE_TYPE>::value_type& value)
	{
		return insert(GASHA_ calcCRC32(key), value);
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::insert(const std::string& key, const typename container<OPE_TYPE>::value_type& value)
	{
		return insert(GASHA_ calcCRC32(key.c_str()), value);
	}

	//値を挿入（コピー）し、キーは自動割り当て
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::insertAuto(const typename container<OPE_TYPE>::value_type& value)
	{
		return insert(ope_type::getKey(value), value);
	}

	//キー割り当てして値を初期化（本体）
	template<class OPE_TYPE>
	template<typename... Tx>
	typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::_emplace(const typename container<OPE_TYPE>::key_type key, Tx&&... args)
	{
		value_type* assigned_value = _assign(key);
		if (!assigned_value)
			return nullptr;
		GASHA_ callConstructor<value_type>(assigned_value, std::forward<Tx>(args)...);//コンストラクタ呼び出し
		return assigned_value;
	}

	//キー割り当てして値を初期化
	template<class OPE_TYPE>
	template<typename... Tx>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::emplace(const typename container<OPE_TYPE>::key_type key, Tx&&... args)
	{
		lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
		return _emplace(key, std::forward<Tx>(args)...);
	}
	template<class OPE_TYPE>
	template<typename... Tx>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::emplace(const char* key, Tx&&... args)
	{
		return emplace(GASHA_ calcCRC32(key), std::forward<Tx>(args)...);
	}
	template<class OPE_TYPE>
	template<typename... Tx>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::emplace(const std::string& key, Tx&&... args)
	{
		return emplace(GASHA_ calcCRC32(key.c_str()), std::forward<Tx>(args)...);
	}

	//値を初期化して自動的にキー割り当て
	template<class OPE_TYPE>
	template<typename... Tx>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::emplaceAuto(Tx&&... args)
	{
		value_type value(std::forward<Tx>(args)...);
		return insertAuto(value);
	}

	//キーを削除
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::erase(const typename container<OPE_TYPE>::key_type key)
	{
		lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
		return _erase(key);
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::erase(const char* key)
	{
		return erase(GASHA_ calcCRC32(key));
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::erase(const std::string& key)
	{
		return erase(GASHA_ calcCRC32(key.c_str()));
	}

	//キーを削除
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::eraseAuto(const typename container<OPE_TYPE>::value_type& value)
	{
		return erase(ope_type::getKey(value));
	}

	//リハッシュ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::rehash()
	{
		lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
		return _rehash();
	}

	//クリア
	//※処理中、排他ロック（ライトロック）を取得する
	template<class OPE_TYPE>
	inline void container<OPE_TYPE>::clear()
	{
		lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
		_clear();
	}

	//デフォルトコンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::container() :
		m_using(),
		m_deleted(),
		m_usingCount(0),
		m_deletedCount(0),
		m_maxFindingCycle(0),
		m_lock()
	{}

}//namespace hash_table

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_HASH_TABLE_INL

// End of file

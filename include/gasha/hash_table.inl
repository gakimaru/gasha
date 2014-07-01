#pragma once
#ifndef GASHA_INCLUDED_HASH_TABLE_INL
#define GASHA_INCLUDED_HASH_TABLE_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// hash_table.inl
// 開番地法ハッシュテーブルコンテナ【インライン関数／テンプレート関数定義部】
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

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

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
		//iterator ite(*m_con, false);
		iterator ite(*this);
		ite += index;
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator[](const int index)
	{
		//iterator ite(*m_con, false);
		iterator ite(*this);
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
			m_set.m_index == rhs.m_set.m_index;
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
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator+=(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type rhs)
	{
		updateForward(rhs);
		return *this;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-=(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type rhs)
	{
		updateBackward(rhs);
		return *this;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator+(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type rhs) const
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type rhs) const
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::difference_type container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& rhs) const
	{
		if (m_set.m_index == INVALID_INDEX || rhs.m_set.m_index == INVALID_INDEX || m_set.m_index < rhs.m_set.m_index)
			return 0;
		return static_cast<difference_type>(m_set.m_index) - static_cast<difference_type>(rhs.m_set.m_index);
	}
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
		//reverse_iterator ite(*m_con, false);
		reverse_iterator ite(*this);
		ite += index;
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator[](const int index)
	{
		//reverse_iterator ite(*m_con, false);
		reverse_iterator ite(*this);
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
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator+=(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type rhs)
	{
		updateForward(rhs);
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator-=(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type rhs)
	{
		updateBackward(rhs);
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator+(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type rhs) const
	{
		reverse_iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator-(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type rhs) const
	{
		reverse_iterator ite(*this);
		ite -= rhs;
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::difference_type container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator-(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs) const
	{
		if (m_set.m_index == INVALID_INDEX || rhs.m_set.m_index == INVALID_INDEX || rhs.m_set.m_index < m_set.m_index)
			return 0;
		return static_cast<difference_type>(rhs.m_set.m_index) - static_cast<difference_type>(m_set.m_index);
	}
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

	//キーからインデックスの歩幅（第二ハッシュ）を計算
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::calcIndexStep(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key) const
	{
		return INDEX_STEP_BASE - key % INDEX_STEP_BASE;
	}
	//キーからインデックス（第一ハッシュ）を計算
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::calcIndex(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key) const
	{
		return calcIndexImpl<(TABLE_SIZE >= KEY_RANGE && KEY_RANGE > 0), TABLE_SIZE, KEY_MIN, KEY_RANGE >::calc(key);
	}
	//次のインデックスを計算（指定のインデックスに歩幅を加算）
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::calcNextIndex(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key, const typename container<OPE_TYPE, _TABLE_SIZE>::index_type index) const
	{
		return (index + calcIndexStep(key)) % TABLE_SIZE;
	}

	//キーで検索してインデックスを取得
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::_findIndex(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key) const
	{
		return _findIndexCommon(key);
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::_findIndex(const char* key) const
	{
		return _findIndexCommon(GASHA_ calcCRC32(key));
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::_findIndex(const std::string& key) const
	{
		return _findIndexCommon(GASHA_ calcCRC32(key.c_str()));
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::index_type container<OPE_TYPE, _TABLE_SIZE>::_findIndex(const typename container<OPE_TYPE, _TABLE_SIZE>::value_type& value) const
	{
		return _findIndexCommon(ope_type::getKey(value));
	}

	//キーで検索して値を取得
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::findValue(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key) const
	{
		return _findValue(key);
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::findValue(const char* key) const
	{
		return _findValue(GASHA_ calcCRC32(key));
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::findValue(const std::string& key) const
	{
		return _findValue(GASHA_ calcCRC32(key.c_str()));
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::findValue(const typename container<OPE_TYPE, _TABLE_SIZE>::value_type& value) const
	{
		return _findValue(ope_type::getKey(value));
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::findValue(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key)
	{
		return const_cast<value_type*>(_findValue(key));
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::findValue(const char* key)
	{
		return const_cast<value_type*>(_findValue(GASHA_ calcCRC32(key)));
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::findValue(const std::string& key)
	{
		return const_cast<value_type*>(_findValue(GASHA_ calcCRC32(key.c_str())));
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::findValue(const typename container<OPE_TYPE, _TABLE_SIZE>::value_type& value)
	{
		return const_cast<value_type*>(_findValue(ope_type::getKey(value)));
	}

	//キーで検索してイテレータを取得
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::find(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key) const
	{
		iterator ite(*this, true);
		_find(ite, key);
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::find(const char* key) const
	{
		iterator ite(*this, true);
		_find(ite, GASHA_ calcCRC32(key));
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::find(const std::string& key) const
	{
		iterator ite(*this, INVALID_INDEX);
		_find(ite, GASHA_ calcCRC32(key.c_str()));
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline const typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::find(const typename container<OPE_TYPE, _TABLE_SIZE>::value_type& value) const
	{
		iterator ite(*this, true);
		_find(ite, ope_type::getKey(value));
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::find(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key)
	{
		iterator ite(*this, INVALID_INDEX);
		_find(ite, key);
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::find(const char* key)
	{
		iterator ite(*this, true);
		_find(ite, GASHA_ calcCRC32(key));
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::find(const std::string& key)
	{
		iterator ite(*this, true);
		_find(ite, GASHA_ calcCRC32(key.c_str()));
		return ite;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::find(const typename container<OPE_TYPE, _TABLE_SIZE>::value_type& value)
	{
		iterator ite(*this, true);
		_find(ite, ope_type::getKey(value));
		return ite;
	}

	//キー割り当て
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::assign(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key)
	{
		lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
		return _assign(key);
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::assign(const char* key)
	{
		return assign(GASHA_ calcCRC32(key));
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::assign(const std::string& key)
	{
		return assign(GASHA_ calcCRC32(key.c_str()));
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::assign(const value_type& value)
	{
		return assign(ope_type::getKey(value));
	}

	//キー割り当てして値を挿入（コピー）
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::insert(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key, const typename container<OPE_TYPE, _TABLE_SIZE>::value_type& value)
	{
		lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
		return _insert(key, value);
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::insert(const char* key, const typename container<OPE_TYPE, _TABLE_SIZE>::value_type& value)
	{
		return insert(GASHA_ calcCRC32(key), value);
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::insert(const std::string& key, const typename container<OPE_TYPE, _TABLE_SIZE>::value_type& value)
	{
		return insert(GASHA_ calcCRC32(key.c_str()), value);
	}

	//値を挿入（コピー）し、キーは自動割り当て
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::insertAuto(const typename container<OPE_TYPE, _TABLE_SIZE>::value_type& value)
	{
		return insert(ope_type::getKey(value), value);
	}

	//キー割り当てして値を初期化（本体）
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	template<typename... Tx>
	typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::_emplace(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key, Tx&&... args)
	{
		value_type* assigned_value = _assign(key);
		if (!assigned_value)
			return nullptr;
		assigned_value = new(assigned_value)value_type(args...);//コンストラクタ呼び出し
		return assigned_value;
	}

	//キー割り当てして値を初期化
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	template<typename... Tx>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::emplace(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key, Tx&&... args)
	{
		lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
		return _emplace(key, args...);
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	template<typename... Tx>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::emplace(const char* key, Tx&&... args)
	{
		return emplace(GASHA_ calcCRC32(key), args...);
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	template<typename... Tx>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::emplace(const std::string& key, Tx&&... args)
	{
		return emplace(GASHA_ calcCRC32(key.c_str()), args...);
	}

	//値を初期化して自動的にキー割り当て
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	template<typename... Tx>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::value_type* container<OPE_TYPE, _TABLE_SIZE>::emplaceAuto(Tx&&... args)
	{
		value_type value(args...);
		return insertAuto(value);
	}

	//キーを削除
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::erase(const typename container<OPE_TYPE, _TABLE_SIZE>::key_type key)
	{
		lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
		return _erase(key);
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::erase(const char* key)
	{
		return erase(GASHA_ calcCRC32(key));
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::erase(const std::string& key)
	{
		return erase(GASHA_ calcCRC32(key.c_str()));
	}

	//キーを削除
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::eraseAuto(const typename container<OPE_TYPE, _TABLE_SIZE>::value_type& value)
	{
		return erase(ope_type::getKey(value));
	}

	//リハッシュ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::rehash()
	{
		lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
		return _rehash();
	}

	//クリア
	//※処理中、排他ロック（ライトロック）を取得する
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline void container<OPE_TYPE, _TABLE_SIZE>::clear()
	{
		lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
		_clear();
	}

	//デフォルトコンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline container<OPE_TYPE, _TABLE_SIZE>::container() :
		m_using(),
		m_deleted(),
		m_usingCount(0),
		m_deletedCount(0),
		m_maxFindingCycle(0)
	{}

}//namespace hash_table

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_HASH_TABLE_INL

// End of file

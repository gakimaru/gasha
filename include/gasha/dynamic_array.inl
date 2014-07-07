#pragma once
#ifndef GASHA_INCLUDED_DYNAMIC_ARRAY_INL
#define GASHA_INCLUDED_DYNAMIC_ARRAY_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// dynamic_array.inl
// 動的配列コンテナ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dynamic_array.h>//動的配列コンテナ【宣言部】

#include <gasha/is_ordered.h>//整列状態確認

#include <gasha/intro_sort.h>//イントロソート
#include <gasha/insertion_sort.h>//挿入ソート

#include <gasha/linear_search.h>//線形探索
#include <gasha/binary_search.h>//二分探索

#include <utility>//C++11 std::forwad

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace dynamic_array
{
	//--------------------
	//イテレータのインライン関数
	
	//基本オペレータ
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator[](const int index) const
	{
		//iterator ite(*m_con, false);
		//ite.update(index);
		iterator ite(*this);
		ite.addIndexAndUpdate(index);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator[](const int index)
	{
		//iterator ite(*m_con, false);
		//ite.update(index);
		iterator ite(*this);
		ite.addIndexAndUpdate(index);
		return ite;
	}
	//比較オペレータ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator==(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_index == rhs.m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator!=(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_index != rhs.m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator>(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_index > rhs.m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator>=(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_index >= rhs.m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator<(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_index < rhs.m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator<=(const iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_index <= rhs.m_index;
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
		if (m_index == INVALID_INDEX || rhs.m_index == INVALID_INDEX)
			return 0;
		return static_cast<difference_type>(m_index) - static_cast<difference_type>(rhs.m_index);
	}
	//アクセッサ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isExist() const
	{
		return m_index != INVALID_INDEX && m_index < m_con->m_size;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isEnabled() const
	{
		return m_index != INVALID_INDEX;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isEnd() const//終端か？
	{
		return m_index == m_con->m_size;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::iterator::getIndex() const//インデックス
	{ 
		return m_index;
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
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator[](const int index) const
	{
		//reverse_iterator ite(*m_con, false);
		//ite.update(m_con->m_size - index);
		reverse_iterator ite(*this);
		ite.addIndexAndUpdate(index);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator[](const int index)
	{
		//reverse_iterator ite(*m_con, false);
		//ite.update(m_con->m_size - index);
		reverse_iterator ite(*this);
		ite.addIndexAndUpdate(index);
		return ite;
	}
	//比較オペレータ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator==(const reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_index == m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator!=(const reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_index != m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator>(const reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_index > m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator>=(const reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_index >= m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator<(const reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_index < m_index;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator<=(const reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_index <= m_index;
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
		if (m_index == INVALID_INDEX || rhs.m_index == INVALID_INDEX)
			return 0;
		return static_cast<difference_type>(rhs.m_index) - static_cast<difference_type>(m_index);
	}
	//アクセッサ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isExist() const
	{
		return m_index != INVALID_INDEX && m_index > 0;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isEnabled() const
	{
		return m_index != INVALID_INDEX;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isEnd() const//終端か？
	{
		return m_index == 0;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::index_type container<OPE_TYPE>::reverse_iterator::getIndex() const//インデックス
	{
		return m_index - 1;
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
	//コンテナ本体のインライン関数／テンプレート関数

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
			value_type* value = _refElement(m_size);
			for (index_type index = m_size; index < _size; ++index, ++value)
				new(value)value_type(std::forward<Tx>(args)...);//コンストラクタ呼び出し
		}
		else if (_size < m_size)
		{
			value_type* value = _refElement(size);
			for (index_type index = size; index < m_size; ++index, ++value)
			{
				ope_type::callDestructor(value);//デストラクタ呼び出し
				operator delete(value, value);//（作法として）deleteオペレータ呼び出し
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
			value_type* value = _refFront();
			for (index_type index = 0; index < used_size; ++index, ++value)
			{
				ope_type::callDestructor(value);//デストラクタ呼び出し
				operator delete(value, value);//（作法として）deleteオペレータ呼び出し
			}
		}
		{
			value_type* value = _refFront();
			for (index_type index = 0; index < _size; ++index, ++value)
				new(value)value_type(std::forward<Tx>(args)...);//コンストラクタ呼び出し
		}
		if (m_size < _size)
			m_size = _size;
		return m_size;
	}
	
	//末尾に要素を追加
	//※パラメータ渡し
	template<class OPE_TYPE>
	template<typename... Tx>
	typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::push_back(Tx&&... args)
	{
		value_type* obj = refNew();//サイズチェック含む
		if (!obj)
			return nullptr;
		new(obj)value_type(std::forward<Tx>(args)...);//コンストラクタ呼び出し
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
		value_type* new_value = _refElement(index);
		for (size_type i = 0; i < _num; ++i, ++new_value)
			new(new_value)value_type(std::forward<Tx>(args)...);
		//終了
		iterator now(*this, index);
		return now;
	}
	
	//ソート
	template<class OPE_TYPE>
	inline void container<OPE_TYPE>::sort()
	{
		GASHA_ introSort(_refFront(), m_size, typename ope_type::predicateForSort());
	}
	//※プレディケート関数指定版
	template<class OPE_TYPE>
	template<class PREDICATE>
	inline void container<OPE_TYPE>::sort(PREDICATE predicate)
	{
		GASHA_ introSort(_refFront(), m_size, predicate);
	}
	//安定ソート
	template<class OPE_TYPE>
	inline void container<OPE_TYPE>::stableSort()
	{
		GASHA_ insertionSort(_refFront(), m_size, typename ope_type::predicateForSort());
	}
	//※プレディケート関数指定版
	template<class OPE_TYPE>
	template<class PREDICATE>
	inline void container<OPE_TYPE>::stableSort(PREDICATE predicate)
	{
		GASHA_ insertionSort(_refFront(), m_size, predicate);
	}
	//ソート済み状態チェック
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::isOrdered() const
	{
		return GASHA_ isOrdered(_refFront(), m_size, typename ope_type::predicateForSort());
	}
	//※プレディケート関数指定版
	template<class OPE_TYPE>
	template<class PREDICATE>
	inline bool container<OPE_TYPE>::isOrdered(PREDICATE predicate) const
	{
		return GASHA_ isOrdered(_refFront(), m_size, predicate);
	}
	//線形探索
	//※探索値指定版
	template<class OPE_TYPE>
	template<typename V>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::findValue(const V& value)
	{
		value_type* found = GASHA_ linearSearchValue(_refFront(), m_size, value, typename ope_type::predicateForFind());
		const index_type index = found ? _refIndex(found) : INVALID_INDEX;
		iterator ite(*this, index);
		return ite;
	}
	//※比較関数＋値指定版
	template<class OPE_TYPE>
	template<typename V, class PREDICATE>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::findValue(const V& value, PREDICATE predicate)
	{
		value_type* found = GASHA_ linearSearchValue(_refFront(), m_size, value, predicate);
		const index_type index = found ? _refIndex(found) : INVALID_INDEX;
		iterator ite(*this, index);
		return ite;
	}
	//※比較関数指定版
	template<class OPE_TYPE>
	template<class PREDICATE>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::find(PREDICATE predicate)
	{
		value_type* found = GASHA_ linearSearch(_refFront(), m_size, predicate);
		const index_type index = found ? _refIndex(found) : INVALID_INDEX;
		iterator ite(*this, index);
		return ite;
	}
	//二分探索
	//※探索値指定版
	template<class OPE_TYPE>
	template<typename V>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::binarySearchValue(const V& value)
	{
		value_type* found = GASHA_ binarySearchValue(_refFront(), m_size, value, typename ope_type::comparisonForSearch());
		const index_type index = found ? _refIndex(found) : INVALID_INDEX;
		iterator ite(*this, index);
		return ite;
	}
	//※比較関数＋値指定版
	template<class OPE_TYPE>
	template<typename V, class COMPARISON>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::binarySearchValue(const V& value, COMPARISON comparison)
	{
		value_type* found = GASHA_ binarySearchValue(_refFront(), m_size, value, comparison);
		const index_type index = found ? _refIndex(found) : INVALID_INDEX;
		iterator ite(*this, index);
		return ite;
	}
	//※比較関数指定版
	template<class OPE_TYPE>
	template<class COMPARISON>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::binary_search(COMPARISON comparison)
	{
		value_type* found = GASHA_ binarySearch(_refFront(), m_size, comparison);
		const index_type index = found ? _refIndex(found) : INVALID_INDEX;
		iterator ite(*this, index);
		return ite;
	}
	
	//コンストラクタ
	//※初期状態で使用中の要素数を指定する（-1で全要素を使用中にする）
	//※要素の初期化は行わない（必要なら size に 0 を指定して、後で resize() を呼び出す）
	template<class OPE_TYPE>
	template<std::size_t N>
	container<OPE_TYPE>::container(typename container<OPE_TYPE>::value_type(&array)[N], const int size, const autoClearAttr_t auto_clear_attr) :
		m_array(array),
		m_maxSizeReal(N),
		m_maxSize(N),
		m_size(size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size)),
		m_autoClearAttr(auto_clear_attr)
	{}
	
	//デフォルトコンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::container() :
		m_array(nullptr),
		m_maxSizeReal(0),
		m_maxSize(0),
		m_size(0),
		m_autoClearAttr(NEVER_CLEAR)
	{}

}//namespace dynamic_array

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_DYNAMIC_ARRAY_INL

// End of file

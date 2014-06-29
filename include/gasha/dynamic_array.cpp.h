#pragma once
#ifndef __DYNAMIC_ARRAY_CPP_H_
#define __DYNAMIC_ARRAY_CPP_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// dynamic_array.cpp.h
// 動的配列コンテナ【関数定義部】
//
// ※コンテナクラスの実体化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dynamic_array.inl>//動的配列コンテナ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace dynamic_array
{
	//----------------------------------------
	//イテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::update(const typename container<OPE_TYPE>::index_type index) const
	{
		//if (index == INVALID_INDEX || index < 0 || index > static_cast<index_type>(m_con->m_size))
		if (index > static_cast<index_type>(m_con->m_size))
		{
			m_index = INVALID_INDEX;
			m_value = nullptr;
		}
		else
		{
			m_index = index;
			m_value = const_cast<value_type*>(m_con->_refElement(m_index));
		}
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::addIndexAndUpdate(const typename container<OPE_TYPE>::difference_type add) const
	{
		update(static_cast<index_type>(static_cast<difference_type>(m_index) + add));
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
	}
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const container<OPE_TYPE>& con, const bool is_end) :
		m_con(&con),
		m_index(INVALID_INDEX),
		m_value(nullptr)
	{
		if (!is_end)
			update(0);//先頭データ
		else
			update(m_con->m_size);//末尾データ
	}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const container<OPE_TYPE>& con, const typename container<OPE_TYPE>::index_type index) :
		m_con(&con),
		m_index(INVALID_INDEX),
		m_value(nullptr)
	{
		update(index);
	}

	//----------------------------------------
	//リバースイテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::update(const typename container<OPE_TYPE>::index_type index) const
	{
		//if (index == INVALID_INDEX || index < 0 || index > static_cast<index_type>(m_con->m_size))
		if (index > static_cast<index_type>(m_con->m_size))
		{
			m_index = INVALID_INDEX;
			m_value = nullptr;
		}
		else
		{
			m_index = index;
			m_value = const_cast<value_type*>(m_con->_refElement(m_index)) - 1;
		}
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::addIndexAndUpdate(const typename container<OPE_TYPE>::difference_type add) const
	{
		update(static_cast<index_type>(static_cast<difference_type>(m_index) - add));
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(obj.m_index);
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
	}
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const container<OPE_TYPE>& con, const bool is_end) :
		m_con(&con),
		m_index(INVALID_INDEX),
		m_value(nullptr)
	{
		if (!is_end)
			update(m_con->m_size);//末尾データ
		else
			update(0);//先頭データ
	}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const  container<OPE_TYPE>& con, const typename  container<OPE_TYPE>::index_type index) :
		m_con(&con),
		m_index(INVALID_INDEX),
		m_value(nullptr)
	{
		update(index);
	}

	//----------------------------------------
	//コンテナ本体のメソッド

	//配列の再割り当て
	//※ポインタと配列要素数指定版
	template<class OPE_TYPE>
	void container<OPE_TYPE>::assignArray(value_type* array, const typename container<OPE_TYPE>::size_type max_size, const int size)
	{
		if (m_array && m_autoClearAttr == AUTO_CLEAR)
			clear();//クリア
		m_array = array;
		m_maxSizeReal = max_size;
		m_maxSize = max_size;
		m_size = size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size);
	}
	
	//最大要素数の変更（新しい最大要素数を返す）
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::size_type container<OPE_TYPE>::reserve(const typename container<OPE_TYPE>::size_type max_size)
	{
		if (max_size < m_size)
			m_maxSize = m_size;
		else if (max_size > m_maxSizeReal)
			m_maxSize = m_maxSizeReal;
		else
			m_maxSize = max_size;
		return m_maxSize;
	}
	
	//最大要素数の変更（新しい最大要素数を返す）
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::size_type container<OPE_TYPE>::shrink(const typename container<OPE_TYPE>::size_type max_size)
	{
		if (max_size < m_size)
		{
			value_type* value = _refElement(max_size);
			for (index_type index = max_size; index < m_size; ++index, ++value)
			{
				ope_type::callDestructor(value);//デストラクタ呼び出し
				operator delete(value, value);//（作法として）deleteオペレータ呼び出し
			}
			m_size = max_size;
			m_maxSize = m_size;
		}
		else if (max_size > m_maxSizeReal)
			m_maxSize = m_maxSizeReal;
		else
			m_maxSize = max_size;
		return m_maxSize;
	}
	
	//最大要素数の変更（新しい最大要素数を返す）
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::size_type container<OPE_TYPE>::shrink_to_fit()
	{
		m_maxSize = m_size;
		return m_maxSize;
	}
	
	//使用中のサイズを変更（新しいサイズを返す）
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::size_type container<OPE_TYPE>::resize(const int size, const typename container<OPE_TYPE>::value_type& new_value)
	{
		const size_type _size = size < 0 ? m_maxSize : static_cast<size_type>(size) < m_maxSize ? static_cast<size_type>(size) : m_maxSize;
		if (_size > m_size)
		{
			value_type* value = _refElement(m_size);
			for (index_type index = m_size; index < _size; ++index, ++value)
				*value = new_value;//新しい値を初期化
		}
		else if (_size < m_size)
		{
			value_type* value = _refElement(_size);
			for (index_type index = _size; index < m_size; ++index, ++value)
			{
				ope_type::callDestructor(value);//デストラクタ呼び出し
				operator delete(value, value);//（作法として）deleteオペレータ呼び出し
			}
		}
		m_size = _size;
		return m_size;
	}
	
	//使用中のサイズを変更（新しいサイズを返す）
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::size_type container<OPE_TYPE>::resizeSilently(const int size)
	{
		const size_type _size = size < 0 ? m_maxSize : static_cast<size_type>(size) < m_maxSize ? static_cast<size_type>(size) : m_maxSize;
		m_size = _size;
		return m_size;
	}
	
	//先頭から指定数の要素にデータを割り当てる
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::size_type container<OPE_TYPE>::assign(const int size, const typename container<OPE_TYPE>::value_type& new_value)
	{
		const size_type _size = size < 0 ? m_maxSize : static_cast<size_type>(size) < m_maxSize ? static_cast<size_type>(size) : m_maxSize;
	#if 0
		//上書き前のデストラクタ呼び出し → ..は、やらず、コピーオペレータで直接上書きする
		{
			const size_type used_size = _size < m_size ? _size : m_size;
			value_type* value = _refFront();
			for (index_type index = 0; index < used_size; ++index, ++value)
			{
				ope_type::callDestructor(value);//デストラクタ呼び出し
				operator delete(value, value);//（作法として）deleteオペレータ呼び出し
			}
		}
	#endif
		{
			value_type* value = _refFront();
			for (index_type index = 0; index < _size; ++index, ++value)
				*value = new_value;//コピーオペレータでデータを上書き
		}
		if(m_size < _size)
			m_size = _size;
		return m_size;
	}
	
	//末尾に要素を追加
	//※オブジェクト渡し
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::push_back(const typename container<OPE_TYPE>::value_type&& src)//ムーブ版
	{
		value_type* obj = refNew();//サイズチェック含む
		if (!obj)
			return nullptr;
		*obj = std::move(src);
		++m_size;
		return obj;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::push_back(const typename container<OPE_TYPE>::value_type& src)//コピー版
	{
		value_type* obj = refNew();//サイズチェック含む
		if (!obj)
			return nullptr;
		*obj = src;
		++m_size;
		return obj;
	}
	
	//末尾の要素を削除
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::pop_back()
	{
		if (m_size == 0)
			return false;
		value_type* value = const_cast<value_type*>(refBack());
		ope_type::callDestructor(value);//デストラクタ呼び出し
		operator delete(value, value);//（作法として）deleteオペレータ呼び出し
		--m_size;
		return true;
	}
	//※オブジェクトの値を受け取る
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::pop_back(value_type& value)
	{
		if (m_size == 0)
			return false;
		value_type* obj = const_cast<value_type*>(refBack());
		value = std::move(*obj);//ムーブ
		ope_type::callDestructor(obj);//デストラクタ呼び出し
		operator delete(obj, obj);//（作法として）deleteオペレータ呼び出し
		--m_size;
		return true;
	}
	
	//クリア
	template<class OPE_TYPE>
	void container<OPE_TYPE>::clear()
	{
		if (m_size == 0)
			return;
		value_type* value = _refFront();
		for (size_type i = 0; i < m_size; ++i, ++value)
		{
			ope_type::callDestructor(value);//デストラクタ呼び出し
			operator delete(value, value);//（作法として）deleteオペレータ呼び出し
		}
		m_size = 0;
	}
	
	//要素の移動（昇順）
	template<class OPE_TYPE>
	void container<OPE_TYPE>::moveAsc(const typename container<OPE_TYPE>::index_type dst_pos, const typename container<OPE_TYPE>::index_type src_pos, const typename container<OPE_TYPE>::size_type num)
	{
		value_type* dst = _refElement(dst_pos);
		value_type* src = _refElement(src_pos);
		value_type* end = _refNew();
		for (size_type i = 0; i < num; ++i)
		{
			if (dst >= end)
				new(dst)value_type(std::move(*src));//ムーブコンストラクタ
			else
				*dst = std::move(*src);//ムーブオペレータ
			++dst;
			++src;
		}
	}
	
	//要素の移動（降順）
	template<class OPE_TYPE>
	void container<OPE_TYPE>::moveDesc(const typename container<OPE_TYPE>::index_type dst_pos, const typename container<OPE_TYPE>::index_type src_pos, const typename container<OPE_TYPE>::size_type num)
	{
		value_type* dst = _refElement(dst_pos + num - 1);
		value_type* src = _refElement(src_pos + num - 1);
		value_type* end = _refNew();
		for (size_type i = 0; i < num; ++i)
		{
			if (dst >= end)
				new(dst)value_type(std::move(*src));//ムーブコンストラクタ
			else
				*dst = std::move(*src);//ムーブオペレータ
			--dst;
			--src;
		}
	}
	
	//要素の挿入
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::insert(typename container<OPE_TYPE>::iterator pos, const int num, typename container<OPE_TYPE>::value_type& value)
	{
		if (pos.isNotEnabled() || num == 0 || m_size == m_maxSize)
		{
			iterator ite(*this, INVALID_INDEX);
			return ite;
		}
		index_type index = pos.getIndex();
		const size_type remain = m_maxSize - m_size;
		const size_type _num = num < 0 || static_cast<size_type>(num) > remain ? remain : static_cast<size_type>(num);
		//移動
		moveDesc(index + _num, index, _num);
		//要素数変更
		m_size += _num;
		//挿入
		value_type* new_value = _refElement(index);
		for (size_type i = 0; i < _num; ++i, ++new_value)
			*new_value = value;
		//終了
		iterator now(*this, index);
		return now;
	}
	
	//要素の削除
	template<class OPE_TYPE>
	void container<OPE_TYPE>::_erase(const typename container<OPE_TYPE>::index_type index, const typename container<OPE_TYPE>::size_type num)
	{
		const size_type remain = m_maxSize - m_size;
		const size_type _num = num < 0 || num > remain ? remain : num;
		const size_type move_num = m_size - index;
		//削除
		value_type* delete_value = _refElement(index);
		for (size_type i = 0; i < _num; ++i, ++delete_value)
		{
			ope_type::callDestructor(delete_value);//デストラクタ呼び出し
			operator delete(delete_value, delete_value);//（作法として）deleteオペレータ呼び出し
		}
		//移動
		moveAsc(index, index + _num, move_num);
		//要素数変更
		m_size -= _num;
	}
	
	//要素の削除
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::erase(typename container<OPE_TYPE>::iterator pos, const int num)
	{
		if (pos.isNotExist() || num == 0 || m_size == 0)
		{
			iterator ite(*this, INVALID_INDEX);
			return ite;
		}
		const index_type index = pos.getIndex();
		//削除
		_erase(index, num);
		//終了
		iterator now(*this, index);
		return now;
	}
	//※範囲指定版
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator container<OPE_TYPE>::erase(typename container<OPE_TYPE>::iterator start, typename container<OPE_TYPE>::iterator end)
	{
		if (start.isNotExist() || end.isNotExist() || start >= end || m_size == 0)
		{
			iterator ite(*this, INVALID_INDEX);
			return ite;
		}
		index_type index = start.getIndex();
		index_type end_index = end.getIndex();
		const size_type num = end_index - index;
		//削除
		_erase(index, num);
		//終了
		iterator now(*this, index);
		return now;
	}
	
	//コンストラクタ
	//※ポインタと配列要素数指定版
	template<class OPE_TYPE>
	container<OPE_TYPE>::container(typename container<OPE_TYPE>::value_type* array, const typename container<OPE_TYPE>::size_type max_size, const int size, const autoClearAttr_t auto_clear_attr) :
		m_array(array),
		m_maxSizeReal(max_size),
		m_maxSize(max_size),
		m_size(size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size)),
		m_autoClearAttr(auto_clear_attr)
	{}
	//※voidポインタとバッファサイズ数指定版
	template<class OPE_TYPE>
	container<OPE_TYPE>::container(void* buff_ptr, const typename container<OPE_TYPE>::size_type buff_size, const int size, const autoClearAttr_t auto_clear_attr) :
		m_array(static_cast<value_type*>(buff_ptr)),
		m_maxSizeReal(buff_size / sizeof(value_type)),
		m_maxSize(m_maxSizeReal),
		m_size(size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size)),
		m_autoClearAttr(auto_clear_attr)
	{}
	//デストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::~container()
	{
		//自動クリア属性が有効なら、自動クリアする
		if (m_array && m_autoClearAttr == AUTO_CLEAR)
			clear();
	}
	
}//namespace dynamic_array

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__DYNAMIC_ARRAY_CPP_H_

// End of file

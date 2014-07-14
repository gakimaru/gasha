#pragma once
#ifndef GASHA_INCLUDED_RING_BUFFER_CPP_H
#define GASHA_INCLUDED_RING_BUFFER_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// ring_buffer.cpp.h
// リングバッファコンテナ【関数／実体定義部】
//
// ※クラスのインスタンス化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。
// 　（コンパイル・リンク時間への影響を気にしないならOK）
// ※明示的なインスタンス化を避けたい場合は、ヘッダーファイルと共にインクルード。
// 　（この場合、実際に使用するメンバー関数しかインスタンス化されないので、対象クラスに不要なインターフェースを実装しなくても良い）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/ring_buffer.inl>//リングバッファコンテナ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace ring_buffer
{
	//----------------------------------------
	//イテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::update(const typename container<OPE_TYPE>::index_type logical_index) const
	{
		//if (logical_index == INVALID_INDEX || logical_index < 0 || logical_index > static_cast<index_type>(m_con->m_size))
		if (logical_index > static_cast<index_type>(m_con->m_size))
		{
			m_logicalIndex = INVALID_INDEX;
			m_value = nullptr;
		}
		else
		{
			m_logicalIndex = logical_index;
			const index_type real_index = m_con->_toRealIndex(m_logicalIndex);
			m_value = const_cast<value_type*>(m_con->_refRealElement(real_index));
		}
	}
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::addIndexAndUpdate(const typename container<OPE_TYPE>::difference_type add) const
	{
		update(static_cast<index_type>(static_cast<difference_type>(m_logicalIndex) + add));
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		update(m_logicalIndex);
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		update(m_logicalIndex);
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(nullptr)
	{
		update(m_logicalIndex);
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(nullptr)
	{
		update(m_logicalIndex);
	}
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const container<OPE_TYPE>& con, const bool is_end) :
		m_con(&con),
		m_logicalIndex(INVALID_INDEX),
		m_value(nullptr)
	{
		if (!is_end)
			update(0);//先頭データ
		else
			update(m_con->m_size);//末尾データ
	}
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const container<OPE_TYPE>& con, const typename container<OPE_TYPE>::index_type logical_index) :
		m_con(&con),
		m_logicalIndex(INVALID_INDEX),
		m_value(nullptr)
	{
		update(logical_index);
	}

	//----------------------------------------
	//リバースイテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::update(const typename container<OPE_TYPE>::index_type logical_index) const
	{
		//if (logical_index == INVALID_INDEX || logical_index < 0 || index > static_cast<index_type>(m_con->m_size))
		if (logical_index > static_cast<index_type>(m_con->m_size))
		{
			m_logicalIndex = INVALID_INDEX;
			m_value = nullptr;
		}
		else
		{
			m_logicalIndex = logical_index;
			const index_type real_index = m_con->_toRealIndex(m_logicalIndex);
			m_value = real_index == 0 ? const_cast<value_type*>(m_con->_refRealElement(m_con->m_maxSize - 1)) : const_cast<value_type*>(m_con->_refRealElement(real_index - 1));
		}
	}
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::addIndexAndUpdate(const typename container<OPE_TYPE>::difference_type add) const
	{
		update(static_cast<index_type>(static_cast<difference_type>(m_logicalIndex) - add));
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(typename container<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		update(m_logicalIndex);
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(const typename container<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		update(m_logicalIndex);
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(nullptr)
	{
		update(m_logicalIndex);
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(nullptr)
	{
		update(m_logicalIndex);
	}
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const container<OPE_TYPE>& con, const bool is_end) :
		m_con(&con),
		m_logicalIndex(INVALID_INDEX),
		m_value(nullptr)
	{
		if (!is_end)
			update(m_con->m_size);//末尾データ
		else
			update(0);//先頭データ
	}
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const  container<OPE_TYPE>& con, const typename  container<OPE_TYPE>::index_type logical_index) :
		m_con(&con),
		m_logicalIndex(INVALID_INDEX),
		m_value(nullptr)
	{
		update(logical_index);
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
		m_maxSize = max_size;
		m_size = size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size);
		m_offset = 0;
	}

	//使用中のサイズを変更（新しいサイズを返す）
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::size_type container<OPE_TYPE>::resize(const int size, const typename container<OPE_TYPE>::value_type& new_value)
	{
		const size_type _size = size < 0 ? m_maxSize : static_cast<size_type>(size) < m_maxSize ? static_cast<size_type>(size) : m_maxSize;
		if (_size > m_size)
		{
			for (index_type index = m_size; index < _size; ++index)
			{
				value_type* value = _refElement(index);
				*value = new_value;//新しい値を初期化
			}
		}
		else if (_size < m_size)
		{
			for (index_type index = _size; index < m_size; ++index)
			{
				value_type* value = _refElement(index);
				ope_type::callDestructor(value);//デストラクタ呼び出し
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
	typename container<OPE_TYPE>::size_type container<OPE_TYPE>::assign(const int size, const value_type& new_value)
	{
		const size_type _size = size < 0 ? m_maxSize : static_cast<size_type>(size) < m_maxSize ? static_cast<size_type>(size) : m_maxSize;
	#if 0
		//上書き前のデストラクタ呼び出し → ..は、やらず、コピーオペレータで直接上書きする
		{
			const size_type used_size = _size < m_size ? _size : m_size;
			for (index_type index = 0; index < used_size; ++index)
			{
				value_type* value = _refElement(index);
				ope_type::callDestructor(value);//デストラクタ呼び出し
			}
		}
	#endif
		{
			for (index_type index = 0; index < _size; ++index)
			{
				value_type* value = _refElement(index);
				*value = new_value;//コピーオペレータでデータを上書き
			}
		}
		if (m_size < _size)
			m_size = _size;
		return m_size;
	}

	//先頭に要素を追加
	//※オブジェクト渡し
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::push_front(typename container<OPE_TYPE>::value_type&& src)//ムーブ版
	{
		value_type* obj = refFrontNew();//サイズチェック含む
		if (!obj)
			return nullptr;
		*obj = std::move(src);
		++m_size;
		m_offset = m_offset == 0 ? m_maxSize - 1 : m_offset - 1;
		return obj;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::push_front(const typename container<OPE_TYPE>::value_type& src)//コピー版
	{
		value_type* obj = refFrontNew();//サイズチェック含む
		if (!obj)
			return nullptr;
		*obj = src;
		++m_size;
		m_offset = m_offset == 0 ? m_maxSize - 1 : m_offset - 1;
		return obj;
	}

	//末尾に要素を追加
	//※オブジェクト渡し
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::push_back(typename container<OPE_TYPE>::value_type&& src)//ムーブ版
	{
		value_type* obj = refBackNew();//サイズチェック含む
		if (!obj)
			return nullptr;
		*obj = std::move(src);
		++m_size;
		return obj;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::push_back(const typename container<OPE_TYPE>::value_type& src)//コピー版
	{
		value_type* obj = refBackNew();//サイズチェック含む
		if (!obj)
			return nullptr;
		*obj = src;
		++m_size;
		return obj;
	}

	//先頭の要素を削除
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::pop_front()
	{
		if (m_size == 0)
			return false;
		value_type* value = const_cast<value_type*>(refFront());
		ope_type::callDestructor(value);//デストラクタ呼び出し
		--m_size;
		m_offset = m_offset == m_maxSize - 1 ? 0 : m_offset + 1;
		return true;
	}
	//※オブジェクトの値を受け取る
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::pop_front(typename container<OPE_TYPE>::value_type& value)
	{
		if (m_size == 0)
			return false;
		value_type* obj = const_cast<value_type*>(refFront());
		value = std::move(*obj);//ムーブ
		ope_type::callDestructor(obj);//デストラクタ呼び出し
		--m_size;
		m_offset = m_offset == m_maxSize - 1 ? 0 : m_offset + 1;
		return true;
	}

	//末尾の要素を削除
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::pop_back()
	{
		if (m_size == 0)
			return false;
		value_type* value = const_cast<value_type*>(refFront());
		ope_type::callDestructor(value);//デストラクタ呼び出し
		--m_size;
		return true;
	}
	//※オブジェクトの値を受け取る
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::pop_back(typename container<OPE_TYPE>::value_type& value)
	{
		if (m_size == 0)
			return false;
		value_type* obj = const_cast<value_type*>(refBack());
		value = std::move(*obj);//ムーブ
		ope_type::callDestructor(obj);//デストラクタ呼び出し
		--m_size;
		return true;
	}

	//クリア
	template<class OPE_TYPE>
	void container<OPE_TYPE>::clear()
	{
		if (m_size == 0)
			return;
		for (size_type i = 0; i < m_size; ++i)
		{
			value_type* value = _refElement(i);
			ope_type::callDestructor(value);//デストラクタ呼び出し
		}
		m_size = 0;
		m_offset = 0;
	}
	
	//要素の移動（昇順）
	template<class OPE_TYPE>
	void container<OPE_TYPE>::moveAsc(const index_type dst_pos, const index_type src_pos, const size_type num)
	{
		index_type _dst_pos = dst_pos;
		index_type _src_pos = src_pos;
		for (size_type i = 0; i < num; ++i)
		{
			value_type* dst = _refElement(_dst_pos);
			value_type* src = _refElement(_src_pos);
			if (_dst_pos >= m_size)
				GASHA_ callConstructor<value_type>(dst, std::move(*src));//ムーブコンストラクタ
			else
				*dst = std::move(*src);//ムーブオペレータ
			++_dst_pos;
			++_src_pos;
		}
	}

	//要素の移動（降順）
	template<class OPE_TYPE>
	void container<OPE_TYPE>::moveDesc(const index_type dst_pos, const index_type src_pos, const size_type num)
	{
		index_type _dst_pos = dst_pos + num - 1;
		index_type _src_pos = src_pos + num - 1;
		for (size_type i = 0; i < num; ++i)
		{
			value_type* dst = _refElement(_dst_pos);
			value_type* src = _refElement(_src_pos);
			if (_dst_pos >= m_size)
				GASHA_ callConstructor<value_type>(dst, std::move(*src));//ムーブコンストラクタ
			else
				*dst = std::move(*src);//ムーブオペレータ
			--_dst_pos;
			--_src_pos;
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
		index_type _index = index;
		for (size_type i = 0; i < _num; ++i)
		{
			value_type* new_value = _refElement(_index);
			*new_value = value;
			++_index;
		}
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
		index_type _index = index;
		for (size_type i = 0; i < _num; ++i)
		{
			value_type* delete_value = _refElement(_index);
			ope_type::callDestructor(delete_value);//デストラクタ呼び出し
			++_index;
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
		m_maxSize(max_size),
		m_size(size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size)),
		m_offset(0),
		m_autoClearAttr(auto_clear_attr)
	{}
	//※voidポインタとバッファサイズ数指定版
	template<class OPE_TYPE>
	container<OPE_TYPE>::container(void* buff_ptr, const typename container<OPE_TYPE>::size_type buff_size, const int size, const autoClearAttr_t auto_clear_attr) :
		m_array(static_cast<value_type*>(buff_ptr)),
		m_maxSize(buff_size / sizeof(value_type)),
		m_size(size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size)),
		m_offset(0),
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

}//namespace ring_buffer

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//リングバッファコンテナの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_rBuff(OPE_TYPE) \
	template class GASHA_ ring_buffer::container<OPE_TYPE>;

//シンプルリングバッファコンテナの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_simpleRBuff(VALUE_TYPE) \
	template class GASHA_ ring_buffer::simpleContainer<VALUE_TYPE>; \
	template class GASHA_ ring_buffer::container<typename GASHA_ ring_buffer::simpleContainer<VALUE_TYPE>::ope>;

//--------------------------------------------------------------------------------
//【注】明示的インスタンス化に失敗する場合
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
// 　  source_file.cpp.h:114:17: エラー: duplicate explicit instantiation of ‘class gasha::templateClass<>’ [-fpermissive]
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
// ※このコメントは、「明示的なインスタンス化マクロ」が定義されている全てのソースコードに
// 　同じ内容のものをコピーしています。

#endif//GASHA_INCLUDED_RING_BUFFER_CPP_H

// End of file

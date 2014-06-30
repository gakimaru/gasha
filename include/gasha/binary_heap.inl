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

#include <utility>//C++11 std::move

//【VC++】ワーニング設定を退避
#pragma warning(push)

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
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator+=(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type rhs)
	{
		addIndexAndUpdate(rhs);
		return *this;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-=(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type rhs)
	{
		addIndexAndUpdate(-rhs);
		return *this;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator+(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type rhs) const
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::iterator container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type rhs) const
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::difference_type container<OPE_TYPE, _TABLE_SIZE>::iterator::operator-(const typename container<OPE_TYPE, _TABLE_SIZE>::iterator& rhs) const
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

#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	//--------------------
	//リバースイテレータのインライン関数

	//基本オペレータ
#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE//std::input_iterator_tag には本来必要ではない
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
#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE//std::input_iterator_tag には本来必要ではない
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
#ifdef GASHA_BINARY_HEAP_ENABLE_RANDOM_ACCESS_INTERFACE//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator+=(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type rhs)
	{
		addIndexAndUpdate(rhs);
		return *this;
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator::operator-=(const typename container<OPE_TYPE, _TABLE_SIZE>::difference_type rhs)
	{
		addIndexAndUpdate(-rhs);
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
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR

	//--------------------
	//単一操作オブジェクト（安全なプッシュ／ポップ操作クラス）

	//プッシュ開始
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	template<typename... Tx>
	typename container<OPE_TYPE, _TABLE_SIZE>::uniqueOperation::node_type* container<OPE_TYPE, _TABLE_SIZE>::uniqueOperation::pushBegin(const Tx&... args)
	{
		if (m_status == status_t::PUSH_BEGINNING || m_status == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
			return nullptr;
		node_type* node = m_container.pushBegin(args...);//プッシュ開始
		if (node)
			m_status = status_t::PUSH_BEGINNING;//ステータス変更
		return node;
	}

	//ムーブコンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline container<OPE_TYPE, _TABLE_SIZE>::uniqueOperation::uniqueOperation(typename container<OPE_TYPE, _TABLE_SIZE>::uniqueOperation&& obj) :
		m_container(obj.m_container),
		m_status(obj.m_status)
	{
		obj.m_status = status_t::IDLE;
	}

	//コンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline container<OPE_TYPE, _TABLE_SIZE>::uniqueOperation::uniqueOperation(typename container<OPE_TYPE, _TABLE_SIZE>::uniqueOperation::container_type& container) :
		m_container(container),
		m_status(status_t::IDLE)
	{}

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

	//プッシュ
	//※オブジェクト渡し
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::node_type* container<OPE_TYPE, _TABLE_SIZE>::pushCopying(const typename container<OPE_TYPE, _TABLE_SIZE>::node_type&& src)//ムーブ
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		return _pushCopying(std::move(src));
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::node_type* container<OPE_TYPE, _TABLE_SIZE>::pushCopying(const typename container<OPE_TYPE, _TABLE_SIZE>::node_type& src)//コピー
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		return _pushCopying(src);
	}

	//プッシュ（本体）
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	template<typename... Tx>
	typename container<OPE_TYPE, _TABLE_SIZE>::node_type* container<OPE_TYPE, _TABLE_SIZE>::_push(const Tx&... args)
	{
		if (m_status == PUSH_BEGINNING || m_status == POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
			return nullptr;
		node_type* obj = pushBegin(args...);
		if (!obj)
			return nullptr;
		return pushEnd();
	}

	//プッシュ
	//※パラメータ渡し
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	template<typename... Tx>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::node_type* container<OPE_TYPE, _TABLE_SIZE>::push(const Tx&... args)
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		return _push(args...);
	}

	//プッシュ開始（本体）
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	template<typename... Tx>
	typename container<OPE_TYPE, _TABLE_SIZE>::node_type* container<OPE_TYPE, _TABLE_SIZE>::_pushBegin(const Tx&... args)
	{
		if (m_status == PUSH_BEGINNING || m_status == POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
			return nullptr;
		node_type* obj = refNew();
		if (!obj)
			return nullptr;
		obj = new(obj)node_type(args...);//コンストラクタ呼び出し
		if (obj)
			m_status = PUSH_BEGINNING;
		return obj;
	}

	//プッシュ開始
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	template<typename... Tx>
	typename container<OPE_TYPE, _TABLE_SIZE>::node_type* container<OPE_TYPE, _TABLE_SIZE>::pushBegin(const Tx&... args)
	{
		m_lock.lock();//ロックを取得（そのまま関数を抜ける）
		node_type* obj = _pushBegin(args...);//プッシュ開始
		if (!obj)
			m_lock.unlock();//プッシュ失敗時はロック解放
		return obj;
	}

	//ポップ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline bool container<OPE_TYPE, _TABLE_SIZE>::popCopying(typename container<OPE_TYPE, _TABLE_SIZE>::node_type& dst)
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		return _popCopying(dst);
	}

	//ノードを上方に移動
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::node_type* container<OPE_TYPE, _TABLE_SIZE>::upHeap(typename container<OPE_TYPE, _TABLE_SIZE>::node_type* obj)
	{
		return binary_heap::upHeap<ope_type>(_refTop(), m_used, obj, ope_type::less());
	}

	//ノードを下方に移動
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline typename container<OPE_TYPE, _TABLE_SIZE>::node_type* container<OPE_TYPE, _TABLE_SIZE>::downHeap(typename container<OPE_TYPE, _TABLE_SIZE>::node_type* obj)
	{
		return binary_heap::downHeap<ope_type>(_refTop(), m_used, obj, ope_type::less());
	}

	//デフォルトコンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	inline container<OPE_TYPE, _TABLE_SIZE>::container() :
		m_used(0),
		m_status(IDLE)
	{}

}//namespace binary_heap

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//__BINARY_HEAP_INL_

// End of file

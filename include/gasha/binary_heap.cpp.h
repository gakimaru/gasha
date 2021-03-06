﻿#pragma once
#ifndef GASHA_INCLUDED_BINARY_HEAP_CPP_H
#define GASHA_INCLUDED_BINARY_HEAP_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// binary_heap.cpp.h
// 二分ヒープコンテナ【関数／実体定義部】
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

#include <gasha/binary_heap.inl>//二分ヒープコンテナ【インライン関数／テンプレート関数定義部】

#include <utility>//C++11 std::move

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace binary_heap
{
	//----------------------------------------
	//イテレータのメソッド
	
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::update(const index_type index) const
	{
		//if (index == INVALID_INDEX || index < 0 || index > static_cast<index_type>(m_con->m_used))
		if (index > static_cast<index_type>(m_con->m_used))
		{
			m_index = INVALID_INDEX;
			m_value = nullptr;
		}
		else
		{
			m_index = index;
			m_value = const_cast<value_type*>(m_con->_refNode(m_index));
		}
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::iterator::addIndexAndUpdate(const typename container<OPE_TYPE>::difference_type add) const
	{
		update(static_cast<index_type>(static_cast<difference_type>(m_index) + add));
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
	//コピーオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const iterator& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
	//コピーコンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR
	//コンストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::iterator::iterator(const container& con, const bool is_end) :
		m_con(&con),
		m_index(INVALID_INDEX),
		m_value(nullptr)
	{
		if (!is_end)
			update(0);//先頭データ
		else
			update(m_con->m_used);//末尾データ
	}

#ifdef GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR//std::input_iterator_tag には本来必要ではない
	//----------------------------------------
	//リバースイテレータのメソッド

	//メソッド
	//参照を更新
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::update(const index_type index) const
	{
		//if (index == INVALID_INDEX || index < 0 || index > static_cast<index_type>(m_con->m_used))
		if (index > static_cast<index_type>(m_con->m_used))
		{
			m_index = INVALID_INDEX;
			m_value = nullptr;
		}
		else
		{
			m_index = index;
			m_value = const_cast<value_type*>(m_con->_refNode(m_index)) - 1;
		}
	}
	template<class OPE_TYPE>
	void container<OPE_TYPE>::reverse_iterator::addIndexAndUpdate(const typename container<OPE_TYPE>::difference_type add) const
	{
		update(static_cast<index_type>(static_cast<difference_type>(m_index) - add));
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(typename container<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator=(typename container<OPE_TYPE>::iterator&& rhs)
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
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(typename container<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
	template<class OPE_TYPE>
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(typename container<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
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
	container<OPE_TYPE>::reverse_iterator::reverse_iterator(const container& con, const bool is_end) :
		m_con(&con),
		m_index(INVALID_INDEX),
		m_value(nullptr)
	{
		if (!is_end)
			update(m_con->m_used);//末尾データ
		else
			update(0);//先頭データ
	}
#endif//GASHA_BINARY_HEAP_ENABLE_REVERSE_ITERATOR

	//--------------------
	//単一操作オブジェクト（安全なプッシュ／ポップ操作クラス）
	
	//プッシュ終了
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::uniqueOperation::node_type* container<OPE_TYPE>::uniqueOperation::pushEnd()
	{
		if (m_status != status_t::PUSH_BEGINNING)//プッシュ開始中以外なら処理しない
			return nullptr;
		node_type* node = m_container.pushEnd();//プッシュ終了
		m_status = status_t::PUSH_ENDED;//ステータス変更
		return node;
	}

	//プッシュ取り消し
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::uniqueOperation::pushCancel()
	{
		if (m_status != status_t::PUSH_BEGINNING)//プッシュ開始中以外なら処理しない
			return false;
		m_container.pushCancel();//プッシュ取り消し
		m_status = status_t::PUSH_CANCELLED;//ステータス変更
		return true;
	}

	//ポップ開始
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::uniqueOperation::node_type* container<OPE_TYPE>::uniqueOperation::popBegin()
	{
		if (m_status == status_t::PUSH_BEGINNING || m_status == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
			return nullptr;
		node_type* node = m_container.popBegin();//ポップ開始
		if (node)
			m_status = status_t::POP_BEGINNING;//ステータス変更
		return node;
	}

	//ポップ終了
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::uniqueOperation::popEnd()
	{
		if (m_status != status_t::POP_BEGINNING)//ポップ開始中以外なら処理しない
			return false;
		const bool result = m_container.popEnd();//ポップ終了
		m_status = status_t::POP_ENDED;//ステータス変更
		return result;
	}

	//ポップ取り消し
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::uniqueOperation::popCancel()
	{
		if (m_status != status_t::POP_BEGINNING)//ポップ開始中以外なら処理しない
			return false;
		m_container.popCancel();//ポップ取り消し
		m_status = status_t::POP_CANCELLED;//ステータス変更
		return true;
	}

	//デストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::uniqueOperation::~uniqueOperation()
	{
		pushEnd();//プッシュ終了
		popEnd();//ポップ終了
	}

	//----------------------------------------
	//コンテナ本体のメソッド

	//最大の深さを取得
	template<class OPE_TYPE>
	int container<OPE_TYPE>::maxDepth() const
	{
		if (m_used == 0)
			return -1;
		int depth = 0;
		std::size_t used = m_used >> 1;
		while (used != 0)
		{
			++depth;
			used >>= 1;
		}
		return depth;
	}
		
	//プッシュ（本体）：ムーブ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::_pushCopying(typename container<OPE_TYPE>::node_type&& src)
	{
		if (m_status == PUSH_BEGINNING || m_status == POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
			return nullptr;
		node_type* obj = refNew();
		if (!obj)
			return nullptr;
		*obj = std::move(src);
		m_status = PUSH_BEGINNING;
		return _pushEnd();
	}
	
	//プッシュ（本体）：コピー
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::_pushCopying(const typename container<OPE_TYPE>::node_type& src)
	{
		if (m_status == PUSH_BEGINNING || m_status == POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
			return nullptr;
		node_type* obj = refNew();
		if (!obj)
			return nullptr;
		*obj = src;
		m_status = PUSH_BEGINNING;
		return _pushEnd();
	}
		
	//プッシュ終了（本体）
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::_pushEnd()
	{
		if (m_status != PUSH_BEGINNING)//プッシュ開始中以外なら処理しない
			return nullptr;
		node_type* obj = refNew();
		if (!obj)
			return nullptr;
		++m_used;
		m_status = PUSH_ENDED;
		//末端の葉ノードとして登録された新規ノードを上方に移動
		return upHeap(obj);
	}
		
	//プッシュ終了
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::pushEnd()
	{
		const bool unlock = (m_status == PUSH_BEGINNING);//プッシュ開始中ならアンロックする
		node_type* new_obj = _pushEnd();//プッシュ終了
		if (unlock)
			m_lock.unlock();//ロック解放
		return new_obj;
	}
		
	//プッシュ取り消し（本体）
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::_pushCancel()
	{
		if (m_status != PUSH_BEGINNING)//プッシュ開始中以外なら処理しない
			return false;
		m_status = PUSH_CANCELLED;
		return true;
	}
		
	//プッシュ取り消し
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::pushCancel()
	{
		const bool unlock = (m_status == PUSH_BEGINNING);//プッシュ開始中ならアンロックする
		const bool result = _pushCancel();//プッシュ取り消し
		if (unlock)
			m_lock.unlock();//ロック解放
		return result;
	}
	
	//ポップ（本体）
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::_popCopying(typename container<OPE_TYPE>::node_type& dst)
	{
		if (m_status == PUSH_BEGINNING || m_status == POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
			return false;
		const node_type* obj = _popBegin();
		if (!obj)
			return false;
		dst = *obj;
		return _popEnd();
	}
	
	//ポップ開始（本体）
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::_popBegin()
	{
		if (m_status == PUSH_BEGINNING || m_status == POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
			return nullptr;
		node_type* obj = refTop();
		if (obj)
			m_status = POP_BEGINNING;
		return obj;
	}
	
	//ポップ開始
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::popBegin()
	{
		m_lock.lock();//ロックを取得（そのまま関数を抜ける）
		node_type* obj = _popBegin();//ポップ開始
		if (!obj)
			m_lock.unlock();//プッシュ失敗時はロック解放
		return obj;
	}
	
	//ポップ終了（本体）
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::_popEnd()
	{
		if (m_status != POP_BEGINNING)//ポップ開始中以外なら処理しない
			return false;
		node_type* obj = refBottom();
		if (!obj)
			return false;
		ope_type::callDestructor(obj);//デストラクタ呼び出し
		m_status = POP_ENDED;
		//根ノードがポップされたので、末端の葉ノードを根ノードに上書きした上で、それを下方に移動
		node_type* top_obj = _refTop();
		*top_obj = std::move(*obj);
		--m_used;
		downHeap(top_obj);
		return true;
	}
	
	//ポップ終了
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::popEnd()
	{
		const bool unlock = (m_status == POP_BEGINNING);//ポップ開始中ならアンロックする
		const bool result = _popEnd();//ポップ終了
		if (unlock)
			m_lock.unlock();//ロック解放
		return result;
	}
	
	//ポップ取り消し（本体）
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::_popCancel()
	{
		if (m_status != POP_BEGINNING)//ポップ開始中以外なら処理しない
			return false;
		m_status = POP_CANCELLED;
		return true;
	}
	
	//ポップ取り消し
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::popCancel()
	{
		const bool unlock = (m_status == POP_BEGINNING);//ポップ開始中ならアンロックする
		const bool result = _popCancel();//ポップ取り消し
		if (unlock)
			m_lock.unlock();//ロック解放
		return result;
	}
	
	//クリア（本体）
	template<class OPE_TYPE>
	void container<OPE_TYPE>::_clear()
	{
		if (m_used == 0)
			return;
		node_type* obj_end = _refTop() + m_used;
		for (node_type* obj = _refTop(); obj < obj_end; ++obj)
		{
			ope_type::callDestructor(obj);//デストラクタ呼び出し
		}
		m_used = 0;
	}
	
	//クリア
	//※処理中、ロックを取得する
	template<class OPE_TYPE>
	inline void container<OPE_TYPE>::clear()
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		_clear();
	}

	//デストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::~container()
	{
		pushCancel();//プッシュ取り消し
		popCancel();//ポップ取り消し
	}

}//namespace binary_heap

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//二分ヒープコンテナの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_bHeap(OPE_TYPE) \
	template class GASHA_ binary_heap::container<OPE_TYPE>;

//シンプル二分ヒープコンテナの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_simpleBHeap(NODE_TYPE, _TABLE_SIZE) \
	template class GASHA_ binary_heap::simpleContainer<NODE_TYPE, _TABLE_SIZE>; \
	template class GASHA_ binary_heap::container<typename GASHA_ priority_queue::simpleContainer<NODE_TYPE, _TABLE_SIZE>::ope>;

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

#endif//GASHA_INCLUDED_BINARY_HEAP_CPP_H

// End of file

﻿#pragma once
#ifndef GASHA_INCLUDED_PRIORITY_QUEUE_INL
#define GASHA_INCLUDED_PRIORITY_QUEUE_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// priority_queue.inl
// 優先度付きキューコンテナ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/priority_queue.h>//優先度付きキューコンテナアダプタ【宣言部】

#include <gasha/binary_heap.inl>//二分ヒープコンテナ【インライン関数／テンプレート定義部】

#include <utility>//C++11 std::move, std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace priority_queue
{
	//--------------------
	//単一操作オブジェクト（安全なエンキュー／デキュー操作クラス）

	//エンキュー開始
	template<class OPE_TYPE>
	template<typename... Tx>
	typename container<OPE_TYPE>::uniqueOperation::node_type* container<OPE_TYPE>::uniqueOperation::enqueueBegin(const typename container<OPE_TYPE>::uniqueOperation::priority_type priority, Tx&&... args)
	{
		if (m_status == status_t::PUSH_BEGINNING || m_status == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
			return nullptr;
		node_type* node = m_container.enqueueBegin(priority, std::forward<Tx>(args)...);//エンキュー開始
		if (node)
			m_status = status_t::PUSH_BEGINNING;//ステータス変更
		return node;
	}

	//ムーブコンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::uniqueOperation::uniqueOperation(typename container<OPE_TYPE>::uniqueOperation&& obj) :
		m_container(obj.m_container),
		m_status(obj.m_status)
	{
		obj.m_status = status_t::IDLE;
	}

	//コンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::uniqueOperation::uniqueOperation(typename container<OPE_TYPE>::uniqueOperation::container_adapter_type& container_adapter) :
		m_container(container_adapter),
		m_status(status_t::IDLE)
	{}

	//----------------------------------------
	//コンテナ本体のメソッド

	//シーケンス番号発行
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::seq_no_type container<OPE_TYPE>::getNextSeqNo()
	{
		return ope_type::countupSeqNo(m_seqNo);
	}

	//エンキュー
	//※オブジェクト渡し
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::enqueueCopying(typename container<OPE_TYPE>::node_type&& obj)//ムーブ
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		return _enqueueCopying(std::move(obj));
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::enqueueCopying(const typename container<OPE_TYPE>::node_type& obj)//コピー
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		return _enqueueCopying(obj);
	}

	//エンキュー（本体）
	template<class OPE_TYPE>
	template<typename... Tx>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::_enqueue(const typename container<OPE_TYPE>::priority_type priority, Tx&&... args)
	{
		//if (m_container.status() == status_t::PUSH_BEGINNING || m_container.status() == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
		//	return nullptr;
		node_type* obj = m_container.pushBegin(std::forward<Tx>(args)...);//コンテナにプッシュ開始
		if (!obj)
			return nullptr;
		ope_type::setPriority(*obj, priority);//優先度を設定
		ope_type::setSeqNo(*obj, getNextSeqNo());//シーケンス番号をセット
		obj = m_container.pushEnd();//コンテナにプッシュ終了
		return obj;
	}

	//エンキュー
	//※パラメータ渡し
	template<class OPE_TYPE>
	template<typename... Tx>
	inline typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::enqueue(const typename container<OPE_TYPE>::priority_type priority, Tx&&... args)
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		return _enqueue(priority, std::forward<Tx>(args)...);
	}

	//エンキュー開始（本体）
	template<class OPE_TYPE>
	template<typename... Tx>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::_enqueueBegin(const typename container<OPE_TYPE>::priority_type priority, Tx&&... args)
	{
		//if (m_container.status() == status_t::PUSH_BEGINNING || m_container.status() == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
		//	return nullptr;
		node_type* obj = m_container.pushBegin(std::forward<Tx>(args)...);//コンテナにプッシュ開始
		if (!obj)
			return nullptr;
		ope_type::setPriority(*obj, priority);//優先度を設定
		ope_type::setSeqNo(*obj, getNextSeqNo());//シーケンス番号を設定
		return obj;
	}

	//エンキュー開始
	template<class OPE_TYPE>
	template<typename... Tx>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::enqueueBegin(const typename container<OPE_TYPE>::priority_type priority, Tx&&... args)
	{
		m_lock.lock();//ロックを取得（そのまま関数を抜ける）
		node_type* obj = _enqueueBegin(priority, std::forward<Tx>(args)...);//エンキュー開始
		if (!obj)
			m_lock.unlock();//プッシュ失敗時はロック解放
		return obj;
	}

	//デキュー
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::dequeueCopying(typename container<OPE_TYPE>::node_type& dst)
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		return _dequeueCopying(dst);
	}

	//先頭（根）キューのプライオリティ変更
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::changePriorityOnTop(const typename container<OPE_TYPE>::priority_type priority)
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		return _changePriorityOnTop(priority);
	}
	//※ロックなし版（top()参照～プライオリティ変更までを任意にロックするならこちらを使用する）
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::changePriorityOnTopWithoutLock(const typename container<OPE_TYPE>::priority_type priority)
	{
		return _changePriorityOnTop(priority);
	}

	//クリア
	//※処理中、ロックを取得する
	template<class OPE_TYPE>
	inline void container<OPE_TYPE>::clear()
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		_clear();
	}

	//デフォルトコンストラクタ
	template<class OPE_TYPE>
	inline container<OPE_TYPE>::container() :
		m_container(),
		m_lock()
	{
		ope_type::resetSeqNo(m_seqNo);
	}

	//----------------------------------------
	//シンプル優先度付きキューコンテナ

	//ムーブオペレータ
	template<typename VALUE_TYPE, std::size_t _TABLE_SIZE>
	inline typename simpleContainer<VALUE_TYPE, _TABLE_SIZE>::node& simpleContainer<VALUE_TYPE, _TABLE_SIZE>::node::operator=(typename simpleContainer<VALUE_TYPE, _TABLE_SIZE>::core_value_type&& value)
	{
		m_value = std::move(value);
		return *this;
	}

	//コピーオペレータ
	template<typename VALUE_TYPE, std::size_t _TABLE_SIZE>
	inline typename simpleContainer<VALUE_TYPE, _TABLE_SIZE>::node& simpleContainer<VALUE_TYPE, _TABLE_SIZE>::node::operator=(const typename simpleContainer<VALUE_TYPE, _TABLE_SIZE>::core_value_type& value)
	{
		m_value = value;
		return *this;
	}

	//ムーブコンストラクタ
	template<typename VALUE_TYPE, std::size_t _TABLE_SIZE>
	inline simpleContainer<VALUE_TYPE, _TABLE_SIZE>::node::node(const typename simpleContainer<VALUE_TYPE, _TABLE_SIZE>::core_priority_type priority, typename simpleContainer<VALUE_TYPE, _TABLE_SIZE>::core_value_type&& value) :
		m_value(std::move(value)),
		m_priority(priority),
		m_seqNo(0)
	{}

	//コピーコンストラクタ
	template<typename VALUE_TYPE, std::size_t _TABLE_SIZE>
	inline simpleContainer<VALUE_TYPE, _TABLE_SIZE>::node::node(const typename simpleContainer<VALUE_TYPE, _TABLE_SIZE>::core_priority_type priority, const typename simpleContainer<VALUE_TYPE, _TABLE_SIZE>::core_value_type& value) :
		m_value(value),
		m_priority(priority),
		m_seqNo(0)
	{}

	//コンストラクタ
	template<typename VALUE_TYPE, std::size_t _TABLE_SIZE>
	template<typename... Tx>
	inline simpleContainer<VALUE_TYPE, _TABLE_SIZE>::node::node(const core_priority_type priority, Tx&&... args) :
		m_value(std::forward<Tx>(args)...),
		m_priority(priority),
		m_seqNo(0)
	{}

	//デフォルトコンストラクタ
	template<typename VALUE_TYPE, std::size_t _TABLE_SIZE>
	inline simpleContainer<VALUE_TYPE, _TABLE_SIZE>::node::node() :
		m_value(),
		m_priority(0),
		m_seqNo(0)
	{}

	//デストラクタ
	template<typename VALUE_TYPE, std::size_t _TABLE_SIZE>
	inline simpleContainer<VALUE_TYPE, _TABLE_SIZE>::node::~node()
	{}

}//namespace priority_queue

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_PRIORITY_QUEUE_INL

// End of file

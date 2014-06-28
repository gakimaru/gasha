#pragma once
#ifndef __PRIORITY_QUEUE_INL_
#define __PRIORITY_QUEUE_INL_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// priority_queue.inl
// 優先度付きキューコンテナ【インライン関数／テンプレート関数定義部】
//
// ※コンテナクラスの操作が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/priority_queue.h>//優先度付きキューコンテナアダプタ【宣言部】

#include <gasha/binary_heap.inl>//二分ヒープコンテナ【インライン関数／テンプレート定義部】

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace priority_queue
{
	//--------------------
	//イテレータのインライン関数
	
#if 0
	//ムーブオペレータ
	template<class OPE_TYPE>
	inline typename container_adapter<OPE_TYPE>::iterator& container_adapter<OPE_TYPE>::iterator::operator=(const typename container_adapter<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	inline typename container_adapter<OPE_TYPE>::iterator& container_adapter<OPE_TYPE>::iterator::operator=(const typename container_adapter<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	inline typename container_adapter<OPE_TYPE>::iterator& container_adapter<OPE_TYPE>::iterator::operator=(const typename container_adapter<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	inline typename container_adapter<OPE_TYPE>::iterator& container_adapter<OPE_TYPE>::iterator::operator=(const typename container_adapter<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	inline container_adapter<OPE_TYPE>::iterator::iterator(const typename container_adapter<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	inline container_adapter<OPE_TYPE>::iterator::iterator(const typename container_adapter<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	inline container_adapter<OPE_TYPE>::iterator::iterator(const typename container_adapter<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	inline container_adapter<OPE_TYPE>::iterator::iterator(const typename container_adapter<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
	}
	//コンストラクタ
	template<class OPE_TYPE>
	inline container_adapter<OPE_TYPE>::iterator::iterator(const container_adapter<OPE_TYPE>& con, const bool is_end) :
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
	inline container_adapter<OPE_TYPE>::iterator::iterator(const container_adapter<OPE_TYPE>& con, const typename container_adapter<OPE_TYPE>::index_type index) :
		m_con(&con),
		m_index(INVALID_INDEX),
		m_value(nullptr)
	{
		update(index);
	}
#endif
	
	//--------------------
	//リバースイテレータのインライン関数
	
#if 0
	//ムーブオペレータ
	template<class OPE_TYPE>
	inline typename container_adapter<OPE_TYPE>::reverse_iterator& container_adapter<OPE_TYPE>::reverse_iterator::operator=(const typename container_adapter<OPE_TYPE>::reverse_iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
	//ムーブオペレータ
	template<class OPE_TYPE>
	inline typename container_adapter<OPE_TYPE>::reverse_iterator& container_adapter<OPE_TYPE>::reverse_iterator::operator=(const typename container_adapter<OPE_TYPE>::iterator&& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	inline typename container_adapter<OPE_TYPE>::reverse_iterator& container_adapter<OPE_TYPE>::reverse_iterator::operator=(const typename container_adapter<OPE_TYPE>::reverse_iterator& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		m_value = rhs.m_value;
		return *this;
	}
	//コピーオペレータ
	template<class OPE_TYPE>
	inline typename container_adapter<OPE_TYPE>::reverse_iterator& container_adapter<OPE_TYPE>::reverse_iterator::operator=(const typename container_adapter<OPE_TYPE>::iterator& rhs)
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	inline container_adapter<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container_adapter<OPE_TYPE>::reverse_iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
	//ムーブコンストラクタ
	template<class OPE_TYPE>
	inline container_adapter<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container_adapter<OPE_TYPE>::iterator&& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(obj.m_index);
	}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	inline container_adapter<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container_adapter<OPE_TYPE>::reverse_iterator& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(obj.m_value)
	{}
	//コピーコンストラクタ
	template<class OPE_TYPE>
	inline container_adapter<OPE_TYPE>::reverse_iterator::reverse_iterator(const typename container_adapter<OPE_TYPE>::iterator& obj) :
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
	}
	//コンストラクタ
	template<class OPE_TYPE>
	inline container_adapter<OPE_TYPE>::reverse_iterator::reverse_iterator(const container_adapter<OPE_TYPE>& con, const bool is_end) :
		m_con(&con),
		m_index(INVALID_INDEX),
		m_value(nullptr)
	{
		if (!is_end)
			update(m_con->m_size);//末尾データ
		else
			update(0);//先頭データ
	}
	//コンストラクタ
	template<class OPE_TYPE>
	inline container_adapter<OPE_TYPE>::reverse_iterator::reverse_iterator(const  container_adapter<OPE_TYPE>& con, const typename  container_adapter<OPE_TYPE>::index_type index):
		m_con(&con),
		m_index(INVALID_INDEX),
		m_value(nullptr)
	{
		update(index);
	}
#endif

	//----------------------------------------
	//コンテナ本体のメソッド

	//シーケンス番号発行
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, class CONTAINER_TYPE>
	inline typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::seq_type container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::getNextSeqNo()
	{
		return ope_type::countupSeqNo(m_seqNo);
	}

	//エンキュー
	//※オブジェクト渡し
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, class CONTAINER_TYPE>
	inline typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::node_type* container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::enqueueCopying(const typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::node_type&& obj)//ムーブ
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		return _enqueueCopying(std::move(obj));
	}
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, class CONTAINER_TYPE>
	inline typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::node_type* container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::enqueueCopying(const typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::node_type& obj)//コピー
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		return _enqueueCopying(obj);
	}

	//エンキュー（本体）
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, class CONTAINER_TYPE>
	template<typename... Tx>
	typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::node_type* container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::_enqueue(const typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::priority_type priority, Tx... args)
	{
		//if (m_container.status() == status_t::PUSH_BEGINNING || m_container.status() == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
		//	return nullptr;
		node_type* obj = m_container.pushBegin(args...);//コンテナにプッシュ開始
		if (!obj)
			return nullptr;
		ope_type::setPriority(*obj, priority);//優先度を設定
		ope_type::setSeqNo(*obj, getNextSeqNo());//シーケンス番号をセット
		obj = m_container.pushEnd();//コンテナにプッシュ終了
		return obj;
	}

	//エンキュー
	//※パラメータ渡し
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, class CONTAINER_TYPE>
	template<typename... Tx>
	inline typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::node_type* container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::enqueue(const typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::priority_type priority, Tx... args)
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		return _enqueue(priority, args...);
	}

	//エンキュー開始（本体）
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, class CONTAINER_TYPE>
	template<typename... Tx>
	typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::node_type* container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::_enqueueBegin(const typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::priority_type priority, Tx... args)
	{
		//if (m_container.status() == status_t::PUSH_BEGINNING || m_container.status() == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
		//	return nullptr;
		node_type* obj = m_container.pushBegin(args...);//コンテナにプッシュ開始
		if (!obj)
			return nullptr;
		ope_type::setPriority(*obj, priority);//優先度を設定
		ope_type::setSeqNo(*obj, getNextSeqNo());//シーケンス番号を設定
		return obj;
	}

	//エンキュー開始
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, class CONTAINER_TYPE>
	template<typename... Tx>
	typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::node_type* container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::enqueueBegin(const typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::priority_type priority, Tx... args)
	{
		m_lock.lock();//ロックを取得（そのまま関数を抜ける）
		node_type* obj = _enqueueBegin(args...);//エンキュー開始
		if (!obj)
			m_lock.unlock();//プッシュ失敗時はロック解放
		return obj;
	}

	//デキュー
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, class CONTAINER_TYPE>
	inline bool container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::dequeueCopying(typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::node_type& dst)
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		return _dequeueCopying(dst);
	}

	//先頭（根）キューのプライオリティ変更
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, class CONTAINER_TYPE>
	inline typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::node_type* container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::changePriorityOnTop(const typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::priority_type priority)
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		return _changePriorityOnTop(priority);
	}
	//※ロックなし版（top()参照～プライオリティ変更までを任意にロックするならこちらを使用する）
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, class CONTAINER_TYPE>
	inline typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::node_type* container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::changePriorityOnTopWithoutLock(const typename container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::priority_type priority)
	{
		return _changePriorityOnTop(priority);
	}

	//クリア
	//※処理中、ロックを取得する
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, class CONTAINER_TYPE>
	inline void container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::clear()
	{
		lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
		_clear();
	}

	//デフォルトコンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, class CONTAINER_TYPE>
	inline container_adapter<OPE_TYPE, _TABLE_SIZE, CONTAINER_TYPE>::container_adapter() :
		m_container(),
		m_lock()
	{
		ope_type::resetSeqNo(m_seqNo);
	}

	//--------------------
	//安全なエンキュー／デキュー操作クラス

	//エンキュー開始
	template<class CON>
	template<typename... Tx>
	typename operation_guard<CON>::node_type* operation_guard<CON>::enqueueBegin(const typename operation_guard<CON>::priority_type priority, Tx... args)
	{
		if (m_status == status_t::PUSH_BEGINNING || m_status == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
			return nullptr;
		node_type* node = m_containerAdapter.enqueueBegin(priority, args...);//エンキュー開始
		if (node)
			m_status = status_t::PUSH_BEGINNING;//ステータス変更
		return node;
	}

	//コンストラクタ
	template<class CON>
	inline operation_guard<CON>::operation_guard(typename operation_guard<CON>::container_adapter_type& container_adapter) :
		m_containerAdapter(container_adapter),
		m_status(status_t::IDLE)
	{}

}//namespace priority_queue

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__PRIORITY_INL_

// End of file

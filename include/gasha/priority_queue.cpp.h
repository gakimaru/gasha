﻿#pragma once
#ifndef GASHA_INCLUDED_PRIORITY_QUEUE_CPP_H
#define GASHA_INCLUDED_PRIORITY_QUEUE_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// priority_queue.cpp.h
// 優先度付きキューコンテナ【関数／実体定義部】
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

#include <gasha/priority_queue.inl>//優先度付きキューコンテナアダプタ【インライン関数／テンプレート関数定義部】

#include <gasha/binary_heap.cpp.h>//二分ヒープコンテナ【関数／実体定義部】

#include <utility>//C++11 std::move

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace priority_queue
{
	//--------------------
	//単一操作オブジェクト（安全なエンキュー／デキュー操作クラス）
	
	//エンキュー終了
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::uniqueOperation::node_type* container<OPE_TYPE>::uniqueOperation::enqueueEnd()
	{
		if (m_status != status_t::PUSH_BEGINNING)//プッシュ開始中以外なら処理しない
			return nullptr;
		node_type* node = m_container.enqueueEnd();//エンキュー終了
		m_status = status_t::PUSH_ENDED;//ステータス変更
		return node;
	}

	//エンキュー取り消し
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::uniqueOperation::enqueueCancel()
	{
		if (m_status != status_t::PUSH_BEGINNING)//プッシュ開始中以外なら処理しない
			return nullptr;
		m_container.enqueueCancel();//エンキュー取り消し
		m_status = status_t::PUSH_CANCELLED;//ステータス変更
		return true;
	}

	//デキュー開始
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::uniqueOperation::node_type* container<OPE_TYPE>::uniqueOperation::dequeueBegin()
	{
		if (m_status == status_t::PUSH_BEGINNING || m_status == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
			return nullptr;
		node_type* node = m_container.dequeueBegin();//デキュー開始
		if (node)
			m_status = status_t::POP_BEGINNING;//ステータス変更
		return node;
	}

	//デキュー終了
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::uniqueOperation::dequeueEnd()
	{
		if (m_status != status_t::POP_BEGINNING)//ポップ開始中以外なら処理しない
			return false;
		const bool result = m_container.dequeueEnd();//デキュー終了
		m_status = status_t::POP_ENDED;//ステータス変更
		return result;
	}

	//デキュー取り消し
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::uniqueOperation::dequeueCancel()
	{
		if (m_status != status_t::POP_BEGINNING)//ポップ開始中以外なら処理しない
			return false;
		m_container.dequeueCancel();//デキュー取り消し
		m_status = status_t::POP_CANCELLED;//ステータス変更
		return true;
	}

	//デストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::uniqueOperation::~uniqueOperation()
	{
		enqueueEnd();//エンキュー終了
		dequeueEnd();//デキュー終了
	}

	//----------------------------------------
	//コンテナ本体のメソッド
	
	//可能ならシーケンス番号をリセット
	template<class OPE_TYPE>
	void container<OPE_TYPE>::checkAndResetSeqNo()
	{
		if (m_container.empty())
			ope_type::resetSeqNo(m_seqNo);
	}
	
	//エンキュー（本体）：ムーブ
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::_enqueueCopying(typename container<OPE_TYPE>::node_type&& obj)
	{
		if (m_container.status() == status_t::PUSH_BEGINNING || m_container.status() == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
			return nullptr;
		node_type obj_tmp(std::move(obj));//一時オブジェクトにムーブ
		ope_type::setSeqNo(obj_tmp, getNextSeqNo());//シーケンス番号をセット
		return m_container.pushCopying(std::move(obj_tmp));//コンテナにプッシュ（ムーブ）
	}

	//エンキュー（本体）：コピー
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::_enqueueCopying(const typename container<OPE_TYPE>::node_type& obj)
	{
		if (m_container.status() == status_t::PUSH_BEGINNING || m_container.status() == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
			return nullptr;
		node_type obj_tmp(obj);//一時オブジェクトにコピー
		ope_type::setSeqNo(obj_tmp, getNextSeqNo());//シーケンス番号をセット
		return m_container.pushCopying(std::move(obj_tmp));//コンテナにプッシュ（ムーブ）
	}
	
	//エンキュー終了（本体）
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::_enqueueEnd()
	{
		//if (m_container.status() != status_t::PUSH_BEGINNING)//プッシュ開始中以外なら処理しない
		//	return;
		return m_container.pushEnd();//コンテナにプッシュ終了
	}
	
	//エンキュー終了
	//※enqueueBeginで取得したロックを解放する
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::enqueueEnd()
	{
		const bool unlock = (m_container.status() == status_t::PUSH_BEGINNING);//プッシュ開始中ならアンロックする
		node_type* new_obj = _enqueueEnd();//エンキュー終了
		if (unlock)
			m_lock.unlock();//ロック解放
		return new_obj;
	}
	
	//エンキュー取り消し（本体）
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::_enqueueCancel()
	{
		//if (m_container.status() != status_t::PUSH_BEGINNING)//プッシュ開始中以外なら処理しない
		//	return;
		return m_container.pushCancel();//プッシュ取り消し
	}
	
	//エンキュー取り消し
	//※enqueueBeginで取得したロックを解放する
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::enqueueCancel()
	{
		const bool unlock = (m_container.status() == status_t::PUSH_BEGINNING);//プッシュ開始中ならアンロックする
		const bool result = m_container.pushCancel();//プッシュ取り消し
		if (unlock)
			m_lock.unlock();//ロック解放
		return result;
	}
	
	//デキュー（本体）
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::_dequeueCopying(typename  container<OPE_TYPE>::node_type& dst)
	{
		//if (m_container.status() == status_t::PUSH_BEGINNING || m_container.status() == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
		//	return nullptr;
		const bool result = m_container.popCopying(dst);//コンテナからポップ
		if (!result)
			return false;
		checkAndResetSeqNo();//キューが空になったらシーケンス番号をリセットする
		return true;
	}
	
	//デキュー開始（本体）
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::_dequeueBegin()
	{
		//if (m_container.status() == status_t::PUSH_BEGINNING || m_container.status() == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
		//	return nullptr;
		node_type* obj = m_container.popBegin();//コンテナからポップ開始
		if (!obj)
			return nullptr;
		return obj;
	}
	
	//デキュー開始
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::dequeueBegin()
	{
		m_lock.lock();//ロックを取得（そのまま関数を抜ける）
		node_type* obj = _dequeueBegin();//デキュー開始
		if (!obj)
			m_lock.unlock();//プッシュ失敗時はロック解放
		return obj;
	}

	//デキュー終了（本体）
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::_dequeueEnd()
	{
		//if (m_container.status() != status_t::POP_BEGINNING)//ポップ開始中以外なら処理しない
		//	return false;
		const bool result = m_container.popEnd();//コンテナからポップ終了
		checkAndResetSeqNo();//キューが空になったらシーケンス番号をリセットする
		return result;
	}
	
	//デキュー終了
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::dequeueEnd()
	{
		const bool unlock = (m_container.status() == status_t::POP_BEGINNING);//ポップ開始中ならアンロックする
		const bool result = _dequeueEnd();//デキュー終了
		if (unlock)
			m_lock.unlock();//ロック解放
		return result;
	}
	
	//デキュー取り消し（本体）
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::_dequeueCancel()
	{
		//if (m_container.status() != status_t::POP_BEGINNING)//ポップ開始中以外なら処理しない
		//	return false;
		return m_container.popCancel();//ポップ取り消し
	}
	
	//デキュー取り消し
	//※dequeueBeginで取得したロックを解放する
	template<class OPE_TYPE>
	bool container<OPE_TYPE>::dequeueCancel()
	{
		const bool unlock = (m_container.status() == status_t::POP_BEGINNING);//ポップ開始中ならアンロックする
		const bool result = _dequeueCancel();//デキュー取り消し
		if (unlock)
			m_lock.unlock();//ロック解放
		return result;
	}

	//先頭（根）キューを参照
	template<class OPE_TYPE>
	const typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::top() const
	{
		return m_container.top();//コンテナの先頭（根）ノードを取得
	}
	
	//先頭（根）キューのプライオリティ変更（本体）
	template<class OPE_TYPE>
	typename container<OPE_TYPE>::node_type* container<OPE_TYPE>::_changePriorityOnTop(const typename container<OPE_TYPE>::priority_type priority)
	{
		node_type* obj = m_container.top();
		if (!obj)
			return nullptr;
		ope_type::setPriority(*obj, priority);//優先度を更新
		ope_type::setSeqNo(*obj, getNextSeqNo());//シーケンス番号を更新
		return m_container.downHeap(obj);//ダウンヒープ
	}
	
	//クリア（本体）
	template<class OPE_TYPE>
	void container<OPE_TYPE>::_clear()
	{
		m_container.clear();
		ope_type::resetSeqNo(m_seqNo);
	}

	//デストラクタ
	template<class OPE_TYPE>
	container<OPE_TYPE>::~container()
	{
		enqueueCancel();//エンキュー取り消し
		dequeueCancel();//デキュー取り消し
	}

}//namespace priority_queue

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//優先度付きキューコンテナアダプタの明示的なインスタンス化用マクロ
//※二分ヒープコンテナも同時にインスタンス化
#define GASHA_INSTANCING_pQueue_withBHeap(OPE_TYPE) \
	template class GASHA_ priority_queue::container<OPE_TYPE>; \
	template class GASHA_ binary_heap::container<typename OPE_TYPE::container_ope_type>;

//シンプル優先度付きキューコンテナアダプタの明示的なインスタンス化用マクロ
//※二分ヒープコンテナも同時にインスタンス化
#define GASHA_INSTANCING_simplePQueue(NODE_TYPE, _TABLE_SIZE) \
	template class GASHA_ priority_queue::simpleContainer<NODE_TYPE, _TABLE_SIZE>; \
	template class GASHA_ priority_queue::container<typename GASHA_ priority_queue::simpleContainer<NODE_TYPE, _TABLE_SIZE>::ope>; \
	template class GASHA_ binary_heap::container<typename GASHA_ priority_queue::simpleContainer<NODE_TYPE, _TABLE_SIZE>::ope::container_ope_type>;

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

#endif//GASHA_INCLUDED_PRIORITY_QUEUE_CPP_H

// End of file

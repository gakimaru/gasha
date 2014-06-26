#pragma once
#ifndef __SINGLY_LINKED_LIST_CPP_H_
#define __SINGLY_LINKED_LIST_CPP_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// singly_linked_list.cpp.h
// 片方向連結リストコンテナ【関実装部】
//
// ※コンテナクラスの実体化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/singly_linked_list.inl>//片方向連結リストコンテナ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace singly_linked_list
{
	//----------------------------------------
	//イテレータのメソッド
	
	//参照を更新
//	template<class OPE_TYPE>
//	void container<OPE_TYPE>::iterator::update(const typename container<OPE_TYPE>::index_type index) const
//	{
//	}

	//----------------------------------------
	//リバースイテレータのメソッド
	
	//参照を更新
//	template<class OPE_TYPE>
//	void container<OPE_TYPE>::reverse_iterator::update(const typename container<OPE_TYPE>::index_type index) const
//	{
//	}

	//----------------------------------------
	//コンテナ本体のメソッド

	//配列の再割り当て
	//※ポインタと配列要素数指定版
//	template<class OPE_TYPE>
//	void container<OPE_TYPE>::assignArray(value_type* array, const typename container<OPE_TYPE>::size_type max_size, const int size)
//	{
//	}
	
}//namespace singly_linked_list

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__SINGLY_LINKED_LIST_CPP_H_

// End of file

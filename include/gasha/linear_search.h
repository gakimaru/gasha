#pragma once
#ifndef GASHA_INCLUDED_LINERAR_SEARCH_H
#define GASHA_INCLUDED_LINERAR_SEARCH_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// linear_search.h
// 線形探索【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/search_basic.h>//探索処理基本

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//探索アルゴリズムの説明
//========================================

//・計算時間：
//    - O(n)       ... データ件数分の時間
//    - O(n ^ 2)   ... データ件数の２乗分の時間
//    - O(log n)   ... log2(データ件数)分の時間（4→2, 16→4, 1024→10,1048576→20）
//    - O(n log n) ... n×log n 分の時間

//----------------------------------------
//アルゴリズム：線形探索
//----------------------------------------
//・最良計算時間：O(1)
//・平均計算時間：O(n/2)
//・最悪計算時間：O(n)
//・探索失敗時：  O(n)
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value)//value == 探索値ならtrueを返す
template<class T, class PREDICATE>
T* linearSearch(T* array, const std::size_t size, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_SEARCH_WITH_PREDICATE(linearSearch);

//----------------------------------------
//アルゴリズム：線形探索
//※イテレータ対応版
//----------------------------------------
//・最良計算時間：O(1)
//・平均計算時間：O(n/2)
//・最悪計算時間：O(n)
//・探索失敗時：  O(n)
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const typename ITERATOR::value_type& value)//value == 探索値ならtrueを返す
template<class ITERATOR, class PREDICATE>
ITERATOR iteratorLinearSearch(ITERATOR begin, ITERATOR end, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_ITERATOR_SEARCH_WITH_PREDICATE(iteratorLinearSearch);

//----------------------------------------
//アルゴリズム：線形探索
//※片方向連結リスト対応版
//----------------------------------------
//・最良計算時間：O(1)
//・平均計算時間：O(n/2)
//・最悪計算時間：O(n)
//・探索失敗時：  O(n)
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value)//value == 探索値ならtrueを返す
//・T* GET_NEXT_FUNC(T& node)//次のノードを返す
template<class T, class GET_NEXT_FUNC, class PREDICATE>
const T* singlyLinkedListLinearSearch(const T* first, GET_NEXT_FUNC get_next_func, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_SINGLY_LINKED_LIST_SEARCH_WITH_PREDICATE(singlyLinkedListLinearSearch);

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LINERAR_SEARCH_H

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/linear_search.inl>

// End of file

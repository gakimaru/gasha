#pragma once
#ifndef GASHA_INCLUDED_BINARY_SEARCH_H
#define GASHA_INCLUDED_BINARY_SEARCH_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// binary_search.h
// 二分探索【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
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
//アルゴリズム：二分探索
//----------------------------------------
//・最良計算時間：O(1)
//・平均計算時間：O(log n)
//・最悪計算時間：O(log n)
//・探索失敗時：  O(log n)
//----------------------------------------
//プロトタイプ：
//・bool COMPARISON(const T& value)//value == 探索値なら0を、value < 探索値なら-1以下を、value > 探索値なら1以上を返す
template<class T, class COMPARISON>
T* binarySearch(T* array, const std::size_t size, COMPARISON comparison);
searchFuncSetByComparison(binarySearch);

//----------------------------------------
//アルゴリズム：二分探索
//※イテレータ対応版
//----------------------------------------
//・最良計算時間：O(1)
//・平均計算時間：O(log n)
//・最悪計算時間：O(log n)
//・探索失敗時：  O(log n)
//----------------------------------------
//プロトタイプ：
//・bool COMPARISON(const typename ITERATOR::value_type& value)//value == 探索値なら0を、value < 探索値なら-1以下を、value > 探索値なら1以上を返す
template<class ITERATOR, class COMPARISON>
ITERATOR iteratorBinarySearch(ITERATOR begin, ITERATOR end, COMPARISON comparison);
iteratorSearchFuncSetByComparison(iteratorBinarySearch);

//----------------------------------------
//アルゴリズム：二分探索
//※双方向連結リスト対応版
//----------------------------------------
//・最良計算時間：O(1)
//・平均計算時間：O(log n)
//・最悪計算時間：O(log n)
//・探索失敗時：  O(log n)
//----------------------------------------
//プロトタイプ：
//・bool COMPARISON(const T& value)//value == 探索値なら0を、value < 探索値なら-1以下を、value > 探索値なら1以上を返す
//・T* GET_NEXT_FUNC(T& node)//次のノードを返す
//・T* GET_PREV_FUNC(T& node)//前のノードを返す
template<class T, class GET_NEXT_FUNC, class GET_PREV_FUNC, class COMPARISON>
const T* linkedListBinarySearch(const T* first, GET_NEXT_FUNC get_next_func, GET_PREV_FUNC get_prev_func, COMPARISON comparison);
linkedListSearchFuncSetByComparison(linkedListBinarySearch);

//----------------------------------------
//アルゴリズム：二分探索
//※片方向連結リスト対応版
//----------------------------------------
//・最良計算時間：O(1)
//・平均計算時間：O(log n)
//・最悪計算時間：O(log n)
//・探索失敗時：  O(log n)
//----------------------------------------
//プロトタイプ：
//・bool COMPARISON(const T& value)//value == 探索値なら0を、value < 探索値なら-1以下を、value > 探索値なら1以上を返す
//・T* GET_NEXT_FUNC(T& node)//次のノードを返す
template<class T, class GET_NEXT_FUNC, class COMPARISON>
const T* singlyLinkedListBinarySearch(const T* first, GET_NEXT_FUNC get_next_func, COMPARISON comparison);
singlyLinkedListSearchFuncSetByComparison(singlyLinkedListBinarySearch);

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/binary_search.inl>

#endif//GASHA_INCLUDED_BINARY_SEARCH_H

// End of file

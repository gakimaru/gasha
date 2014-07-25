#pragma once
#ifndef GASHA_INCLUDED_SHELL_SORT_H
#define GASHA_INCLUDED_SHELL_SORT_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shell_sort.h
// シェルソート【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/sort_basic.h>//ソート処理基本

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//ソートアルゴリズムの説明
//========================================

//・計算時間：
//    - O(n)       ... データ件数分の時間
//    - O(n ^ 2)   ... データ件数の２乗分の時間
//    - O(log n)   ... log2(データ件数)分の時間（4→2, 16→4, 1024→10,1048576→20）
//    - O(n log n) ... n×log n 分の時間
//・メモリ使用量：
//    - O(1)       ... １件分のメモリが必要
//    - O(n)       ... データ件数分のメモリが必要
//    - O(log n)   ... log2(データ件数)分のメモリが必要
//・安定性：
//    - ○         ... キーが同じデータの順序性が維持されることを保証する
//                     例：{ 3-a, 5-b, 4-c, 5-d, 9-e, 3-f, 4-g, 3-h, 5-i } → { 3-a, 3-f, 3-h, 4-c, 4-g, 5-b, 5-d, 5-i, 9-e }
//    - ×         ... 例：(同上)                        

//========================================
//ソートアルゴリズム分類：挿入ソート
//========================================

//----------------------------------------
//アルゴリズム：シェルソート
//----------------------------------------
//・最良計算時間：O(n)
//・平均計算時間：O(n log^2 n) or O(n^3/2)
//・最悪計算時間：O(n log^2 n)
//・メモリ使用量：O(1)
//・安定性：　　　×
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
template<class T, class PREDICATE>
std::size_t shellSort(T* array, const std::size_t size, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_SORT(shellSort);

//----------------------------------------
//アルゴリズム：シェルソート
//※イテレータ対応版
//----------------------------------------
//・最良計算時間：O(n)
//・平均計算時間：O(n log^2 n) or O(n^3/2)
//・最悪計算時間：O(n log^2 n)
//・メモリ使用量：O(1)
//・安定性：　　　×
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const typename ITERATOR::value_type& value1, const typename ITERATOR::value_type& value2)//value1 == value2 ならtrueを返す
template<class ITERATOR, class PREDICATE>
std::size_t iteratorShellSort(ITERATOR begin, ITERATOR end, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_ITERATOR_SORT(iteratorShellSort);

//----------------------------------------
//アルゴリズム：シェルソート ※双方向連結リスト対応版
//----------------------------------------
//・最良計算時間：O(n)
//・平均計算時間：O(n log^2 n) or O(n^3/2)
//・最悪計算時間：O(n log^2 n)
//・メモリ使用量：O(1)
//・安定性：　　　×
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
//・T* GET_NEXT_FUNC(T& node)//次のノードを返す
//・T* GET_PREV_FUNC(T& node)//前のノードを返す
//・void INSERT_NODE_BEFORE_FUNC(T& new_node, T* target_node, T*& first_ref, T*& last_ref)//target_nodeの前にnew_nodeを連結する(target_nodeがnullptrだと末尾に連結)
//・void REMOVE_NODE_FUNC(T& target_node, T*& first_ref, T*& last_ref)//target_nodeを連結から解除する
template<class T, class GET_NEXT_FUNC, class GET_PREV_FUNC, class INSERT_NODE_BEFORE_FUNC, class REMOVE_NODE_FUNC, class PREDICATE>
std::size_t linkedListShellSort(T*& first, T*& last, GET_NEXT_FUNC get_next_func, GET_PREV_FUNC get_prev_func, INSERT_NODE_BEFORE_FUNC insert_node_before_func, REMOVE_NODE_FUNC remove_node_func, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_LINKED_LIST_SORT(linkedListShellSort);

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/shell_sort.inl>

#endif//GASHA_INCLUDED_SHELL_SORT_H

// End of file

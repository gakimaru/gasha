#pragma once
#ifndef GASHA_INCLUDED_QUICK_SORT_H
#define GASHA_INCLUDED_QUICK_SORT_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// quick_sort.h
// クイックソート【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
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
//ソートアルゴリズム分類：分割交換ソート
//========================================

//----------------------------------------
//アルゴリズム：クイックソート
//----------------------------------------
//・最良計算時間：O(n log n)
//・平均計算時間：O(n log n)
//・最悪計算時間：O(n^2)
//・メモリ使用量：O(log n)～O(n) ※ループ処理版は O(32*2)
//・安定性：　　　×
//----------------------------------------
//※再帰処理を使用せず、スタックを使用したループ処理にして最適化する。
//　（最大件数を log2(4294967296) = 32 とする）
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
template<class T, class PREDICATE>
inline std::size_t quickSort(T* array, const std::size_t size, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_SORT(quickSort);

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/quick_sort.inl>

#endif//GASHA_INCLUDED_QUICK_SORT_H

// End of file

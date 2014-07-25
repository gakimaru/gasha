#pragma once
#ifndef GASHA_INCLUDED_RADIX_SORT_H
#define GASHA_INCLUDED_RADIX_SORT_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// radix_sort.h
// 基数ソート【宣言部】
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
//ソートアルゴリズム分類：分布ソート
//========================================

//----------------------------------------
//アルゴリズム：基数ソート
//----------------------------------------
//・最良計算時間：O(n*k/d) ※k=キーの範囲、d=基数(256)
//・平均計算時間：O(n*k/d)
//・最悪計算時間：O(n*k/d)
//・メモリ使用量：O(n) ※実際は O(n * 2) のキー情報(12 bytes) + O(256 * 256) のキー分布情報(4 bytes) + O(1～8) のスタック情報（4 bytes）
//・安定性：　　　○
//----------------------------------------
//※内部で_aligned_malloc()を使用し、一時的にメモリ確保するので注意！
//----------------------------------------
//プロトタイプ：
//・KEY_TYPE GET_KEY_FUNCTOR(const T& value)//オブジェクトを受け取りキーを返す
//基数ソート本体
template<class T, class GET_KEY_FUNCTOR>
std::size_t radixSort(T* array, const std::size_t size, GET_KEY_FUNCTOR get_key_functor);
GASHA_OVERLOAD_SET_FOR_DISTRIBUTED_SORT(radixSort);

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/radix_sort.inl>

#endif//GASHA_INCLUDED_RADIX_SORT_H

// End of file

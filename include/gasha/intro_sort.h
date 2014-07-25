#pragma once
#ifndef GASHA_INCLUDED_INTRO_SORT_H
#define GASHA_INCLUDED_INTRO_SORT_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// intro_sort.h
// イントロソート【宣言部】
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
//ソートアルゴリズム分類：混成ソート
//========================================

//----------------------------------------
//アルゴリズム：イントロソート
//----------------------------------------
//・最良計算時間：O(n log n)
//・平均計算時間：O(n log n)
//・最悪計算時間：O(n log n)
//・メモリ使用量：O(n log n) ※ループ処理版は O(32*2)
//・安定性：　　　×
//----------------------------------------
//※クイックソートの再帰レベルが log n に達したら、
//　ヒープソートに切り替える。
//　また、再起の末、ソートの対象件数が一定数（32など）未満に
//　なったら、挿入ソートに切り替える。
//※STLのstd::sort()と同様の手法。
//----------------------------------------
//※再帰処理を使用せず、ループ処理にして最適化する。
//　（最大件数を log2(4294967296) = 32 とする）
//※挿入ソート切り替えタイミングを16に設定する。
//　（VC++2013のSTL std::sortでは32）
//※（再帰の末）対象件数が一定以下になったときに切り替えるアルゴリズムは、
//　本来の挿入ソートではなく、シェルソートを使用するスタイルに改良。
//※再帰が一定以上深くなったときに切り替えるアルゴリズムは、
//　本来のヒープソートではなく、シェルソートを使用するスタイルに改良。
//※整列済み判定を最初に一度行うことで最適化する。
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const T& value1, const T& value2)//value1 == value2 ならtrueを返す
template<class T, class PREDICATE>
inline std::size_t introSort(T* array, const std::size_t size, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_SORT(introSort);

//----------------------------------------
//アルゴリズム：イントロソート
//※イテレータ対応版
//----------------------------------------
//・最良計算時間：O(n log n)
//・平均計算時間：O(n log n)
//・最悪計算時間：O(n log n)
//・メモリ使用量：O(n log n) ※ループ処理版は O(32*2)
//・安定性：　　　×
//----------------------------------------
//プロトタイプ：
//・bool PREDICATE(const typename ITERATOR::value_type& value1, const typename ITERATOR::value_type& value2)//value1 == value2 ならtrueを返す
template<class ITERATOR, class PREDICATE>
inline std::size_t iteratorIntroSort(ITERATOR begin, ITERATOR end, PREDICATE predicate);
GASHA_OVERLOAD_SET_FOR_ITERATOR_SORT(iteratorIntroSort);

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/intro_sort.inl>

#endif//GASHA_INCLUDED_INTRO_SORT_H

// End of file

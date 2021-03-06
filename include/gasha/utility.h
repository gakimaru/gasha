﻿#pragma once
#ifndef GASHA_INCLUDED_UTILITY_H
#define GASHA_INCLUDED_UTILITY_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// utility.h
// 汎用ユーティリティ【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <chrono>//C++11 std::chrono

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//最大値／最小値
//--------------------------------------------------------------------------------

//※max(), min()は、プリミティブ型専用。戻り値に値を返す。引数の型がそれぞれ異なる場合、第一引数の型にキャストされる。
//※maxObj(), minObj()は、オブジェクトに対応。戻り値に、いずれかの引数の参照を返す。全ての引数の型が一致している必要がある。なお、型は operator<() を実装している必要がある。

//----------------------------------------
//最大値取得
//----------------------------------------

//値が二つの max()
template<typename T1, typename T2> inline constexpr T1 max(const T1 n1, const T2 n2);
template<typename T>               inline constexpr const T& maxObj(const T& obj1, const T& obj2);
template<typename T>               inline constexpr T& maxObj(T&& obj1, T&& obj2);

//値が三つ以上の max() : 再帰処理
//※テンプレートの特殊化ではなく、関数のオーバーロードで再起を終結させる
//nxが空になったら、引数が二つの関数が呼ばれる
template<typename T1, typename T2, typename T3, typename... Tx> inline constexpr T1 max(const T1 n1, const T2 n2, const T3 n3, const Tx... nx);
template<typename T1, typename T2, typename T3, typename... Tx> inline constexpr const T1& maxObj(const T1& obj1, const T2& obj2, const T3& obj3, const Tx&... nx);
template<typename T1, typename T2, typename T3, typename... Tx> inline constexpr T1& maxObj(T1&& obj1, T2&& obj2, T3&& obj3, Tx&&... nx);

//値が二つの max()
//ただし、n1 が NULL_VALUE なら、無条件で n2 を返す
template<typename T1, typename T2>
inline constexpr T1 maxIf(const T1 n1, const T2 n2, const T1 null_value = static_cast<T1>(0));

//----------------------------------------
//最小値取得
//----------------------------------------

//値が二つの min()
template<typename T1, typename T2> inline constexpr T1 min(const T1 n1, const T2 n2);
template<typename T>               inline constexpr const T& minObj(const T& obj1, const T& obj2);
template<typename T>               inline constexpr T& minObj(T&& obj1, T&& obj2);

//値が三つ以上の min() : 再帰処理
//※テンプレートの特殊化ではなく、関数のオーバーロードで再起を終結させる
//nxが空になったら、引数が二つの関数が呼ばれる
template<typename T1, typename T2, typename T3, typename... Tx> inline constexpr T1 min(const T1 n1, const T2 n2, const T3 n3, const Tx... nx);
template<typename T1, typename T2, typename T3, typename... Tx> inline constexpr const T1& minObj(const T1& obj1, const T2& obj2, const T3& obj3, const Tx&... nx);
template<typename T1, typename T2, typename T3, typename... Tx> inline constexpr T1& minObj(T1&& obj1, T2&& obj2, T3&& obj3, Tx&&... nx);

//値が二つの min()
//ただし、n1 が NULL_VALUE なら、無条件で n2 を返す
template<typename T1, typename T2>
inline constexpr T1 minIf(const T1 n1, const T2 n2, const T1 null_value = static_cast<T1>(0));

//----------------------------------------
//nullptr以外の値を取得
//----------------------------------------

//値が一つの nvl()
//※nullptr 時は "(null)" を返す
template<typename T> inline constexpr const T* nvl(const T* n1);

//値が二つの nvl()
template<typename T> inline constexpr const T* nvl(const T* n1, const T* n2);
template<typename T> inline constexpr T* nvl(T* n1, T* n2);

//値が三つ以上の nvl() : 再帰処理
//※テンプレートの特殊化ではなく、関数のオーバーロードで再起を終結させる
//nxが空になったら、引数が二つの関数が呼ばれる
template<typename T, typename... Tx> inline constexpr const T* nvl(const T* n1, const T* n2, const T* n3, const Tx&... nx);
template<typename T, typename... Tx> inline constexpr T* nvl(T* n1, T* n2, T* n3, Tx&&... nx);

//--------------------------------------------------------------------------------
//値の交換
//--------------------------------------------------------------------------------

//----------------------------------------
//値の交換
template<class T>
inline void swapValues(T& val1, T& val2);

//----------------------------------------
//値の交換
//※イテレータ対応版
template<class ITERATOR>
inline void iteratorSwapValues(ITERATOR val1, ITERATOR val2);

//--------------------------------------------------------------------------------
//複数の値（要素）のローテーション
//--------------------------------------------------------------------------------

//----------------------------------------
//値のローテーション
template<class T>
inline void rotateValues(T* val1, T* val2, int step);

//----------------------------------------
//値のローテーション
//※イテレータ対応版
template<class ITERATOR>
inline void iteratorRotateValues(ITERATOR val1, ITERATOR val2, typename ITERATOR::difference_type step);

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/utility.inl>

#endif//GASHA_INCLUDED_UTILITY_H

// End of file

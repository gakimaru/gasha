#pragma once
#ifndef GASHA_INCLUDED_UTILITY_H
#define GASHA_INCLUDED_UTILITY_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ】
// utility.h
// 汎用ユーティリティ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
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
template<typename T1, typename T2> constexpr inline T1 max(const T1& n1, const T2& n2);
template<typename T>               constexpr inline const T& maxObj(const T& obj1, const T& obj2);
template<typename T>               constexpr inline T& maxObj(T&& obj1, T&& obj2);

//値が三つ以上の max() : 再帰処理
//※テンプレートの特殊化ではなく、関数のオーバーロードで再起を終結させる
//nxが空になったら、引数が二つの関数が呼ばれる
template<typename T1, typename T2, typename T3, typename... Tx> constexpr inline T1 max(const T1& n1, const T2& n2, const T3& n3, const Tx&... nx);
template<typename T1, typename T2, typename T3, typename... Tx> constexpr inline const T1& maxObj(const T1& obj1, const T2& obj2, const T3& obj3, const Tx&... nx);
template<typename T1, typename T2, typename T3, typename... Tx> constexpr inline T1& maxObj(T1&& obj1, T2&& obj2, T3&& obj3, Tx&&... nx);

//----------------------------------------
//最小値取得
//----------------------------------------

//値が二つの min()
template<typename T1, typename T2> constexpr inline T1 min(const T1& n1, const T2& n2);
template<typename T>               constexpr inline const T& minObj(const T& obj1, const T& obj2);
template<typename T>               constexpr inline T& minObj(T&& obj1, T&& obj2);

//値が三つ以上の min() : 再帰処理
//※テンプレートの特殊化ではなく、関数のオーバーロードで再起を終結させる
//nxが空になったら、引数が二つの関数が呼ばれる
template<typename T1, typename T2, typename T3, typename... Tx> constexpr inline T1 min(const T1& n1, const T2& n2, const T3& n3, const Tx&... nx);
template<typename T1, typename T2, typename T3, typename... Tx> constexpr inline const T1& minObj(const T1& obj1, const T2& obj2, const T3& obj3, const Tx&... nx);
template<typename T1, typename T2, typename T3, typename... Tx> constexpr inline T1& minObj(T1&& obj1, T2&& obj2, T3&& obj3, Tx&&... nx);

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

//--------------------------------------------------------------------------------
//時間
//--------------------------------------------------------------------------------

//----------------------------------------
//経過時間を算出
//※秒を返す
inline std::chrono::system_clock::time_point nowTime();
inline double calcElapsedTime(const std::chrono::system_clock::time_point begin);
inline double calcElapsedTime(const std::chrono::system_clock::time_point begin, const std::chrono::system_clock::time_point end);

//----------------------------------------
//経過時間クラス
class elapsedTime
{
public:
	//開始時間を取得
	inline double now();
	//開始時間をリセット
	//※開始時間を現在時間にリセット
	inline void reset();
public:
	//コンストラクタ
	inline elapsedTime();
private:
	std::chrono::system_clock::time_point m_beginTime;//開始時間
};

//----------------------------------------
//システム経過時間

//取得
inline double nowSysElapsedTime();
//リセット
//※開始時間を現在時間にリセット
inline void resetSysElapsedTime();

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/utility.inl>

#endif//GASHA_INCLUDED_UTILITY_H

// End of file

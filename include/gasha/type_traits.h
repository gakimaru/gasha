#pragma once
#ifndef __TYPE_TRAITS_H_
#define __TYPE_TRAITS_H_

//--------------------------------------------------------------------------------
//【テンプレートライブラリ】
// type_traits.h
// 型特性ユーティリティ
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//多くの型情報は #include <type_traits> で扱えます
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//配列型情報
//--------------------------------------------------------------------------------

//----------------------------------------
//配列要素数／次元数／要素サイズ取得
//----------------------------------------
//※四次元配列まで対応。
//※std::extentと異なり、型ではなく変数を指定する。
//※std::extentと異なり、extentof()は全次元の合計要素数を返す。（std::extentは対象次元をテンプレート引数で指定。デフォルトは一次元目）
//----------------------------------------

//非配列用
template<typename T> inline std::size_t rankof(const T& data){ return 0; }
template<typename T> inline std::size_t sizeofelemof(const T& data){ return sizeof(T); }
template<typename T> inline std::size_t extentof(const T& data){ return 0; }
template<typename T> inline std::size_t extent1of(const T& data){ return 0; }
template<typename T> inline std::size_t extent2of(const T& data){ return 0; }
template<typename T> inline std::size_t extent3of(const T& data){ return 0; }
template<typename T> inline std::size_t extent4of(const T& data){ return 0; }

//一次配列用
template<typename T, std::size_t N1> inline std::size_t rankof(const T(&data)[N1]){ return 1; }
template<typename T, std::size_t N1> inline std::size_t sizeofelemof(const T(&data)[N1]){ return sizeof(T); }
template<typename T, std::size_t N1> inline std::size_t extentof(const T(&data)[N1]){ return N1; }
template<typename T, std::size_t N1> inline std::size_t extent1of(const T(&data)[N1]){ return N1; }
//template<typename T, std::size_t N1> inline std::size_t extent2of(const T(&data)[N1]){ return 0; }//※不要
//template<typename T, std::size_t N1> inline std::size_t extent3of(const T(&data)[N1]){ return 0; }//※不要
//template<typename T, std::size_t N1> inline std::size_t extent4of(const T(&data)[N1]){ return 0; }//※不要

//二次配列用
template<typename T, std::size_t N1, std::size_t N2> inline std::size_t rankof(const T(&data)[N1][N2]){ return 2; }
template<typename T, std::size_t N1, std::size_t N2> inline std::size_t sizeofelemof(const T(&data)[N1][N2]){ return sizeof(T); }
template<typename T, std::size_t N1, std::size_t N2> inline std::size_t extentof(const T(&data)[N1][N2]){ return N1 * N2; }
//template<typename T, std::size_t N1, std::size_t N2> inline std::size_t extent1of(const T(&data)[N1][N2]){ return N1; }//※不要
template<typename T, std::size_t N1, std::size_t N2> inline std::size_t extent2of(const T(&data)[N1][N2]){ return N2; }
//template<typename T, std::size_t N1, std::size_t N2> inline std::size_t extent3of(const T(&data)[N1][N2]){ return 0; }//※不要
//template<typename T, std::size_t N1, std::size_t N2> inline std::size_t extent4of(const T(&data)[N1][N2]){ return 0; }//※不要

//三次配列用
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline std::size_t rankof(const T(&data)[N1][N2][N3]){ return 3; }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline std::size_t sizeofelemof(const T(&data)[N1][N2][N3]){ return sizeof(T); }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline std::size_t extentof(const T(&data)[N1][N2][N3]){ return N1 * N2 * N3; }
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline std::size_t extent1of(const T(&data)[N1][N2][N3]){ return N1; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline std::size_t extent2of(const T(&data)[N1][N2][N3]){ return N2; }//※不要
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline std::size_t extent3of(const T(&data)[N1][N2][N3]){ return N3; }
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> inline std::size_t extent4of(const T(&data)[N1][N2][N3]){ return 0; }//※不要

//四次配列用
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline std::size_t rankof(const T(&data)[N1][N2][N3][N4]){ return 4; }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline std::size_t sizeofelemof(const T(&data)[N1][N2][N3][N4]){ return sizeof(T); }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline std::size_t extentof(const T(&data)[N1][N2][N3][N4]){ return N1 * N2 * N3 * N4; }
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline std::size_t extent1of(const T(&data)[N1][N2][N3][N4]){ return N1; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline std::size_t extent2of(const T(&data)[N1][N2][N3][N4]){ return N2; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline std::size_t extent3of(const T(&data)[N1][N2][N3][N4]){ return N3; }//※不要
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> inline std::size_t extent4of(const T(&data)[N1][N2][N3][N4]){ return N4; }

//--------------------------------------------------------------------------------
//ポインター型情報
//--------------------------------------------------------------------------------

//----------------------------------------
//ポインターでも実体でも値（クラス／構造体）のサイズを返す sizeof()
//----------------------------------------
template<typename T> inline std::size_t sizeofvalueof(T val){ return sizeof(T); }
template<typename T> inline std::size_t sizeofvalueof(T* val){ return sizeof(T); }

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__TYPE_TRAITS_H_

// End of file

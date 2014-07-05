#pragma once
#ifndef GASHA_INCLUDED_TYPE_TRAITS_H
#define GASHA_INCLUDED_TYPE_TRAITS_H

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

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

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
template<typename T> constexpr inline std::size_t rankof(const T& data){ return 0; }
template<typename T> constexpr inline std::size_t sizeofelemof(const T& data){ return sizeof(T); }
template<typename T> constexpr inline std::size_t extentof(const T& data){ return 0; }
template<typename T> constexpr inline std::size_t extent1of(const T& data){ return 0; }
template<typename T> constexpr inline std::size_t extent2of(const T& data){ return 0; }
template<typename T> constexpr inline std::size_t extent3of(const T& data){ return 0; }
template<typename T> constexpr inline std::size_t extent4of(const T& data){ return 0; }
template<typename T> constexpr inline std::size_t extent5of(const T& data){ return 0; }

//一次配列用
template<typename T, std::size_t N1> constexpr inline std::size_t rankof(const T(&data)[N1]){ return 1; }
template<typename T, std::size_t N1> constexpr inline std::size_t sizeofelemof(const T(&data)[N1]){ return sizeof(T); }
template<typename T, std::size_t N1> constexpr inline std::size_t extentof(const T(&data)[N1]){ return N1; }
template<typename T, std::size_t N1> constexpr inline std::size_t extent1of(const T(&data)[N1]){ return N1; }
//template<typename T, std::size_t N1> constexpr inline std::size_t extent2of(const T(&data)[N1]){ return 0; }//※不要
//template<typename T, std::size_t N1> constexpr inline std::size_t extent3of(const T(&data)[N1]){ return 0; }//※不要
//template<typename T, std::size_t N1> constexpr inline std::size_t extent4of(const T(&data)[N1]){ return 0; }//※不要
//template<typename T, std::size_t N1> constexpr inline std::size_t extent5of(const T(&data)[N1]){ return 0; }//※不要

//二次配列用
template<typename T, std::size_t N1, std::size_t N2> constexpr inline std::size_t rankof(const T(&data)[N1][N2]){ return 2; }
template<typename T, std::size_t N1, std::size_t N2> constexpr inline std::size_t sizeofelemof(const T(&data)[N1][N2]){ return sizeof(T); }
template<typename T, std::size_t N1, std::size_t N2> constexpr inline std::size_t extentof(const T(&data)[N1][N2]){ return N1 * N2; }
//template<typename T, std::size_t N1, std::size_t N2> constexpr inline std::size_t extent1of(const T(&data)[N1][N2]){ return N1; }//※不要
template<typename T, std::size_t N1, std::size_t N2> constexpr inline std::size_t extent2of(const T(&data)[N1][N2]){ return N2; }
//template<typename T, std::size_t N1, std::size_t N2> constexpr inline std::size_t extent3of(const T(&data)[N1][N2]){ return 0; }//※不要
//template<typename T, std::size_t N1, std::size_t N2> constexpr inline std::size_t extent4of(const T(&data)[N1][N2]){ return 0; }//※不要
//template<typename T, std::size_t N1, std::size_t N2> constexpr inline std::size_t extent5of(const T(&data)[N1][N2]){ return 0; }//※不要

//三次配列用
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> constexpr inline std::size_t rankof(const T(&data)[N1][N2][N3]){ return 3; }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> constexpr inline std::size_t sizeofelemof(const T(&data)[N1][N2][N3]){ return sizeof(T); }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> constexpr inline std::size_t extentof(const T(&data)[N1][N2][N3]){ return N1 * N2 * N3; }
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> constexpr inline std::size_t extent1of(const T(&data)[N1][N2][N3]){ return N1; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> constexpr inline std::size_t extent2of(const T(&data)[N1][N2][N3]){ return N2; }//※不要
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> constexpr inline std::size_t extent3of(const T(&data)[N1][N2][N3]){ return N3; }
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> constexpr inline std::size_t extent4of(const T(&data)[N1][N2][N3]){ return 0; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3> constexpr inline std::size_t extent5of(const T(&data)[N1][N2][N3]){ return 0; }//※不要

//四次配列用
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> constexpr inline std::size_t rankof(const T(&data)[N1][N2][N3][N4]){ return 4; }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> constexpr inline std::size_t sizeofelemof(const T(&data)[N1][N2][N3][N4]){ return sizeof(T); }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> constexpr inline std::size_t extentof(const T(&data)[N1][N2][N3][N4]){ return N1 * N2 * N3 * N4; }
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> constexpr inline std::size_t extent1of(const T(&data)[N1][N2][N3][N4]){ return N1; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> constexpr inline std::size_t extent2of(const T(&data)[N1][N2][N3][N4]){ return N2; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> constexpr inline std::size_t extent3of(const T(&data)[N1][N2][N3][N4]){ return N3; }//※不要
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> constexpr inline std::size_t extent4of(const T(&data)[N1][N2][N3][N4]){ return N4; }
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4> constexpr inline std::size_t extent5of(const T(&data)[N1][N2][N3][N4]){ return 0; }

//五次配列用
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> constexpr inline std::size_t rankof(const T(&data)[N1][N2][N3][N4][N5]){ return 5; }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> constexpr inline std::size_t sizeofelemof(const T(&data)[N1][N2][N3][N4][N5]){ return sizeof(T); }
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> constexpr inline std::size_t extentof(const T(&data)[N1][N2][N3][N4][N5]){ return N1 * N2 * N3 * N4 * N5; }
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> constexpr inline std::size_t extent1of(const T(&data)[N1][N2][N3][N4][N5]){ return N1; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> constexpr inline std::size_t extent2of(const T(&data)[N1][N2][N3][N4][N5]){ return N2; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> constexpr inline std::size_t extent3of(const T(&data)[N1][N2][N3][N4][N5]){ return N3; }//※不要
//template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> constexpr inline std::size_t extent4of(const T(&data)[N1][N2][N3][N4][N5]){ return N4; }//※不要
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t N4, std::size_t N5> constexpr inline std::size_t extent5of(const T(&data)[N1][N2][N3][N4][N5]){ return N5; }

//※constexprが使えない環境でのメタプログラミング用（extent*ofだけマクロで用意）
#define extent1of_m(data) (sizeof(data) / sizeof(data[0]))
#define extent2of_m(data) (sizeof(data[0]) / sizeof(data[0][0]))
#define extent3of_m(data) (sizeof(data[0][0]) / sizeof(data[0][0][0]))
#define extent4of_m(data) (sizeof(data[0][0][0]) / sizeof(data[0][0][0][0]))
#define extent5of_m(data) (sizeof(data[0][0][0][0]) / sizeof(data[0][0][0][0][0]))

//--------------------------------------------------------------------------------
//ポインター型情報
//--------------------------------------------------------------------------------

//----------------------------------------
//ポインターでも実体でも値（クラス／構造体）のサイズを返す sizeof()
//----------------------------------------
template<typename T> constexpr inline std::size_t sizeofvalueof(T val){ return sizeof(T); }
template<typename T> constexpr inline std::size_t sizeofvalueof(T* val){ return sizeof(T); }

//--------------------------------------------------------------------------------
//クラス／構造体に比較演算子を自動定義するためのCRPTクラス
//--------------------------------------------------------------------------------
//使用方法：
//  struct derived : public operatorCRTP<derived, int>//テンプレート引数には、「自身の型」と「比較に用いる型」を指定する
//  {                                                 //※自身の型には、「比較に用いる型」へのキャストオペレータを実装しておく必要がある
//      operator int() const { return m_primaryData; }//int型にキャスト
//      int m_primaryData;//主要データ（比較に用いる）
//      int m_otherData;//他のデータ
//  };
template<class base, typename cast_type>
class operatorCRTP
{
public:
	friend inline bool operator==(const base& lhs, const base& rhs){ return static_cast<cast_type>(lhs) == static_cast<cast_type>(rhs); }
	friend inline bool operator!=(const base& lhs, const base& rhs){ return static_cast<cast_type>(lhs) != static_cast<cast_type>(rhs); }
	friend inline bool operator<(const base& lhs, const base& rhs){ return static_cast<cast_type>(lhs) < static_cast<cast_type>(rhs); }
	friend inline bool operator>(const base& lhs, const base& rhs){ return static_cast<cast_type>(lhs) > static_cast<cast_type>(rhs); }
	friend inline bool operator<=(const base& lhs, const base& rhs){ return static_cast<cast_type>(lhs) <= static_cast<cast_type>(rhs); }
	friend inline bool operator>=(const base& lhs, const base& rhs){ return static_cast<cast_type>(lhs) >= static_cast<cast_type>(rhs); }
};

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_TYPE_TRAITS_H

// End of file

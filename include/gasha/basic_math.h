#pragma once
#ifndef GASHA_INCLUDED_BASIC_MATH_H
#define GASHA_INCLUDED_BASIC_MATH_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// basic_math.h
// 基本算術【宣言部】
//
//【注意】GCCの場合、#pragma GCC diagnostic ignored "-Wstrict-overflow" を
//        設定しているため、インクルードすると "strict-overflow" 警告に
//        反応しなくなる点に注意。
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/limits.h>//限界値

#include <cstddef>//std::size_t
#include <type_traits>//C++11 std::conditional, std:integral_constant

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//符号判定
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】符号判定（sign）
//----------------------------------------
//※値の符号に応じて、int 値を返す。
//※0 なら 0、正の数なら 1、負の数なら -1 を返す。
//----------------------------------------

template<typename T>
inline constexpr int sign(const T value);
template<>
inline constexpr int sign<float>(const float value);
template<>
inline constexpr int sign<double>(const double value);
template<>
inline constexpr int sign<bool>(const bool value);

//--------------------------------------------------------------------------------
//範囲計算
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】範囲計算（range）
//----------------------------------------
//※整数なら 最大値 - 最小値 + 1 を返す。
//※浮動小数点なら 最大値 - 最小値 を返す。
//----------------------------------------

template<typename T>
inline constexpr T range(const T max, const T min);
template<>
inline constexpr float range<float>(const float max, const float min);
template<>
inline constexpr double range<double>(const double max, const double min);
template<>
inline constexpr bool range<bool>(const bool max, const bool min);

//--------------------------------------------------------------------------------
//べき乗
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】べき乗（power）
//----------------------------------------
//※浮動小数点も使用可。
//※指数はint型固定。
//※コンパイル時に計算結果が算出されるかどうかはコンパイラ次第。
//※指数に負の数を指定するとコンパイルエラー。
//※関数の再帰呼び出しを静的に展開するので、指数が小さければ関数が消滅して高速になる（はず）。
//※引数が定数なら、コンパイル時に計算が済む可能性も高い。
//----------------------------------------

//※直接使用する関数
template<typename T, int E>
inline T pow(const T n);

//※再帰終端用のオーバーロード関数（関数テンプレートは部部特殊化できないため、各型のバリエーションを用意）
template<> inline char pow<char, 0>(const char n);//再帰終端用
template<> inline unsigned char pow<unsigned char, 0>(const unsigned char n){ return 1; }//再帰終端用
template<> inline short pow<short, 0>(const short n){ return 1; }//再帰終端用
template<> inline unsigned short pow<unsigned short, 0>(const unsigned short n){ return 1; }//再帰終端用
template<> inline int pow<int, 0>(const int n){ return 1; }//再帰終端用
template<> inline unsigned int pow<unsigned int, 0>(const unsigned int n){ return 1; }//再帰終端用
template<> inline long pow<long, 0>(const long n){ return 1; }//再帰終端用
template<> inline unsigned long pow<unsigned long, 0>(const unsigned long n){ return 1; }//再帰終端用
template<> inline long long pow<long long, 0>(const long long n){ return 1; }//再帰終端用
template<> inline unsigned long long pow<unsigned long long, 0>(const unsigned long long n){ return 1; }//再帰終端用
template<> inline float pow<float, 0>(const float n){ return 1.f; }//再帰終端用
template<> inline double pow<double, 0>(const double n){ return 1.; }//再帰終端用

//----------------------------------------
//【ランタイム版】負の指数のべき乗（negative power）
//----------------------------------------
//※浮動小数点も使用可。
//※指数はint型固定。
//※コンパイル時に計算結果が算出されるかどうかはコンパイラ次第。
//※指数に正の数また0を指定するとコンパイルエラー。
//----------------------------------------
template<typename T, int E>
inline T negaPow(const T n);

//----------------------------------------
//【メタプログラミング版】べき乗（power）
//----------------------------------------
//※整数専用。
//※指数はint型固定。
//※確実にコンパイル時に計算結果を算出。
//※指数に負の数を指定するとコンパイルエラー。
//----------------------------------------

//※直接使用するクラス
template<typename T, T N, int E>
struct staticPow
{
	static_assert(E > 0, "E is only supported greater than 0.");
	static const T value = N * GASHA_ staticPow<T, N, E - 1>::value;
};

//※再帰終端用の特殊化クラス
template<typename T, int N>
struct staticPow<T, N, 0>
{
	static const T value = 1;
};

//--------------------------------------------------------------------------------
//素数計算
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】
//----------------------------------------

//----------------------------------------
//【ランタイム版】素数判定
bool isPrime(const unsigned int n);

//----------------------------------------
//【ランタイム版】指定の値より小さい最初の素数を算出
unsigned int makePrimeLT(const unsigned int n);

//----------------------------------------
//【ランタイム版】指定の値と同じか、それより小さい最初の素数を算出
inline unsigned int makePrimeLE(const unsigned int n);

//----------------------------------------
//【ランタイム版】指定の値より大きい最初の素数を算出
unsigned int makePrimeGT(const unsigned int n);

//----------------------------------------
//【ランタイム版】指定の値と同じか、それより大きい最初の素数を算出
inline unsigned int makePrimeGE(const unsigned int n);

//----------------------------------------
//【メタプログラミング版】
//----------------------------------------

//----------------------------------------
//【メタプログラミング版】静的素数判定
//※偶数の判定を避けるために階層化する
//静的素数判定用の再帰クラス（直接使用しない）
template <unsigned int N, unsigned int DIV>
struct _isStaticPrime{
	typedef
		typename std::conditional<
			(DIV > N / DIV),//DIV = ～N/DIVの範囲で割り切れる値があるか判定
			std::integral_constant<bool, true>,//範囲を超えたので素数と判定
			typename std::conditional<
				(N % DIV == 0),//割り切れる値か判定
				std::integral_constant<bool, false>,//割り切れたので素数ではない
				GASHA_ _isStaticPrime<N, DIV + 2>//再帰で次の値が割り切れるか探索 ※偶数は判定しない
			>::type
		>::type
		type;
	static const bool value = type::value;
};
//静的素数判定クラス
template <unsigned int N>
struct isStaticPrime{
	typedef
		typename std::conditional<
			(N & 1) == 0,//偶数判定
			std::integral_constant<bool, false>,//偶数は素数ではない
			typename GASHA_ _isStaticPrime<N, 3>::type//素数判定ループ（再帰処理）呼び出し
		>::type
		type;
	static const bool value = type::value;
};
//特殊化：0は素数ではない
template <>
struct isStaticPrime<0>{
	static const bool value = false;
};
//特殊化：1は素数ではない
template <>
struct isStaticPrime<1>{
	static const bool value = false;
};
//特殊化：2は素数
template <>
struct isStaticPrime<2>{
	static const bool value = true;
};

//----------------------------------------
//【メタプログラミング版】指定の値より小さい最初の素数を静的に算出
//※偶数の判定を避けるために階層化する
//静的素数算出用の再帰クラス（直接使用しない）
template<unsigned int N>
struct _makeStaticPrimeLT{
	typedef
		typename std::conditional<
			GASHA_ isStaticPrime<N>::value,//素数判定
			std::integral_constant<unsigned int, N>,//素数が見つかった
			GASHA_ _makeStaticPrimeLT<N - 2>//再帰で次に小さい値を探索 ※偶数は判定しない
		>::type
		type;
	static const unsigned int value = type::value;
};
//静的素数算出クラス
template<unsigned int N>
struct makeStaticPrimeLT{
	typedef
		typename std::conditional<
			(N & 1) == 0,//素数判定ループの初期値を奇数にするための判定
			GASHA_ _makeStaticPrimeLT<N - 1>,//素数判定ループ（再帰処理）呼び出し
			GASHA_ _makeStaticPrimeLT<N - 2>//素数判定ループ（再帰処理）呼び出し
		>::type
		type;
	static const unsigned int value = type::value;
};
//特殊化：0より小さい素数はなし
template<>
struct makeStaticPrimeLT<0>{
	static const unsigned int value = 0;
};
//特殊化：1より小さい素数はなし
template<>
struct makeStaticPrimeLT<1>{
	static const unsigned int value = 0;
};
//特殊化：2より小さい素数はなし
template<>
struct makeStaticPrimeLT<2>{
	static const unsigned int value = 0;
};
//特殊化：3より小さい素数は2
template<>
struct makeStaticPrimeLT<3>{
	static const unsigned int value = 2;
};
//----------------------------------------
//【メタプログラミング版】指定の値と同じか、それより小さい最初の素数を静的に算出
//静的素数算出クラス
template<unsigned int N>
struct makeStaticPrimeLE{
	typedef
		typename std::conditional<
			GASHA_ isStaticPrime<N>::value,//指定の値が素数か？
			std::integral_constant<unsigned int, N>,//素数が見つかった
			GASHA_ makeStaticPrimeLT<N>//次に小さい値を探索
		>::type
	type;
	static const unsigned int value = type::value;
};

//----------------------------------------
//【メタプログラミング版】指定の値より大きい最初の素数を静的に算出
//※偶数の判定を避けるために階層化する
//静的素数算出用の再帰クラス（直接使用しない）
template<unsigned int N>
struct _makeStaticPrimeGT{
	typedef
		typename std::conditional<
			GASHA_ isStaticPrime<N>::value,//素数判定
			std::integral_constant<unsigned int, N>,//素数が見つかった
			GASHA_ _makeStaticPrimeGT<N + 2>//再帰で次に大きい値を探索 ※偶数は判定しない
		>::type
		type;
	static const unsigned int value = type::value;
};
//静的素数算出クラス
template<unsigned int N>
struct makeStaticPrimeGT{
	typedef
		typename std::conditional<
			(N & 1) == 0,//素数判定ループの初期値を奇数にするための判定
			GASHA_ _makeStaticPrimeGT<N + 1>,//素数判定ループ（再帰処理）呼び出し
			GASHA_ _makeStaticPrimeGT<N + 2>//素数判定ループ（再帰処理）呼び出し
		>::type
		type;
	static const unsigned int value = type::value;
};
//特殊化：0より大きい素数は2
template<>
struct makeStaticPrimeGT<0>{
	static const unsigned int value = 2;
};
//特殊化：1より大きい素数は2
template<>
struct makeStaticPrimeGT<1>{
	static const unsigned int value = 2;
};

//----------------------------------------
//【メタプログラミング版】指定の値と同じか、それより大きい最初の素数を静的に算出
//静的素数算出クラス
template<unsigned int N>
struct makeStaticPrimeGE{
	typedef
		typename std::conditional<
			GASHA_ isStaticPrime<N>::value,//指定の値が素数か？
			std::integral_constant<unsigned int, N>,//素数が見つかった
			GASHA_ makeStaticPrimeGT<N>//次に小さい値を探索
		>::type
		type;
	static const unsigned int value = type::value;
};

//----------------------------------------
//【メタプログラミングの使用上の注意】
// メタプログラミング版では、コンパイラにより、テンプレートの
// インスタンス化の深度に限界がある。
// そのため、静的素数判定・算出では、扱える値に上限がある。
// VC++による実測では、上限は下記の通り。
//
//     static const bool         _is_prime = isStaticPrime<1262477>::value;   //限界OK ※次の isStaticPrime<1262479>    はコンパイルエラー
//     static const unsigned int _prime_gt = makeStaticPrimeGT<952788>::value;//限界OK ※次の makeStaticPrimeGT<952789> はコンパイルエラー
//
// makeStaticPrimeGTの結果からも分かるとおり、テンプレートをネストすると
// この限界値は更に下がっていく。
// また、大きな素数を指定すると、コンパイル時間が長くなっていく点にも注意。
// なお、単純に深度限界をチェックするテンプレートクラスを作ってテストした
// 結果は下記のとおり。
//
//     template<int N> struct recursive{ static const int value = recursive<N - 1>::value; };
//     template<>      struct recursive<0>{ static const int value = 1; };
//     static const int _n = recursive<499>::value;//VC++2013では限界OK ※次の recursive<500> はコンパイルエラー
//     static const int _n = recursive<900>::value;//GCC4.8.2では限界OK ※次の recursive<901> はコンパイルエラー
//----------------------------------------

//--------------------------------------------------------------------------------
//ビット計算
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】
//----------------------------------------

//----------------------------------------
//【ランタイム版】ビット数を数える
#ifdef GASHA_IS_VC
#ifdef GASHA_USE_POPCNT
#define GASHA_ENABLE_BUILTIN_POPCNT//ビルトインビット数計測が有効
#endif//GASHA_USE_POPCNT
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
#define GASHA_ENABLE_BUILTIN_POPCNT//ビルトインビット数計測が有効
#endif//GASHA_IS_GCC

#ifdef GASHA_USE_SSE4_2
#define GASHA_ENABLE_SSE_POPCNT//SSE命令版ビット数計測が有効
#endif//GASHA_USE_SSE4_2

//※独自実装版
int countBits_custom(const unsigned int value);

#ifdef GASHA_ENABLE_BUILTIN_POPCNT
//※ビルトイン処理版
int countBits_builtin(const unsigned int value);
#endif//GASHA_ENABLE_BUILTIN_POPCNT

#ifdef GASHA_ENABLE_SSE_POPCNT
//※SSE命令版
int countBits_sse(const unsigned int value);
#endif//GASHA_ENABLE_SSE_POPCNT

//標準版（中身は上記のいずれか）
//※SS命令E版→ビルトイン版→独自版の優先順位で適用する
//※なお、ビルトイン版とSSE命令版は本質的に同じ（はず）。
inline int countBits(const unsigned int value)
{
#ifdef GASHA_ENABLE_SSE_POPCNT
	return countBits_sse(value);//SSE命令版
#else//GASHA_ENABLE_SSE_POPCNT
#ifdef GASHA_ENABLE_BUILTIN_POPCNT
	return countBits_builtin(value);//ビルトイン版
#else//GASHA_ENABLE_BUILTIN_POPCNT
	return countBits_custom(value);//独自実装版
#endif//GASHA_ENABLE_BUILTIN_POPCNT
#endif//GASHA_ENABLE_SSE_POPCNT
}

//----------------------------------------
//【ランタイム版】MSB(Most Significant Bit)を算出
int calcMSB(const unsigned int value);

//----------------------------------------
//【ランタイム版】LSB(Least Significant Bit)を算出
int calcLSB(const unsigned int value);

//----------------------------------------
//【ランタイム版】指定の値以上（Greater than or Equal）で最も小さい2のべき乗を算出
inline unsigned int calcNearPow2GE(const unsigned int value);

//----------------------------------------
//【ランタイム版】指定の値以下（Less than or Equal）で最も大きい2のべき乗を算出
inline unsigned int calcNearPow2LE(const unsigned int value);

//----------------------------------------
//【メタプログラミング版】
//----------------------------------------

//----------------------------------------
//【メタプログラミング版】ビット数を数える
template<unsigned int BITS> struct _countStaticBits_0x0000ffff{ static const int value =                             (BITS & 0x0000ffff) + (BITS >> 16 & 0x0000ffff);         };
template<unsigned int BITS> struct _countStaticBits_0x00ff00ff{ static const int value = _countStaticBits_0x0000ffff<(BITS & 0x00ff00ff) + (BITS >>  8 & 0x00ff00ff)>::value; };
template<unsigned int BITS> struct _countStaticBits_0x0f0f0f0f{ static const int value = _countStaticBits_0x00ff00ff<(BITS & 0x0f0f0f0f) + (BITS >>  4 & 0x0f0f0f0f)>::value; };
template<unsigned int BITS> struct _countStaticBits_0x33333333{ static const int value = _countStaticBits_0x0f0f0f0f<(BITS & 0x33333333) + (BITS >>  2 & 0x33333333)>::value; };
template<unsigned int BITS> struct _countStaticBits_0x55555555{ static const int value = _countStaticBits_0x33333333<(BITS & 0x55555555) + (BITS >>  1 & 0x55555555)>::value; };
template<unsigned int BITS> struct countStaticBits{	            static const int value = _countStaticBits_0x55555555<BITS>::value;                                            };

//----------------------------------------
//【メタプログラミング版】MSB(Most Significant Bit)を算出
template<unsigned int BITS> struct _calcStaticMSB_s16{ static const int value =                    BITS | (BITS >> 16);         };
template<unsigned int BITS> struct _calcStaticMSB_s08{ static const int value = _calcStaticMSB_s16<BITS | (BITS >>  8)>::value; };
template<unsigned int BITS> struct _calcStaticMSB_s04{ static const int value = _calcStaticMSB_s08<BITS | (BITS >>  4)>::value; };
template<unsigned int BITS> struct _calcStaticMSB_s02{ static const int value = _calcStaticMSB_s04<BITS | (BITS >>  2)>::value; };
template<unsigned int BITS> struct _calcStaticMSB_s01{ static const int value = _calcStaticMSB_s02<BITS | (BITS >>  1)>::value; };
template<unsigned int BITS> struct calcStaticMSB{      static const int value = countStaticBits<_calcStaticMSB_s01<BITS>::value>::value - 1; };
template<>                  struct calcStaticMSB<0>{   static const int value = -1; };

//----------------------------------------
//【メタプログラミング版】LSB(Least Significant Bit)を算出
template<unsigned int BITS> struct _calcStaticLSB_s16{ static const int value =                    BITS | (BITS << 16);         };
template<unsigned int BITS> struct _calcStaticLSB_s08{ static const int value = _calcStaticLSB_s16<BITS | (BITS <<  8)>::value; };
template<unsigned int BITS> struct _calcStaticLSB_s04{ static const int value = _calcStaticLSB_s08<BITS | (BITS <<  4)>::value; };
template<unsigned int BITS> struct _calcStaticLSB_s02{ static const int value = _calcStaticLSB_s04<BITS | (BITS <<  2)>::value; };
template<unsigned int BITS> struct _calcStaticLSB_s01{ static const int value = _calcStaticLSB_s02<BITS | (BITS <<  1)>::value; };
template<unsigned int BITS> struct calcStaticLSB{      static const int value = countStaticBits<~_calcStaticLSB_s01<BITS>::value>::value; };
template<>                  struct calcStaticLSB<0>{   static const int value = -1; };

//----------------------------------------
//【メタプログラミング版】指定の値以上（Greater than or Equal）で最も小さい２のべき乗を算出
template<unsigned int SHIFT> struct _calcStaticNearPow2_shift{     static const int value = 1 << SHIFT; };
template<>                   struct _calcStaticNearPow2_shift<32>{ static const int value = 0; };
template<unsigned int VAL> struct calcStaticNearPow2GE{ static const int value = _calcStaticNearPow2_shift<(calcStaticMSB<VAL - 1>::value + 1)>::value; };
template<>                 struct calcStaticNearPow2GE<0>{ static const int value = 0; };

//----------------------------------------
//【メタプログラミング版】指定の値以下（Less than or Equal）で最も大きい２のべき乗を算出
template<unsigned int VAL> struct calcStaticNearPow2LE{ static const int value = _calcStaticNearPow2_shift<calcStaticMSB<VAL>::value>::value; };
template<>                 struct calcStaticNearPow2LE<0>{ static const int value = 0; };

//--------------------------------------------------------------------------------
//テンプレート平方根
//※fast_math.hをインクルードすることで、float型の時に特殊化して、高速演算を利用可能
//--------------------------------------------------------------------------------

template<typename T> inline T sqr(const T value);

//--------------------------------------------------------------------------------
//テンプレートベクトル演算
//※fast_math.hをインクルードすることで、float×3,float×4の時に特殊化して、高速演算を利用可能
//--------------------------------------------------------------------------------

//----------------------------------------
//ノルム
//※平方根適用
template<typename T, std::size_t N>
inline T norm(const T (&vec)[N]);

//----------------------------------------
//ノルムの二乗
template<typename T, std::size_t N>
inline T normSq(const T (&vec)[N]);

//----------------------------------------
//合成（加算）
//※ベクトルを返す
template<typename T, std::size_t N>
inline void merge(T (&result)[N], const T (&vec1)[N], const T (&vec2)[N]);

//----------------------------------------
//二点間の差（減算）
//※ベクトルを返す
template<typename T, std::size_t N>
inline void difference(T (&result)[N], const T (&vec1)[N], const T (&vec2)[N]);

//----------------------------------------
//二点間の長さ
//※平方根適用
template<typename T, std::size_t N>
inline T length(const T (&vec1)[N], const T (&vec2)[N]);

//----------------------------------------
//二点間の長さの二乗
template<typename T, std::size_t N>
inline T lengthSq(const T (&vec1)[N], const T (&vec2)[N]);

//----------------------------------------
//正規化
//※ベクトルを返す
template<typename T, std::size_t N>
inline void normalize(T (&result)[N], const T (&vec)[N]);

//----------------------------------------
//スカラー倍
//※ベクトルを返す
template<typename T, std::size_t N>
inline void mul(T (&result)[N], const T (&vec)[N], const T scalar);

//----------------------------------------
//スカラー進行（スカラー長の加算）
//※ベクトルを返す
template<typename T, std::size_t N>
inline void forward(T(&result)[N], const T(&vec)[N], const T scalar);

//----------------------------------------
//内積
template<typename T, std::size_t N>
inline T dot(const T (&vec1)[N], const T (&vec2)[N]);

//----------------------------------------
//正規化して内積
template<typename T, std::size_t N>
inline T normalizedDot(const T (&vec1)[N], const T (&vec2)[N]);

//----------------------------------------
//外積
//※3D専用
//※ベクトルを返す
template<typename T, std::size_t N>
inline void cross(T (&result)[N], const T (&vec1)[N], const T (&vec2)[N]);

//--------------------------------------------------------------------------------
//テンプレート行列
//※fast_math.hをインクルードすることで、float×4×4の時に特殊化して、高速演算を利用可能
//--------------------------------------------------------------------------------

//----------
//行列の加算
template<typename T, std::size_t N, std::size_t M>
void add(T (&mat_result)[N][M], const T (&mat1)[N][M], const T (&mat2)[N][M]);
//※ループアンローリング版（コンパイル時のループ展開をより確実にするため、コードサイズが増えるが速くなる）
template<typename T, std::size_t N, std::size_t M>
void addLU(T (&mat_result)[N][M], const T (&mat1)[N][M], const T (&mat2)[N][M]);
//----------
//行列の減算
template<typename T, std::size_t N, std::size_t M>
void sub(T (&mat_result)[N][M], const T (&mat1)[N][M], const T (&mat2)[N][M]);
//※ループアンローリング版（コンパイル時のループ展開をより確実にするため、コードサイズが増えるが速くなる）
template<typename T, std::size_t N, std::size_t M>
void subLU(T(&mat_result)[N][M], const T(&mat1)[N][M], const T(&mat2)[N][M]);
//----------
//行列のスカラー倍
template<typename T, std::size_t N, std::size_t M>
void mul(T (&mat_result)[N][M], const T (&mat)[N][M], const T scalar);
//※ループアンローリング版（コンパイル時のループ展開をより確実にするため、コードサイズが増えるが速くなる）
template<typename T, std::size_t N, std::size_t M>
void mulLU(T(&mat_result)[N][M], const T(&mat)[N][M], const T scalar);
//----------
//行列の乗算
template<typename T, std::size_t N, std::size_t M, std::size_t NM>
void mul(T (&mat_result)[N][M], const T (&mat1)[N][NM], const T (&mat2)[NM][M]);
//※ループアンローリング版（コンパイル時のループ展開をより確実にするため、コードサイズが増えるが速くなる）
template<typename T, std::size_t N, std::size_t M, std::size_t NM>
void mulLU(T(&mat_result)[N][M], const T(&mat1)[N][NM], const T(&mat2)[NM][M]);

//--------------------------------------------------------------------------------
//基本演算
//--------------------------------------------------------------------------------

//演算方法指定用の型
struct wraparound_type{};//ラップアラウンド演算指定用構造体
struct saturation_type{};//飽和演算指定用構造体

//演算方法指定用の定数
extern const wraparound_type wraparound;//ラップアラウンド演算指定用
extern const saturation_type saturation;//飽和演算指定用

//演算
//※戻り値は演算後の値
template<typename T>
inline T inc(const T value);//インクリメント
template<typename T>
inline T inc(const T value, const wraparound_type&, const T max, const T min = GASHA_ numeric_limits<T>::zero());//インクリメント ※ラップアラウンド演算（wrap-around）：演算結果が範囲を超えたらループする
template<typename T>
inline T inc(const T value, const saturation_type&, const T max, const T min = GASHA_ numeric_limits<T>::zero());//インクリメント ※飽和演算（saturation）：演算結果が範囲を超えたら限界値を返す
template<typename T>
inline T dec(const T value);//デクリメント
template<typename T>
inline T dec(const T value, const wraparound_type&, const T max, const T min = GASHA_ numeric_limits<T>::zero());//デクリメント ※ラップアラウンド演算（wrap-around）：演算結果が範囲を超えたらループする
template<typename T>
inline T dec(const T value, const saturation_type&, const T max, const T min = GASHA_ numeric_limits<T>::zero());//デクリメント ※飽和演算（saturation）：演算結果が範囲を超えたら限界値を返す
template<typename T>
inline T add(const T lhs, const T rhs);//加算
template<typename T>
inline T add(const T lhs, const T rhs, const wraparound_type&, const T max, const T min = GASHA_ numeric_limits<T>::zero());//加算 ※ラップアラウンド演算（wrap-around）：演算結果が範囲を超えたらループする
template<typename T>
inline T add(const T lhs, const T rhs, const saturation_type&, const T max, const T min = GASHA_ numeric_limits<T>::zero());//加算 ※飽和演算（saturation）：演算結果が範囲を超えたら限界値を返す
template<typename T>
inline T sub(const T lhs, const T rhs);//減算
template<typename T>
inline T sub(const T lhs, const T rhs, const wraparound_type&, const T max, const T min = GASHA_ numeric_limits<T>::zero());//減算 ※ラップアラウンド演算（wrap-around）：演算結果が範囲を超えたらループする
template<typename T>
inline T sub(const T lhs, const T rhs, const saturation_type&, const T max, const T min = GASHA_ numeric_limits<T>::zero());//減算 ※飽和演算（saturation）：演算結果が範囲を超えたら限界値を返す
template<typename T>
inline T mul(const T lhs, const T rhs);//乗算
template<typename T>
inline T mul(const T lhs, const T rhs, const wraparound_type&, const T max, const T min = GASHA_ numeric_limits<T>::zero());//乗算 ※ラップアラウンド演算（wrap-around）：演算結果が範囲を超えたらループする
template<typename T>
inline T mul(const T lhs, const T rhs, const saturation_type&, const T max, const T min = GASHA_ numeric_limits<T>::zero());//乗算 ※飽和演算（saturation）：演算結果が範囲を超えたら限界値を返す
template<typename T>
inline T div(const T lhs, const T rhs);//除算
template<typename T>
inline T mod(const T lhs, const T rhs);//剰余算
template<typename T>
inline T bitAnd(const T lhs, const T rhs);//ビット論理積
template<typename T>
inline T bitOr(const T lhs, const T rhs);//ビット論理和
template<typename T>
inline T bitXor(const T lhs, const T rhs);//ビット排他的論理和
template<typename T>
inline T bitNot(const T value);//ビット反転
template<typename T>
inline T lShift(const T lhs, const int rhs);//左シフト
template<typename T>
inline T rShift(const T lhs, const int rhs);//右シフト
template<typename T>
inline T bitOn(const T lhs, const T rhs);//ビットオン（論理和）
template<typename T>
inline T bitOff(const T lhs, const T rhs);//ビットオフ（ビット反転して論理積）
//※ポリシー用
//※引数を統一化し、テンプレート引数にポリシーとして指定可能な式
template<typename T>
inline T inc_policy(const T lhs, const T rhs_dummy, const T max_dummy, const T min_dummy);//インクリメント
template<typename T>
inline T inc_wraparound_policy(const T lhs, const T rhs_dummy, const T max, const T min);//インクリメント ※ラップアラウンド演算（wrap-around）
template<typename T>
inline T inc_saturation_policy(const T lhs, const T rhs_dummy, const T max, const T min);//インクリメント ※飽和演算（saturation）
template<typename T>
inline T dec_policy(const T lhs, const T rhs_dummy, const T max_dummy, const T min_dummy);//デクリメント
template<typename T>
inline T dec_wraparound_policy(const T lhs, const T rhs_dummy, const T max, const T min);//デクリメント ※ラップアラウンド演算（wrap-around）
template<typename T>
inline T dec_saturation_policy(const T lhs, const T rhs_dummy, const T max, const T min);//デクリメント ※飽和演算（saturation）
template<typename T>
inline T add_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy);//加算
template<typename T>
inline T add_wraparound_policy(const T lhs, const T rhs, const T max, const T min);//加算 ※ラップアラウンド演算（wrap-around）
template<typename T>
inline T add_saturation_policy(const T lhs, const T rhs, const T max, const T min);//加算 ※飽和演算（saturation）
template<typename T>
inline T sub_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy);//減算
template<typename T>
inline T sub_wraparound_policy(const T lhs, const T rhs, const T max, const T min);//減算 ※ラップアラウンド演算（wrap-around）
template<typename T>
inline T sub_saturation_policy(const T lhs, const T rhs, const T max, const T min);//減算 ※飽和演算（saturation）
template<typename T>
inline T mul_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy);//乗算
template<typename T>
inline T mul_wraparound_policy(const T lhs, const T rhs, const T max, const T min);//乗算 ※ラップアラウンド演算（wrap-around）
template<typename T>
inline T mul_saturation_policy(const T lhs, const T rhs, const T max, const T min);//乗算 ※飽和演算（saturation）
template<typename T>
inline T div_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy);//除算
template<typename T>
inline T mod_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy);//剰余算
template<typename T>
inline T bitAnd_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy);//ビット論理積
template<typename T>
inline T bitOr_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy);//ビット論理和
template<typename T>
inline T bitXor_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy);//ビット排他的論理和
template<typename T>
inline T bitNot_policy(const T lhs, const T rhs_dummy, const T max_dummy, const T min_dummy);//ビット反転
template<typename T>
inline T lShift_policy(const T lhs, const int rhs, const T max_dummy, const T min_dummy);//左シフト
template<typename T>
inline T rShift_policy(const T lhs, const int rhs, const T max_dummy, const T min_dummy);//右シフト
template<typename T>
inline T bitOn_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy);//ビットオン（論理和）
template<typename T>
inline T bitOff_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy);//ビットオフ（ビット反転して論理積）
//比較
//※戻り値は判定結果の論理値
template<typename T>
inline bool eq(const T lhs, const T rhs);//比較 ==
template<typename T>
inline bool ne(const T lhs, const T rhs);//比較 !=
template<typename T>
inline bool gt(const T lhs, const T rhs);//比較 >
template<typename T>
inline bool ge(const T lhs, const T rhs);//比較 >=
template<typename T>
inline bool lt(const T lhs, const T rhs);//比較 <
template<typename T>
inline bool le(const T lhs, const T rhs);//比較 <=
template<typename T>
inline bool isOn(const T lhs, const T rhs);//ビットが立っているか？
template<typename T>
inline bool isOff(const T lhs, const T rhs);//ビットが立っているか？
template<typename T>
inline bool logicalAnd(const T lhs, const T rhs);//比較 
template<typename T>
inline bool logicalOr(const T lhs, const T rhs);//比較 ||
template<typename T>
inline bool isTrue(const T value);//真か？（0以外か？）
template<typename T>
inline bool isFalse(const T value);//偽か？（0か？）
//※ポリシー用
//※引数を統一化し、テンプレート引数にポリシーとして指定可能な式
template<typename T>
inline bool eq_policy(const T lhs, const T rhs);//比較 ==
template<typename T>
inline bool ne_policy(const T lhs, const T rhs);//比較 !=
template<typename T>
inline bool gt_policy(const T lhs, const T rhs);//比較 >
template<typename T>
inline bool ge_policy(const T lhs, const T rhs);//比較 >=
template<typename T>
inline bool lt_policy(const T lhs, const T rhs);//比較 <
template<typename T>
inline bool le_policy(const T lhs, const T rhs);//比較 <=
template<typename T>
inline bool isOn_policy(const T lhs, const T rhs);//ビットが立っているか？
template<typename T>
inline bool isOff_policy(const T lhs, const T rhs);//ビットが立っているか？
template<typename T>
inline bool logicalAnd_policy(const T lhs, const T rhs);//比較 &&
template<typename T>
inline bool logicalOr_policy(const T lhs, const T rhs);//比較 ||
template<typename T>
inline bool isTrue_policy(const T lhs, const T rhs_dummy);//真か？（0以外か？）
template<typename T>
inline bool isFalse_policy(const T lhs, const T rhs_dummy);//偽か？（0か？）

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/basic_math.inl>

#endif//GASHA_INCLUDED_BASIC_MATH_H

// End of file

#pragma once
#ifndef GASHA_INCLUDED_BASIC_MATH_INL
#define GASHA_INCLUDED_BASIC_MATH_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// basic_math.inl
// 基本算術【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/basic_math.h>//基本算術【宣言部】
#include <gasha/simple_assert.h>//シンプルアサーション

#include <cmath>//std::sqrt()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//べき乗
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】べき乗（power）
template<typename T, int E>
inline T pow(const T n)
{
	static_assert(E > 0, "E is only supported greater than 0.");
	return n * GASHA_ pow<T, E - 1>(n);
}

//----------------------------------------
//【ランタイム版】負の指数のべき乗（negative power）
template<typename T, int E>
inline T negaPow(const T n)
{
	static_assert(E <= 0, "E is only supported less than 0.");
	return static_cast<T>(1) / GASHA_ pow<T, -E>(n);
}

//--------------------------------------------------------------------------------
//素数計算
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】指定の値と同じか、それより小さい最初の素数を算出
inline unsigned int makePrimeLE(const unsigned int n)
{
	return isPrime(n) ? n : GASHA_ makePrimeLT(n);
}

//----------------------------------------
//【ランタイム版】指定の値と同じか、それより大きい最初の素数を算出
inline unsigned int makePrimeGE(const unsigned int n)
{
	return isPrime(n) ? n : GASHA_ makePrimeGT(n);
}

//--------------------------------------------------------------------------------
//ビット計算
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】指定の値以上（Greater than or Equal）で最も小さい2のべき乗を算出
inline unsigned int calcNearPow2GE(const unsigned int value)
{
	if (value == 0)
		return 0;
	const unsigned int msb = GASHA_ calcMSB(value - 1) + 1;
	return msb == 32 ? 0 : 1 << msb;
}

//----------------------------------------
//【ランタイム版】指定の値以下（Less than or Equal）で最も大きい2のべき乗を算出
inline unsigned int calcNearPow2LE(const unsigned int value)
{
	if (value == 0)
		return 0;
	const unsigned int msb = GASHA_ calcMSB(value);
	return msb == 32 ? 0 : 1 << msb;
}

//--------------------------------------------------------------------------------
//テンプレート平方根
//--------------------------------------------------------------------------------

//----------------------------------------
//平方根
template<typename T> inline T sqr(const T value){ return std::sqrt(value); }

//--------------------------------------------------------------------------------
//テンプレートベクトル演算
//--------------------------------------------------------------------------------

//----------------------------------------
//ノルム
template<typename T, std::size_t N>
inline T norm(const T(&vec)[N])
{
	T norm_sq = 0.f;
	for (int i = 0; i < N; ++i)
		norm_sq += vec[i] * vec[i];
	return GASHA_ sqr(norm_sq);
}

//----------------------------------------
//ノルムの二乗
template<typename T, std::size_t N>
inline T normSq(const T(&vec)[N])
{
	T norm_sq = 0.f;
	for (int i = 0; i < N; ++i)
		norm_sq += vec[i] * vec[i];
	return norm_sq;
}

//----------------------------------------
//合成（加算）
template<typename T, std::size_t N>
inline void merge(T(&result)[N], const T(&vec1)[N], const T(&vec2)[N])
{
	for (int i = 0; i < N; ++i)
		result[i] = vec1[i] + vec2[i];
}

//----------------------------------------
//二点間の差（減算）
template<typename T, std::size_t N>
inline void difference(T(&result)[N], const T(&vec1)[N], const T(&vec2)[N])
{
	for (int i = 0; i < N; ++i)
		result[i] = vec1[i] - vec2[i];
}

//----------------------------------------
//二点間の長さ
template<typename T, std::size_t N>
inline T length(const T(&vec1)[N], const T(&vec2)[N])
{
	T len_sq = 0.f;
	for (int i = 0; i < N; ++i)
	{
		const T diff = vec1[i] - vec2[i];
		len_sq += diff * diff;
	}
	return GASHA_ sqr(len_sq);
}

//----------------------------------------
//二点間の長さの二乗
template<typename T, std::size_t N>
inline T lengthSq(const T(&vec1)[N], const T(&vec2)[N])
{
	T len_sq = 0.f;
	for (int i = 0; i < N; ++i)
	{
		const T diff = vec1[i] - vec2[i];
		len_sq += diff * diff;
	}
	return len_sq;
}

//----------------------------------------
//正規化
template<typename T, std::size_t N>
inline void normalize(T(&result)[N], const T(&vec)[N])
{
	const T n = norm(vec);
	for (int i = 0; i < N; ++i)
		result[i] = vec[i] / n;
}

//----------------------------------------
//スカラー倍
template<typename T, std::size_t N>
inline void mul(T(&result)[N], const T(&vec)[N], const T scalar)
{
	for (int i = 0; i < N; ++i)
		result[i] = vec[i] * scalar;
}

//----------------------------------------
//スカラー進行（スカラー長の加算）
template<typename T, std::size_t N>
inline void forward(T(&result)[N], const T(&vec)[N], const T scalar)
{
	const T n = norm(vec);
	for (int i = 0; i < N; ++i)
		result[i] = vec[i] + vec[i] / n * scalar;
}

//----------------------------------------
//内積
template<typename T, std::size_t N>
inline T dot(const T(&vec1)[N], const T(&vec2)[N])
{
	T dot = 0.f;
	for (int i = 0; i < N; ++i)
		dot += (vec1[i] * vec2[i]);
	return dot;
}

//----------------------------------------
//正規化して内積
template<typename T, std::size_t N>
inline T normalizedDot(const T(&vec1)[N], const T(&vec2)[N])
{
	const T n1 = norm(vec1);
	const T n2 = norm(vec2);
	T dot = 0.f;
	for (int i = 0; i < N; ++i)
		dot += ((vec1[i] / n1) * (vec2[i] / n2));
	return dot;
}

//----------------------------------------
//外積
template<typename T, std::size_t N>
inline void cross(T(&result)[N], const T(&vec1)[N], const T(&vec2)[N])
{
	//static_assert(N == 3 || N == 4, "Cross-product is only available for 3D vector.");
	GASHA_SIMPLE_ASSERT(N == 3 || N == 4, "Cross-product is only available for 3D vector.");
	result[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
	result[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
	result[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
}

//--------------------------------------------------------------------------------
//テンプレート行列
//--------------------------------------------------------------------------------

//----------
//行列の加算
template<typename T, std::size_t N, std::size_t M>
void add(T(&mat_result)[N][M], const T (&mat1)[N][M], const T (&mat2)[N][M])
{
	for (int n = 0; n < N; ++n)
		for (int m = 0; m < M; ++m)
			mat_result[n][m] = mat1[n][m] + mat2[n][m];
}
//※ループアンローリング版
template<typename T, std::size_t N, std::size_t M, std::size_t NOW_N, std::size_t NOW_M>
struct _addRU{
	inline static void calc(T (&mat_result)[N][M], const T (&mat1)[N][M], const T (&mat2)[N][M])
	{
		mat_result[NOW_N][NOW_M] = mat1[NOW_N][NOW_M] + mat2[NOW_N][NOW_M];
		_addRU<T, N, M, NOW_N, NOW_M - 1>::calc(mat_result, mat1, mat2);
	}
};
template<typename T, std::size_t N, std::size_t M, std::size_t NOW_N>
struct _addRU<T, N, M, NOW_N, 0>{
	inline static void calc(T (&mat_result)[N][M], const T (&mat1)[N][M], const T (&mat2)[N][M])
	{
		mat_result[NOW_N][0] = mat1[NOW_N][0] + mat2[NOW_N][0];
		_addRU<T, N, M, NOW_N - 1, M - 1>::calc(mat_result, mat1, mat2);
	}
};
template<typename T, std::size_t N, std::size_t M>
struct _addRU<T, N, M, 0, 0>{
	inline static void calc(T (&mat_result)[N][M], const T (&mat1)[N][M], const T (&mat2)[N][M])
	{
		mat_result[0][0] = mat1[0][0] + mat2[0][0];
	}
};
template<typename T, std::size_t N, std::size_t M>
void addLU(T (&mat_result)[N][M], const T (&mat1)[N][M], const T (&mat2)[N][M])
{
	_addRU<T, N, M, N - 1, M - 1>::calc(mat_result, mat1, mat2);
}

//----------
//行列の減算
template<typename T, std::size_t N, std::size_t M>
void sub(T (&mat_result)[N][M], const T (&mat1)[N][M], const T (&mat2)[N][M])
{
	for (int n = 0; n < N; ++n)
		for (int m = 0; m < M; ++m)
			mat_result[n][m] = mat1[n][m] - mat2[n][m];
}
//※ループアンローリング版
template<typename T, std::size_t N, std::size_t M, std::size_t NOW_N, std::size_t NOW_M>
struct _subRU{
	inline static void calc(T(&mat_result)[N][M], const T(&mat1)[N][M], const T(&mat2)[N][M])
	{
		mat_result[NOW_N][NOW_M] = mat1[NOW_N][NOW_M] - mat2[NOW_N][NOW_M];
		_subRU<T, N, M, NOW_N, NOW_M - 1>::calc(mat_result, mat1, mat2);
	}
};
template<typename T, std::size_t N, std::size_t M, std::size_t NOW_N>
struct _subRU<T, N, M, NOW_N, 0>{
	inline static void calc(T(&mat_result)[N][M], const T(&mat1)[N][M], const T(&mat2)[N][M])
	{
		mat_result[NOW_N][0] = mat1[NOW_N][0] - mat2[NOW_N][0];
		_subRU<T, N, M, NOW_N - 1, M - 1>::calc(mat_result, mat1, mat2);
	}
};
template<typename T, std::size_t N, std::size_t M>
struct _subRU<T, N, M, 0, 0>{
	inline static void calc(T(&mat_result)[N][M], const T(&mat1)[N][M], const T(&mat2)[N][M])
	{
		mat_result[0][0] = mat1[0][0] - mat2[0][0];
	}
};
template<typename T, std::size_t N, std::size_t M>
void subLU(T(&mat_result)[N][M], const T(&mat1)[N][M], const T(&mat2)[N][M])
{
	_subRU<T, N, M, N - 1, M - 1>::calc(mat_result, mat1, mat2);
}

//----------
//行列のスカラー倍
template<typename T, std::size_t N, std::size_t M>
void mul(T (&mat_result)[N][M], const T (&mat)[N][M], const T scalar)
{
	for (int n = 0; n < N; ++n)
		for (int m = 0; m < M; ++m)
			mat_result[n][m] = mat[n][m] * scalar;
}
//※ループアンローリング版
template<typename T, std::size_t N, std::size_t M, std::size_t NOW_N, std::size_t NOW_M>
struct _mulRU1{
	inline static void calc(T (&mat_result)[N][M], const T (&mat)[N][M], const T scalar)
	{
		mat_result[NOW_N][NOW_M] = mat[NOW_N][NOW_M] * scalar;
		_mulRU1<T, N, M, NOW_N, NOW_M - 1>::calc(mat_result, mat, scalar);
	}
};
template<typename T, std::size_t N, std::size_t M, std::size_t NOW_N>
struct _mulRU1<T, N, M, NOW_N, 0>{
	inline static void calc(T (&mat_result)[N][M], const T (&mat)[N][M], const T scalar)
	{
		mat_result[NOW_N][0] = mat[NOW_N][0] * scalar;
		_mulRU1<T, N, M, NOW_N - 1, M - 1>::calc(mat_result, mat, scalar);
	}
};
template <typename T, std::size_t N, std::size_t M>
struct _mulRU1<T, N, M, 0, 0>{
	inline static void calc(T (&mat_result)[N][M], const T (&mat)[N][M], const T scalar)
	{
		mat_result[0][0] = mat[0][0] * scalar;
	}
};
template<typename T, std::size_t N, std::size_t M>
void mulLU(T(&mat_result)[N][M], const T (&mat)[N][M], const T scalar)
{
	_mulRU1<T, N, M, N - 1, M - 1>::calc(mat_result, mat, scalar);
}

//----------
//行列の乗算
template<typename T, std::size_t N, std::size_t M, std::size_t NM>
void mul(T (&mat_result)[N][M], const T (&mat1)[N][NM], const T(&mat2)[NM][M])
{
	for (int n = 0; n < N; ++n)
		for (int m = 0; m < M; ++m)
			mat_result[n][m] = static_cast<T>(0);
	for (int n = 0; n < N; ++n)
		for (int m = 0; m < M; ++m)
			for (int nm = 0; nm < NM; ++nm)
				mat_result[n][m] += (mat1[n][nm] * mat2[nm][m]);
}
//※ループアンローリング版
template<typename T, std::size_t N, std::size_t M, std::size_t NM, std::size_t NOW_N, std::size_t NOW_M, std::size_t NOW_NM>
struct _mulRU2{
	inline static void calc(T (&mat_result)[N][M], const T (&mat1)[N][NM], const T (&mat2)[NM][M])
	{
		mat_result[NOW_N][NOW_M] += (mat1[NOW_N][NOW_NM] * mat2[NOW_NM][NOW_M]);
		_mulRU2<T, N, M, NM, NOW_N, NOW_M, NOW_NM - 1>::calc(mat_result, mat1, mat2);
	}
};
template<typename T, std::size_t N, std::size_t M, std::size_t NM, std::size_t NOW_N, std::size_t NOW_M>
struct _mulRU2<T, N, M, NM, NOW_N, NOW_M, 0>{
	inline static void calc(T (&mat_result)[N][M], const T (&mat1)[N][NM], const T (&mat2)[NM][M])
	{
		mat_result[NOW_N][NOW_M] += (mat1[NOW_N][0] * mat2[0][NOW_M]);
		_mulRU2<T, N, M, NM, NOW_N, NOW_M - 1, NM - 1>::calc(mat_result, mat1, mat2);
	}
};
template<typename T, std::size_t N, std::size_t M, std::size_t NM, std::size_t NOW_N>
struct _mulRU2<T, N, M, NM, NOW_N, 0, 0>{
	inline static void calc(T (&mat_result)[N][M], const T (&mat1)[N][NM], const T (&mat2)[NM][M])
	{
		mat_result[NOW_N][0] += (mat1[NOW_N][0] * mat2[0][0]);
		_mulRU2<T, N, M, NM, NOW_N - 1, M - 1, NM - 1>::calc(mat_result, mat1, mat2);
	}
};
template<typename T, std::size_t N, std::size_t M, std::size_t NM>
struct _mulRU2<T, N, M, NM, 0, 0, 0>{
	inline static void calc(T (&mat_result)[N][M], const T (&mat1)[N][NM], const T (&mat2)[NM][M])
	{
		mat_result[0][0] += (mat1[0][0] * mat2[0][0]);
	}
};
template<typename T, std::size_t N, std::size_t M, std::size_t NM>
void mulLU(T(&mat_result)[N][M], const T(&mat1)[N][NM], const T(&mat2)[NM][M])
{
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
			mat_result[i][j] = static_cast<T>(0);
	_mulRU2<T, N, M, NM, N - 1, M - 1, NM - 1>::calc(mat_result, mat1, mat2);
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_BASIC_MATH_INL

// End of file

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
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/basic_math.h>//基本算術【宣言部】
#include <gasha/simple_assert.h>//シンプルアサーション

#include <cmath>//std::sqrt()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//符号判定
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】符号判定（sign）
template<typename T>
inline constexpr int sign(const T value)
{
	return
		value > 0 ? 1 :
		value < 0 ? -1 :
		0;
}
template<>
inline constexpr int sign<float>(const float value)
{
	return
		value > 0.f ? 1 :
		value < 0.f ? -1 :
		0;
}
template<>
inline constexpr int sign<double>(const double value)
{
	return
		value > 0. ? 1 :
		value < 0. ? -1 :
		0;
}
template<>
inline constexpr int sign<bool>(const bool value)
{
	return
		value ? 1 :
		0;
}

//--------------------------------------------------------------------------------
//範囲計算
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】範囲計算（range）
template<typename T>
inline constexpr T range(const T max, const T min)
{
	return max - min + static_cast<T>(1);
}
template<>
inline constexpr float range<float>(const float max, const float min)
{
	return max - min;
}
template<>
inline constexpr double range<double>(const double max, const double min)
{
	return max - min;
}
template<>
inline constexpr bool range<bool>(const bool max, const bool min)
{
	return !(max == min);
}

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
	for (std::size_t i = 0; i < N; ++i)
		norm_sq += vec[i] * vec[i];
	return GASHA_ sqr(norm_sq);
}

//----------------------------------------
//ノルムの二乗
template<typename T, std::size_t N>
inline T normSq(const T(&vec)[N])
{
	T norm_sq = 0.f;
	for (std::size_t i = 0; i < N; ++i)
		norm_sq += vec[i] * vec[i];
	return norm_sq;
}

//----------------------------------------
//合成（加算）
template<typename T, std::size_t N>
inline void merge(T(&result)[N], const T(&vec1)[N], const T(&vec2)[N])
{
	for (std::size_t i = 0; i < N; ++i)
		result[i] = vec1[i] + vec2[i];
}

//----------------------------------------
//二点間の差（減算）
template<typename T, std::size_t N>
inline void difference(T(&result)[N], const T(&vec1)[N], const T(&vec2)[N])
{
	for (std::size_t i = 0; i < N; ++i)
		result[i] = vec1[i] - vec2[i];
}

//----------------------------------------
//二点間の長さ
template<typename T, std::size_t N>
inline T length(const T(&vec1)[N], const T(&vec2)[N])
{
	T len_sq = 0.f;
	for (std::size_t i = 0; i < N; ++i)
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
	for (std::size_t i = 0; i < N; ++i)
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
	for (std::size_t i = 0; i < N; ++i)
		result[i] = vec[i] / n;
}

//----------------------------------------
//スカラー倍
template<typename T, std::size_t N>
inline void mul(T(&result)[N], const T(&vec)[N], const T scalar)
{
	for (std::size_t i = 0; i < N; ++i)
		result[i] = vec[i] * scalar;
}

//----------------------------------------
//スカラー進行（スカラー長の加算）
template<typename T, std::size_t N>
inline void forward(T(&result)[N], const T(&vec)[N], const T scalar)
{
	const T n = norm(vec);
	for (std::size_t i = 0; i < N; ++i)
		result[i] = vec[i] + vec[i] / n * scalar;
}

//----------------------------------------
//内積
template<typename T, std::size_t N>
inline T dot(const T(&vec1)[N], const T(&vec2)[N])
{
	T dot = 0.f;
	for (std::size_t i = 0; i < N; ++i)
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
	for (std::size_t i = 0; i < N; ++i)
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
	for (std::size_t n = 0; n < N; ++n)
		for (std::size_t m = 0; m < M; ++m)
			mat_result[n][m] = mat1[n][m] + mat2[n][m];
}
//※ループアンローリング版
namespace _private
{
	template<typename T, std::size_t N, std::size_t M, std::size_t NOW_N, std::size_t NOW_M>
	struct _addLU{
		inline static void calc(T(&mat_result)[N][M], const T(&mat1)[N][M], const T(&mat2)[N][M])
		{
			mat_result[NOW_N][NOW_M] = mat1[NOW_N][NOW_M] + mat2[NOW_N][NOW_M];
			_addLU<T, N, M, NOW_N, NOW_M - 1>::calc(mat_result, mat1, mat2);
		}
	};
	template<typename T, std::size_t N, std::size_t M, std::size_t NOW_N>
	struct _addLU<T, N, M, NOW_N, 0>{
		inline static void calc(T(&mat_result)[N][M], const T(&mat1)[N][M], const T(&mat2)[N][M])
		{
			mat_result[NOW_N][0] = mat1[NOW_N][0] + mat2[NOW_N][0];
			_addLU<T, N, M, NOW_N - 1, M - 1>::calc(mat_result, mat1, mat2);
		}
	};
	template<typename T, std::size_t N, std::size_t M>
	struct _addLU<T, N, M, 0, 0>{
		inline static void calc(T(&mat_result)[N][M], const T(&mat1)[N][M], const T(&mat2)[N][M])
		{
			mat_result[0][0] = mat1[0][0] + mat2[0][0];
		}
	};
}
template<typename T, std::size_t N, std::size_t M>
void addLU(T (&mat_result)[N][M], const T (&mat1)[N][M], const T (&mat2)[N][M])
{
	_private::_addLU<T, N, M, N - 1, M - 1>::calc(mat_result, mat1, mat2);
}

//----------
//行列の減算
template<typename T, std::size_t N, std::size_t M>
void sub(T (&mat_result)[N][M], const T (&mat1)[N][M], const T (&mat2)[N][M])
{
	for (std::size_t n = 0; n < N; ++n)
		for (std::size_t m = 0; m < M; ++m)
			mat_result[n][m] = mat1[n][m] - mat2[n][m];
}
//※ループアンローリング版
namespace _private
{
	template<typename T, std::size_t N, std::size_t M, std::size_t NOW_N, std::size_t NOW_M>
	struct _subLU{
		inline static void calc(T(&mat_result)[N][M], const T(&mat1)[N][M], const T(&mat2)[N][M])
		{
			mat_result[NOW_N][NOW_M] = mat1[NOW_N][NOW_M] - mat2[NOW_N][NOW_M];
			_subLU<T, N, M, NOW_N, NOW_M - 1>::calc(mat_result, mat1, mat2);
		}
	};
	template<typename T, std::size_t N, std::size_t M, std::size_t NOW_N>
	struct _subLU<T, N, M, NOW_N, 0>{
		inline static void calc(T(&mat_result)[N][M], const T(&mat1)[N][M], const T(&mat2)[N][M])
		{
			mat_result[NOW_N][0] = mat1[NOW_N][0] - mat2[NOW_N][0];
			_subLU<T, N, M, NOW_N - 1, M - 1>::calc(mat_result, mat1, mat2);
		}
	};
	template<typename T, std::size_t N, std::size_t M>
	struct _subLU<T, N, M, 0, 0>{
		inline static void calc(T(&mat_result)[N][M], const T(&mat1)[N][M], const T(&mat2)[N][M])
		{
			mat_result[0][0] = mat1[0][0] - mat2[0][0];
		}
	};
}
template<typename T, std::size_t N, std::size_t M>
void subLU(T(&mat_result)[N][M], const T(&mat1)[N][M], const T(&mat2)[N][M])
{
	_private::_subLU<T, N, M, N - 1, M - 1>::calc(mat_result, mat1, mat2);
}

//----------
//行列のスカラー倍
template<typename T, std::size_t N, std::size_t M>
void mul(T (&mat_result)[N][M], const T (&mat)[N][M], const T scalar)
{
	for (std::size_t n = 0; n < N; ++n)
		for (std::size_t m = 0; m < M; ++m)
			mat_result[n][m] = mat[n][m] * scalar;
}
//※ループアンローリング版
namespace _private
{
	template<typename T, std::size_t N, std::size_t M, std::size_t NOW_N, std::size_t NOW_M>
	struct _mulLU1{
		inline static void calc(T(&mat_result)[N][M], const T(&mat)[N][M], const T scalar)
		{
			mat_result[NOW_N][NOW_M] = mat[NOW_N][NOW_M] * scalar;
			_mulLU1<T, N, M, NOW_N, NOW_M - 1>::calc(mat_result, mat, scalar);
		}
	};
	template<typename T, std::size_t N, std::size_t M, std::size_t NOW_N>
	struct _mulLU1<T, N, M, NOW_N, 0>{
		inline static void calc(T(&mat_result)[N][M], const T(&mat)[N][M], const T scalar)
		{
			mat_result[NOW_N][0] = mat[NOW_N][0] * scalar;
			_mulLU1<T, N, M, NOW_N - 1, M - 1>::calc(mat_result, mat, scalar);
		}
	};
	template <typename T, std::size_t N, std::size_t M>
	struct _mulLU1<T, N, M, 0, 0>{
		inline static void calc(T(&mat_result)[N][M], const T(&mat)[N][M], const T scalar)
		{
			mat_result[0][0] = mat[0][0] * scalar;
		}
	};
}
template<typename T, std::size_t N, std::size_t M>
void mulLU(T(&mat_result)[N][M], const T (&mat)[N][M], const T scalar)
{
	_private::_mulLU1<T, N, M, N - 1, M - 1>::calc(mat_result, mat, scalar);
}

//----------
//行列の乗算
template<typename T, std::size_t N, std::size_t M, std::size_t NM>
void mul(T (&mat_result)[N][M], const T (&mat1)[N][NM], const T(&mat2)[NM][M])
{
	for (std::size_t n = 0; n < N; ++n)
		for (std::size_t m = 0; m < M; ++m)
			mat_result[n][m] = static_cast<T>(0);
	for (std::size_t n = 0; n < N; ++n)
		for (std::size_t m = 0; m < M; ++m)
			for (std::size_t nm = 0; nm < NM; ++nm)
				mat_result[n][m] += (mat1[n][nm] * mat2[nm][m]);
}
//※ループアンローリング版
namespace _private
{
	template<typename T, std::size_t N, std::size_t M, std::size_t NM, std::size_t NOW_N, std::size_t NOW_M, std::size_t NOW_NM>
	struct _mulLU2{
		inline static void calc(T(&mat_result)[N][M], const T(&mat1)[N][NM], const T(&mat2)[NM][M])
		{
			mat_result[NOW_N][NOW_M] += (mat1[NOW_N][NOW_NM] * mat2[NOW_NM][NOW_M]);
			_mulLU2<T, N, M, NM, NOW_N, NOW_M, NOW_NM - 1>::calc(mat_result, mat1, mat2);
		}
	};
	template<typename T, std::size_t N, std::size_t M, std::size_t NM, std::size_t NOW_N, std::size_t NOW_M>
	struct _mulLU2<T, N, M, NM, NOW_N, NOW_M, 0>{
		inline static void calc(T(&mat_result)[N][M], const T(&mat1)[N][NM], const T(&mat2)[NM][M])
		{
			mat_result[NOW_N][NOW_M] += (mat1[NOW_N][0] * mat2[0][NOW_M]);
			_mulLU2<T, N, M, NM, NOW_N, NOW_M - 1, NM - 1>::calc(mat_result, mat1, mat2);
		}
	};
	template<typename T, std::size_t N, std::size_t M, std::size_t NM, std::size_t NOW_N>
	struct _mulLU2<T, N, M, NM, NOW_N, 0, 0>{
		inline static void calc(T(&mat_result)[N][M], const T(&mat1)[N][NM], const T(&mat2)[NM][M])
		{
			mat_result[NOW_N][0] += (mat1[NOW_N][0] * mat2[0][0]);
			_mulLU2<T, N, M, NM, NOW_N - 1, M - 1, NM - 1>::calc(mat_result, mat1, mat2);
		}
	};
	template<typename T, std::size_t N, std::size_t M, std::size_t NM>
	struct _mulLU2<T, N, M, NM, 0, 0, 0>{
		inline static void calc(T(&mat_result)[N][M], const T(&mat1)[N][NM], const T(&mat2)[NM][M])
		{
			mat_result[0][0] += (mat1[0][0] * mat2[0][0]);
		}
	};
}
template<typename T, std::size_t N, std::size_t M, std::size_t NM>
void mulLU(T(&mat_result)[N][M], const T(&mat1)[N][NM], const T(&mat2)[NM][M])
{
	for (std::size_t i = 0; i < N; ++i)
		for (std::size_t j = 0; j < M; ++j)
			mat_result[i][j] = static_cast<T>(0);
	_private::_mulLU2<T, N, M, NM, N - 1, M - 1, NM - 1>::calc(mat_result, mat1, mat2);
}

//--------------------------------------------------------------------------------
//基本演算
//--------------------------------------------------------------------------------

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4068)//【VC++】C4068を抑える
//#pragma GCC diagnostic push//【GCC】ワーニング設定を退避 ※問題箇所だけワーニングを抑えて復元しても、結局ワーニングが出るのでpush/popしない
#pragma GCC diagnostic ignored "-Wstrict-overflow"//【GCC】"strict-overflow"を抑える

//演算
template<typename T>
inline T inc(const T value)
{
	T new_value = value;
	++new_value;
	return new_value;
}
template<typename T>
inline T inc(const T value, const wraparound_type&, const T max, const T min)
{
	T new_value = value;
	if (new_value < max)
		++new_value;
	if (new_value > max)
		new_value = min;
	return new_value;
}
template<typename T>
inline T inc(const T value, const saturation_type&, const T max, const T min)
{
	T new_value = value;
	if (new_value < max)
		++new_value;
	if (new_value > max)
		new_value = max;
	return new_value;
}
template<typename T>
inline T dec(const T value)
{
	T new_value = value;
	--new_value;
	return new_value;
}
template<typename T>
inline T dec(const T value, const wraparound_type&, const T max, const T min)
{
	T new_value = value;
	if (new_value > min)
		--new_value;
	if (new_value < min)
		new_value = max;
	return new_value;
}
template<typename T>
inline T dec(const T value, const saturation_type&, const T max, const T min)
{
	T new_value = value;
	if (new_value > min)
		--new_value;
	if (new_value < min)
		new_value = min;
	return new_value;
}
template<typename T>
inline T add(const T lhs, const T rhs)
{
	T new_value = lhs;
	new_value += rhs;
	return new_value;
}
template<typename T>
inline T add(const T lhs, const T rhs, const wraparound_type&, const T max, const T min)
{
	T new_value = lhs;
	const T value_range = range<T>(max, min);
	new_value += rhs;
	if (value_range > GASHA_ numeric_limits<T>::zero())
	{
		while (rhs > GASHA_ numeric_limits<T>::zero() && new_value < min)
			new_value -= value_range;
		while (rhs < GASHA_ numeric_limits<T>::zero() && new_value > max)
			new_value += value_range;
		while (new_value > max)
			new_value -= value_range;
		while (new_value < min)
			new_value += value_range;
	}
	else
		new_value = min;
	return new_value;
}
template<typename T>
inline T add(const T lhs, const T rhs, const saturation_type&, const T max, const T min)
{
	T new_value = lhs;
	new_value += rhs;
	if (new_value > max || (new_value < lhs && rhs > GASHA_ numeric_limits<T>::zero()))
		new_value = max;
	if (new_value < min || (new_value > lhs && rhs < GASHA_ numeric_limits<T>::zero()))
		new_value = min;
	return new_value;
}
template<typename T>
inline T sub(const T lhs, const T rhs)
{
	T new_value = lhs;
	new_value -= rhs;
	return new_value;
}
template<typename T>
inline T sub(const T lhs, const T rhs, const wraparound_type&, const T max, const T min)
{
	T new_value = lhs;
	const T value_range = range<T>(max, min);
	new_value -= rhs;
	if (value_range > GASHA_ numeric_limits<T>::zero())
	{
		while (rhs > GASHA_ numeric_limits<T>::zero() && new_value > max)
			new_value += value_range;
		while (rhs < GASHA_ numeric_limits<T>::zero() && new_value < min)
			new_value -= value_range;
		while (new_value > max)
			new_value -= value_range;
		while (new_value < min)
			new_value += value_range;
	}
	else
		new_value = min;
	return new_value;
}
template<typename T>
inline T sub(const T lhs, const T rhs, const saturation_type&, const T max, const T min)
{
	T new_value = lhs;
	new_value -= rhs;
	if (new_value > max || (new_value < lhs && rhs < GASHA_ numeric_limits<T>::zero()))
		new_value = max;
	if (new_value < min || (new_value > lhs && rhs > GASHA_ numeric_limits<T>::zero()))
		new_value = min;
	return new_value;
}
template<typename T>
inline T mul(const T lhs, const T rhs)
{
	T new_value = lhs;
	new_value *= rhs;
	return new_value;
}
template<typename T>
inline T mul(const T lhs, const T rhs, const wraparound_type&, const T max, const T min)
{
	T new_value = lhs;
	const T value_range = range<T>(max, min);
	new_value *= rhs;
	if (value_range > GASHA_ numeric_limits<T>::zero())
	{
		while (sign<T>(lhs) == sign<T>(rhs) && new_value < min)
			new_value -= value_range;
		while (sign<T>(lhs) != sign<T>(rhs) && new_value > max)
			new_value += value_range;
		while (new_value > max)
			new_value -= value_range;
		while (new_value < min)
			new_value += value_range;
	}
	else
		new_value = min;
	return new_value;
}
template<typename T>
inline T mul(const T lhs, const T rhs, const saturation_type&, const T max, const T min)
{
	T new_value = lhs;
	new_value *= rhs;
	if (new_value > max || (new_value < lhs && sign<T>(lhs) == sign<T>(rhs)))
		new_value = max;
	if (new_value < min || (new_value > lhs && sign<T>(lhs) != sign<T>(rhs)))
		new_value = min;
	return new_value;
}
template<typename T>
inline T div(const T lhs, const T rhs)
{
	T new_value = lhs;
	new_value /= rhs;
	return new_value;
}
template<typename T>
inline T mod(const T lhs, const T rhs)
{
	T new_value = lhs;
	new_value %= rhs;
	return new_value;
}
template<typename T>
inline T bitAnd(const T lhs, const T rhs)
{
	T new_value = lhs;
	new_value &= rhs;
	return new_value;
}
template<typename T>
inline T bitOr(const T lhs, const T rhs)
{
	T new_value = lhs;
	new_value |= rhs;
	return new_value;
}
template<typename T>
inline T bitXor(const T lhs, const T rhs)
{
	T new_value = lhs;
	new_value ^= rhs;
	return new_value;
}
template<typename T>
inline T bitNot(const T value)
{
	const T new_value = ~value;
	return new_value;
}
template<typename T>
inline T lShift(const T lhs, const int rhs)
{
	T new_value = lhs;
	new_value <<= rhs;
	return new_value;
}
template<typename T>
inline T rShift(const T lhs, const int rhs)
{
	T new_value = lhs;
	new_value >>= rhs;
	return new_value;
}
template<typename T>
inline T bitOn(const T lhs, const T rhs)
{
	T new_value = lhs;
	new_value |= rhs;
	return new_value;
}
template<typename T>
inline T bitOff(const T lhs, const T rhs)
{
	T new_value = lhs;
	new_value &= (~rhs);
	return new_value;
}

//#pragma GCC diagnostic pop//【GCC】ワーニング設定を復元
#pragma warning(pop)//【VC++】ワーニング設定を復元

//※ポリシー用
template<typename T>
inline T inc_policy(const T lhs, const T rhs_dummy, const T max_dummy, const T min_dummy)
{
	return inc<T>(lhs);
}
template<typename T>
inline T inc_wraparound_policy(const T lhs, const T rhs_dummy, const T max, const T min)
{
	return inc<T>(lhs, wraparound, max, min);
}
template<typename T>
inline T inc_saturation_policy(const T lhs, const T rhs_dummy, const T max, const T min)
{
	return inc<T>(lhs, saturation, max, min);
}
template<typename T>
inline T dec_policy(const T lhs, const T rhs_dummy, const T max_dummy, const T min_dummy)
{
	return dec<T>(lhs);
}
template<typename T>
inline T dec_wraparound_policy(const T lhs, const T rhs_dummy, const T max, const T min)
{
	return dec<T>(lhs, wraparound, max, min);
}
template<typename T>
inline T dec_saturation_policy(const T lhs, const T rhs_dummy, const T max, const T min)
{
	return dec<T>(lhs, saturation, max, min);
}
template<typename T>
inline T add_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy)
{
	return add<T>(lhs, rhs);
}
template<typename T>
inline T add_wraparound_policy(const T lhs, const T rhs, const T max, const T min)
{
	return add<T>(lhs, rhs, wraparound, max, min);
}
template<typename T>
inline T add_saturation_policy(const T lhs, const T rhs, const T max, const T min)
{
	return add<T>(lhs, rhs, saturation, max, min);
}
template<typename T>
inline T sub_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy)
{
	return sub<T>(lhs, rhs);
}
template<typename T>
inline T sub_wraparound_policy(const T lhs, const T rhs, const T max, const T min)
{
	return sub<T>(lhs, rhs, wraparound, max, min);
}
template<typename T>
inline T sub_saturation_policy(const T lhs, const T rhs, const T max, const T min)
{
	return sub<T>(lhs, rhs, saturation, max, min);
}
template<typename T>
inline T mul_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy)
{
	return mul<T>(lhs, rhs);
}
template<typename T>
inline T mul_wraparound_policy(const T lhs, const T rhs, const T max, const T min)
{
	return mul<T>(lhs, rhs, wraparound, max, min);
}
template<typename T>
inline T mul_saturation_policy(const T lhs, const T rhs, const T max, const T min)
{
	return mul<T>(lhs, rhs, saturation, max, min);
}
template<typename T>
inline T div_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy)
{
	return div<T>(lhs, rhs);
}
template<typename T>
inline T mod_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy)
{
	return mod<T>(lhs, rhs);
}
template<typename T>
inline T bitAnd_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy)
{
	return bitAnd<T>(lhs, rhs);
}
template<typename T>
inline T bitOr_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy)
{
	return bitOr<T>(lhs, rhs);
}
template<typename T>
inline T bitXor_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy)
{
	return bitXor<T>(lhs, rhs);
}
template<typename T>
inline T bitNot_policy(const T lhs, const T rhs_dummy, const T max_dummy, const T min_dummy)
{
	return bitNot<T>(lhs);
}
template<typename T>
inline T lShift_policy(const T lhs, const int rhs, const T max_dummy, const T min_dummy)
{
	return lShift<T>(lhs, rhs);
}
template<typename T>
inline T rShift_policy(const T lhs, const int rhs, const T max_dummy, const T min_dummy)
{
	return rShift<T>(lhs, rhs);
}
template<typename T>
inline T bitOn_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy)
{
	return bitOn<T>(lhs, rhs);
}
template<typename T>
inline T bitOff_policy(const T lhs, const T rhs, const T max_dummy, const T min_dummy)
{
	return bitOff<T>(lhs, rhs);
}

//比較
template<typename T>
inline bool eq(const T lhs, const T rhs)
{
	return lhs == rhs;
}
template<typename T>
inline bool ne(const T lhs, const T rhs)
{
	return lhs != rhs;
}
template<typename T>
inline bool gt(const T lhs, const T rhs)
{
	return lhs > rhs;
}
template<typename T>
inline bool ge(const T lhs, const T rhs)
{
	return lhs >= rhs;
}
template<typename T>
inline bool lt(const T lhs, const T rhs)
{
	return lhs < rhs;
}
template<typename T>
inline bool le(const T lhs, const T rhs)
{
	return lhs <= rhs;
}
template<typename T>
inline bool isOn(const T lhs, const T rhs)
{
	return (lhs & rhs) != GASHA_ numeric_limits<T>::zero();
}
template<typename T>
inline bool isOff(const T lhs, const T rhs)
{
	return (lhs & rhs) == GASHA_ numeric_limits<T>::zero();
}
template<typename T>
inline bool logicalAnd(const T lhs, const T rhs)
{
	return lhs && rhs;
}
template<typename T>
inline bool logicalOr(const T lhs, const T rhs)
{
	return lhs || rhs;
}
template<typename T>
inline bool isTrue(const T value)
{
	return value != GASHA_ numeric_limits<T>::zero();
}
template<typename T>
inline bool isFalse(const T value)
{
	return value == GASHA_ numeric_limits<T>::zero();
}
//※ポリシー用
template<typename T>
inline bool eq_policy(const T lhs, const T rhs)
{
	return eq(lhs, rhs);
}
template<typename T>
inline bool ne_policy(const T lhs, const T rhs)
{
	return ne(lhs, rhs);
}
template<typename T>
inline bool gt_policy(const T lhs, const T rhs)
{
	return gt(lhs, rhs);
}
template<typename T>
inline bool ge_policy(const T lhs, const T rhs)
{
	return ge(lhs, rhs);
}
template<typename T>
inline bool lt_policy(const T lhs, const T rhs)
{
	return lt(lhs, rhs);
}
template<typename T>
inline bool le_policy(const T lhs, const T rhs)
{
	return le(lhs, rhs);
}
template<typename T>
inline bool isOn_policy(const T lhs, const T rhs)
{
	return isOn(lhs, rhs);
}
template<typename T>
inline bool isOff_policy(const T lhs, const T rhs)
{
	return isOff(lhs, rhs);
}
template<typename T>
inline bool logicalAnd_policy(const T lhs, const T rhs)
{
	return logicalAnd(lhs, rhs);
}
template<typename T>
inline bool logicalOr_policy(const T lhs, const T rhs)
{
	return logicalOr(lhs, rhs);
}
template<typename T>
inline bool isTrue_policy(const T lhs, const T rhs_dummy)
{
	return isTrue(lhs);
}
template<typename T>
inline bool isFalse_policy(const T lhs, const T rhs_dummy)
{
	return isFalse(lhs);
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_BASIC_MATH_INL

// End of file

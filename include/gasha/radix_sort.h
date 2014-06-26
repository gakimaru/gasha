#pragma once
#ifndef __RADIX_SORT_H_
#define __RADIX_SORT_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// radix_sort.h
// 基数ソート
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/sort_basic.h>//ソート処理基本

#include <climits>//***_MAX
#include <type_traits>//C++11 std:is_same, std::is_signed, std::make_unsigned, std::conditional, std::integral_constant
#include <malloc.h>//_aligned_malloc(), _aligned_free()

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

//キーの最大値取得
template<typename KEY_TYPE>
struct _radix_key{
	typedef
		typename std::conditional <
			std::is_same<KEY_TYPE, unsigned char>::value,
			std::integral_constant<unsigned char, UCHAR_MAX>,
			typename std::conditional <
				std::is_same<KEY_TYPE, signed char>::value,
				std::integral_constant<signed char, SCHAR_MAX>,
				typename std::conditional <
					std::is_same<KEY_TYPE, unsigned short>::value,
					std::integral_constant<unsigned short, USHRT_MAX>,
					typename std::conditional <
						std::is_same<KEY_TYPE, signed short>::value,
						std::integral_constant<signed short, SHRT_MAX>,
						typename std::conditional <
							std::is_same<KEY_TYPE, unsigned int>::value,
							std::integral_constant<unsigned int, UINT_MAX>,
							typename std::conditional <
								std::is_same<KEY_TYPE, signed int>::value,
								std::integral_constant<signed int, INT_MAX>,
								typename std::conditional <
									std::is_same<KEY_TYPE, unsigned long>::value,
									std::integral_constant<unsigned long, ULONG_MAX>,
									typename std::conditional <
										std::is_same<KEY_TYPE, signed long>::value,
										std::integral_constant<signed long, LONG_MAX>,
										typename std::conditional <
											std::is_same<KEY_TYPE, unsigned long long>::value,
											std::integral_constant<unsigned long long, ULLONG_MAX>,
											typename std::conditional <
												std::is_same<KEY_TYPE, signed long long>::value,
												std::integral_constant<signed long long, LLONG_MAX>,
												std::integral_constant<std::size_t, UINT_MAX>
											>::type
										>::type
									>::type
								>::type
							>::type
						>::type
					>::type
				>::type
			>::type
		>::type
			max_t;
	static const KEY_TYPE MAX = max_t::value;
};
//キーを取得　※キーが符号付型の場合
template<class T, class GET_KEY_FUNCTOR>
struct _radix_key_s{
	typedef typename GET_KEY_FUNCTOR::key_type KEY_TYPE;
	typedef typename std::make_unsigned<KEY_TYPE>::type KEY_TYPE_U;
	inline static KEY_TYPE_U getKey(const T& obj, GET_KEY_FUNCTOR get_key_functor)
	{
		KEY_TYPE key = get_key_functor(obj);
		return key >= 0 ?
					static_cast<KEY_TYPE_U>(key) + _radix_key<KEY_TYPE>::MAX + 1:
					static_cast<KEY_TYPE_U>(key  + _radix_key<KEY_TYPE>::MAX + 1);
	}
};
//キーを取得　※キーが符号なし型の場合
template<class T, class GET_KEY_FUNCTOR>
struct _radix_key_u{
	typedef typename GET_KEY_FUNCTOR::key_type KEY_TYPE;
	typedef KEY_TYPE KEY_TYPE_U;
	inline static KEY_TYPE_U getKey(const T& obj, GET_KEY_FUNCTOR get_key_functor)
	{
		return get_key_functor(obj);
	}
};
//キーを取得
template<class T, class GET_KEY_FUNCTOR>
struct radix_key{
	typedef typename GET_KEY_FUNCTOR::key_type KEY_TYPE;
	typedef typename std::make_unsigned<KEY_TYPE>::type KEY_TYPE_U;
	typedef
		typename std::conditional<
			std::is_signed<KEY_TYPE>::value,
			_radix_key_s<T, GET_KEY_FUNCTOR>,
			_radix_key_u<T, GET_KEY_FUNCTOR>
		>::type
			get_key_t;
	inline static KEY_TYPE_U getKey(const T& obj, GET_KEY_FUNCTOR get_key_functor){ return get_key_t::getKey(obj, get_key_functor); }
};
//基数ソート本体
template<class T, class GET_KEY_FUNCTOR>
inline std::size_t radixSort(T* array, const std::size_t size, GET_KEY_FUNCTOR get_key_functor)
{
	if (!array || size <= 1)
		return 0;

#ifdef GASHA_RADIX_SORT_USE_RECURSIVE_CALL
	//--------------------
	//再帰処理版
	std::size_t swapped_count = 0;

	typedef typename GET_KEY_FUNCTOR::key_type KEY_TYPE;//キー型
	typedef typename std::make_unsigned<KEY_TYPE>::type KEY_TYPE_U;//符号なしキー型

	static const std::size_t RADIX = 256;//基数

	//キー情報定義
	struct key_t
	{
		mutable const key_t* next;//次のキー情報
		const T* ref;//実データの参照
		KEY_TYPE_U key;//キー
		inline KEY_TYPE_U set(const T* _ref, const KEY_TYPE_U _key)//値のセット
		{
			next = nullptr;
			ref = _ref;
			key = _key;
			return key;
		}
	};
	//キー情報生成
	KEY_TYPE_U key_max = 0;//キーの最大値
	key_t* key_tbl = new key_t[size];//※newでメモリ確保
	{
		T* elem_p = array;
		key_t* key_p = key_tbl;
		for (std::size_t index = 0; index < size; ++index, ++elem_p, ++key_p)
			key_max = std::max(key_max, key_p->set(elem_p, radix_key<T, GET_KEY_FUNCTOR>::getKey(*elem_p, get_key_functor)));
	}
	//キーの最大値から最大の長さを算出
	std::size_t key_len =
		(key_max & 0xff00000000000000llu) != 0 ? 8 :
		(key_max & 0x00ff000000000000llu) != 0 ? 7 :
		(key_max & 0x0000ff0000000000llu) != 0 ? 6 :
		(key_max & 0x000000ff00000000llu) != 0 ? 5 :
		(key_max & 0xff000000) != 0 ? 4 :
		(key_max & 0x00ff0000) != 0 ? 3 :
		(key_max & 0xff00) != 0 ? 2 :
		(key_max & 0xff) != 0 ? 1 :
		0;
	if (key_len == 0)//キーが 0 しかなければこの時点で終了
	{
		delete key_tbl;//メモリ破棄
		return 0;
	}
	
	//ソート用関数オブジェクト
	struct sort_functor
	{
		//分布情報
		struct bucket_t
		{
			const key_t* keys;//キー情報の連結リスト
			std::size_t count;//キー情報数
			inline void add(const key_t* key_p)//キー追加（連結）
			{
				key_p->next = keys;
				keys = key_p;
				++count;
			}
		};
		//カウントテーブル型
		typedef bucket_t bucket_tbl_t[RADIX];
		//インデックス計算
		inline static unsigned char calcDigit(const KEY_TYPE_U key, const std::size_t key_len)
		{
			return (key >> (key_len << 3)) & 0xff;
		}
		//ソート内部処理用関数オブジェクト
		//※次の基数別にキーを集める／ソート済み配列にデータをコピー
		struct inner_functor
		{
			void operator()(T* sorted_array, std::size_t& array_index, const key_t* key_list, const std::size_t key_list_count, std::size_t key_len, const bool is_odd)
			{
				//キーがなければ、ソート済み配列にデータをコピー
				if (key_len == 0)
				{
					if (is_odd)//奇数回目の実行の場合
					{
						T* sorted_p = sorted_array + array_index + key_list_count;
						const key_t* key_p = key_list;
						while (key_p)
						{
							*(--sorted_p) = std::move(*key_p->ref);
							++array_index;
							key_p = key_p->next;
						}
					}
					else//if(!is_odd)//偶数回目の実行の場合
					{
						T* sorted_p = sorted_array + array_index;
						const key_t* key_p = key_list;
						while (key_p)
						{
							*(sorted_p++) = std::move(*key_p->ref);
							++array_index;
							key_p = key_p->next;
						}
					}
					return;//終了
				}
				//次の基数別にキーを集める
				--key_len;
				bucket_tbl_t child_tbl;
				memset(child_tbl, 0, sizeof(child_tbl));
				const key_t* key_p = key_list;
				while (key_p)
				{
					const key_t* next_p = key_p->next;
					child_tbl[calcDigit(key_p->key, key_len)].add(key_p);
					key_p = next_p;
				}
				//さらに次の基数別にキーを集める
				for (auto& child : child_tbl)
				{
					if (child.keys)
						inner_functor()(sorted_array, array_index, child.keys, child.count, key_len, !is_odd);
				}
			}
		};
		//ソート初回処理
		//※基数別にキーを集める
		inline void operator()(T* sorted_array, std::size_t& array_index, const key_t* key_tbl, std::size_t size, std::size_t key_len)
		{
			//基数別にキーを集める
			--key_len;
			bucket_tbl_t child_tbl;
			memset(child_tbl, 0, sizeof(child_tbl));
			const key_t* key_p = key_tbl;
			for (std::size_t index = 0; index < size; ++index, ++key_p)
				child_tbl[calcDigit(key_p->key, key_len)].add(key_p);
			//次の基数別にキーを集める／ソート済み配列にデータをコピー
			for (auto& child : child_tbl)
			{
				if (child.keys)
					inner_functor()(sorted_array, array_index, child.keys, child.count, key_len, true);
			}
		}
	};
	//ソート済みデータ配列作成
	T* sorted_array = static_cast<T*>(_aligned_malloc(sizeof(T)* size, alignof(T)));//※_aligned_mallocでメモリ確保
	std::size_t array_index = 0;
	//ソート
	sort_functor()(sorted_array, array_index, key_tbl, size, key_len);
	//ソート済みデータを元の配列にコピー
	{
		const T* src_p = sorted_array;
		T* dst_p = array;
		for (std::size_t index = 0; index < size; ++index, ++src_p, ++dst_p)
			*dst_p = std::move(*src_p);
	}
	delete key_tbl;//メモリ破棄
	_aligned_free(sorted_array);//メモリ破棄
#else//GASHA_RADIX_SORT_USE_RECURSIVE_CALL
	//--------------------
	//スタック処理版
	std::size_t swapped_count = 0;

	//#define RADIX_IS_16//基数を16にする場合は、このマクロを有効化する（無効化時の基数は256)
	//※基数は、256の方が速いが、16の方が所用メモリ量を抑えられる

	typedef typename GET_KEY_FUNCTOR::key_type KEY_TYPE;//キー型
	typedef typename std::make_unsigned<KEY_TYPE>::type KEY_TYPE_U;//符号なしキー型

	typedef unsigned int size_type;//サイズ型
	typedef unsigned int index_type;//インデックス型

	//キー情報定義
	struct key_t
	{
		mutable const key_t* next;//次のキー情報
		const T* ref;//実データの参照
		KEY_TYPE_U key;//キー

		//初期化
		inline KEY_TYPE_U init(const T* _ref, const KEY_TYPE_U _key)
		{
			next = nullptr;
			ref = _ref;
			key = _key;
			return key;
		}
	};

	//キー情報メモリ確保
	key_t* key_tbl = new key_t[size];
	if (!key_tbl)//メモリ確保に失敗したら終了
		return 0;

	//キー情報生成
	KEY_TYPE_U key_max = 0;//キーの最大値
	{
		T* elem_p = array;
		key_t* key_p = key_tbl;
		for (index_type index = 0; index < size; ++index, ++elem_p, ++key_p)
			key_max = std::max(key_max, key_p->init(elem_p, radix_key<T, GET_KEY_FUNCTOR>::getKey(*elem_p, get_key_functor)));
	}
	//キーの最大値から最大の長さを算出
	const index_type KEY_LEN = (
	#ifndef RADIX_IS_16//基数が256(8bit)の場合の計算
		(key_max & 0xff00000000000000llu) != 0llu ? 8 :
		(key_max & 0x00ff000000000000llu) != 0llu ? 7 :
		(key_max & 0x0000ff0000000000llu) != 0llu ? 6 :
		(key_max & 0x000000ff00000000llu) != 0llu ? 5 :
		(key_max & 0x00000000ff000000u)   != 0u   ? 4 :
		(key_max & 0x0000000000ff0000u)   != 0u   ? 3 :
		(key_max & 0x000000000000ff00u)   != 0u   ? 2 :
		(key_max & 0x00000000000000ffu)   != 0u   ? 1 :
	#else//RADIX_IS_16//基数が16(4bit)の場合の計算
		(key_max & 0xf000000000000000llu) != 0llu ? 16 :
		(key_max & 0x0f00000000000000llu) != 0llu ? 15 :
		(key_max & 0x00f0000000000000llu) != 0llu ? 14 :
		(key_max & 0x000f000000000000llu) != 0llu ? 13 :
		(key_max & 0x0000f00000000000llu) != 0llu ? 12 :
		(key_max & 0x00000f0000000000llu) != 0llu ? 11 :
		(key_max & 0x000000f000000000llu) != 0llu ? 10 :
		(key_max & 0x0000000f00000000llu) != 0llu ?  9 :
		(key_max & 0x00000000f0000000u)   != 0u   ?  8 :
		(key_max & 0x000000000f000000u)   != 0u   ?  7 :
		(key_max & 0x0000000000f00000u)   != 0u   ?  6 :
		(key_max & 0x00000000000f0000u)   != 0u   ?  5 :
		(key_max & 0x000000000000f000u)   != 0u   ?  4 :
		(key_max & 0x0000000000000f00u)   != 0u   ?  3 :
		(key_max & 0x00000000000000f0u)   != 0u   ?  2 :
		(key_max & 0x000000000000000fu)   != 0u   ?  1 :
	#endif//RADIX_IS_16
		0);
	if (KEY_LEN == 0)//キーの桁数が 0 ならこの時点で終了
	{
		delete[] key_tbl;//メモリ破棄
		return 0;
	}

#ifndef RADIX_IS_16//基数が256(8bit)の場合の計算
	static const index_type RADIX = 256;//基数が256(8bit)の場合 ※基数 = 分布数
#else//RADIX_IS_16//基数が16(4bit)の場合の計算
	static const index_type RADIX = 16;//基数が16(4bit)の場合 ※基数 = 分布数
#endif//RADIX_IS_16

	//分布情報型定義
	struct bucket_t
	{
		const key_t* keys;//キー情報の連結リスト

		//キーを追加（連結）
		inline const key_t* add(const key_t* key_p)
		{
			const key_t* next_p = key_p->next;
			key_p->next = keys;
			keys = key_p;
			return next_p;
		}
	};
	//分布情報セット型定義
	struct bucket_set_t
	{
		bucket_t bucket_tbl[RADIX];//分布情報

		//初期化
		inline bucket_t* init()
		{
			memset(this, 0, sizeof(bucket_set_t));
			return bucket_tbl;
		}
		//指定の桁の分布位置計算
		inline static unsigned char calcDigit(const KEY_TYPE_U key, const index_type key_column)
		{
		#ifndef RADIX_IS_16//基数が256(8bit)の場合の計算
			return (key >> (key_column << 3)) & 0xff;//基数が256(8bit)の場合の計算
		#else//RADIX_IS_16//基数が16(4bit)の場合の計算
			return (key >> (key_column << 2)) & 0xf;//基数が16(4bit)の場合の計算
		#endif//RADIX_IS_16
		};
	};
	//スタック型定義
	struct stack_t
	{
		index_type bucket_index;//分布情報のインデックス

		//初期化
		inline void init()
		{
			bucket_index = 0;
		}
	};
	
	//メモリ確保
	const key_t** sorted_key_tbl = new const key_t*[size];//ソート済みキー情報
	bucket_set_t* bucket_tbl_set = new bucket_set_t[KEY_LEN];//分布情報セット
	stack_t* stack = new stack_t[KEY_LEN];//スタック
	if (!sorted_key_tbl || !bucket_tbl_set || !stack)//メモリ確保に失敗したら終了
	{
		if (key_tbl)
			delete[] key_tbl;//メモリ破棄
		if (sorted_key_tbl)
			delete[] sorted_key_tbl;//メモリ破棄
		if (bucket_tbl_set)
			delete[] bucket_tbl_set;//メモリ破棄
		if (stack)
			delete[] stack;//メモリ破棄
		return 0;
	}

	//最初の桁（最上位の桁）の分布を集計
	index_type stack_depth = 1;//スタックの深さ
	{
		const index_type stack_index = stack_depth - 1;//スタックインデックス
		const index_type key_column = KEY_LEN - 1;//処理対象桁
		//最初の桁用の分布情報初期化
		bucket_t* bucket_tbl = bucket_tbl_set[stack_index].init();
		//最初の桁用の分布情報にキーを追加
		const key_t* key_p = key_tbl;
		for (index_type index = 0; index < size; ++index, ++key_p)
			bucket_tbl[bucket_set_t::calcDigit(key_p->key, key_column)].add(key_p);
		//最初の桁用のスタック初期化
		stack[stack_index].init();
	}
	
	//ソート処理メイン
	index_type sorted_key_index = 0;//ソート済みキー情報のインデックス（コピー先の位置）初期化
	while (stack_depth != 0)
	{
		const index_type stack_index = stack_depth - 1;//スタックインデックス

		//スタック取り出し
		stack_t* stack_p = &stack[stack_index];
		const index_type bucket_index = stack_p->bucket_index++;
		if (bucket_index == RADIX)//分布情報の終端に達したら前の桁に戻る
		{
			--stack_depth;
			continue;
		}
		//分布情報取得
		bucket_t* bucket_tbl = bucket_tbl_set[stack_index].bucket_tbl;
		const bucket_t* bucket_p = &bucket_tbl[bucket_index];
		const key_t* keys = bucket_p->keys;
		if (keys)
		{
			//末端（最下位）の桁なら、ソート済み配列にキーをコピー
			if (stack_depth == KEY_LEN)
			{
				const key_t** sorted_key_p = sorted_key_tbl + sorted_key_index;//キーコピーの初期位置
				if ((stack_depth & 0x1) != 0x0)//奇数回目の実行か？　※基数回目ではリンクリストが逆順に連結されている
				{
					//コピー開始位置を移動
					for (const key_t* key_p = keys; key_p; key_p = key_p->next)
						++sorted_key_p;
					//逆順にコピー
					for (const key_t* key_p = keys; key_p; key_p = key_p->next)
					{
						*(--sorted_key_p) = key_p;
						++sorted_key_index;
					}
				}
				else
				{
					//正順にコピー
					for (const key_t* key_p = keys; key_p; key_p = key_p->next)
					{
						*(sorted_key_p++) = key_p;
						++sorted_key_index;
					}
				}
			}
			//次の桁の分布を集計
			else//if(stack_depth < key_len_real)
			{
				//スタックの深さを次に進める
				const index_type next_stack_index = stack_depth;//次のスタックインデックス
				++stack_depth;//スタックの深さ
				const index_type next_key_column = KEY_LEN - stack_depth;//次の処理対象桁
				//分布情報初期化
				bucket_tbl = bucket_tbl_set[next_stack_index].init();
				//次の桁の分布情報にキーを追加 ※キー情報を使いまわすため、リンクリストがつなぎ直される
				for (const key_t* key_p = keys; key_p;)
					key_p = bucket_tbl[bucket_set_t::calcDigit(key_p->key, next_key_column)].add(key_p);
				//次の桁の処理用スタックを初期化
				stack[next_stack_index].init();
			}
		}
	}

	//ソート済みデータを元に配列の入れ替え
	auto copyToArray = [&array, &size, &sorted_key_tbl]() -> std::size_t
	{
		std::size_t swapped_count = 0;
		const key_t** sorted_key_pp = sorted_key_tbl;//ソート済み位置（移動先）のキー情報を参照
		T* dst_p = array;//実際のデータを参照
		for (index_type dst_index = 0; dst_index < size; ++dst_index, ++sorted_key_pp, ++dst_p)
		{
			const key_t** dst_tmp_key_pp = sorted_key_pp;//ソート済み位置（移動先）のキー情報を参照
			const key_t* dst_tmp_key_p = *dst_tmp_key_pp;//ソート済み位置（移動先）のキー情報を取得
			if (!dst_tmp_key_p || dst_tmp_key_p->ref == dst_p)//並び替え済み、もしくは、最初から正しい位置にあるなら処理しない
				continue;
			T* dst_tmp_p = dst_p;//最初の移動先
			T tmp = std::move(*dst_tmp_p);//ソート済み位置の値を退避　※連鎖的な値の移動の最後に使用する値
			while (dst_tmp_key_p)//移動済みのキーを参照したら終了
			{
				const T* src_p = dst_tmp_key_p->ref;//移動元の値を参照 ※ソート前の位置
				if (src_p == dst_p)//退避した値の位置を指したらループ終了
					break;
				*dst_tmp_p = std::move(*src_p);//元の位置からソート済み位置に値をコピー
				++swapped_count;

				//連鎖的な値の移動
				*dst_tmp_key_pp = nullptr;//移動済みにする
				dst_tmp_p = const_cast<T*>(src_p);//移動元の位置を次のループでの移動先にする
				index_type dst_tmp_index = static_cast<index_type>(dst_tmp_p - array);//移動先のインデックスを算出
				dst_tmp_key_pp = sorted_key_tbl + dst_tmp_index;//ソート済み位置（移動先）のキー情報を参照
				dst_tmp_key_p = *(dst_tmp_key_pp);//ソート済み位置（移動先）のキー情報を取得
			}
			*dst_tmp_p = std::move(tmp);//ループ前に退避した値を最後の位置にコピー
			*dst_tmp_key_pp = nullptr;//移動済みにする
			++swapped_count;
		}
		return swapped_count;
	};
	swapped_count = copyToArray();
	
	//メモリ破棄
	delete[] key_tbl;//メモリ破棄
	delete[] sorted_key_tbl;//メモリ破棄
	delete[] bucket_tbl_set;//メモリ破棄
	delete[] stack;//メモリ破棄
#endif//GASHA_RADIX_SORT_USE_RECURSIVE_CALL
	return swapped_count;
}
distributedSortingFuncSet(radixSort);

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__RADIX_SORT_H_

// End of file

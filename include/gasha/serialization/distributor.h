#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_DISTRIBUTOR_H
#define GASHA_INCLUDED_SERIALIZATION_DISTRIBUTOR_H

//--------------------------------------------------------------------------------
// serialization/distributor.h
// シリアライズ/データ分配処理【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/version.h>//シリアライズ/バージョン
#include <gasha/serialization/item_info_base.h>//シリアライズ/データ項目情報基底クラス

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/データ分配処理
	//--------------------------------------------------------------------------------

	//--------------------
	//データ分配処理用関数オブジェクトテンプレートクラス
	//※デシリアライズ専用処理
	//※特殊化によりユーザー処理を実装
	//※処理の中で使えるのは operator>>() のみ
	//　operator&() は禁止
	//※収集処理で登録されたオブジェクトがセーブデータから見つかるごとに
	//　何度も呼び出される
	//※見つかったオブジェクトの情報（データ項目情報 = target_item）が渡される
	//※基本オブジェクト（obj）は、配列だった場合、その先頭の要素が渡される
	//※セーブデータ上の配列要素数と、ロードできた配列要素数（メモリ上の配列要素数）が渡される
	//※標準では何もしない
	template<class ARCHIVE_TYPE, class T>
	struct distributor {
		typedef int IS_UNDEFINED;//SFINAE用:関数オブジェクトの未定義チェック用の型定義
		inline void operator()(ARCHIVE_TYPE& arc, T& obj, const std::size_t array_num_on_save_data, const std::size_t array_num_loaded, const GASHA_ serialization::version& ver, const GASHA_ serialization::version& now_ver, const GASHA_ serialization::itemInfoBase& target_item)
		{}
	};
	//--------------------
	//データ分配前処理用関数オブジェクトテンプレートクラス
	//※デシリアライズ専用処理
	//※特殊化によりユーザー処理を実装
	template<class ARCHIVE_TYPE, class T>
	struct beforeDistribute {
		typedef int IS_UNDEFINED;//SFINAE用:関数オブジェクトの未定義チェック用の型定義
		inline void operator()(ARCHIVE_TYPE& arc, T& obj, const std::size_t array_num_on_save_data, const std::size_t array_num_loaded, const GASHA_ serialization::version& ver, const GASHA_ serialization::version& now_ver)
		{}
	};
	//--------------------
	//データ分配後処理用関数オブジェクトテンプレートクラス
	//※デシリアライズ専用処理
	//※特殊化によりユーザー処理を実装
	template<class ARCHIVE_TYPE, class T>
	struct afterDistribute {
		typedef int IS_UNDEFINED;//SFINAE用:関数オブジェクトの未定義チェック用の型定義
		inline void operator()(ARCHIVE_TYPE& arc, T& obj, const std::size_t array_num_on_save_data, const std::size_t array_num_loaded, const GASHA_ serialization::version& ver, const GASHA_ serialization::version& now_ver)
		{}
	};

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SERIALIZATION_DISTRIBUTOR_H

// End of file

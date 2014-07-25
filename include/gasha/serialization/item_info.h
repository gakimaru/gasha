#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_ITEM_INFO_H
#define GASHA_INCLUDED_SERIALIZATION_ITEM_INFO_H

//--------------------------------------------------------------------------------
// serialization/item_info.h
// シリアライズ/データ項目情報【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/item_info_base.h>//シリアライズ/データ項目情報基底クラス
#include <gasha/serialization/type_traits.h>//シリアライズ/型特性処理
#include <gasha/serialization/type_ctrl.h>//シリアライズ/シリアライズ/型操作

#include <cstddef>//std::size_t

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <functional>//C++11 std::function
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/データ項目情報
	//--------------------------------------------------------------------------------

	//--------------------
	//データ項目情報テンプレートクラス
	template<typename T>
	class itemInfo : public GASHA_ serialization::itemInfoBase
	{
	public:
		//コンストラクタ
		inline itemInfo(const char* name, const T* item_p, const std::size_t arr_num, const bool is_ptr, const bool is_vlen);
		inline itemInfo(const char* name, const std::size_t size, const T* item_p, const bool is_ptr, const bool is_vlen);
		inline itemInfo(const char* name, const std::size_t size);
		inline itemInfo(const char* name);
		inline itemInfo(const GASHA_ serialization::itemInfoBase& src);
		//デストラクタ
		inline ~itemInfo();
	};
	//--------------------
	//データ項目情報作成テンプレート関数
	template<class T>
	inline itemInfo<typename GASHA_ serialization::isPtr<T>::TYPE> pair(const char* name, const T& item);
	//※個別デシリアライズ処理指定版
	template<class T, typename OBJ>
	inline itemInfo<typename GASHA_ serialization::isPtr<T>::TYPE> pair(const char* name, const T& item, std::function<void(OBJ&, const GASHA_ serialization::itemInfoBase&)> deserializer);
	//--------------------
	//データ項目情報作成テンプレート関数（配列自動判定用）
	template<class T, std::size_t N>
	inline itemInfo<T> pair(const char* name, const T(&item)[N]);
	//※個別デシリアライズ処理指定版
	template<class T, std::size_t N, typename OBJ>
	inline itemInfo<T> pair(const char* name, const T(&item)[N], std::function<void(OBJ&)> deserializer);
	//--------------------
	//データ項目情報作成テンプレート関数（ポインタを配列扱いにしたい場合に使用）
	template<class T>
	inline itemInfo<typename GASHA_ serialization::isPtr<T>::TYPE> pairArray(const char* name, const T& item, const std::size_t N);
	//※個別デシリアライズ処理指定版
	template<class T, typename OBJ>
	inline itemInfo<typename GASHA_ serialization::isPtr<T>::TYPE> pairArray(const char* name, const T& item, const std::size_t N, std::function<void(OBJ&, const GASHA_ serialization::itemInfoBase&)> deserializer);
	//--------------------
	//データ項目情報作成テンプレート関数（バイナリ用）
	//※operator&()専用
	//※operator<<() / operator>>() には使用禁止
#if 1//バイナリ型扱いにする場合
	template<class T>
	inline itemInfo<GASHA_ serialization::bin_type> pairBin(const char* name, const T& item);
	//※個別デシリアライズ処理指定版
	template<class T, typename OBJ>
	inline itemInfo<GASHA_ serialization::bin_type> pairBin(const char* name, const T& item, std::function<void(OBJ&, const GASHA_ serialization::itemInfoBase&)> deserializer);
#else//charの配列扱いにする場合
	template<class T>
	inline itemInfo<char> pairBin(const char* name, const T& item);
	//※個別デシリアライズ処理指定版
	template<class T, typename OBJ>
	inline itemInfo<char> pairBin(const char* name, const T& item, DESERIALIZER deserializer);
#endif
	//--------------------
	//データ項目情報作成テンプレート関数（文字列用）
	//※operator&()専用
	//※operator<<() / operator>>() には使用禁止
	template<std::size_t N>
	inline itemInfo<GASHA_ serialization::str_type> pairStr(const char* name, const char (&item)[N]);
	//※個別デシリアライズ処理指定版
	template<std::size_t N, typename OBJ>
	inline itemInfo<GASHA_ serialization::str_type> pairStr(const char* name, const char(&item)[N], std::function<void(OBJ&, const GASHA_ serialization::itemInfoBase&)> deserializer);
	//※ポインタ用
	inline itemInfo<GASHA_ serialization::str_type> pairStr(const char* name, char* item, const std::size_t item_max_size = GASHA_ serialization::itemInfoBase::VARIABLE_SIZE);
	//※個別デシリアライズ処理指定版
	template<typename OBJ>
	inline itemInfo<GASHA_ serialization::str_type> pairStr(const char* name, char* item, std::function<void(OBJ&, const GASHA_ serialization::itemInfoBase&)> deserializer, const std::size_t item_max_size = GASHA_ serialization::itemInfoBase::VARIABLE_SIZE);
	//--------------------
	//データ項目情報作成テンプレート関数（初回用）
	template<class T>
	inline itemInfo<T> pair(const char* name, const std::size_t size);
	template<class T>
	inline itemInfo<T> pair(const char* name);

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/serialization/item_info.inl>

#endif//GASHA_INCLUDED_SERIALIZATION_ITEM_INFO_H

// End of file

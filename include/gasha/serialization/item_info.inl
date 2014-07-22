#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_ITEM_INFO_INL
#define GASHA_INCLUDED_SERIALIZATION_ITEM_INFO_INL

//--------------------------------------------------------------------------------
// serialization/item_info.inl
// シリアライズ/データ項目情報【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/item_info.h>//シリアライズ/データ項目情報【宣言部】

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

	//コンストラクタ
	template<typename T>
	inline itemInfo<T>::itemInfo(const char* name, const T* item_p, const std::size_t arr_num, const bool is_ptr, const bool is_vlen) :
		GASHA_ serialization::itemInfoBase(name, item_p, typeid(T), sizeof(T), arr_num, hasAnyFunctor<T>(), is_ptr, is_vlen, typeCtrl<T>())
	{}
	template<typename T>
	inline itemInfo<T>::itemInfo(const char* name, const std::size_t size, const T* item_p, const bool is_ptr, const bool is_vlen) :
		GASHA_ serialization::itemInfoBase(name, item_p, typeid(T), size, 0, hasAnyFunctor<T>(), is_ptr, is_vlen, typeCtrl<T>())
	{}
	template<typename T>
	inline itemInfo<T>::itemInfo(const char* name, const std::size_t size) :
		GASHA_ serialization::itemInfoBase(name, nullptr, typeid(T), size, 0, hasAnyFunctor<T>(), false, false, typeCtrl<T>())
	{}
	template<typename T>
	inline itemInfo<T>::itemInfo(const char* name) :
		GASHA_ serialization::itemInfoBase(name, nullptr, typeid(T), 0, 0, hasAnyFunctor<T>(), false, false, typeCtrl<T>())
	{}
	template<typename T>
	inline itemInfo<T>::itemInfo(const GASHA_ serialization::itemInfoBase& src) :
		GASHA_ serialization::itemInfoBase(src)
	{}
	//デストラクタ
	template<typename T>
	inline itemInfo<T>::~itemInfo()
	{}
	
	//--------------------
	//データ項目情報作成テンプレート関数
	template<class T>
	inline itemInfo<typename GASHA_ serialization::isPtr<T>::value_type> pair(const char* name, const T& item)
	{
		itemInfo<typename GASHA_ serialization::isPtr<T>::value_type> item_obj(name, GASHA_ serialization::isPtr<T>::toPtr(item), 0, GASHA_ serialization::isPtr<T>::value, false);
		return item_obj;
	}
	
	//--------------------
	//データ項目情報作成テンプレート関数（配列自動判定用）
	template<class T, std::size_t N>
	inline itemInfo<T> pair(const char* name, const T(&item)[N])
	{
		itemInfo<T> item_obj(name, item, N, false, false);
		return item_obj;
	}
	
	//--------------------
	//データ項目情報作成テンプレート関数（ポインタを配列扱いにしたい場合に使用）
	template<class T>
	inline itemInfo<typename GASHA_ serialization::isPtr<T>::value_type> pairArray(const char* name, const T& item, const std::size_t N)
	{
		itemInfo<typename GASHA_ serialization::isPtr<T>::value_type> item_obj(name, GASHA_ serialization::isPtr<T>::toPtr(item), N, GASHA_ serialization::isPtr<T>::value, false);
		return item_obj;
	}
	
	//--------------------
	//データ項目情報作成テンプレート関数（バイナリ用）
#if 1//バイナリ型扱いにする場合
	template<class T>
	inline itemInfo<GASHA_ serialization::bin_type> pairBin(const char* name, const T& item)
	{
		const std::size_t item_size = sizeof(T);
		const GASHA_ serialization::bin_type* item_p = reinterpret_cast<const GASHA_ serialization::bin_type*>(&item);
		itemInfo<GASHA_ serialization::bin_type> item_obj(name, item_size, item_p, false, false);
		return item_obj;
	}
#else//charの配列扱いにする場合
	template<class T>
	inline itemInfo<char> pairBin(const char* name, const T& item)
	{
		const std::size_t arra_num = sizeof(T);
		const char* item_p = reinterpret_cast<const char*>(&item);
		itemInfo<char> item_obj(name, item_p, arra_num, false, false);
		return item_obj;
	}
#endif
	
	//--------------------
	//データ項目情報作成テンプレート関数（文字列用）
	template<std::size_t N>
	inline itemInfo<GASHA_ serialization::str_type> pairStr(const char* name, const char (&item)[N])
	{
		const std::size_t item_size = sizeof(char)* N;
		const GASHA_ serialization::str_type* item_p = reinterpret_cast<const GASHA_ serialization::str_type*>(&item);
		itemInfo<GASHA_ serialization::str_type> item_obj(name, item_size, item_p, false, true);
		return item_obj;
	}
	//※ポインタ用
	inline itemInfo<GASHA_ serialization::str_type> pairStr(const char* name, char* item)
	{
		const std::size_t item_size = 0;//シリアライズ読み込み用のサイズ（シリアライズ書き込み時は実際の文字列長+1に更新する／ヌルなら0）
		const GASHA_ serialization::str_type* item_p = reinterpret_cast<const GASHA_ serialization::str_type*>(item);
		itemInfo<GASHA_ serialization::str_type> item_obj(name, item_size, item_p, true, true);
		return item_obj;
	}
	
	//--------------------
	//データ項目情報作成テンプレート関数（初回用）
	template<class T>
	inline itemInfo<T> pair(const char* name, const std::size_t size)
	{
		itemInfo<T> item_obj(name, size);
		return item_obj;
	}
	template<class T>
	inline itemInfo<T> pair(const char* name)
	{
		itemInfo<T> item_obj(name, 0);
		return item_obj;
	}

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SERIALIZATION_ITEM_INFO_INL

// End of file

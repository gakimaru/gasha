#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_TYPE_TRAITS_H
#define GASHA_INCLUDED_SERIALIZATION_TYPE_TRAITS_H

//--------------------------------------------------------------------------------
// serialization/type_traits.h
// シリアライズ/型特性処理【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/型特性処理
	//--------------------------------------------------------------------------------

	//--------------------
	//オブジェクト型か？
	//※いずれかの関数オブジェクトが登録されていればオブジェクト型とみなす
	//※オブジェクト型はシリアライズの際にデータブロックとして扱う
	template<class T>
	inline bool hasAnyFunctor();

	//--------------------
	//ポインタ型チェック用クラス
	template<class T>
	struct isPtr
	{
		static const bool value = false;//ポインタ型か？ = 非ポインタ型
		typedef T value_type;//通常型（非ポインタ型）変換用の型
		typedef T* pointer;//ポインタ型変換用の型
		inline static const T& toValue(const T& var){ return var; }//値に変換
		inline static const T* toPtr(const T& var){ return reinterpret_cast<const T*>(&var); }//ポインタに変換
		inline static bool isNull(const T& var){ return false; }//ヌルか？
	};
	//※ポインタの場合の部分特殊化
	template<class T>
	struct isPtr<T*>
	{
		static const bool value = true;//ポインタ型か？ = ポインタ型
		typedef T value_type;//通常型（非ポインタ型）変換用の型
		typedef T* pointer;//ポインタ型変換用の型
		inline static const T& toValue(const T* var){ return *var; }//値に変換
		inline static const T* toPtr(const T* var){ return var; }//ポインタに変換
		inline static bool isNull(const T* var){ return var == nullptr; }//ヌルか？
	};

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/serialization/type_traits.inl>

#endif//GASHA_INCLUDED_SERIALIZATION_TYPE_TRAITS_H

// End of file

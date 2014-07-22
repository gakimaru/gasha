#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_TYPE_TRAITS_INL
#define GASHA_INCLUDED_SERIALIZATION_TYPE_TRAITS_INL

//--------------------------------------------------------------------------------
// serialization/type_traits.inl
// シリアライズ/型特性処理【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/serialize.h>//シリアライズ/シリアライズ／デシリアライズ共通処理
#include <gasha/serialization/save.h>//シリアライズ/セーブ処理
#include <gasha/serialization/load.h>//シリアライズ/ロード処理
#include <gasha/serialization/notice.h>//シリアライズ/通知処理
#include <gasha/serialization/collector.h>//シリアライズ/データ収集処理
#include <gasha/serialization/distributor.h>//シリアライズ/データ分配処理
#include <gasha/serialization/error.h>//シリアライズ/致命的エラー発生時処理

#include <gasha/simple_assert.h>//シンプルアサーション

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/型特性処理
	//--------------------------------------------------------------------------------

	namespace _private
	{
		//--------------------
		//ユーザー定義処理用特殊化テンプレートクラス定義済みチェック関数
		//※SFINAEにより、IS_UNDEFINED が定義されている型のオーバーロード関数が選ばれたら未定義とみなす
		template<class F>
		inline bool isDefinedFunctor(const typename F::IS_UNDEFINED){ return false; }//未定義
		template<class F>
		inline bool isDefinedFunctor(...){ return true; }//定義済み
	
		//--------------------
		//ダミーシリアライズクラス
		class serializationDummy{};
	}//namespace _private
	
	//--------------------
	//オブジェクト型か？
	template<class T>
	inline bool hasAnyFunctor()
	{
		//ここでチェック：収集処理と分配処理は必ずワンセットで定義する必要あり
		GASHA_SIMPLE_ASSERT((_private::isDefinedFunctor<GASHA_ serialization::collector<_private::serializationDummy, T>>(0)) == (_private::isDefinedFunctor<GASHA_ serialization::distributor<_private::serializationDummy, T>>(0)), "collector and distributor must are defined.");
		//関数オブジェクトのどれか一つでも定義されているかチェック
		return _private::isDefinedFunctor<GASHA_ serialization::serialize<_private::serializationDummy, T>>(0) ||
			_private::isDefinedFunctor<GASHA_ serialization::save<_private::serializationDummy, T>>(0) ||
			_private::isDefinedFunctor<GASHA_ serialization::load<_private::serializationDummy, T>>(0) ||
			_private::isDefinedFunctor<GASHA_ serialization::beforeLoad<_private::serializationDummy, T>>(0) ||
			_private::isDefinedFunctor<GASHA_ serialization::afterLoad<_private::serializationDummy, T>>(0) ||
			_private::isDefinedFunctor<GASHA_ serialization::noticeUnrecognizedItem<_private::serializationDummy, T>>(0) ||
			_private::isDefinedFunctor<GASHA_ serialization::noticeUnloadedItem<_private::serializationDummy, T>>(0) ||
			_private::isDefinedFunctor<GASHA_ serialization::collector<_private::serializationDummy, T>>(0) ||
			_private::isDefinedFunctor<GASHA_ serialization::distributor<_private::serializationDummy, T>>(0) ||
			_private::isDefinedFunctor<GASHA_ serialization::beforeDistribute<_private::serializationDummy, T>>(0) ||
			_private::isDefinedFunctor<GASHA_ serialization::afterDistribute<_private::serializationDummy, T>>(0);
			//_private::isDefinedFunctor<GASHA_ serialization::fatalSerializeErrorOccurred<_private::serializationDummy, T>>(0);//これは数えない
			//_private::isDefinedFunctor<GASHA_ serialization::fatalDeserializeErrorOccurred<_private::serializationDummy, T>>(0);//これは数えない
	}

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SERIALIZATION_TYPE_TRAITS_INL

// End of file

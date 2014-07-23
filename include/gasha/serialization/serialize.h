#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_SERIALIZE_H
#define GASHA_INCLUDED_SERIALIZATION_SERIALIZE_H

//--------------------------------------------------------------------------------
// serialization/serialize.h
// シリアライズ/シリアライズ／デシリアライズ共通処理【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/version.h>//シリアライズ/バージョン
#include <gasha/serialization/item_info_base.h>//シリアライズ/データ項目情報基底クラス

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/シリアライズ／デシリアライズ共通処理
	//--------------------------------------------------------------------------------

	//--------------------
	//シリアライズ／デシリアライズ共通処理用関数オブジェクトテンプレートクラス
	//※シリアライズとデシリアライズ兼用共通処理
	//※特殊化によりユーザー処理を実装
	//※標準では何もしない
	template<class ARCHIVE_TYPE, class T>
	struct serialize {
		typedef int IS_UNDEFINED;//SFINAE用:関数オブジェクトの未定義チェック用の型定義
		inline void operator()(ARCHIVE_TYPE& arc, const T& obj, const GASHA_ serialization::version& ver, const GASHA_ serialization::version& now_ver, const GASHA_ serialization::itemInfoBase* target_item)
		{}
	};

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SERIALIZATION_SERIALIZE_H

// End of file

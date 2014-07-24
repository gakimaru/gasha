#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_SAVE_H
#define GASHA_INCLUDED_SERIALIZATION_SAVE_H

//--------------------------------------------------------------------------------
// serialization/save.h
// シリアライズ/セーブ処理【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/version.h>//シリアライズ/バージョン

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/セーブ処理
	//--------------------------------------------------------------------------------

	//--------------------
	//セーブ処理用関数オブジェクトテンプレートクラス
	//※シリアライズ専用処理
	//※特殊化によりユーザー処理を実装
	//※標準では何もしない
	template<class ARCHIVE, class T>
	struct save {
		typedef int IS_UNDEFINED;//SFINAE用:関数オブジェクトの未定義チェック用の型定義
		inline void operator()(ARCHIVE& arc, const T& obj, const GASHA_ serialization::version& ver)
		{}
	};

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SERIALIZATION_SAVE_H

// End of file

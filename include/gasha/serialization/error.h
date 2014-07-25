#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_ERROR_H
#define GASHA_INCLUDED_SERIALIZATION_ERROR_H

//--------------------------------------------------------------------------------
// serialization/error.h
// シリアライズ/致命的エラー発生時処理【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
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
	//シリアライズ/致命的エラー発生時処理
	//--------------------------------------------------------------------------------

	//--------------------
	//シリアライズ時に致命的エラー発生時処理用関数オブジェクトテンプレートクラス
	//※シリアライズ専用処理
	//※エラーが発生したタイミングではなく、最後に呼び出される
	//※特殊化によりユーザー処理を実装
	template<class ARCHIVE, class T>
	struct fatalSerializeErrorOccurred {
		typedef int IS_UNDEFINED;//SFINAE用:関数オブジェクトの未定義チェック用の型定義
		inline void operator()(ARCHIVE& arc, const T& obj, const GASHA_ serialization::version& ver)
		{}
	};
	//--------------------
	//デシリアライズ時に致命的エラー発生時処理用関数オブジェクトテンプレートクラス
	//※デシリアライズ専用処理
	//※エラーが発生したタイミングではなく、最後に呼び出される
	//※特殊化によりユーザー処理を実装
	template<class ARCHIVE, class T>
	struct fatalDeserializeErrorOccurred {
		typedef int IS_UNDEFINED;//SFINAE用:関数オブジェクトの未定義チェック用の型定義
		inline void operator()(ARCHIVE& arc, const T& obj, const GASHA_ serialization::version& ver, const GASHA_ serialization::version& now_ver)
		{}
	};

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SERIALIZATION_ERROR_H

// End of file

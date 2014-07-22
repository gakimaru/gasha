#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_NOTICE_H
#define GASHA_INCLUDED_SERIALIZATION_NOTICE_H

//--------------------------------------------------------------------------------
// serialization/notice.h
// シリアライズ/デシリアライズ時（ロード時）不整合通知処理【宣言部】
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
	//シリアライズ/デシリアライズ時（ロード時）不整合通知処理
	//--------------------------------------------------------------------------------

	//--------------------
	//セーブデータにはあったが、保存先の指定がなく、デシリアライズ（ロード）できなかった項目の通知
	//※デシリアライズ専用処理
	//※特殊化によりユーザー処理を実装
	//※見つかったら時点で知されるので、objが不完全な状態である点に注意
	//※委譲データ項目 delegate_item にデータ項目をセットして返すとリトライしてそこに読み込む
	//※標準では何もしない
	template<class ARCHIVE_TYPE, class T>
	struct noticeUnrecognizedItem {
		typedef int IS_UNDEFINED;//SFINAE用:関数オブジェクトの未定義チェック用の型定義
		inline void operator()(ARCHIVE_TYPE& arc, T& obj, GASHA_ serialization::version& ver, const GASHA_ serialization::version& now_ver, const GASHA_ serialization::itemInfoBase& unrecognized_item)
		{}
	};
	//--------------------
	//保存先の指定があるが、セーブデータになく、デシリアライズ（ロード）できなかった項目の通知
	//※デシリアライズ専用処理
	//※特殊化によりユーザー処理を実装
	//※objのロードが一通り終わったあと、まとめて通知する
	//※noticeUnrecognizedItem の後、afterLoad より先に実行される
	//※標準では何もしない
	template<class ARCHIVE_TYPE, class T>
	struct noticeUnloadedItem {
		typedef int IS_UNDEFINED;//SFINAE用:関数オブジェクトの未定義チェック用の型定義
		inline void operator()(ARCHIVE_TYPE& arc, T& obj, GASHA_ serialization::version& ver, const GASHA_ serialization::version& now_ver, const GASHA_ serialization::itemInfoBase& unloaded_item)
		{}
	};

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SERIALIZATION_NOTICE_H

// End of file

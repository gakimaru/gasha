#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_FRIENDS_H
#define GASHA_INCLUDED_SERIALIZATION_FRIENDS_H

//--------------------------------------------------------------------------------
// serialization/friends.h
// シリアライズ/フレンド宣言【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//クラス宣言

	//シリアライズ／デシリアライズ共通処理
	template<class ARCHIVE_TYPE, class T> 
	struct serialize;

	//セーブ処理
	template<class ARCHIVE_TYPE, class T>
	struct save;
	
	//ロード処理
	template<class ARCHIVE_TYPE, class T>
	struct load;
	//ロード前処理
	template<class ARCHIVE_TYPE, class T>
	struct beforeLoad;
	//ロード後処理
	template<class ARCHIVE_TYPE, class T>
	struct afterLoad;
	
	//デシリアライズ時（ロード時）不整合通知処理：セーブデータにあるが保存先の指定がないデータ
	template<class ARCHIVE_TYPE, class T>
	struct noticeUnrecognizedItem;
	//デシリアライズ時（ロード時）不整合通知処理：セーブデータにないが保存先の指定があるデータ
	template<class ARCHIVE_TYPE, class T>
	struct noticeUnloadedItem;
	
	//データ収集処理
	template<class ARCHIVE_TYPE, class T>
	struct collector;

	//データ分配処理
	template<class ARCHIVE_TYPE, class T>
	struct distributor;
	//データ分配前処理
	template<class ARCHIVE_TYPE, class T>
	struct beforeDistribute;
	//データ分配後処理
	template<class ARCHIVE_TYPE, class T>
	struct afterDistribute;
	
	//致命的エラー発生時処理：シリアライズ時
	template<class ARCHIVE_TYPE, class T>
	struct fatalSerializeErrorOccurred;
	//致命的エラー発生時処理：デシリアライズ時
	template<class ARCHIVE_TYPE, class T>
	struct fatalDeserializeErrorOccurred;

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

//--------------------
//データクラスのフレンド化マクロ
#define GASHA_SERIALIZATION_FRIENDS() \
	template<class ARCHIVE_TYPE, class T> \
	friend struct GASHA_ serialization::serialize; \
	template<class ARCHIVE_TYPE, class T> \
	friend struct GASHA_ serialization::save; \
	template<class ARCHIVE_TYPE, class T> \
	friend struct GASHA_ serialization::load; \
	template<class ARCHIVE_TYPE, class T> \
	friend struct GASHA_ serialization::beforeLoad; \
	template<class ARCHIVE_TYPE, class T> \
	friend struct GASHA_ serialization::afterLoad; \
	template<class ARCHIVE_TYPE, class T> \
	friend struct GASHA_ serialization::noticeUnrecognizedItem; \
	template<class ARCHIVE_TYPE, class T> \
	friend struct GASHA_ serialization::noticeUnloadedItem; \
	template<class ARCHIVE_TYPE, class T> \
	friend struct GASHA_ serialization::collector; \
	template<class ARCHIVE_TYPE, class T> \
	friend struct GASHA_ serialization::distributor; \
	template<class ARCHIVE_TYPE, class T> \
	friend struct GASHA_ serialization::beforeDistribute; \
	template<class ARCHIVE_TYPE, class T> \
	friend struct GASHA_ serialization::afterDistribute; \
	template<class ARCHIVE_TYPE, class T> \
	friend struct GASHA_ serialization::fatalSerializeErrorOccurred; \
	template<class ARCHIVE_TYPE, class T> \
	friend struct GASHA_ serialization::fatalDeserializeErrorOccurred;

#endif//GASHA_INCLUDED_SERIALIZATION_FRIENDS_H

// End of file

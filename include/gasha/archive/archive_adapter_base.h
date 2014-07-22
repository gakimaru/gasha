#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_ARCHIVE_ADAPTER_BASE_H
#define GASHA_INCLUDED_ARCHIVE_ARCHIVE_ADAPTER_BASE_H

//--------------------------------------------------------------------------------
// archive/archive_adapter_base.h
// アーカイブ/アーカイブアダプター基底クラス【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/archive_base.h>//アーカイブ/アーカイブ基底クラス
#include <gasha/serialization/item_info_base.h>//アーカイブ/データ項目情報
#include <gasha/archive/results.h>//アーカイブ/処理結果

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/アーカイブアダプター基底クラス
	//--------------------------------------------------------------------------------

	//--------------------
	//アーカイブアダプター基底クラス
	class archiveAdapterBase
	{
	public:
		inline GASHA_ archive::results& result();//処理結果を取得
		inline const GASHA_ archive::results& result() const;//処理結果を取得
		inline bool hasFatalError() const;//致命的なエラーありか？
		inline const GASHA_ archive::byte* buffTop() const;//セーブデータバッファの先頭ポインタを取得
		inline const std::size_t maxSize() const;//セーブデータバッファのサイズを取得
		inline const std::size_t size() const;//セーブデータバッファの使用量を取得
		inline const std::size_t remain() const;//セーブデータバッファの残量を取得
		inline GASHA_ archive::byte* buffNow();//セーブデータバッファの現在位置のポインタを取得
		inline bool isFull() const;//バッファの現在位置が末端に到達したか？
		inline std::size_t itemsNum() const;//データ項目数
	public:
		//メソッド
		//処理結果を合成
		inline void addResult(const GASHA_ archive::results& src);
		//リストからデータ項目を検索
		inline const GASHA_ serialization::itemInfoBase* findItem(const GASHA_ crc32_t name_crc) const;
		//バッファのカレントポインタを移動
		//※範囲外への移動が要求されたら端まで移動して false を返す
		bool seek(const int seek_, int& real_seek);
		//※処理結果オブジェクト使用版
		bool seek(GASHA_ archive::results& result, const int seek_);
	protected:
		//コンストラクタ
		inline archiveAdapterBase(GASHA_ archive::archiveBase& arc);
		//デストラクタ
		inline ~archiveAdapterBase();
	protected:
		//フィールド
		GASHA_ archive::archiveBase& m_arc;//アーカイブオブジェクト（セーブデータイメージバッファ）
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/archive/archive_adapter_base.inl>

#endif//GASHA_INCLUDED_ARCHIVE_ARCHIVE_ADAPTER_BASE_H

// End of file

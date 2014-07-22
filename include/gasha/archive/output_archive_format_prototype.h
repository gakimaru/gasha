#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_FORMAT_PROTOTYPE_H
#define GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_FORMAT_PROTOTYPE_H

//--------------------------------------------------------------------------------
// archive/output_archive_format_prototype.h
// アーカイブ/アーカイブ形式プロトタイプ（書き込み用）【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/archive_format_base.h>//アーカイブ/アーカイブフォーマット基底クラス
#include <gasha/serialization/item_info_base.h>//アーカイブ/データ項目情報基底クラス
#include <gasha/archive/output_archive_adapter.h>//アーカイブ/アーカイブ書き込みアダプター
#include <gasha/serialization/version.h>//シリアライズ/バージョン

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/アーカイブ形式プロトタイプ（書き込み用）
	//--------------------------------------------------------------------------------

	//--------------------
	//アーカイブ形式プロトタイプ（書き込み用）
	//※必要なメソッドを定義しているだけのサンプル（クラス追加のための参考用）
	//※実際には使用しない
	class outputArchiveFormat_prototype : public GASHA_ archive::archiveFormatBase
	{
	public:
		//メソッド
		//シグネチャ書き込み
		inline bool writeSignature(GASHA_ archive::outputArchiveAdapter arc){ return true; }
		//ブロックヘッダー書き込み
		inline bool writeBlockHeader(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::version& ver){ return true; }
		//配列ブロックヘッダー書き込み
		inline bool writeArrayHeader(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t array_elem_num){ return true; }
		//要素ヘッダー書き込み
		inline bool writeElemHeader(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index){ return true; }
		//データ項目書き込み
		inline bool writeDataItem(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::itemInfoBase& child_item){ return true; }
		//要素フッター書き込み
		inline bool writeElemFooter(GASHA_ archive::outputArchiveAdapter parent_arc, GASHA_ archive::outputArchiveAdapter child_arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index, short& items_num, std::size_t& elem_size){ return true; }
		//配列ブロックフッター書き込み
		inline bool writeArrayFooter(GASHA_ archive::outputArchiveAdapter parent_arc, GASHA_ archive::outputArchiveAdapter child_arc, const GASHA_ serialization::itemInfoBase& item, std::size_t& array_block_size){ return true; }
		//ブロックフッター書き込み
		inline bool writeBlockFooter(GASHA_ archive::outputArchiveAdapter parent_arc, GASHA_ archive::outputArchiveAdapter child_arc, const GASHA_ serialization::itemInfoBase& item, std::size_t& block_size){ return true; }
		//ターミネータ書き込み
		inline bool writeTerminator(GASHA_ archive::outputArchiveAdapter arc){ return true; }
	public:
		//コンストラクタ
		inline outputArchiveFormat_prototype() :
			GASHA_ archive::archiveFormatBase()
		{}
		//親を受け取るコンストラクタ
		//※処理階層が深くなるごとにコピーが行われる
		inline outputArchiveFormat_prototype(outputArchiveFormat_prototype& parent) :
			GASHA_ archive::archiveFormatBase(parent)
		{}
		//デストラクタ
		inline ~outputArchiveFormat_prototype()
		{}
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
//#include <gasha/archive/output_archive_format_prototype.inl>

#endif//GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_FORMAT_PROTOTYPE_H

// End of file

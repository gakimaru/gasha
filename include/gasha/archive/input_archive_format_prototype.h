#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_FORMAT_PROTOTYPE_H
#define GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_FORMAT_PROTOTYPE_H

//--------------------------------------------------------------------------------
// archive/input_archive_format_prototype.h
// アーカイブ/アーカイブ形式プロトタイプ（読み込み用）【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/archive_format_base.h>//アーカイブ/アーカイブフォーマット基底クラス
#include <gasha/archive/input_archive_adapter.h>//アーカイブ/アーカイブ読み込みアダプター
#include <gasha/serialization/item_info_base.h>//アーカイブ/データ項目情報基底クラス
#include <gasha/serialization/version.h>//シリアライズ/バージョン

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/アーカイブ形式プロトタイプ（読み込み用）
	//--------------------------------------------------------------------------------

	//--------------------
	//アーカイブ形式プロトタイプ（読み込み用）
	//※必要なメソッドを定義しているだけのサンプル（クラス追加のための参考用）
	//※実際には使用しない
	class inputArchiveFormat_prototype : public GASHA_ archive::archiveFormatBase
	{
	public:
		//メソッド
		//パース
		inline bool parse(GASHA_ archive::inputArchiveAdapter arc){ return true; }
		//シグネチャ読み込み
		inline bool readSignature(GASHA_ archive::inputArchiveAdapter arc){ return true; }
		//ブロックヘッダー読み込み
		//※読み込んだオブジェクトの型情報とバージョンを返す
		inline bool readBlockHeader(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::itemInfoBase* delegate_item, const GASHA_ serialization::version& ver, GASHA_ serialization::itemInfoBase& input_item, GASHA_ serialization::version& input_ver, std::size_t& block_size){ return true; }
		//配列ブロックヘッダー読み込み
		inline bool readArrayHeader(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, std::size_t& array_elem_num, std::size_t& array_block_size){ return true; }
		//要素ヘッダー読み込み
		inline bool readElemHeader(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index, std::size_t& items_num, std::size_t& elem_size){ return true; }
		//データ項目読み込み
		inline bool readDataItem(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::itemInfoBase* delegate_child_item_now, GASHA_ serialization::itemInfoBase& child_item, const bool item_is_valid, const bool is_required_retry){ return true; }
		//要素フッター読み込み
		//※読み込みテストの結果、要素フッターでなければデータ項目の読み込みを継続する
		inline bool tryAndReadElemFooter(GASHA_ archive::inputArchiveAdapter child_arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index, bool& is_elem_end){ return true; }
		//要素読み込み終了
		inline bool finishReadElem(GASHA_ archive::inputArchiveAdapter parent_arc, inputArchiveAdapter child_arc){ return true; }
		//配列ブロックフッター読み込み
		//※読み込みテストの結果、配列ブロックフッターでなければデータ項目の読み込みを継続する
		inline bool tryAndReadArrayFooter(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, bool& is_array_block_end){ return true; }
		//配列ブロック読み込み終了
		inline bool finishReadArray(GASHA_ archive::inputArchiveAdapter parent_arc, inputArchiveAdapter child_arc){ return true; }
		//ブロックの読み込みをスキップ
		inline bool skipReadBlock(GASHA_ archive::inputArchiveAdapter arc){ return true; }
		//ブロックフッター読み込み
		//※読み込みテストの結果、ブロックフッターでなければデータ項目（オブジェクト）の読み込みを継続する
		inline bool tryAndReadBlockFooter(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, bool& is_block_end){ return true; }
		//ブロック読み込み終了
		inline bool finishReadBlock(GASHA_ archive::inputArchiveAdapter parent_arc, inputArchiveAdapter child_arc){ return true; }
		//次のブロックヘッダー問い合わせ（先行読み込み）
		//※処理を進めず、次の情報を読み取るのみ
		//※（例えば、バイナリスタイルなら、読み込みバッファのポインタを進めない）
		inline bool requireNextBlockHeader(GASHA_ archive::inputArchiveAdapter arc, GASHA_ serialization::itemInfoBase& require_item, std::size_t& child_block_size, bool& is_found_next_block){ return true; }
		//ターミネータ読み込み
		inline bool readTerminator(GASHA_ archive::inputArchiveAdapter arc){ return true; }
	public:
		//コンストラクタ
		inline inputArchiveFormat_prototype() :
			GASHA_ archive::archiveFormatBase()
		{}
		//親を受け取るコンストラクタ
		//※処理階層が深くなるごとにコピーが行われる
		inline inputArchiveFormat_prototype(inputArchiveFormat_prototype& parent) :
			GASHA_ archive::archiveFormatBase(parent)
		{}
		//デストラクタ
		inline ~inputArchiveFormat_prototype()
		{}
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
//#include <gasha/archive/input_archive_format_prototype.inl>

#endif//GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_FORMAT_PROTOTYPE_H

// End of file

#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_INPUT_BINARY_ARCHIVE_FORMAT_H
#define GASHA_INCLUDED_ARCHIVE_INPUT_BINARY_ARCHIVE_FORMAT_H

//--------------------------------------------------------------------------------
// archive/input_binary_archive_format.h
// アーカイブ/バイナリアーカイブ形式クラス（読み込み用）【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/binary_archive_format_base.h>//アーカイブ/バイナリ形式アーカイブ基底クラス
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
	//アーカイブ/バイナリアーカイブ形式クラス（読み込み用）
	//--------------------------------------------------------------------------------

	//--------------------
	//バイナリ形式アーカイブクラス（アーカイブ読み込み用）
	class inputBinaryArchiveFormat : public GASHA_ archive::binaryArchiveFormatBase
	{
	public:
		//メソッド
		//パース
		bool parse(GASHA_ archive::inputArchiveAdapter arc);
		//シグネチャ読み込み
		bool readSignature(GASHA_ archive::inputArchiveAdapter arc);
		//ブロックヘッダー読み込み
		//※読み込んだオブジェクトの型情報とバージョンを返す
		bool readBlockHeader(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::itemInfoBase* delegate_item, const GASHA_ serialization::version& ve, GASHA_ serialization::itemInfoBase& input_item, GASHA_ serialization::version& input_ver, std::size_t& block_size);
		//配列ブロックヘッダー読み込み
		bool readArrayHeader(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, std::size_t& array_elem_num, std::size_t& array_block_size);
		//要素ヘッダー読み込み
		bool readElemHeader(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index, std::size_t& items_num, std::size_t& elem_size);
		//データ項目読み込み
		bool readDataItem(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::itemInfoBase* delegate_child_item_now, GASHA_ serialization::itemInfoBase& child_item, const bool item_is_valid, const bool is_required_retry);
		//要素フッター読み込み
		//※読み込みテストの結果、要素フッターでなければデータ項目の読み込みを継続する
		bool tryAndReadElemFooter(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index, bool& is_elem_end);
		//要素読み込み終了
		bool finishReadElem(GASHA_ archive::inputArchiveAdapter parent_arc, GASHA_ archive::inputArchiveAdapter child_arc);
		//配列ブロックフッター読み込み
		//※読み込みテストの結果、配列ブロックフッターでなければデータ項目の読み込みを継続する
		bool tryAndReadArrayFooter(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, bool& is_array_block_end);
		//配列ブロック読み込み終了
		bool finishReadArray(GASHA_ archive::inputArchiveAdapter parent_arc, GASHA_ archive::inputArchiveAdapter child_arc);
		//ブロックの読み込みをスキップ
		bool skipReadBlock(GASHA_ archive::inputArchiveAdapter arc);
		//ブロックフッター読み込み
		//※読み込みテストの結果、ブロックフッターでなければデータ項目（オブジェクト）の読み込みを継続する
		bool tryAndReadBlockFooter(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, bool& is_block_end);
		//ブロック読み込み終了
		bool finishReadBlock(GASHA_ archive::inputArchiveAdapter parent_arc, GASHA_ archive::inputArchiveAdapter child_arc);
		//※読み込みテストの結果、ブロックフッターでなければデータ項目（オブジェクト）の読み込みを継続する
		//次のブロックヘッダー問い合わせ（先行読み込み）
		//※処理を進めず、次の情報を読み取るのみ
		//※（例えば、バイナリスタイルなら、読み込みバッファのポインタを進めない）
		bool requireNextBlockHeader(GASHA_ archive::inputArchiveAdapter arc, GASHA_ serialization::itemInfoBase& require_item, std::size_t& require_block_size, bool& is_found_next_block);
		//ターミネータ読み込み
		bool readTerminator(GASHA_ archive::inputArchiveAdapter arc);
	public:
		//コンストラクタ
		inline inputBinaryArchiveFormat();
		//親を受け取るコンストラクタ
		//※処理階層が深くなるごとにコピーが行われる
		inline inputBinaryArchiveFormat(inputBinaryArchiveFormat& parent);
		//デストラクタ
		inline ~inputBinaryArchiveFormat();
	private:
		//フィールド
		std::size_t m_readSizeForPrevDataItem;//【リトライ用】前回のデータ読み込みサイズ
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/archive/input_binary_archive_format.inl>

#endif//GASHA_INCLUDED_ARCHIVE_INPUT_BINARY_ARCHIVE_FORMAT_H

// End of file

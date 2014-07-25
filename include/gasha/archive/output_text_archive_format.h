#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_OUTPUT_TEXT_ARCHIVE_FORMAT_H
#define GASHA_INCLUDED_ARCHIVE_OUTPUT_TEXT_ARCHIVE_FORMAT_H

//--------------------------------------------------------------------------------
// archive/output_text_archive_format.h
// アーカイブ/テキストアーカイブ形式クラス（書き込み用）【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/text_archive_format_base.h>//アーカイブ/テキストアーカイブ形式基底クラス
#include <gasha/archive/output_archive_adapter.h>//アーカイブ/アーカイブ書き込みアダプター
#include <gasha/serialization/item_info_base.h>//アーカイブ/データ項目情報基底クラス
#include <gasha/serialization/version.h>//シリアライズ/バージョン

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/テキストアーカイブ形式クラス（書き込み用）
	//--------------------------------------------------------------------------------

	//--------------------
	//テキスト形式アーカイブクラス（アーカイブ書き込み用）
	class outputTextArchiveFormat : public GASHA_ archive::textArchiveFormatBase
	{
	public:
		//定数
		enum process_type
		{
			PROCESS_UNKNOWN = 0,
			PROCESS_TOP,
			PROCESS_BLOCK,
			PROCESS_ARRAY_BLOCK,
			PROCESS_ELEM,
			PROCESS_ITEM,
		};
	public:
		//メソッド
		//シグネチャ書き込み
		bool writeSignature(GASHA_ archive::outputArchiveAdapter arc);
		//ブロックヘッダー書き込み
		bool writeBlockHeader(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::version& ver);
		//配列ブロックヘッダー書き込み
		bool writeArrayHeader(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t array_elem_num);
		//要素ヘッダー書き込み
		bool writeElemHeader(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index);
		//データ項目書き込み
		bool writeDataItem(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::itemInfoBase& child_item);
		//要素フッター書き込み
		bool writeElemFooter(GASHA_ archive::outputArchiveAdapter parent_arc, GASHA_ archive::outputArchiveAdapter child_arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index, std::size_t& items_num, std::size_t& elem_size);
		//配列ブロックフッター書き込み
		bool writeArrayFooter(GASHA_ archive::outputArchiveAdapter parent_arc, GASHA_ archive::outputArchiveAdapter child_arc, const GASHA_ serialization::itemInfoBase& item, std::size_t& array_block_size);
		//ブロックフッター書き込み
		bool writeBlockFooter(GASHA_ archive::outputArchiveAdapter parent_arc, GASHA_ archive::outputArchiveAdapter child_arc, const GASHA_ serialization::itemInfoBase& item, std::size_t& block_size);
		//ターミネータ書き込み
		bool writeTerminator(GASHA_ archive::outputArchiveAdapter arc);
	public:
		//コンストラクタ
		inline outputTextArchiveFormat();
		//親を受け取るコンストラクタ
		//※処理階層が深くなるごとにコピーが行われる
		inline outputTextArchiveFormat(outputTextArchiveFormat& src);
		//デストラクタ
		inline ~outputTextArchiveFormat();
	private:
		//フィールド
		outputTextArchiveFormat* m_parent;//親アーカイブ形式オブジェクト
		int m_nestLevel;//データのネストレベル
		process_type m_process;//処理状態
		int m_blockIndex;//ブロックインデックス
		int m_arrayBlockIndex;//配列ブロックインデックス
		int m_elemIndex;//要素インデックス
		int m_itemIndex;//データ項目インデックス
		process_type m_parentProcess;//親の処理状態
		int m_parentBlockIndex;//親のブロックインデックス
		int m_parentArrayBlockIndex;//親の配列ブロックインデックス
		int m_parentElemIndex;//親の要素インデックス
		int m_parentItemIndex;//親のデータ項目インデックス
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/archive/output_text_archive_format.inl>

#endif//GASHA_INCLUDED_ARCHIVE_OUTPUT_TEXT_ARCHIVE_FORMAT_H

// End of file

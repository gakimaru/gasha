#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_BINARY_ARCHIVE_FORMAT_BASE_H
#define GASHA_INCLUDED_ARCHIVE_BINARY_ARCHIVE_FORMAT_BASE_H

//--------------------------------------------------------------------------------
// archive/binary_archive_format_base.h
// アーカイブ/バイナリアーカイブ形式基底クラス【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/archive_format_base.h>//アーカイブ/アーカイブ形式基底クラス

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/バイナリアーカイブ形式基底クラス
	//--------------------------------------------------------------------------------

	//--------------------
	//バイナリ形式アーカイブクラス（共通）
	class binaryArchiveFormatBase : public GASHA_ archive::archiveFormatBase
	{
	public:
		//定数
		static const std::size_t SIGNATURE_SIZE = 16;//シグネチャサイズ
		static const std::size_t TERMINATOR_SIZE = 16;//ターミネータサイズ
		static const std::size_t BEGIN_MARK_SIZE = 2;//各種始端マークサイズ
		static const std::size_t END_MARK_SIZE = 2;//各種終端マークサイズ
		static const unsigned char SIGNATURE[SIGNATURE_SIZE];//シグネチャ
		static const unsigned char TERMINATOR[TERMINATOR_SIZE];//ターミネータ
		static const unsigned char BLOCK_BEGIN[BEGIN_MARK_SIZE];//ブロック始端
		static const unsigned char BLOCK_END[END_MARK_SIZE];//ブロック終端
		static const unsigned char ARRAY_BEGIN[BEGIN_MARK_SIZE];//配列始端
		static const unsigned char ARRAY_END[END_MARK_SIZE];//配列終端
		static const unsigned char ELEM_BEGIN[BEGIN_MARK_SIZE];//要素始端
		static const unsigned char ELEM_END[END_MARK_SIZE];//要素終端
		static const unsigned char ITEM_BEGIN[BEGIN_MARK_SIZE];//データ項目始端
		static const unsigned char ITEM_END[END_MARK_SIZE];//データ項目終端
	public:
		//コンストラクタ
		inline binaryArchiveFormatBase();
		//親を受け取るコンストラクタ
		//※処理階層が深くなるごとにコピーが行われる
		inline binaryArchiveFormatBase(binaryArchiveFormatBase& parent);
		//デストラクタ
		inline ~binaryArchiveFormatBase();
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/archive/binary_archive_format_base.inl>

#endif//GASHA_INCLUDED_ARCHIVE_BINARY_ARCHIVE_FORMAT_BASE_H

// End of file

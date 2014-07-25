#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_TEXT_ARCHIVE_FORMAT_BASE_INL
#define GASHA_INCLUDED_ARCHIVE_TEXT_ARCHIVE_FORMAT_BASE_INL

//--------------------------------------------------------------------------------
// archive/text_archive_format_base.inl
// アーカイブ/テキストアーカイブ形式基底クラス【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/text_archive_format_base.h>//アーカイブ/テキストアーカイブ形式基底クラス【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/テキストアーカイブ形式基底クラス
	//--------------------------------------------------------------------------------

	//--------------------
	//テキスト形式アーカイブクラス（共通）

	//コンストラクタ
	inline textArchiveFormatBase::textArchiveFormatBase() :
		GASHA_ archive::archiveFormatBase()
	{}

	//親を受け取るコンストラクタ
	textArchiveFormatBase::textArchiveFormatBase(textArchiveFormatBase& parent) :
		GASHA_ archive::archiveFormatBase(parent)
	{}

	//デストラクタ
	textArchiveFormatBase::~textArchiveFormatBase()
	{}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_TEXT_ARCHIVE_FORMAT_BASE_INL

// End of file

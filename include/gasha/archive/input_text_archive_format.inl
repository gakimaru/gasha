#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_INPUT_TEXT_ARCHIVE_FORMAT_INL
#define GASHA_INCLUDED_ARCHIVE_INPUT_TEXT_ARCHIVE_FORMAT_INL

//--------------------------------------------------------------------------------
// archive/input_text_archive_format.inl
// アーカイブ/テキストアーカイブ形式クラス（読み込み用）【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/input_text_archive_format.h>//アーカイブ/テキストアーカイブ形式クラス（読み込み用）【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/テキストアーカイブ形式クラス（読み込み用）
	//--------------------------------------------------------------------------------

	//--------------------
	//テキスト形式アーカイブクラス（アーカイブ読み込み用）

	//コンストラクタ
	inline inputTextArchiveFormat::inputTextArchiveFormat() :
		GASHA_ archive::textArchiveFormatBase()
	{}
	
	//親を受け取るコンストラクタ
	inline inputTextArchiveFormat::inputTextArchiveFormat(inputTextArchiveFormat& parent) :
		GASHA_ archive::textArchiveFormatBase(parent)
	{}
	
	//デストラクタ
	inline inputTextArchiveFormat::~inputTextArchiveFormat()
	{}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_INPUT_TEXT_ARCHIVE_FORMAT_INL

// End of file

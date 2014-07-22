#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_INPUT_BINARY_ARCHIVE_FORMAT_INL
#define GASHA_INCLUDED_ARCHIVE_INPUT_BINARY_ARCHIVE_FORMAT_INL

//--------------------------------------------------------------------------------
// archive/input_binary_archive_format.inl
// アーカイブ/バイナリアーカイブ形式クラス（読み込み用）【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/input_binary_archive_format.h>//アーカイブ/バイナリアーカイブ形式クラス（読み込み用）【宣言部】

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
	
	//コンストラクタ
	inline inputBinaryArchiveFormat::inputBinaryArchiveFormat() :
		GASHA_ archive::binaryArchiveFormatBase(),
		m_readSizeForPrevDataItem(0)
	{}
	
	//親を受け取るコンストラクタ
	inline inputBinaryArchiveFormat::inputBinaryArchiveFormat(inputBinaryArchiveFormat& parent) :
		GASHA_ archive::binaryArchiveFormatBase(parent),
		m_readSizeForPrevDataItem(0)
	{}
	
	//デストラクタ
	inline inputBinaryArchiveFormat::~inputBinaryArchiveFormat()
	{}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_INPUT_BINARY_ARCHIVE_FORMAT_INL

// End of file

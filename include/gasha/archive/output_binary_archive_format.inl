#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_OUTPUT_BINARY_ARCHIVE_FORMAT_INL
#define GASHA_INCLUDED_ARCHIVE_OUTPUT_BINARY_ARCHIVE_FORMAT_INL

//--------------------------------------------------------------------------------
// archive/output_binary_archive_format.inl
// アーカイブ/バイナリアーカイブ形式クラス（書き込み用）【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/output_binary_archive_format.h>//アーカイブ/バイナリアーカイブ形式クラス（書き込み用）【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/バイナリアーカイブ形式クラス（書き込み用）
	//--------------------------------------------------------------------------------

	//--------------------
	//バイナリ形式アーカイブクラス（アーカイブ書き込み用）
	
	//コンストラクタ
	inline outputBinaryArchiveFormat::outputBinaryArchiveFormat() :
		GASHA_ archive::binaryArchiveFormatBase()
	{}
	
	//親を受け取るコンストラクタ
	inline outputBinaryArchiveFormat::outputBinaryArchiveFormat(outputBinaryArchiveFormat& parent) :
		GASHA_ archive::binaryArchiveFormatBase(parent)
	{}
	
	//デストラクタ
	inline outputBinaryArchiveFormat::~outputBinaryArchiveFormat()
	{}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_OUTPUT_BINARY_ARCHIVE_FORMAT_INL

// End of file

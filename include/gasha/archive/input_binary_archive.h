#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_INPUT_BINARY_ARCHIVE_H
#define GASHA_INCLUDED_ARCHIVE_INPUT_BINARY_ARCHIVE_H

//--------------------------------------------------------------------------------
// archive/input_binary_archive.h
// アーカイブ/バイナリアーカイブクラス（読み込み用）【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/input_archive.h>//アーカイブ/アーカイブ読み込みクラス
#include <gasha/archive/input_binary_archive_format.h>//アーカイブ/バイナリアーカイブ形式クラス（読み込み用）

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/バイナリアーカイブクラス（読み込み用）
	//--------------------------------------------------------------------------------

	//--------------------
	//バイナリ形式アーカイブ読み込みクラス
	using inputBinaryArchive = GASHA_ archive::inputArchive<GASHA_ archive::inputBinaryArchiveFormat>;

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_INPUT_BINARY_ARCHIVE_H

// End of file

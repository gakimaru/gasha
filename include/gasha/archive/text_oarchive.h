#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_TEXT_OARCHIVE_H
#define GASHA_INCLUDED_ARCHIVE_TEXT_OARCHIVE_H

//--------------------------------------------------------------------------------
// archive/text_oarchive.h
// アーカイブ/テキストアーカイブクラス（書き込み用）【宣言部】
// ※Boost C++ 形式
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/output_archive.h>//アーカイブ/アーカイブ書き込みクラス
#include <gasha/archive/output_text_archive_format.h>//アーカイブ/テキストアーカイブ形式クラス（書き込み用）

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/テキストアーカイブクラス（書き込み用）
	//--------------------------------------------------------------------------------

	//--------------------
	//テキスト形式アーカイブ書き込みクラス
	//※Boost C++ 形式
	using text_oarchive = GASHA_ archive::outputArchive<GASHA_ archive::outputTextArchiveFormat>;

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_TEXT_OARCHIVE_H

// End of file

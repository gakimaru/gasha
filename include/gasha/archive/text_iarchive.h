#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_TEXT_IARCHIVE_H
#define GASHA_INCLUDED_ARCHIVE_TEXT_IARCHIVE_H

//--------------------------------------------------------------------------------
// archive/text_iarchive.h
// アーカイブ/テキストアーカイブクラス（読み込み用）【宣言部】
// ※Boost C++ 形式
// ※未実装
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#if 0//未実装

#include <gasha/archive/input_archive.h>//アーカイブ/アーカイブ読み込みクラス
#include <gasha/archive/input_text_archive_format.h>//アーカイブ/テキストアーカイブ形式クラス（読み込み用）

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/テキストアーカイブクラス（読み込み用）
	//--------------------------------------------------------------------------------

	//--------------------
	//テキスト形式アーカイブ読み込みクラス
	//※Boost C++ 形式
	using text_iarchive = GASHA_ archive::inputArchive<GASHA_ archive::inputTextArchiveFormat>;

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//未実装

#endif//GASHA_INCLUDED_ARCHIVE_TEXT_IARCHIVE_H

// End of file

#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_TEXT_ARCHIVE_FORMAT_BASE_H
#define GASHA_INCLUDED_ARCHIVE_TEXT_ARCHIVE_FORMAT_BASE_H

//--------------------------------------------------------------------------------
// archive/text_archive_format_base.h
// アーカイブ/テキストアーカイブ形式基底クラス【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/archive_format_base.h>//アーカイブ/アーカイブ形式基底クラス

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
	class textArchiveFormatBase : public GASHA_ archive::archiveFormatBase
	{
	public:
		//コンストラクタ
		inline textArchiveFormatBase();
		//親を受け取るコンストラクタ
		//※処理階層が深くなるごとにコピーが行われる
		inline textArchiveFormatBase(textArchiveFormatBase& parent);
		//デストラクタ
		inline ~textArchiveFormatBase();
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/archive/text_archive_format_base.inl>

#endif//GASHA_INCLUDED_ARCHIVE_TEXT_ARCHIVE_FORMAT_BASE_H

// End of file

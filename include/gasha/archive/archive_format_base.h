#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_ARCHIVE_FORMAT_BASE_H
#define GASHA_INCLUDED_ARCHIVE_ARCHIVE_FORMAT_BASE_H

//--------------------------------------------------------------------------------
// archive/archive_format_base.h
// アーカイブ/アーカイブ形式基底クラス【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/アーカイブ形式基底クラス
	//--------------------------------------------------------------------------------

	//--------------------
	//アーカイブ形式基底クラス
	class archiveFormatBase
	{
	public:
		//コンストラクタ
		inline archiveFormatBase();
		//親を受け取るコンストラクタ
		//※処理階層が深くなるごとにコピーが行われる
		inline archiveFormatBase(archiveFormatBase& parent);
		//デストラクタ
		inline ~archiveFormatBase();
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/archive/archive_format_base.inl>

#endif//GASHA_INCLUDED_ARCHIVE_ARCHIVE_FORMAT_BASE_H

// End of file

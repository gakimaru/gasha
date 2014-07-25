#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_ADAPTER_INL
#define GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_ADAPTER_INL

//--------------------------------------------------------------------------------
// archive/input_archive_adapter.inl
// アーカイブ/アーカイブ読み込みアダプター【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/input_archive_adapter.h>//アーカイブ/アーカイブ読み込みアダプター【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/アーカイブ読み込みアダプター
	//--------------------------------------------------------------------------------

	//--------------------
	//アーカイブ読み込みアダプタークラス

	//コンストラクタ
	inline inputArchiveAdapter::inputArchiveAdapter(GASHA_ archive::archiveBase& arc) :
		GASHA_ archive::archiveAdapterBase(arc)
	{}
	//デストラクタ
	inline inputArchiveAdapter::~inputArchiveAdapter()
	{}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_ADAPTER_INL

// End of file

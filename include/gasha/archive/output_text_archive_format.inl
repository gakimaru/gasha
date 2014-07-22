#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_OUTPUT_TEXT_ARCHIVE_FORMAT_INL
#define GASHA_INCLUDED_ARCHIVE_OUTPUT_TEXT_ARCHIVE_FORMAT_INL

//--------------------------------------------------------------------------------
// archive/output_text_archive_format.inl
// アーカイブ/テキストアーカイブ形式クラス（書き込み用）【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/output_text_archive_format.h>//アーカイブ/テキストアーカイブ形式クラス（書き込み用）【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/テキストアーカイブ形式クラス（書き込み用）
	//--------------------------------------------------------------------------------

	//--------------------
	//テキスト形式アーカイブクラス（アーカイブ書き込み用）
	
	//コンストラクタ
	inline outputTextArchiveFormat::outputTextArchiveFormat() :
		textArchiveFormatBase(),
		m_parent(nullptr),
		m_nestLevel(0),
		m_process(PROCESS_UNKNOWN),
		m_blockIndex(0),
		m_arrayBlockIndex(0),
		m_elemIndex(0),
		m_itemIndex(0),
		m_parentProcess(PROCESS_UNKNOWN),
		m_parentBlockIndex(0),
		m_parentArrayBlockIndex(0),
		m_parentElemIndex(0),
		m_parentItemIndex(0)
	{}
	
	//親を受け取るコンストラクタ
	inline outputTextArchiveFormat::outputTextArchiveFormat(outputTextArchiveFormat& src) :
		textArchiveFormatBase(src),
		m_parent(&src),
		m_nestLevel(src.m_nestLevel + 1),
		m_process(src.m_process),
		m_blockIndex(0),
		m_arrayBlockIndex(0),
		m_elemIndex(0),
		m_itemIndex(0),
		m_parentProcess(src.m_process),
		m_parentBlockIndex(src.m_blockIndex),
		m_parentArrayBlockIndex(src.m_arrayBlockIndex),
		m_parentElemIndex(src.m_elemIndex),
		m_parentItemIndex(src.m_itemIndex)
	{}
	
	//デストラクタ
	inline outputTextArchiveFormat::~outputTextArchiveFormat()
	{}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_OUTPUT_TEXT_ARCHIVE_FORMAT_INL

// End of file

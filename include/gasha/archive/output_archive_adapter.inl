#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_ADAPTER_INL
#define GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_ADAPTER_INL

//--------------------------------------------------------------------------------
// archive/output_archive_adapter.inl
// アーカイブ/アーカイブ書き込みアダプタークラス【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/output_archive_adapter.h>//アーカイブ/アーカイブ書き込みアダプタークラス【宣言部】

#include <gasha/string.h>//文字列処理

#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/アーカイブ書き込みアダプタークラス
	//--------------------------------------------------------------------------------

	//--------------------
	//アーカイブ書き込みアダプタークラス

	//バッファへの文字列書き込み
	template<typename... Tx>
	std::size_t outputArchiveAdapter::print(GASHA_ archive::results& result, const char* fmt, Tx&&... args)
	{
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		const std::size_t remain = m_arc.remain();
		std::size_t pos = 0;
		const std::size_t ret = GASHA_ spprintf(reinterpret_cast<char*>(m_arc.m_buff + m_arc.m_buffPos), remain, pos, fmt, std::forward<Tx>(args)...);
		m_arc.m_buffPos += ret;
		return ret;
	}
	//コンストラクタ
	inline outputArchiveAdapter::outputArchiveAdapter(archiveBase& arc) :
		GASHA_ archive::archiveAdapterBase(arc)
	{}
	//デストラクタ
	inline outputArchiveAdapter::~outputArchiveAdapter()
	{}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_ADAPTER_INL

// End of file

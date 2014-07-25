#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_ADAPTER_H
#define GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_ADAPTER_H

//--------------------------------------------------------------------------------
// archive/output_archive_adapter.h
// アーカイブ/アーカイブ書き込みアダプタークラス【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/archive_adapter_base.h>//アーカイブ/アーカイブアダプター基底クラス
#include <gasha/serialization/type_ctrl_base.h>//アーカイブ/型操作基底クラス
#include <gasha/archive/results.h>//アーカイブ/処理結果

#include <cstddef>//std::size_t

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
	class outputArchiveAdapter : public GASHA_ archive::archiveAdapterBase
	{
		template<class FORMAT>
		friend class outputArchive;
	public:
		//メソッド
		//バッファへのデータ書き込み
		//※要求サイズが全て書き込めなかったら false を返す
		bool write(const void* data, const std::size_t size, std::size_t& written_size);
		//※処理結果オブジェクト使用版
		bool write(GASHA_ archive::results& result, const void* data, const std::size_t size, std::size_t* written_size = nullptr);
		//バッファへの文字列書き込み
		template<typename... Tx>
		std::size_t print(GASHA_ archive::results& result, const char* fmt, Tx&&... args);
		//バッファへの文字列書き込み
		std::size_t printWithFunc(GASHA_ archive::results& result, GASHA_ serialization::toStrFunc_type to_str_func, const void* data_p, const std::size_t data_size);
		//バッファへのインデント書き込み
		void printIndent(GASHA_ archive::results& result, const int add);
	public:
		//コンストラクタ
		inline outputArchiveAdapter(GASHA_ archive::archiveBase& arc);
		//デストラクタ
		inline ~outputArchiveAdapter();
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/archive/output_archive_adapter.inl>

#endif//GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_ADAPTER_H

// End of file

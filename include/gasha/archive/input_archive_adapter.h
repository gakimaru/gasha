#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_ADAPTER_H
#define GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_ADAPTER_H

//--------------------------------------------------------------------------------
// archive/input_archive_adapter.h
// アーカイブ/アーカイブ読み込みアダプター【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/archive_adapter_base.h>//アーカイバブ/アーカイブアダプター基底クラス
#include <gasha/archive/results.h>//アーカイブ/処理結果
#include <gasha/serialization/type_ctrl_base.h>//アーカイブ/型操作基底クラス

#include <cstddef>//std::size_t

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
	class inputArchiveAdapter : public GASHA_ archive::archiveAdapterBase
	{
		template<class FORMAT>
		friend class inputArchive;
	public:
		//メソッド
		//バッファからのデータ読み込み
		//※要求サイズが全て書き込めなかったら false を返す
		bool read(void* data, const std::size_t size, std::size_t& read_size);
		//※処理結果オブジェクト使用版
		bool read(GASHA_ archive::results& result, void* data, const std::size_t size, std::size_t* read_size = nullptr);
		//バッファからのデータ読み込み
		//※要求サイズが全て書き込めなかったら false を返す
		bool readWithFunc(GASHA_ serialization::fromMemFunc_type from_mem_func, void* data, const std::size_t dst_size, const std::size_t src_size, std::size_t& read_size);
		//※処理結果オブジェクト使用版
		bool readWithFunc(GASHA_ archive::results& result, GASHA_ serialization::fromMemFunc_type from_mem_func, void* data, const std::size_t dst_size, const std::size_t src_size, std::size_t* read_size = nullptr);
	public:
		//コンストラクタ
		inline inputArchiveAdapter(GASHA_ archive::archiveBase& arc);
		//デストラクタ
		inline ~inputArchiveAdapter();
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/archive/input_archive_adapter.inl>

#endif//GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_ADAPTER_H

// End of file

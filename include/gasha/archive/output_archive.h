#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_H
#define GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_H

//--------------------------------------------------------------------------------
// archive/output_archive.h
// アーカイブ/アーカイブ書き込みクラス【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/archive_base.h>//アーカイブ/アーカイブ基底クラス
#include <gasha/serialization/item_info.h>//アーカイブ/データ項目情報

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/アーカイブ書き込みクラス
	//--------------------------------------------------------------------------------

	//--------------------
	//アーカイブ書き込みクラス
	template<class FORMAT>
	class outputArchive : public GASHA_ archive::archiveBase
	{
	public:
		//定数
		enum class_type : bool
		{
			isInputClass = false,//読み込みクラスか？
			isOutputClass = true,//書き込みクラスか？
		};
	public:
		//型
		typedef FORMAT format_type;//アーカイブ形式型
	private:
		//定数
		//ステータス
		enum status_type : int
		{
			SERIALIZE_BEGINNING,//シリアライズ中
			SERIALIZE_PHASE_BLOCK,//シリアライズ：ブロック処理フェーズ
			SERIALIZE_PHASE_ARRAY,//シリアライズ：配列ブロック処理フェーズ
			SERIALIZE_PHASE_ELEMENT,//シリアライズ：要素処理フェーズ
			SERIALIZE_PHASE_SAVE_DATA,//シリアライズ：データのセーブフェーズ
			SERIALIZE_PHASE_COLLECT,//シリアライズ：収集フェーズ
			SERIALIZE_PHASE_COLLECT_END,//シリアライズ：収集フェーズ終了
			SERIALIZE_ENDED,//シリアライズ終了
		};
	private:
		//アクセッサ
		inline status_type status() const;//ステータスを取得
		inline status_type setStatus(const status_type status);//ステータスを変更　※変更前のステータスを返す
	public:
		//オペレータ
		//「&」オペレータ
		//※データ項目指定＆データ書き込み用処理
		template<class T>
		outputArchive& operator&(const GASHA_ serialization::itemInfo<T> item_obj);
		//「<<」オペレータ
		//※データ書き込み
		template<class T>
		outputArchive& operator<<(const GASHA_ serialization::itemInfo<T> item_obj);
	public:
		//コンストラクタ
		inline outputArchive(void* buff, const std::size_t buff_size, void* work_buff, std::size_t work_buff_size);
		template<typename BUFF_T, std::size_t BUFF_SIZE, typename WORK_T, std::size_t WORK_SIZE>
		inline outputArchive(BUFF_T(&buff)[BUFF_SIZE], WORK_T(&work_buff)[WORK_SIZE]);
		template<typename WORK_T, std::size_t WORK_SIZE>
		inline outputArchive(void* buff, const std::size_t buff_size, WORK_T(&work_buff)[WORK_SIZE]);
		//親を受け取るコンストラクタ
		//※処理階層が深くなるごとにコピーが行われる
		inline outputArchive(outputArchive& parent, const status_type status);
		//デストラクタ
		inline ~outputArchive();
	private:
		//フィールド
		outputArchive* m_parent;//親アーカイブオブジェクト
		format_type m_format;//アーカイブ形式オブジェクト
		status_type m_status;//ステータス
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/archive/output_archive.inl>

#endif//GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_H

// End of file

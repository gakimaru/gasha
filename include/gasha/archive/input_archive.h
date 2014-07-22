#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_H
#define GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_H

//--------------------------------------------------------------------------------
// archive/input_archive.h
// アーカイブ/アーカイブ読み込みクラス【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
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
	//アーカイブ/アーカイブ読み込みクラス
	//--------------------------------------------------------------------------------

	//--------------------
	//アーカイブ読み込みクラス
	template<class FORMAT>
	class inputArchive : public GASHA_ archive::archiveBase
	{
	public:
		//定数
		enum class_type : bool
		{
			isInputClass = true,//読み込みクラスか？
			isOutputClass = false,//書き込みクラスか？
		};
	public:
		//型
		typedef FORMAT format_type;//アーカイブ形式型
	private:
		//定数
		//ステータス
		enum status_type : int
		{
			DESERIALIZE_BEGINNING,//デシリアライズ中
			DESERIALIZE_PHASE_BLOCK,//シリアライズ：ブロック処理フェーズ
			DESERIALIZE_PHASE_ARRAY,//シリアライズ：配列ブロック処理フェーズ
			DESERIALIZE_PHASE_ELEMENT,//シリアライズ：要素処理フェーズ
			DESERIALIZE_PHASE_BEFORE_LOAD,//デシリアライズ：ロード前処理フェーズ
			DESERIALIZE_PHASE_MAKE_LIST,//デシリアライズ：対象データ項目リスト作成フェーズ
			DESERIALIZE_PHASE_LOAD_DATA,//デシリアライズ：データのロードフェーズ
			DESERIALIZE_PHASE_LOAD_OBJECT,//デシリアライズ：オブジェクトのロードフェーズ
			DESERIALIZE_PHASE_LOAD_OBJECT_END,//デシリアライズ：オブジェクトのロードフェーズ終了
			DESERIALIZE_PHASE_NOTICE_UNRECOGNIZED,//デシリアライズ：認識できなかったデータ項目の通知フェーズ
			DESERIALIZE_PHASE_NOTICE_UNLOADED,//デシリアライズ：ロードできなかったデータ項目の通知フェーズ
			DESERIALIZE_PHASE_AFTEER_LOAD,//デシリアライズ：ロード後処理フェーズ
			DESERIALIZE_PHASE_BEFORE_DISTRIBUTE,//デシリアライズ：分配前フェーズ
			DESERIALIZE_PHASE_DISTRIBUTE,//デシリアライズ：分配フェーズ
			DESERIALIZE_PHASE_DISTRIBUTE_OBJECT,//デシリアライズ：オブジェクトの分配フェーズ
			DESERIALIZE_PHASE_DISTRIBUTE_OBJECT_END,//デシリアライズ：オブジェクトの分配フェーズ終了
			DESERIALIZE_PHASE_AFTER_DISTRIBUTE,//デシリアライズ：分配後フェーズ
			DESERIALIZE_ENDED,//デシリアライズ終了
		};
	private:
		//アクセッサ
		inline status_type status() const;//ステータスを取得
		inline status_type setStatus(const status_type status);//ステータスを変更　※変更前のステータスを返す
		inline GASHA_ serialization::itemInfoBase* targetObjItem();//オブジェクト処理の対象データ項目を取得
		inline const GASHA_ serialization::itemInfoBase* targetObjItem() const;//オブジェクト処理の対象データ項目を取得
		inline void setTargetObjItem(GASHA_ serialization::itemInfoBase& item);//オブジェクト処理の対象データ項目をセット
		inline void resetTargetObjItem();//オブジェクト処理の対象データ項目をリセット
		inline GASHA_ serialization::itemInfoBase* targetObjItemDelegate();//オブジェクト処理の対象データ項目の委譲データ項目を取得
		inline const GASHA_ serialization::itemInfoBase* targetObjItemDelegate() const;//オブジェクト処理の対象データ項目の委譲データ項目を取得
		inline void setTargetObjItemDelegate(GASHA_ serialization::itemInfoBase* item);//オブジェクト処理の対象データ項目の委譲データ項目をセット
		inline void resetTargetObjItemDelegate();//オブジェクト処理の対象データ項目の委譲データ項目をリセット
	public:
		//オペレータ
		//「&」オペレータ
		//※データ項目指定用処理
		template<class T>
		inputArchive& operator&(const GASHA_ serialization::itemInfo<T> item_obj);
		
		//「>>」オペレータ
		//※データ読み込み
		template<class T>
		inputArchive& operator>>(GASHA_ serialization::itemInfo<T> item_obj_now);
	public:
		//コンストラクタ
		inline inputArchive(const void* buff, const std::size_t buff_size, void* work_buff, std::size_t work_buff_size);
		template<typename BUFF_T, std::size_t BUFF_SIZE, typename WORK_T, std::size_t WORK_SIZE>
		inline inputArchive(const BUFF_T(&buff)[BUFF_SIZE], WORK_T(&work_buff)[WORK_SIZE]);
		template<typename WORK_T, std::size_t WORK_SIZE>
		inline inputArchive(const void* buff, const std::size_t buff_size, WORK_T(&work_buff)[WORK_SIZE]);
		//親を受け取るコンストラクタ
		//※処理階層が深くなるごとにコピーが行われる
		inline inputArchive(inputArchive& parent, const status_type status);
		//デストラクタ
		~inputArchive();
	private:
		//フィールド
		inputArchive* m_parent;//親アーカイブオブジェクト
		format_type m_format;//アーカイブ形式オブジェクト
		status_type m_status;//ステータス
		GASHA_ serialization::itemInfoBase* m_targetObjItem;//オブジェクト処理の対象データ項目
		GASHA_ serialization::itemInfoBase* m_targetObjItemDelegate;//オブジェクト処理の対象データ項目の委譲データ項目
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/archive/input_archive.inl>

#endif//GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_H

// End of file

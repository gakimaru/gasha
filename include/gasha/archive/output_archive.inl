#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_INL
#define GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_INL

//--------------------------------------------------------------------------------
// archive/output_archive.inl
// アーカイブ/アーカイブ書き込みクラス【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/output_archive.h>//アーカイブ/アーカイブ書き込みクラス【宣言部】

#include <gasha/serialization/type_ctrl_base.h>//アーカイブ/型操作基底クラス
#include <gasha/serialization/version_def.h>//シリアライズ/バージョン定義
#include <gasha/serialization/version.h>//シリアライズ/バージョン
#include <gasha/serialization/serialize.h>//シリアライズ/シリアライズ／デシリアライズ共通処理
#include <gasha/serialization/save.h>//シリアライズ/セーブ処理
#include <gasha/serialization/collector.h>//シリアライズ/データ収集処理
#include <gasha/serialization/error.h>//シリアライズ/致命的エラー発生時処理

#include <gasha/fast_string.h>//高速文字列処理
#include <gasha/simple_assert.h>//シンプルアサーション

#include <cstddef>//std::size_t
#include <typeinfo>//std::type_info

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

	//ステータスを取得
	template<class FORMAT>
	inline typename outputArchive<FORMAT>::status_type outputArchive<FORMAT>::status() const
	{
		return m_status;
	}
	
	//ステータスを変更
	template<class FORMAT>
	inline typename outputArchive<FORMAT>::status_type outputArchive<FORMAT>::setStatus(const typename outputArchive<FORMAT>::status_type status)
	{
		status_type prev = m_status;
		m_status = status;
		return prev;
	}

	//「&」オペレータ
	//※データ項目指定＆データ書き込み用処理
	template<class FORMAT>
	template<class T>
	outputArchive<FORMAT>& outputArchive<FORMAT>::operator&(const GASHA_ serialization::itemInfo<T> item_obj)
	{
		if (m_result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return *this;

		//現在処理中のアーカイブオブジェクト
		outputArchive& arc = *this;

		//std::printf("[operator&] name=\"%s\"(0x%08x), typeName=%s, item=0x%p, size=%d, arrNum=%d, isObj=%d, isArr=%d, isPtr=%d, isNul=%d, isOnlyOnMem=%d, isOnlyOnSaveData=%d\n",
		//	item_obj.m_name, item_obj.m_nameCrc, item_obj.m_itemType->name(), item_obj.m_itemP, item_obj.m_itemSize, item_obj.m_arrNum, item_obj.isObj(), item_obj.isArr(), item_obj.isPtr(), item_obj.isNul(), item_obj.isOnlyOnMem(), item_obj.isOnlyOnSaveData());

		//SERIALIZE_PHASE_SAVE_DATA 時以外にこの処理に来るのはプログラムに間違いがある
		//※収集処理で operator&() を使っている時など
		GASHA_SIMPLE_ASSERT(arc.status() == SERIALIZE_PHASE_SAVE_DATA, "Illegal phase.");

		//文字列型専用処理
		if (typeid(T) == typeid(GASHA_ serialization::str_type))
		{
			if (item_obj.m_itemP)
			{
				//文字列長を格納し直す
				*const_cast<std::size_t*>(&item_obj.m_itemSize) = GASHA_ strlen_fast(reinterpret_cast<const char*>(item_obj.m_itemP)) + 1;
			}
			else
			{
				//ヌル時は0に
				*const_cast<std::size_t*>(&item_obj.m_itemSize) = 0;
			}
		}

		//データ項目を記録
		//※重複チェックのため
		const bool result = addItem(item_obj);
		if (!result)
			return *this;//データ項目が重複していたので処理しない（致命的なエラーとせず、処理は続行する）
		
		//データ書き込み
		if (item_obj.isObj())
		{
			//オブジェクトなら operator<<() で書き込み
			arc << item_obj;
		}
		else
		{
			//データをアーカイブに記録
			arc.m_format.writeDataItem(arc, item_obj, item_obj);

			//データ書き込み済み
			item_obj.setIsAlready();
		}
		
		return *this;
	}
	//「<<」オペレータ
	//※データ書き込み
	template<class FORMAT>
	template<class T>
	outputArchive<FORMAT>& outputArchive<FORMAT>::operator<<(const GASHA_ serialization::itemInfo<T> item_obj)
	{
		if (m_result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return *this;

		//現在処理中のアーカイブオブジェクト
		outputArchive& arc = *this;

		//std::printf("[operator<<] name=\"%s\"(0x%08x), typeName=%s, item=0x%p, size=%d, arrNum=%d, isObj=%d, isArr=%d, isPtr=%d, isNul=%d, isOnlyOnMem=%d, isOnlyOnSaveData=%d\n",
		//	item_obj.m_name, item_obj.m_nameCrc, item_obj.m_itemType->name(), item_obj.m_itemP, item_obj.m_itemSize, item_obj.m_arrNum, item_obj.isObj(), item_obj.isArr(), item_obj.isPtr(), item_obj.isNul(), item_obj.isOnlyOnMem(), item_obj.isOnlyOnSaveData());

		//SERIALIZE_BEGINNING, SERIALIZE_PHASE_SAVE_DATA, SERIALIZE_PHASE_COLLECT 時以外に
		//この処理に来るのはプログラムに間違いがある
		GASHA_SIMPLE_ASSERT(
			arc.status() == SERIALIZE_BEGINNING ||
			arc.status() == SERIALIZE_PHASE_SAVE_DATA ||
			arc.status() == SERIALIZE_PHASE_COLLECT, "Illegal phase.");

		//オブジェクトでなければダメ
		//※何かしらのシリアライズ用関数オブジェクトを実装した型か？
		GASHA_SIMPLE_ASSERT(item_obj.isObj() == true, "Item must be object-type.");

		//ネストレベルが0ならシグネチャーを書き込み
		if (m_nestLevel == 0)
			arc.m_format.writeSignature(arc);//シグネチャー書き込み

		//バージョン取得
		GASHA_ serialization::versionDef<T> ver_def;
		GASHA_ serialization::version ver(ver_def);

		//ブロック開始情報書き込み
		arc.m_format.writeBlockHeader(arc, item_obj, ver);
		
		//ブロック開始
		std::size_t block_size = 0;//ブロックのサイズ
		
		{
			//ブロック処理用の子アーカイブオブジェクトを生成
			outputArchive& parent_arc = arc;
			outputArchive arc(parent_arc, SERIALIZE_PHASE_BLOCK);

			//ヌルでなければ処理する
			if (!item_obj.isNul() && !arc.hasFatalError())
			{
				//配列要素数取得
				const std::size_t array_elem_num = item_obj.m_arrNum;
				
				//配列ブロック開始情報書き込み
				arc.m_format.writeArrayHeader(arc, item_obj, array_elem_num);

				//配列ブロック開始
				std::size_t array_block_size = 0;//配列ブロックのサイズ

				{
					//配列ブロック処理用の孫アーカイブオブジェクトを生成
					outputArchive& parent_arc = arc;
					outputArchive arc(parent_arc, SERIALIZE_PHASE_ARRAY);

					//データ書き込み先の先頭ポインタ（配列の先頭）を記憶
					//※配列ループ処理中に item_obj.m_itemP を書き換えるので、元に戻せるようにしておく
					const void* item_p_top = item_obj.m_itemP;

					//要素ループ
					const std::size_t loop_elem_num = item_obj.extent();//配列の要素数もしくは（配列じゃなければ）1を返す
					for (std::size_t index = 0; index < loop_elem_num && !arc.hasFatalError(); ++index)
					{
						//要素開始情報書き込み
						arc.m_format.writeElemHeader(arc, item_obj, index);

						//要素開始
						short items_num = 0;//データ項目数
						std::size_t elem_size = 0;//この要素のデータサイズ
						{
							//要素処理用のひ孫アーカイブオブジェクトを生成
							outputArchive& parent_arc = arc;
							outputArchive arc(parent_arc, SERIALIZE_PHASE_ELEMENT);
							
							//データのセーブフェーズに変更
							arc.setStatus(SERIALIZE_PHASE_SAVE_DATA);

							//シリアライズ処理（シリアライズ＆デシリアライズ兼用処理）呼び出し
							{
								GASHA_ serialization::serialize<outputArchive, T> functor;
								functor(arc, item_obj.template getConst<T>(), ver, ver, nullptr);
							}

							//セーブ処理（シリアライズ専用処理）呼び出し
							{
								GASHA_ serialization::save<outputArchive, T> functor;
								functor(arc, item_obj.template getConst<T>(), ver);
							}

							//要素終了情報書き込み
							//※例えば、バイナリ形式では、要素のヘッダ部にデータ項目数とデータサイズを書き込み、
							//　要素の最後までシークする
							arc.m_format.writeElemFooter(parent_arc, arc, item_obj, index, items_num, elem_size);

							//データ書き込み先のポインタを配列の次の要素に更新
							if (item_obj.m_itemP)
							{
								*const_cast<void**>(&item_obj.m_itemP) = reinterpret_cast<T*>(const_cast<void*>(item_obj.m_itemP)) + 1;
							}
						}
					}

					//配列の先頭ポインタ（元のポインタ）に戻す
					*const_cast<const void**>(&item_obj.m_itemP) = item_p_top;

					//配列ブロック終了情報書き込み
					//※例えば、バイナリ形式では、配列ブロックのヘッダ部に配列要素数とデータサイズを書き込み、
					//　要素の最後までシークする
					arc.m_format.writeArrayFooter(parent_arc, arc, item_obj, array_block_size);
				}
			}

			//データ収集処理（シリアライズ専用処理）呼び出し
			arc.setStatus(SERIALIZE_PHASE_COLLECT);
			{
				GASHA_ serialization::collector<outputArchive, T> functor;
				functor(arc, item_obj.template getConst<T>(), ver);
			}
			arc.setStatus(SERIALIZE_PHASE_COLLECT_END);

			//ブロック終了情報書き込み
			//※例えば、バイナリ形式では、ブロックのヘッダ部にデータサイズを書き込み、
			//　ブロックの最後までシークする
			arc.m_format.writeBlockFooter(parent_arc, arc, item_obj, block_size);
		}

		//ネストレベルが0ならターミネータを書き込み
		if (m_nestLevel == 0)
			arc.m_format.writeTerminator(arc);//ターミネータ書き込み

		//データ書き込み済みにする
		item_obj.setIsAlready();

		//処理結果にセーブデータサイズをセットする
		m_result.setSaveDataSize(size());

		//最終的に致命的なエラーがあれば呼び出し
		if (m_nestLevel == 0 && arc.hasFatalError())
		{
			GASHA_ serialization::fatalSerializeErrorOccurred<outputArchive, T> functor;
			functor(arc, item_obj.template get<T>(), ver);
		}

		return *this;
	}

	//コンストラクタ
	template<class FORMAT>
	inline outputArchive<FORMAT>::outputArchive(void* buff, const std::size_t buff_size, void* work_buff, std::size_t work_buff_size) :
		GASHA_ archive::archiveBase(buff, buff_size, work_buff, work_buff_size),
		m_parent(nullptr),
		m_format(),
		m_status(SERIALIZE_BEGINNING)
	{}
	template<class FORMAT>
	template<typename BUFF_T, std::size_t BUFF_SIZE, typename WORK_T, std::size_t WORK_SIZE>
	inline outputArchive<FORMAT>::outputArchive(BUFF_T(&buff)[BUFF_SIZE], WORK_T(&work_buff)[WORK_SIZE]) :
		GASHA_ archive::archiveBase(buff, BUFF_SIZE, work_buff, WORK_SIZE),
		m_parent(nullptr),
		m_format(),
		m_status(SERIALIZE_BEGINNING)
	{}
	template<class FORMAT>
	template<typename WORK_T, std::size_t WORK_SIZE>
	inline outputArchive<FORMAT>::outputArchive(void* buff, const std::size_t buff_size, WORK_T(&work_buff)[WORK_SIZE]) :
		GASHA_ archive::archiveBase(buff, buff_size, work_buff, WORK_SIZE),
		m_parent(nullptr),
		m_format(),
		m_status(SERIALIZE_BEGINNING)
	{}

	//親を受け取るコンストラクタ
	template<class FORMAT>
	inline outputArchive<FORMAT>::outputArchive(outputArchive& parent, const status_type status) :
		GASHA_ archive::archiveBase(parent),
		m_parent(&parent),
		m_format(parent.m_format),
		m_status(status)
	{}

	//デストラクタ
	template<class FORMAT>
	inline outputArchive<FORMAT>::~outputArchive()
	{}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_OUTPUT_ARCHIVE_INL

// End of file

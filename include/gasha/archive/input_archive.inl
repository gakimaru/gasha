#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_INL
#define GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_INL

//--------------------------------------------------------------------------------
// archive/input_archive.inl
// アーカイブ/アーカイブ読み込みクラス【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/input_archive.h>//アーカイブ/アーカイブ読み込みクラス【宣言部】

#include <gasha/serialization/version_def.h>//シリアライズ/バージョン定義
#include <gasha/serialization/version.h>//シリアライズ/バージョン
#include <gasha/serialization/serialize.h>//シリアライズ/シリアライズ／デシリアライズ共通処理
#include <gasha/serialization/load.h>//シリアライズ/ロード処理
#include <gasha/serialization/notice.h>//シリアライズ/通知処理
#include <gasha/serialization/distributor.h>//シリアライズ/データ分配処理
#include <gasha/serialization/error.h>//シリアライズ/致命的エラー発生時処理

#include <gasha/iterator.h>//イテレータ操作
#include <gasha/simple_assert.h>//シンプルアサーション

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

	//ステータスを取得
	template<class FORMAT>
	inline typename inputArchive<FORMAT>::status_type inputArchive<FORMAT>::status() const
	{
		return m_status;
	}
	//ステータスを変更　※変更前のステータスを返す
	template<class FORMAT>
	inline typename inputArchive<FORMAT>::status_type inputArchive<FORMAT>::setStatus(const typename inputArchive<FORMAT>::status_type status)
	{
		status_type prev = m_status;
		m_status = status;
		return prev;
	}
	//オブジェクト処理の対象データ項目を取得
	template<class FORMAT>
	inline GASHA_ serialization::itemInfoBase* inputArchive<FORMAT>::targetObjItem()
	{
		return m_targetObjItem;
	}
	//オブジェクト処理の対象データ項目を取得
	template<class FORMAT>
	inline const GASHA_ serialization::itemInfoBase* inputArchive<FORMAT>::targetObjItem() const
	{
		return m_targetObjItem;
	}
	//オブジェクト処理の対象データ項目をセット
	template<class FORMAT>
	inline void inputArchive<FORMAT>::setTargetObjItem(GASHA_ serialization::itemInfoBase& item)
	{
		m_targetObjItem = &item;
	}
	//オブジェクト処理の対象データ項目をリセット
	template<class FORMAT>
	inline void inputArchive<FORMAT>::resetTargetObjItem()
	{
		m_targetObjItem = nullptr;
	}
	//オブジェクト処理の対象データ項目の委譲データ項目を取得
	template<class FORMAT>
	inline GASHA_ serialization::itemInfoBase* inputArchive<FORMAT>::targetObjItemDelegate()
	{
		return m_targetObjItemDelegate;
	}
	//オブジェクト処理の対象データ項目の委譲データ項目を取得
	template<class FORMAT>
	inline const GASHA_ serialization::itemInfoBase* inputArchive<FORMAT>::targetObjItemDelegate() const
	{
		return m_targetObjItemDelegate;
	}
	//オブジェクト処理の対象データ項目の委譲データ項目をセット
	template<class FORMAT>
	inline void inputArchive<FORMAT>::setTargetObjItemDelegate(GASHA_ serialization::itemInfoBase* item)
	{
		m_targetObjItemDelegate = item;
	}
	//オブジェクト処理の対象データ項目の委譲データ項目をリセット
	template<class FORMAT>
	inline void inputArchive<FORMAT>::resetTargetObjItemDelegate()
	{
		m_targetObjItemDelegate = nullptr;
	}

	//「&」オペレータ
	template<class FORMAT>
	template<class T>
	inputArchive<FORMAT>& inputArchive<FORMAT>::operator&(const GASHA_ serialization::itemInfo<T> item_obj)
	{
		if (m_result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return *this;

		//現在処理中のアーカイブオブジェクト
		inputArchive& arc = *this;

		//std::printf("[operator&] name=\"%s\"(0x%08x), typeName=%s, item=0x%p, size=%d, arrNum=%d, isObj=%d, isArr=%d, isPtr=%d, isNul=%d, isOnlyOnMem=%d, isOnlyOnSaveData=%d\n",
		//	item_obj.m_name, item_obj.m_nameCrc, item_obj.m_itemType->name(), item_obj.m_itemP, item_obj.m_itemSize, item_obj.m_arrNum, item_obj.isObj(), item_obj.isArr(), item_obj.isPtr(), item_obj.isNul(), item_obj.isOnlyOnMem(), item_obj.isOnlyOnSaveData());

		//DESERIALIZE_PHASE_MAKE_LIST, DESERIALIZE_PHASE_LOAD_OBJECT,
		//DESERIALIZE_PHASE_LOAD_OBJECT_END 時以外にこの処理に来るのはプログラムに間違いがある
		//※ロード前処理やロード後処理、通知処理、分配処理で operator&() を使っている時など
		GASHA_SIMPLE_ASSERT(
			arc.status() == DESERIALIZE_PHASE_MAKE_LIST ||
			arc.status() == DESERIALIZE_PHASE_LOAD_OBJECT ||
			arc.status() == DESERIALIZE_PHASE_LOAD_OBJECT_END ||
			arc.status() == DESERIALIZE_PHASE_NOTICE_UNRECOGNIZED, "Illegal phase.");

		//オブジェクト処理モード終了時はなにもしない
		if (arc.status() == DESERIALIZE_PHASE_LOAD_OBJECT_END)
		{
			return *this;
		}
		//オブジェクト処理モード時は対象データ項目のみ処理する
		else if (arc.status() == DESERIALIZE_PHASE_LOAD_OBJECT)
		{
			if (*m_targetObjItem == item_obj)
			{
				arc >> item_obj;//オブジェクト読み込み
				arc.setStatus(DESERIALIZE_PHASE_LOAD_OBJECT_END);//オブジェクト処理モード終了
			}
			return *this;
		}
		//認識できなかったオブジェクトの通知モード時は委譲データ項目をセットして終了する
		else if (arc.status() == DESERIALIZE_PHASE_NOTICE_UNRECOGNIZED)
		{
			if (m_targetObjItemDelegate)
			{
				m_targetObjItemDelegate->copyForce(item_obj);
			}
			return *this;
		}

		//一旦は「セーブデータに存在しないデータ項目」という扱いにしておく
		//※ロード終了後もそのままならロードできなかったデータ項目として処理する
		item_obj.setIsOnlyOnMem();
		
		//データ項目を記録
		//※全ての記録が終わった後、データを読み込みながらデータ項目に書き込んでいく
		addItem(item_obj);
		
		return *this;
	}
	
	//「>>」オペレータ
	//※データ読み込み
	template<class FORMAT>
	template<class T>
	inputArchive<FORMAT>& inputArchive<FORMAT>::operator>>(GASHA_ serialization::itemInfo<T> item_obj_now)
	{
		if (m_result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return *this;

		//現在処理中のアーカイブオブジェクト
		inputArchive& arc = *this;

		//std::printf("[operator>>] name=\"%s\"(0x%08x), typeName=%s, item=0x%p, size=%d, arrNum=%d, isObj=%d, isArr=%d, isPtr=%d, isNul=%d, isOnlyOnMem=%d, isOnlyOnSaveData=%d\n",
		//	item_obj_now.m_name, item_obj_now.m_nameCrc, item_obj_now.m_itemType->name(), item_obj_now.m_itemP, item_obj_now.m_itemSize, item_obj_now.m_arrNum, item_obj_now.isObj(), item_obj_now.isArr(), item_obj_now.isPtr(), item_obj_now.isNul(), item_obj_now.isOnlyOnMem(), item_obj_now.isOnlyOnSaveData());

		//DESERIALIZE_BEGINNING, DESERIALIZE_PHASE_MAKE_LIST, DESERIALIZE_PHASE_LOAD_DATA,
		//DESERIALIZE_PHASE_LOAD_OBJECT,DESERIALIZE_PHASE_LOAD_OBJECT_END 時以外に
		//この処理に来るのはプログラムに間違いがある
		//※ロード処理（ロード前後処理含む）、通知処理で operator>>() を使っている時など
		GASHA_SIMPLE_ASSERT(
			arc.status() == DESERIALIZE_BEGINNING ||
			arc.status() == DESERIALIZE_PHASE_MAKE_LIST ||
			arc.status() == DESERIALIZE_PHASE_LOAD_OBJECT ||
			arc.status() == DESERIALIZE_PHASE_LOAD_OBJECT_END ||
			arc.status() == DESERIALIZE_PHASE_DISTRIBUTE_OBJECT ||
			arc.status() == DESERIALIZE_PHASE_DISTRIBUTE_OBJECT_END, "Illegal phase.");

		//データ項目リトライ処理⇒ブロック読み込みのための委譲データ項目を準備
		GASHA_ serialization::itemInfoBase* delebate_item = nullptr;

		//対象データ項目リスト作成フェーズ中なら operator&() に処理を回す
		if (arc.status() == DESERIALIZE_PHASE_MAKE_LIST)
		{
			return *this & item_obj_now;
		}
		//オブジェクト処理モード時は委譲データ項目を取得する
		else if (arc.status() == DESERIALIZE_PHASE_LOAD_OBJECT)
		{
			//委譲データ項目を取得
			delebate_item = m_targetObjItemDelegate;
		}
		//分配オブジェクト処理モード終了時はなにもしない
		else if (arc.status() == DESERIALIZE_PHASE_DISTRIBUTE_OBJECT_END)
		{
			return *this;
		}
		//分配オブジェクトモード時は対象データ項目のみ処理する
		else if (arc.status() == DESERIALIZE_PHASE_DISTRIBUTE_OBJECT)
		{
			if (*m_targetObjItem == item_obj_now)
			{
				//オブジェクト処理モード終了状態にして処理続行
				arc.setStatus(DESERIALIZE_PHASE_DISTRIBUTE_OBJECT_END);
			}
		}

		//パラメータチェック
		//assert(!item_obj_now.isOnlyOnSaveData());
		//※セーブデータにしか存在しないデータは処理不可
		//※配信処理を間違えて以内限り、そのような状態にはならないはず

		//ネストレベルが0ならパース
		if (m_nestLevel == 0)
			arc.m_format.parse(arc);//パース
		
		//ネストレベルが0ならシグネチャーを読み込み
		if (m_nestLevel == 0)
			arc.m_format.readSignature(arc);//シグネチャー読み込み
		
		//バージョン取得
		GASHA_ serialization::versionDef<T> now_ver_def;
		GASHA_ serialization::version now_ver(now_ver_def);//現在のバージョン
		GASHA_ serialization::version ver;//読み込み用のバージョン
		//ブロック開始情報読み込み
		std::size_t block_size = 0;
		GASHA_ serialization::itemInfo<T> item_obj(item_obj_now);
		arc.m_format.readBlockHeader(arc, item_obj_now, delebate_item, now_ver, item_obj, ver, block_size);
		//std::printf("  readDataItem:name=\"%s\"(0x%08x), typeName=%s, item=0x%p, size=%d, arrNum=%d, isObj=%d, isArr=%d, isPtr=%d, isNul=%d\n", item_obj.m_name, item_obj.m_nameCrc, item_obj.m_itemType->name(), item_obj.m_itemP, item_obj.m_itemSize, item_obj.m_arrNum, item_obj.isObj(), item_obj.isArr(), item_obj.isPtr(), item_obj.isNul());

		//ブロック開始
		{
			//ブロック処理用の子アーカイブオブジェクトを生成
			inputArchive& parent_arc = arc;
			inputArchive arc(parent_arc, DESERIALIZE_PHASE_BLOCK);
			
			//集計準備
			const std::size_t elem_num = item_obj.getElemNum();//要素数（配列要素ではなく、非配列なら1）
			std::size_t elem_num_loaded = 0;//実際に読み込んだ要素数（配列要素ではなく、非配列なら1）
			
			if (!item_obj.isNul() && !arc.hasFatalError())//【セーブデータ上の】要素がヌルでなければ処理する
			{

				//配列ブロック開始情報読み込み
				std::size_t array_elem_num = 0;//配列要素数
				std::size_t array_block_size = 0;//配列ブロックサイズ
				arc.m_format.readArrayHeader(arc, item_obj, array_elem_num, array_block_size);

				//配列ブロック開始
				{
					//配列ブロック処理用の孫アーカイブオブジェクトを生成
					inputArchive& parent_arc = arc;
					inputArchive arc(parent_arc, DESERIALIZE_PHASE_ARRAY);

					//データ読み込み先の先頭ポインタ（配列の先頭）を記憶
					//※配列ループ処理中に item_obj.m_itemP を書き換えるので、元に戻せるようにしておく
					const void* item_p_top = item_obj.m_itemP;

					//配列ループ
					//※ループ終了条件判定に index < elem_num は含めない
					//　（アーカイブ形式によっては事前に正確な配列要素数が得られない可能性があるため）
					for (std::size_t index = 0; !arc.hasFatalError(); ++index)//【セーブデータ上の】配列要素数分データ読み込み
					{
						if (!item_obj.isArr())
						{
							//非配列要素の終了判定
							if (index == 1)
								break;
						}
						else
						{
							//配列ブロック終了判定
							bool is_array_block_end = true;
							arc.m_format.tryAndReadArrayFooter(arc, item_obj, is_array_block_end);
							if (is_array_block_end)
								break;//配列ブロックの終了を検出したらループから抜ける
						}

						//デバッグ用の変数キャスト
						//※デバッガで変数の内容を確認したい時に使用する
						//const T* debug_p = reinterpret_cast<const T*>(item_obj.m_itemP);

						//有効な配列要素か？（有効でなければ処理せず読み込むだけ）
						const bool is_valid_element = (!item_obj.nowIsNul() && index < item_obj.getNowElemNum());
						if (is_valid_element)
							++elem_num_loaded;

						//要素開始情報読み込み
						std::size_t elem_size = 0;
						short items_num = 0;
						arc.m_format.readElemHeader(arc, item_obj, index, items_num, elem_size);

						//要素開始
						{
							//新しいアーカイブオブジェクトを生成
							inputArchive& parent_arc = arc;
							inputArchive arc(parent_arc, DESERIALIZE_PHASE_ELEMENT);

							//ロード前処理フェーズに変更
							arc.setStatus(DESERIALIZE_PHASE_BEFORE_LOAD);

							//ロード前処理（デシリアライズ専用処理）呼び出し
							if (is_valid_element)
							{
								GASHA_ serialization::beforeLoad<inputArchive, T> functor;
								functor(arc, item_obj.template get<T>(), ver, now_ver);
							}

							//対象データ項目リスト作成フェーズに変更
							arc.setStatus(DESERIALIZE_PHASE_MAKE_LIST);

							//デシリアライズ処理（シリアライズ＆デシリアライズ兼用処理）呼び出し
							//※データ項目リストを記録するだけ
							if (is_valid_element)
							{
								GASHA_ serialization::serialize<inputArchive, T> functor;
								functor(arc, item_obj.template getConst<T>(), ver, now_ver);
							}

							//ロード処理（デシリアライズ専用処理）呼び出し
							//※データ項目リストを記録するだけ
							if (is_valid_element)
							{
								GASHA_ serialization::load<inputArchive, T> functor;
								functor(arc, item_obj.template get<T>(), ver, now_ver);
							}

							//データのロードフェーズに変更
							arc.setStatus(DESERIALIZE_PHASE_LOAD_DATA);

							//実際のロード処理
							//※ループ終了条件判定に item_idx < items_num は含めない
							//　（アーカイブ形式によっては事前に正確な項目数が得られない可能性があるため）
							for (short item_idx = 0; !arc.hasFatalError(); ++item_idx)
							{
								//要素終了情報読み込み
								bool is_elem_end = true;
								arc.m_format.tryAndReadElemFooter(arc, item_obj, index, is_elem_end);
								if (is_elem_end)
									break;//要素の終了を検出したらループから抜ける

								bool is_init = true;//初回処理
								bool is_required_retry = false;//リトライ処理要求
								bool is_already_retry = false;//リトライ処理済み
								GASHA_ serialization::itemInfoBase delegate_child_item_for_retry;//リトライ用の委譲項目（実体）
								GASHA_ serialization::itemInfoBase* delegate_child_item_now = nullptr;//リトライ用の委譲項目
								while (is_init || (is_required_retry && !is_already_retry))//リトライは一回だけ有効
								{
									is_init = false;//初回処理終了
									if (is_required_retry)//リトライ時
										is_already_retry = true;//リトライ済みにする（リトライは一回だけ）

									//要素の読み込み
									GASHA_ serialization::itemInfoBase child_item;
									arc.m_format.readDataItem(arc, item_obj, delegate_child_item_now, child_item, is_valid_element, is_required_retry);

									//ロードしたが保存先の指定がなかった項目の処理
									//※既にリトライしている場合は実行しない
									if (child_item.isOnlyOnSaveData() && !is_already_retry)
									{
										//通知フェーズに変更
										arc.setStatus(DESERIALIZE_PHASE_NOTICE_UNRECOGNIZED);

										//通知
										if (is_valid_element)
										{
											//委譲データ項目の受け取り用にインスタンスをセット
											arc.setTargetObjItemDelegate(&delegate_child_item_for_retry);

											//通知処理呼び出し
											GASHA_ serialization::noticeUnrecognizedItem<inputArchive, T> functor;
											functor(arc, item_obj.template get<T>(), ver, now_ver, child_item);

											//委譲データ項目の受け取り終了
											arc.resetTargetObjItemDelegate();

											//委譲データ項目の受け取り状態を判定
											if (delegate_child_item_for_retry.m_nameCrc != 0 && delegate_child_item_for_retry.m_itemP)
											{
												//委譲データ項目が設定されたのでリトライ
												delegate_child_item_now = &delegate_child_item_for_retry;
												is_required_retry = true;
											}
										}

										//データのロードフェーズに戻す
										arc.setStatus(DESERIALIZE_PHASE_LOAD_DATA);

										//リトライ
										//※オブジェクトの処理に入る前にリトライを実行
										if (is_required_retry)
											continue;
									}

									//オブジェクトの場合の処理
									if (child_item.isObj())
									{
										//オブジェクトのロードフェーズに変更
										arc.setStatus(DESERIALIZE_PHASE_LOAD_OBJECT);

										//オブジェクト処理対象データ項目をセット
										arc.setTargetObjItem(child_item);
										arc.setTargetObjItemDelegate(delegate_child_item_now);

										//通知処理呼び出し
										//※委譲データ項目がセットされている時のみ
										//if (arc.status() != DESERIALIZE_PHASE_LOAD_OBJECT_END)
										if (delegate_child_item_now)
										{
											GASHA_ serialization::noticeUnrecognizedItem<inputArchive, T> functor;
											functor(arc, item_obj.template get<T>(), ver, now_ver, child_item);
										}

										//デシリアライズ処理（シリアライズ＆デシリアライズ兼用処理）呼び出し
										//※対象オブジェクトアイテムを処理する
										if (arc.status() != DESERIALIZE_PHASE_LOAD_OBJECT_END)
										{
											GASHA_ serialization::serialize<inputArchive, T> functor;
											functor(arc, item_obj.template getConst<T>(), ver, now_ver);
										}

										//ロード処理（デシリアライズ専用処理）呼び出し
										//※対象オブジェクトアイテムを処理する
										if (arc.status() != DESERIALIZE_PHASE_LOAD_OBJECT_END)
										{
											GASHA_ serialization::load<inputArchive, T> functor;
											functor(arc, item_obj.template get<T>(), ver, now_ver);
										}

										//オブジェクト処理対象データ項目をリセット
										arc.resetTargetObjItem();
										arc.resetTargetObjItemDelegate();
										delegate_child_item_now = nullptr;

										//未処理のままだったらブロックをスキップする
										if (arc.status() != DESERIALIZE_PHASE_LOAD_OBJECT_END)
										{
											//オブジェクトのブロックをスキップ
											arc.m_format.skipReadBlock(arc);

											//処理済みにする
											child_item.setIsAlready();
										}

										//データのロードフェーズに戻す
										arc.setStatus(DESERIALIZE_PHASE_LOAD_DATA);
									}
								}
								//リトライ用の委譲項目リセット
								delegate_child_item_now = nullptr;
							}

							//処理されなかったデータ項目の処理
							GASHA_ forEach(arc.m_itemInfoTree, [&arc, &item_obj, &ver, &now_ver, &is_valid_element](GASHA_ archive::archiveBase::itemInfoNode& node)
								{
									const GASHA_ serialization::itemInfoBase& child_item = node.m_info;
									//保存先の指定があるが、セーブデータになくロードできなかった項目
									if (child_item.isOnlyOnMem())
									{
										//通知フェーズに変更
										arc.setStatus(DESERIALIZE_PHASE_NOTICE_UNLOADED);

										//処理結果計上
										arc.result().addResult(child_item);

										//通知処理呼び出し
										if (is_valid_element)
										{
											GASHA_ serialization::noticeUnloadedItem<inputArchive, T> functor;
											functor(arc, item_obj.template get<T>(), ver, now_ver, child_item);
										}
									}
								}
							);

							//ロード後処理フェーズに変更
							arc.setStatus(DESERIALIZE_PHASE_AFTEER_LOAD);

							//ロード後処理（デシリアライズ専用処理）呼び出し
							if (is_valid_element)
							{
								GASHA_ serialization::afterLoad<inputArchive, T> functor;
								functor(arc, item_obj.template get<T>(), ver, now_ver);
							}

							//要素終了
							arc.m_format.finishReadElem(parent_arc, arc);
						}

						//データ読み込み先のポインタを配列の次の要素に更新
						if (item_obj.m_itemP)
						{
							*const_cast<void**>(&item_obj.m_itemP) = reinterpret_cast<T*>(const_cast<void*>(item_obj.m_itemP)) + 1;
						}
					}

					//配列の先頭ポインタ（元のポインタ）に戻す
					item_obj.m_itemP = item_p_top;

					//配列ブロック終了
					arc.m_format.finishReadArray(parent_arc, arc);
				}
			}
				
			//分配前フェーズに変更
			arc.setStatus(DESERIALIZE_PHASE_BEFORE_DISTRIBUTE);

			//分配前処理（デシリアライズ専用処理）呼び出し
			{
				GASHA_ serialization::beforeDistribute<inputArchive, T> functor;
				functor(arc, item_obj.template get<T>(), elem_num, elem_num_loaded, ver, now_ver);
			}

			//分配フェーズに変更
			arc.setStatus(DESERIALIZE_PHASE_DISTRIBUTE);

			//分配処理（デシリアライズ専用処理）呼び出し
			while (!arc.hasFatalError())
			{
				//ブロック終了判定
				bool is_block_end = true;
				arc.m_format.tryAndReadBlockFooter(arc, item_obj, is_block_end);
				if (is_block_end)
					break;//ブロックの終了を検出したらループから抜ける

				//オブジェクトブロック開始
				{
					//オブジェクトブロック処理用の子アーカイブオブジェクトを生成
					inputArchive& parent_arc = arc;
					inputArchive arc(parent_arc, DESERIALIZE_PHASE_DISTRIBUTE);

					//ブロック開始情報を仮読みし、分配処理に回す
					GASHA_ serialization::itemInfoBase require_item;
					std::size_t require_block_size = 0;
					bool is_found_next_block = false;
					arc.m_format.requireNextBlockHeader(arc, require_item, require_block_size, is_found_next_block);
					if (!is_found_next_block)
						break;//ブロックが見つからなかった場合もループから抜ける（ありえない？）

					//オブジェクト分配処理フェーズに変更
					arc.setStatus(DESERIALIZE_PHASE_DISTRIBUTE_OBJECT);

					//オブジェクト処理対象データ項目をセット
					arc.setTargetObjItem(require_item);

					//分配処理（デシリアライズ専用処理）呼び出し
					{
						GASHA_ serialization::distributor<inputArchive, T> functor;
						functor(arc, item_obj.template get<T>(), elem_num, elem_num_loaded, ver, now_ver, require_item);
					}

					//オブジェクト処理対象データ項目をリセット
					arc.resetTargetObjItem();

					//未処理のままだったらブロックをスキップする
					if (arc.status() != DESERIALIZE_PHASE_DISTRIBUTE_OBJECT_END)
					{
						//オブジェクトのブロックをスキップ
						arc.m_format.skipReadBlock(arc);

						//処理済みにする
						require_item.setIsAlready();
					}

					//オブジェクトブロック終了
					arc.m_format.finishReadBlock(parent_arc, arc);
				}
			}

			//分配後フェーズに変更
			arc.setStatus(DESERIALIZE_PHASE_AFTER_DISTRIBUTE);

			//分配後処理（デシリアライズ専用処理）呼び出し
			{
				GASHA_ serialization::afterDistribute<inputArchive, T> functor;
				functor(arc, item_obj.template get<T>(), elem_num, elem_num_loaded, ver, now_ver);
			}

			//ブロック終了
			arc.m_format.finishReadBlock(parent_arc, arc);
		}
		
		//ネストレベルが0ならターミネータを読み込み
		if (m_nestLevel == 0)
			arc.m_format.readTerminator(arc);//ターミネータ読み込み

		//処理結果にセーブデータサイズをセットする
		m_result.setSaveDataSize(size());

		//最終的に致命的なエラーがあれば呼び出し
		if (m_nestLevel == 0 && arc.hasFatalError())
		{
			GASHA_ serialization::fatalDeserializeErrorOccurred<inputArchive, T> functor;
			functor(arc, item_obj.template get<T>(), ver, now_ver);
		}
		
		return *this;
	}

	//コンストラクタ
	template<class FORMAT>
	inline inputArchive<FORMAT>::inputArchive(const void* buff, const std::size_t buff_size, void* work_buff, std::size_t work_buff_size) :
		archiveBase(const_cast<void*>(buff), buff_size, work_buff, work_buff_size),
		m_parent(nullptr),
		m_format(),
		m_status(DESERIALIZE_BEGINNING),
		m_targetObjItem(nullptr),
		m_targetObjItemDelegate(nullptr)
	{}
	template<class FORMAT>
	template<typename BUFF_T, std::size_t BUFF_SIZE, typename WORK_T, std::size_t WORK_SIZE>
	inline inputArchive<FORMAT>::inputArchive(const BUFF_T(&buff)[BUFF_SIZE], WORK_T(&work_buff)[WORK_SIZE]) :
		archiveBase(const_cast<BUFF_T*>(&buff[0]), BUFF_SIZE, work_buff, WORK_SIZE),
		m_parent(nullptr),
		m_format(),
		m_status(DESERIALIZE_BEGINNING),
		m_targetObjItem(nullptr),
		m_targetObjItemDelegate(nullptr)
	{}
	template<class FORMAT>
	template<typename WORK_T, std::size_t WORK_SIZE>
	inline inputArchive<FORMAT>::inputArchive(const void* buff, const std::size_t buff_size, WORK_T(&work_buff)[WORK_SIZE]) :
		archiveBase(const_cast<void*>(buff), buff_size, work_buff, WORK_SIZE),
		m_parent(nullptr),
		m_format(),
		m_status(DESERIALIZE_BEGINNING),
		m_targetObjItem(nullptr),
		m_targetObjItemDelegate(nullptr)
	{}
	//親を受け取るコンストラクタ
	template<class FORMAT>
	inline inputArchive<FORMAT>::inputArchive(inputArchive& parent, const status_type status) :
		archiveBase(parent),
		m_parent(&parent),
		m_format(parent.m_format),
		m_status(status),
		m_targetObjItem(nullptr),
		m_targetObjItemDelegate(nullptr)
	{}

	//デストラクタ
	template<class FORMAT>
	inputArchive<FORMAT>::~inputArchive()
	{}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_INPUT_ARCHIVE_INL

// End of file

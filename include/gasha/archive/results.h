#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_RESULTS_H
#define GASHA_INCLUDED_ARCHIVE_RESULTS_H

//--------------------------------------------------------------------------------
// archive/results.h
// アーカイブ/処理結果【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/item_info_base.h>//アーカイブ/データ項目情報基底クラス

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/処理結果
	//--------------------------------------------------------------------------------

	//--------------------
	//処理結果クラス
	class results
	{
	public:
		//アクセッサ
		inline bool hasFatalError() const;//致命的なエラーあり
		inline void setHasFatalError();//致命的なエラーあり
		inline void setHasFatalError(const bool enabled);//致命的なエラーあり
		inline int getNumSmallerSizeItem() const;//サイズが縮小されたデータ項目の数を取得
		inline int getNumLargerSizeItem() const;//サイズが拡大されたデータ項目の数を取得
		inline int getNumSmallerArrItem() const;//配列要素数が縮小されたデータ項目の数を取得
		inline int getNumLargerArrItem() const;//配列要素数が拡大されたデータ項目の数を取得
		inline int getNumIsOnlyOnSaveData() const;//セーブデータ上にのみ存在するデータ項目の数を取得
		inline int getNumIsOnlyOnMem() const;//セーブデータ上にないデータ項目の数を取得
		inline int getNumIsObjOnSaveDataOnly() const;//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数を取得
		inline int getNumIsObjOnMemOnly() const;//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数を取得
		inline int getNumIsArrOnSaveDataOnly() const;//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数を取得
		inline int getNumIsArrOnMemOnly() const;//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数を取得
		inline int getNumIsPtrOnSaveDataOnly() const;//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数を取得
		inline int getNumIsPtrOnMemOnly() const;//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数を取得
		inline int getNumIsNulOnSaveDataOnly() const;//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数を取得
		inline int getNumIsNulOnMemOnly() const;//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数を取得
		inline void addNumSmallerSizeItem();//サイズが縮小されたデータ項目の数をカウントアップ
		inline void addNumLargerSizeItem();//サイズが拡大されたデータ項目の数をカウントアップ
		inline void addNumSmallerArrItem();//配列要素数が縮小されたデータ項目の数をカウントアップ
		inline void addNumLargerArrItem();//配列要素数が拡大されたデータ項目の数をカウントアップ
		inline void addNumIsOnlyOnSaveData();//セーブデータ上にのみ存在するデータ項目の数をカウントアップ
		inline void addNumIsOnlyOnMem();//セーブデータ上にないデータ項目の数をカウントアップ
		inline void addNumIsObjOnSaveDataOnly();//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumIsObjOnMemOnly();//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline void addNumIsArrOnSaveDataOnly();//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumIsArrOnMemOnly();//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline void addNumIsPtrOnSaveDataOnly();//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumIsPtrOnMemOnly();//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline void addNumIsNulOnSaveDataOnly();//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumIsNulOnMemOnly();//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline void addNumSmallerSizeItem(const bool enabled);//サイズが縮小されたデータ項目の数をカウントアップ
		inline void addNumLargerSizeItem(const bool enabled);//サイズが拡大されたデータ項目の数をカウントアップ
		inline void addNumSmallerArrItem(const bool enabled);//配列要素数が縮小されたデータ項目の数をカウントアップ
		inline void addNumLargerArrItem(const bool enabled);//配列要素数が拡大されたデータ項目の数をカウントアップ
		inline void addNumIsOnlyOnSaveData(const bool enabled);//セーブデータ上にのみ存在するデータ項目の数をカウントアップ
		inline void addNumIsOnlyOnMem(const bool enabled);//セーブデータ上にないデータ項目の数をカウントアップ
		inline void addNumIsObjOnSaveDataOnly(const bool enabled);//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumIsObjOnMemOnly(const bool enabled);//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline void addNumIsArrOnSaveDataOnly(const bool enabled);//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumIsArrOnMemOnly(const bool enabled);//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline void addNumIsPtrOnSaveDataOnly(const bool enabled);//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumIsPtrOnMemOnly(const bool enabled);//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline void addNumIsNulOnSaveDataOnly(const bool enabled);//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumIsNulOnMemOnly(const bool enabled);//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline std::size_t getCopiedSize() const;//コピー済みサイズ取得
		inline std::size_t getSaveDataSize() const;//セーブデータサイズ取得
		inline void setSaveDataSize(const std::size_t save_data_size);//セーブデータサイズをセット
	public:
		//メソッド
		//コピー済みサイズ追加
		inline std::size_t addCopiedSize(const std::size_t size);
		//処理結果に加算
		void addResult(const results& src);
		//処理結果を計上
		void addResult(const GASHA_ serialization::itemInfoBase& src);
	public:
		//コンストラクタ
		inline results();
		//デストラクタ
		inline ~results();
	private:
		//フィールド
		bool m_hasFatalError;//致命的なエラーあり
		short m_numSmallerSizeItem;//サイズが縮小されたデータ項目の数
		short m_numLargerSizeItem;//サイズが拡大されたデータ項目の数
		short m_numSmallerArrItem;//配列要素数が縮小されたデータ項目の数
		short m_numLargerArrItem;//配列要素数が拡大されたデータ項目の数
		short m_numIsOnlyOnSaveData;//セーブデータ上にのみ存在するデータ項目の数
		short m_numIsOnlyOnMem;//セーブデータ上にないデータ項目の数
		short m_numIsObjOnSaveDataOnly;//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数
		short m_numIsObjOnMemOnly;//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数
		short m_numIsArrOnSaveDataOnly;//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数
		short m_numIsArrOnMemOnly;//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数
		short m_numIsPtrOnSaveDataOnly;//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数
		short m_numIsPtrOnMemOnly;//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数
		short m_numIsNulOnSaveDataOnly;//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数
		short m_numIsNulOnMemOnly;//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数
		std::size_t m_copiedSize;//コピー済みサイズ
		std::size_t m_saveDataSize;//セーブデータサイズ
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/archive/results.inl>

#endif//GASHA_INCLUDED_ARCHIVE_RESULTS_H

// End of file

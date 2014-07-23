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
		inline void setHasFatalError(const bool occured);//致命的なエラーあり
		inline int numSmallerSizeItem() const;//サイズが縮小されたデータ項目の数を取得
		inline int numLargerSizeItem() const;//サイズが拡大されたデータ項目の数を取得
		inline int numSmallerArrItem() const;//配列要素数が縮小されたデータ項目の数を取得
		inline int numLargerArrItem() const;//配列要素数が拡大されたデータ項目の数を取得
		inline int numOnlyOnSaveData() const;//セーブデータ上にのみ存在するデータ項目の数を取得
		inline int numOnlyOnMem() const;//セーブデータ上にないデータ項目の数を取得
		inline int numObjOnSaveDataOnly() const;//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数を取得
		inline int numObjOnMemOnly() const;//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数を取得
		inline int numArrOnSaveDataOnly() const;//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数を取得
		inline int numArrOnMemOnly() const;//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数を取得
		inline int numPtrOnSaveDataOnly() const;//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数を取得
		inline int numPtrOnMemOnly() const;//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数を取得
		inline int numNulOnSaveDataOnly() const;//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数を取得
		inline int numNulOnMemOnly() const;//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数を取得
		inline void addNumSmallerSizeItem();//サイズが縮小されたデータ項目の数をカウントアップ
		inline void addNumLargerSizeItem();//サイズが拡大されたデータ項目の数をカウントアップ
		inline void addNumSmallerArrItem();//配列要素数が縮小されたデータ項目の数をカウントアップ
		inline void addNumLargerArrItem();//配列要素数が拡大されたデータ項目の数をカウントアップ
		inline void addNumOnlyOnSaveData();//セーブデータ上にのみ存在するデータ項目の数をカウントアップ
		inline void addNumOnlyOnMem();//セーブデータ上にないデータ項目の数をカウントアップ
		inline void addNumObjOnSaveDataOnly();//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumObjOnMemOnly();//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline void addNumArrOnSaveDataOnly();//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumArrOnMemOnly();//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline void addNumPtrOnSaveDataOnly();//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumPtrOnMemOnly();//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline void addNumNulOnSaveDataOnly();//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumNulOnMemOnly();//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline void addNumSmallerSizeItem(const bool enabled);//サイズが縮小されたデータ項目の数をカウントアップ
		inline void addNumLargerSizeItem(const bool enabled);//サイズが拡大されたデータ項目の数をカウントアップ
		inline void addNumSmallerArrItem(const bool enabled);//配列要素数が縮小されたデータ項目の数をカウントアップ
		inline void addNumLargerArrItem(const bool enabled);//配列要素数が拡大されたデータ項目の数をカウントアップ
		inline void addNumOnlyOnSaveData(const bool enabled);//セーブデータ上にのみ存在するデータ項目の数をカウントアップ
		inline void addNumOnlyOnMem(const bool enabled);//セーブデータ上にないデータ項目の数をカウントアップ
		inline void addNumObjOnSaveDataOnly(const bool enabled);//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumObjOnMemOnly(const bool enabled);//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline void addNumArrOnSaveDataOnly(const bool enabled);//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumArrOnMemOnly(const bool enabled);//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline void addNumPtrOnSaveDataOnly(const bool enabled);//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumPtrOnMemOnly(const bool enabled);//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline void addNumNulOnSaveDataOnly(const bool enabled);//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
		inline void addNumNulOnMemOnly(const bool enabled);//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
		inline std::size_t copiedSize() const;//コピー済みサイズ取得
		inline std::size_t saveDataSize() const;//セーブデータサイズ取得
		inline void setSaveDataSize(const std::size_t save_data_size);//セーブデータサイズをセット
		inline std::size_t peakWorkSize() const;//最も多く消費したワークバッファサイズ取得
	public:
		//メソッド
		//コピー済みサイズ追加
		inline std::size_t addCopiedSize(const std::size_t size);
		//最も多く消費したワークバッファサイズを更新
		inline std::size_t updatePeakWorkSize(const std::size_t size);
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
		short m_numOnlyOnSaveData;//セーブデータ上にのみ存在するデータ項目の数
		short m_numOnlyOnMem;//セーブデータ上にないデータ項目の数
		short m_numObjOnSaveDataOnly;//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数
		short m_numObjOnMemOnly;//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数
		short m_numArrOnSaveDataOnly;//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数
		short m_numArrOnMemOnly;//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数
		short m_numPtrOnSaveDataOnly;//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数
		short m_numPtrOnMemOnly;//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数
		short m_numNulOnSaveDataOnly;//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数
		short m_numNulOnMemOnly;//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数
		std::size_t m_copiedSize;//コピー済みサイズ
		std::size_t m_saveDataSize;//セーブデータサイズ
		std::size_t m_peakWorkSize;//最も多く消費したワークバッファサイズ
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/archive/results.inl>

#endif//GASHA_INCLUDED_ARCHIVE_RESULTS_H

// End of file

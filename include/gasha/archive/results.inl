#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_RESULTS_INL
#define GASHA_INCLUDED_ARCHIVE_RESULTS_INL

//--------------------------------------------------------------------------------
// archive/results.inl
// アーカイブ/処理結果【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/results.h>//アーカイブ/処理結果【宣言部】

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

	//致命的なエラーあり
	inline bool results::hasFatalError() const
	{
		return m_hasFatalError;
	}
	//致命的なエラーあり
	inline void results::setHasFatalError()
	{
		m_hasFatalError = true;
	}
	//致命的なエラーあり
	inline void results::setHasFatalError(const bool occured)
	{
		if (occured)
			setHasFatalError();
	}
	//（名前が衝突するなどして）無効となったデータ項目の数を取得
	inline int results::numInvalidItem() const
	{
		return m_numInvalidItem;
	}
	//サイズが縮小されたデータ項目の数を取得
	inline int results::numSmallerSizeItem() const
	{
		return m_numSmallerSizeItem;
	}
	//サイズが拡大されたデータ項目の数を取得
	inline int results::numLargerSizeItem() const
	{
		return m_numLargerSizeItem;
	}
	//配列要素数が縮小されたデータ項目の数を取得
	inline int results::numSmallerArrItem() const
	{
		return m_numSmallerArrItem;
	}
	//配列要素数が拡大されたデータ項目の数を取得
	inline int results::numLargerArrItem() const
	{
		return m_numLargerArrItem;
	}
	//セーブデータ上にのみ存在するデータ項目の数を取得
	inline int results::numOnlyOnSaveData() const
	{
		return m_numOnlyOnSaveData;
	}
	//セーブデータ上にないデータ項目の数を取得
	inline int results::numOnlyOnMem() const
	{
		return m_numOnlyOnMem;
	}
	//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数を取得
	inline int results::numObjOnSaveDataOnly() const
	{
		return m_numObjOnSaveDataOnly;
	}
	//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数を取得
	inline int results::numObjOnMemOnly() const
	{
		return m_numObjOnMemOnly;
	}
	//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数を取得
	inline int results::numArrOnSaveDataOnly() const
	{
		return m_numArrOnSaveDataOnly;
	}
	//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数を取得
	inline int results::numArrOnMemOnly() const
	{
		return m_numArrOnMemOnly;
	}
	//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数を取得
	inline int results::numPtrOnSaveDataOnly() const
	{
		return m_numPtrOnSaveDataOnly;
	}
	//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数を取得
	inline int results::numPtrOnMemOnly() const
	{
		return m_numPtrOnMemOnly;
	}
	//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数を取得
	inline int results::numNulOnSaveDataOnly() const
	{
		return m_numNulOnSaveDataOnly;
	}
	//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数を取得
	inline int results::numNulOnMemOnly() const
	{
		return m_numNulOnMemOnly;
	}
	//（名前が衝突するなどして）無効となったデータ項目の数をカウントアップ
	inline void results::addNumInvalidItem()
	{
		++m_numInvalidItem;
	}
	//サイズが縮小されたデータ項目の数をカウントアップ
	inline void results::addNumSmallerSizeItem()
	{
		++m_numSmallerSizeItem;
	}
	//サイズが拡大されたデータ項目の数をカウントアップ
	inline void results::addNumLargerSizeItem()
	{
		++m_numLargerSizeItem;
	}
	//配列要素数が縮小されたデータ項目の数をカウントアップ
	inline void results::addNumSmallerArrItem()
	{
		++m_numSmallerArrItem;
	}
	//配列要素数が拡大されたデータ項目の数をカウントアップ
	inline void results::addNumLargerArrItem()
	{
		++m_numLargerArrItem;
	}
	//セーブデータ上にのみ存在するデータ項目の数をカウントアップ
	inline void results::addNumOnlyOnSaveData()
	{
		++m_numOnlyOnSaveData;
	}
	//セーブデータ上にないデータ項目の数をカウントアップ
	inline void results::addNumOnlyOnMem()
	{
		++m_numOnlyOnMem;
	}
	//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumObjOnSaveDataOnly()
	{
		++m_numObjOnSaveDataOnly;
	}
	//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumObjOnMemOnly()
	{
		++m_numObjOnMemOnly;
	}
	//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumArrOnSaveDataOnly()
	{
		++m_numArrOnSaveDataOnly;
	}
	//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumArrOnMemOnly()
	{
		++m_numArrOnMemOnly;
	}
	//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumPtrOnSaveDataOnly()
	{
		++m_numPtrOnSaveDataOnly;
	}
	//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumPtrOnMemOnly()
	{
		++m_numPtrOnMemOnly;
	}
	//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumNulOnSaveDataOnly()
	{
		++m_numNulOnSaveDataOnly;
	}
	//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumNulOnMemOnly()
	{
		++m_numNulOnMemOnly;
	}
	//（名前が衝突するなどして）無効となったデータ項目の数を取得
	inline void results::addNumInvalidItem(const bool enabled)
	{
		if (enabled)
			addNumInvalidItem();
	}
	//サイズが縮小されたデータ項目の数をカウントアップ
	inline void results::addNumSmallerSizeItem(const bool enabled)
	{
		if (enabled)
			addNumSmallerSizeItem();
	}
	//サイズが拡大されたデータ項目の数をカウントアップ
	inline void results::addNumLargerSizeItem(const bool enabled)
	{
		if (enabled)
			addNumLargerSizeItem();
	}
	//配列要素数が縮小されたデータ項目の数をカウントアップ
	inline void results::addNumSmallerArrItem(const bool enabled)
	{
		if (enabled)
			addNumSmallerArrItem();
	}
	//配列要素数が拡大されたデータ項目の数をカウントアップ
	inline void results::addNumLargerArrItem(const bool enabled)
	{
		if (enabled)
			addNumLargerArrItem();
	}
	//セーブデータ上にのみ存在するデータ項目の数をカウントアップ
	inline void results::addNumOnlyOnSaveData(const bool enabled)
	{
		if (enabled)
			addNumOnlyOnSaveData();
	}
	//セーブデータ上にないデータ項目の数をカウントアップ
	inline void results::addNumOnlyOnMem(const bool enabled)
	{
		if (enabled)
			addNumOnlyOnMem();
	}
	//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumObjOnSaveDataOnly(const bool enabled)
	{
		if (enabled)
			addNumObjOnSaveDataOnly();
	}
	//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumObjOnMemOnly(const bool enabled)
	{
		if (enabled)
			addNumObjOnMemOnly();
	}
	//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumArrOnSaveDataOnly(const bool enabled)
	{
		if (enabled)
			addNumArrOnSaveDataOnly();
	}
	//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumArrOnMemOnly(const bool enabled)
	{
		if (enabled)
			addNumArrOnMemOnly();
	}
	//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumPtrOnSaveDataOnly(const bool enabled)
	{
		if (enabled)
			addNumPtrOnSaveDataOnly();
	}
	//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumPtrOnMemOnly(const bool enabled)
	{
		if (enabled)
			addNumPtrOnMemOnly();
	}
	//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumNulOnSaveDataOnly(const bool enabled)
	{
		if (enabled)
			addNumNulOnSaveDataOnly();
	}
	//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumNulOnMemOnly(const bool enabled)
	{
		if (enabled)
			addNumNulOnMemOnly();
	}
	//コピー済みサイズ取得
	inline std::size_t results::copiedSize() const
	{
		return m_copiedSize;
	}
	//セーブデータサイズ取得
	inline std::size_t results::saveDataSize() const
	{
		return m_hasFatalError ? 0 : m_saveDataSize;
	}
	//セーブデータサイズをセット
	inline void results::setSaveDataSize(const std::size_t save_data_size)
	{
		m_saveDataSize = save_data_size;
	}
	//最も多く消費したワークバッファサイズ取得
	inline std::size_t results::peakWorkSize() const
	{
		return m_peakWorkSize;
	}

	//コピー済みサイズ追加
	inline std::size_t results::addCopiedSize(const std::size_t size)
	{
		m_copiedSize += size;
		return m_copiedSize;
	}
	
	//最も多く消費したワークバッファサイズを更新
	inline std::size_t results::updatePeakWorkSize(const std::size_t size)
	{
		if (m_peakWorkSize < size)
			m_peakWorkSize = size;
		return m_peakWorkSize;
	}

	//コンストラクタ
	inline results::results() :
		m_hasFatalError(false),
		m_numInvalidItem(0),
		m_numSmallerSizeItem(0),
		m_numLargerSizeItem(0),
		m_numSmallerArrItem(0),
		m_numLargerArrItem(0),
		m_numOnlyOnSaveData(0),
		m_numOnlyOnMem(0),
		m_numObjOnSaveDataOnly(0),
		m_numObjOnMemOnly(0),
		m_numArrOnSaveDataOnly(0),
		m_numArrOnMemOnly(0),
		m_numPtrOnSaveDataOnly(0),
		m_numPtrOnMemOnly(0),
		m_numNulOnSaveDataOnly(0),
		m_numNulOnMemOnly(0),
		m_copiedSize(0),
		m_saveDataSize(0),
		m_peakWorkSize(0)
	{}
	
	//デストラクタ
	inline results::~results()
	{}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_RESULTS_INL

// End of file

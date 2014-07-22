#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_RESULTS_INL
#define GASHA_INCLUDED_ARCHIVE_RESULTS_INL

//--------------------------------------------------------------------------------
// archive/results.inl
// アーカイブ/処理結果【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
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
	inline void results::setHasFatalError(const bool enabled)
	{
		if (enabled) setHasFatalError();
	}
	//サイズが縮小されたデータ項目の数を取得
	inline int results::getNumSmallerSizeItem() const
	{
		return m_numSmallerSizeItem;
	}
	//サイズが拡大されたデータ項目の数を取得
	inline int results::getNumLargerSizeItem() const
	{
		return m_numLargerSizeItem;
	}
	//配列要素数が縮小されたデータ項目の数を取得
	inline int results::getNumSmallerArrItem() const
	{
		return m_numSmallerArrItem;
	}
	//配列要素数が拡大されたデータ項目の数を取得
	inline int results::getNumLargerArrItem() const
	{
		return m_numLargerArrItem;
	}
	//セーブデータ上にのみ存在するデータ項目の数を取得
	inline int results::getNumIsOnlyOnSaveData() const
	{
		return m_numIsOnlyOnSaveData;
	}
	//セーブデータ上にないデータ項目の数を取得
	inline int results::getNumIsOnlyOnMem() const
	{
		return m_numIsOnlyOnMem;
	}
	//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数を取得
	inline int results::getNumIsObjOnSaveDataOnly() const
	{
		return m_numIsObjOnSaveDataOnly;
	}
	//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数を取得
	inline int results::getNumIsObjOnMemOnly() const
	{
		return m_numIsObjOnMemOnly;
	}
	//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数を取得
	inline int results::getNumIsArrOnSaveDataOnly() const
	{
		return m_numIsArrOnSaveDataOnly;
	}
	//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数を取得
	inline int results::getNumIsArrOnMemOnly() const
	{
		return m_numIsArrOnMemOnly;
	}
	//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数を取得
	inline int results::getNumIsPtrOnSaveDataOnly() const
	{
		return m_numIsPtrOnSaveDataOnly;
	}
	//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数を取得
	inline int results::getNumIsPtrOnMemOnly() const
	{
		return m_numIsPtrOnMemOnly;
	}
	//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数を取得
	inline int results::getNumIsNulOnSaveDataOnly() const
	{
		return m_numIsNulOnSaveDataOnly;
	}
	//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数を取得
	inline int results::getNumIsNulOnMemOnly() const
	{
		return m_numIsNulOnMemOnly;
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
	inline void results::addNumIsOnlyOnSaveData()
	{
		++m_numIsOnlyOnSaveData;
	}
	//セーブデータ上にないデータ項目の数をカウントアップ
	inline void results::addNumIsOnlyOnMem()
	{
		++m_numIsOnlyOnMem;
	}
	//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumIsObjOnSaveDataOnly()
	{
		++m_numIsObjOnSaveDataOnly;
	}
	//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumIsObjOnMemOnly()
	{
		++m_numIsObjOnMemOnly;
	}
	//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumIsArrOnSaveDataOnly()
	{
		++m_numIsArrOnSaveDataOnly;
	}
	//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumIsArrOnMemOnly()
	{
		++m_numIsArrOnMemOnly;
	}
	//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumIsPtrOnSaveDataOnly()
	{
		++m_numIsPtrOnSaveDataOnly;
	}
	//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumIsPtrOnMemOnly()
	{
		++m_numIsPtrOnMemOnly;
	}
	//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumIsNulOnSaveDataOnly()
	{
		++m_numIsNulOnSaveDataOnly;
	}
	//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumIsNulOnMemOnly()
	{
		++m_numIsNulOnMemOnly;
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
	inline void results::addNumIsOnlyOnSaveData(const bool enabled)
	{
		if (enabled)
			addNumIsOnlyOnSaveData();
	}
	//セーブデータ上にないデータ項目の数をカウントアップ
	inline void results::addNumIsOnlyOnMem(const bool enabled)
	{
		if (enabled)
			addNumIsOnlyOnMem();
	}
	//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumIsObjOnSaveDataOnly(const bool enabled)
	{
		if (enabled)
			addNumIsObjOnSaveDataOnly();
	}
	//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumIsObjOnMemOnly(const bool enabled)
	{
		if (enabled)
			addNumIsObjOnMemOnly();
	}
	//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumIsArrOnSaveDataOnly(const bool enabled)
	{
		if (enabled)
			addNumIsArrOnSaveDataOnly();
	}
	//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumIsArrOnMemOnly(const bool enabled)
	{
		if (enabled)
			addNumIsArrOnMemOnly();
	}
	//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumIsPtrOnSaveDataOnly(const bool enabled)
	{
		if (enabled)
			addNumIsPtrOnSaveDataOnly();
	}
	//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumIsPtrOnMemOnly(const bool enabled)
	{
		if (enabled)
			addNumIsPtrOnMemOnly();
	}
	//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数をカウントアップ
	inline void results::addNumIsNulOnSaveDataOnly(const bool enabled)
	{
		if (enabled)
			addNumIsNulOnSaveDataOnly();
	}
	//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数をカウントアップ
	inline void results::addNumIsNulOnMemOnly(const bool enabled)
	{
		if (enabled)
			addNumIsNulOnMemOnly();
	}
	//コピー済みサイズ取得
	inline std::size_t results::getCopiedSize() const
	{
		return m_copiedSize;
	}
	//セーブデータサイズ取得
	inline std::size_t results::getSaveDataSize() const
	{
		return m_hasFatalError ? 0 : m_saveDataSize;
	}
	//セーブデータサイズをセット
	inline void results::setSaveDataSize(const std::size_t save_data_size)
	{
		m_saveDataSize = save_data_size;
	}

	//コピー済みサイズ追加
	inline std::size_t results::addCopiedSize(const std::size_t size)
	{
		m_copiedSize += size;
		return m_copiedSize;
	}

	//コンストラクタ
	inline results::results() :
		m_hasFatalError(false),
		m_numSmallerSizeItem(0),
		m_numLargerSizeItem(0),
		m_numSmallerArrItem(0),
		m_numLargerArrItem(0),
		m_numIsOnlyOnSaveData(0),
		m_numIsOnlyOnMem(0),
		m_numIsObjOnSaveDataOnly(0),
		m_numIsObjOnMemOnly(0),
		m_numIsArrOnSaveDataOnly(0),
		m_numIsArrOnMemOnly(0),
		m_numIsPtrOnSaveDataOnly(0),
		m_numIsPtrOnMemOnly(0),
		m_numIsNulOnSaveDataOnly(0),
		m_numIsNulOnMemOnly(0),
		m_copiedSize(0),
		m_saveDataSize(0)
	{}
	
	//デストラクタ
	inline results::~results()
	{}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_RESULTS_INL

// End of file

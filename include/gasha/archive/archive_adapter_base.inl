#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_ARCHIVE_ADAPTER_BASE_INL
#define GASHA_INCLUDED_ARCHIVE_ARCHIVE_ADAPTER_BASE_INL

//--------------------------------------------------------------------------------
// archive/archive_adapter_base.inl
// アーカイブ/アーカイブアダプタ基底クラス【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/archive_adapter_base.h>//アーカイブ/アーカイブアダプタ基底クラス【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/アーカイブアダプタ基底クラス
	//--------------------------------------------------------------------------------

	//--------------------
	//アーカイブアダプタ基底クラス

	//処理結果を取得
	inline GASHA_ archive::results& archiveAdapterBase::result()
	{
		return m_arc.result();
	}
	//処理結果を取得
	inline const GASHA_ archive::results& archiveAdapterBase::result() const
	{
		return m_arc.result();
	}
	//致命的なエラーありか？
	inline bool archiveAdapterBase::hasFatalError() const
	{
		return m_arc.hasFatalError();
	}
	//セーブデータバッファの先頭ポインタを取得
	inline const GASHA_ archive::byte* archiveAdapterBase::topPtr() const
	{
		return m_arc.topPtr();
	}
	//セーブデータバッファのサイズを取得
	inline const std::size_t archiveAdapterBase::maxSize() const
	{
		return m_arc.maxSize();
	}
	//セーブデータバッファの使用量を取得
	inline const std::size_t archiveAdapterBase::size() const
	{
		return m_arc.size();
	}
	//セーブデータバッファの残量を取得
	inline const std::size_t archiveAdapterBase::remain() const
	{
		return m_arc.remain();
	}
	//セーブデータバッファの現在位置のポインタを取得
	inline GASHA_ archive::byte* archiveAdapterBase::nowPtr()
	{
		return m_arc.nowPtr();
	}
	//バッファの現在位置が末端に到達したか？
	inline bool archiveAdapterBase::isFull() const
	{
		return m_arc.isFull();
	}
	//データ項目数
	inline std::size_t archiveAdapterBase::itemsNum() const
	{
		return m_arc.itemsNum();
	}
	
	//処理結果を合成
	inline void archiveAdapterBase::addResult(const GASHA_ archive::results& src)
	{
		m_arc.addResult(src);
	}
	
	//リストからデータ項目を検索
	inline const GASHA_ serialization::itemInfoBase* archiveAdapterBase::findItem(const GASHA_ crc32_t name_crc) const
	{
		return m_arc.findItem(name_crc);
	}
	
	//コンストラクタ
	inline archiveAdapterBase::archiveAdapterBase(GASHA_ archive::archiveBase& arc) :
		m_arc(arc)
	{}
	
	//デストラクタ
	inline archiveAdapterBase::~archiveAdapterBase()
	{}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_ARCHIVE_ADAPTER_BASE_INL

// End of file

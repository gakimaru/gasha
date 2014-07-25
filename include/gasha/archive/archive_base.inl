#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_ARCHIVE_BASE_INL
#define GASHA_INCLUDED_ARCHIVE_ARCHIVE_BASE_INL

//--------------------------------------------------------------------------------
// archive/archive_base.inl
// アーカイブ/アーカイブ基底クラス【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/archive_base.h>//アーカイブ/アーカイブ基底クラス【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/アーカイブ基底クラス
	//--------------------------------------------------------------------------------

	//--------------------
	//アーカイブ基底クラス

	//アイテム情報ノード：コンストラクタ
	inline archiveBase::itemInfoNode::itemInfoNode(const GASHA_ serialization::itemInfoBase& info) :
		m_info(info),
		m_childS(nullptr),
		m_childL(nullptr),
		m_isBlack(false)
	{}
	
	//アイテム情報ノード：デストラクタ
	inline archiveBase::itemInfoNode::~itemInfoNode()
	{}

	//処理結果を取得
	inline GASHA_ archive::results& archiveBase::result()
	{
		return m_result;
	}
	//処理結果を取得
	inline const GASHA_ archive::results& archiveBase::result() const
	{
		return m_result;
	}
	//致命的なエラーありか？
	inline bool archiveBase::hasFatalError() const
	{
		return m_result.hasFatalError();
	}
	//セーブデータサイズ取得
	inline std::size_t archiveBase::saveDataSize() const
	{
		return m_result.saveDataSize();
	}
	//セーブデータバッファの先頭ポインタを取得
	inline const void* archiveBase::saveData() const
	{
		return m_buff;
	}
	//最も多く消費したワークバッファサイズ取得
	inline std::size_t archiveBase::peakWorkSize() const
	{
		return m_result.peakWorkSize();
	}

	//セーブデータバッファの先頭ポインタを取得
	inline const byte* archiveBase::topPtr() const
	{
		return m_buff;
	}
	//セーブデータバッファのサイズを取得
	inline const std::size_t archiveBase::maxSize() const
	{
		return m_buffSize;
	}
	//セーブデータバッファの使用量を取得
	inline const std::size_t archiveBase::size() const
	{
		return m_buffPos;
	}
	//セーブデータバッファの残量を取得
	inline const std::size_t archiveBase::remain() const
	{
		return m_buffSize - m_buffPos;
	}
	//セーブデータバッファの現在位置のポインタを取得
	inline byte* archiveBase::nowPtr()
	{
		return m_buff + m_buffPos;
	}
	//バッファの現在位置が末端に到達したか？
	inline bool archiveBase::isFull() const
	{
		return m_buffPos >= m_buffSize;
	}
	//データ項目リスト
	inline std::size_t archiveBase::itemsNum() const
	{
		return m_itemInfoTree.size();
	}

	//処理結果を合成
	inline void archiveBase::addResult(const GASHA_ archive::results& src)
	{
		m_result.addResult(src);
	}
	
	//リストからデータ項目を検索
	inline const GASHA_ serialization::itemInfoBase* archiveBase::findItem(const crc32_t name_crc) const
	{
		const itemInfoNode* node = m_itemInfoTree.at(name_crc);
		if (!node)
			return nullptr;
		return &node->m_info;
	}
	
	//コンストラクタ
	inline archiveBase::archiveBase(void* buff, const std::size_t buff_size, void* work_buff, std::size_t work_buff_size) :
		m_nestLevel(0),
		m_buff(reinterpret_cast<byte*>(buff)),
		m_buffSize(buff_size),
		m_buffPos(0),
		m_workBuff(work_buff, work_buff_size),
		m_itemInfoTree()
	{}
	
	//親を受け取るコンストラクタ
	inline archiveBase::archiveBase(archiveBase& parent) :
		m_nestLevel(parent.m_nestLevel + 1),
		m_buff(parent.nowPtr()),
		m_buffSize(parent.remain()),
		m_buffPos(0),
		m_workBuff(reinterpret_cast<char*>(const_cast<void*>(parent.m_workBuff.buff())) + parent.m_workBuff.size(), parent.m_workBuff.remain()),
		m_itemInfoTree()
	{}
	
	//デストラクタ
	inline archiveBase::~archiveBase()
	{}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ARCHIVE_ARCHIVE_BASE_INL

// End of file

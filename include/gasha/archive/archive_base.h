#pragma once
#ifndef GASHA_INCLUDED_ARCHIVE_ARCHIVE_BASE_H
#define GASHA_INCLUDED_ARCHIVE_ARCHIVE_BASE_H

//--------------------------------------------------------------------------------
// archive/archive_base.h
// アーカイブ/アーカイブ基底クラス【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/results.h>//アーカイブ/処理結果

#include <gasha/stack_allocator.h>//スタックアロケータ
#include <gasha/rb_tree.h>//赤黒木
#include <gasha/crc32.h>//CRC32計算

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/アーカイブ基底クラス
	//--------------------------------------------------------------------------------
	
	//--------------------
	//型
	typedef unsigned char byte;//バイト型（バッファ用）
	
	//--------------------
	//アーカイブ基底クラス
	class archiveBase
	{
		friend class archiveAdapterBase;
		friend class outputArchiveAdapter;
		friend class inputArchiveAdapter;
	public:
		//型
		//アイテム情報ノード
		struct itemInfoNode
		{
			const GASHA_ serialization::itemInfoBase m_info;//データ項目情報
			mutable const itemInfoNode* m_childS;//大（右）側の子
			mutable const itemInfoNode* m_childL;//小（左）側の子
			mutable bool m_isBlack;//色
			inline itemInfoNode(const GASHA_ serialization::itemInfoBase& info);
			inline ~itemInfoNode();
		};
		//データ項目情報操作型
		struct itemInfoOpe : public GASHA_ rb_tree::baseOpe<itemInfoOpe, itemInfoNode, GASHA_ crc32_t>
		{
			//子ノードを取得
			inline static const node_type* getChildL(const node_type& node){ return node.m_childS; }//大（右）側
			inline static const node_type* getChildS(const node_type& node){ return node.m_childL; }//小（左）側
			//子ノードを変更
			inline static void setChildL(node_type& node, const node_type* child){ node.m_childS = child; }//大（右）側
			inline static void setChildS(node_type& node, const node_type* child){ node.m_childL = child; }//小（左）側
	
			//ノードの色を取得
			inline static color_t getColor(const node_type& node){ return node.m_isBlack ? rb_tree::BLACK : rb_tree::RED; }
			//ノードの色を変更
			inline static void setColor(node_type& node, const color_t color){ node.m_isBlack = color == rb_tree::BLACK; }
	
			//キーを取得
			inline static key_type getKey(const node_type& node){ return node.m_info.m_nameCrc; }
		};
		typedef GASHA_ rb_tree::container<itemInfoOpe> itemInfoTree_type;//データ項目情報木型
	public:
		//アクセッサ
		inline GASHA_ archive::results& result();//処理結果を取得
		inline const GASHA_ archive::results& result() const;//処理結果を取得
		inline bool hasFatalError() const;//致命的なエラーありか？
		inline std::size_t saveDataSize() const;//セーブデータサイズ取得
		inline const void* saveData() const;//セーブデータバッファの先頭ポインタを取得
		inline std::size_t peakWorkSize() const;//最も多く消費したワークバッファサイズ取得
	protected:
		inline const byte* topPtr() const;//セーブデータバッファの先頭ポインタを取得
		inline const std::size_t maxSize() const;//セーブデータバッファのサイズを取得
		inline const std::size_t size() const;//セーブデータバッファの使用量を取得
		inline const std::size_t remain() const;//セーブデータバッファの残量を取得
		inline byte* nowPtr();//セーブデータバッファの現在位置のポインタを取得
		inline bool isFull() const;//バッファの現在位置が末端に到達したか？
		inline std::size_t itemsNum() const;//データ項目リスト
	protected:
		//メソッド
		//処理結果を合成
		inline void addResult(const GASHA_ archive::results& src);
		//リストにデータ項目追加
		bool addItem(const GASHA_ serialization::itemInfoBase& item);
		//リストからデータ項目を検索
		inline const GASHA_ serialization::itemInfoBase* findItem(const GASHA_ crc32_t name_crc) const;
	protected:
		//コンストラクタ
		inline archiveBase(void* buff, const std::size_t buff_size, void* work_buff, std::size_t work_buff_size);
		//親を受け取るコンストラクタ
		//※処理階層が深くなるごとにコピーが行われる
		inline archiveBase(archiveBase& parent);
		//デストラクタ
		inline ~archiveBase();
	protected:
		//フィールド
		GASHA_ archive::results m_result;//処理結果
		int m_nestLevel;//データのネストレベル
		byte* m_buff;//セーブデータバッファ
		const std::size_t m_buffSize;//セーブデータバッファのサイズ
		std::size_t m_buffPos;//セーブデータバッファの処理位置
		GASHA_ stackAllocator<> m_workBuff;//ワークバッファ用スタックアロケータ
		itemInfoTree_type m_itemInfoTree;//データ項目情報木
	};

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/archive/archive_base.inl>

#endif//GASHA_INCLUDED_ARCHIVE_ARCHIVE_BASE_H

// End of file

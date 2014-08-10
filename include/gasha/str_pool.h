#pragma once
#ifndef GASHA_INCLUDED_STR_POOL_H
#define GASHA_INCLUDED_STR_POOL_H

//--------------------------------------------------------------------------------
// str_pool.h
// 文字列プール【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/hash_table.h>//開番地法ハッシュテーブル
#include <gasha/crc32.h>//CRC32計算
#include <gasha/stack_allocator.h>//スタックアロケータ
#include <gasha/lf_stack_allocator.h>//ロックフリースタックアロケータ
#include <gasha/dummy_shared_lock.h>//ダミー共有ロック

#include <type_traits>//C++11 std::is_same, std::conditional

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//文字列プール
//--------------------------------------------------------------------------------

//----------------------------------------
//文字列プールクラス
template<std::size_t _STR_POOL_BUFF_SIZE, std::size_t _STR_POOL_TABLE_SIZE, class LOCK_POLICY = GASHA_ dummySharedLock>
class strPool
{
public:
	//定数
	static const std::size_t STR_POOL_BUFF_SIZE = _STR_POOL_BUFF_SIZE;//文字列プールバッファサイズ（最大バイト数） 
	static const std::size_t STR_POOL_TABLE_SIZE = _STR_POOL_TABLE_SIZE;//文字列プールテーブルサイズ（最大個数） 
public:
	//----------------------------------------
	//文字列プール情報
	struct pool
	{
		//フィールド
		const char* m_str;//文字列
		//コンストラクタ
		inline pool(const char* str);
	};
public:
	//----------------------------------------
	//文字列プール操作型
	struct strPoolOpe : public GASHA_ hash_table::baseOpe<strPoolOpe, STR_POOL_TABLE_SIZE, pool, GASHA_ crc32_t>
	{
		typedef LOCK_POLICY lock_type;//ロック型
		static const std::size_t AUTO_REHASH_RATIO = 0;//自動リハッシュなし ※削除しないのでリハッシュ不要
	};
public:
	typedef LOCK_POLICY lock_type;//ロック型
	typedef typename std::conditional<std::is_same<lock_type, GASHA_ dummySharedLock>::value, GASHA_ stackAllocator_withBuff<STR_POOL_BUFF_SIZE>, GASHA_ lfStackAllocator_withBuff<STR_POOL_BUFF_SIZE>>::type strPool_type;//文字列プールバッファ型
	typedef GASHA_ hash_table::container<strPoolOpe> strPoolTable_type;//文字列プールテーブル型
	struct unsafe_t{};//高速登録（安全保証なし版）指定用の型

public:
	//アクセッサ
	inline std::size_t maxSize() const;//最大登録数
	inline std::size_t size() const;//登録数
	inline std::size_t remain() const;//登録数残量
	inline std::size_t buffMaxSize() const;//バッファ最大サイズ
	inline std::size_t buffSize() const;//バッファ使用量
	inline std::size_t buffRemain() const;//バッファ残量

public:
	//関数オペレータ
	//※文字列参照
	inline const char* operator()(const GASHA_ crc32_t key);

public:
	//文字列プール登録
	//※既に登録済みの場合、登録済みの文字列と照合して一致したらその情報を返す。
	//　不一致だった場合、アサーション違反とした上で、nullptr を返す。
	//※戻り値はプールされた文字列のポインタである点に注意（str とは一致しない）
	const char* regist(GASHA_ crc32_t key, const char* str);//※文字列のキーと文字列をそれぞれ指定して登録する
	inline const char* regist(const char* str);//※文字列のCRCをキーとして登録する
	//※【高速版（安全保証なし版）】文字列が登録済みかどうかはキーの判定のみで判断する場合
	//　（実際の文字列の称号を行わない）
	const char* regist(const unsafe_t&, GASHA_ crc32_t key, const char* str);//※文字列のキーと文字列をそれぞれ指定して登録する
	inline const char* regist(const unsafe_t&, const char* str);//※文字列のCRCをキーとして登録する
private:
	//【共通処理】
	const char* _registCommon(GASHA_ crc32_t key, const char* str);

public:
	//【使用注意】クリア
	//※強制的に記録を全てをクリアするので注意。
	//※スレッドセーフではないので注意。
	inline void clear();

public:
	//デフォルトコンストラクタ
	strPool();
	//デストラクタ
	inline ~strPool();

private:
	//フィールド
	strPool_type m_strPoolBuff;//文字列プールバッファ
	strPoolTable_type m_strPoolTable;//文字列プールテーブル

public:
	//静的フィールド
	static unsafe_t unsafe;//高速登録（安全保証なし版）指定用
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/str_pool.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_STR_POOL_ALLWAYS_TOGETHER_CPP_H
#include <gasha/str_pool.cpp.h>
#endif//GASHA_STR_POOL_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_STR_POOL_H

// End of file

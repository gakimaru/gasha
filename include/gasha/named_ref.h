#pragma once
#ifndef GASHA_INCLUDED_NAMED_REF_H
#define GASHA_INCLUDED_NAMED_REF_H

//--------------------------------------------------------------------------------
// named_ref.h
// 名前付きデータ参照【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/crc32.h>//CRC32操作
#include <gasha/hash_table.h>//ハッシュテーブル
#include <gasha/dummy_shared_lock.h>//ダミー共有ロック
#include <gasha/unique_shared_lock.h>//単一共有ロック

#include <cstddef>//std::size_t
#include <typeinfo>//std::type_info

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <functional>//C++11 std::function
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//名前付きデータ参照
//--------------------------------------------------------------------------------

//----------------------------------------
//名前付きデータ参照クラス
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY = GASHA_ dummySharedLock>
class namedRef
{
public:
	//定数
	static const std::size_t TABLE_SIZE = _TABLE_SIZE;
public:
	//型
	typedef IDENTIFIER_TYPE identifier_type;//インスタンス識別型
	typedef LOCK_POLICY lock_type;//ロック型
	//--------------------
	//参照情報
	struct refInfo
	{
		GASHA_ crc32_t m_nameCrc;//名前のCRC
		const std::type_info* m_typeInfo;//型情報
		const void* m_loadRef;//読み込み用参照先
		void* m_storeRef;//書き込み用参照先
		mutable lock_type m_lock;//ロック型
		
		//コンストラクタ
		template<typename T>
		inline refInfo(const GASHA_ crc32_t name_crc, const T& ref);
		template<typename T>
		inline refInfo(const GASHA_ crc32_t name_crc, T& ref);
		//デフォルトコンストラクタ
		inline refInfo();
		//デストラクタ
		inline ~refInfo();
	};
	//--------------------
	//参照情報プロキシー
	//※スコープロックを伴う
	template<typename T>
	class refProxy
	{
	public:
		//型
		typedef typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lock_type lock_type;
	public:
		//オペレータ
		inline const T* operator->() const;
		inline T* operator->();
		inline const T& operator*() const;
		inline T& operator*();
		//ムーブオペレータ
		refProxy& operator=(refProxy&& obj) = delete;
		//コピーオペレータ
		refProxy& operator=(const refProxy& obj) = delete;
		//ムーブコンストラクタ
		inline refProxy(typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T>&& obj);
		inline refProxy(const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T>&& obj);
		//コピーコンストラクタ
		refProxy(const refProxy& obj) = delete;
		//コンストラクタ
		inline refProxy(const refInfo& ref_info, GASHA_ unique_shared_lock<lock_type>&& lock);
		inline refProxy(refInfo& ref_info, GASHA_ unique_shared_lock<lock_type>&& lock);
		//デフォルトコンストラクタ
		//inline refProxy();
		//デストラクタ
		inline ~refProxy();
	private:
		//フィールド
		const refInfo& m_refInfo;//参照情報
		mutable GASHA_ unique_shared_lock<lock_type> m_lock;//単一共有ロック
	};
	//ハッシュテーブル操作型
	struct tableOpe : public GASHA_ hash_table::baseOpe<tableOpe, refInfo, GASHA_ crc32_t>
	{
		typedef typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lock_type lock_type;//ロック型
	};
	//ハッシュテーブル型
	typedef GASHA_ hash_table::container<tableOpe, TABLE_SIZE> table_type;
public:
	//アクセッサ
	inline bool isRegistered(const GASHA_ crc32_t name_crc) const;//登録済みチェック
	inline bool isRegistered(const char* name) const;//登録済みチェック
	template<typename T>
	inline bool isReadOnly(const GASHA_ crc32_t name_crc) const;//読み取り専用チェック ※未登録時は false
	template<typename T>
	inline bool isReadOnly(const char* name) const;//読み取り専用チェック ※未登録時は false
	template<typename T>
	inline bool isReadable(const GASHA_ crc32_t name_crc) const;//読み取り可能チェック ※未登録時は false
	template<typename T>
	inline bool isReadable(const char* name) const;//読み取り可能チェック ※未登録時は false
	template<typename T>
	inline bool isWritable(const GASHA_ crc32_t name_crc) const;//書き込み可能チェック ※未登録時は false
	template<typename T>
	inline bool isWritable(const char* name) const;//書き込み可能チェック ※未登録時は false
public:
	//メソッド
	//データ参照
	//※読み取り専用参照
	//※戻り値のスコープを抜けるまで、共有ロックを取得する
	template<typename T>
	inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T> ref(const char* name) const;
	template<typename T>
	inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T> ref(const GASHA_ crc32_t name_crc) const;
	//※読み書き参照
	//※戻り値のスコープを抜けるまで、排他ロックを取得する
	template<typename T>
	inline typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T> ref(const char* name);
	template<typename T>
	inline typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T> ref(const GASHA_ crc32_t name_crc);
	template<typename T>
	//※読み取り専用参照
	//※戻り値のスコープを抜けるまで、共有ロックを取得する
	inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T> cref(const char* name) const;
	template<typename T>
	inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T> cref(const GASHA_ crc32_t name_crc) const;
	//データ取得
	//※一時的に共有ロックを取得する（関数を抜けた時点でロックは解除されている）
	template<typename T>
	inline T load(const char* name) const;
	template<typename T>
	inline T load(const GASHA_ crc32_t name_crc) const;
	//データ更新
	//※一時的に排他ロックを取得する（関数を抜けた時点でロックは解除されている）
	template<typename T>
	inline void store(const char* name, T&& value);
	template<typename T>
	inline void store(const GASHA_ crc32_t name_crc, T&& value);
	template<typename T>
	inline void store(const char* name, const T& value);
	template<typename T>
	inline void store(const GASHA_ crc32_t name_crc, const T& value);
	//データ参照登録
	//※読み書き可能データ登録
	template<typename T>
	inline void regist(const char* name, T& ref);
	template<typename T>
	inline void regist(const crc32_t name_crc, T& ref);
	//※読み取り専用データ登録
	template<typename T>
	inline void regist(const char* name, const T& ref);
	template<typename T>
	inline void regist(const crc32_t name_crc, const T& ref);
public:
	//デフォルトコンストラクタ
	inline namedRef();
	//デストラクタ
	inline ~namedRef();
private:
	//フィールド
	table_type m_refTable;
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/named_ref.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_NAMED_REF_ALLWAYS_TOGETHER_CPP_H
#include <gasha/named_ref.cpp.h>
#endif//GASHA_NAMED_REF_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_NAMED_REF_H

// End of file

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
#include <gasha/lf_pool_allocator.h>//ロックフリープールアロケータ
#include <gasha/hash_table.h>//ハッシュテーブル

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
class namedRef
{
public:
	//型
	enum reference_type
	{
		REF_VALUE = 0,
		REF_ACCESSOR,
		REF_METHOD
	};
	struct referenceInfo
	{
		const char* m_name;//名前
		GASHA_ crc32_t m_nameCrc;//名前のCRC
		std::type_info* m_typeInfo;//型情報
		reference_type m_type;//参照種別
		const void* m_loadRef;//読み込み用参照先
		void* m_storeeRef;//書き込み用参照先
	};
public:
	//オペレータ
	template<typename T>
	const T& ref(const char* name) const;
	template<typename T>
	T& ref(const char* name);
	template<typename T>
	T load(const char* name) const;
	template<typename T>
	void store(const char* name, T&& value);
	template<typename T>
	void store(const char* name, const T& value);
public:
	//メソッド
	template<typename T>
	void regist(const char* name, const T& ref);
	template<typename T>
	void regist(const char* name, T& ref);
	template<typename T>
	void regist(const char* name, std::function<const T&()> load_func, std::function<T&()> store_func);
	template<typename T>
	void regist(const char* name, std::function<const T&()> load_func, std::function<void(const T&)> store_func);
public:
	//デフォルトコンストラクタ
	inline namedRef();
	//デストラクタ
	inline ~namedRef();
private:
	//フィールド
	referenceInfo m_ref;
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/named_ref.inl>

#endif//GASHA_INCLUDED_NAMED_REF_H

// End of file

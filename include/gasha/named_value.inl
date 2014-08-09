#pragma	 once
#ifndef GASHA_INCLUDED_NAMED_VALUE_INL
#define GASHA_INCLUDED_NAMED_VALUE_INL

//--------------------------------------------------------------------------------
// named_value.inl
// 名前付き定数【インライン定数／テンプレート定数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/named_value.h>//名前付き定数【宣言部】

#include <gasha/limits.h>//限界値
#include <gasha/simple_assert.h>//シンプルアサーション

#include <cstddef>//std::intptr_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//名前付き定数
//--------------------------------------------------------------------------------

namespace named_value
{
	//----------------------------------------
	//名前付き定数クラス

	//--------------------
	//定数情報

	//比較オペレータ
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::valueInfo::operator<(const typename table<OPE_TYPE>::valueInfo& rhs) const
	{
		return true;//m_nameCrc < rhs.m_nameCrc;
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::valueInfo::operator==(const typename table<OPE_TYPE>::valueInfo& rhs) const
	{
		return m_nameCrc == rhs.m_nameCrc;
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::valueInfo::operator!=(const typename table<OPE_TYPE>::valueInfo& rhs) const
	{
		return m_nameCrc != rhs.m_nameCrc;
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::valueInfo::operator==(const GASHA_ crc32_t rhs) const
	{
		return m_nameCrc == rhs;
	}


	//コンストラクタ
	template<class OPE_TYPE>
	template<typename T>
	inline table<OPE_TYPE>::valueInfo::valueInfo(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, const T& value) :
		m_nameCrc(name_crc),
		m_groupNameCrc(group_name_crc),
		m_typeInfo(&typeid(T)),
		m_value(reinterpret_cast<void*>(const_cast<T*>(&value))),
		m_next(nullptr),
		m_prev(nullptr)
	{
		//(sizeof(T) <= sizeof(void*) の時は、value の参照ではなく値を保持する（void* に値を強引に書き込む）
		if (sizeof(T) <= sizeof(void*))
		{
			{
				const std::intptr_t p = reinterpret_cast<std::intptr_t>(&m_value);
				*reinterpret_cast<T*>(p) = value;
			}
		}
	}
	
	//デフォルトコンストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::valueInfo::valueInfo() :
		m_nameCrc(0),
		m_groupNameCrc(0),
		m_typeInfo(nullptr),
		m_value(nullptr),
		m_next(nullptr),
		m_prev(nullptr)
	{}
	
	//デストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::valueInfo::~valueInfo()
	{}
	
	//--------------------
	//定数グループ情報
		
	//定数リストイテレータを取得
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::valueList_type::iterator table<OPE_TYPE>::groupInfo::cbegin() const
	{
		return m_valueList.cbegin();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::valueList_type::iterator table<OPE_TYPE>::groupInfo::cend() const
	{
		return m_valueList.cend();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::valueList_type::iterator table<OPE_TYPE>::groupInfo::begin() const
	{
		return m_valueList.begin();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::valueList_type::iterator table<OPE_TYPE>::groupInfo::end() const
	{
		return m_valueList.end();
	}
	//定数リストリバースイテレータを取得
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::valueList_type::reverse_iterator table<OPE_TYPE>::groupInfo::crbegin() const
	{
		return m_valueList.crbegin();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::valueList_type::reverse_iterator table<OPE_TYPE>::groupInfo::crend() const
	{
		return m_valueList.crend();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::valueList_type::reverse_iterator table<OPE_TYPE>::groupInfo::rbegin() const
	{
		return m_valueList.rbegin();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::valueList_type::reverse_iterator table<OPE_TYPE>::groupInfo::rend() const
	{
		return m_valueList.rend();
	}

	//定数リスト情報を取得
	template<class OPE_TYPE>
	inline std::size_t table<OPE_TYPE>::groupInfo::size() const
	{
		return m_valueList.size();
	}

	//コンストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::groupInfo::groupInfo(const GASHA_ crc32_t group_name_crc):
		m_groupNameCrc(group_name_crc),
		m_lock(),
		m_valueList()
	{}
	
	//デフォルトコンストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::groupInfo::groupInfo() :
		m_groupNameCrc(0),
		m_lock(),
		m_valueList()
	{}
	
	//デストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::groupInfo::~groupInfo()
	{}

	//--------------------
	//名前付き定数本体

	//データ参照情報取得時のアサーション
#define GASHA_NAMED_VALUE_ASSERT(T, value_info, name_crc) \
	GASHA_SIMPLE_ASSERT(value_info != nullptr, "name_crc(0x%08x) is not registered.", name_crc); \
	GASHA_SIMPLE_ASSERT(value_info->m_typeInfo, "name_crc(0x%08x)' has not type.", name_crc); \
	if (value_info->m_typeInfo) \
	{ \
		GASHA_SIMPLE_ASSERT(*value_info->m_typeInfo == typeid(T), "name_crc(0x%08x)'s type and T are different types.(%s and %s)", name_crc, value_info->m_typeInfo->name(), typeid(T).name()); \
	}

	//登録済みチェック
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::isRegistered(const GASHA_ crc32_t name_crc) const
	{
		return m_valueTable->at(name_crc) != nullptr;
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::isRegistered(const char* name) const
	{
		return isRegistered(GASHA_ calcCRC32(name));
	}

	//型チェック
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isType(const GASHA_ crc32_t name_crc) const
	{
		const valueInfo* info = m_valueTable->at(name_crc);
		return info && info->m_typeInfo && *info->m_typeInfo == typeid(T);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isType(const char* name) const
	{
		return isType<T>(GASHA_ calcCRC32(name));
	}

	//定数取得
	template<class OPE_TYPE>
	template<typename T>
	T table<OPE_TYPE>::value(const GASHA_ crc32_t name_crc) const
	{
		const valueInfo* value_info = findValue(name_crc);
		GASHA_NAMED_VALUE_ASSERT(T, value_info, name_crc);
		if (sizeof(T) <= sizeof(void*))
		{
			const std::intptr_t p = reinterpret_cast<std::intptr_t>(&value_info->m_value);
			return *reinterpret_cast<const T*>(p);
		}
		return *reinterpret_cast<const T*>(value_info->m_value);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::value(const char* name) const
	{
		return value<T>(GASHA_ calcCRC32(name));
	}
	//※次の定数を取得
	template<class OPE_TYPE>
	inline GASHA_ crc32_t table<OPE_TYPE>::next(const char* group_name, const char* name, const int step) const
	{
		return next(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), step);
	}
	template<class OPE_TYPE>
	inline GASHA_ crc32_t table<OPE_TYPE>::next(const char* group_name, const char* name, const wraparound_tag&, const int step) const
	{
		return next(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), wraparound, step);
	}
	//※前の定数を取得
	template<class OPE_TYPE>
	inline GASHA_ crc32_t table<OPE_TYPE>::prev(const char* group_name, const char* name, const int step) const
	{
		return prev(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), step);
	}
	template<class OPE_TYPE>
	inline GASHA_ crc32_t table<OPE_TYPE>::prev(const char* group_name, const char* name, const wraparound_tag&, const int step) const
	{
		return prev(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), wraparound, step);
	}

	//定数登録
	template<class OPE_TYPE>
	template<typename T>
	bool table<OPE_TYPE>::regist(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, const T& value)
	{
		groupInfo* group_info = nullptr;
		group_info = registAndGetGroup(group_name_crc);
		if (!group_info)
			return false;
		{
			auto lock = m_groupTable->lockScoped();
			valueInfo* value_info = m_valueTable->emplace(name_crc, group_name_crc, name_crc, value);
			if (group_info && value_info)
				group_info->m_valueList.push_back(*value_info);
			return value_info ? true : false;
		}
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const char* group_name, const char* name, const T& value)
	{
		return regist<T>(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), value);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, const T&& value)
	{
		return regist<T>(group_name_crc, name_crc, value);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const char* group_name, const char* name, const T&& value)
	{
		return regist<T>(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), value);
	}
	
	//定数登録解除
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::unregist(const char* name)
	{
		return unregist(GASHA_ calcCRC32(name));
	}

	//クリア
	template<class OPE_TYPE>
	inline void table<OPE_TYPE>::clear()
	{
		m_groupTable->clear();
		m_valueTable->clear();
	}
	//初期状態にする
	template<class OPE_TYPE>
	inline void table<OPE_TYPE>::initialize()
	{
		std::call_once(m_initialized, initializeOnce);
	}

	//明示的な再初期化
	template<class OPE_TYPE>
	inline void table<OPE_TYPE>::explicitInitialize()
	{
		initializeOnce();
	}

	//ハッシュテーブルアクセス：アクセッサ
	//※定数情報
	template<class OPE_TYPE>
	inline std::size_t table<OPE_TYPE>::max_size() const
	{
		return m_valueTable->max_size();
	}
	template<class OPE_TYPE>
	inline std::size_t table<OPE_TYPE>::size() const
	{
		return m_valueTable->size();
	}
	template<class OPE_TYPE>
	inline std::size_t table<OPE_TYPE>::remain() const
	{
		return m_valueTable->remain();
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::empty() const
	{
		return m_valueTable->empty();
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::full() const
	{
		return m_valueTable->full();
	}
	//※グループ情報
	template<class OPE_TYPE>
	inline std::size_t table<OPE_TYPE>::groupMaxSize() const
	{
		return m_groupTable->max_size();
	}
	template<class OPE_TYPE>
	inline std::size_t table<OPE_TYPE>::groupSize() const
	{
		return m_groupTable->size();
	}
	template<class OPE_TYPE>
	inline std::size_t table<OPE_TYPE>::groupRemain() const
	{
		return m_groupTable->remain();
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::groupIsEmpty() const
	{
		return m_groupTable->empty();
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::groupIsFull() const
	{
		return m_groupTable->full();
	}

	//ハッシュテーブルアクセス：ロック取得系メソッド
	//単一ロック取得
	template<class OPE_TYPE>
	inline GASHA_ unique_shared_lock<typename table<OPE_TYPE>::lock_type> table<OPE_TYPE>::lockUnique(const GASHA_ with_lock_shared_t&) const
	{
		return m_groupTable->lockUnique(GASHA_ with_lock_shared);
	}
	template<class OPE_TYPE>
	inline GASHA_ unique_shared_lock<typename table<OPE_TYPE>::lock_type> table<OPE_TYPE>::lockUnique(const GASHA_ try_to_lock_shared_t&) const
	{
		return m_groupTable->lockUnique(GASHA_ try_to_lock_shared);
	}
	//スコープロック取得
	template<class OPE_TYPE>
	inline GASHA_ shared_lock_guard<typename table<OPE_TYPE>::lock_type> table<OPE_TYPE>::lockSharedScoped() const
	{
		return m_groupTable->lockSharedScoped();
	}
	//ハッシュテーブルアクセス：イテレータ取得系メソッド
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::iterator table<OPE_TYPE>::cbegin() const
	{
		return m_groupTable->cbegin();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::iterator table<OPE_TYPE>::cend() const
	{
		return m_groupTable->cend();
	}

	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::iterator table<OPE_TYPE>::begin() const
	{
		return m_groupTable->begin();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::iterator table<OPE_TYPE>::end() const
	{
		return m_groupTable->end();
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	//リバースイテレータを取得
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::reverse_iterator table<OPE_TYPE>::crbegin() const
	{
		return m_groupTable->crbegin();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::reverse_iterator table<OPE_TYPE>::crend() const
	{
		return m_groupTable->crend();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::reverse_iterator table<OPE_TYPE>::rbegin() const
	{
		return m_groupTable->rbegin();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::reverse_iterator table<OPE_TYPE>::rend() const
	{
		return m_groupTable->rend();
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR

	//ハッシュテーブルアクセス：情報取得系メソッド
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::groupInfo* table<OPE_TYPE>::findGroup(const GASHA_ crc32_t group_name_crc) const
	{
		return m_groupTable->at(group_name_crc);
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::groupInfo* table<OPE_TYPE>::findGroup(const char* group_name) const
	{
		return findGroup(GASHA_ calcCRC32(group_name));
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::valueInfo* table<OPE_TYPE>::findValue(const GASHA_ crc32_t name_crc) const
	{
		auto lock = m_groupTable->lockSharedScoped();
		return m_valueTable->at(name_crc);
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::valueInfo* table<OPE_TYPE>::findValue(const char* name) const
	{
		return findValue(GASHA_ calcCRC32(name));
	}

	//明示的な初期化用コンストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::table(const typename table<OPE_TYPE>::explicitInit_tag&)
	{
		explicitInitialize();
	}

	//デフォルトコンストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::table() :
		m_groupTable(reinterpret_cast<groupTable_type*>(GASHA_ adjustAlign<char, alignof(groupTable_type)>(m_groupTableBuff))),
		m_valueTable(reinterpret_cast<valueTable_type*>(GASHA_ adjustAlign<char, alignof(valueTable_type)>(m_valueTableBuff)))
	{
	//	initialize();//実行しない
	}

	//デストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::~table()
	{}
}//namespace named_value


GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_NAMED_VALUE_INL

// End of file

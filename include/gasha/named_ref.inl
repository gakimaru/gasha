#pragma	 once
#ifndef GASHA_INCLUDED_NAMED_REF_INL
#define GASHA_INCLUDED_NAMED_REF_INL

//--------------------------------------------------------------------------------
// named_ref.inl
// 名前付きデータ参照【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/named_ref.h>//名前付きデータ参照【宣言部】

#include <gasha/simple_assert.h>//シンプルアサーション

#include <type_traits>//std::declval()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//名前付きデータ参照
//--------------------------------------------------------------------------------

//----------------------------------------
//名前付きデータ参照クラス

//--------------------
//参照情報

//コンストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refInfo::refInfo(const GASHA_ crc32_t name_crc, const T& ref) :
	m_nameCrc(name_crc),
	m_typeInfo(&typeid(T)),
	m_loadRef(&ref),
	m_storeRef(nullptr)
{}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refInfo::refInfo(const GASHA_ crc32_t name_crc, T& ref) :
	m_nameCrc(name_crc),
	m_typeInfo(&typeid(T)),
	m_loadRef(&ref),
	m_storeRef(&ref)
{}

//デフォルトコンストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refInfo::refInfo() :
	m_nameCrc(0),
	m_typeInfo(nullptr),
	m_loadRef(nullptr),
	m_storeRef(nullptr)
{}

//デストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refInfo::~refInfo()
{}

//--------------------
//参照情報プロキシー

//オペレータ

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline const T* namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refProxy<T>::operator->() const
{
	return reinterpret_cast<const T*>(m_refInfo.m_loadRef);
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T* namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refProxy<T>::operator->()
{
	return reinterpret_cast<T*>(m_refInfo.m_storeRef);
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline const T& namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refProxy<T>::operator*() const
{
	return *reinterpret_cast<const T*>(m_refInfo.m_loadRef);
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T& namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refProxy<T>::operator*()
{
	return *reinterpret_cast<T*>(m_refInfo.m_storeRef);
}

//ムーブコンストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refProxy<T>::refProxy(typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T>&& obj) :
	m_refInfo(obj.m_refInfo),
	m_lock(std::move(obj.m_lock))
{}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refProxy<T>::refProxy(const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T>&& obj) :
	m_refInfo(obj.m_refInfo),
	m_lock(std::move(obj.m_lock))
{}

//コンストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refProxy<T>::refProxy(const refInfo& ref_info, GASHA_ unique_shared_lock<lock_type>&& lock) :
	m_refInfo(ref_info),
	m_lock(std::move(lock))
{}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refProxy<T>::refProxy(refInfo& ref_info, GASHA_ unique_shared_lock<lock_type>&& lock) :
	m_refInfo(ref_info),
	m_lock(std::move(lock))
{}

//デフォルトコンストラクタ
//template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
//template<typename T>
//inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refProxy<T>::refProxy() :
//	m_refInfo(),
//	m_lock()
//{}

//デストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refProxy<T>::~refProxy()
{}

//--------------------
//名前付きデータ参照本体

//登録済みチェック
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isRegistered(const GASHA_ crc32_t name_crc) const
{
	return m_refTable.at(name_crc) != nullptr;
}

//登録済みチェック
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isRegistered(const char* name) const
{
	return isRegistered(GASHA_ calcCRC32(name));
}

//読み取り専用チェック
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isReadOnly(const GASHA_ crc32_t name_crc) const
{
	const refInfo* info = m_refTable.at(name_crc);
	return info && info->m_loadRef && !info->m_storeRef && info->m_typeInfo && *info->m_typeInfo == typeid(T);
}

//読み取り専用チェック
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isReadOnly(const char* name) const
{
	return isReadOnly<T>(GASHA_ calcCRC32(name));
}

//読み取り可能チェック
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isReadable(const GASHA_ crc32_t name_crc) const
{
	const refInfo* info = m_refTable.at(name_crc);
	return info && info->m_loadRef && info->m_typeInfo && *info->m_typeInfo == typeid(T);
}

//読み取り可能チェック
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isReadable(const char* name) const
{
	return isWritable<T>(GASHA_ calcCRC32(name));
}

//書き込み可能チェック
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isWritable(const GASHA_ crc32_t name_crc) const
{
	const refInfo* info = m_refTable.at(name_crc);
	return info && info->m_storeRef && info->m_typeInfo && *info->m_typeInfo == typeid(T);
}

//書き込み可能チェック
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isWritable(const char* name) const
{
	return isWritable<T>(GASHA_ calcCRC32(name));
}

//データ参照
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T> namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::ref(const char* name) const
{
	return ref<T>(GASHA_ calcCRC32(name));
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T> namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::ref(const GASHA_ crc32_t name_crc) const
{
	const refInfo* info = m_refTable.at(name_crc);
	GASHA_SIMPLE_ASSERT(info != nullptr, "name_crc(0x%08x) is not registered.", name_crc);
	GASHA_SIMPLE_ASSERT(info->m_typeInfo && *info->m_typeInfo == typeid(T), "name_crc(0x%08x)'s type and T are different types.", name_crc);
	GASHA_SIMPLE_ASSERT(info->m_loadRef != nullptr, "name_crc(0x%08x) has not reference for load.", name_crc);
	return refProxy<T>(*info, GASHA_ unique_shared_lock<lock_type>(info->m_lock, GASHA_ with_lock_shared));
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T> namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::ref(const char* name)
{
	return ref<T>(GASHA_ calcCRC32(name));
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T> namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::ref(const GASHA_ crc32_t name_crc)
{
	const refInfo* info = m_refTable.at(name_crc);
	GASHA_SIMPLE_ASSERT(info != nullptr, "name_crc(0x%08x) is not registered.", name_crc);
	GASHA_SIMPLE_ASSERT(info->m_typeInfo && *info->m_typeInfo == typeid(T), "name_crc(0x%08x)'s type and T are different types.", name_crc);
	GASHA_SIMPLE_ASSERT(info->m_storeRef != nullptr, "name_crc(0x%08x) has not reference for store.", name_crc);
	return refProxy<T>(*info, GASHA_ unique_shared_lock<lock_type>(info->m_lock, GASHA_ with_lock));
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T> namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::cref(const char* name) const
{
	return cref<T>(GASHA_ calcCRC32(name));
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T> namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::cref(const GASHA_ crc32_t name_crc) const
{
	const refInfo* info = m_refTable.at(name_crc);
	GASHA_SIMPLE_ASSERT(info != nullptr, "name_crc(0x%08x) is not registered.", name_crc);
	GASHA_SIMPLE_ASSERT(info->m_typeInfo && *info->m_typeInfo == typeid(T), "name_crc(0x%08x)'s type and T are different types.", name_crc);
	GASHA_SIMPLE_ASSERT(info->m_loadRef != nullptr, "name_crc(0x%08x) has not reference for load.", name_crc);
	return refProxy<T>(*info, GASHA_ unique_shared_lock<lock_type>(info->m_lock, GASHA_ with_lock_shared));
}

//データ取得
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::load(const char* name) const
{
	return load<T>(GASHA_ calcCRC32(name));
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::load(const GASHA_ crc32_t name_crc) const
{
	const refInfo* info = m_refTable.at(name_crc);
	GASHA_SIMPLE_ASSERT(info != nullptr, "name_crc(0x%08x) is not registered.", name_crc);
	GASHA_SIMPLE_ASSERT(info->m_typeInfo && *info->m_typeInfo == typeid(T), "name_crc(0x%08x)'s type and T are different types.", name_crc);
	GASHA_SIMPLE_ASSERT(info->m_loadRef != nullptr, "name_crc(0x%08x) has not reference for load.", name_crc);
	auto lock = info->m_lock.lockSharedScoped();
	return *reinterpret_cast<const T*>(info->m_loadRef);
}

//データ更新
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline void namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::store(const char* name, T&& value)
{
	return store<T>(GASHA_ calcCRC32(name), std::move(value));
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline void namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::store(const GASHA_ crc32_t name_crc, T&& value)
{
	const refInfo* info = m_refTable.at(name_crc);
	GASHA_SIMPLE_ASSERT(info != nullptr, "name_crc(0x%08x) is not registered.", name_crc);
	GASHA_SIMPLE_ASSERT(info->m_typeInfo && *info->m_typeInfo == typeid(T), "name_crc(0x%08x)'s type and T are different types.", name_crc);
	GASHA_SIMPLE_ASSERT(info->m_storeRef != nullptr, "name_crc(0x%08x) has not reference for store.", name_crc);
	auto lock = info->m_lock.lockScoped();
	*reinterpret_cast<T*>(info->m_storeRef) = value;
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline void namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::store(const char* name, const T& value)
{
	return store<T>(GASHA_ calcCRC32(name), value);
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline void namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::store(const GASHA_ crc32_t name_crc, const T& value)
{
	const refInfo* info = m_refTable.at(name_crc);
	GASHA_SIMPLE_ASSERT(info != nullptr, "name_crc(0x%08x) is not registered.", name_crc);
	GASHA_SIMPLE_ASSERT(info->m_typeInfo && *info->m_typeInfo == typeid(T), "name_crc(0x%08x)'s type and T are different types.", name_crc);
	GASHA_SIMPLE_ASSERT(info->m_storeRef != nullptr, "name_crc(0x%08x) has not reference for store.", name_crc);
	auto lock = info->m_lock.lockScoped();
	*reinterpret_cast<T*>(info->m_storeRef) = value;
}

//データ参照登録
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline void namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::regist(const char* name, T& ref)
{
	regist(GASHA_ calcCRC32(name), ref);
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline void namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::regist(const crc32_t name_crc, T& ref)
{
	m_refTable.emplace(name_crc, name_crc, ref);
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline void namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::regist(const char* name, const T& ref)
{
	regist(GASHA_ calcCRC32(name), ref);
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline void namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::regist(const crc32_t name_crc, const T& ref)
{
	m_refTable.emplace(name_crc, name_crc, ref);
}

//デフォルトコンストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::namedRef()
{}

//デストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::~namedRef()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_NAMED_REF_INL

// End of file

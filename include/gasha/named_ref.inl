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
#include <gasha/allocator_common.h>//メモリアロケータ共通設定・処理：callConstructor()
#include <gasha/memory.h>//メモリ操作：adjustAlign()
#include <gasha/basic_math.h>//基本算術：sign()

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

//コンストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::refProxy<T>::refProxy(const refInfo& ref_info, GASHA_ unique_shared_lock<lock_type>&& lock) :
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
///読み取り専用参照情報プロキシー

//オペレータ

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline const T* namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::constRefProxy<T>::operator->() const
{
	return reinterpret_cast<const T*>(m_refInfo.m_loadRef);
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline const T& namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::constRefProxy<T>::operator*() const
{
	return *reinterpret_cast<const T*>(m_refInfo.m_loadRef);
}

//ムーブコンストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::constRefProxy<T>::constRefProxy(const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template constRefProxy<T>&& obj) :
m_refInfo(obj.m_refInfo),
m_lock(std::move(obj.m_lock))
{}

//コンストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::constRefProxy<T>::constRefProxy(const refInfo& ref_info, GASHA_ unique_shared_lock<lock_type>&& lock) :
m_refInfo(ref_info),
m_lock(std::move(lock))
{}

//デフォルトコンストラクタ
//template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
//template<typename T>
//inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::constRefProxy<T>::constRefProxy() :
//	m_refInfo(),
//	m_lock()
//{}

//デストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::constRefProxy<T>::~constRefProxy()
{}

//--------------------
//名前付きデータ参照本体

//データ参照情報取得時のアサーション
//※読み込み操作用
#define GASHA_NAMED_REF_ASSERT_FOR_READ_OPERATION(info, name_crc) \
	GASHA_SIMPLE_ASSERT(info != nullptr, "name_crc(0x%08x) is not registered.", name_crc); \
	GASHA_SIMPLE_ASSERT(info->m_typeInfo && *info->m_typeInfo == typeid(T), "name_crc(0x%08x)'s type and T are different types.", name_crc); \
	GASHA_SIMPLE_ASSERT(info->m_loadRef != nullptr, "name_crc(0x%08x) has not reference for read operation.", name_crc);
//※書き込み操作用
#define GASHA_NAMED_REF_ASSERT_FOR_WRITE_OPERATION(info, name_crc) \
	GASHA_SIMPLE_ASSERT(info != nullptr, "name_crc(0x%08x) is not registered.", name_crc); \
	GASHA_SIMPLE_ASSERT(info->m_typeInfo && *info->m_typeInfo == typeid(T), "name_crc(0x%08x)'s type and T are different types.", name_crc); \
	GASHA_SIMPLE_ASSERT(info->m_storeRef != nullptr, "name_crc(0x%08x) has not reference for write operatuib.", name_crc);

//登録済みチェック
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isRegistered(const GASHA_ crc32_t name_crc) const
{
	return m_refTable->at(name_crc) != nullptr;
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
	const refInfo* info = m_refTable->at(name_crc);
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
	const refInfo* info = m_refTable->at(name_crc);
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
	const refInfo* info = m_refTable->at(name_crc);
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
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template constRefProxy<T> namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::ref(const char* name) const
{
	return ref<T>(GASHA_ calcCRC32(name));
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template constRefProxy<T> namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::ref(const GASHA_ crc32_t name_crc) const
{
	const refInfo* info = m_refTable->at(name_crc);
	GASHA_NAMED_REF_ASSERT_FOR_READ_OPERATION(info, name_crc);
	return constRefProxy<T>(*info, GASHA_ unique_shared_lock<lock_type>(info->m_lock, GASHA_ with_lock_shared));
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
	const refInfo* info = m_refTable->at(name_crc);
	GASHA_NAMED_REF_ASSERT_FOR_WRITE_OPERATION(info, name_crc);
	return refProxy<T>(*info, GASHA_ unique_shared_lock<lock_type>(info->m_lock, GASHA_ with_lock));
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template constRefProxy<T> namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::cref(const char* name) const
{
	return cref<T>(GASHA_ calcCRC32(name));
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template constRefProxy<T> namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::cref(const GASHA_ crc32_t name_crc) const
{
	const refInfo* info = m_refTable->at(name_crc);
	GASHA_NAMED_REF_ASSERT_FOR_READ_OPERATION(info, name_crc);
	return constRefProxy<T>(*info, GASHA_ unique_shared_lock<lock_type>(info->m_lock, GASHA_ with_lock_shared));
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
	const refInfo* info = m_refTable->at(name_crc);
	GASHA_NAMED_REF_ASSERT_FOR_READ_OPERATION(info, name_crc);
	auto lock = info->m_lock.lockSharedScoped();
	return *reinterpret_cast<const T*>(info->m_loadRef);
}

//データ更新
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline void namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::store(const char* name, const T value)
{
	return store<T>(GASHA_ calcCRC32(name), value);
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline void namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::store(const GASHA_ crc32_t name_crc, const T value)
{
	const refInfo* info = m_refTable->at(name_crc);
	GASHA_NAMED_REF_ASSERT_FOR_WRITE_OPERATION(info, name_crc);
	auto lock = info->m_lock.lockScoped();
	*reinterpret_cast<T*>(info->m_storeRef) = value;
}

//データ交換
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::exchange(const GASHA_ crc32_t name_crc, const T value)
{
	const refInfo* info = m_refTable->at(name_crc);
	GASHA_NAMED_REF_ASSERT_FOR_WRITE_OPERATION(info, name_crc);
	auto lock = info->m_lock.lockScoped();
	const T before_value = *reinterpret_cast<T*>(info->m_storeRef);
	*reinterpret_cast<T*>(info->m_storeRef) = value;
	return before_value;
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::exchange(const char* name, const T value)
{
	return exchange<T>(GASHA_ calcCRC32(name), value);
}
//演算
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T, class OPERATOR_POLICY>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::calcurate(const GASHA_ crc32_t name_crc, OPERATOR_POLICY ope, const T rhs, const T max, const T min)
{
	const refInfo* info = m_refTable->at(name_crc);
	GASHA_NAMED_REF_ASSERT_FOR_WRITE_OPERATION(info, name_crc);
	auto lock = info->m_lock.lockScoped();
	const T new_value = ope(*reinterpret_cast<T*>(info->m_storeRef), rhs, max, min);
	*reinterpret_cast<T*>(info->m_storeRef) = new_value;
	return new_value;
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::inc(const GASHA_ crc32_t name_crc)
{
	return calcurate<T>(name_crc, GASHA_ inc_policy<T>, static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::inc(const char* name)
{
	return inc<T>(GASHA_ calcCRC32(name));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::incWA(const GASHA_ crc32_t name_crc, const T max, const T min)
{
	return calcurate<T>(name_crc, GASHA_ incWA_policy<T>, static_cast<T>(0), max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::incWA(const char* name, const T max, const T min)
{
	return incWA<T>(GASHA_ calcCRC32(name), max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::incSA(const GASHA_ crc32_t name_crc, const T max, const T min)
{
	return calcurate<T>(name_crc, GASHA_ incSA_policy<T>, static_cast<T>(0), max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::incSA(const char* name, const T max, const T min)
{
	return incSA<T>(GASHA_ calcCRC32(name), max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::dec(const GASHA_ crc32_t name_crc)
{
	return calcurate<T>(name_crc, GASHA_ dec_policy<T>, static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::dec(const char* name)
{
	return dec<T>(GASHA_ calcCRC32(name));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::decWA(const GASHA_ crc32_t name_crc, const T max, const T min)
{
	return calcurate<T>(name_crc, GASHA_ decWA_policy<T>, static_cast<T>(0), max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::decWA(const char* name, const T max, const T min)
{
	return decWA<T>(GASHA_ calcCRC32(name), max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::decSA(const GASHA_ crc32_t name_crc, const T max, const T min)
{
	return calcurate<T>(name_crc, GASHA_ decSA_policy<T>, static_cast<T>(0), max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::decSA(const char* name, const T max, const T min)
{
	return decSA<T>(GASHA_ calcCRC32(name), max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::add(const GASHA_ crc32_t name_crc, const T rhs)
{
	return calcurate<T>(name_crc, GASHA_ add_policy<T>, rhs, static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::add(const char* name, const T rhs)
{
	return add<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::addWA(const GASHA_ crc32_t name_crc, const T rhs, const T max, const T min)
{
	return calcurate<T>(name_crc, GASHA_ addWA_policy<T>, rhs, max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::addWA(const char* name, const T rhs, const T max, const T min)
{
	return addWA<T>(GASHA_ calcCRC32(name), rhs, max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::addSA(const GASHA_ crc32_t name_crc, const T rhs, const T max, const T min)
{
	return calcurate<T>(name_crc, GASHA_ addSA_policy<T>, rhs, max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::addSA(const char* name, const T rhs, const T max, const T min)
{
	return addSA<T>(GASHA_ calcCRC32(name), rhs, max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::sub(const GASHA_ crc32_t name_crc, const T rhs)
{
	return calcurate<T>(name_crc, GASHA_ sub_policy<T>, rhs, static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::sub(const char* name, const T rhs)
{
	return sub<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::subWA(const GASHA_ crc32_t name_crc, const T rhs, const T max, const T min)
{
	return calcurate<T>(name_crc, GASHA_ subWA_policy<T>, rhs, max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::subWA(const char* name, const T rhs, const T max, const T min)
{
	return subWA<T>(GASHA_ calcCRC32(name), rhs, max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::subSA(const GASHA_ crc32_t name_crc, const T rhs, const T max, const T min)
{
	return calcurate<T>(name_crc, GASHA_ subSA_policy<T>, rhs, max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::subSA(const char* name, const T rhs, const T max, const T min)
{
	return subSA<T>(GASHA_ calcCRC32(name), rhs, max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::mul(const GASHA_ crc32_t name_crc, const T rhs)
{
	return calcurate<T>(name_crc, GASHA_ mul_policy<T>, rhs, static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::mul(const char* name, const T rhs)
{
	return mul<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::mulWA(const GASHA_ crc32_t name_crc, const T rhs, const T max, const T min)
{
	return calcurate<T>(name_crc, GASHA_ mulWA_policy<T>, rhs, max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::mulWA(const char* name, const T rhs, const T max, const T min)
{
	return mulWA<T>(GASHA_ calcCRC32(name), rhs, max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::mulSA(const GASHA_ crc32_t name_crc, const T rhs, const T max, const T min)
{
	return calcurate<T>(name_crc, GASHA_ mulSA_policy<T>, rhs, max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::mulSA(const char* name, const T rhs, const T max, const T min)
{
	return mulSA<T>(GASHA_ calcCRC32(name), rhs, max, min);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::div(const GASHA_ crc32_t name_crc, const T rhs)
{
	return calcurate<T>(name_crc, GASHA_ div_policy<T>, rhs, static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::div(const char* name, const T rhs)
{
	return div<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::mod(const GASHA_ crc32_t name_crc, const T rhs)
{
	return calcurate<T>(name_crc, GASHA_ mod_policy<T>, rhs, static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::mod(const char* name, const T rhs)
{
	return mod<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::bitAnd(const GASHA_ crc32_t name_crc, const T rhs)
{
	return calcurate<T>(name_crc, GASHA_ bitAnd_policy<T>, rhs, static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::bitAnd(const char* name, const T rhs)
{
	return bitAnd<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::bitOr(const GASHA_ crc32_t name_crc, const T rhs)
{
	return calcurate<T>(name_crc, GASHA_ bitOr_policy<T>, rhs, static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::bitOr(const char* name, const T rhs)
{
	return bitOr<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::bitXor(const GASHA_ crc32_t name_crc, const T rhs)
{
	return calcurate<T>(name_crc, GASHA_ bitXor_policy<T>, rhs, static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::bitXor(const char* name, const T rhs)
{
	return bitXor<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::bitNot(const GASHA_ crc32_t name_crc)
{
	return calcurate<T>(name_crc, GASHA_ bitNot_policy<T>, static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::bitNot(const char* name)
{
	return bitNot<T>(GASHA_ calcCRC32(name));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lShift(const GASHA_ crc32_t name_crc, const int rhs)
{
	return calcurate<T>(name_crc, GASHA_ lShift_policy<T>, static_cast<T>(rhs), static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lShift(const char* name, const int rhs)
{
	return lShift<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::rShift(const GASHA_ crc32_t name_crc, const int rhs)
{
	return calcurate<T>(name_crc, GASHA_ rShift_policy<T>, static_cast<T>(rhs), static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::rShift(const char* name, const int rhs)
{
	return rShift<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::bitOn(const GASHA_ crc32_t name_crc, const T rhs)
{
	return calcurate<T>(name_crc, GASHA_ bitOn_policy<T>, rhs, static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::bitOn(const char* name, const T rhs)
{
	return bitOn<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::bitOff(const GASHA_ crc32_t name_crc, const T rhs)
{
	return calcurate<T>(name_crc, GASHA_ bitOff_policy<T>, rhs, static_cast<T>(0), static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline T namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::bitOff(const char* name, const T rhs)
{
	return bitOff<T>(GASHA_ calcCRC32(name), rhs);
}
//比較
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T, class OPERATOR_POLICY>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::compare(const GASHA_ crc32_t name_crc, OPERATOR_POLICY ope, const T rhs)
{
	const refInfo* info = m_refTable->at(name_crc);
	GASHA_NAMED_REF_ASSERT_FOR_READ_OPERATION(info, name_crc);
	auto lock = info->m_lock.lockSharedScoped();
	return ope(*reinterpret_cast<const T*>(info->m_loadRef), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::eq(const GASHA_ crc32_t name_crc, const T rhs)
{
	return compare<T>(name_crc, GASHA_ eq_policy<T>, rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::eq(const char* name, const T rhs)
{
	return eq<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::ne(const GASHA_ crc32_t name_crc, const T rhs)
{
	return compare<T>(name_crc, GASHA_ ne_policy<T>, rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::ne(const char* name, const T rhs)
{
	return ne<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::gt(const GASHA_ crc32_t name_crc, const T rhs)
{
	return compare<T>(name_crc, GASHA_ gt_policy<T>, rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::gt(const char* name, const T rhs)
{
	return gt<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::ge(const GASHA_ crc32_t name_crc, const T rhs)
{
	return compare<T>(name_crc, GASHA_ ge_policy<T>, rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::ge(const char* name, const T rhs)
{
	return ge<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lt(const GASHA_ crc32_t name_crc, const T rhs)
{
	return compare<T>(name_crc, GASHA_ lt_policy<T>, rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lt(const char* name, const T rhs)
{
	return lt<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::le(const GASHA_ crc32_t name_crc, const T rhs)
{
	return compare<T>(name_crc, GASHA_ le_policy<T>, rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::le(const char* name, const T rhs)
{
	return le<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::logicalAnd(const GASHA_ crc32_t name_crc, const T rhs)
{
	return compare<T>(name_crc, GASHA_ logicalAnd_policy<T>, rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::logicalAnd(const char* name, const T rhs)
{
	return logicalAnd<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::logicalOr(const GASHA_ crc32_t name_crc, const T rhs)
{
	return compare<T>(name_crc, GASHA_ logicalOr_policy<T>, rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::logicalOr(const char* name, const T rhs)
{
	return logicalOr<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isOn(const GASHA_ crc32_t name_crc, const T rhs)
{
	return compare<T>(name_crc, GASHA_ isOn_policy<T>, rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isOn(const char* name, const T rhs)
{
	return isOn<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isOff(const GASHA_ crc32_t name_crc, const T rhs)
{
	return compare<T>(name_crc, GASHA_ isOff_policy<T>, rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isOff(const char* name, const T rhs)
{
	return isOff<T>(GASHA_ calcCRC32(name), rhs);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isTrue(const GASHA_ crc32_t name_crc)
{
	return compare<T>(name_crc, GASHA_ isTrue_policy<T>, static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isTrue(const char* name)
{
	return isTrue<T>(GASHA_ calcCRC32(name));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isFalse(const GASHA_ crc32_t name_crc)
{
	return compare<T>(name_crc, GASHA_ isFalse_policy<T>, static_cast<T>(0));
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::isFalse(const char* name)
{
	return isFalse<T>(GASHA_ calcCRC32(name));
}

//データ参照登録
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::regist(const crc32_t name_crc, T& ref)
{
	const refInfo* info = m_refTable->emplace(name_crc, name_crc, ref);
	return info ? true : false;
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::regist(const char* name, T& ref)
{
	return regist(GASHA_ calcCRC32(name), ref);
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::regist(const crc32_t name_crc, const T& ref)
{
	const refInfo* info = m_refTable->emplace(name_crc, name_crc, ref);
	return info ? true : false;
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
template<typename T>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::regist(const char* name, const T& ref)
{
	return regist(GASHA_ calcCRC32(name), ref);
}

//データ登録解除
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::unregist(const GASHA_ crc32_t name_crc)
{
	return m_refTable->erase(name_crc);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::unregist(const char* name)
{
	return unregist(GASHA_ calcCRC32(name));
}

//クリア
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline void namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::clear()
{
	m_refTable->clear();
}
//初期状態にする
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline void namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::initialize()
{
	std::call_once(m_initialized, initializeOnce);
}

//明示的な再初期化
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline void namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::explicitInitialize()
{
	initializeOnce();
}

//ハッシュテーブルアクセス：アクセッサ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline std::size_t namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::max_size() const
{
	return m_refTable->max_size();
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline std::size_t namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::size() const
{
	return m_refTable->size();
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline std::size_t namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::remain() const
{
	return m_refTable->remain();
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::empty() const
{
	return m_refTable->empty();
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline bool namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::full() const
{
	return m_refTable->full();
}

//ハッシュテーブルアクセス：ロック取得系メソッド
//単一ロック取得
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline GASHA_ unique_shared_lock<typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lock_type> namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lockUnique(const GASHA_ with_lock_shared_t&) const
{
	return m_refTable->lockUnique(GASHA_ with_lock_shared);
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline GASHA_ unique_shared_lock<typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lock_type> namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lockUnique(const GASHA_ try_to_lock_shared_t&) const
{
	return m_refTable->lockUnique(GASHA_ try_to_lock_shared);
}
//スコープロック取得
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline GASHA_ shared_lock_guard<typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lock_type> namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lockSharedScoped() const
{
	return m_refTable->lockSharedScoped();
}
//ハッシュテーブルアクセス：イテレータ取得系メソッド
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::iterator namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::cbegin() const
{
	return m_refTable->cbegin();
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::iterator namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::cend() const
{
	return m_refTable->cend();
}

template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::iterator namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::begin() const
{
	return m_refTable->begin();
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::iterator namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::end() const
{
	return m_refTable->end();
}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
//リバースイテレータを取得
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::reverse_iterator namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::crbegin() const
{
	return m_refTable->crbegin();
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::reverse_iterator namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::crend() const
{
	return m_refTable->crend();
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::reverse_iterator namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::rbegin() const
{
	return m_refTable->rbegin();
}
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::reverse_iterator namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::rend() const
{
	return m_refTable->rend();
}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR

//明示的な初期化用コンストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::namedRef(const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::explicitInit_type&)
{
	explicitInitialize();
}

//デフォルトコンストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::namedRef() :
	m_refTable(reinterpret_cast<table_type*>(GASHA_ adjustAlign<char, alignof(table_type)>(m_refTableBuff)))
{
//	initialize();//実行しない
}

//デストラクタ
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY>
inline namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::~namedRef()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_NAMED_REF_INL

// End of file

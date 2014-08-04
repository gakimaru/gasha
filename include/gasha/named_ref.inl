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

#include <gasha/basic_math.h>//基本算術
#include <gasha/allocator_common.h>//メモリアロケータ共通設定・処理：callConstructor()
#include <gasha/memory.h>//メモリ操作：adjustAlign()
#include <gasha/limits.h>//限界値
#include <gasha/simple_assert.h>//シンプルアサーション

#include <utility>//C++11 std::move()
#include <cstddef>//std::intptr_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//名前付きデータ参照
//--------------------------------------------------------------------------------

namespace named_ref
{
	//----------------------------------------
	//名前付きデータ参照クラス

	//--------------------
	//参照情報

	//アクセッサ
	template<class OPE_TYPE>
	template<typename T>
	inline const T& table<OPE_TYPE>::refInfo::max() const
	{
		if (sizeof(T) <= sizeof(void*))
		{
			const std::intptr_t p = reinterpret_cast<std::intptr_t>(&m_refMax);
			return *reinterpret_cast<const T*>(p);
		}
		return *reinterpret_cast<const T*>(m_refMax);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline const T& table<OPE_TYPE>::refInfo::min() const
	{
		if (sizeof(T) <= sizeof(void*))
		{
			const std::intptr_t p = reinterpret_cast<std::intptr_t>(&m_refMin);
			return *reinterpret_cast<const T*>(p);
		}
		return *reinterpret_cast<const T*>(m_refMin);
	}
	
	//メソッド
	template<class OPE_TYPE>
	template<typename T>
	inline void table<OPE_TYPE>::refInfo::setMinMax(const T& max, const T& min)
	{
		//(sizeof(T) <= sizeof(void*) の時は、max/min の参照ではなく値を保持する（void* に値を強引に書き込む）
		if (sizeof(T) <= sizeof(void*))
		{
			{
				const std::intptr_t p = reinterpret_cast<std::intptr_t>(&m_refMax);
				*reinterpret_cast<T*>(p) = max;
			}
			{
			const std::intptr_t p = reinterpret_cast<std::intptr_t>(&m_refMin);
				*reinterpret_cast<T*>(p) = min;
			}
		}
	}

	//コンストラクタ
	template<class OPE_TYPE>
	template<typename T>
	inline table<OPE_TYPE>::refInfo::refInfo(const GASHA_ crc32_t name_crc, T& value) :
		m_nameCrc(name_crc),
		m_typeInfo(&typeid(T)),
		m_accessType(WRITABLE),
		m_ref(&value),
		m_refMax(nullptr),
		m_refMin(nullptr)
	{}
	template<class OPE_TYPE>
	template<typename T>
	inline table<OPE_TYPE>::refInfo::refInfo(const GASHA_ crc32_t name_crc, T& value, const wraparound_tag&, const T& max, const T& min) :
		m_nameCrc(name_crc),
		m_typeInfo(&typeid(T)),
		m_accessType(WRITABLE_WRAPAROUND),
		m_ref(&value),
		m_refMax(&max),
		m_refMin(&min)
	{
		setMinMax(max, min);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline table<OPE_TYPE>::refInfo::refInfo(const GASHA_ crc32_t name_crc, T& value, const saturation_tag&, const T& max, const T& min) :
		m_nameCrc(name_crc),
		m_typeInfo(&typeid(T)),
		m_accessType(WRITABLE_SATURATION),
		m_ref(&value),
		m_refMax(&max),
		m_refMin(&min)
	{
		setMinMax(max, min);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline table<OPE_TYPE>::refInfo::refInfo(const GASHA_ crc32_t name_crc, const T& value) :
		m_nameCrc(name_crc),
		m_typeInfo(&typeid(T)),
		m_accessType(READ_ONLY),
		m_ref(&value),
		m_refMax(nullptr),
		m_refMin(nullptr)
	{}
	template<class OPE_TYPE>
	template<typename T>
	inline table<OPE_TYPE>::refInfo::refInfo(const GASHA_ crc32_t name_crc, const T& value, const readonly_tag&) :
		m_nameCrc(name_crc),
		m_typeInfo(&typeid(T)),
		m_accessType(READ_ONLY),
		m_ref(&value),
		m_refMax(nullptr),
		m_refMin(nullptr)
	{}

	//デフォルトコンストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::refInfo::refInfo() :
		m_nameCrc(0),
		m_typeInfo(nullptr),
		m_accessType(READ_ONLY),
		m_ref(nullptr),
		m_refMax(nullptr),
		m_refMin(nullptr)
	{}

	//デストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::refInfo::~refInfo()
	{}

	//--------------------
	//参照情報プロキシー

	//オペレータ

	template<class OPE_TYPE>
	template<typename T>
	inline const T* table<OPE_TYPE>::refProxy<T>::operator->() const
	{
		return reinterpret_cast<const T*>(m_refInfo.m_ref);
	}

	template<class OPE_TYPE>
	template<typename T>
	inline T* table<OPE_TYPE>::refProxy<T>::operator->()
	{
		return const_cast<T*>(reinterpret_cast<const T*>(m_refInfo.m_ref));
	}

	template<class OPE_TYPE>
	template<typename T>
	inline const T& table<OPE_TYPE>::refProxy<T>::operator*() const
	{
		return *reinterpret_cast<const T*>(m_refInfo.m_ref);
	}

	template<class OPE_TYPE>
	template<typename T>
	inline T& table<OPE_TYPE>::refProxy<T>::operator*()
	{
		return *const_cast<T*>(reinterpret_cast<const T*>(m_refInfo.m_ref));
	}

	//ムーブコンストラクタ
	template<class OPE_TYPE>
	template<typename T>
	inline table<OPE_TYPE>::refProxy<T>::refProxy(typename table<OPE_TYPE>::template refProxy<T>&& obj) :
		m_refInfo(obj.m_refInfo),
		m_lock(std::move(obj.m_lock))
	{}

	//コンストラクタ
	template<class OPE_TYPE>
	template<typename T>
	inline table<OPE_TYPE>::refProxy<T>::refProxy(const refInfo& ref_info, GASHA_ unique_shared_lock<lock_type>&& lock) :
		m_refInfo(ref_info),
		m_lock(std::move(lock))
	{}

	//デフォルトコンストラクタ
	//template<class OPE_TYPE>
	//template<typename T>
	//inline table<OPE_TYPE>::refProxy<T>::refProxy() :
	//	m_refInfo(),
	//	m_lock()
	//{}

	//デストラクタ
	template<class OPE_TYPE>
	template<typename T>
	inline table<OPE_TYPE>::refProxy<T>::~refProxy()
	{}

	//--------------------
	///読み取り専用参照情報プロキシー

	//オペレータ

	template<class OPE_TYPE>
	template<typename T>
	inline const T* table<OPE_TYPE>::constRefProxy<T>::operator->() const
	{
		return reinterpret_cast<const T*>(m_refInfo.m_ref);
	}

	template<class OPE_TYPE>
	template<typename T>
	inline const T& table<OPE_TYPE>::constRefProxy<T>::operator*() const
	{
		return *reinterpret_cast<const T*>(m_refInfo.m_ref);
	}

	//ムーブコンストラクタ
	template<class OPE_TYPE>
	template<typename T>
	inline table<OPE_TYPE>::constRefProxy<T>::constRefProxy(const typename table<OPE_TYPE>::template constRefProxy<T>&& obj) :
		m_refInfo(obj.m_refInfo),
		m_lock(std::move(obj.m_lock))
	{}

	//コンストラクタ
	template<class OPE_TYPE>
	template<typename T>
	inline table<OPE_TYPE>::constRefProxy<T>::constRefProxy(const refInfo& ref_info, GASHA_ unique_shared_lock<lock_type>&& lock) :
		m_refInfo(ref_info),
		m_lock(std::move(lock))
	{}

	//デフォルトコンストラクタ
	//template<class OPE_TYPE>
	//template<typename T>
	//inline table<OPE_TYPE>::constRefProxy<T>::constRefProxy() :
	//	m_refInfo(),
	//	m_lock()
	//{}

	//デストラクタ
	template<class OPE_TYPE>
	template<typename T>
	inline table<OPE_TYPE>::constRefProxy<T>::~constRefProxy()
	{}

	//--------------------
	//名前付きデータ参照本体

	//データ参照情報取得時のアサーション
	//※読み込み操作用
#define GASHA_NAMED_REF_ASSERT_FOR_READ_OPERATION(info, name_crc) \
	GASHA_SIMPLE_ASSERT(info != nullptr, "name_crc(0x%08x) is not registered.", name_crc); \
	GASHA_SIMPLE_ASSERT(info->m_typeInfo && *info->m_typeInfo == typeid(T), "name_crc(0x%08x)'s type and T are different types.(%s and %s)", name_crc, info->m_typeInfo->name(), typeid(T).name()); \
	GASHA_SIMPLE_ASSERT(info->m_ref != nullptr, "name_crc(0x%08x) has not reference for read operation.", name_crc);
	//※書き込み操作用
#define GASHA_NAMED_REF_ASSERT_FOR_WRITE_OPERATION(info, name_crc) \
	GASHA_SIMPLE_ASSERT(info != nullptr, "name_crc(0x%08x) is not registered.", name_crc); \
	GASHA_SIMPLE_ASSERT(info->m_typeInfo && *info->m_typeInfo == typeid(T), "name_crc(0x%08x)'s type and T are different types.(%s and %s)", name_crc, info->m_typeInfo->name(), typeid(T).name()); \
	GASHA_SIMPLE_ASSERT(info->m_ref != nullptr && info->m_accessType != READ_ONLY, "name_crc(0x%08x) has not reference for write operation.", name_crc);

	//登録済みチェック
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::isRegistered(const GASHA_ crc32_t name_crc) const
	{
		return m_refTable->at(name_crc) != nullptr;
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::isRegistered(const char* name) const
	{
		return isRegistered(GASHA_ calcCRC32(name));
	}

	//読み取り専用チェック
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isReadOnly(const GASHA_ crc32_t name_crc) const
	{
		const refInfo* info = m_refTable->at(name_crc);
		return info && info->m_ref && info->m_accessType == READ_ONLY && info->m_typeInfo && *info->m_typeInfo == typeid(T);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isReadOnly(const char* name) const
	{
		return isReadOnly<T>(GASHA_ calcCRC32(name));
	}

	//読み取り可能チェック
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isReadable(const GASHA_ crc32_t name_crc) const
	{
		const refInfo* info = m_refTable->at(name_crc);
		return info && info->m_ref && info->m_typeInfo && *info->m_typeInfo == typeid(T);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isReadable(const char* name) const
	{
		return isReadable<T>(GASHA_ calcCRC32(name));
	}

	//書き込み可能チェック
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isWritable(const GASHA_ crc32_t name_crc) const
	{
		const refInfo* info = m_refTable->at(name_crc);
		return info && info->m_ref && info->m_accessType != READ_ONLY && info->m_typeInfo && *info->m_typeInfo == typeid(T);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isWritable(const char* name) const
	{
		return isWritable<T>(GASHA_ calcCRC32(name));
	}
	//ラップアラウンド演算／飽和演算で書き込み可能チェック
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isWritableRanged(const GASHA_ crc32_t name_crc) const
	{
		const refInfo* info = m_refTable->at(name_crc);
		return info && info->m_ref && (info->m_accessType == WRITABLE_WRAPAROUND || info->m_accessType == WRITABLE_SATURATION) && info->m_typeInfo && *info->m_typeInfo == typeid(T);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isWritableRanged(const char* name) const
	{
		return isWritableRanged<T>(GASHA_ calcCRC32(name));
	}

	//ラップアラウンド演算で書き込み可能チェック
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isWritableWraparound(const GASHA_ crc32_t name_crc) const
	{
		const refInfo* info = m_refTable->at(name_crc);
		return info && info->m_ref && info->m_accessType == WRITABLE_WRAPAROUND && info->m_typeInfo && *info->m_typeInfo == typeid(T);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isWritableWraparound(const char* name) const
	{
		return isWritableWraparound<T>(GASHA_ calcCRC32(name));
	}

	//飽和演算で書き込み可能チェック
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isWritableSaturation(const GASHA_ crc32_t name_crc) const
	{
		const refInfo* info = m_refTable->at(name_crc);
		return info && info->m_ref && info->m_accessType == WRITABLE_SATURATION && info->m_typeInfo && *info->m_typeInfo == typeid(T);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isWritableSaturation(const char* name) const
	{
		return isWritableSaturation<T>(GASHA_ calcCRC32(name));
	}

	//データ参照
	template<class OPE_TYPE>
	template<typename T>
	inline const typename table<OPE_TYPE>::template constRefProxy<T> table<OPE_TYPE>::ref(const GASHA_ crc32_t name_crc) const
	{
		const refInfo* info = m_refTable->at(name_crc);
		GASHA_NAMED_REF_ASSERT_FOR_READ_OPERATION(info, name_crc);
		return constRefProxy<T>(*info, GASHA_ unique_shared_lock<lock_type>(info->m_lock, GASHA_ with_lock_shared));
	}
	template<class OPE_TYPE>
	template<typename T>
	inline const typename table<OPE_TYPE>::template constRefProxy<T> table<OPE_TYPE>::ref(const char* name) const
	{
		return ref<T>(GASHA_ calcCRC32(name));
	}

	template<class OPE_TYPE>
	template<typename T>
	inline typename table<OPE_TYPE>::template refProxy<T> table<OPE_TYPE>::ref(const GASHA_ crc32_t name_crc)
	{
		const refInfo* info = m_refTable->at(name_crc);
		GASHA_NAMED_REF_ASSERT_FOR_WRITE_OPERATION(info, name_crc);
		return refProxy<T>(*info, GASHA_ unique_shared_lock<lock_type>(info->m_lock, GASHA_ with_lock));
	}
	template<class OPE_TYPE>
	template<typename T>
	inline typename table<OPE_TYPE>::template refProxy<T> table<OPE_TYPE>::ref(const char* name)
	{
		return ref<T>(GASHA_ calcCRC32(name));
	}

	template<class OPE_TYPE>
	template<typename T>
	inline const typename table<OPE_TYPE>::template constRefProxy<T> table<OPE_TYPE>::cref(const GASHA_ crc32_t name_crc) const
	{
		const refInfo* info = m_refTable->at(name_crc);
		GASHA_NAMED_REF_ASSERT_FOR_READ_OPERATION(info, name_crc);
		return constRefProxy<T>(*info, GASHA_ unique_shared_lock<lock_type>(info->m_lock, GASHA_ with_lock_shared));
	}
	template<class OPE_TYPE>
	template<typename T>
	inline const typename table<OPE_TYPE>::template constRefProxy<T> table<OPE_TYPE>::cref(const char* name) const
	{
		return cref<T>(GASHA_ calcCRC32(name));
	}

	//ラップアラウンド演算／飽和演算用の最大値取得
	template<class OPE_TYPE>
	template<typename T>
	inline const T table<OPE_TYPE>::max(const GASHA_ crc32_t name_crc) const
	{
		const refInfo* info = m_refTable->at(name_crc);
		GASHA_NAMED_REF_ASSERT_FOR_READ_OPERATION(info, name_crc);
		return info->max<T>();
	}
	template<class OPE_TYPE>
	template<typename T>
	inline const T table<OPE_TYPE>::max(const char* name) const
	{
		return max<T>(GASHA_ calcCRC32(name));
	}

	//ラップアラウンド演算／飽和演算用の最小値取得
	template<class OPE_TYPE>
	template<typename T>
	inline const T table<OPE_TYPE>::min(const GASHA_ crc32_t name_crc) const
	{
		const refInfo* info = m_refTable->at(name_crc);
		GASHA_NAMED_REF_ASSERT_FOR_READ_OPERATION(info, name_crc);
		return info->min<T>();
	}
	template<class OPE_TYPE>
	template<typename T>
	inline const T table<OPE_TYPE>::min(const char* name) const
	{
		return min<T>(GASHA_ calcCRC32(name));
	}

	//データ取得
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::load(const GASHA_ crc32_t name_crc) const
	{
		const refInfo* info = m_refTable->at(name_crc);
		GASHA_NAMED_REF_ASSERT_FOR_READ_OPERATION(info, name_crc);
		{
			auto lock = info->m_lock.lockSharedScoped();
			return *reinterpret_cast<const T*>(info->m_ref);
		}
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::load(const char* name) const
	{
		return load<T>(GASHA_ calcCRC32(name));
	}

	//データ更新
	template<class OPE_TYPE>
	template<typename T>
	inline void table<OPE_TYPE>::store(const GASHA_ crc32_t name_crc, const T value)
	{
		const refInfo* info = m_refTable->at(name_crc);
		GASHA_NAMED_REF_ASSERT_FOR_WRITE_OPERATION(info, name_crc);
		{
			auto lock = info->m_lock.lockScoped();
			*const_cast<T*>(reinterpret_cast<const T*>(info->m_ref)) = value;
		}
	}
	template<class OPE_TYPE>
	template<typename T>
	inline void table<OPE_TYPE>::store(const char* name, const T value)
	{
		return store<T>(GASHA_ calcCRC32(name), value);
	}

	//データ交換
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::exchange(const GASHA_ crc32_t name_crc, const T value)
	{
		const refInfo* info = m_refTable->at(name_crc);
		GASHA_NAMED_REF_ASSERT_FOR_WRITE_OPERATION(info, name_crc);
		{
			auto lock = info->m_lock.lockScoped();
			const T before_value = *reinterpret_cast<const T*>(info->m_ref);
			*const_cast<T*>(reinterpret_cast<const T*>(info->m_ref)) = value;
			return before_value;
		}
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::exchange(const char* name, const T value)
	{
		return exchange<T>(GASHA_ calcCRC32(name), value);
	}
	//演算
	template<class OPE_TYPE>
	template<typename T, typename T2, class OPERATOR_POLICY, class OPERATOR_WRAPAROUND_POLICY, class OPERATOR_SATURATION_POLICY>
	inline T table<OPE_TYPE>::calcurate(const GASHA_ crc32_t name_crc, OPERATOR_POLICY ope, OPERATOR_WRAPAROUND_POLICY ope_wraparound, OPERATOR_SATURATION_POLICY ope_saturation, const T2 rhs)
	{
		const refInfo* info = m_refTable->at(name_crc);
		GASHA_NAMED_REF_ASSERT_FOR_WRITE_OPERATION(info, name_crc);
		if (info->m_accessType == WRITABLE)
		{
			auto lock = info->m_lock.lockScoped();
			const T new_value = ope(*reinterpret_cast<const T*>(info->m_ref), rhs, GASHA_ numeric_limits<T>::zero(), GASHA_ numeric_limits<T>::zero());
			*const_cast<T*>(reinterpret_cast<const T*>(info->m_ref)) = new_value;
			return new_value;
		}
		else if (info->m_accessType == WRITABLE_WRAPAROUND)
		{
			auto lock = info->m_lock.lockScoped();
			const T new_value = ope_wraparound(*reinterpret_cast<const T*>(info->m_ref), rhs, info->max<T>(), info->min<T>());
			*const_cast<T*>(reinterpret_cast<const T*>(info->m_ref)) = new_value;
			return new_value;
		}
		else if (info->m_accessType == WRITABLE_SATURATION)
		{
			auto lock = info->m_lock.lockScoped();
			const T new_value = ope_saturation(*reinterpret_cast<const T*>(info->m_ref), rhs, info->max<T>(), info->min<T>());
			*const_cast<T*>(reinterpret_cast<const T*>(info->m_ref)) = new_value;
			return new_value;
		}
		return GASHA_ numeric_limits<T>::zero();//ダミー
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::inc(const GASHA_ crc32_t name_crc)
	{
		return calcurate<T, T>(name_crc, GASHA_ inc_policy<T>, GASHA_ inc_wraparound_policy<T>, GASHA_ inc_saturation_policy<T>, GASHA_ numeric_limits<T>::zero());
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::inc(const char* name)
	{
		return inc<T>(GASHA_ calcCRC32(name));
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::dec(const GASHA_ crc32_t name_crc)
	{
		return calcurate<T, T>(name_crc, GASHA_ dec_policy<T>, GASHA_ dec_wraparound_policy<T>, GASHA_ dec_saturation_policy<T>, GASHA_ numeric_limits<T>::zero());
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::dec(const char* name)
	{
		return dec<T>(GASHA_ calcCRC32(name));
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::add(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return calcurate<T, T>(name_crc, GASHA_ add_policy<T>, GASHA_ add_wraparound_policy<T>, GASHA_ add_saturation_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::add(const char* name, const T rhs)
	{
		return add<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::sub(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return calcurate<T, T>(name_crc, GASHA_ sub_policy<T>, GASHA_ sub_wraparound_policy<T>, GASHA_ sub_saturation_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::sub(const char* name, const T rhs)
	{
		return sub<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::mul(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return calcurate<T, T>(name_crc, GASHA_ mul_policy<T>, GASHA_ mul_wraparound_policy<T>, GASHA_ mul_saturation_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::mul(const char* name, const T rhs)
	{
		return mul<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::div(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return calcurate<T, T>(name_crc, GASHA_ div_policy<T>, GASHA_ div_policy<T>, GASHA_ div_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::div(const char* name, const T rhs)
	{
		return div<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::mod(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return calcurate<T, T>(name_crc, GASHA_ mod_policy<T>, GASHA_ mod_policy<T>, GASHA_ mod_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::mod(const char* name, const T rhs)
	{
		return mod<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::bitAnd(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return calcurate<T, T>(name_crc, GASHA_ bitAnd_policy<T>, GASHA_ bitAnd_policy<T>, GASHA_ bitAnd_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::bitAnd(const char* name, const T rhs)
	{
		return bitAnd<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::bitOr(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return calcurate<T, T>(name_crc, GASHA_ bitOr_policy<T>, GASHA_ bitOr_policy<T>, GASHA_ bitOr_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::bitOr(const char* name, const T rhs)
	{
		return bitOr<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::bitXor(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return calcurate<T, T>(name_crc, GASHA_ bitXor_policy<T>, GASHA_ bitXor_policy<T>, GASHA_ bitXor_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::bitXor(const char* name, const T rhs)
	{
		return bitXor<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::bitNot(const GASHA_ crc32_t name_crc)
	{
		return calcurate<T, T>(name_crc, GASHA_ bitNot_policy<T>, GASHA_ bitNot_policy<T>, GASHA_ bitNot_policy<T>, GASHA_ numeric_limits<T>::zero());
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::bitNot(const char* name)
	{
		return bitNot<T>(GASHA_ calcCRC32(name));
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::lShift(const GASHA_ crc32_t name_crc, const int rhs)
	{
		return calcurate<T, int>(name_crc, GASHA_ lShift_policy<T>, GASHA_ lShift_policy<T>, GASHA_ lShift_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::lShift(const char* name, const int rhs)
	{
		return lShift<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::rShift(const GASHA_ crc32_t name_crc, const int rhs)
	{
		return calcurate<T, int>(name_crc, GASHA_ rShift_policy<T>, GASHA_ rShift_policy<T>, GASHA_ rShift_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::rShift(const char* name, const int rhs)
	{
		return rShift<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::bitOn(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return calcurate<T, T>(name_crc, GASHA_ bitOn_policy<T>, GASHA_ bitOn_policy<T>, GASHA_ bitOn_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::bitOn(const char* name, const T rhs)
	{
		return bitOn<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::bitOff(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return calcurate<T, T>(name_crc, GASHA_ bitOff_policy<T>, GASHA_ bitOff_policy<T>, GASHA_ bitOff_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline T table<OPE_TYPE>::bitOff(const char* name, const T rhs)
	{
		return bitOff<T>(GASHA_ calcCRC32(name), rhs);
	}
	//比較
	template<class OPE_TYPE>
	template<typename T, class OPERATOR_POLICY>
	inline bool table<OPE_TYPE>::compare(const GASHA_ crc32_t name_crc, OPERATOR_POLICY ope, const T rhs)
	{
		const refInfo* info = m_refTable->at(name_crc);
		GASHA_NAMED_REF_ASSERT_FOR_READ_OPERATION(info, name_crc);
		{
			auto lock = info->m_lock.lockSharedScoped();
			return ope(*reinterpret_cast<const T*>(info->m_ref), rhs);
		}
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::eq(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return compare<T>(name_crc, GASHA_ eq_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::eq(const char* name, const T rhs)
	{
		return eq<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::ne(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return compare<T>(name_crc, GASHA_ ne_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::ne(const char* name, const T rhs)
	{
		return ne<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::gt(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return compare<T>(name_crc, GASHA_ gt_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::gt(const char* name, const T rhs)
	{
		return gt<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::ge(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return compare<T>(name_crc, GASHA_ ge_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::ge(const char* name, const T rhs)
	{
		return ge<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::lt(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return compare<T>(name_crc, GASHA_ lt_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::lt(const char* name, const T rhs)
	{
		return lt<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::le(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return compare<T>(name_crc, GASHA_ le_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::le(const char* name, const T rhs)
	{
		return le<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::logicalAnd(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return compare<T>(name_crc, GASHA_ logicalAnd_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::logicalAnd(const char* name, const T rhs)
	{
		return logicalAnd<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::logicalOr(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return compare<T>(name_crc, GASHA_ logicalOr_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::logicalOr(const char* name, const T rhs)
	{
		return logicalOr<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isOn(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return compare<T>(name_crc, GASHA_ isOn_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isOn(const char* name, const T rhs)
	{
		return isOn<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isOff(const GASHA_ crc32_t name_crc, const T rhs)
	{
		return compare<T>(name_crc, GASHA_ isOff_policy<T>, rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isOff(const char* name, const T rhs)
	{
		return isOff<T>(GASHA_ calcCRC32(name), rhs);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isTrue(const GASHA_ crc32_t name_crc)
	{
		return compare<T>(name_crc, GASHA_ isTrue_policy<T>, GASHA_ numeric_limits<T>::zero());
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isTrue(const char* name)
	{
		return isTrue<T>(GASHA_ calcCRC32(name));
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isFalse(const GASHA_ crc32_t name_crc)
	{
		return compare<T>(name_crc, GASHA_ isFalse_policy<T>, GASHA_ numeric_limits<T>::zero());
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::isFalse(const char* name)
	{
		return isFalse<T>(GASHA_ calcCRC32(name));
	}
	
	//データ参照登録
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const crc32_t name_crc, T& ref)
	{
		const refInfo* info = m_refTable->emplace(name_crc, name_crc, ref);
		return info ? true : false;
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const char* name, T& ref)
	{
		return regist(GASHA_ calcCRC32(name), ref);
	}

	//※ラップアラウンド演算用
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const GASHA_ crc32_t name_crc, T& ref, const wraparound_tag&, const T&& max, const T&& min)
	{
		static_assert(sizeof(T) <= sizeof(void*), "max/min must been lvalue reference.");
		const refInfo* info = m_refTable->emplace(name_crc, name_crc, ref, wraparound, max, min);
		return info ? true : false;
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const char* name, T& ref, const wraparound_tag&, const T&& max, const T&& min)
	{
		static_assert(sizeof(T) <= sizeof(void*), "max/min must been lvalue reference.");
		return regist(GASHA_ calcCRC32(name), ref, wraparound, max, min);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const GASHA_ crc32_t name_crc, T& ref, const wraparound_tag&, const T& max, const T& min)
	{
		const refInfo* info = m_refTable->emplace(name_crc, name_crc, ref, wraparound, max, min);
		return info ? true : false;
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const char* name, T& ref, const wraparound_tag&, const T& max, const T& min)
	{
		return regist(GASHA_ calcCRC32(name), ref, wraparound, max, min);
	}

	//※飽和演算用
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const GASHA_ crc32_t name_crc, T& ref, const saturation_tag&, const T&& max, const T&& min)
	{
		static_assert(sizeof(T) <= sizeof(void*), "max/min must been lvalue reference.");
		const refInfo* info = m_refTable->emplace(name_crc, name_crc, ref, saturation, max, min);
		return info ? true : false;
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const char* name, T& ref, const saturation_tag&, const T&& max, const T&& min)
	{
		static_assert(sizeof(T) <= sizeof(void*), "max/min must been lvalue reference.");
		return regist(GASHA_ calcCRC32(name), ref, saturation, max, min);
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const GASHA_ crc32_t name_crc, T& ref, const saturation_tag&, const T& max, const T& min)
	{
		const refInfo* info = m_refTable->emplace(name_crc, name_crc, ref, saturation, max, min);
		return info ? true : false;
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const char* name, T& ref, const saturation_tag&, const T& max, const T& min)
	{
		return regist(GASHA_ calcCRC32(name), ref, saturation, max, min);
	}

	//※読み取り専用
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const crc32_t name_crc, const T& ref)
	{
		const refInfo* info = m_refTable->emplace(name_crc, name_crc, ref);
		return info ? true : false;
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const char* name, const T& ref)
	{
		return regist(GASHA_ calcCRC32(name), ref);
	}

	//※読み取り専用
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const crc32_t name_crc, const T& ref, const readonly_tag&)
	{
		const refInfo* info = m_refTable->emplace(name_crc, name_crc, ref, readonly);
		return info ? true : false;
	}
	template<class OPE_TYPE>
	template<typename T>
	inline bool table<OPE_TYPE>::regist(const char* name, const T& ref, const readonly_tag&)
	{
		return regist(GASHA_ calcCRC32(name), ref, readonly);
	}

	//データ登録解除
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::unregist(const GASHA_ crc32_t name_crc)
	{
		return m_refTable->erase(name_crc);
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::unregist(const char* name)
	{
		return unregist(GASHA_ calcCRC32(name));
	}

	//クリア
	template<class OPE_TYPE>
	inline void table<OPE_TYPE>::clear()
	{
		m_refTable->clear();
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
	template<class OPE_TYPE>
	inline std::size_t table<OPE_TYPE>::max_size() const
	{
		return m_refTable->max_size();
	}
	template<class OPE_TYPE>
	inline std::size_t table<OPE_TYPE>::size() const
	{
		return m_refTable->size();
	}
	template<class OPE_TYPE>
	inline std::size_t table<OPE_TYPE>::remain() const
	{
		return m_refTable->remain();
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::empty() const
	{
		return m_refTable->empty();
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::full() const
	{
		return m_refTable->full();
	}

	//ハッシュテーブルアクセス：ロック取得系メソッド
	//単一ロック取得
	template<class OPE_TYPE>
	inline GASHA_ unique_shared_lock<typename table<OPE_TYPE>::lock_type> table<OPE_TYPE>::lockUnique(const GASHA_ with_lock_shared_t&) const
	{
		return m_refTable->lockUnique(GASHA_ with_lock_shared);
	}
	template<class OPE_TYPE>
	inline GASHA_ unique_shared_lock<typename table<OPE_TYPE>::lock_type> table<OPE_TYPE>::lockUnique(const GASHA_ try_to_lock_shared_t&) const
	{
		return m_refTable->lockUnique(GASHA_ try_to_lock_shared);
	}
	//スコープロック取得
	template<class OPE_TYPE>
	inline GASHA_ shared_lock_guard<typename table<OPE_TYPE>::lock_type> table<OPE_TYPE>::lockSharedScoped() const
	{
		return m_refTable->lockSharedScoped();
	}
	//ハッシュテーブルアクセス：イテレータ取得系メソッド
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::iterator table<OPE_TYPE>::cbegin() const
	{
		return m_refTable->cbegin();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::iterator table<OPE_TYPE>::cend() const
	{
		return m_refTable->cend();
	}

	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::iterator table<OPE_TYPE>::begin() const
	{
		return m_refTable->begin();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::iterator table<OPE_TYPE>::end() const
	{
		return m_refTable->end();
	}
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	//リバースイテレータを取得
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::reverse_iterator table<OPE_TYPE>::crbegin() const
	{
		return m_refTable->crbegin();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::reverse_iterator table<OPE_TYPE>::crend() const
	{
		return m_refTable->crend();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::reverse_iterator table<OPE_TYPE>::rbegin() const
	{
		return m_refTable->rbegin();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::reverse_iterator table<OPE_TYPE>::rend() const
	{
		return m_refTable->rend();
	}
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR

	//明示的な初期化用コンストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::table(const typename table<OPE_TYPE>::explicitInit_tag&)
	{
		explicitInitialize();
	}

	//デフォルトコンストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::table() :
		m_refTable(reinterpret_cast<table_type*>(GASHA_ adjustAlign<char, alignof(table_type)>(m_refTableBuff)))
	{
		//	initialize();//実行しない
	}

	//デストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::~table()
	{}
}//namespace named_ref


GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_NAMED_REF_INL

// End of file

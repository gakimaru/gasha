#pragma	 once
#ifndef GASHA_INCLUDED_NAMED_FUNC_INL
#define GASHA_INCLUDED_NAMED_FUNC_INL

//--------------------------------------------------------------------------------
// named_func.inl
// 名前付き関数【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/named_func.h>//名前付き関数【宣言部】

#include <gasha/allocator_common.h>//メモリアロケータ共通設定・処理：callConstructor()
#include <gasha/memory.h>//メモリ操作：adjustAlign()
#include <gasha/limits.h>//限界値
#include <gasha/simple_assert.h>//シンプルアサーション

#include <utility>//C++11 std::move(), std::forward()
#include <cstddef>//std::intptr_t
#include <type_traits>//C++11 std::is_const

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//名前付き関数
//--------------------------------------------------------------------------------

namespace named_func
{
	//----------------------------------------
	//名前付き関数クラス

	//--------------------
	//関数情報

	//オペレータ
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::funcInfoKey::operator<(const typename table<OPE_TYPE>::funcInfoKey& rhs) const
	{
		return m_key < rhs.m_key;
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::funcInfoKey::operator==(const typename table<OPE_TYPE>::funcInfoKey& rhs) const
	{
		return m_key == rhs.m_key;
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::funcInfoKey::operator!=(const typename table<OPE_TYPE>::funcInfoKey& rhs) const
	{
		return m_key != rhs.m_key;
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::funcInfoKey::operator==(const typename table<OPE_TYPE>::infoKey_type& rhs) const
	{
		return m_key == rhs;
	}
	//ムーブ／コピーオペレータ
	template<class OPE_TYPE>
	inline typename table<OPE_TYPE>::funcInfoKey& table<OPE_TYPE>::funcInfoKey::operator=(typename table<OPE_TYPE>::funcInfoKey&& rhs)
	{
		m_key = rhs.m_key;
		return *this;
	}
	template<class OPE_TYPE>
	inline typename table<OPE_TYPE>::funcInfoKey& table<OPE_TYPE>::funcInfoKey::operator=(const typename table<OPE_TYPE>::funcInfoKey& rhs)
	{
		m_key = rhs.m_key; return *this;
	}
	//ムーブ／コピーコンストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::funcInfoKey::funcInfoKey(typename table<OPE_TYPE>::funcInfoKey&& obj) :
		m_key(obj.m_key)
	{}
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::funcInfoKey::funcInfoKey(const typename table<OPE_TYPE>::funcInfoKey& obj) :
		m_key(obj.m_key)
	{}
	//コンストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::funcInfoKey::funcInfoKey(GASHA_ crc32_t group_name_crc, GASHA_ crc32_t name_crc) :
		m_groupNameCrc(group_name_crc),
		m_nameCrc(name_crc)
	{}

	//比較オペレータ
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::funcInfo::operator<(const typename table<OPE_TYPE>::funcInfo& rhs) const
	{
		return true;//m_key < rhs.m_key;
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::funcInfo::operator==(const typename table<OPE_TYPE>::funcInfo& rhs) const
	{
		return m_key == rhs.m_key;
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::funcInfo::operator!=(const typename table<OPE_TYPE>::funcInfo& rhs) const
	{
		return m_key != rhs.m_key;
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::funcInfo::operator==(const typename table<OPE_TYPE>::funcInfoKey& rhs) const
	{
		return m_key == rhs;
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::funcInfo::operator==(const typename table<OPE_TYPE>::infoKey_type& rhs) const
	{
		return m_key == rhs;
	}

	//コンストラクタ
	template<class OPE_TYPE>
	template<typename RET, typename... Tx>
	inline table<OPE_TYPE>::funcInfo::funcInfo(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, std::function<RET(Tx...)> function, const writableFunc_tag&) :
		m_key(group_name_crc, name_crc),
		m_retTypeInfo(&typeid(RET)),
		m_objTypeInfo(nullptr),
		m_funcType(WRITABLE_FUNCTION),
		m_func(*reinterpret_cast<std::function<void()>*>(&function)),
		m_next(nullptr)
	{}
	template<class OPE_TYPE>
	template<typename RET, typename... Tx>
	inline table<OPE_TYPE>::funcInfo::funcInfo(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, std::function<RET(Tx...)> function, const constFunc_tag&) :
		m_key(group_name_crc, name_crc),
		m_retTypeInfo(&typeid(RET)),
		m_objTypeInfo(nullptr),
		m_funcType(CONST_FUNCTION),
		m_func(*reinterpret_cast<std::function<void()>*>(&function)),
		m_next(nullptr)
	{}
	template<class OPE_TYPE>
	template<typename RET, class OBJ, typename... Tx>
	inline table<OPE_TYPE>::funcInfo::funcInfo(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, OBJ*, std::function<RET(OBJ&, Tx...)> function, const writableFunc_tag&) :
		m_key(group_name_crc, name_crc),
		m_retTypeInfo(&typeid(RET)),
		m_objTypeInfo(&typeid(OBJ)),
		m_funcType(WRITABLE_FUNCTION),
		m_func(*reinterpret_cast<std::function<void()>*>(&function)),
		m_next(nullptr)
	{}
	template<class OPE_TYPE>
	template<typename RET, class OBJ, typename... Tx>
	inline table<OPE_TYPE>::funcInfo::funcInfo(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, OBJ*, std::function<RET(OBJ&, Tx...)> function, const constFunc_tag&) :
		m_key(group_name_crc, name_crc),
		m_retTypeInfo(&typeid(RET)),
		m_objTypeInfo(&typeid(OBJ)),
		m_funcType(CONST_FUNCTION),
		m_func(*reinterpret_cast<std::function<void()>*>(&function)),
		m_next(nullptr)
	{}
	
	//デフォルトコンストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::funcInfo::funcInfo() :
		m_key(0, 0),
		m_retTypeInfo(nullptr),
		m_objTypeInfo(nullptr),
		m_funcType(CONST_FUNCTION),
		m_func(nullptr),
		m_next(nullptr)
	{}
	
	//デストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::funcInfo::~funcInfo()
	{}
	
	//--------------------
	//関数グループ情報
		
	//関数リストイテレータを取得
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::funcList_type::iterator table<OPE_TYPE>::groupInfo::cbegin() const
	{
		return m_funcList.cbegin();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::funcList_type::iterator table<OPE_TYPE>::groupInfo::cend() const
	{
		return m_funcList.cend();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::funcList_type::iterator table<OPE_TYPE>::groupInfo::begin() const
	{
		return m_funcList.begin();
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::funcList_type::iterator table<OPE_TYPE>::groupInfo::end() const
	{
		return m_funcList.end();
	}

	//関数リスト情報を取得
	template<class OPE_TYPE>
	inline std::size_t table<OPE_TYPE>::groupInfo::size() const
	{
		return m_funcList.size();
	}

	//コンストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::groupInfo::groupInfo(const GASHA_ crc32_t group_name_crc):
		m_groupNameCrc(group_name_crc),
		m_lock(),
		m_funcList()
	{}
	
	//デフォルトコンストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::groupInfo::groupInfo() :
		m_groupNameCrc(0),
		m_lock(),
		m_funcList()
	{}
	
	//デストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::groupInfo::~groupInfo()
	{}

	//--------------------
	//名前付き関数本体

	//データ参照情報取得時のアサーション
	//※関数操作用
#define GASHA_NAMED_FUNC_ASSERT_FOR_FUNCTION(RET, group_info, func_info, group_name_crc, name_crc) \
	GASHA_SIMPLE_ASSERT(func_info != nullptr, "name_crc(0x%08x:0x%08x) is not registered.", group_name_crc, name_crc); \
	GASHA_SIMPLE_ASSERT(func_info->m_retTypeInfo, "name_crc(0x%08x:0x%08x)' has not return-types.", group_name_crc, name_crc); \
	if (func_info->m_retTypeInfo) \
	{ \
		GASHA_SIMPLE_ASSERT(*func_info->m_retTypeInfo == typeid(RET), "name_crc(0x%08x:0x%08x)'s type and RET are different return-types.(%s and %s)", group_name_crc, name_crc, func_info->m_retTypeInfo->name(), typeid(RET).name()); \
	} \
	GASHA_SIMPLE_ASSERT(!func_info->m_objTypeInfo, "name_crc(0x%08x:0x%08x)' is not an fucntion(is object-member-function).", group_name_crc, name_crc); \
	GASHA_SIMPLE_ASSERT(func_info->m_func, "name_crc(0x%08x:0x%08x) has not function.", group_name_crc, name_crc);

	//※オブジェクト操作用
#define GASHA_NAMED_FUNC_ASSERT_FOR_OBJ(RET, OBJ, group_info, func_info, group_name_crc, name_crc) \
	GASHA_SIMPLE_ASSERT(func_info != nullptr, "name_crc(0x%08x:0x%08x) is not registered.", group_name_crc, name_crc); \
	GASHA_SIMPLE_ASSERT(func_info->m_retTypeInfo, "name_crc(0x%08x:0x%08x)' has not return-types.", group_name_crc, name_crc); \
	if (func_info->m_retTypeInfo) \
	{ \
		GASHA_SIMPLE_ASSERT(*func_info->m_retTypeInfo == typeid(RET), "name_crc(0x%08x:0x%08x)'s type and RET are different return-types.(%s and %s)", group_name_crc, name_crc, func_info->m_retTypeInfo->name(), typeid(RET).name()); \
	} \
	GASHA_SIMPLE_ASSERT(func_info->m_objTypeInfo, "name_crc(0x%08x:0x%08x)' is not an object-member-fucntion(is function).", group_name_crc, name_crc); \
	if (func_info->m_objTypeInfo) \
	{ \
		GASHA_SIMPLE_ASSERT(*func_info->m_objTypeInfo == typeid(OBJ), "name_crc(0x%08x:0x%08x)'s type and OBJ are different object-types.(%s and %s)", group_name_crc, name_crc, func_info->m_objTypeInfo->name(), typeid(OBJ).name()); \
	} \
	GASHA_SIMPLE_ASSERT(func_info->m_func, "name_crc(0x%08x:0x%08x) has not function.", group_name_crc, name_crc); \
	GASHA_SIMPLE_ASSERT(!std::is_const<OBJ>::value || (std::is_const<OBJ>::value && func_info->m_funcType == CONST_FUNCTION), "name_crc(0x%08x:0x%08x)' is a writable fucntion. That can not use constant-object.", group_name_crc, name_crc);

	//登録済みチェック
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::isRegistered(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc) const
	{
		funcInfoKey key(group_name_crc, name_crc);
		return m_funcTable->at(key.m_key) != nullptr;
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::isRegistered(const char* group_name, const char* name) const
	{
		return isRegistered(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name));
	}

	//const関数チェック
	template<class OPE_TYPE>
	template<typename RET>
	inline bool table<OPE_TYPE>::isConstType(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc) const
	{
		funcInfoKey key(group_name_crc, name_crc);
		const funcInfo* info = m_funcTable->at(key.m_key);
		return info && info->m_func && info->m_funcType == CONST_FUNCTION && info->m_retTypeInfo && *info->m_retTypeInfo == typeid(RET) && !info->m_objTypeInfo;
	}
	template<class OPE_TYPE>
	template<typename RET>
	inline bool table<OPE_TYPE>::isConstType(const char* group_name, const char* name) const
	{
		return isConstType<RET>(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name));
	}
	template<class OPE_TYPE>
	template<typename RET, class OBJ>
	inline bool table<OPE_TYPE>::isConstObj(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc) const
	{
		funcInfoKey key(group_name_crc, name_crc);
		const funcInfo* info = m_funcTable->at(key.m_key);
		return info && info->m_func && info->m_funcType == CONST_FUNCTION && info->m_retTypeInfo && *info->m_retTypeInfo == typeid(RET) && *info->m_objTypeInfo == typeid(OBJ);
	}
	template<class OPE_TYPE>
	template<typename RET, class OBJ>
	inline bool table<OPE_TYPE>::isConstObj(const char* group_name, const char* name) const
	{
		return isConstObj<RET, OBJ>(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name));
	}

	//書き込み可能チェック
	template<class OPE_TYPE>
	template<typename RET>
	inline bool table<OPE_TYPE>::isWritable(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc) const
	{
		funcInfoKey key(group_name_crc, name_crc);
		const funcInfo* info = m_funcTable->at(key.m_key);
		return info && info->m_func && info->m_funcType != CONST_FUNCTION && info->m_retTypeInfo && *info->m_retTypeInfo == typeid(RET) && !info->m_objTypeInfo;
	}
	template<class OPE_TYPE>
	template<typename RET>
	inline bool table<OPE_TYPE>::isWritable(const char* group_name, const char* name) const
	{
		return isWritable<RET>(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name));
	}
	template<class OPE_TYPE>
	template<typename RET, class OBJ>
	inline bool table<OPE_TYPE>::isWritableObj(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc) const
	{
		funcInfoKey key(group_name_crc, name_crc);
		const funcInfo* info = m_funcTable->at(key.m_key);
		return info && info->m_func && info->m_funcType != CONST_FUNCTION && info->m_retTypeInfo && *info->m_retTypeInfo == typeid(RET) && *info->m_objTypeInfo == typeid(OBJ);
	}
	template<class OPE_TYPE>
	template<typename RET, class OBJ>
	inline bool table<OPE_TYPE>::isWritableObj(const char* group_name, const char* name) const
	{
		return isWritableObj<RET, OBJ>(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name));
	}

	//関数呼び出し（共通処理）
	//※戻り値あり
	//※const関数
	template<class OPE_TYPE>
	template<typename RET, class LOCK_TYPE, typename... Tx>
	inline RET table<OPE_TYPE>::_func(const std::function<void()>& func, const constFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args) const
	{
		auto lock = lock_obj.lockSharedScoped();
		return (*reinterpret_cast<const std::function<RET(Tx...)>*>(&func))(std::forward<Tx>(args)...);
	}
	//※書き込み可能関数
	template<class OPE_TYPE>
	template<typename RET, class LOCK_TYPE, typename... Tx>
	inline RET table<OPE_TYPE>::_func(const std::function<void()>& func, const writableFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args)
	{
		auto lock = lock_obj.lockScoped();
		return (*reinterpret_cast<const std::function<RET(Tx...)>*>(&func))(std::forward<Tx>(args)...);
	}
	//※戻り値なし
	//※const関数
	template<class OPE_TYPE>
	template<class LOCK_TYPE, typename... Tx>
	inline void table<OPE_TYPE>::_proc(const std::function<void()>& func, const constFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args) const
	{
		auto lock = lock_obj.lockSharedScoped();
		(*reinterpret_cast<const std::function<void(Tx...)>*>(&func))(std::forward<Tx>(args)...);
	}
	//※書き込み可能関数
	template<class OPE_TYPE>
	template<class LOCK_TYPE, typename... Tx>
	inline void table<OPE_TYPE>::_proc(const std::function<void()>& func, const writableFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args)
	{
		auto lock = lock_obj.lockScoped();
		(*reinterpret_cast<const std::function<void(Tx...)>*>(&func))(std::forward<Tx>(args)...);
	}
	//オブジェクトメンバー関数呼び出し（共通処理）
	//※戻り値あり
	//※const関数
	template<class OPE_TYPE>
	template<typename RET, class OBJ, class LOCK_TYPE, typename... Tx>
	inline RET table<OPE_TYPE>::_objFunc(const OBJ& obj, const std::function<void()>& func, const constFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args) const
	{
		auto lock = lock_obj.lockSharedScoped();
		return (*reinterpret_cast<const std::function<RET(const OBJ&, Tx...)>*>(&func))(obj, std::forward<Tx>(args)...);
	}
	//※書き込み可能関数
	template<class OPE_TYPE>
	template<typename RET, class OBJ, class LOCK_TYPE, typename... Tx>
	inline RET table<OPE_TYPE>::_objFunc(OBJ& obj, const std::function<void()>& func, const writableFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args)
	{
		auto lock = lock_obj.lockScoped();
		return (*reinterpret_cast<const std::function<RET(const OBJ&, Tx...)>*>(&func))(obj, std::forward<Tx>(args)...);
	}
	//※戻り値なし
	//※const関数
	template<class OPE_TYPE>
	template<class OBJ, class LOCK_TYPE, typename... Tx>
	inline void table<OPE_TYPE>::_objProc(const OBJ& obj, const std::function<void()>& func, const constFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args) const
	{
		auto lock = lock_obj.lockSharedScoped();
		return (*reinterpret_cast<const std::function<void(const OBJ&, Tx...)>*>(&func))(obj, std::forward<Tx>(args)...);
	}
	//※書き込み可能関数
	template<class OPE_TYPE>
	template<class OBJ, class LOCK_TYPE, typename... Tx>
	inline void table<OPE_TYPE>::_objProc(OBJ& obj, const std::function<void()>& func, const writableFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args)
	{
		auto lock = lock_obj.lockScoped();
		return (*reinterpret_cast<const std::function<void(const OBJ&, Tx...)>*>(&func))(obj, std::forward<Tx>(args)...);
	}

	//関数呼び出し
	//※戻り値あり
	//※const関数
	template<class OPE_TYPE>
	template<typename RET, typename... Tx>
	RET table<OPE_TYPE>::func(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args) const
	{
		const groupInfo* group_info = findGroup(group_name_crc);
		const funcInfo* func_info = findFunc(group_name_crc, name_crc);
		GASHA_NAMED_FUNC_ASSERT_FOR_FUNCTION(RET, group_info, func_info, group_name_crc, name_crc);
		if (group_info)
			return _func<RET>(func_info->m_func, constFunc, group_info->m_lock, std::forward<Tx>(args)...);
		else
			return _func<RET>(func_info->m_func, constFunc, GASHA_ dummySharedLock(), std::forward<Tx>(args)...);
	}
	template<class OPE_TYPE>
	template<typename RET, typename... Tx>
	inline RET table<OPE_TYPE>::func(const char* group_name, const char* name, Tx&&... args) const
	{
		return func<RET>(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), std::forward<Tx>(args)...);
	}
	//※書き込み可能関数
	template<class OPE_TYPE>
	template<typename RET, typename... Tx>
	RET table<OPE_TYPE>::func(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args)
	{
		const groupInfo* group_info = findGroup(group_name_crc);
		const funcInfo* func_info = findFunc(group_name_crc, name_crc);
		GASHA_NAMED_FUNC_ASSERT_FOR_FUNCTION(RET, group_info, func_info, group_name_crc, name_crc);
		if (group_info)
		{
			if (func_info->m_funcType == CONST_FUNCTION)
				return _func<RET>(func_info->m_func, constFunc, group_info->m_lock, std::forward<Tx>(args)...);
			else
				return _func<RET>(func_info->m_func, writableFunc, group_info->m_lock, std::forward<Tx>(args)...);
		}
		else
		{
			if (func_info->m_funcType == CONST_FUNCTION)
				return _func<RET>(func_info->m_func, constFunc, GASHA_ dummySharedLock(), std::forward<Tx>(args)...);
			else
				return _func<RET>(func_info->m_func, writableFunc, GASHA_ dummySharedLock(), std::forward<Tx>(args)...);
		}
	}
	template<class OPE_TYPE>
	template<typename RET, typename... Tx>
	inline RET table<OPE_TYPE>::func(const char* group_name, const char* name, Tx&&... args)
	{
		return func<RET>(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), std::forward<Tx>(args)...);
	}
	//※戻り値なし
	//※const関数
	template<class OPE_TYPE>
	template<typename... Tx>
	void table<OPE_TYPE>::proc(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args) const
	{
		const groupInfo* group_info = findGroup(group_name_crc);
		const funcInfo* func_info = findFunc(group_name_crc, name_crc);
		GASHA_NAMED_FUNC_ASSERT_FOR_FUNCTION(void, group_info, func_info, group_name_crc, name_crc);
		if (group_info)
			_proc(func_info->m_func, constFunc, group_info->m_lock, std::forward<Tx>(args)...);
		else
			_proc(func_info->m_func, constFunc, GASHA_ dummySharedLock(), std::forward<Tx>(args)...);
	}
	template<class OPE_TYPE>
	template<typename... Tx>
	inline void table<OPE_TYPE>::proc(const char* group_name, const char* name, Tx&&... args) const
	{
		proc(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), std::forward<Tx>(args)...);
	}
	//※書き込み可能関数
	template<class OPE_TYPE>
	template<typename... Tx>
	void table<OPE_TYPE>::proc(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args)
	{
		const groupInfo* group_info = findGroup(group_name_crc);
		const funcInfo* func_info = findFunc(group_name_crc, name_crc);
		GASHA_NAMED_FUNC_ASSERT_FOR_FUNCTION(void, group_info, func_info, group_name_crc, name_crc);
		if (group_info)
		{
			if (func_info->m_funcType == CONST_FUNCTION)
				_proc(func_info->m_func, constFunc, group_info->m_lock, std::forward<Tx>(args)...);
			else
				_proc(func_info->m_func, writableFunc, group_info->m_lock, std::forward<Tx>(args)...);
		}
		else
		{
			if (func_info->m_funcType == CONST_FUNCTION)
				_proc(func_info->m_func, constFunc, GASHA_ dummySharedLock(), std::forward<Tx>(args)...);
			else
				_proc(func_info->m_func, writableFunc, GASHA_ dummySharedLock(), std::forward<Tx>(args)...);
		}
	}
	template<class OPE_TYPE>
	template<typename... Tx>
	inline void table<OPE_TYPE>::proc(const char* group_name, const char* name, Tx&&... args)
	{
		proc(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), std::forward<Tx>(args)...);
	}
	//オブジェクトメンバー関数呼び出し
	//※戻り値あり
	//※const関数
	template<class OPE_TYPE>
	template<typename RET, class OBJ, typename... Tx>
	RET table<OPE_TYPE>::func(const OBJ& obj, const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args) const
	{
		const groupInfo* group_info = findGroup(group_name_crc);
		const funcInfo* func_info = findFunc(group_name_crc, name_crc);
		GASHA_NAMED_FUNC_ASSERT_FOR_OBJ(RET, OBJ, group_info, func_info, group_name_crc, name_crc);
		if (group_info)
			return _objFunc<RET>(obj, func_info->m_func, constFunc, group_info->m_lock, std::forward<Tx>(args)...);
		else
			return _objFunc<RET>(obj, func_info->m_func, constFunc, GASHA_ dummySharedLock(), std::forward<Tx>(args)...);
	}
	template<class OPE_TYPE>
	template<typename RET, class OBJ, typename... Tx>
	inline RET table<OPE_TYPE>::func(const OBJ& obj, const char* group_name, const char* name, Tx&&... args) const
	{
		return func<RET>(obj, GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), std::forward<Tx>(args)...);
	}
	//※書き込み可能関数
	template<class OPE_TYPE>
	template<typename RET, class OBJ, typename... Tx>
	RET table<OPE_TYPE>::func(OBJ& obj, const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args)
	{
		const groupInfo* group_info = findGroup(group_name_crc);
		const funcInfo* func_info = findFunc(group_name_crc, name_crc);
		GASHA_NAMED_FUNC_ASSERT_FOR_OBJ(RET, OBJ, group_info, func_info, group_name_crc, name_crc);
		if (group_info)
		{
			if (func_info->m_funcType == CONST_FUNCTION)
				return _objFunc<RET>(obj, func_info->m_func, constFunc, group_info->m_lock, std::forward<Tx>(args)...);
			else
				return _objFunc<RET>(obj, func_info->m_func, writableFunc, group_info->m_lock, std::forward<Tx>(args)...);
		}
		else
		{
			if (func_info->m_funcType == CONST_FUNCTION)
				return _objFunc<RET>(obj, func_info->m_func, constFunc, GASHA_ dummySharedLock(), std::forward<Tx>(args)...);
			else
				return _objFunc<RET>(obj, func_info->m_func, writableFunc, GASHA_ dummySharedLock(), std::forward<Tx>(args)...);
		}
	}
	template<class OPE_TYPE>
	template<typename RET, class OBJ, typename... Tx>
	inline RET table<OPE_TYPE>::func(OBJ& obj, const char* group_name, const char* name, Tx&&... args)
	{
		return func<RET>(obj, GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), std::forward<Tx>(args)...);
	}
	//※戻り値なし
	//※const関数
	template<class OPE_TYPE>
	template<class OBJ, typename... Tx>
	void table<OPE_TYPE>::proc(const OBJ& obj, const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args) const
	{
		const groupInfo* group_info = findGroup(group_name_crc);
		const funcInfo* func_info = findFunc(group_name_crc, name_crc);
		GASHA_NAMED_FUNC_ASSERT_FOR_OBJ(void, OBJ, group_info, func_info, group_name_crc, name_crc);
		if (group_info)
			_objProc(obj, func_info->m_func, constFunc, group_info->m_lock, std::forward<Tx>(args)...);
		else
			_objProc(obj, func_info->m_func, constFunc, GASHA_ dummySharedLock(), std::forward<Tx>(args)...);
	}
	template<class OPE_TYPE>
	template<class OBJ, typename... Tx>
	inline void table<OPE_TYPE>::proc(const OBJ& obj, const char* group_name, const char* name, Tx&&... args) const
	{
		proc(obj, GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), std::forward<Tx>(args)...);
	}
	//※書き込み可能関数
	template<class OPE_TYPE>
	template<class OBJ, typename... Tx>
	void table<OPE_TYPE>::proc(OBJ& obj, const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args)
	{
		const groupInfo* group_info = findGroup(group_name_crc);
		const funcInfo* func_info = findFunc(group_name_crc, name_crc);
		GASHA_NAMED_FUNC_ASSERT_FOR_OBJ(void, OBJ, group_info, func_info, group_name_crc, name_crc);
		if (group_info)
		{
			if (func_info->m_funcType == CONST_FUNCTION)
				_objProc(obj, func_info->m_func, constFunc, group_info->m_lock, std::forward<Tx>(args)...);
			else
				_objProc(obj, func_info->m_func, writableFunc, group_info->m_lock, std::forward<Tx>(args)...);
		}
		else
		{
			if (func_info->m_funcType == CONST_FUNCTION)
				_objProc(obj, func_info->m_func, constFunc, GASHA_ dummySharedLock(), std::forward<Tx>(args)...);
			else
				_objProc(obj, func_info->m_func, writableFunc, GASHA_ dummySharedLock(), std::forward<Tx>(args)...);
		}
	}
	template<class OPE_TYPE>
	template<class OBJ, typename... Tx>
	inline void table<OPE_TYPE>::proc(OBJ& obj, const char* group_name, const char* name, Tx&&... args)
	{
		proc(obj, GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), std::forward<Tx>(args)...);
	}

	//関数登録
	template<class OPE_TYPE>
	template<class FUNC_SPEC, class FUNCTION>
	bool table<OPE_TYPE>::regist(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, FUNCTION function)
	{
		groupInfo* group_info = nullptr;
		if (group_name_crc != 0)
		{
			group_info = registAndGetGroup(group_name_crc);
			if (!group_info)
				return false;
		}
		{
			auto lock = m_groupTable->lockScoped();
			funcInfoKey key(group_name_crc, name_crc);
			funcSpec<FUNC_SPEC> spec(function);
			funcInfo* func_info = m_funcTable->emplace(key.m_key, group_name_crc, name_crc, spec.m_func, writableFunc);
			if (group_info && func_info)
				group_info->m_funcList.push_back(*func_info);
			return func_info ? true : false;
		}
	}
	template<class OPE_TYPE>
	template<class FUNC_SPEC, class FUNCTION>
	inline bool table<OPE_TYPE>::regist(const char* group_name, const char* name, FUNCTION function)
	{
		return regist<FUNC_SPEC>(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), function);
	}
	//※const関数
	template<class OPE_TYPE>
	template<class FUNC_SPEC, class FUNCTION>
	bool table<OPE_TYPE>::regist(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, FUNCTION function, const constFunc_tag&)
	{
		groupInfo* group_info = nullptr;
		if (group_name_crc != 0)
		{
			group_info = registAndGetGroup(group_name_crc);
			if (!group_info)
				return false;
		}
		{
			auto lock = m_groupTable->lockScoped();
			funcInfoKey key(group_name_crc, name_crc);
			funcSpec<FUNC_SPEC> spec(function);
			funcInfo* func_info = m_funcTable->emplace(key.m_key, group_name_crc, name_crc, spec.m_func, constFunc);
			if (group_info && func_info)
				group_info->m_funcList.push_back(*func_info);
			return func_info ? true : false;
		}
	}
	template<class OPE_TYPE>
	template<class FUNC_SPEC, class FUNCTION>
	inline bool table<OPE_TYPE>::regist(const char* group_name, const char* name, FUNCTION function, const constFunc_tag&)
	{
		return regist<FUNC_SPEC>(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), function, constFunc);
	}
	//オブジェクトメンバー関数登録
	template<class OPE_TYPE>
	template<class FUNC_SPEC, class FUNCTION>
	bool table<OPE_TYPE>::registObj(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, FUNCTION function)
	{
		groupInfo* group_info = nullptr;
		if (group_name_crc != 0)
		{
			group_info = registAndGetGroup(group_name_crc);
			if (!group_info)
				return false;
		}
		{
			auto lock = m_groupTable->lockScoped();
			funcInfoKey key(group_name_crc, name_crc);
			memberFuncSpec<FUNC_SPEC> spec(function);
			typename memberFuncSpec<FUNC_SPEC>::object_type* obj_p = nullptr;
			funcInfo* func_info = m_funcTable->emplace(key.m_key, group_name_crc, name_crc, obj_p, spec.m_func, writableFunc);
			if (group_info && func_info)
				group_info->m_funcList.push_back(*func_info);
			return func_info ? true : false;
		}
	}
	template<class OPE_TYPE>
	template<class FUNC_SPEC, class FUNCTION>
	inline bool table<OPE_TYPE>::registObj(const char* group_name, const char* name, FUNCTION function)
	{
		return registObj<FUNC_SPEC>(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), function);
	}
	//※const関数
	template<class OPE_TYPE>
	template<class FUNC_SPEC, class FUNCTION>
	bool table<OPE_TYPE>::registObj(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, FUNCTION function, const constFunc_tag&)
	{
		groupInfo* group_info = nullptr;
		if (group_name_crc != 0)
		{
			group_info = registAndGetGroup(group_name_crc);
			if (!group_info)
				return false;
		}
		{
			auto lock = m_groupTable->lockScoped();
			funcInfoKey key(group_name_crc, name_crc);
			memberFuncSpec<FUNC_SPEC> spec(function);
			typename memberFuncSpec<FUNC_SPEC>::object_type* obj_p = nullptr;
			funcInfo* func_info = m_funcTable->emplace(key.m_key, name_crc, group_name_crc, obj_p, spec.m_func, constFunc);
			if (group_info && func_info)
				group_info->m_funcList.push_back(*func_info);
			return func_info ? true : false;
		}
	}
	template<class OPE_TYPE>
	template<class FUNC_SPEC, class FUNCTION>
	inline bool table<OPE_TYPE>::registObj(const char* group_name, const char* name, FUNCTION function, const constFunc_tag&)
	{
		return registObj<FUNC_SPEC>(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name), function, constFunc);
	}
	//関数登録解除
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::unregist(const char* group_name, const char* name)
	{
		return unregist(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name));
	}

	//クリア
	template<class OPE_TYPE>
	inline void table<OPE_TYPE>::clear()
	{
		m_groupTable->clear();
		m_funcTable->clear();
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
	//※関数情報
	template<class OPE_TYPE>
	inline std::size_t table<OPE_TYPE>::max_size() const
	{
		return m_funcTable->max_size();
	}
	template<class OPE_TYPE>
	inline std::size_t table<OPE_TYPE>::size() const
	{
		return m_funcTable->size();
	}
	template<class OPE_TYPE>
	inline std::size_t table<OPE_TYPE>::remain() const
	{
		return m_funcTable->remain();
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::empty() const
	{
		return m_funcTable->empty();
	}
	template<class OPE_TYPE>
	inline bool table<OPE_TYPE>::full() const
	{
		return m_funcTable->full();
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
	inline const typename table<OPE_TYPE>::funcInfo* table<OPE_TYPE>::findFunc(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc) const
	{
		auto lock = m_groupTable->lockSharedScoped();
		funcInfoKey key(group_name_crc, name_crc);
		return m_funcTable->at(key.m_key);
	}
	template<class OPE_TYPE>
	inline const typename table<OPE_TYPE>::funcInfo* table<OPE_TYPE>::findFunc(const char* group_name, const char* name) const
	{
		return findFunc(GASHA_ calcCRC32(group_name), GASHA_ calcCRC32(name));
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
		m_funcTable(reinterpret_cast<funcTable_type*>(GASHA_ adjustAlign<char, alignof(funcTable_type)>(m_funcTableBuff)))
	{
	//	initialize();//実行しない
	}

	//デストラクタ
	template<class OPE_TYPE>
	inline table<OPE_TYPE>::~table()
	{}
}//namespace named_func


GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_NAMED_FUNC_INL

// End of file

#pragma once
#ifndef GASHA_INCLUDED_SINGLETON_INL
#define GASHA_INCLUDED_SINGLETON_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ】
// singleton.inl
// シングルトン【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/singleton.h>//シングルトン【宣言部】

//#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し
#include <gasha/memory.h>//メモリ操作：adjustAlign()

#include <utility>//C++11 std::forward()

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <new>//配置new/配置delete用
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シングルトン
//--------------------------------------------------------------------------------

//----------------------------------------
//固定バッファシングルトン
namespace _private
{
	//インスタンス生成用クラス：インスタンス生成
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	template<typename... Tx>
	inline bool singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::createInstance_t::create(const char* procedure_name, Tx&&... args)
	{
		return m_singleton.create(procedure_name, std::forward<Tx>(args)...);
	}
	//インスタンス生成用クラス：コンストラクタ
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	inline singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::createInstance_t::createInstance_t(singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>& obj) :
		m_singleton(obj)
	{}

	//インスタンス生成用ダミークラス：インスタンス生成
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	template<typename... Tx>
	inline bool singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::dummyCreateInstance_t::create(const char* procedure_name, Tx&&... args)
	{
		return false;
	}
	//インスタンス生成用ダミークラス：コンストラクタ
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	inline singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::dummyCreateInstance_t::dummyCreateInstance_t(singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>& obj)
	{}

	//シングルトンインスタンスの明示的な生成
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	template<typename... Tx>
	inline bool singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::create(const char* procedure_name, Tx&&... args)
	{
		//ロック取得またはアップグレード
		const bool is_any_locked = m_lock.owns_any_lock();
		const bool is_shared_locked = m_lock.owns_shared_lock();
		if (!is_any_locked)
			m_lock.lock();
		else if (is_shared_locked)
			m_lock.upgrade();
		//インスタンス生成
		if (!m_staticInstanceIsCreated.exchange(true))
		{
			char* buff_p = GASHA_ adjustAlign<char, alignof(class_type)>(m_staticInstanceBuff);
			//m_staticInstanceRef = GASHA_ callConstructor<class_type>(buff_p, std::forward<Tx>(args)...);//コンストラクタを private 化していることがあるので、共通関数を使用しない
			m_staticInstanceRef = new(buff_p)class_type(std::forward<Tx>(args)...);
			if (m_staticInstanceRef)
			{
				//シングルトンデバッグ用処理呼び出し
				m_staticDebug.create(procedure_name);
			}
		}
		//ロック解放またはダウングレード
		if (!is_any_locked)
			m_lock.unlock();
		else if (is_shared_locked)
			m_lock.downgrade();
		return m_staticInstanceRef != nullptr;
	}
	
	//シングルトンインスタンスの明示的な破棄
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	inline bool singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::destroy(const char* procedure_name)
	{
		//ロック取得またはアップグレード
		const bool is_any_locked = m_lock.owns_any_lock();
		const bool is_shared_locked = m_lock.owns_shared_lock();
		if (!is_any_locked)
			m_lock.lock();
		else if (is_shared_locked)
			m_lock.upgrade();
		//インスタンス破棄
		if (m_staticInstanceIsCreated.exchange(false))
		{
			//GASHA_ callDestructor(m_staticInstanceRef);//デストラクタを private 化していることがあるので、共通関数を使用しない
			m_staticInstanceRef->~class_type();
			m_staticInstanceRef = nullptr;
			//シングルトンデバッグ用処理呼び出し
			m_staticDebug.destroy(procedure_name);
		}
		//ロック解放またはダウングレード
		if (!is_any_locked)
			m_lock.unlock();
		else if (is_shared_locked)
			m_lock.downgrade();
		return true;
	}

	//デバッグ情報作成
	//※十分なサイズのバッファを渡す必要あり。
	//※使用したバッファのサイズを返す。
	//※作成中、ロックを取得する。
	//インスタンス生成用クラス：インスタンス生成
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	inline std::size_t singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::debugInfo(char* message, const std::size_t max_size) const
	{
		return m_staticDebug.debugInfo(message, max_size);
	}

	//デフォルトコンストラクタでインスタンスを生成
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	inline bool singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::createDefault(const char* procedure_name)
	{
		createDefaultInstance_t create_instance(*this);
		return create_instance.create(procedure_name);
	}

	//コンストラクタ：デフォルト：排他ロック
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	inline singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::singleton(const char* procedure_name) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ with_lock),
		m_debugId(m_staticDebug.invalidId())
	{
		createDefault(procedure_name);
		m_instanceRef = m_staticInstanceRef;
		if (m_instanceRef)
		{
			//シングルトンデバッグ用処理呼び出し
			m_debugId = m_staticDebug.enter(procedure_name);
		}
	}
	//コンストラクタ：排他ロック
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	inline singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::singleton(const char* procedure_name, const GASHA_ with_lock_t&) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ with_lock),
		m_debugId(m_staticDebug.invalidId())
	{
		createDefault(procedure_name);
		m_instanceRef = m_staticInstanceRef;
		if (m_instanceRef)
		{
			//シングルトンデバッグ用処理呼び出し
			m_debugId = m_staticDebug.enter(procedure_name);
		}
	}
	//コンストラクタ：共有ロック
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	inline singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::singleton(const char* procedure_name, const GASHA_ with_lock_shared_t&) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ with_lock_shared),
		m_debugId(m_staticDebug.invalidId())
	{
		createDefault(procedure_name);
		m_instanceRef = m_staticInstanceRef;
		if (m_instanceRef)
		{
			//シングルトンデバッグ用処理呼び出し
			m_debugId = m_staticDebug.enter(procedure_name);
		}
	}
	//コンストラクタ：排他ロックを試行
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	inline singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::singleton(const char* procedure_name, const GASHA_ try_to_lock_t&) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ try_to_lock),
		m_debugId(m_staticDebug.invalidId())
	{
		createDefault(procedure_name);
		if (m_lock.owns_lock())
		{
			m_instanceRef = m_staticInstanceRef;
			if (m_instanceRef)
			{
				//シングルトンデバッグ用処理呼び出し
				m_debugId = m_staticDebug.enter(procedure_name);
			}
		}
	}
	//コンストラクタ：共有ロックを試行
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	inline singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::singleton(const char* procedure_name, const GASHA_ try_to_lock_shared_t&) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ try_to_lock_shared),
		m_debugId(m_staticDebug.invalidId())
	{
		createDefault(procedure_name);
		if (m_lock.owns_shared_lock())
		{
			m_instanceRef = m_staticInstanceRef;
			if (m_instanceRef)
			{
				//シングルトンデバッグ用処理呼び出し
				m_debugId = m_staticDebug.enter(procedure_name);
			}
		}
	}
	//コンストラクタ：排他ロック状態を引き継ぐ
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	inline singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::singleton(const char* procedure_name, const GASHA_ adopt_lock_t&) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ adopt_lock),
		m_debugId(m_staticDebug.invalidId())
	{
		createDefault(procedure_name);
		m_instanceRef = m_staticInstanceRef;
		if (m_instanceRef)
		{
			//シングルトンデバッグ用処理呼び出し
			m_debugId = m_staticDebug.enter(procedure_name);
		}
	}
	//コンストラクタ：共有ロック状態を引き継ぐ
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	inline singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::singleton(const char* procedure_name, const GASHA_ adopt_shared_lock_t&) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ adopt_shared_lock),
		m_debugId(m_staticDebug.invalidId())
	{
		createDefault(procedure_name);
		m_instanceRef = m_staticInstanceRef;
		if (m_instanceRef)
		{
			//シングルトンデバッグ用処理呼び出し
			m_debugId = m_staticDebug.enter(procedure_name);
		}
	}
	//コンストラクタ：ロックなし
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	inline singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::singleton(const char* procedure_name, const GASHA_ defer_lock_t&) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ defer_lock),
		m_debugId(m_staticDebug.invalidId())
	{
		createDefault(procedure_name);
		m_instanceRef = m_staticInstanceRef;
		if (m_instanceRef)
		{
			//シングルトンデバッグ用処理呼び出し
			m_debugId = m_staticDebug.enter(procedure_name);
		}
	}
	
	//デストラクタ
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	inline singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::~singleton()
	{
		//シングルトンデバッグ用処理呼び出し
		m_staticDebug.leave(m_debugId);
	}

}//namepace _private

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SINGLETON_INL

// End of file

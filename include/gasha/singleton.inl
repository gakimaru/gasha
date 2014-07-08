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
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/singleton.h>//シングルトン【宣言部】

#include <utility>//C++11 std::forward()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シングルトン
//--------------------------------------------------------------------------------

//----------------------------------------
//固定バッファシングルトン
namespace _private
{
	//インスタンス生成用クラス：インスタンス生成
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	template<typename... Tx>
	inline bool singleton<T, LOCK_TYPE, DEBUG_TYPE>::createInstance_t::create(const char* procedure_name, Tx&&... args)
	{
		return m_singleton.create(procedure_name, std::forward<Tx>(args)...);
	}
	//インスタンス生成用クラス：コンストラクタ
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	inline singleton<T, LOCK_TYPE, DEBUG_TYPE>::createInstance_t::createInstance_t(singleton<T, LOCK_TYPE, DEBUG_TYPE>& obj) :
		m_singleton(obj)
	{}

	//インスタンス生成用ダミークラス：インスタンス生成
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	template<typename... Tx>
	inline bool singleton<T, LOCK_TYPE, DEBUG_TYPE>::dummyCreateInstance_t::create(const char* procedure_name, Tx&&... args)
	{
		return false;
	}
	//インスタンス生成用ダミークラス：コンストラクタ
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	inline singleton<T, LOCK_TYPE, DEBUG_TYPE>::dummyCreateInstance_t::dummyCreateInstance_t(singleton<T, LOCK_TYPE, DEBUG_TYPE>& obj)
	{}

	//シングルトンインスタンスの明示的な生成
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	template<typename... Tx>
	inline bool singleton<T, LOCK_TYPE, DEBUG_TYPE>::create(const char* procedure_name, Tx&&... args)
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
			m_staticInstanceRef = new(m_staticInstanceBuff)class_type(std::forward<Tx>(args)...);
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
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	inline bool singleton<T, LOCK_TYPE, DEBUG_TYPE>::destroy(const char* procedure_name)
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
			m_staticInstanceRef->~class_type();
			//operator delete(m_staticInstanceRef, m_staticInstanceRef);
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
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	inline std::size_t singleton<T, LOCK_TYPE, DEBUG_TYPE>::debugInfo(char* message)
	{
		return m_staticDebug.debugInfo(message);
	}

	//デフォルトコンストラクタでインスタンスを生成
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	inline bool singleton<T, LOCK_TYPE, DEBUG_TYPE>::createDefault(const char* procedure_name)
	{
		createDefaultInstance_t create_instance(*this);
		return create_instance.create(procedure_name);
	}

	//コンストラクタ：デフォルト：排他ロック
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	inline singleton<T, LOCK_TYPE, DEBUG_TYPE>::singleton(const char* procedure_name) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ with_lock)
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
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	inline singleton<T, LOCK_TYPE, DEBUG_TYPE>::singleton(const char* procedure_name, const GASHA_ with_lock_t) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ with_lock)
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
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	inline singleton<T, LOCK_TYPE, DEBUG_TYPE>::singleton(const char* procedure_name, const GASHA_ with_lock_shared_t) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ with_lock_shared)
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
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	inline singleton<T, LOCK_TYPE, DEBUG_TYPE>::singleton(const char* procedure_name, const GASHA_ try_to_lock_t) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ try_to_lock)
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
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	inline singleton<T, LOCK_TYPE, DEBUG_TYPE>::singleton(const char* procedure_name, const GASHA_ try_to_lock_shared_t) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ try_to_lock_shared)
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
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	inline singleton<T, LOCK_TYPE, DEBUG_TYPE>::singleton(const char* procedure_name, const GASHA_ adopt_lock_t) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ adopt_lock)
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
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	inline singleton<T, LOCK_TYPE, DEBUG_TYPE>::singleton(const char* procedure_name, const GASHA_ adopt_shared_lock_t) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ adopt_shared_lock)
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
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	inline singleton<T, LOCK_TYPE, DEBUG_TYPE>::singleton(const char* procedure_name, const GASHA_ defer_lock_t) :
		m_instanceRef(nullptr),
		m_lock(m_staticLock, GASHA_ defer_lock)
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
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	inline singleton<T, LOCK_TYPE, DEBUG_TYPE>::~singleton()
	{
		//シングルトンデバッグ用処理呼び出し
		m_staticDebug.leave(m_debugId);
	}

}//namepace _private

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_SINGLETON_INL

// End of file

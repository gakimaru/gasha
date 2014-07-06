#pragma once
#ifndef GASHA_INCLUDED_SINGLETON_H
#define GASHA_INCLUDED_SINGLETON_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ】
// singleton.h
// シングルトン【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dummy_singleton_debug.h>//シングルトンデバッグ用ダミー処理

#include <gasha/dummy_shared_lock.h>//ダミー共有ロック
#include <gasha/unique_shared_lock.h>//単一共有ロック

#include <atomic>//C++11 std::atomic_flag
#include <type_traits>//c++11 std::is_default_constructible, std::conditional

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//固定バッファシングルトン
//--------------------------------------------------------------------------------

//----------------------------------------
//固定バッファシングルトン
//※シングルトン対象クラスのインスタンスの自動生成を禁止したい場合は、そのデフォルトコンストラクタを delete もしくは private 化しておく。
//　これにより、パラメータ付きの明示的なインスタンス生成だけが有効となる。
//※静的メンバーを持つため、明示的なインスタンス化が必須。
namespace _private
{
	//固定バッファシングルトン（本体）
	//※直接使用しないクラス
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	class singleton
	{
	public:
		//型
		typedef T class_type;//シングルトン対象クラス型
		typedef LOCK_TYPE lock_type;//ロック型
		typedef GASHA_ unique_shared_lock<lock_type> unique_lock_type;//単一ロック型
		typedef DEBUG_TYPE debug_type;//デバッグ処理型
	private:
		//インスタンス生成用クラス
		class createInstance_t
		{
		public:
			//インスタンス生成
			template<typename... Tx>
			inline bool create(const char* procedure_name, Tx&&... args);
			//コンストラクタ
			inline createInstance_t(singleton& obj);
		private:
			singleton& m_singleton;
		};
		//インスタンス生成用ダミークラス
		class dummyCreateInstance_t
		{
		public:
			//インスタンス生成
			template<typename... Tx>
			inline bool create(const char* procedure_name, Tx&&... args);
			//コンストラクタ
			inline dummyCreateInstance_t(singleton& obj);
		};
		//デフォルトコンストラクタによるインスタンス生成用クラス
		typedef typename std::conditional<
			std::is_default_constructible<class_type>::value,
			createInstance_t,
			dummyCreateInstance_t
		>::type createDefaultInstance_t;
	public:
		//アクセッサ
		inline bool isCreated() const { return m_instanceRef != nullptr; }
		inline unique_lock_type& lock(){ return m_lock; }//共有単一ロックオブジェクト
		inline unique_lock_type& lock() const { return m_lock; }//共有単一ロックオブジェクト ※mutable
		inline debug_type& debug(){ return m_staticDebug; }//デバッグ用オブジェクト
		inline debug_type& debug() const { return m_staticDebug; }//デバッグ用オブジェクト ※mutable
	public:
		//キャストオペレータ
		inline operator bool() const { return isCreated(); }//インスタンスを参照しているか？
		inline operator unique_lock_type&(){ return m_lock; }//共有単一ロックオブジェクト
		inline operator unique_lock_type&() const { return m_lock; }//共有単一ロックオブジェクト ※mutable
		inline operator debug_type&(){ return m_staticDebug; }//デバッグ用オブジェクト
		inline operator debug_type&() const { return m_staticDebug; }//デバッグ用オブジェクト ※mutable
	public:
		//基本オペレータ
		inline const T* operator->() const { return m_instanceRef; }
		inline T* operator->(){ return m_instanceRef; }
		inline const T& operator*() const { return *m_instanceRef; }
		inline T& operator*(){ return *m_instanceRef; }
	public:
		//メソッド
		//シングルトンインスタンスの明示的な生成
		template<typename... Tx>
		inline bool create(const char* procedure_name, Tx&&... args);
		//シングルトンインスタンスの明示的な破棄
		inline bool destroy(const char* procedure_name);
	private:
		//デフォルトコンストラクタでインスタンスを生成
		inline bool createDefault(const char* procedure_name);
	public:
		//コンストラクタ
		//※デバッグ用の「手続き名」を指定し、シングルトンインスタンスを操作するためのシングルトンオブジェクトを生成する
		//※ロック制御を指定した場合、同時に単一ロックオブジェクトを生成し、デストラクタ時にロックを開放する。
		//　try_to_lock に失敗した場合はインスタンスへの参照を得られない。
		inline singleton(const char* procedure_name);//デフォルト：排他ロック
		inline singleton(const char* procedure_name, const GASHA_ with_lock_t);//排他ロック
		inline singleton(const char* procedure_name, const GASHA_ with_lock_shared_t);//共有ロック
		inline singleton(const char* procedure_name, const GASHA_ try_to_lock_t);//排他ロックを試行し、可能ならロックを取得し、不可能ならインスタンスの参照に失敗
		inline singleton(const char* procedure_name, const GASHA_ try_to_lock_shared_t);//共有ロックを試行し、可能ならロックを取得し、不可能ならインスタンスの参照に失敗
		inline singleton(const char* procedure_name, const GASHA_ adopt_lock_t);//排他ロック状態を引き継ぐ（排他ロック済みであるものとして処理する）
		inline singleton(const char* procedure_name, const GASHA_ adopt_shared_lock_t);//共有ロック状態を引き継ぐ（共有ロック済みであるものとして処理する）
		inline singleton(const char* procedure_name, const GASHA_ defer_lock_t);//ロックなし（引数なし時と同じ）
		//デストラクタ
		inline ~singleton();
	private:
		//フィールド
		mutable T* m_instanceRef;//シングルトンインスタンス参照
		mutable unique_lock_type m_lock;//単一ロック
		mutable typename debug_type::id_type m_debugId;//デバッグアクセスID
	private:
		//静的フィールド
		static unsigned char m_staticInstanceBuff[];//静的シングルトンインスタンス用バッファ
		static T* m_staticInstanceRef;//静的シングルトンインスタンス参照
		static std::atomic<bool> m_staticInstanceIsCreated;//インスタンス生成済み ※インスタンスの明示的な破棄があるので、std::call_once を使用しない
		static lock_type m_staticLock;//静的ロックオブジェクト
		static debug_type m_staticDebug;//デバッグ用オブジェクト
	};
}//namespace _private

//----------------------------------------
//固定バッファシングルトン：通常シングルトン
//※対象クラスメンバーに lock_type と debug_type の定義が必要。
template<class T>
using singleton = _private::singleton<T, typename T::lock_type, typename T::debug_type>;

//----------------------------------------
//固定バッファシングルトン：単純シングルトン
//※対象クラスメンバーに lock_type と debug_type の定義が不要。
template<class T, class LOCK_TYPE = GASHA_ dummySharedLock, class DEBUG_TYPE = GASHA_ dummySingletonDebug>
using simpleSingleton = _private::singleton<T, LOCK_TYPE, DEBUG_TYPE>;

//----------------------------------------
//通常シングルトンのフレンド宣言用マクロ
#define GASHA_SINGLETON_FRIEND_CLASS(T) friend class _private::singleton<T, T::lock_type, T::debug_type>;

//----------------------------------------
//単純シングルトンのフレンド宣言用マクロ
#define GASHA_SIMPLE_SINGLETON_FRIEND_CLASS(T) friend class _private::singleton<T, GASHA_ dummySharedLock, GASHA_ dummySingletonDebug>;

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/singleton.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_SINGLETON_ALLWAYS_TOGETHER_CPP_H
#include <gasha/singleton.cpp.h>
#endif//GASHA_SINGLETON_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_SINGLETON_H

// End of file

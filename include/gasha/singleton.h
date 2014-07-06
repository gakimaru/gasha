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

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シングルトン
//--------------------------------------------------------------------------------



GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/singleton.inl>

#endif//GASHA_INCLUDED_SINGLETON_H

// End of file



#if 0
//--------------------
//汎用固定バッファシングルトンテンプレートクラス
template<class T>
class CSingleton
{
	//配置newテンプレートをフレンド化
	template<class U>
	friend void* operator new(const std::size_t size, CSingleton<U>& singleton) throw();
public:
	//オペレータ（シングルトン本体のプロキシー）
	T* operator->(){ return m_singleton; }
	T& operator*(){ return *m_singleton; }
public:
	//メソッド
	//シングルトンインスタンスの明示的な破棄
	void destroy();
public:
	//コンストラクタ
	CSingleton();
private:
	//フィールド
	static T* m_singleton;//シングルトン本体参照
	static unsigned char m_singletonBuff[];//シングルトン本体用バッファ
};
#endif



#if 0
//----------------------------------------
//シングルトン処理用定数定義
class CSingletonConst
{
public:
	//定数定義

	//シングルトン属性
	enum E_ATTR
	{
		ATTR_AUTO_CREATE,             //自動生成（破棄しない）
		ATTR_AUTO_CREATE_AND_DELETE,  //自動生成と自動破棄
		ATTR_MANUAL_CREATE_AND_DELETE,//手動生成と手動破棄
	};
	//スレッドセーフ宣言
	enum E_IS_THREAD_SAFE
	{
		IS_THREAD_SAFE = true,//スレッドセーフ宣言
		IS_NOT_THREAD_SAFE = false,//非スレッドセーフ宣言
	};
	//管理シングルトン宣言
	enum E_IS_MANAGED_SINGLETON
	{
		IS_NORMAL_SINGLETON = false,//通常シングルトン宣言
		IS_MANAGED_SINGLETON = true,//管理シングルトン宣言
	};
	//シングルトン生成済み状態
	enum E_IS_CREATED
	{
		IS_NOT_CREATED = false,//未生成
		IS_CREATED = true,//生成済み
	};
	//強制処理指定
	enum E_IS_FORCED
	{
		IS_FORCED = true,//強制
		IS_NORMAL = false,//通常
	};
	//ファイナライズ処理指定
	enum E_IS_FINALIZE
	{
		IS_FINALIZE = true,//ファイナライズ
		IS_RELEASE = false,//参照カウンタリリース
	};
	//シングルトンイニシャライザー指定
	enum E_IS_INITIALIZER
	{
		IS_INITIALIZER = true,//シングルトンイニシャライザー
		IS_USING = false,//シングルトンアクセス
	};
public:
	//データ型

	//【テンプレートクラスの挙動を宣言し、静的アサーション（static_assert）を行うための構造体】
	//コンパイル動作設定用構造体：通常シングルトン宣言
	struct AUTO_SINGLETON_TYPE
	{
		static const E_IS_MANAGED_SINGLETON THIS_IS_MANAGED_SINGLETON = IS_NORMAL_SINGLETON;
	};
	//コンパイル動作設定用構造体：管理シングルトン宣言
	struct MANAGED_SINGLETON_TYPE
	{
		static const E_IS_MANAGED_SINGLETON THIS_IS_MANAGED_SINGLETON = IS_MANAGED_SINGLETON;
	};
public:
	//メソッド
	//【定数文字列化】シングルトン属性
	static const char* Attr_ToStr(const E_ATTR attr)
	{
#define CASE_TO_STR(x) case ATTR_##x: return #x; break;
		switch (attr)
		{
			CASE_TO_STR(AUTO_CREATE);
			CASE_TO_STR(AUTO_CREATE_AND_DELETE);
			CASE_TO_STR(MANUAL_CREATE_AND_DELETE);
		}
#undef CASE_TO_STR
		return "(unknown)";
	}
	//【定数文字列化】スレッドセーフ宣言
	static const char* IsThreadSafe_ToStr(const E_IS_THREAD_SAFE is_thread_safe)
	{
#define CASE_TO_STR(x) case x: return #x; break;
		switch (is_thread_safe)
		{
			CASE_TO_STR(IS_THREAD_SAFE);
			CASE_TO_STR(IS_NOT_THREAD_SAFE);
		}
#undef CASE_TO_STR
		return "(unknown)";
	}
	//【定数文字列化】管理シングルトン宣言
	static const char* IsManagedSingleton_ToStr(const E_IS_MANAGED_SINGLETON is_managed_singleton)
	{
#define CASE_TO_STR(x) case x: return #x; break;
		switch (is_managed_singleton)
		{
			CASE_TO_STR(IS_MANAGED_SINGLETON);
			CASE_TO_STR(IS_NORMAL_SINGLETON);
		}
#undef CASE_TO_STR
		return "(unknown)";
	}
	//【定数文字列化】シングルトン生成済み状態
	static const char* IsCreated_ToStr(const E_IS_CREATED is_created)
	{
#define CASE_TO_STR(x) case x: return #x; break;
		switch (is_created)
		{
			CASE_TO_STR(IS_CREATED);
			CASE_TO_STR(IS_NOT_CREATED);
		}
#undef CASE_TO_STR
		return "(unknown)";
	}
	//【定数文字列化】強制処理指定
	static const char* IsForced_ToStr(const E_IS_FORCED is_forced)
	{
#define CASE_TO_STR(x) case x: return #x; break;
		switch (is_forced)
		{
			CASE_TO_STR(IS_FORCED);
			CASE_TO_STR(IS_NORMAL);
		}
#undef CASE_TO_STR
		return "(unknown)";
	}
	//【定数文字列化】ファイナライズ指定
	static const char* IsFinalize_ToStr(const E_IS_FINALIZE is_finalize)
	{
#define CASE_TO_STR(x) case x: return #x; break;
		switch (is_finalize)
		{
			CASE_TO_STR(IS_FINALIZE);
			CASE_TO_STR(IS_RELEASE);
		}
#undef CASE_TO_STR
		return "(unknown)";
	}
	//【定数文字列化】イニシャライザー指定
	static const char* IsInitializer_ToStr(const E_IS_INITIALIZER is_initializer)
	{
#define CASE_TO_STR(x) case x: return #x; break;
		switch (is_initializer)
		{
			CASE_TO_STR(IS_INITIALIZER);
			CASE_TO_STR(IS_USING);
		}
#undef CASE_TO_STR
		return "(unknown)";
	}
	//【定数文字列化】論理値
	static const char* Bool_ToStr(const bool flag)
	{
		return flag ? "TRUE" : "FALSE";
	}
};

//----------------------------------------
//共通シングルトンテンプレートクラス　※継承専用
template<class T, class U>
class CSingletonCommon
{
	//フレンドクラス
	friend class CSingleton<T>;
	friend class CManagedSingleton<T>;
	friend class CSingletonProxy<T>;
	friend class CSingletonInitializer<T>;
	friend class CSingletonUsing<T>;
public:
	//シングルトン設定をテンプレート引数のクラスに基づいて静的に確定
	//※クラス T には、定数 CLASS_NAME, SINGLETON_ATTR, IS_THREAD_SAFE が定義されている必要がある。
	//※クラス U には、定数 THIS_IS_MANAGED_SINGLETON が定義されている必要がある。
	//static const char* T::CLASS_NAME;//クラス名 ※char*型のため、クラスの静的な値として反映できない
	static const CSingletonConst::E_ATTR THIS_SINGLETON_ATTR = T::SINGLETON_ATTR;//シングルトン属性
	static const CSingletonConst::E_IS_THREAD_SAFE THIS_IS_THREAD_SAFE = T::THIS_IS_THREAD_SAFE;//スレッドセーフ宣言
	static const CSingletonConst::E_IS_MANAGED_SINGLETON THIS_IS_MANAGED_SINGLETON = U::THIS_IS_MANAGED_SINGLETON;//管理シングルトン宣言

	//【静的アサーション】通常シングルトンでは、「手動生成属性：ATTR_MANUAL_CREATE_AND_DELETE」を使用するとアサーション違反
	STATIC_ASSERT(THIS_IS_MANAGED_SINGLETON == CSingletonConst::IS_NORMAL_SINGLETON && THIS_SINGLETON_ATTR != CSingletonConst::ATTR_MANUAL_CREATE_AND_DELETE ||
		THIS_IS_MANAGED_SINGLETON != CSingletonConst::IS_NORMAL_SINGLETON,
		"CSingleton<T> is not supported ATTR_MANUAL_CREATE_AND_DELETE in THIS_SINGLETON_ATTR.");

	//【静的静的アサーション】管理シングルトンでは、「手動生成属性：ATTR_MANUAL_CREATE_AND_DELETE」以外を使用するとアサーション違反
	STATIC_ASSERT(THIS_IS_MANAGED_SINGLETON == CSingletonConst::IS_MANAGED_SINGLETON && THIS_SINGLETON_ATTR == CSingletonConst::ATTR_MANUAL_CREATE_AND_DELETE ||
		THIS_IS_MANAGED_SINGLETON != CSingletonConst::IS_MANAGED_SINGLETON,
		"CManagedSingleton<T> is only supported ATTR_MANUAL_CREATE_AND_DELETE in THIS_SINGLETON_ATTR.");

private:
	//インスタンス生成・破棄用配置new/delete
	//※new は内部で持つ static バッファのポインタを返すだけ
	//※delete は何もしない
	//※コンストラクタ、デストラクタを実行して、オブジェクトの初期化・終了処理を実行することが目的
	void* operator new(const size_t size) throw(){ return m_buff; }
	void operator delete(void*) throw() {}
public:
	//アクセッサ
	const char* getClassName() const { return T::CLASS_NAME; }//クラス名取得
	CSingletonConst::E_ATTR getAttr() const { return THIS_SINGLETON_ATTR; }//シングルトン属性取得
	const char* getAttr_Named() const { return CSingletonConst::Attr_ToStr(getAttr()); }//シングルトン属性名取得（デバッグ用）
	CSingletonConst::E_IS_THREAD_SAFE isThreadSafe() const { return THIS_IS_THREAD_SAFE; }//スレッドセーフ宣言取得
	const char* isThreadSafe_Named() const { return CSingletonConst::IsThreadSafe_ToStr(isThreadSafe()); }//スレッドセーフ宣言名取得（デバッグ用）
	CSingletonConst::E_IS_MANAGED_SINGLETON isManagedSingleton() const { return THIS_IS_MANAGED_SINGLETON; }//管理シングルトン宣言取得
	const char* isManagedSingleton_Named() const { return CSingletonConst::IsManagedSingleton_ToStr(isManagedSingleton()); }//管理シングルトン宣言名取得（デバッグ用）
	CSingletonConst::E_IS_CREATED isCreated() const { return m_this.load() != nullptr ? CSingletonConst::IS_CREATED : CSingletonConst::IS_NOT_CREATED; }//クラス(T)インスタンス生成済み状態取得
	const char* isCreated_Named() const { return CSingletonConst::IsCreated_ToStr(isCreated()); }//クラス(T)インスタンス生成済み状態名取得（デバッグ用）
private:
	T* getThis() { return m_this.load(); }//クラス(T)インスタンスの参照を取得（禁止）
	const T* getThis() const { return m_this.load(); }//クラス(T)インスタンスのconst参照を取得（禁止）
public:
	int getRefCount() const { return m_refCount.load(); }//参照カウンタ取得
	int getRefCountMax() const { return m_refCountMax.load(); }//参照カウンタの最大到達値を取得
	int getRefCountOnThread() const { return m_refCountOnThread; }//現在のスレッド内の参照カウンタ数を取得
	int getThreadCount() const { return m_threadCount.load(); }//参照スレッド数取得
	int getThreadCountMax() const { return m_threadCountMax.load(); }//参照スレッド数の最大到達値を取得
	THREAD_ID getCreatedThreadID() const { return m_createdThreadId; }//クラス(T)インスタンスを生成したスレッドのスレッドIDを取得
	const char* getCreatedThreadName() const { return m_createdThreadName; }//クラス(T)インスタンスを生成したスレッドのスレッド名を取得
public:
	//オペレータ
	T* operator->(){ return m_this.load(); }//アロー演算子：シングルトンクラスがクラス(T)のインスタンスを偽装（代理）する
	const T* operator->() const { return m_this.load(); }//const アロー演算子：（同上）
private:
	//オペレータ（禁止）
	T& operator*(){ return *m_this.load(); }//ポインタ演算子（禁止）
	const T& operator*() const { return *m_this.load(); }//constポインタ演算子（禁止）
	operator T*(){ return m_this.load(); }//クラス T* キャスト演算子（禁止）
	operator const T*() const { return m_this.load(); }//クラスconst T* キャスト演算子（禁止）
	operator T&(){ return *m_this.load(); }//クラス T& キャスト演算子（禁止）
	operator const T&() const { return *m_this.load(); }//クラス const T& キャスト演算子（禁止）
	CSingletonCommon<T, U>& operator=(const CSingletonCommon<T, U>&){ return *this; }//コピー演算子（禁止）
	CSingletonCommon<T, U>& operator=(const CSingletonCommon<T, U>*){ return *this; }//コピー演算子（禁止）
	CSingletonCommon<T, U>& operator=(const T&){ return *this; }//コピー演算子（禁止）
	CSingletonCommon<T, U>& operator=(const T*){ return *this; }//コピー演算子（禁止）
private:
	//コピーコンストラクタ（禁止）
	explicit CSingletonCommon(CSingletonCommon<T, U>&){}
	explicit CSingletonCommon(CSingletonCommon<T, U>*){}
	explicit CSingletonCommon(T&){}
	explicit CSingletonCommon(T*){}
private:
	//参照カウンタのカウントアップ
	//※最初のカウントアップ時にインスタンスを生成
	//※なるべくロックフリーにするためにカウンタの判定は演算後の値のみを用いている
	//　...が、結局インスタンスの生成を確実に待たせる必要があるので、結局全体をロックする。
	//※「自動生成属性：ATTR_AUTO_CREATE」の時は、CallOnceにより初期化の衝突を保護
	//　... が、当初の予定に反してスレッドセーフをより強化したので、CallOnceは廃止。
	//　参考用に CallOnce を使用する場合の処理をコメントとして残す。
	//※C++11仕様の可変長テンプレートを使用し、クラス T をインスタンス化する際に、
	//　任意のパラメータを与えることを可能にしている。
	//　C++11非対応のコンパイラでは、クラス T にデフォルトコンストラクタしか
	//　使えないものとする。
	template<typename... Tx>
	bool addRef(Tx... nx)
	{
		//インスタンス生成用ラムダ式
		//※可変長テンプレート引数を使用した関数の仮引数を、ラムダ式の外部参照に指定している
		auto creator = [&nx...](bool& is_created)
		{
			if (m_this.load() == nullptr)
			{
				//クラス内 配置 new を利用し、static 領域を割り当て
				//※可変長テンプレート引数を使用した関数の仮引数を受け渡し
				m_this.store(new T(nx...));

				//生成時情報を記録
				{
					CThreadID thread_id;//現在のスレッド情報
					m_createdThreadId = thread_id.getID();//スレッドID記録
					m_createdThreadName = thread_id.getName();//スレッド名記録
					is_created = true;//生成OK
				}
			}
		};
		//参照カウンタカウントアップ（共通処理）
		return addRefCore(creator);
	}
	//参照カウンタのカウントアップ
	//※インスタンスを生成しない
	bool addRefWitoutCreate()
	{
		//インスタンス生成用ラムダ式（ダミー）
		auto creator_dummy = [](bool& is_created)
		{
		};
		//参照カウンタカウントアップ（共通処理）
		return addRefCore(creator_dummy);
	}
	//参照カウンタのカウントアップ（共通処理部）
	//※実際のインスタンス生成処理部はラムダ式で受け取る
	//※ラムダ式を受け取るためにテンプレート関数化している
	template<typename L>
	bool addRefCore(L& creator)
	{
		//カウントアップ済み判定
		if (m_isCounted)
			return false;
		m_isCounted = true;

		//ロック取得
		m_instanceLock.lock();

		//参照カウンタをカウントアップ
		bool is_allow_create = false;//インスタンス生成許可フラグ
		{
			const int ref_count_prev = m_refCount.fetch_add(1);//カウントアップ
			const int ref_count_now = ref_count_prev + 1;
			if (ref_count_prev == 0)
				is_allow_create = true;//初めてのカウントアップ時にインスタンス生成

			//最大到達値を記録
			if (m_refCountMax.load() < ref_count_now)
				m_refCountMax.store(ref_count_now);
		}

		//スレッド内参照数をカウントアップ
		{
			const int ref_count_on_thread_prev = m_refCountOnThread++;
			if (ref_count_on_thread_prev == 0)
			{
				//初めてのカウントアップ時に参照スレッド数をカウントアップ
				const int thread_count_prev = m_threadCount.fetch_add(1);
				const int thread_count_now = thread_count_prev + 1;

				//最大到達値を記録
				if (m_threadCountMax.load() < thread_count_now)
					m_threadCountMax.store(thread_count_now);
			}
		}

		//インスタンス生成
		bool is_created = false;//生成済みフラグ
		if (is_allow_create)
		{
			//※CallOnce廃止
			//	//「自動生成属性：ATTR_AUTO_CREATE」の時は CallOnce で生成
			//	if (THIS_SINGLETON_ATTR == CSingletonConst::ATTR_AUTO_CREATE)
			//	{
			//		std::call_once(m_once, creator, is_created);
			//	}
			//	//「自動生成属性：ATTR_AUTO_CREATE」以外の時は普通に生成
			//	else
			//	{
			//		creator(is_created);
			//	}
			//常に普通にインスタンス生成
			creator(is_created);
		}

		//ロック解放
		m_instanceLock.unlock();

		//インスタンス生成を行ったかどうかを返す
		return is_created;
	}
	//参照カウンタのカウントダウン
	//※最後のカウントダウン時にインスタンスを破棄
	//※なるべくロックフリーにするためにカウンタの判定は演算後の値のみを用いている
	//　...が、結局インスタンスの生成を確実に待たせる必要があるので、結局全体をロックする。
	bool release()
	{
		//カウントアップ済み判定
		if (!m_isCounted)
			return false;
		m_isCounted = false;

		//ロック取得
		m_instanceLock.lock();

		//参照カウンタをカウントダウン
		bool is_allow_delete = false;//インスタンス破棄許可フラグ
		{
			const int ref_count_prev = m_refCount.fetch_sub(1);//カウントダウン
			const int ref_count_now = ref_count_prev - 1;
			if (ref_count_now == 0)
				is_allow_delete = true;//最後のカウントダウン時にインスタンス破棄
		}

		//スレッド内参照数をカウントダウン
		{
			const int ref_count_on_thread_now = --m_refCountOnThread;
			if (ref_count_on_thread_now == 0)
			{
				//カウント 0 時に参照スレッド数をカウントダウン
				if (m_threadCount.load() > 0)
				{
					m_threadCount.fetch_sub(1);
				}
			}
		}

		//インスタンス破棄
		bool is_deleted = false;//破棄済みフラグ
		if (is_allow_delete)
		{
			//「自動生成・破棄属性：ATTR_AUTO_CREATE_AND_DELETE」の時だけ自動破棄
			if (THIS_SINGLETON_ATTR == CSingletonConst::ATTR_AUTO_CREATE_AND_DELETE)
			{
				//インスタンス破棄（共通処理利用）
				is_deleted = deleteThisCore();
			}
		}

		//ロック解放
		m_instanceLock.unlock();

		//インスタンス破棄を行ったかどうかを返す
		return is_deleted;
	}
	//手動破棄
	//※参照カウンタは更新しない
	bool deleteThis()
	{
		//カウントアップ済み状態解放
		m_isCounted = false;

		//ロック取得
		m_instanceLock.lock();

		//インスタンス破棄（共通処理利用）
		const bool is_deleted = deleteThisCore();

		//ロック解放
		m_instanceLock.unlock();

		//インスタンス破棄を行ったかどうかを返す
		return is_deleted;
	}
	//破棄（共通部）
	bool deleteThisCore()
	{
		//インスタンス破棄
		bool is_deleted = false;//破棄済みフラグ
		if (m_this.load() != nullptr)
		{
			//インスタンスを破棄
			delete m_this.load();
			m_this.store(nullptr);

			//生成時情報をリセット
			m_createdThreadId = INVALID_THREAD_ID;//スレッドID
			m_createdThreadName = nullptr;//スレッド名
			is_deleted = true;//破棄OK
		}

		//インスタンス破棄を行ったかどうかを返す
		return is_deleted;
	}
private:
	//通常シングルトン用コンストラクタ
	//※コンストラクタで自動的に参照カウンタをカウントアップし、インスタンスを生成する。
	//※C++11仕様の可変長テンプレートを使用し、クラス T をインスタンス化する際に、
	//　コンストラクタに任意のパラメータを与えることを可能にしている。
	//　C++11非対応のコンパイラでは、クラス T にデフォルトコンストラクタしか
	//　使えないものとする。
	template<typename... Tx>
	CSingletonCommon(CSingleton<T>*, Tx... nx) :
		m_isCounted(false)
	{
		//【アサーション】通常シングルトンでは、「手動生成属性：ATTR_MANUAL_CREATE_AND_DELETE」を使用するとアサーション違反（処理続行可）
		ASSERT(THIS_SINGLETON_ATTR != CSingletonConst::ATTR_MANUAL_CREATE_AND_DELETE,
			"CSingleton<T> is not supported ATTR_MANUAL_CREATE_AND_DELETE in THIS_SINGLETON_ATTR.");

		//【アサーション】インスタンス生成済みの場合、スレッドセーフじゃないクラスに対して、生成時と異なるスレッドからアクセスするとアサーション違反（処理続行可）
		CThreadID this_thread;
		ASSERT(m_createdThreadId == INVALID_THREAD_ID ||
			THIS_IS_THREAD_SAFE == CSingletonConst::IS_THREAD_SAFE ||
			THIS_IS_THREAD_SAFE == CSingletonConst::IS_NOT_THREAD_SAFE && m_createdThreadId == this_thread.getID(),
			"CSingleton<T> is not thread safe. Create thread and this thread are different.");

		//参照カウンタをカウントアップ
		//※初めてのカウントアップならインスタンスを生成する
		addRef(nx...);
	}
	//管理シングルトン用コンストラクタ
	//※コンストラクタで参照カウンタを更新せず、インスタンスを生成しない。
	CSingletonCommon(CManagedSingleton<T>*) :
		m_isCounted(false)
	{
		//【アサーション】管理シングルトンでは、「手動生成属性：ATTR_MANUAL_CREATE_AND_DELETE」以外を使用するとアサーション違反（処理続行可）
		ASSERT(THIS_SINGLETON_ATTR == CSingletonConst::ATTR_MANUAL_CREATE_AND_DELETE,
			"CManagedSingleton<T> is only supported ATTR_MANUAL_CREATE_AND_DELETE in THIS_SINGLETON_ATTR.");

		//【アサーション】インスタンス生成済みの場合、スレッドセーフじゃないクラスに対して、生成時と異なるスレッドからアクセスするとアサーション違反（処理続行可）
		CThreadID this_thread;
		ASSERT(m_createdThreadId == INVALID_THREAD_ID ||
			THIS_IS_THREAD_SAFE == CSingletonConst::IS_THREAD_SAFE ||
			THIS_IS_THREAD_SAFE == CSingletonConst::IS_NOT_THREAD_SAFE && m_createdThreadId == this_thread.getID(),
			"CManagedSingleton<T> is not thread safe. Create thread and this thread are different.");
	}
	//デストラクタ
	//※デストラクタで自動的に参照カウンタをカウントダウンする。
	//※「自動生成属性：ATTR_AUTO_CREATE_AND_DELTE」の時は、参照カウンタが 0 で自動破棄する。
	~CSingletonCommon()
	{
		//参照カウンタをカウントダウン
		//※最後のカウントダウンならインスタンスを破棄する
		release();
	}
private:
	//フィールド
	bool m_isCounted;//カウントアップ済み
	//static std::once_flag m_once;//CallOnceフラグ ※CallOnce廃止
	static CSpinLock m_instanceLock;//生成・破棄処理ロックフラグ
	static std::atomic<T*> m_this;//クラス T のインスタンス（ポインタ）
	static char m_buff[sizeof(T)];//クラス T のインスタンス用の static 領域
	static std::atomic<int> m_refCount;//参照カウンタ
	static std::atomic<int> m_refCountMax;//参照カウンタの最大到達値
	static thread_local int m_refCountOnThread;//【TLS】現在のスレッド内の参照カウンタ
	static std::atomic<int> m_threadCount;//参照スレッド数
	static std::atomic<int> m_threadCountMax;//参照スレッド数の最大到達値
	static THREAD_ID m_createdThreadId;//インスタンス生成時のスレッドのスレッドID
	static const char* m_createdThreadName;//インスタンス生成時のスレッドのスレッド名
};

//----------------------------------------
//friend クラス指定補助マクロ
//※このマクロを直接使用せず、SINGLETON_FRIEND か MANAGED_SINGLETON_FRIEND を使用する
#define SINGLETON_COMMON_FRIEND(T, U) \
	friend class CSingletonCommon<T, U>;

//----------------------------------------
//シングルトン設定補助マクロ
//※このマクロを直接使用せず、SINGLETON_ATTR か MANAGED_SINGLETON_ATTR を使用する
#define SINGLETON_COMMON_ATTR(attr, is_thread_safe) \
	static const char* CLASS_NAME; \
	static const CSingletonConst::E_ATTR SINGLETON_ATTR = attr; \
	static const CSingletonConst::E_IS_THREAD_SAFE THIS_IS_THREAD_SAFE = is_thread_safe;

//----------------------------------------
//シングルトンクラスの static インスタンス生成用マクロ
//※このマクロを直接使用せず、MAKE_SINGLETON_INSTANCE か MAKE_MANAGED_SINGLETON_INSTANCE_ALL を使用する
#define MAKE_SINGLETON_COMMON_INSTANCE(T, U) \
	/*std::once_flag CSingletonCommon<T, U>::m_once;*//*CallOnceフラグ ※CallOnce廃止*/ \
	CSpinLock CSingletonCommon<T, U>::m_instanceLock;/*生成・破棄処理ロックフラグ*/ \
	std::atomic<T*> CSingletonCommon<T, U>::m_this(nullptr);/*クラス T のインスタンス（ポインタ）*/ \
	char CSingletonCommon<T, U>::m_buff[sizeof(T)];/*クラス T のインスタンス用の static 領域*/ \
	std::atomic<int> CSingletonCommon<T, U>::m_refCount(0);/*参照カウンタ*/ \
	std::atomic<int> CSingletonCommon<T, U>::m_refCountMax(0);/*参照カウンタの最大到達値*/ \
	thread_local int CSingletonCommon<T, U>::m_refCountOnThread = 0;/*【TLS】現在のスレッド内の参照カウンタ*/ \
	std::atomic<int> CSingletonCommon<T, U>::m_threadCount(0);/*参照スレッド数*/ \
	std::atomic<int> CSingletonCommon<T, U>::m_threadCountMax(0);/*参照スレッド数の最大到達値*/ \
	THREAD_ID CSingletonCommon<T, U>::m_createdThreadId = INVALID_THREAD_ID;/*インスタンス生成時のスレッドのスレッドID*/ \
	const char* CSingletonCommon<T, U>::m_createdThreadName = nullptr;/*インスタンス生成時のスレッドのスレッド名*/ \
	const char* T::CLASS_NAME = #T;/*クラス名*/

//----------------------------------------
//通常シングルトンテンプレートクラス
template<class T>
class CSingleton : public CSingletonCommon<T, CSingletonConst::AUTO_SINGLETON_TYPE>
{
public:
	//メソッド

	//デバッグ情報表示
	void printDebugInfo(FILE* fp = stdout)
	{
		CThreadID thread_id;
		DEBUG_FPRINT(fp, "----------------------------------------\n");
		m_instanceLock.lock();//ロック取得
		DEBUG_FPRINT(fp, "Debug Info: [%s] on \"%s\"(0x%08x)\n", getClassName(), thread_id.getName(), thread_id.getID());
		DEBUG_FPRINT(fp, "  ClassAttribute       = %s\n", getAttr_Named());
		DEBUG_FPRINT(fp, "  ClassIsThreadSafe    = %s\n", isThreadSafe_Named());
		DEBUG_FPRINT(fp, "  ClassIsManaged       = %s\n", isManagedSingleton_Named());
		DEBUG_FPRINT(fp, "  ClassIsCreated       = %s\n", isCreated_Named());
		DEBUG_FPRINT(fp, "  RefCount             = %d (max=%d)\n", getRefCount(), getRefCountMax());
		DEBUG_FPRINT(fp, "  RefCountOnThisThread = %d\n", getRefCountOnThread());
		DEBUG_FPRINT(fp, "  ThreadCount          = %d (max=%d)\n", getThreadCount(), getThreadCountMax());
		DEBUG_FPRINT(fp, "  CreatedThread        = \"%s\"(0x%08x)\n", getCreatedThreadName(), getCreatedThreadID());
		m_instanceLock.unlock();//ロック解放
		DEBUG_FPRINT(fp, "----------------------------------------\n");
		DEBUG_FFLUSH(fp);
	}
public:
	//コンストラクタ
	//※コンストラクタで自動的に参照カウンタをカウントアップし、インスタンスを生成する。
	//※C++11仕様の可変長テンプレートを使用し、クラス T をインスタンス化する際に、
	//　コンストラクタに任意のパラメータを与えることを可能にしている。
	//　C++11非対応のコンパイラでは、クラス T にデフォルトコンストラクタしか
	//　使えないものとする。
	template<typename... Tx>
	CSingleton(Tx... nx) :
		CSingletonCommon<T, CSingletonConst::AUTO_SINGLETON_TYPE>::CSingletonCommon(this, nx...)
	{
		}
	//デストラクタ
	//※デストラクタで自動的に参照カウンタをカウントダウンする。
	//※「自動生成属性：ATTR_AUTO_CREATE_AND_DELTE」の時は、参照カウンタが 0 で自動破棄する。
	~CSingleton()
	{
	}
};

//----------------------------------------
//通常シングルトンクラス用 friend クラス指定補助マクロ
//※シングルトン対象クラス内に記述する
//※シングルトン対象クラス自身のクラス名を渡す
#define SINGLETON_FRIEND(T) \
	SINGLETON_COMMON_FRIEND(T, CSingletonConst::AUTO_SINGLETON_TYPE);

//----------------------------------------
//通常シングルトンクラス用シングルトン設定補助マクロ
//※シングルトン対象クラス内に記述する（privateスコープでもよい）
//※シングルトン属性とスレッドセーフ宣言を渡す
#define SINGLETON_ATTR(attr, is_thread_safe) \
	SINGLETON_COMMON_ATTR(attr, is_thread_safe);

//通常シングルトンクラス用シングルトン属性：自動生成のみ＋スレッドセーフ宣言
#define SINGLETON_ATTR_AUTO_CREATE_WITH_THREAD_SAFE() \
	SINGLETON_COMMON_ATTR(CSingletonConst::ATTR_AUTO_CREATE, CSingletonConst::IS_THREAD_SAFE);

//通常シングルトンクラス用シングルトン属性：自動生成のみ＋非スレッドセーフ宣言
#define SINGLETON_ATTR_AUTO_CREATE_WITHOUT_THREAD_SAFE() \
	SINGLETON_COMMON_ATTR(CSingletonConst::ATTR_AUTO_CREATE, CSingletonConst::IS_NOT_THREAD_SAFE);

//通常シングルトンクラス用シングルトン属性：自動生成／自動削除＋スレッドセーフ宣言
#define SINGLETON_ATTR_AUTO_CREATE_AND_DELETE_WITH_THREAD_SAFE() \
	SINGLETON_COMMON_ATTR(CSingletonConst::ATTR_AUTO_CREATE_AND_DELETE, CSingletonConst::IS_THREAD_SAFE);

//通常シングルトンクラス用シングルトン属性：自動生成／自動削除＋非スレッドセーフ宣言
#define SINGLETON_ATTR_AUTO_CREATE_AND_DELETE_WITHOUT_THREAD_SAFE() \
	SINGLETON_COMMON_ATTR(CSingletonConst::ATTR_AUTO_CREATE_AND_DELETE, CSingletonConst::IS_NOT_THREAD_SAFE);

//----------------------------------------
//通常シングルトンクラス用 static インスタンス生成用マクロ
//※.cpp ファイル中に記述する
//※シングルトン対象クラスのクラス名を渡す
#define MAKE_SINGLETON_INSTANCE(T) \
	MAKE_SINGLETON_COMMON_INSTANCE(T, CSingletonConst::AUTO_SINGLETON_TYPE)

//----------------------------------------
//管理シングルトンテンプレートクラス
template<class T>
class CManagedSingleton : public CSingletonCommon<T, CSingletonConst::MANAGED_SINGLETON_TYPE>
{
	//フレンドクラス
	friend class CSingletonProxy<T>;
	friend class CSingletonInitializer<T>;
	friend class CSingletonUsing<T>;
public:
	//シングルトン設定をテンプレート引数のクラスに基づいて静的に確定
	//※クラス T には、定数 SINGLETON_USING_LIST_MAX が定義されている必要がある
	static const std::size_t THIS_SINGLETON_USING_LIST_MAX = T::SINGLETON_USING_LIST_MAX;//使用中処理リスト数

	//【静的アサーション】T::SINGLETON_USING_LIST_MAX は 1 以上の指定が必要
	STATIC_ASSERT(THIS_SINGLETON_USING_LIST_MAX > 0, "class THIS_SINGLETON_USING_LIST_MAX is under 0.");
public:
	//アクセッサ
	const char* getInitializerName() const { return m_initializerName.load(); }//インスタンスを生成したイニシャライザー名取得
	int getInitializerExists() const { return m_initializerExists.load(); }//イニシャライザー数取得
	CRWLock& getRWLock() { return m_lock; }//リード・ライトロック取得
	const char* getDebugTrapName() const { return m_debugTrapName.load(); }//デバッグ用トラップ対象処理名取得
	void setDebugTrapName(const char* name){ m_debugTrapName.store(const_cast<char*>(name)); }//デバッグ用トラップ対象処理名更新
	void resetDebugTrapName(){ m_debugTrapName.store(nullptr); }//デバッグ用トラップ対象処理名リセット
	const char* getDebugTrapThreadName() const { return m_debugTrapThreadName.load(); }//デバッグ用トラップ対象スレッド名取得
	void setDebugTrapThreadName(const char* name){ m_debugTrapThreadName.store(const_cast<char*>(name)); }//デバッグ用トラップ対象スレッド名更新
	void resetDebugTrapThreadName(){ m_debugTrapThreadName.store(nullptr); }//デバッグ用トラップ対象スレッド名リセット
private:
	void setInitializerName(const char* name){ m_initializerName.store(const_cast<char*>(name)); }//インスタンスを生成したイニシャライザー名更新
	void resetInitializerName(){ m_initializerName.store(nullptr); }//インスタンスを生成したイニシャライザー名リセット
	int addInitializerExists(){ return m_initializerExists.fetch_add(1); }//イニシャライザー数をカウントアップ
	int subInitializerExists(){ return m_initializerExists.fetch_sub(1); }//イニシャライザー数をカウントダウン
public:
	//オペレータ
	operator CRWLock& (){ return m_lock; }//リード・ライトロックキャストオペレータ
private:
	//メソッド

	//使用中処理リストに処理情報を追加
	//※処理名、スレッドID、スレッド名を渡す
	void addUsingList(const char* name, const THREAD_ID thread_id, const char* thread_name, const CSingletonConst::E_IS_INITIALIZER is_initializer)
	{
		//トラップ対象処理名＆スレッド名チェック
		//※文字列の一致はポインタで判定（文字列リテラルが同じならポインタが一致する⇒コンパイルオプション依存）
		const char* trap_name = m_debugTrapName.load();
		const char* trap_thread_name = m_debugTrapThreadName.load();
		if ((trap_name || trap_thread_name) &&
			(!trap_name || (name && name == trap_name)) &&
			(!trap_thread_name || (thread_name && thread_name == trap_thread_name)))
		{
			//メッセージ
			DEBUG_PRINT("Singleton catch the trap!! (\"%s\", Thread=\"%s\")\n", trap_name, trap_thread_name);

			//ブレークポイント
			BREAK_POINT();
		}
		//使用中情報作成
		//※THIS_SINGLETON_USING_LIST_MAX で指定された数まで同時に記録可能
		m_thisUsingInfo = m_usingListBuff.createData(m_usingList.load(), name, thread_id, thread_name, is_initializer);
		if (m_thisUsingInfo)
		{
			//【参考】ロックフリーなスタックプッシュ（先頭ノード追加）アルゴリズム
			//m_thisUsingInfo->m_next = m_usingList.load();
			//while (!m_usingList.compare_exchange_weak(m_thisUsingInfo->m_next, m_thisUsingInfo)){}
			//※破棄処理側をロックフリーにできないので、このアルゴリズムを使用しない

			//使用中情報リスト追加
			m_usingListLock.lock();//ロック取得
			m_thisUsingInfo->m_next = m_usingList.load();
			m_usingList.store(m_thisUsingInfo);//先頭ノードに挿入（連結リスト）
			m_usingListNum.fetch_add(1);//使用中処理リストの使用数カウントアップ
			if (m_usingListNumMax.load() < m_usingListNum.load())
				m_usingListNumMax.store(m_usingListNum.load());//使用中処理リストの使用数の最大到達値更新
			m_usingListLock.unlock();//ロック解放
		}
	}
	//使用中処理リストから処理情報削除
	//※m_thisUsingInfo をリストから検索して削除
	void deleteUsingList()
	{
		if (m_thisUsingInfo)
		{
			//【参考】ロックフリーなスタックポップ（先頭ノード取り出し）アルゴリズム
			//USING_INFO* old_head = m_usingList.load();
			//while (old_head && !m_usingList.compare_exchange_weak(old_head, old_head->next));
			//※この要件には適合しないので、ロックを使用する

			//使用中情報リスト削除
			m_usingListLock.lock();//ロック取得
			USING_INFO* now = m_usingList.load();
			if (now && now == m_thisUsingInfo)
			{
				//先頭ノードの場合
				m_usingList.store(m_thisUsingInfo->m_next);
			}
			else
			{
				//先頭以降のノードの場合
				while (now && now->m_next != m_thisUsingInfo){ now = now->m_next; }
				if (now)
					now->m_next = m_thisUsingInfo->m_next;
			}
			m_usingListBuff.destroyData(m_thisUsingInfo);//削除
			m_usingListNum.fetch_sub(1);//使用中処理リストの使用数カウントダウン
			m_usingListLock.unlock();//ロック解放
		}
	}
public:
	//使用中処理リスト表示
	void printUsingList(const char* name, FILE* fp = stdout)
	{
		CThreadID thread_id;
		DEBUG_FPRINT(fp, "----------------------------------------\n");
		DEBUG_FPRINT(fp, "Using List: [%s] by \"%s\" on \"%s\"(0x%08x)\n", getClassName(), name, thread_id.getName(), thread_id.getID());
		m_usingListLock.lock();//ロック取得
		USING_INFO* info = m_usingList;
		while (info)
		{
			DEBUG_FPRINT(fp, " \"%s\" %s\ton \"%s\"(0x%08x)\n",
				info->m_name,
				CSingletonConst::IsInitializer_ToStr(info->m_isInitializer ? CSingletonConst::IS_INITIALIZER : CSingletonConst::IS_USING),
				info->m_threadName,
				info->m_threadId
				);
			info = info->m_next;
		}
		DEBUG_FPRINT(fp, "(num=%d, max=%d)\n", m_usingListNum.load(), m_usingListNumMax.load());
		m_usingListLock.unlock();//ロック解放
		DEBUG_FPRINT(fp, "----------------------------------------\n");
		DEBUG_FFLUSH(fp);
	}
	//デバッグ情報表示
	void printDebugInfo(const char* name, FILE* fp = stdout)
	{
		CThreadID thread_id;
		DEBUG_FPRINT(fp, "----------------------------------------\n");
		m_instanceLock.lock();//ロック取得
		DEBUG_FPRINT(fp, "Debug Info: [%s] by \"%s\" on \"%s\"(0x%08x)\n", getClassName(), name, thread_id.getName(), thread_id.getID());
		DEBUG_FPRINT(fp, "  ClassAttribute       = %s\n", getAttr_Named());
		DEBUG_FPRINT(fp, "  ClassIsThreadSafe    = %s\n", isThreadSafe_Named());
		DEBUG_FPRINT(fp, "  ClassIsManaged       = %s\n", isManagedSingleton_Named());
		DEBUG_FPRINT(fp, "  ClassIsCreated       = %s\n", isCreated_Named());
		DEBUG_FPRINT(fp, "  RefCount             = %d (max=%d)\n", getRefCount(), getRefCountMax());
		DEBUG_FPRINT(fp, "  RefCountOnThisThread = %d\n", getRefCountOnThread());
		DEBUG_FPRINT(fp, "  ThreadCount          = %d (max=%d)\n", getThreadCount(), getThreadCountMax());
		DEBUG_FPRINT(fp, "  CreatedThread        = \"%s\"(0x%08x)\n", getCreatedThreadName(), getCreatedThreadID());
		DEBUG_FPRINT(fp, "  InitializerName      = \"%s\"\n", getInitializerName());
		DEBUG_FPRINT(fp, "  InitializerExists    = %d\n", getInitializerExists());
		DEBUG_FPRINT(fp, "  DebugTrap            = \"%s\" on \"%s\"\n", getDebugTrapName(), getDebugTrapThreadName());
		m_instanceLock.unlock();//ロック解放
		DEBUG_FPRINT(fp, "----------------------------------------\n");
		DEBUG_FFLUSH(fp);
	}
private:
	//コンストラクタ
	//※コンストラクタで参照カウンタを更新せず、インスタンスを生成しない。
	CManagedSingleton() :
		CSingletonCommon<T, CSingletonConst::MANAGED_SINGLETON_TYPE>::CSingletonCommon(this)
	{
		}
	//デストラクタ
	//※デストラクタで自動的に参照カウンタをカウントダウンする。
	~CManagedSingleton()
	{
	}
private:
	//構造体：使用中処理情報型
	struct USING_INFO
	{
		USING_INFO* m_next;//次のノード
		const char* m_name;//処理名
		const THREAD_ID m_threadId;//処理スレッドのスレッドID
		const char* m_threadName;//処理スレッドのスレッド名
		bool m_isInitializer;//イニシャライザーフラグ
		//コンストラクタ
		USING_INFO(USING_INFO* next, const char* name, const THREAD_ID thread_id, const char* thread_name, const CSingletonConst::E_IS_INITIALIZER is_initializer) :
			m_next(next),
			m_name(name),
			m_threadId(thread_id),
			m_threadName(thread_name),
			m_isInitializer(is_initializer == CSingletonConst::IS_INITIALIZER)
		{
		}
		//デストラクタ
		~USING_INFO()
		{
		}
	};
private:
	//フィールド
	USING_INFO* m_thisUsingInfo;//自身の使用中処理情報
	static std::atomic<char*> m_initializerName;//インスタンスを生成したイニシャライザー名
	static std::atomic<int> m_initializerExists;//イニシャライザー数（普通は 0 か 1）
	static CRWLock m_lock;//リード・ライトロック
	static std::atomic<char*> m_debugTrapName;//デバッグ用トラップ対象処理名
	static std::atomic<char*> m_debugTrapThreadName;//デバッグ用トラップ対象スレッド名
	static std::atomic<int> m_usingListNum;//使用中処理リストの使用数
	static std::atomic<int> m_usingListNumMax;//使用中処理リストの使用数の最大到達値
	static std::atomic<USING_INFO*> m_usingList;//使用中処理リスト
	static CSpinLock m_usingListLock;//使用中処理リストのロック用フラグ
	static CPoolAllocatorWithType<USING_INFO, THIS_SINGLETON_USING_LIST_MAX> m_usingListBuff;//使用中処理リストの領域
};

//----------------------------------------
//管理シングルトンクラス用 friend クラス指定補助マクロ
//※シングルトン対象クラス内に記述する
//※シングルトン対象クラス自身のクラス名を渡す
#define MANAGED_SINGLETON_FRIEND(T) \
	SINGLETON_COMMON_FRIEND(T, CSingletonConst::MANAGED_SINGLETON_TYPE);

//----------------------------------------
//管理シングルトンクラス用シングルトン設定補助マクロ
//※シングルトン対象クラス内に記述する（privateスコープでもよい）
//※シングルトン属性とスレッドセーフ宣言を渡す
#define MANAGED_SINGLETON_ATTR(is_thread_safe, using_list_num) \
	SINGLETON_COMMON_ATTR(CSingletonConst::ATTR_MANUAL_CREATE_AND_DELETE, is_thread_safe); \
	static const std::size_t SINGLETON_USING_LIST_MAX = using_list_num;

//管理シングルトンクラス用シングルトン属性：手動生成＋スレッドセーフ宣言
#define MANAGED_SINGLETON_ATTR_WITH_THREAD_SAFE(using_list_num) \
	MANAGED_SINGLETON_ATTR(CSingletonConst::IS_THREAD_SAFE, using_list_num);

//管理シングルトンクラス用シングルトン属性：手動生成＋非スレッドセーフ宣言
#define MANAGED_SINGLETON_ATTR_WITHOUT_THREAD_SAFE(using_list_num) \
	MANAGED_SINGLETON_ATTR(CSingletonConst::IS_NOT_THREAD_SAFE, using_list_num);

//----------------------------------------
//管理シングルトンクラス用 static インスタンス生成用マクロ
//※このマクロを直接使用せず、MAKE_MANAGED_SINGLETON_INSTANCE_ALL を使用する
#define MAKE_MANAGED_SINGLETON_INSTANCE(T) \
	std::atomic<char*> CManagedSingleton<T>::m_initializerName(nullptr);/*インスタンスを生成したイニシャライザー名*/ \
	std::atomic<int> CManagedSingleton<T>::m_initializerExists(0);/*イニシャライザー数*/ \
	CRWLock CManagedSingleton<T>::m_lock;/*リード・ライトロック*/ \
	std::atomic<char*> CManagedSingleton<T>::m_debugTrapName(nullptr);/*デバッグ用トラップ対象処理名*/ \
	std::atomic<char*> CManagedSingleton<T>::m_debugTrapThreadName(nullptr);/*デバッグ用トラップ対象スレッド名*/ \
	std::atomic<int> CManagedSingleton<T>::m_usingListNum(0);/*使用中処理リストの使用数*/ \
	std::atomic<int> CManagedSingleton<T>::m_usingListNumMax(0);/*使用中処理リストの使用数の最大到達値*/ \
	std::atomic< CManagedSingleton<T>::USING_INFO* > CManagedSingleton<T>::m_usingList(nullptr);/*使用中処理リスト*/ \
	CSpinLock CManagedSingleton<T>::m_usingListLock;/*使用中処理リストのロック用フラグ*/ \
	CPoolAllocatorWithType<CManagedSingleton<T>::USING_INFO, CManagedSingleton<T>::THIS_SINGLETON_USING_LIST_MAX> CManagedSingleton<T>::m_usingListBuff;/*使用中処理リストの領域*/

//----------------------------------------
//【シングルトン用ヘルパー】シングルトンプロキシーテンプレートクラス　※継承専用
template<class T>
class CSingletonProxy
{
public:
	//クラスの属性、動作モードをテンプレート引数に基づいて静的に決定
	//※クラス T には、定数 CLASS_NAME, SINGLETON_ATTR, IS_THREAD_SAFE が定義されている必要がある。
	//※クラス U には、定数 THIS_IS_MANAGED_SINGLETON が定義されている必要がある。
	static const CSingletonConst::E_ATTR THIS_SINGLETON_ATTR = T::SINGLETON_ATTR;//シングルトン属性
	static const CSingletonConst::E_IS_THREAD_SAFE THIS_IS_THREAD_SAFE = T::THIS_IS_THREAD_SAFE;//スレッドセーフ宣言
public:
	//アクセッサ
	const char* getName() const { return m_name; }//処理名取得
	const CThreadID& getThreadID() const { return m_threadId; }//現在のスレッドID取得
	const char* getClassName() const { return m_singleton.getClassName(); }//現在のスレッド名取得　※委譲
	CSingletonConst::E_ATTR getAttr() const { return m_singleton.getAttr(); }//シングルトン属性取得　※委譲
	const char* getAttr_Named() const { return m_singleton.getAttr_Named(); }//シングルトン属性名取得（デバッグ用）　※委譲
	CSingletonConst::E_IS_THREAD_SAFE isThreadSafe() const { return m_singleton.isThreadSafe(); }//スレッドセーフ宣言取得　※委譲
	const char* isThreadSafe_Named() const { return m_singleton.isThreadSafe_Named(); }//スレッドセーフ宣言名取得（デバッグ用）　※委譲
	CSingletonConst::E_IS_MANAGED_SINGLETON isManagedSingleton() const { return m_singleton.isManagedSingleton(); }//管理シングルトン宣言取得　※委譲
	const char* isManagedSingleton_Named() const { return m_singleton.isManagedSingleton_Named(); }//管理シングルトン宣言名取得（デバッグ用）　※委譲
	CSingletonConst::E_IS_CREATED isCreated() const { return m_singleton.isCreated(); }//クラス(T)インスタンス生成済み状態取得　※委譲
	const char* isCreated_Named() const { return m_singleton.isCreated_Named(); }//クラス(T)インスタンス生成済み状態名取得（デバッグ用）　※委譲
private:
	CManagedSingleton<T>& getSinlgeton() { return m_singleton; }//シングルトン取得
	const CManagedSingleton<T>& getSinlgeton() const { return m_singleton; }//constシングルトン取得
	T* getThis() { return m_singleton.getThis(); }//クラス(T)インスタンスの参照を取得（禁止）　※委譲
	const T* getThis() const { return m_singleton.getThis(); }//クラス(T)インスタンスのconst参照を取得（禁止）　※委譲
public:
	int getRefCount() const { return m_singleton.getRefCount(); }//参照カウンタ取得　※委譲
	int getRefCountMax() const { return m_singleton.getRefCountMax(); }//参照カウンタの最大到達値を取得　※委譲
	int getRefCountOnThread() const { return m_singleton.getRefCountOnThread(); }//現在のスレッド内の参照カウンタ数を取得　※委譲
	int getThreadCount() const { return m_singleton.getThreadCount(); }//参照スレッド数取得　※委譲
	int getThreadCountMax() const { return m_singleton.getThreadCountMax(); }//参照スレッド数の最大到達値を取得　※委譲
	THREAD_ID getCreatedThreadID() const { return m_singleton.getCreatedThreadID(); }//クラス(T)インスタンスを生成したスレッドのスレッドIDを取得　※委譲
	const char* getCreatedThreadName() const { return m_singleton.getCreatedThreadName(); }//クラス(T)インスタンスを生成したスレッドのスレッド名を取得　※委譲
	const char* getInitializerName() const { return m_singleton.getInitializerName(); }//インスタンスを生成したイニシャライザー名取得　※委譲
	int getInitializerExists() const { return m_singleton.getInitializerExists(); }//イニシャライザー数取得　※委譲
	CRWLock& getRWLock() { return m_singleton.getRWLock(); }//リード・ライトロック取得　※委譲
	const char* getDebugTrapName() const { return m_singleton.getDebugTrapName(); }//デバッグ用トラップ対象処理名取得　※委譲
	void setDebugTrapName(const char* name){ m_singleton.setDebugTrapName(name); }//デバッグ用トラップ対象処理名更新　※委譲
	void resetDebugTrapName(){ m_singleton.resetDebugTrapName(); }//デバッグ用トラップ対象処理名リセット　※委譲
	const char* getDebugTrapThreadName() const { return m_singleton.getDebugTrapThreadName(); }//デバッグ用トラップ対象スレッド名取得　※委譲
	void setDebugTrapThreadName(const char* name){ m_singleton.setDebugTrapThreadName(name); }//デバッグ用トラップ対象スレッド名更新　※委譲
	void resetDebugTrapThreadName(){ m_singleton.resetDebugTrapThreadName(); }//デバッグ用トラップ対象スレッド名リセット　※委譲
public:
	//メソッド
	void printUsingList(FILE* fp = stdout){ m_singleton.printUsingList(m_name, fp); }//使用中処理リスト表示　※委譲
	void printDebugInfo(FILE* fp = stdout){ m_singleton.printDebugInfo(m_name, fp); }//デバッグ情報表示　※委譲
public:
	//オペレータ
	T* operator->(){ return m_singleton; }//アロー演算子：シングルトンクラスがクラス(T)のインスタンスを偽装（代理）する　※委譲
	const T* operator->() const { return m_singleton; }//const アロー演算子：（同上）　※委譲
	operator CRWLock& (){ return m_singleton; }//リード・ライトロックキャストオペレータ　※委譲
protected:
	//オペレータ（禁止）
	T& operator*(){ return *m_singleton; }//ポインタ演算子（禁止）　※委譲
	const T& operator*() const { return *m_singleton; }//constポインタ演算子（禁止）　※委譲
	operator T*(){ return m_singleton; }//クラス T* キャスト演算子（禁止）　※委譲
	operator const T*() const { return m_singleton; }//クラスconst T* キャスト演算子（禁止）　※委譲
	operator T&(){ return *m_singleton; }//クラス T& キャスト演算子（禁止）　※委譲
	operator const T&() const { return *m_singleton; }//クラス const T& キャスト演算子（禁止）　※委譲
	CSingletonProxy<T>& operator=(const CSingletonProxy<T>&){ return *this; }//コピー演算子（禁止）
	CSingletonProxy<T>& operator=(const CSingletonProxy<T>*){ return *this; }//コピー演算子（禁止）
	CSingletonProxy<T>& operator=(const T&){ return *this; }//コピー演算子（禁止）
	CSingletonProxy<T>& operator=(const T*){ return *this; }//コピー演算子（禁止）
private:
	//コピーコンストラクタ（禁止）
	explicit CSingletonProxy(CSingletonInitializer<T>&){}
	explicit CSingletonProxy(CSingletonInitializer<T>*){}
	explicit CSingletonProxy(T&){}
	explicit CSingletonProxy(T*){}
private:
	//デフォルトコンストラクタ（禁止）
	CSingletonProxy(){}
protected:
	//コンストラクタ
	CSingletonProxy(const char* name) :
		m_singleton(),
		m_name(name),
		m_threadId(),
		m_isAddRef(false)
	{}
	//デストラクタ
	~CSingletonProxy()
	{}
protected:
	//フィールド
	CManagedSingleton<T> m_singleton;//管理シングルトン
	const char* m_name;//処理名
	CThreadID m_threadId;//スレッドID
	bool m_isAddRef;//参照カウンタカウントアップ済み
};

//----------------------------------------
//シングルトンプロキシークラス用 static インスタンス生成用マクロ
//※このマクロを直接使用せず、MAKE_MANAGED_SINGLETON_INSTANCE_ALL を使用する
#define MAKE_SINGLETON_PROXY_INSTANCE(T) \
	//

//----------------------------------------
//【シングルトン用ヘルパー】シングルトンイニシャライザーテンプレートクラス
template<class T>
class CSingletonInitializer : public CSingletonProxy<T>
{
	//【静的アサーション】CSingletonInitializer<T>は、「手動生成属性：ATTR_MANUAL_CREATE_AND_DELETE」以外利用不可
	STATIC_ASSERT(THIS_SINGLETON_ATTR == CSingletonConst::ATTR_MANUAL_CREATE_AND_DELETE,
	"CSingletonInitializer<T> is only supported ATTR_MANUAL_CREATE_AND_DELETE in THIS_SINGLETON_ATTR.");
public:
	//アクセッサ
	bool isExist() const { return CManagedSingleton<T>::isExistInitializer(); }//イニシャライザーが存在するか？
private:
	void setInitializerName(const char* name) { return m_singleton.setInitializerName(name); }//イニシャライザー名を更新　※委譲
	void resetInitializerName() { return m_singleton.resetInitializerName(); }//イニシャライザー名をリセット　※委譲
private:
	//オペレータ（禁止）
	CSingletonInitializer<T>& operator=(const CSingletonInitializer<T>&){ return *this; }//コピー演算子（禁止）
	CSingletonInitializer<T>& operator=(const CSingletonInitializer<T>*){ return *this; }//コピー演算子（禁止）
	CSingletonInitializer<T>& operator=(const T&){ return *this; }//コピー演算子（禁止）
	CSingletonInitializer<T>& operator=(const T*){ return *this; }//コピー演算子（禁止）
private:
	//コピーコンストラクタ（禁止）
	explicit CSingletonInitializer(CSingletonInitializer<T>&){}
	explicit CSingletonInitializer(CSingletonInitializer<T>*){}
	explicit CSingletonInitializer(T&){}
	explicit CSingletonInitializer(T*){}
public:
	//メソッド

	//初期化（手動インスタンス生成）
	//※C++11仕様の可変長テンプレートを使用し、クラス T をインスタンス化する際に、
	//　任意のパラメータを与えることを可能にしている。
	//　C++11非対応のコンパイラでは、クラス T にデフォルトコンストラクタしか
	//　使えないものとする。
	template<typename... Tx>
	bool initialize(Tx... nx)
	{
		//【アサーション】既にインスタンスが生成済みならアサーション違反（処理続行可）
		ASSERT(!isCreated(),
			"CSingletonInitializer<T> cannot create. Singleton instance is already exist.");

		//生成（参照カウンタカウントアップ）
		bool is_created = false;//生成したか？
		if (!m_isAddRef)//まだ参照カウンタカをカウントアップしていない時に処理実行
		{
			m_isAddRef = true;//参照カウンタカウントアップ状態ON
			is_created = m_singleton.addRef(nx...);//生成（参照カウンタカウントアップ）
			//生成されたか？
			if (is_created)
			{
				//関連情報をセット
				setInitializerName(m_name);//インスタンスを生成したイニシャライザー名
				CRWLock& lock(getRWLock());
				lock.setIgnoreThreadID(getThreadID().getID());//リード・ライトロックの対象スレッドIDを更新
			}
		}
		//インスタンスが生成されたかどうかを返す
		return is_created;
	}
	//破棄（手動破棄）
	bool finalize(CSingletonConst::E_IS_FORCED is_forced = CSingletonConst::IS_NORMAL){ return releaseCore(CSingletonConst::IS_FINALIZE, is_forced); }
	//（明示的な）参照カウンタリリース　※破棄しない
	bool release(){ return releaseCore(CSingletonConst::IS_RELEASE, CSingletonConst::IS_NORMAL); }
private:
	//参照カウンタリリース（共通処理）
	bool releaseCore(CSingletonConst::E_IS_FINALIZE is_finalize, CSingletonConst::E_IS_FORCED is_forced)
	{
		//リリース済みなら即終了
		//※ファイナライズ時はリリース済みでも実行する
		if (!m_isAddRef && is_finalize != CSingletonConst::IS_FINALIZE)
			return false;

		//【アサーション】既に破棄済みならアサーション違反（処理続行可）
		//※強制実行時はアサーション違反としない
		ASSERT(isCreated() == CSingletonConst::IS_CREATED ||
			is_forced == CSingletonConst::IS_FORCED,
			"CSingletonInitializer<T> cannot delete. Singleton instance is already deleted.");

		//【アサーション】ファイナライズ時にまだが参照が残っているならアサーション違反（処理続行可）
		//※強制時刻時はアサーション違反としない
		const int LAST_COUNT = getRefCount() - (m_isAddRef ? 1 : 0);
		ASSERT(is_finalize == CSingletonConst::IS_FINALIZE && LAST_COUNT == 0 ||
			is_finalize != CSingletonConst::IS_FINALIZE ||
			is_forced == CSingletonConst::IS_FORCED,
			"CSingletonInitializer<T> will finalize, yet still using singleton.");

		//参照カウンタリリース
		bool is_deleted = false;//削除済みフラグ
		if (m_isAddRef || is_finalize == CSingletonConst::IS_FINALIZE)//参照カウンタをカウントアップしていたか、強制手動破棄指定時に処理実行
		{
			m_isAddRef = false;//参照カウンタカウントアップ状態OFF
			if (is_finalize == CSingletonConst::IS_FINALIZE)//ファイナライズ（手動破棄）か？
				is_deleted = m_singleton.deleteThis();//手動破棄
			else
				is_deleted = m_singleton.release(); //リリース
			//破棄されたか？
			if (is_deleted)
			{
				//関連情報を破棄
				resetInitializerName();//インスタンスを生成したイニシャライザー名
				resetDebugTrapName();//デバッグ用トラップ対象処理名
				resetDebugTrapThreadName();//デバッグ用トラップ対象スレッド名
			}
		}
		//インスタンスが破棄されたかどうかを返す
		return is_deleted;
	}
public:
	//コンストラクタ
	CSingletonInitializer(const char* name, const CSingletonConst::E_IS_FORCED is_forced = CSingletonConst::IS_NORMAL) :
		CSingletonProxy(name),
		m_isFirst(false)
	{
		//イニシャライザー数カウントアップ
		const int exists_prev = m_singleton.addInitializerExists();

		//使用中処理リスト追加
		m_singleton.addUsingList(m_name, m_threadId.getID(), m_threadId.getName(), CSingletonConst::IS_INITIALIZER);

		//最初のイニシャライザーか判定
		if (exists_prev == 0)
		{
			m_isFirst = true;//最初のイニシャライザーインスタンス
		}

		//【アサーション】他にもイニシャライザーがいるならアサーション違反（処理続行可）
		//※強制破棄指定時はアサーション違反としない
		ASSERT(m_isFirst == true || is_forced == CSingletonConst::IS_FORCED,
			"CSingletonInitializer<T>: already exist!");
	}
	//デストラクタ
	~CSingletonInitializer()
	{
		//イニシャライザー数カウントダウン
		m_singleton.subInitializerExists();

		//使用中処理リストから破棄
		m_singleton.deleteUsingList();

		//参照カウンタをリリース
		release();
	}
private:
	//フィールド
	bool m_isFirst;//最初のイニシャライザーインスタンスか？
};

//----------------------------------------
//シングルトンイニシャライザークラス用 static インスタンス生成用マクロ
//※このマクロを直接使用せず、MAKE_MANAGED_SINGLETON_INSTANCE_ALL を使用する
#define MAKE_SINGLETON_INITIALIZER_INSTANCE(T) \
	//

//----------------------------------------
//【シングルトン用ヘルパー】シングルトンアクセステンプレートクラス
template<class T>
class CSingletonUsing : public CSingletonProxy<T>
{
	//【静的アサーション】CSingletonInitializer<T>は、「手動生成属性：ATTR_MANUAL_CREATE_AND_DELETE」以外利用不可
	STATIC_ASSERT(THIS_SINGLETON_ATTR == CSingletonConst::ATTR_MANUAL_CREATE_AND_DELETE,
	"CSingletonUsing<T> is only supported ATTR_MANUAL_CREATE_AND_DELETE in THIS_SINGLETON_ATTR.");
private:
	//オペレータ（禁止）
	CSingletonUsing<T>& operator=(const CSingletonUsing<T>&){ return *this; }//コピー演算子（禁止）
	CSingletonUsing<T>& operator=(const CSingletonUsing<T>*){ return *this; }//コピー演算子（禁止）
	CSingletonUsing<T>& operator=(const T&){ return *this; }//コピー演算子（禁止）
	CSingletonUsing<T>& operator=(const T*){ return *this; }//コピー演算子（禁止）
private:
	//コピーコンストラクタ禁止
	explicit CSingletonUsing(CSingletonUsing<T>&){}
	explicit CSingletonUsing(CSingletonUsing<T>*){}
	explicit CSingletonUsing(T&){}
	explicit CSingletonUsing(T*){}
public:
	//コンストラクタ
	CSingletonUsing(const char* name) :
		CSingletonProxy(name)
	{
		//※不要
		//	//【アサーション】イニシャライザーが存在していない時はアサーション違反
		//	ASSERT(m_singleton.getInitializerExists() > 0,
		//		"CSingletonUsing<T> cannot use. SingletonInitialize<T> not exist.");
		//	if (getInitializerExists() == 0)
		//	{
		//		return;
		//	}

		//【アサーション】インスタンスが存在していない時はアサーション違反
		ASSERT(m_singleton.isCreated() == CSingletonConst::IS_CREATED,
			"CSingletonUsing<T> cannot use. Singleton instance not exist.");
		if (isCreated() != CSingletonConst::IS_CREATED)
		{
			return;
		}

		//参照カウンタカウントアップ状態ON
		m_isAddRef = true;

		//参照カウンタをカウントアップ
		m_singleton.addRefWitoutCreate();

		//使用中処理リスト追加
		m_singleton.addUsingList(m_name, m_threadId.getID(), m_threadId.getName(), CSingletonConst::IS_USING);
	}
	//デストラクタ
	~CSingletonUsing()
	{
		if (m_isAddRef)
		{
			//参照カウンタカウントアップ状態OFF
			m_isAddRef = false;

			//参照カウンタをカウントダウン
			m_singleton.release();

			//使用中処理リスト破棄
			m_singleton.deleteUsingList();
		}
	}
};

//----------------------------------------
//シングルトンアクセスクラス用 static インスタンス生成用マクロ
//※このマクロを直接使用せず、MAKE_MANAGED_SINGLETON_INSTANCE_ALL を使用する
#define MAKE_SINGLETON_USING_INSTANCE(T) \
	//

//----------------------------------------
//シングルトンアクセスヘルパークラス用 static インスタンス生成用マクロ
//※.cpp ファイル中に記述する
//※シングルトン対象クラスのクラス名を渡す
#define MAKE_MANAGED_SINGLETON_INSTANCE_ALL(T) \
	MAKE_SINGLETON_COMMON_INSTANCE(T, CSingletonConst::MANAGED_SINGLETON_TYPE); \
	MAKE_MANAGED_SINGLETON_INSTANCE(T); \
	MAKE_SINGLETON_INITIALIZER_INSTANCE(T); \
	MAKE_SINGLETON_USING_INSTANCE(T);
#endif

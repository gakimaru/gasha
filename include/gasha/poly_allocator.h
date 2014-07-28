#pragma once
#ifndef GASHA_INCLUDED_POLY_ALLOCATOR_H
#define GASHA_INCLUDED_POLY_ALLOCATOR_H

//--------------------------------------------------------------------------------
// poly_allocator.h
// 多態アロケータ【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_allocator_adapter.h>//アロケータアダプターインターフェース
#include <gasha/std_allocator.h>//標準アロケータ
#include <gasha/chrono.h>//時間処理ユーティリティ

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <functional>//C++11 std::function
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//多態アロケータ
//※アロケータの実装を隠ぺいして、共通インターフェースで様々なアロケータを利用できるようにする。
//※標準 new / delete の中身が、多態アロケータからのメモリ確保／解放に置き換わる。
//※多態アロケータにセットしたい既存のアロケータを、コンストラクタで受け渡して使用する。
//※デストラクタで変更前のアロケータに戻る。
//※他のスレッドには影響しない。
//※new / delete オペレータを直接使用してもよいが、GASHA_NEW / GASHA_DELETE マクロを使用すると、
//　アラインメントを保証し、かつ、デバッグ情報／デバッグコールバックを使用できるようになる。

//--------------------------------------------------------------------------------
//【注意】対策済みだが、TLSが初期化できない環境（0初期化しかできない環境）には注意。
//        ⇒懸念点：m_align の初期値が 0 ではない。
//        ⇒対策：ビルド設定で、プラットフォームに応じて GASHA_INCOMPLETE_TLS_INITIALIZER が設定され、
//                それに基づいて m_align の明示的な初期化を行う。
//--------------------------------------------------------------------------------

//--------------------
//アロケート用デバッグ情報
struct debugAllocationInfo
{
#ifdef GASHA_DEBUG_FEATURE_IS_ENABLED
	const char* m_fileName;//呼び出し元ソースファイル名
	const char* m_funcName;//呼び出し元関数名
	const char* m_cpName;//コールポイント名
	const char* m_criticalCpName;//重大コールポイント名
	GASHA_ sec_t m_time;//プログラム経過時間
	const char* m_typeName;//型名
	std::size_t m_typeSize;//型のサイズ
	std::size_t m_arrayNum;//配列サイズ
#endif//GASHA_DEBUG_FEATURE_IS_ENABLED

	//コンストラクタ
	inline debugAllocationInfo(const char* file_name, const char* func_name, const char* cp_name, const char* critical_cp_name, const GASHA_ sec_t time, const char* type_name, const std::size_t type_size, const std::size_t array_num);
};

//--------------------
//デバッグ用メモリアロケート観察者
enum newMethod_type//new時の状況
{
	methodOfNew,//new時
	methodOfNewArrays,//new[]時
};
enum deleteMethod_type//delete時の状況
{
	methodOfDelete,//delete時
	methodOfDeleteArrays,//delete[]時
};
struct debugAllocationObserver
{
	//new時のコールバック関数
	std::function<void(const iAllocatorAdapter& adapter, const void* p, std::size_t size, std::size_t align, const newMethod_type method, const debugAllocationInfo* info)> m_atNew;
	
	//delete時のコールバック関数
	std::function<void(const iAllocatorAdapter& adapter, const void* p, const deleteMethod_type method, const debugAllocationInfo* info)> m_atDelete;
	
	//アロケータアダプター変更時のコールバック関数 
	std::function<void(const iAllocatorAdapter& adapter, const iAllocatorAdapter& next_adapter)> m_atChangeAllocator;

	//アロケータアダプター復帰時のコールバック関数 
	std::function<void(const iAllocatorAdapter& adapter, const iAllocatorAdapter& prev_adapter)> m_atReturnAllocator;

	//コンストラクタ
	inline debugAllocationObserver();
};

//--------------------
//クラス宣言
namespace _private
{
	template<class T>
	struct newFunctor;
	struct deleteFunctor;
	struct deleteArrayFunctor;
};

//--------------------
//多態アロケータクラス
class polyAllocator
{
	friend void* ::operator new(const std::size_t size) GASHA_STDNEW_THROW;
	friend void* ::operator new[](const std::size_t size) GASHA_STDNEW_THROW;
	friend void* ::operator new(const std::size_t size, const std::nothrow_t&) GASHA_STDNEW_NOTHROW;
	friend void* ::operator new[](const std::size_t size, const std::nothrow_t&) GASHA_STDNEW_NOTHROW;
	friend void ::operator delete(void* p) GASHA_STDDELETE_THROW;
	friend void ::operator delete[](void* p) GASHA_STDDELETE_THROW;
	template<class T>
	friend struct GASHA_ _private::newFunctor;
	friend struct GASHA_ _private::deleteFunctor;
	friend struct GASHA_ _private::deleteArrayFunctor;

public:
	//アクセッサ
	inline const char* name() const;//アロケータ名
	inline const char* mode() const;//アロケータの実装モード名
	inline const GASHA_ iAllocatorAdapter* adapter() const;//アダプター
	inline GASHA_ iAllocatorAdapter* adapter();//アダプター

public:
	//オペレータ
	inline const GASHA_ iAllocatorAdapter& operator*() const;
	inline GASHA_ iAllocatorAdapter& operator*();
	inline const GASHA_ iAllocatorAdapter* operator->() const;
	inline GASHA_ iAllocatorAdapter* operator->();

public:
	//キャストオペレータ
	inline operator const GASHA_ iAllocatorAdapter&() const;
	inline operator GASHA_ iAllocatorAdapter&();

public:
	//デバッグ観察者を変更
	inline const GASHA_ debugAllocationObserver* debugObserver() const;
	//デバッグ観察者を更新
	inline void setDebugObserver(const GASHA_ debugAllocationObserver& observer) const;
	inline void resetDebugObserver() const;

private:
	//アライメントサイズを取得
	inline static std::size_t align();
	//アライメントサイズを変更
	inline static void setAlign(const std::size_t align);
	inline static void resetAlign();
	//デバッグ情報を取得
	inline static const GASHA_ debugAllocationInfo* debugInfo();
	//デバッグ情報を更新
	inline static void setDebugInfo(const GASHA_ debugAllocationInfo* info);
	inline static void resetDebugInfo();

private:
	//コールバック
	void callbackAtNew(void *p, std::size_t size, const GASHA_ newMethod_type method);
	void callbackAtDelete(void *p, const GASHA_ deleteMethod_type method);
	void callbackAtChangeAllocator(const GASHA_ iAllocatorAdapter& adapter, const GASHA_ iAllocatorAdapter& next_adapter);//アロケータアダプター変更時のコールバック
	void callbackAtReturnAllocator(const GASHA_ iAllocatorAdapter& adapter, const GASHA_ iAllocatorAdapter& prev_adapter);//アロケータアダプター復帰時のコールバック

public:
	//標準アロケータアダプターの強制初期化
	//※二重に呼び出されても問題ないので public
	void initlaizeStdAllocatorAdapter();

public:
	//コンストラクタ
	inline polyAllocator(GASHA_ iAllocatorAdapter& adapter);
	//デフォルトコンストラクタ
	//※現在設定されているアダプター、および、観察者を扱う。
	inline polyAllocator();
	//デストラクタ
	inline ~polyAllocator();

private:
	//フィールド
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
	GASHA_ iAllocatorAdapter* m_prevAdapter;//変更前のアロケータ
	const GASHA_ debugAllocationObserver* m_prevObserver;//変更前の観察者
	bool m_isChanged;//アロケータ変更時
	//静的フィールド
	static GASHA_ stdAllocator<> m_stdAllocator;//標準アロケータ
	static GASHA_ allocatorAdapter<GASHA_ stdAllocator<>> m_stdAllocatorAdapter;//標準アロケータアダプター
	static thread_local  GASHA_ iAllocatorAdapter* m_adapter;//現在のアロケータアダプター
	static thread_local const GASHA_ debugAllocationObserver* m_observer;//現在の観察者
	static thread_local std::size_t m_align;//現在のアラインメントサイズ（一時利用のみ）
	static thread_local const GASHA_ debugAllocationInfo* m_debugInfo;//現在のデバッグ情報（一時利用のみ）
#else//GASHA_ENABLE_POLY_ALLOCATOR
	static GASHA_ iAllocatorAdapter* m_dummyAdapter;//アロケータアダプターダミー
#endif//GASHA_ENABLE_POLY_ALLOCATOR
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/poly_allocator.inl>

#endif//GASHA_INCLUDED_POLY_ALLOCATOR_H

// End of file

﻿#pragma once
#ifndef GASHA_INCLUDED_POLY_ALLOCATOR_H
#define GASHA_INCLUDED_POLY_ALLOCATOR_H

//--------------------------------------------------------------------------------
// poly_allocator.h
// 多態アロケータ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_allocator_adapter.h>//アロケータアダプターインターフェース
#include <gasha/std_allocator.h>//標準アロケータ

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <functional> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <functional>//std::function

//【VC++】例外を無効化した状態で例外つきのnewをオーバーロードすると、warning C4290 が発生する
//warning C4290: C++ の例外の指定は無視されます。関数が __declspec(nothrow) でないことのみ表示されます。
#pragma warning(disable: 4290)//C4290を抑える

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

//--------------------
//アロケート用デバッグ情報
struct debugAllocationInfo
{
#ifdef GASHA_HAS_DEBUG_FEATURE
	const char* m_fileName;//ファイル名
	const char* m_funcName;//関数名
	const char* m_callPointName;//コールポイント名
	double m_time;//プログラム経過時間
	const char* m_typeName;//型名
	std::size_t m_typeSize;//型のサイズ
	std::size_t m_arrayNum;//配列サイズ
#endif//GASHA_HAS_DEBUG_FEATURE

	//コンストラクタ
	inline debugAllocationInfo(const char* file_name, const char* func_name, const char* call_point_name, const double time, const char* type_name, const std::size_t type_size, const std::size_t array_num);
};

//--------------------
//デバッグ用メモリアロケート観察者
enum newMethod_t//new時の状況
{
	methodOfNew,//new時
	methodOfNewArrays,//new[]時
};
enum deleteMethod_t//delete時の状況
{
	methodOfDelete,//delete時
	methodOfDeleteArrays,//delete[]時
};
struct debugAllocationObserver
{
	//new時のコールバック関数
	std::function<void(const IAllocatorAdapter& adapter, const void* p, std::size_t size, std::size_t align, const newMethod_t method, const debugAllocationInfo* info)> m_atNew;
	
	//delete時のコールバック関数
	std::function<void(const IAllocatorAdapter& adapter, const void* p, const deleteMethod_t method, const debugAllocationInfo* info)> m_atDelete;
	
	//アロケータアダプター変更時のコールバック関数 
	std::function<void(const IAllocatorAdapter& adapter, const IAllocatorAdapter& next_adapter)> m_atChangeAllocator;

	//アロケータアダプター復帰時のコールバック関数 
	std::function<void(const IAllocatorAdapter& adapter, const IAllocatorAdapter& prev_adapter)> m_atReturnAllocator;

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
	inline const GASHA_ IAllocatorAdapter* adapter() const;//アダプター
	inline GASHA_ IAllocatorAdapter* adapter();//アダプター

public:
	//オペレータ
	inline const GASHA_ IAllocatorAdapter& operator*() const;
	inline GASHA_ IAllocatorAdapter& operator*();
	inline const GASHA_ IAllocatorAdapter* operator->() const;
	inline GASHA_ IAllocatorAdapter* operator->();

public:
	//キャストオペレータ
	inline operator const GASHA_ IAllocatorAdapter&() const;
	inline operator GASHA_ IAllocatorAdapter&();

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
	//デバッグ情報を変更
	inline static const GASHA_ debugAllocationInfo* debugInfo();
	//デバッグ情報を更新
	inline static void setDebugInfo(const GASHA_ debugAllocationInfo* info);
	inline static void resetDebugInfo();

private:
	//コールバック
	void callbackAtNew(void *p, std::size_t size, const GASHA_ newMethod_t method);
	void callbackAtDelete(void *p, const GASHA_ deleteMethod_t method);
	void callbackAtChangeAllocator(const GASHA_ IAllocatorAdapter& adapter, const GASHA_ IAllocatorAdapter& next_adapter);//アロケータアダプター変更時のコールバック
	void callbackAtReturnAllocator(const GASHA_ IAllocatorAdapter& adapter, const GASHA_ IAllocatorAdapter& prev_adapter);//アロケータアダプター復帰時のコールバック

public:
	//コンストラクタ
	inline polyAllocator(GASHA_ IAllocatorAdapter& adapter);
	//デフォルトコンストラクタ
	//現在のアダプター、および、観察者を操作可能。
	//※標準アロケータアダプターの強制初期化（二重に呼び出されても問題ない）
	inline polyAllocator();
	void initlaizeStdAllocatorAdapter();
	//デストラクタ
	inline ~polyAllocator();

private:
	//フィールド
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
	GASHA_ IAllocatorAdapter* m_prevAdapter;//変更前のアロケータ
	const GASHA_ debugAllocationObserver* m_prevObserver;//変更前の観察者
	bool m_isChanged;//アロケータ変更時
	//静的フィールド
	static GASHA_ stdAllocator<> m_stdAllocator;//標準アロケータ
	static GASHA_ allocatorAdapter<GASHA_ stdAllocator<>> m_stdAllocatorAdapter;//標準アロケータアダプター
	thread_local static GASHA_ IAllocatorAdapter* m_adapter;//現在のアロケータアダプター
	thread_local static const GASHA_ debugAllocationObserver* m_observer;//現在の観察者
	thread_local static std::size_t m_align;//現在のアラインメントサイズ（一時利用のみ）
	thread_local static const GASHA_ debugAllocationInfo* m_debugInfo;//現在のデバッグ情報（一時利用のみ）
#else//GASHA_ENABLE_POLY_ALLOCATOR
	static GASHA_ IAllocatorAdapter* m_dummyAdapter;//アロケータアダプターダミー
#endif//GASHA_ENABLE_POLY_ALLOCATOR
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/poly_allocator.inl>

#endif//GASHA_INCLUDED_POLY_ALLOCATOR_H

// End of file

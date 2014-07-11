#pragma once
#ifndef GASHA_INCLUDED_NEW_INL
#define GASHA_INCLUDED_NEW_INL

//--------------------------------------------------------------------------------
// new.inl
// 多態アロケータ対応標準new/delete【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/new.h>//多態アロケータ対応標準new/delete【宣言部】

#include <gasha/poly_allocator.h>//多態アロケータ
#include <gasha/chrono.h>//時間系ユーティリティ

#include <utility>//C++11 std::forward

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new/配置delete用

//【VC++】アライメントが指定されたクラス／構造体を new すると、warning C4316 が発生する
//  warning C4316 : '(class_name)' : ヒープで割り当てられたオブジェクトが (16) にアラインメントされていない可能性があります
#pragma warning(disable: 4316)//C4316を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//【多態アロケータ適用版】標準new/delete
//--------------------------------------------------------------------------------
#ifdef GASHA_ENABLE_POLY_ALLOCATOR

namespace _private
{
	//--------------------
	//アライメント＋デバッグ情報付き new
	template<class T>
	template<typename... Tx>
	T* newFunctor<T>::operator()(Tx&& ...nx) const
	{
		const char* call_point_name = nullptr;// getCurrentCallPointNameDummy();//コールポイント名取得
		const double time = nowElapsedTime();//プログラム経過取得
		const GASHA_ debugAllocationInfo info(m_fileName, m_funcName, call_point_name, time, typeid(T).name(), sizeof(T), 0);//デバッグ情報生成
		GASHA_ polyAllocator allocator;
		allocator.setAlign(alignof(T));//アライメント
		allocator.setDebugInfo(&info);//デバッグ情報
		T* p = new T(std::forward<Tx>(nx)...);//メモリ確保
		allocator.resetAlign();//アライメントリセット
		allocator.resetDebugInfo();//デバッグ情報リセット
		return p;
	}
	template<class T>
	inline newFunctor<T>::newFunctor(const char* file_name, const char* func_name) :
		m_fileName(file_name),
		m_funcName(func_name)
	{}
	//※配列版
	template<class T, std::size_t N>
	T* newFunctor<T[N]>::operator()() const
	{
		const char* call_point_name = nullptr;// getCurrentCallPointNameDummy();//コールポイント名取得
		const double time = nowElapsedTime();//プログラム経過取得
		const GASHA_ debugAllocationInfo info(m_fileName, m_funcName, call_point_name, time, typeid(T).name(), sizeof(T), N);//デバッグ情報生成
		GASHA_ polyAllocator allocator;
		allocator.setAlign(alignof(T));//アライメント
		allocator.setDebugInfo(&info);//デバッグ情報
		T* p = new T[N];//メモリ確保
		allocator.resetAlign();//アライメントリセット
		allocator.resetDebugInfo();//デバッグ情報リセット
		return p;
	}
	template<class T, std::size_t N>
	inline newFunctor<T[N]>::newFunctor(const char* file_name, const char* func_name) :
		m_fileName(file_name),
		m_funcName(func_name)
	{}

	//--------------------
	//デバッグ情報付き delete
	template<typename T>
	void deleteFunctor::operator()(T* p) const
	{
		const char* call_point_name = nullptr;// getCurrentCallPointNameDummy();//コールポイント名取得
		const double time = nowElapsedTime();//プログラム経過取得
		const GASHA_ debugAllocationInfo info(m_fileName, m_funcName, call_point_name, time, typeid(T).name(), sizeof(T), 0);//デバッグ情報生成
		GASHA_ polyAllocator::setDebugInfo(&info);//デバッグ情報
		delete p;//メモリ破棄
		GASHA_ polyAllocator::resetDebugInfo();//デバッグ情報リセット
	}
	inline deleteFunctor::deleteFunctor(const char* file_name, const char* func_name) :
		m_fileName(file_name),
		m_funcName(func_name)
	{}
	//※配列版
	template<typename T>
	void deleteArrayFunctor::operator()(T* p) const
	{
		const char* call_point_name = nullptr;// getCurrentCallPointNameDummy();//コールポイント名取得
		const double time = nowElapsedTime();//プログラム経過取得
		const GASHA_ debugAllocationInfo info(m_fileName, m_funcName, call_point_name, time, typeid(T).name(), sizeof(T), 0);//デバッグ情報生成
		GASHA_ polyAllocator::setDebugInfo(&info);//デバッグ情報
		delete[] p;//メモリ破棄
		GASHA_ polyAllocator::resetDebugInfo();//デバッグ情報リセット
	}
	inline deleteArrayFunctor::deleteArrayFunctor(const char* file_name, const char* func_name) :
		m_fileName(file_name),
		m_funcName(func_name)
	{}
}

#endif//GASHA_ENABLE_POLY_ALLOCATOR

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_NEW_INL

// End of file

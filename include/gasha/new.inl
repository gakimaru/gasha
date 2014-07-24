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
#include <gasha/call_point.h>//コールポイント
#include <gasha/chrono.h>//時間処理ユーティリティ

#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//【多態アロケータ適用版】標準new/delete
//--------------------------------------------------------------------------------
#ifdef GASHA_ENABLE_POLY_ALLOCATOR

namespace _private
{
	//--------------------
	//コールポイント名取得
	inline void getCallpointName(const char*& cp_name, const char*& critical_cp_name)
	{
		GASHA_ callPoint cp;
		const GASHA_ callPoint* recent_cp = cp.find();
		if (recent_cp)
		{
			cp_name = recent_cp->name();
			if (recent_cp->type() == GASHA_ callPoint::isCritical)
				critical_cp_name = cp_name;
			else
			{
				recent_cp = cp.findCritical();
				if (recent_cp)
					critical_cp_name = recent_cp->name();
			}
		}
	}

	//--------------------
	//アライメント＋デバッグ情報付き new
	template<class T>
	template<typename... Tx>
	T* newFunctor<T>::operator()(Tx&& ...nx) const
	{
		const char* cp_name = nullptr;//コールポイント名
		const char* critical_cp_name = nullptr;//重大コールポイント名
		getCallpointName(cp_name, critical_cp_name);//コールポイント名取得
		const GASHA_ sec_t time = nowElapsedTime();//プログラム経過取得
		const GASHA_ debugAllocationInfo info(m_fileName, m_funcName, cp_name, critical_cp_name, time, typeid(T).name(), sizeof(T), 0);//デバッグ情報生成
		GASHA_ polyAllocator allocator;
		allocator.setAlign(alignof(T));//アライメント
		allocator.setDebugInfo(&info);//デバッグ情報
		
	#pragma warning(push)//【VC++】ワーニング設定を退避
	#pragma warning(disable: 4316)//【VC++】C4316を抑える
		T* p = new T(std::forward<Tx>(nx)...);//メモリ確保
	#pragma warning(pop)//【VC++】ワーニング設定を復元

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
		const char* cp_name = nullptr;//コールポイント名
		const char* critical_cp_name = nullptr;//重大コールポイント名
		getCallpointName(cp_name, critical_cp_name);//コールポイント名取得
		const GASHA_ sec_t time = nowElapsedTime();//プログラム経過取得
		const GASHA_ debugAllocationInfo info(m_fileName, m_funcName, cp_name, critical_cp_name, time, typeid(T).name(), sizeof(T), N);//デバッグ情報生成
		GASHA_ polyAllocator allocator;
		allocator.setAlign(alignof(T));//アライメント
		allocator.setDebugInfo(&info);//デバッグ情報
		
	#pragma warning(push)//【VC++】ワーニング設定を退避
	#pragma warning(disable: 4316)//【VC++】C4316を抑える
		T* p = new T[N];//メモリ確保
	#pragma warning(pop)//【VC++】ワーニング設定を復元
		
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
		const char* cp_name = nullptr;//コールポイント名
		const char* critical_cp_name = nullptr;//重大コールポイント名
		getCallpointName(cp_name, critical_cp_name);//コールポイント名取得
		const GASHA_ sec_t time = nowElapsedTime();//プログラム経過取得
		const GASHA_ debugAllocationInfo info(m_fileName, m_funcName, cp_name, critical_cp_name, time, typeid(T).name(), sizeof(T), 0);//デバッグ情報生成
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
		const char* cp_name = nullptr;//コールポイント名
		const char* critical_cp_name = nullptr;//重大コールポイント名
		getCallpointName(cp_name, critical_cp_name);//コールポイント名取得
		const GASHA_ sec_t time = nowElapsedTime();//プログラム経過取得
		const GASHA_ debugAllocationInfo info(m_fileName, m_funcName, cp_name, critical_cp_name, time, typeid(T).name(), sizeof(T), 0);//デバッグ情報生成
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

#endif//GASHA_INCLUDED_NEW_INL

// End of file

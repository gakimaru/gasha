﻿#pragma once
#ifndef GASHA_INCLUDED_NEW_H
#define GASHA_INCLUDED_NEW_H

//--------------------------------------------------------------------------------
// new.h
// 多態アロケータ対応標準new/delete【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <new>//配置new/配置delete用
#pragma warning(pop)//【VC++】ワーニング設定を復元

//--------------------------------------------------------------------------------
//【多態アロケータ適用版】標準new/delete
//--------------------------------------------------------------------------------
#ifdef GASHA_ENABLE_POLY_ALLOCATOR

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4290)//【VC++】C4290を抑える
void* operator new(const std::size_t size) GASHA_STDNEW_THROW;//new
void* operator new[](const std::size_t size) GASHA_STDNEW_THROW;//配列new
void* operator new(const std::size_t size, const std::nothrow_t&) GASHA_STDNEW_NOTHROW;//new
void* operator new[](const std::size_t size, const std::nothrow_t&) GASHA_STDNEW_NOTHROW;//配列new
void operator delete(void* p) GASHA_STDDELETE_THROW;//delete
void operator delete[](void* p) GASHA_STDDELETE_THROW;//配列版delete
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace _private
{
	//--------------------
	//アライメント＋デバッグ情報付き new
	template<class T>
	struct newFunctor
	{
		template<typename... Tx>
		T* operator()(Tx&& ...nx) const;
		inline newFunctor(const char* file_name, const char* func_name);
		const char* m_fileName;//呼び出し元ソースファイル名
		const char* m_funcName;//呼び出し元関数名
	};
	//※配列版
	//※【問題点】VC++の場合、32バイトアラインメント以上でデストラクタ付きのクラス／構造体の確保は正常に動作しないので注意
	//　　　　　　（なぜかVC++の計算では、配列要素数の格納領域のサイズが16バイト以上にならないため、32バイト以上のアラインメントに適合できなくなる。）
	template<class T, std::size_t N>
	struct newFunctor<T[N]>
	{
		T* operator()() const;
		inline newFunctor(const char* file_name, const char* func_name);
		const char* m_fileName;//呼び出し元ソースファイル名
		const char* m_funcName;//呼び出し元関数名
	};

	//--------------------
	//デバッグ情報付き delete
	struct deleteFunctor
	{
		template<typename T>
		void operator()(T* p) const;
		inline deleteFunctor(const char* file_name, const char* func_name);
		const char* m_fileName;//呼び出し元ソースファイル名
		const char* m_funcName;//呼び出し元関数名
	};
	//※配列版
	struct deleteArrayFunctor
	{
		template<typename T>
		void operator()(T* p) const;
		inline deleteArrayFunctor(const char* file_name, const char* func_name);
		const char* m_fileName;//呼び出し元ソースファイル名
		const char* m_funcName;//呼び出し元関数名
	};
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//--------------------
//newマクロ
#define GASHA_NEW(T, ...) GASHA_ _private::newFunctor<T>(GASHA_SRC_FILE_LINE_TIME(), GASHA_FUNC_NAME())(__VA_ARGS__)//new or new[]
#define GASHA_NEW_ARRAY(T, N) GASHA_ _private::newFunctor<T[N]>(GASHA_SRC_FILE_LINE_TIME(), GASHA_FUNC_NAME())()//new[]

//--------------------
//deleteマクロ
#define GASHA_DELETE(p) GASHA_ _private::deleteFunctor(GASHA_SRC_FILE_LINE_TIME(), GASHA_FUNC_NAME())(p)//delete
#define GASHA_DELETE_ARRAY(p) GASHA_ _private::deleteArrayFunctor(GASHA_SRC_FILE_LINE_TIME(), GASHA_FUNC_NAME())(p)//delete[]

#else//GASHA_ENABLE_POLY_ALLOCATOR

//--------------------
//newマクロ
#define GASHA_NEW(T, ...) new T(__VA_ARGS__)
#define GASHA_NEW_ARRAY(T, N, ...) new T[N](__VA_ARGS__)

//--------------------
//deleteマクロ
#define GASHA_DELETE(p) delete p;
#define GASHA_DELETE_ARRAY(p) delete[] p

#endif//GASHA_ENABLE_POLY_ALLOCATOR

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/new.inl>

//利便性のためにいっしょにインクルード
#include <gasha/poly_allocator.h>//多態アロケータ
#include <gasha/std_allocator.h>//標準アロケータ

#endif//GASHA_INCLUDED_NEW_H

// End of file

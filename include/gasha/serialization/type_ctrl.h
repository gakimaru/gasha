#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_TYPE_CTRL_H
#define GASHA_INCLUDED_SERIALIZATION_TYPE_CTRL_H

//--------------------------------------------------------------------------------
// serialization/type_ctrl.h
// シリアライズ/型操作クラス【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/type_ctrl_base.h>//シリアライズ/型操作基底クラス

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/型操作クラス
	//--------------------------------------------------------------------------------

	//--------------------
	//型操作テンプレートクラス
	template<typename T>
	class typeCtrl : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};
	//--------------------
	//型操作テンプレートクラス：int型に特殊化
	template<>
	class typeCtrl<int> : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		inline static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		inline static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		inline static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		inline static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t data_size_max);
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};
	//--------------------
	//型操作テンプレートクラス：unsigned int型に特殊化
	template<>
	class typeCtrl<unsigned int> : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		inline static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		inline static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		inline static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		inline static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max);
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};
	//--------------------
	//型操作テンプレートクラス：long型に特殊化
	template<>
	class typeCtrl<long> : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		inline static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		inline static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		inline static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		inline static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t data_size_max);
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};
	//--------------------
	//型操作テンプレートクラス：unsigned long型に特殊化
	template<>
	class typeCtrl<unsigned long> : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		inline static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		inline static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		inline static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		inline static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max);
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};
	//--------------------
	//型操作テンプレートクラス：long long型に特殊化
	template<>
	class typeCtrl<long long> : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		inline static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		inline static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		inline static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		inline static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t data_size_max);
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};
	//--------------------
	//型操作テンプレートクラス：unsigned long long型に特殊化
	template<>
	class typeCtrl<unsigned long long> : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		inline static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		inline static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		inline static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		inline static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max);
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};
	//--------------------
	//型操作テンプレートクラス：short型に特殊化
	template<>
	class typeCtrl<short> : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		inline static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		inline static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		inline static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		inline static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max);
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};
	//--------------------
	//型操作テンプレートクラス：unsigned short型に特殊化
	template<>
	class typeCtrl<unsigned short> : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		inline static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		inline static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		inline static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		inline static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max);
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};
	//--------------------
	//型操作テンプレートクラス：char型に特殊化
	template<>
	class typeCtrl<char> : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		inline static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		inline static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		inline static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		inline static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max);
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};
	//--------------------
	//型操作テンプレートクラス：unsigned char型に特殊化
	template<>
	class typeCtrl<unsigned char> : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		inline static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		inline static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		inline static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		inline static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max);
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};
	//--------------------
	//型操作テンプレートクラス：float型に特殊化
	template<>
	class typeCtrl<float> : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		inline static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		inline static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		inline static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max);
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};
	//--------------------
	//型操作テンプレートクラス：double型に特殊化
	template<>
	class typeCtrl<double> : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		inline static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		inline static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		inline static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max);
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};
	//--------------------
	//型操作テンプレートクラス：バイナリ型（bin_type型）に特殊化
	template<>
	class typeCtrl<GASHA_ serialization::bin_type> : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		inline static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		inline static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		inline static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		inline static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max);
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};
	//--------------------
	//型操作テンプレートクラス：文字列型（str_type型）に特殊化
	template<>
	class typeCtrl<GASHA_ serialization::str_type> : public GASHA_ serialization::typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		inline static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		inline static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		inline static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max);
	public:
		//コンストラクタ
		inline typeCtrl();
		//デストラクタ
		inline ~typeCtrl();
	};

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/serialization/type_ctrl.inl>

#endif//GASHA_INCLUDED_SERIALIZATION_TYPE_CTRL_H

// End of file

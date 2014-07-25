#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_TYPE_CTRL_INL
#define GASHA_INCLUDED_SERIALIZATION_TYPE_CTRL_INL

//--------------------------------------------------------------------------------
// serialization/type_ctrl.inl
// シリアライズ/型操作クラス【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/type_ctrl.h>//シリアライズ/型操作【宣言部】

#include <gasha/string.h>//文字列処理

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
	
	//コンストラクタ
	template<typename T>
	inline typeCtrl<T>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase()
	{}
	//デストラクタ
	template<typename T>
	inline typeCtrl<T>::~typeCtrl()
	{}
	
	//--------------------
	//型操作テンプレートクラス：int型に特殊化

	//メモリへコピー
	inline std::size_t typeCtrl<int>::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::toMemWithConv(mem, mem_size, value_p, value_size);
	}
	//メモリからコピー
	inline std::size_t typeCtrl<int>::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::fromMemWithConv(mem, mem_size, value_p, value_size);
	}
	//文字列へ変換
	inline std::size_t typeCtrl<int>::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		const int value = *reinterpret_cast<const int*>(value_p);
		std::size_t pos = 0;
		return GASHA_ spprintf(str, str_max, pos, "%d", value);
	}
	//文字列から変換
	inline std::size_t typeCtrl<int>::fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t data_size_max)
	{
		//未実装
		return 0;
	}
	//コンストラクタ
	inline typeCtrl<int>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase(toMem, fromMem, toStr, fromStr)
	{}
	//デストラクタ
	inline typeCtrl<int>::~typeCtrl()
	{}

	//--------------------
	//型操作テンプレートクラス：unsigned int型に特殊化

	//メモリへコピー
	inline std::size_t typeCtrl<unsigned int>::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::toMemWithConv(mem, mem_size, value_p, value_size);
	}
	//メモリからコピー
	inline std::size_t typeCtrl<unsigned int>::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::fromMemWithConv(mem, mem_size, value_p, value_size);
	}
	//文字列へ変換
	inline std::size_t typeCtrl<unsigned int>::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		const unsigned int value = *reinterpret_cast<const unsigned int*>(value_p);
		std::size_t pos = 0;
		return GASHA_ spprintf(str, str_max, pos, "0x%08x", value);
	}
	//文字列から変換
	inline std::size_t typeCtrl<unsigned int>::fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max)
	{
		//未実装
		return 0;
	}
	//コンストラクタ
	inline typeCtrl<unsigned int>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase(toMem, fromMem, toStr, fromStr)
	{}
	//デストラクタ
	inline typeCtrl<unsigned int>::~typeCtrl()
	{}

	//--------------------
	//型操作テンプレートクラス：long型に特殊化

	//メモリへコピー
	inline std::size_t typeCtrl<long>::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::toMemWithConv(mem, mem_size, value_p, value_size);
	}
	//メモリからコピー
	inline std::size_t typeCtrl<long>::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::fromMemWithConv(mem, mem_size, value_p, value_size);
	}
	//文字列へ変換
	inline std::size_t typeCtrl<long>::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		const long value = *reinterpret_cast<const long*>(value_p);
		std::size_t pos = 0;
		return GASHA_ spprintf(str, str_max, pos, "%ld", value);
	}
	//文字列から変換
	inline std::size_t typeCtrl<long>::fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t data_size_max)
	{
		//未実装
		return 0;
	}
	//コンストラクタ
	inline typeCtrl<long>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase(toMem, fromMem, toStr, fromStr)
	{}
	//デストラクタ
	inline typeCtrl<long>::~typeCtrl()
	{}

	//--------------------
	//型操作テンプレートクラス：unsigned long型に特殊化

	//メモリへコピー
	inline std::size_t typeCtrl<unsigned long>::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::toMemWithConv(mem, mem_size, value_p, value_size);
	}
	//メモリからコピー
	inline std::size_t typeCtrl<unsigned long>::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::fromMemWithConv(mem, mem_size, value_p, value_size);
	}
	//文字列へ変換
	inline std::size_t typeCtrl<unsigned long>::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		const unsigned long value = *reinterpret_cast<const unsigned long*>(value_p);
		std::size_t pos = 0;
		return GASHA_ spprintf(str, str_max, pos, "0x%08lx", value);
	}
	//文字列から変換
	inline std::size_t typeCtrl<unsigned long>::fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max)
	{
		//未実装
		return 0;
	}
	//コンストラクタ
	inline typeCtrl<unsigned long>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase(toMem, fromMem, toStr, fromStr)
	{}
	//デストラクタ
	inline typeCtrl<unsigned long>::~typeCtrl()
	{}

	//--------------------
	//型操作テンプレートクラス：long long型に特殊化

	//メモリへコピー
	inline std::size_t typeCtrl<long long>::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::toMemWithConv(mem, mem_size, value_p, value_size);
	}
	//メモリからコピー
	inline std::size_t typeCtrl<long long>::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::fromMemWithConv(mem, mem_size, value_p, value_size);
	}
	//文字列へ変換
	inline std::size_t typeCtrl<long long>::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		const long long value = *reinterpret_cast<const long long*>(value_p);
		std::size_t pos = 0;
		return GASHA_ spprintf(str, str_max, pos, "%lld", value);
	}
	//文字列から変換
	inline std::size_t typeCtrl<long long>::fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t data_size_max)
	{
		//未実装
		return 0;
	}
	//コンストラクタ
	inline typeCtrl<long long>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase(toMem, fromMem, toStr, fromStr)
	{}
	//デストラクタ
	inline typeCtrl<long long>::~typeCtrl()
	{}

	//--------------------
	//型操作テンプレートクラス：unsigned long long型に特殊化

	//メモリへコピー
	inline std::size_t typeCtrl<unsigned long long>::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::toMemWithConv(mem, mem_size, value_p, value_size);
	}
	//メモリからコピー
	inline std::size_t typeCtrl<unsigned long long>::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::fromMemWithConv(mem, mem_size, value_p, value_size);
	}
	//文字列へ変換
	inline std::size_t typeCtrl<unsigned long long>::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		const unsigned long long value = *reinterpret_cast<const unsigned long long*>(value_p);
		std::size_t pos = 0;
		return GASHA_ spprintf(str, str_max, pos, "0x%016llx", value);
	}
	//文字列から変換
	inline std::size_t typeCtrl<unsigned long long>::fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max)
	{
		//未実装
		return 0;
	}
	//コンストラクタ
	inline typeCtrl<unsigned long long>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase(toMem, fromMem, toStr, fromStr)
	{}
	//デストラクタ
	inline typeCtrl<unsigned long long>::~typeCtrl()
	{}

	//--------------------
	//型操作テンプレートクラス：short型に特殊化

	//メモリへコピー
	inline std::size_t typeCtrl<short>::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::toMemWithConv(mem, mem_size, value_p, value_size);
	}
	//メモリからコピー
	inline std::size_t typeCtrl<short>::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::fromMemWithConv(mem, mem_size, value_p, value_size);
	}
	//文字列へ変換
	inline std::size_t typeCtrl<short>::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		const short value = *reinterpret_cast<const short*>(value_p);
		std::size_t pos = 0;
		return GASHA_ spprintf(str, str_max, pos, "%d", value);
	}
	//文字列から変換
	inline std::size_t typeCtrl<short>::fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max)
	{
		//未実装
		return 0;
	}
	//コンストラクタ
	inline typeCtrl<short>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase(toMem, fromMem, toStr, fromStr)
	{}
	//デストラクタ
	inline typeCtrl<short>::~typeCtrl()
	{}

	//--------------------
	//型操作テンプレートクラス：unsigned short型に特殊化

	//メモリへコピー
	inline std::size_t typeCtrl<unsigned short>::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::toMemWithConv(mem, mem_size, value_p, value_size);
	}
	//メモリからコピー
	inline std::size_t typeCtrl<unsigned short>::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::fromMemWithConv(mem, mem_size, value_p, value_size);
	}
	//文字列へ変換
	inline std::size_t typeCtrl<unsigned short>::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		const unsigned short value = *reinterpret_cast<const unsigned short*>(value_p);
		std::size_t pos = 0;
		return GASHA_ spprintf(str, str_max, pos, "0x%04x", value);
	}
	//文字列から変換
	inline std::size_t typeCtrl<unsigned short>::fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max)
	{
		//未実装
		return 0;
	}
	//コンストラクタ
	inline typeCtrl<unsigned short>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase(toMem, fromMem, toStr, fromStr)
	{}
	//デストラクタ
	inline typeCtrl<unsigned short>::~typeCtrl()
	{}

	//--------------------
	//型操作テンプレートクラス：char型に特殊化

	//メモリへコピー
	inline std::size_t typeCtrl<char>::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::toMemWithConv(mem, mem_size, value_p, value_size);
	}
	//メモリからコピー
	inline std::size_t typeCtrl<char>::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::fromMemWithConv(mem, mem_size, value_p, value_size);
	}
	//文字列へ変換
	inline std::size_t typeCtrl<char>::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		const char value = *reinterpret_cast<const char*>(value_p);
		std::size_t pos = 0;
		return GASHA_ spprintf(str, str_max, pos, "%d", value);
	}
	//文字列から変換
	inline std::size_t typeCtrl<char>:: fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max)
	{
		//未実装
		return 0;
	}
	//コンストラクタ
	inline typeCtrl<char>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase(toMem, fromMem, toStr, fromStr)
	{}
	//デストラクタ
	inline typeCtrl<char>::~typeCtrl()
	{}

	//--------------------
	//型操作テンプレートクラス：unsigned char型に特殊化

	//メモリへコピー
	inline std::size_t typeCtrl<unsigned char>::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::toMemWithConv(mem, mem_size, value_p, value_size);
	}
	//メモリからコピー
	inline std::size_t typeCtrl<unsigned char>::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::fromMemWithConv(mem, mem_size, value_p, value_size);
	}
	//文字列へ変換
	inline std::size_t typeCtrl<unsigned char>::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		const unsigned char value = *reinterpret_cast<const unsigned char*>(value_p);
		std::size_t pos = 0;
		return GASHA_ spprintf(str, str_max, pos, "0x%02x", value);
	}
	//文字列から変換
	inline std::size_t typeCtrl<unsigned char>::fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max)
	{
		//未実装
		return 0;
	}
	//コンストラクタ
	inline typeCtrl<unsigned char>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase(toMem, fromMem, toStr, fromStr)
	{}
	//デストラクタ
	inline typeCtrl<unsigned char>::~typeCtrl()
	{}

	//--------------------
	//型操作テンプレートクラス：float型に特殊化

	//メモリへコピー
	inline std::size_t typeCtrl<float>::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		//return GASHA_ serialization::typeCtrlBase::toMemWithConv(mem, mem_size, value_p, value_size);
		return GASHA_ serialization::typeCtrlBase::toMem(mem, mem_size, value_p, value_size);//エンディアン調整なしでコピー
	}
	//文字列へ変換
	inline std::size_t typeCtrl<float>::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		const float value = *reinterpret_cast<const float*>(value_p);
		std::size_t pos = 0;
		return GASHA_ spprintf(str, str_max, pos, "%f", value);
	}
	//文字列から変換
	inline std::size_t typeCtrl<float>::fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max)
	{
		//未実装
		return 0;
	}
	//コンストラクタ
	inline typeCtrl<float>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase(toMem, fromMem, toStr, fromStr)
	{}
	//デストラクタ
	inline typeCtrl<float>::~typeCtrl()
	{}

	//--------------------
	//型操作テンプレートクラス：double型に特殊化

	//メモリへコピー
	inline std::size_t typeCtrl<double>::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		//return GASHA_ serialization::typeCtrlBase::toMemWithConv(mem, mem_size, value_p, value_size);
		return GASHA_ serialization::typeCtrlBase::toMem(mem, mem_size, value_p, value_size);//エンディアン調整なしでコピー
	}
	//文字列へ変換
	inline std::size_t typeCtrl<double>::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		const double value = *reinterpret_cast<const double*>(value_p);
		std::size_t pos = 0;
		return GASHA_ spprintf(str, str_max, pos, "%lf", value);
	}
	//文字列から変換
	inline std::size_t typeCtrl<double>::fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max)
	{
		//未実装
		return 0;
	}
	//コンストラクタ
	inline typeCtrl<double>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase(toMem, fromMem, toStr, fromStr)
	{}
	//デストラクタ
	inline typeCtrl<double>::~typeCtrl()
	{}

	//--------------------
	//型操作テンプレートクラス：バイナリ型（bin_type型）に特殊化

	//メモリへコピー
	inline std::size_t typeCtrl<GASHA_ serialization::bin_type>::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::toMem(mem, mem_size, value_p, value_size);
	}
	//メモリからコピー
	inline std::size_t typeCtrl<GASHA_ serialization::bin_type>::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::fromMem(mem, mem_size, value_p, value_size);
	}
	//文字列へ変換
	inline std::size_t typeCtrl<GASHA_ serialization::bin_type>::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::toStr(str, str_max, value_p, value_size);
	}
	//文字列から変換
	inline std::size_t typeCtrl<GASHA_ serialization::bin_type>::fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max)
	{
		//未実装
		return 0;
	}
	//コンストラクタ
	inline typeCtrl<GASHA_ serialization::bin_type>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase(toMem, fromMem, toStr, fromStr)
	{}
	//デストラクタ
	inline typeCtrl<GASHA_ serialization::bin_type>::~typeCtrl()
	{}

	//--------------------
	//型操作テンプレートクラス：文字列型（str_type型）に特殊化

	//メモリへコピー
	inline std::size_t typeCtrl<GASHA_ serialization::str_type>::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::toMem(mem, mem_size, value_p, value_size);
	}
	//メモリからコピー
	inline std::size_t typeCtrl<GASHA_ serialization::str_type>::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		return GASHA_ serialization::typeCtrlBase::fromMem(mem, mem_size, value_p, value_size);
	}
	//文字列から変換
	inline std::size_t typeCtrl<GASHA_ serialization::str_type>::fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max)
	{
		//未実装
		return 0;
	}
	//コンストラクタ
	inline typeCtrl<GASHA_ serialization::str_type>::typeCtrl() :
		GASHA_ serialization::typeCtrlBase(toMem, fromMem, toStr, fromStr)
	{}
	//デストラクタ
	inline typeCtrl<GASHA_ serialization::str_type>::~typeCtrl()
	{}

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SERIALIZATION_TYPE_CTRL_INL

// End of file

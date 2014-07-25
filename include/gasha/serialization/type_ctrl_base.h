#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_TYPE_CTRL_BASE_H
#define GASHA_INCLUDED_SERIALIZATION_TYPE_CTRL_BASE_H

//--------------------------------------------------------------------------------
// serialization/type_ctrl_base.h
// シリアライズ/型操作基底クラス【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/型操作基底クラス
	//--------------------------------------------------------------------------------

	//--------------------
	//汎用データ型指定用構造体
	struct str_type{};//文字列型
	struct bin_type{};//バイナリ型
	
	//--------------------
	//型操作基底クラス
	typedef std::size_t (*toMemFunc_type)(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);//メモリへコピー関数型
	typedef std::size_t (*fromMemFunc_type)(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);//メモリからコピー関数型
	typedef std::size_t (*toStrFunc_type)(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);//文字列へ変換関数型
	typedef std::size_t (*fromStrFunc_type)(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max);//文字列から変換関数型
	class typeCtrlBase
	{
	public:
		//メソッド
		//メモリへコピー
		static std::size_t toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//※エンディアン調整版
		static std::size_t toMemWithConv(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size);
		//メモリからコピー
		inline static std::size_t fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//※エンディアン調整版
		inline static std::size_t fromMemWithConv(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size);
		//文字列へ変換
		static std::size_t toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size);
		//文字列から変換
		//※未実装
		static std::size_t fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max);
	public:
		//デフォルトコンストラクタ
		inline typeCtrlBase();
		//コピーコンストラクタ
		inline typeCtrlBase(const typeCtrlBase& src);
		//コンストラクタ
		inline typeCtrlBase(toMemFunc_type to_mem_func_p, fromMemFunc_type from_mem_func_p, toStrFunc_type to_func_p, fromStrFunc_type from_func_p);
		//デストラクタ
		inline ~typeCtrlBase();
	public:
		//フィールド
		toMemFunc_type m_toMemFuncP;//メモリへコピー関数
		fromMemFunc_type m_fromMemFuncP;//メモリからコピー関数
		toStrFunc_type m_toStrFuncP;//文字列へ変換関数
		fromStrFunc_type m_fromStrFuncP;//文字列から変換関数
	};

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/serialization/type_ctrl_base.inl>

#endif//GASHA_INCLUDED_SERIALIZATION_TYPE_CTRL_BASE_H

// End of file

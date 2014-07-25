#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_TYPE_CTRL_BASE_INL
#define GASHA_INCLUDED_SERIALIZATION_TYPE_CTRL_BASE_INL

//--------------------------------------------------------------------------------
// serialization/type_ctrl_base.inl
// シリアライズ/型操作基底クラス【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/type_ctrl_base.h>//シリアライズ/型操作基底クラス【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/型操作基底クラス
	//--------------------------------------------------------------------------------

	//メモリからコピー
	inline std::size_t typeCtrlBase::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		return toMem(value_p, value_size, mem, mem_size);
	}
	//※エンディアン調整版
	inline std::size_t typeCtrlBase::fromMemWithConv(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		return toMemWithConv(value_p, value_size, mem, mem_size);
	}

	//デフォルトコンストラクタ
	inline typeCtrlBase::typeCtrlBase() :
		m_toMemFuncP(toMem),
		m_fromMemFuncP(fromMem),
		m_toStrFuncP(toStr),
		m_fromStrFuncP(fromStr)
	{}
	//コピーコンストラクタ
	inline typeCtrlBase::typeCtrlBase(const typeCtrlBase& src) :
		m_toMemFuncP(src.m_toMemFuncP),
		m_fromMemFuncP(src.m_fromMemFuncP),
		m_toStrFuncP(src.m_toStrFuncP),
		m_fromStrFuncP(src.m_fromStrFuncP)
	{}
	//コンストラクタ
	inline typeCtrlBase::typeCtrlBase(toMemFunc_type to_mem_func_p, fromMemFunc_type from_mem_func_p, toStrFunc_type to_func_p, fromStrFunc_type from_func_p) :
		m_toMemFuncP(to_mem_func_p),
		m_fromMemFuncP(from_mem_func_p),
		m_toStrFuncP(to_func_p),
		m_fromStrFuncP(from_func_p)
	{}
	//デストラクタ
	inline typeCtrlBase::~typeCtrlBase()
	{}

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SERIALIZATION_TYPE_CTRL_BASE_INL

// End of file

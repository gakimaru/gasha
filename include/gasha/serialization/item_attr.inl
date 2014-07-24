#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_ITEM_ATTR_INL
#define GASHA_INCLUDED_SERIALIZATION_ITEM_ATTR_INL

//--------------------------------------------------------------------------------
// serialization/item_attr.inl
// シリアライズ/データ項目属性【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/item_attr.h>//シリアライズ/データ項目属性【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/データ項目属性
	//--------------------------------------------------------------------------------

	//--------------------
	//データ項目属性

	//アクセッサ
	inline bool itemAttr::isObj() const//オブジェクト型か？
	{
		return (m_value & IS_OBJECT) ? true : false;
	}
	inline bool itemAttr::isArr() const//配列型か？
	{
		return (m_value & IS_ARRAY) ? true : false;
	}
	inline bool itemAttr::isPtr() const//ポインタ型か？
	{
		return (m_value & IS_PTR) ? true : false;
	}
	inline bool itemAttr::isNul() const//ヌルポインタか？（ポインタ型の時だけ扱われる）
	{
		return (m_value & IS_NULL) ? true : false;
	}
	inline bool itemAttr::isVLen() const//可変長か？（主に文字列ポインタの時に扱われる）
	{
		return (m_value & IS_VLEN) ? true : false;
	}
	inline bool itemAttr::hasVer() const//バージョン情報があるか？
	{
		return (m_value & HAS_VER) ? true : false;
	}
	inline void itemAttr::setHasVer() const//バージョン情報ありにする
	{
		*const_cast<value_type*>(&m_value) = (m_value | HAS_VER);
	}
	inline void itemAttr::resetHasVer() const//バージョン情報なしにする
	{
		*const_cast<value_type*>(&m_value) = (m_value & ~HAS_VER);
	}

	//クリア
	inline void itemAttr::clear()
	{
		*const_cast<value_type*>(&m_value) = 0;
	}

	//ムーブオペレータ
	inline itemAttr& itemAttr::operator=(itemAttr&& src)
	{
		*const_cast<value_type*>(&m_value) = src.m_value;
		return *this;
	}

	//コピーオペレータ
	inline itemAttr& itemAttr::operator=(const itemAttr& src)
	{
		*const_cast<value_type*>(&m_value) = src.m_value;
		return *this;
	}

	//ムーブコンストラクタ
	inline itemAttr::itemAttr(itemAttr&& obj) :
		m_value(obj.m_value)
	{}

	//コピーコンストラクタ
	inline itemAttr::itemAttr(const itemAttr& obj) :
		m_value(obj.m_value)
	{}

	//コンストラクタ
	inline itemAttr::itemAttr(const value_type value) :
		m_value(value)
	{}
	inline itemAttr::itemAttr(const bool is_object, const bool is_array, const bool is_ptr, const bool is_null, const bool is_vlen) :
		m_value(
			(is_object ? IS_OBJECT : 0) |
			(is_array ? IS_ARRAY : 0) |
			(is_ptr ? IS_PTR : 0) |
			(is_ptr && is_null ? IS_NULL : 0) |
			(is_vlen ? IS_VLEN : 0)
		)
	{}
	//デフォルトコンストラクタ
	inline itemAttr::itemAttr() :
		m_value(0)
	{}

	//デストラクタ
	inline itemAttr::~itemAttr()
	{}

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SERIALIZATION_ITEM_ATTR_INL

// End of file

#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_ITEM_ATTR_H
#define GASHA_INCLUDED_SERIALIZATION_ITEM_ATTR_H

//--------------------------------------------------------------------------------
// serialization/item_attr.h
// シリアライズ/データ項目属性【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

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
	enum itemAttr_type : unsigned char
	{
		IS_OBJECT = 0x01,//オブジェクト型
		IS_ARRAY = 0x02,//配列型
		IS_PTR = 0x04,//ポインタ型
		IS_NULL = 0x08,//ヌル
		IS_VLEN = 0x10,//可変長か？
		HAS_VER = 0x20,//バージョン情報あり
	};
	class itemAttr
	{
	public:
		//型
		typedef unsigned char value_type;//属性型
	public:
		//アクセッサ
		inline bool isObj() const;//オブジェクト型か？
		inline bool isArr() const;//配列型か？
		inline bool isPtr() const;//ポインタ型か？
		inline bool isNul() const;//ヌルポインタか？（ポインタ型の時だけ扱われる）
		inline bool isVLen() const;//可変長か？（主に文字列ポインタの時に扱われる）
		inline bool hasVer() const;//バージョン情報があるか？
		inline void setHasVer() const;//バージョン情報ありにする
		inline void resetHasVer() const;//バージョン情報なしにする
	public:
		//比較オペレータ
		inline bool operator==(const itemAttr& rhs) const { return m_value == rhs.m_value; }
		inline bool operator!=(const itemAttr& rhs) const { return m_value != rhs.m_value; }
	public:
		//メソッド
		//クリア
		inline void clear();
	public:
		//ムーブオペレータ
		inline itemAttr& operator=(itemAttr&& src);
		//コピーオペレータ
		inline itemAttr& operator=(const itemAttr& src);
	public:
		//ムーブコンストラクタ
		inline itemAttr(itemAttr&& obj);
		//コピーコンストラクタ
		inline itemAttr(const itemAttr& obj);
		//コンストラクタ
		inline itemAttr(const value_type value);
		inline itemAttr(const bool is_object, const bool is_array, const bool is_ptr, const bool is_null, const bool is_vlen);
		//デストラクタ
		inline ~itemAttr();
	public://直接アクセス許可
		//フィールド
		const value_type m_value;//属性
	};

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/serialization/item_attr.inl>

#endif//GASHA_INCLUDED_SERIALIZATION_ITEM_ATTR_H

// End of file

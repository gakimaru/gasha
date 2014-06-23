#pragma once
#ifndef __TAGGED_PTR_H_
#define __TAGGED_PTR_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// tagged_ptr.h
// タグ付きポインタ
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstddef>//std::size_t
#include <cstdint>//std::uint32_t, std::uint64_t

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//タグ付きポインタ型
//※POD型を保証するためにコンストラクタやコピーオペレータを用意しない
//※POD型について：http://ja.wikipedia.org/wiki/C++11#Plain_Old_Data_.E5.9E.8B.E3.81.AE.E5.AE.9A.E7.BE.A9.E3.81.AE.E4.BF.AE.E6.AD.A3
//
//【タグ付きポインタ設定内容】
//  taggedPtr<データ型, タグのビット数, タグのシフトビット数, ポインターのシフトビット数 = 0, タグのデータ型 = std::uint32_t, 値（ポインター＋タグ）のデータ型 = std::uint64_t>
//
//【タグ付きポインタ設定例①】※デフォルト（ポインターが最低限4バイトアラインメントであることが前提）
//  ポインタ=64bit, タグ=下位2bitの場合  ... taggedPtr<data_t, 2, 0> var;//タグのビット数=2, タグのシフトビット数=0
//
//【タグ付きポインタ設定例②】※32bitを前提に、安全性の高い方法
//  ポインタ=32bit, タグ=上位32bitの場合 ... taggedPtr<data_t, 32, 32> var;//タグのビット数=32, タグのシフトビット数=32
//
//【タグ付きポインタ設定例③】※危険、実行環境による
//  ポインタ=64bit, タグ=上位8bitの場合  ... taggedPtr<data_t, 8, -8> var;//タグのビット数=8, タグのシフトビット数=-8（上位8ビット）
template<typename T, std::size_t _TAG_BITS = 2, int _TAG_SHIFT = 0, int _POINTER_SHIFT = 0, typename _TAG_TYPE = std::uint32_t, typename _VALUE_TYPE = std::uint64_t>
struct taggedPtr
{
	//型
	typedef _VALUE_TYPE value_type;//タグ付きポインタ型
	typedef T* pointer_type;//ポインタ型
	typedef _TAG_TYPE tag_type;//タグ型
	//定数
	static const value_type ZERO_VALUE = static_cast<value_type>(0);//ゼロ値
	static const value_type NEGATIVE_ZERO_VALUE = ~ZERO_VALUE;//全ビットが立った値
	static const std::size_t VALUE_BITS = sizeof(value_type) * 8;//値のビット数
	static const std::size_t POINTER_BITS = sizeof(pointer_type) * 8;//ポインターのビット数
	static const std::size_t TAG_BITS = _TAG_BITS;//タグのビット数
	static const value_type TAG_MASK = NEGATIVE_ZERO_VALUE >> (VALUE_BITS - TAG_BITS);//タグのビットマスク（ビットシフトしていない状態でのマスク値）
	static const std::size_t POINTER_SHIFT = static_cast<std::size_t>(_POINTER_SHIFT < 0 ? static_cast<int>(VALUE_BITS) +_POINTER_SHIFT : _POINTER_SHIFT);//ポインターのシフトビット数
	static const std::size_t TAG_SHIFT = static_cast<std::size_t>(_TAG_SHIFT < 0 ? static_cast<int>(VALUE_BITS) + _TAG_SHIFT : _TAG_SHIFT);//タグのシフトビット数
	static const value_type NEGATIVE_TAG_MASK = ~(TAG_MASK << TAG_SHIFT);//タグの反転ビットマスク（ビットシフトした状態でのマスク値）
	//キャストオペレータ
	inline operator value_type() const { return m_value; }//値
	inline operator const pointer_type() const { return ptr(); }//ポインター
	inline operator pointer_type(){ return ptr(); }//ポインター
	//オペレータ
	inline bool operator==(const taggedPtr& lhs) const { return m_value == lhs.m_value; }
	inline bool operator!=(const taggedPtr& lhs) const { return m_value != lhs.m_value; }
	//アクセッサ
	inline value_type value() const { return m_value; }//値を取得
	inline const pointer_type ptr() const { return reinterpret_cast<const pointer_type>((m_value & NEGATIVE_TAG_MASK) >> POINTER_SHIFT); }//ポインターを取得
	inline pointer_type ptr(){ return reinterpret_cast<pointer_type>((m_value & NEGATIVE_TAG_MASK) >> POINTER_SHIFT); }//ポインターを取得
	inline tag_type tag() const { return static_cast<tag_type>((m_value >> TAG_SHIFT) & TAG_MASK); }//タグを取得
	inline bool isNull() const { return m_value == ZERO_VALUE; }//値がゼロか？
	inline bool isNotNull() const { return m_value != ZERO_VALUE; }//値がゼロじゃないか？
	inline void setNull(){ m_value = ZERO_VALUE; }//値をゼロにする
	inline void set(const pointer_type ptr, const tag_type tag)//値にポインターとタグをセット
	{
		m_value = ((reinterpret_cast<value_type>(ptr) << POINTER_SHIFT) & NEGATIVE_TAG_MASK) | ((static_cast<value_type>(tag)& TAG_MASK) << TAG_SHIFT);
	}
	//フィールド
	value_type m_value;//値（タグ＋ポインタ）
};

NAMESPACE_GASHA_END;//ネームスペース：終了

#endif//__TAGGED_PTR_H_

// End of file

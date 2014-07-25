#pragma once
#ifndef GASHA_INCLUDED_TAGGED_PTR_INL
#define GASHA_INCLUDED_TAGGED_PTR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// tagged_ptr.inl
// タグ付きポインタ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/tagged_ptr.h>//タグ付きポインタ【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//タグ付きポインタ型

//値にポインターとタグをセット
template<typename T, std::size_t _TAG_BITS, int _TAG_SHIFT, typename VALUE_TYPE, typename TAG_TYPE>
inline void taggedPtr<T, _TAG_BITS, _TAG_SHIFT, VALUE_TYPE, TAG_TYPE>::set(const pointer_type ptr, const tag_type tag)
{
	m_value = (reinterpret_cast<value_type>(ptr) & NEGATIVE_TAG_MASK) | ((static_cast<value_type>(tag) & TAG_MASK) << TAG_SHIFT);
}
//※タグサイズが0の場合の特殊化
template<typename T, int _TAG_SHIFT, typename VALUE_TYPE, typename TAG_TYPE>
inline void taggedPtr<T, 0, _TAG_SHIFT, VALUE_TYPE, TAG_TYPE>::set(const pointer_type ptr, const tag_type tag)
{
	m_value = reinterpret_cast<value_type>(ptr);
}

//タグを更新
template<typename T, std::size_t _TAG_BITS, int _TAG_SHIFT, typename VALUE_TYPE, typename TAG_TYPE>
inline void taggedPtr<T, _TAG_BITS, _TAG_SHIFT, VALUE_TYPE, TAG_TYPE>::updateTag(const taggedPtr tag_ptr)
{
	tag_type tag = tag_ptr.tag();
	++tag;
	m_value = (m_value & NEGATIVE_TAG_MASK) | ((static_cast<value_type>(tag) & TAG_MASK) << TAG_SHIFT);
}
//※タグサイズが0の場合の特殊化
template<typename T, int _TAG_SHIFT, typename VALUE_TYPE, typename TAG_TYPE>
inline void taggedPtr<T, 0, _TAG_SHIFT, VALUE_TYPE, TAG_TYPE>::updateTag(const taggedPtr tag_ptr)
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_TAGGED_PTR_INL

// End of file

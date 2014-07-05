#pragma once
#ifndef GASHA_INCLUDED_ITERATOR_H
#define GASHA_INCLUDED_ITERATOR_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// iterator.h
// イテレータ操作【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//========================================
//ソート／探索補助処理
//========================================

//----------------------------------------
//イテレータの差（要素数）を返す
//（注）ランダムアクセスイテレータ（動的配列、リングバッファ）以外は、結果として負の数を返す事がないことに注意。
//      なにより、end と begin が順序が逆転していると、処理が終了できないので取扱い厳重注意！
//      （ランダムアクセスイテレータは負の数を返すことができる）
template<class ITERATOR> inline typename ITERATOR::difference_type iteratorDifference(ITERATOR begin, ITERATOR end, std::input_iterator_tag);
template<class ITERATOR> inline typename ITERATOR::difference_type iteratorDifference(ITERATOR begin, ITERATOR end, std::output_iterator_tag);
template<class ITERATOR> inline typename ITERATOR::difference_type iteratorDifference(ITERATOR begin, ITERATOR end, std::forward_iterator_tag);
template<class ITERATOR> inline typename ITERATOR::difference_type iteratorDifference(ITERATOR begin, ITERATOR end, std::bidirectional_iterator_tag);
template<class ITERATOR> inline typename ITERATOR::difference_type iteratorDifference(ITERATOR begin, ITERATOR end, std::random_access_iterator_tag);
template<class ITERATOR> inline typename ITERATOR::difference_type iteratorDifference(ITERATOR begin, ITERATOR end);

//----------------------------------------
//for-each
template<class ITERATOR, class FUNCTOR>            inline void forEach(ITERATOR begin, ITERATOR end, FUNCTOR functor);
template<class CONTAINER, class FUNCTOR>           inline void forEach(CONTAINER& container, FUNCTOR functor);
template<typename T, std::size_t N, class FUNCTOR> inline void forEach(T(&array)[N], FUNCTOR functor);
template<typename T, std::size_t N, class FUNCTOR> inline void forEach(const T(&array)[N], FUNCTOR functor);
template<typename T, class FUNCTOR>                inline void forEach(T* array, const std::size_t size, FUNCTOR functor);
template<typename T, class FUNCTOR>                inline void forEach(const T* array, const std::size_t size, FUNCTOR functor);

//----------------------------------------
//reverse for-each
template<class ITERATOR, class FUNCTOR>            inline void reverseForEach(ITERATOR begin, ITERATOR end, FUNCTOR functor);
template<class CONTAINER, class FUNCTOR>           inline void reverseForEach(CONTAINER& container, FUNCTOR functor);
template<typename T, std::size_t N, class FUNCTOR> inline void reverseForEach(T(&array)[N], FUNCTOR functor);
template<typename T, std::size_t N, class FUNCTOR> inline void reverseForEach(const T(&array)[N], FUNCTOR functor);
template<typename T, class FUNCTOR>                inline void reverseForEach(T* array, const std::size_t size, FUNCTOR functor);
template<typename T, class FUNCTOR>                inline void reverseForEach(const T* array, const std::size_t size, FUNCTOR functor);

//----------------------------------------
//iterator for-each
//※値ではなく、イテレータ自体の参照を返す
template<class ITERATOR, class FUNCTOR>  inline void iteratorForEach(ITERATOR begin, ITERATOR end, FUNCTOR functor);
template<class CONTAINER, class FUNCTOR> inline void iteratorForEach(CONTAINER& container, FUNCTOR functor);

//----------------------------------------
//iterator reverse for-each
//※値ではなく、イテレータ自体の参照を返す
template<class ITERATOR, class FUNCTOR>  inline void iteratorReverseForEach(ITERATOR begin, ITERATOR end, FUNCTOR functor);
template<class CONTAINER, class FUNCTOR> inline void iteratorReverseForEach(CONTAINER& container, FUNCTOR functor);

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/iterator.inl>

#endif//GASHA_INCLUDED_ITERATOR_H

// End of file

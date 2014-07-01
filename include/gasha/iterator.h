#pragma once
#ifndef GASHA_INCLUDED_ITERATOR_H
#define GASHA_INCLUDED_ITERATOR_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// iterator.h
// イテレータ操作
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
template<class ITERATOR>
inline typename ITERATOR::difference_type iteratorDifference(ITERATOR begin, ITERATOR end, std::input_iterator_tag)
{
	typename ITERATOR::difference_type size = 0;
	for (; begin != end; ++begin)
		++size;
	return size;
}
template<class ITERATOR>
inline typename ITERATOR::difference_type iteratorDifference(ITERATOR begin, ITERATOR end, std::output_iterator_tag)
{
	typename ITERATOR::difference_type size = 0;
	for (; begin != end; ++begin)
		++size;
	return size;
}
template<class ITERATOR>
inline typename ITERATOR::difference_type iteratorDifference(ITERATOR begin, ITERATOR end, std::forward_iterator_tag)
{
	typename ITERATOR::difference_type size = 0;
	for (; begin != end; ++begin)
		++size;
	return size;
}
template<class ITERATOR>
inline typename ITERATOR::difference_type iteratorDifference(ITERATOR begin, ITERATOR end, std::bidirectional_iterator_tag)
{
	typename ITERATOR::difference_type size = 0;
	for (; begin != end; ++begin)
		++size;
	return size;
}
template<class ITERATOR>
inline typename ITERATOR::difference_type iteratorDifference(ITERATOR begin, ITERATOR end, std::random_access_iterator_tag)
{
	return end - begin;
}
template<class ITERATOR>
inline typename ITERATOR::difference_type iteratorDifference(ITERATOR begin, ITERATOR end)
{
	typename ITERATOR::iterator_category category;
	return iteratorDifference(begin, end, category);
}

//----------------------------------------
//for-each
template<class ITERATOR, class FUNCTOR>
void forEach(ITERATOR begin, ITERATOR end, FUNCTOR functor)
{
	for (; begin != end; ++begin)
	{
		functor(*begin);
	}
}
template<class CONTAINER, class FUNCTOR>
void forEach(CONTAINER& container, FUNCTOR functor)
{
	typename CONTAINER::iterator begin = container.begin();
	typename CONTAINER::iterator end = container.end();
	forEach(begin, end, functor);
}
template<typename T, std::size_t N, class FUNCTOR>
void forEach(T(&array)[N], FUNCTOR functor)
{
	T* begin = array;
	T* end = array + N;
	forEach(begin, end, functor);
}
template<typename T, std::size_t N, class FUNCTOR>
void forEach(const T(&array)[N], FUNCTOR functor)
{
	const T* begin = array;
	const T* end = array + N;
	forEach(begin, end, functor);
}
template<typename T, class FUNCTOR>
void forEach(T* array, const std::size_t size, FUNCTOR functor)
{
	T* begin = array;
	T* end = array + size;
	forEach(begin, end, functor);
}
template<typename T, class FUNCTOR>
void forEach(const T* array, const std::size_t size, FUNCTOR functor)
{
	const T* begin = array;
	const T* end = array + size;
	forEach(begin, end, functor);
}
//----------------------------------------
//reverse for-each
template<class ITERATOR, class FUNCTOR>
void reverseForEach(ITERATOR begin, ITERATOR end, FUNCTOR functor)
{
	while (begin != end)
	{
		--begin;
		functor(*begin);
	}
}
template<class CONTAINER, class FUNCTOR>
void reverseForEach(CONTAINER& container, FUNCTOR functor)
{
	typename CONTAINER::reverse_iterator rbegin = container.rbegin();
	typename CONTAINER::reverse_iterator rend = container.rend();
	forEach(rbegin, rend, functor);//リバースイテレータ＋forEachで処理
}
template<typename T, std::size_t N, class FUNCTOR>
void reverseForEach(T(&array)[N], FUNCTOR functor)
{
	T* begin = array + N;
	T* end = array;
	reverseForEach(begin, end, functor);
}
template<typename T, std::size_t N, class FUNCTOR>
void reverseForEach(const T(&array)[N], FUNCTOR functor)
{
	const T* begin = array + N;
	const T* end = array;
	reverseForEach(begin, end, functor);
}
template<typename T, class FUNCTOR>
void reverseForEach(T* array, const std::size_t size, FUNCTOR functor)
{
	T* begin = array + size;
	T* end = array;
	reverseForEach(begin, end, functor);
}
template<typename T, class FUNCTOR>
void reverseForEach(const T* array, const std::size_t size, FUNCTOR functor)
{
	const T* begin = array + size;
	const T* end = array;
	reverseForEach(begin, end, functor);
}

//----------------------------------------
//iterator for-each
//※値ではなく、イテレータ自体の参照を返す
template<class ITERATOR, class FUNCTOR>
void iteratorForEach(ITERATOR begin, ITERATOR end, FUNCTOR functor)
{
	for (; begin != end; ++begin)
	{
		functor(begin);
	}
}
template<class CONTAINER, class FUNCTOR>
void iteratorForEach(CONTAINER& container, FUNCTOR functor)
{
	typename CONTAINER::iterator begin = container.begin();
	typename CONTAINER::iterator end = container.end();
	iteratorForEach(begin, end, functor);
}
//----------------------------------------
//iterator reverse for-each
//※値ではなく、イテレータ自体の参照を返す
template<class ITERATOR, class FUNCTOR>
void iteratorReverseForEach(ITERATOR begin, ITERATOR end, FUNCTOR functor)
{
	while (begin != end)
	{
		--begin;
		functor(begin);
	}
}
template<class CONTAINER, class FUNCTOR>
void iteratorReverseForEach(CONTAINER& container, FUNCTOR functor)
{
	typename CONTAINER::reverse_iterator rbegin = container.rbegin();
	typename CONTAINER::reverse_iterator rend = container.rend();
	iteratorforEach(rbegin, rend, functor);//リバースイテレータ＋forEachで処理
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_ITERATOR_H

// End of file

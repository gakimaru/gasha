#pragma once
#ifndef GASHA_INCLUDED_BUILTIN_FUNCTIONS_INL_
#define GASHA_INCLUDED_BUILTIN_FUNCTIONS_INL_

//--------------------------------------------------------------------------------
// build_settings/builtin_functions.inl
// 組み込み関数・マクロ【インライン関数／テンプレート関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/builtin_functions.h>//ビルド設定診断【宣言部】

//--------------------------------------------------------------------------------
//組み込み関数

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//ソースファイル名のファイル名部分の抜き出し
namespace _private
{
	inline constexpr const char* getStaticFileName(const char* str, const char* new_str)
	{
		return *str == '\0' ? new_str : getStaticFileName(str + 1, *str == '/' || *str == '\\' ? str + 1 : new_str);
	}
}
inline constexpr const char* getStaticFileName(const char* str)
{
	return _private::getStaticFileName(str, str);
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_BUILTIN_FUNCTIONS_INL_

// End of file

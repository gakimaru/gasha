#pragma once
#ifndef GASHA_INCLUDED_LOG_PURPOSE_H
#define GASHA_INCLUDED_LOG_PURPOSE_H

//--------------------------------------------------------------------------------
// log_purpose.h
// ログ用途【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint8_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ用途
//--------------------------------------------------------------------------------

//----------------------------------------
//ログ用途の定数
enum logPurposeConst : std::uint8_t
{
	ofLog = 0,//用途：ログ出力
	ofNotice = 1,//用途：画面通知
};

//----------------------------------------
//ログ用途
class logPurpose
{
public:
	//型
	typedef std::uint8_t purpose_type;//ログ用途の値
public:
	//定数
	static const std::size_t NUM = 2;//ログ用途の数
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_purpose.inl>

#endif//GASHA_INCLUDED_LOG_PURPOSE_H

// End of file

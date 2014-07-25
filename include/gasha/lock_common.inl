﻿#pragma once
#ifndef GASHA_INCLUDED_LOCK_COMMON_INL
#define GASHA_INCLUDED_LOCK_COMMON_INL

//--------------------------------------------------------------------------------
// lock_common.inl
// ロック共通設定【インライン関数／テンプレート関数定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_common.h>//ロック共通設定【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//ロック制御共通設定

//ゼロスリープでコンテキストスイッチ
inline void contextSwitch()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(0));
}

//短いスリープでスイッチ
inline void contextSwitch(const short_sleep_switch_t)
{
	std::this_thread::sleep_for(std::chrono::nanoseconds(1));
}

//確実なコンテキストスイッチ
inline void contextSwitch(const force_switch_t)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

//イールド
inline void contextSwitch(const yield_switch_t)
{
	std::this_thread::yield();
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOCK_COMMON_H

// End of file

#pragma once
#ifndef __LOCK_COMMON_H_
#define __LOCK_COMMON_H_

//--------------------------------------------------------------------------------
// lock_common.h
// ロック共通設定
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/build_settings.h>//ビルド設定

NAMESPACE_GASHA_BEGIN//ネームスペース：開始

//----------------------------------------
//ロック制御共通設定

static const int DEFAULT_SPIN_COUNT = 1000;//スピンロックカウントのデフォルト値

static const int SHARED_LOCK_COUNTER_UNLOCKED = 0x01000000;//ロックが取得されていない時の共有ロックのカウンタ

NAMESPACE_GASHA_END//ネームスペース：終了

#endif//__LOCK_COMMON_H_

// End of file

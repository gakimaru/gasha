#pragma once
#ifndef GASHA_INCLUDED_LOCK_COMMON_H
#define GASHA_INCLUDED_LOCK_COMMON_H

//--------------------------------------------------------------------------------
// lock_common.h
// ロック共通設定
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//ロック制御共通設定

static const int DEFAULT_SPIN_COUNT = 1000;//スピンロックカウントのデフォルト値

static const int SHARED_LOCK_COUNTER_UNLOCKED = 0x01000000;//ロックが取得されていない時の共有ロックのカウンタ

//uqnieu_lock用のパラメータ

struct with_lock_t {};//ロック／排他ロック取得 ※コンストラクタでロック／排他ロックを取得する
const with_lock_t with_lock;

struct with_lock_shared_t {};//共有ロック取得 ※コンストラクタで共有ロックを取得する
const with_lock_shared_t with_lock_shared;

struct try_lock_t {};//ロック／排他ロック試行 ※コンストラクタでロック／排他ロック取得を試行する
const try_lock_t try_lock;

struct try_lock_shared_t {};//共有ロック試行 ※コンストラクタで共有ロック取得を試行する
const try_lock_shared_t try_lock_shared;

struct adopt_lock_t {};//ロック／排他取得済み状態管理の委譲 ※コンストラクタでロック／排他ロック取得済み状態にする（ロックを取得するわけではない）
const adopt_lock_t adopt_lock;

struct adopt_shared_lock_t {};//共有ロック取得済み状態管理の委譲 ※コンストラクタで共有ロック取得済み状態にする（ロックを取得するわけではない）
const adopt_shared_lock_t adopt_shared_lock;

struct defer_lock_t {};//遅延ロック ※コンストラクタでロックを取得しない
const defer_lock_t defer_lock;

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOCK_COMMON_H

// End of file

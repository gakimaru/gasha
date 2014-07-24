#pragma once
#ifndef GASHA_INCLUDED_LOCK_COMMON_H
#define GASHA_INCLUDED_LOCK_COMMON_H

//--------------------------------------------------------------------------------
// lock_common.h
// ロック共通設定【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <chrono>//C++11 std::chrono

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <thread>//C++11 std::ths_thread
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//ロック制御共通設定

static const int DEFAULT_SPIN_COUNT = 1000;//スピンロックカウントのデフォルト値

static const int SHARED_LOCK_COUNTER_UNLOCKED = 0x01000000;//ロックが取得されていない時の共有ロックのカウンタ

//uqnieu_lock用のパラメータ

struct with_lock_t {};//ロック／排他ロック取得 ※コンストラクタでロック／排他ロックを取得する
extern const with_lock_t with_lock;

struct with_lock_shared_t {};//共有ロック取得 ※コンストラクタで共有ロックを取得する
extern const with_lock_shared_t with_lock_shared;

struct try_to_lock_t {};//ロック／排他ロック試行 ※コンストラクタでロック／排他ロック取得を試行する
extern const try_to_lock_t try_to_lock;

struct try_to_lock_shared_t {};//共有ロック試行 ※コンストラクタで共有ロック取得を試行する
extern const try_to_lock_shared_t try_to_lock_shared;

struct adopt_lock_t {};//ロック／排他取得済み状態管理の委譲 ※コンストラクタでロック／排他ロック取得済み状態にする（ロックを取得するわけではない）
extern const adopt_lock_t adopt_lock;

struct adopt_shared_lock_t {};//共有ロック取得済み状態管理の委譲 ※コンストラクタで共有ロック取得済み状態にする（ロックを取得するわけではない）
extern const adopt_shared_lock_t adopt_shared_lock;

struct defer_lock_t {};//遅延ロック ※コンストラクタでロックを取得しない
extern const defer_lock_t defer_lock;

//----------------------------------------
//コンテキストスイッチ

struct force_switch_t {};//確実なスイッチ
extern const force_switch_t force_switch;

struct short_sleep_switch_t {};//短いスリープでスイッチ
extern const short_sleep_switch_t short_sleep_switch;

struct yield_switch_t {};//イールド
extern const yield_switch_t yield_switch;

//コンテキストスイッチ
inline void contextSwitch(){ std::this_thread::sleep_for(std::chrono::milliseconds(0)); }//ゼロスリープでコンテキストスイッチ
inline void contextSwitch(const short_sleep_switch_t){ std::this_thread::sleep_for(std::chrono::nanoseconds(1)); }//短いスリープでスイッチ
inline void contextSwitch(const force_switch_t){ std::this_thread::sleep_for(std::chrono::milliseconds(1)); }//確実なコンテキストスイッチ
inline void contextSwitch(const yield_switch_t){ std::this_thread::yield(); }//イールド ※実際にはコンテキストスイッチではなく、同じ優先度の他のスレッドに一時的に処理を譲るだけなので注意。

//デフォルトコンテキストスイッチ
#ifdef GASHA_DEFAULT_CONTEXT_SWITH_IS_FORCE
inline void defaultContextSwitch(){ contextSwitch(short_sleep_switch); }//確実なスイッチ
#else//GASHA_DEFAULT_CONTEXT_SWITH_IS_FORCE
#ifdef GASHA_DEFAULT_CONTEXT_SWITH_IS_SHORT_SLEEP
inline void defaultContextSwitch(){ contextSwitch(short_sleep_switch); }//短いスリープでスイッチ
#else//GASHA_DEFAULT_CONTEXT_SWITH_IS_SHORT_SLEEP
#ifdef GASHA_DEFAULT_CONTEXT_SWITH_IS_YIELD
inline void defaultContextSwitch(){ contextSwitch(yield_switch); }//イールド
#else//GASHA_DEFAULT_CONTEXT_SWITH_IS_YIELD
inline void defaultContextSwitch(){ contextSwitch(); }//ゼロスリープでコンテキストスイッチ
#endif//GASHA_DEFAULT_CONTEXT_SWITH_IS_YIELD
#endif//GASHA_DEFAULT_CONTEXT_SWITH_IS_SHORT_SLEEP
#endif//GASHA_DEFAULT_CONTEXT_SWITH_IS_FORCE

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/lock_common.inl>

#endif//GASHA_INCLUDED_LOCK_COMMON_H

// End of file

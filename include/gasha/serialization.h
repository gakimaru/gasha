﻿#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_H
#define GASHA_INCLUDED_SERIALIZATION_H

//--------------------------------------------------------------------------------
// serialization/serializatoin.h
// シリアライズ/シリアライズ（統合）【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

//シリアライズに必要なインクルードファイルを統合的にインクルード
#include <gasha/serialization/serialize.h>//シリアライズ/シリアライズ／デシリアライズ共通処理
#include <gasha/serialization/save.h>//シリアライズ/セーブ処理
#include <gasha/serialization/load.h>//シリアライズ/ロード処理
#include <gasha/serialization/notice.h>//シリアライズ/通知処理
#include <gasha/serialization/collector.h>//シリアライズ/データ収集処理
#include <gasha/serialization/distributor.h>//シリアライズ/データ分配処理
#include <gasha/serialization/error.h>//シリアライズ/致命的エラー発生時処理
#include <gasha/serialization/version_def.h>//シリアライズ/バージョン定義

#include <gasha/serialization/type_ctrl_base.h>//シリアライズ/型操作基底クラス
#include <gasha/serialization/type_ctrl.h>//シリアライズ/型操作クラス
#include <gasha/serialization/type_traits.h>//シリアライズ/型特性処理

#include <gasha/serialization/item_attr.h>//シリアライズ/データ項目属性クラス
#include <gasha/serialization/item_info_base.h>//シリアライズ/データ項目情報基底クラス
#include <gasha/serialization/item_info.h>//シリアライズ/データ項目情報クラス

#include <gasha/serialization/friends.h>//シリアライズ/フレンド宣言

#endif//GASHA_INCLUDED_SERIALIZATION_H

// End of file

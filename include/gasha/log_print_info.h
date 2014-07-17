#pragma once
#ifndef GASHA_INCLUDED_LOG_PRINT_INFO_H
#define GASHA_INCLUDED_LOG_PRINT_INFO_H

//--------------------------------------------------------------------------------
// log_print_info.h
// ログ出力情報【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース
#include <gasha/console_color.h>//コンソールカラー

#include <gasha/log_purpose.h>//ログ用途
#include <gasha/log_level.h>//ログレベル
#include <gasha/log_category.h>//ログカテゴリ
#include <gasha/log_attr.h>//ログ属性
#include <gasha/chrono.h>//時間処理ユーティリティ

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint64_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ出力情報
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログ出力情報
struct logPrintInfo
{
	//型
	typedef std::uint64_t id_type;//ID型
	typedef GASHA_ logPurpose::purpose_type purpose_type;//ログ用途の値
	typedef GASHA_ logCategory::category_type category_type;//ログカテゴリの値
	typedef GASHA_ logLevel::level_type level_type;//ログレベルの値
	typedef GASHA_ logAttr::attr_type attr_type;//ログ属性の値
	typedef std::uint16_t message_size_type;//メッセージ長

	//定数
	static const purpose_type PURPOSE_NUM = GASHA_ logPurpose::NUM;//ログ用途の数

	//フィールド
	id_type m_id;//ID ※0 で自動発番 ※ログキュー使用時に、キューを一意に識別し、キューどうしの順序性を判定するためのIDとして扱う
	GASHA_ time_type m_time;//時間 ※出力時もしくはキューイング時の時間（予約時の時間ではないので注意）
	const char* m_message;//メッセージ
	message_size_type m_messageSize;//メッセージサイズ ※0で自動計測 ※終端を含んだ長さ
	level_type m_level;//ログレベル
	category_type m_category;//ログカテゴリ
	attr_type m_attr;//ログ属性
	GASHA_ IConsole* m_consoles[PURPOSE_NUM];//ログ出力先
	const GASHA_ consoleColor* m_colors[PURPOSE_NUM];//ログ出力カラー

	//比較オペレータ
	inline bool operator<(const logPrintInfo& rhs) const;

	//ムーブオペレータ
	inline logPrintInfo& operator=(logPrintInfo&& rhs);
	//コピーオペレータ
	inline logPrintInfo& operator=(const logPrintInfo& rhs);
	//ムーブコンストラクタ
	inline logPrintInfo(logPrintInfo&& obj);
	//コピーコンストラクタ
	inline logPrintInfo(const logPrintInfo& obj);
	//コンストラクタ
	inline logPrintInfo(const id_type id, const char* message, const std::size_t message_size, const attr_type attr, const level_type level, const category_type category, GASHA_ IConsole* (&consoles)[PURPOSE_NUM], const GASHA_ consoleColor* (&colors)[PURPOSE_NUM]);
	//デフォルトコンストラクタ
	inline logPrintInfo();

	//デストラクタ
	inline ~logPrintInfo();
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_print_info.inl>

#endif//GASHA_INCLUDED_LOG_PRINT_INFO_H

// End of file

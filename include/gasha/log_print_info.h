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

//----------------------------------------
//ログ出力情報
class logPrintInfo
{
public:
	//型
	typedef std::uint64_t id_type;//ID型
	typedef GASHA_ logPurpose::purpose_type purpose_type;//ログ用途の値
	typedef GASHA_ logCategory::category_type category_type;//ログカテゴリの値
	typedef GASHA_ logLevel::level_type level_type;//ログレベルの値
	typedef GASHA_ logAttr::attr_type attr_type;//ログ属性の値
	typedef std::uint16_t message_size_type;//メッセージ長
public:
	//定数
	static const purpose_type PURPOSE_NUM = GASHA_ logPurpose::NUM;//ログ用途の数

public:
	//比較オペレータ
	inline bool operator<(const logPrintInfo& rhs) const;

public:
	//アクセッサ
#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
	inline id_type id() const { return m_id;}//ID
	inline void setId(const id_type id){ m_id = id; }//ID
	inline GASHA_ sec_t time() const { return m_time; }//時間
	inline void setTime(GASHA_ sec_t time){ m_time = time; }//時間
	inline const char* message() const { return m_message; }//メッセージ
	inline void setMessage(const char* message){ m_message = message; }//メッセージ
	inline message_size_type messageSize() const { return m_messageSize; }//メッセージサイズ
	inline void setMessageSize(const std::size_t message_size){ m_messageSize = static_cast<message_size_type>(message_size); }//メッセージサイズ
	inline level_type level() const { return m_level; }//ログレベル
	inline void setLevel(const level_type level) { m_level = level; }//ログレベル
	inline category_type category() const { return m_category; }//ログカテゴリ
	inline void setCategory(const category_type category){ m_category = category; }//ログカテゴリ
	inline attr_type attr() const { return m_attr; }//ログ属性
	inline void setAttr(const attr_type attr){ m_attr = attr; }//ログ属性
	inline GASHA_ iConsole* console(const purpose_type purpose) const { return m_consoles[purpose]; }//ログ出力先
	inline void setConsole(const purpose_type purpose, GASHA_ iConsole* console){ m_consoles[purpose] = console; }//ログ出力先
	inline const GASHA_ consoleColor* color(const purpose_type purpose) const { return m_colors[purpose]; }//ログ出力カラー
	inline void setColor(const purpose_type purpose, const GASHA_ consoleColor* color){ m_colors[purpose] = color; }//ログ出力カラー
#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
	inline id_type id() const { return 0; }//ID
	inline void setId(const id_type id){}//ID
	inline GASHA_ sec_t time() const { return static_cast<GASHA_ sec_t>(0); }//時間
	inline void setTime(GASHA_ sec_t time){}//時間
	inline const char* message() const { return nullptr; }//メッセージ
	inline void setMessage(const char* message){}//メッセージ
	inline message_size_type messageSize() const { return 0; }//メッセージサイズ
	inline void setMessageSize(const std::size_t message_size){}//メッセージサイズ
	inline level_type level() const { return 0; }//ログレベル
	inline void setLevel(const level_type level) {}//ログレベル
	inline category_type category() const { return 0; }//ログカテゴリ
	inline void setCategory(const category_type category){}//ログカテゴリ
	inline attr_type attr() const { return 0; }//ログ属性
	inline void setAttr(const attr_type attr){}//ログ属性
	inline GASHA_ iConsole* console(const purpose_type purpose) const { return nullptr; }//ログ出力先
	inline void setConsole(const purpose_type purpose, GASHA_ iConsole* console){}//ログ出力先
	inline const GASHA_ consoleColor* color(const purpose_type purpose) const { return nullptr; }//ログ出力カラー
	inline void setColor(const purpose_type purpose, const GASHA_ consoleColor* color){}//ログ出力カラー
#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

public:
	//ムーブオペレータ
	inline logPrintInfo& operator=(logPrintInfo&& rhs);
	//コピーオペレータ
	inline logPrintInfo& operator=(const logPrintInfo& rhs);

public:
	//ムーブコンストラクタ
	inline logPrintInfo(logPrintInfo&& obj);
	//コピーコンストラクタ
	inline logPrintInfo(const logPrintInfo& obj);
	//コンストラクタ
	inline logPrintInfo(const id_type id, const char* message, const std::size_t message_size, const attr_type attr, const level_type level, const category_type category, GASHA_ iConsole* (&consoles)[PURPOSE_NUM], const GASHA_ consoleColor* (&colors)[PURPOSE_NUM]);
	//デフォルトコンストラクタ
	inline logPrintInfo();
	//デストラクタ
	inline ~logPrintInfo();

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
private:
	//フィールド
	id_type m_id;//ID ※0 で自動発番 ※ログキュー使用時に、キューを一意に識別し、キューどうしの順序性を判定するためのIDとして扱う
	GASHA_ sec_t m_time;//時間 ※出力時もしくはキューイング時の時間（予約時の時間ではないので注意）
	const char* m_message;//メッセージ
	message_size_type m_messageSize;//メッセージサイズ ※0で自動計測 ※終端を含んだ長さ
	level_type m_level;//ログレベル
	category_type m_category;//ログカテゴリ
	attr_type m_attr;//ログ属性
	GASHA_ iConsole* m_consoles[PURPOSE_NUM];//ログ出力先
	const GASHA_ consoleColor* m_colors[PURPOSE_NUM];//ログ出力カラー
#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_print_info.inl>

#endif//GASHA_INCLUDED_LOG_PRINT_INFO_H

// End of file

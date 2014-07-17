#pragma once
#ifndef GASHA_INCLUDED_LOG_PRINT_INFO_INL
#define GASHA_INCLUDED_LOG_PRINT_INFO_INL

//--------------------------------------------------------------------------------
// log_print_info.inl
// ログ出力情報【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_print_info.h>//ログ出力情報【宣言部】

#include <gasha/chrono.h>//時間処理系ユーティリティ：nowElapsedTime()

#include <cstring>//std::memcpy(), std::memset()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ出力情報
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログ出力情報

//比較演算子
inline bool logPrintInfo::operator<(const logPrintInfo& rhs) const
{
	//小さい順に並べるため、判定を逆にする
	return m_id > rhs.m_id;
}

//ムーブオペレータ
inline logPrintInfo& logPrintInfo::operator=(logPrintInfo&& rhs)
{
	std::memcpy(this, &rhs, sizeof(*this));
	return *this;
}

//コピーオペレータ
inline logPrintInfo& logPrintInfo::operator=(const logPrintInfo& rhs)
{
	std::memcpy(this, &rhs, sizeof(*this));
	return *this;
}

//ムーブコンストラクタ
inline logPrintInfo::logPrintInfo(logPrintInfo&& obj)
{
	std::memcpy(this, &obj, sizeof(*this));
}

//コピーコンストラクタ
inline logPrintInfo::logPrintInfo(const logPrintInfo& obj)
{
	std::memcpy(this, &obj, sizeof(*this));
}

//コンストラクタ
inline logPrintInfo::logPrintInfo(const id_type id, const char* message, const std::size_t message_size, const attr_type attr, const level_type level, const category_type category, GASHA_ IConsole* (&consoles)[PURPOSE_NUM], const GASHA_ consoleColor* (&colors)[PURPOSE_NUM]) :
	m_id(id),
	m_time(nowElapsedTime()),
	m_message(message),
	m_messageSize(static_cast<message_size_type>(message_size)),
	m_level(level),
	m_category(category),
	m_attr(attr)
{
	for (purpose_type purpose = 0; purpose < PURPOSE_NUM; ++purpose)
	{
		m_consoles[purpose] = consoles[purpose];
		m_colors[purpose] = colors[purpose];
	}
}

//デフォルトコンストラクタ
inline logPrintInfo::logPrintInfo()
{
	std::memset(this, 0, sizeof(*this));
}

//デストラクタ
inline logPrintInfo::~logPrintInfo()
{}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_PRINT_INFO_INL

// End of file

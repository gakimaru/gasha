#pragma once
#ifndef GASHA_INCLUDED_LOG_INL
#define GASHA_INCLUDED_LOG_INL

//--------------------------------------------------------------------------------
// log.inl
// ログ操作【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log.h>//ログ操作【宣言部】

#include <gasha/log_attr.h>//ログ属性
#include <gasha/log_level.h>//ログレベル
#include <gasha/log_category.h>//ログカテゴリ
#include <gasha/log_mask.h>//ログレベルマスク
#include <gasha/log_print_info.h>//ログ出力情報
#include <gasha/log_work_buff.h>//ログワークバッファ
#include <gasha/log_queue.h>//ログキュー
#include <gasha/log_queue_monitor.h>//ログキューモニター
#include <gasha/std_log_print.h>//標準ログ出力
#include <gasha/chrono.h>//時間処理ユーティリティ：nowElapsedTime()

#include <cstddef>//std::size_t
#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ操作
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログ操作

//ログ出力：書式付き出力
template<typename... Tx>
bool log::print(const bool is_reserved, const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask mask;
	GASHA_ logMask::consolesInfo_type masked;
	mask.consolesInfo(masked, level, category);
	if (masked.m_cond == logMask::hasNotAvailableConsole)//利用可能なコンソールがない
		return false;
	else if (masked.m_cond == logMask::everyConsoleIsDummy)//ダミーコンソールしかない
		return true;//成功扱い

	//ワークバッファ取得
	logWorkBuff work_buff;
	char* message = work_buff.alloc();
	if (!message)
		return false;

	//書式付き文字列を変換
	std::size_t message_len = 0;
	work_buff.spprintf(message, message_len, fmt, std::forward<Tx>(args)...);

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	print_info.setId(is_reserved ? m_reservedId : 0);
	print_info.setTime(nowElapsedTime());
	print_info.setMessage(message);
	print_info.setMessageSize(message_len + 1);
	print_info.setLevel(level);
	print_info.setCategory(category);
	print_info.setAttr(*GASHA_ logAttr());
	for (purpose_type purpose = 0; purpose < PURPOSE_NUM; ++purpose)
	{
		print_info.setConsole(purpose, masked.m_consoles[purpose]);
		print_info.setColor(purpose, masked.m_colors[purpose]);
	}

	//キューイング
	logQueue queue;
	const bool result = queue.enqueue(print_info);//キューイング

	//ワークバッファを解放
	work_buff.free(message);

	//予約IDを更新
	if (is_reserved && m_reservedNum > 0)
	{
		--m_reservedNum;
		if (m_reservedNum == 0)
			m_reservedId = 0;
		else
			++m_reservedId;
	}

	//ログキューモニターに通知
	if (result)
	{
		logQueueMonitor mon;
		mon.notify();
	}
	
	//終了
	return result;
}
//※通常版
template<typename... Tx>
inline bool log::print(const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	return print(false, level, category, fmt, std::forward<Tx>(args)...);
}
//※予約出力版
template<typename... Tx>
inline bool log::reservedPrint(const char* fmt, Tx&&... args)
{
	//予約IDを確認
	if (m_reservedNum == 0)
		return false;
	return print(true, m_reservedLevel, m_reservedCategory, fmt, std::forward<Tx>(args)...);
}

//ログ出力：書式なし出力
//※通常版
inline bool log::put(const log::level_type level, const log::category_type category, const char* str)
{
	return put(false, level, category, str);
}
//※予約出力版
inline bool log::reservedPut(const char* str)
{
	//予約IDを確認
	if (m_reservedNum == 0)
		return false;
	return put(true, m_reservedLevel, m_reservedCategory, str);
}

//ログ出力：書式付き出力
//※文字コード変換処理指定版
template<class CONVERTER_FUNC, typename... Tx>
bool log::convPrint(const bool is_reserved, CONVERTER_FUNC converter_func, const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask mask;
	GASHA_ logMask::consolesInfo_type masked;
	mask.consolesInfo(masked, level, category);
	if (masked.m_cond == logMask::hasNotAvailableConsole)//利用可能なコンソールがない
		return false;
	else if (masked.m_cond == logMask::everyConsoleIsDummy)//ダミーコンソールしかない
		return true;//成功扱い

	//ワークバッファ取得
	logWorkBuff work_buff;
	char* message = work_buff.alloc();
	if (!message)
		return false;

	//書式付き文字列を変換
	//※半分のバッファサイズで処理する
	std::size_t message_len = 0;
	work_buff.spprintf_halfSized(message, message_len, fmt, std::forward<Tx>(args)...);

	//文字コード変換
	//※半分のバッファサイズで処理する
	char* converted_message = message + logWorkBuff::HALF_MESSAGE_SIZE;
	const std::size_t converted_message_len = converter_func(converted_message, logWorkBuff::HALF_MESSAGE_SIZE, message, message_len);

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	print_info.setId (is_reserved ? m_reservedId : 0);
	print_info.setTime(nowElapsedTime());
	print_info.setMessage(converted_message);
	print_info.setMessageSize(converted_message_len + 1);
	print_info.setLevel(level);
	print_info.setCategory(category);
	print_info.setAttr(*GASHA_ logAttr());
	for (purpose_type purpose = 0; purpose < PURPOSE_NUM; ++purpose)
	{
		print_info.setConsole(purpose, masked.m_consoles[purpose]);
		print_info.setColor(purpose, masked.m_colors[purpose]);
	}

	//キューイング
	logQueue queue;
	const bool result = queue.enqueue(print_info);//キューイング

	//ワークバッファを解放
	work_buff.free(message);

	//予約IDを更新
	if (is_reserved && m_reservedNum > 0)
	{
		--m_reservedNum;
		if (m_reservedNum == 0)
			m_reservedId = 0;
		else
			++m_reservedId;
	}

	//ログキューモニターに通知
	if (result)
	{
		logQueueMonitor mon;
		mon.notify();
	}

	//終了
	return result;
}
//※通常版
template<class CONVERTER_FUNC, typename... Tx>
inline bool log::convPrint(CONVERTER_FUNC converter_func, const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	return convPrint(false, converter_func, level, category, fmt, std::forward<Tx>(args)...);
}
//※予約出力版
template<class CONVERTER_FUNC, typename... Tx>
inline bool log::reservedConvPrint(CONVERTER_FUNC converter_func, const char* fmt, Tx&&... args)
{
	//予約IDを確認
	if (m_reservedNum == 0)
		return false;
	return convPrint(true, converter_func, m_reservedLevel, m_reservedCategory, fmt, std::forward<Tx>(args)...);
}

//ログ出力：書式なし出力
//※文字コード変換処理指定版
template<class CONVERTER_FUNC>
bool log::convPut(const bool is_reserved, CONVERTER_FUNC converter_func, const log::level_type level, const log::category_type category, const char* str)
{
	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask mask;
	GASHA_ logMask::consolesInfo_type masked;
	mask.consolesInfo(masked, level, category);
	if (masked.m_cond == logMask::hasNotAvailableConsole)//利用可能なコンソールがない
		return false;
	else if (masked.m_cond == logMask::everyConsoleIsDummy)//ダミーコンソールしかない
		return true;//成功扱い

	//ワークバッファ取得
	logWorkBuff work_buff;
	char* converted_str = work_buff.alloc();
	if (!converted_str)
		return false;

	//文字コード変換
	const std::size_t converted_str_len = converter_func(converted_str, logWorkBuff::MAX_MESSAGE_SIZE, str, GASHA_ strlen_fast(str));

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	print_info.setId(is_reserved ? m_reservedId : 0);
	print_info.setTime(nowElapsedTime());
	print_info.setMessage(converted_str);
	print_info.setMessageSize(converted_str_len + 1);
	print_info.setLevel(level);
	print_info.setCategory(category);
	print_info.setAttr(*GASHA_ logAttr());
	for (purpose_type purpose = 0; purpose < PURPOSE_NUM; ++purpose)
	{
		print_info.setConsole(purpose, masked.m_consoles[purpose]);
		print_info.setColor(purpose, masked.m_colors[purpose]);
	}

	//キューイング
	logQueue queue;
	const bool result = queue.enqueue(print_info);//キューイング

	//ワークバッファを解放
	work_buff.free(converted_str);

	//予約IDを更新
	if (is_reserved && m_reservedNum > 0)
	{
		--m_reservedNum;
		if (m_reservedNum == 0)
			m_reservedId = 0;
		else
			++m_reservedId;
	}

	//ログキューモニターに通知
	if (result)
	{
		logQueueMonitor mon;
		mon.notify();
	}

	//終了
	return result;
}
//※通常版
template<class CONVERTER_FUNC>
inline bool log::convPut(CONVERTER_FUNC converter_func, const log::level_type level, const log::category_type category, const char* str)
{
	return convPut(false, converter_func, level, category, str);
}
//※予約出力版
template<class CONVERTER_FUNC>
inline bool log::reservedConvPut(CONVERTER_FUNC converter_func, const char* str)
{
	//予約IDを確認
	if (m_reservedNum == 0)
		return false;
	return convPut(true, converter_func, m_reservedLevel, m_reservedCategory, str);
}

//ログ直接出力：書式付き出力
template<class PRINT_FUNC, typename... Tx>
bool log::printDirect(PRINT_FUNC print_func, const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask mask;
	GASHA_ logMask::consolesInfo_type masked;
	mask.consolesInfo(masked, level, category);
	if (masked.m_cond == logMask::hasNotAvailableConsole)//利用可能なコンソールがない
		return false;
	else if (masked.m_cond == logMask::everyConsoleIsDummy)//ダミーコンソールしかない
		return true;//成功扱い

	//ワークバッファ取得
	logWorkBuff work_buff;
	char* message = work_buff.alloc();
	if (!message)
		return false;

	//書式付き文字列を変換
	std::size_t message_len = 0;
	work_buff.spprintf(message, message_len, fmt, std::forward<Tx>(args)...);

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	print_info.setId(0);
	print_info.setTime(nowElapsedTime());
	print_info.setMessage(message);
	print_info.setMessageSize(message_len + 1);
	print_info.setLevel(level);
	print_info.setCategory(category);
	print_info.setAttr(*GASHA_ logAttr());
	for (purpose_type purpose = 0; purpose < PURPOSE_NUM; ++purpose)
	{
		print_info.setConsole(purpose, masked.m_consoles[purpose]);
		print_info.setColor(purpose, masked.m_colors[purpose]);
	}

	//直接出力
	print_func(print_info);//出力

	//ワークバッファを解放
	work_buff.free(message);

	//終了
	return true;
}
template<typename... Tx>
inline bool log::printDirect(const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	return printDirect(GASHA_ stdLogPrint(), level, category, fmt, std::forward<Tx>(args)...);
}

//ログ直接出力：書式なし出力
template<class PRINT_FUNC>
bool log::putDirect(PRINT_FUNC print_func, const log::level_type level, const log::category_type category, const char* str)
{
	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask mask;
	GASHA_ logMask::consolesInfo_type masked;
	mask.consolesInfo(masked, level, category);
	if (masked.m_cond == logMask::hasNotAvailableConsole)//利用可能なコンソールがない
		return false;
	else if (masked.m_cond == logMask::everyConsoleIsDummy)//ダミーコンソールしかない
		return true;//成功扱い

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	print_info.setId(0);
	print_info.setTime(nowElapsedTime());
	print_info.setMessage(str);
	print_info.setMessageSize(0);
	print_info.setLevel(level);
	print_info.setCategory(category);
	print_info.setAttr(*GASHA_ logAttr());
	for (purpose_type purpose = 0; purpose < PURPOSE_NUM; ++purpose)
	{
		print_info.setConsole(purpose, masked.m_consoles[purpose]);
		print_info.setColor(purpose, masked.m_colors[purpose]);
	}

	//直接出力
	print_func(print_info);//出力

	//終了
	return true;
}
inline bool log::putDirect(const log::level_type level, const log::category_type category, const char* str)
{
	return putDirect(GASHA_ stdLogPrint(), level, category, str);
}

//ログ直接出力：書式付き出力
//※文字コード変換処理指定版
template<class PRINT_FUNC, class CONVERTER_FUNC, typename... Tx>
bool log::convPrintDirect(PRINT_FUNC print_func, CONVERTER_FUNC converter_func, const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask mask;
	GASHA_ logMask::consolesInfo_type masked;
	mask.consolesInfo(masked, level, category);
	if (masked.m_cond == logMask::hasNotAvailableConsole)//利用可能なコンソールがない
		return false;
	else if (masked.m_cond == logMask::everyConsoleIsDummy)//ダミーコンソールしかない
		return true;//成功扱い

	//ワークバッファ取得
	logWorkBuff work_buff;
	char* message = work_buff.alloc();
	if (!message)
		return false;

	//書式付き文字列を変換
	//※半分のバッファサイズで処理する
	std::size_t message_len = 0;
	work_buff.spprintf_halfSized(message, message_len, fmt, std::forward<Tx>(args)...);

	//文字コード変換
	char* converted_message = message + logWorkBuff::HALF_MESSAGE_SIZE;
	const std::size_t converted_message_len = converter_func(converted_message, logWorkBuff::HALF_MESSAGE_SIZE, message, message_len);

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	print_info.setId(0);
	print_info.setTime(nowElapsedTime());
	print_info.setMessage(converted_message);
	print_info.setMessageSize(converted_message_len + 1);
	print_info.setLevel(level);
	print_info.setCategory(category);
	print_info.setAttr(*GASHA_ logAttr());
	for (purpose_type purpose = 0; purpose < PURPOSE_NUM; ++purpose)
	{
		print_info.setConsole(purpose, masked.m_consoles[purpose]);
		print_info.setColor(purpose, masked.m_colors[purpose]);
	}

	//直接出力
	print_func(print_info);//出力

	//ワークバッファを解放
	work_buff.free(message);

	//終了
	return true;
}
template<class CONVERTER_FUNC, typename... Tx>
inline bool log::convPrintDirect(CONVERTER_FUNC converter_func, const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	return convPrintDirect(GASHA_ stdLogPrint(), converter_func, level, category, fmt, std::forward<Tx>(args)...);
}

//ログ直接出力：書式なし出力
//※文字コード変換処理指定版
template<class PRINT_FUNC, class CONVERTER_FUNC>
bool log::convPutDirect(PRINT_FUNC print_func, CONVERTER_FUNC converter_func, const log::level_type level, const log::category_type category, const char* str)
{
	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask mask;
	GASHA_ logMask::consolesInfo_type masked;
	mask.consolesInfo(masked, level, category);
	if (masked.m_cond == logMask::hasNotAvailableConsole)//利用可能なコンソールがない
		return false;
	else if (masked.m_cond == logMask::everyConsoleIsDummy)//ダミーコンソールしかない
		return true;//成功扱い

	//ワークバッファ取得
	logWorkBuff work_buff;
	char* converted_str = work_buff.alloc();
	if (!converted_str)
		return false;

	//文字コード変換
	const std::size_t converted_str_len = converter_func(converted_str, logWorkBuff::MAX_MESSAGE_SIZE, str, GASHA_ strlen_fast(str));

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	print_info.setId(0);
	print_info.setTime(nowElapsedTime());
	print_info.setMessage(converted_str);
	print_info.setMessageSize(converted_str_len + 1);
	print_info.setLevel(level);
	print_info.setCategory(category);
	print_info.setAttr(*GASHA_ logAttr());
	for (purpose_type purpose = 0; purpose < PURPOSE_NUM; ++purpose)
	{
		print_info.setConsole(purpose, masked.m_consoles[purpose]);
		print_info.setColor(purpose, masked.m_colors[purpose]);
	}

	//直接出力
	print_func(print_info);//出力

	//ワークバッファを解放
	work_buff.free(converted_str);

	//終了
	return true;
}
template<class CONVERTER_FUNC>
inline bool log::convPutDirect(CONVERTER_FUNC converter_func, const log::level_type level, const log::category_type category, const char* str)
{
	return convPutDirect(GASHA_ stdLogPrint(), converter_func, level, category, str);
}

//ログ出力予約
inline bool log::reserve(const log::level_type level, const log::category_type category, const int num)
{
	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask mask;
	GASHA_ logMask::consolesInfo_type masked;
	mask.consolesInfo(masked, level, category);
	if (masked.m_cond == logMask::hasNotAvailableConsole)//利用可能なコンソールがない
		return false;
	else if (masked.m_cond == logMask::everyConsoleIsDummy)//ダミーコンソールしかない
		return false;

	GASHA_ logQueue log_queue;
	m_reservedId = log_queue.reserve(num);
	m_reservedNum = num;
	m_reservedLevel = level;
	m_reservedCategory = category;
	return true;
}

//ログ出力予約を取り消す
inline bool log::cancelToReserve()
{
	if (m_reservedNum == 0)
		return false;
	GASHA_ logQueueMonitor log_queue_monitor;
	log_queue_monitor.skip(m_reservedId + m_reservedNum);
	//m_reservedId = 0;
	m_reservedNum = 0;
	//m_reservedLevel = 0;
	//m_reservedCategory = 0;
	return true;
}

//コンストラクタ
inline log::log() :
	//m_reservedId(0),
	m_reservedNum(0)
	//m_reservedLevel(0),
	//m_reservedCategory(0)
{}

//デストラクタ
inline log::~log()
{
	cancelToReserve();//予約があったらキャンセル
}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_INL

// End of file

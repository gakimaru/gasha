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
#include <gasha/log_print_info.h>//ログ出力情報
#include <gasha/log_work_buff.h>//ログワークバッファ
#include <gasha/log_queue.h>//ログキュー
#include <gasha/log_queue_monitor.h>//ログキューモニター
#include <gasha/std_log_print.h>//標準ログ出力
#include <gasha/call_point.h>//コールポイント
#include <gasha/chrono.h>//時間処理ユーティリティ：nowElapsedTime()
#include <gasha/string.h>//文字列処理：spprintf()

#include <cstddef>//std::size_t
#include <utility>//C++11 std::forward
#include <type_traits>//C++11 std::is_same

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ操作
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログ操作

//ログ出力：書式付き出力
template<class ADD_MESSAGE_FUNC, typename... Tx>
bool log::print(const log::ope_type ope, ADD_MESSAGE_FUNC add_message_func, const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	bool result = false;

	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask::consolesInfo_type consoles_info;
	if (!consolesInfo(result, consoles_info, level, category))
		return result;

	//ワークバッファ取得
	logWorkBuff work_buff;
	char* message = work_buff.alloc();
	if (!message)
		return false;

	//書式付き文字列を変換
	std::size_t message_len = 0;
	work_buff.spprintf(message, message_len, fmt, std::forward<Tx>(args)...);

	//メッセージ付加
	add_message_func(message, logWorkBuff::MAX_MESSAGE_SIZE, message_len);

	//メッセージ付加
	addMessage(ope, message, logWorkBuff::MAX_MESSAGE_SIZE, message_len);

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	makeLogPrintInfo(print_info, ope, level, category, message, message_len + 1, consoles_info);

	//ログキューをエンキュー
	result = enqueue(print_info);

	//ワークバッファを解放
	work_buff.free(message);

	//ログキューモニターに通知
	if (result)
		notifyMonitor();
	
	//終了
	return result;
}
//※通常版
template<typename... Tx>
inline bool log::print(const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	return print(normalOpe, dummyAddMessageFunctor(), level, category, fmt, std::forward<Tx>(args)...);
}
//※予約出力版
template<typename... Tx>
inline bool log::reservedPrint(const char* fmt, Tx&&... args)
{
	//予約IDを確認
	if (m_reservedNum == 0)
		return false;
	return print(useReservedId, dummyAddMessageFunctor(), m_reservedLevel, m_reservedCategory, fmt, std::forward<Tx>(args)...);
}

//ログ出力：書式なし出力
template<class ADD_MESSAGE_FUNC>
bool log::put(const ope_type ope, ADD_MESSAGE_FUNC add_message_func, const log::level_type level, const log::category_type category, const char* str)
{
	//メッセージ付加がある場合、書式付き出力に回してワークバッファを使用した出力を行う
	if (isAddMessage(ope, add_message_func))
		return print(ope, add_message_func, level, category, str);

	bool result = false;

	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask::consolesInfo_type consoles_info;
	if (!consolesInfo(result, consoles_info, level, category))
		return result;

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	makeLogPrintInfo(print_info, ope, level, category, str, 0, consoles_info);

	//ログキューをエンキュー
	result = enqueue(print_info);

	//ログキューモニターに通知
	if (result)
		notifyMonitor();

	//終了
	return result;
}
//※通常版
inline bool log::put(const log::level_type level, const log::category_type category, const char* str)
{
	return put(normalOpe, dummyAddMessageFunctor(), level, category, str);
}
//※予約出力版
inline bool log::reservedPut(const char* str)
{
	//予約IDを確認
	if (m_reservedNum == 0)
		return false;
	return put(useReservedId, dummyAddMessageFunctor(), m_reservedLevel, m_reservedCategory, str);
}

//ログ出力：書式付き出力
//※文字コード変換処理指定版
template<class CONVERTER_FUNC, class ADD_MESSAGE_FUNC, typename... Tx>
bool log::convPrint(const log::ope_type ope, CONVERTER_FUNC converter_func, ADD_MESSAGE_FUNC add_message_func, const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	bool result = false;

	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask::consolesInfo_type consoles_info;
	if (!consolesInfo(result, consoles_info, level, category))
		return result;

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
	std::size_t converted_message_len = converter_func(converted_message, logWorkBuff::HALF_MESSAGE_SIZE, message, message_len);

	//メッセージ付加
	add_message_func(message, logWorkBuff::HALF_MESSAGE_SIZE, message_len);

	//メッセージ付加
	addMessage(ope, converted_message, logWorkBuff::HALF_MESSAGE_SIZE, converted_message_len);

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	makeLogPrintInfo(print_info, ope, level, category, converted_message, converted_message_len + 1, consoles_info);

	//ログキューをエンキュー
	result = enqueue(print_info);

	//ワークバッファを解放
	work_buff.free(message);

	//ログキューモニターに通知
	if (result)
		notifyMonitor();

	//終了
	return result;
}
//※通常版
template<class CONVERTER_FUNC, typename... Tx>
inline bool log::convPrint(CONVERTER_FUNC converter_func, const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	return convPrint(normalOpe, converter_func, dummyAddMessageFunctor(), level, category, fmt, std::forward<Tx>(args)...);
}
//※予約出力版
template<class CONVERTER_FUNC, typename... Tx>
inline bool log::reservedConvPrint(CONVERTER_FUNC converter_func, const char* fmt, Tx&&... args)
{
	//予約IDを確認
	if (m_reservedNum == 0)
		return false;
	return convPrint(useReservedId, converter_func, dummyAddMessageFunctor(), m_reservedLevel, m_reservedCategory, fmt, std::forward<Tx>(args)...);
}

//ログ出力：書式なし出力
//※文字コード変換処理指定版
template<class CONVERTER_FUNC, class ADD_MESSAGE_FUNC>
bool log::convPut(const log::ope_type ope, CONVERTER_FUNC converter_func, ADD_MESSAGE_FUNC add_message_func, const log::level_type level, const log::category_type category, const char* str)
{
	//メッセージ付加がある場合、書式付き出力に回してワークバッファを使用した出力を行う
	if (isAddMessage(ope, add_message_func))
		return convPrint(ope, converter_func, add_message_func, level, category, str);

	bool result = false;

	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask::consolesInfo_type consoles_info;
	if (!consolesInfo(result, consoles_info, level, category))
		return result;

	//ワークバッファ取得
	logWorkBuff work_buff;
	char* converted_str = work_buff.alloc();
	if (!converted_str)
		return false;

	//文字コード変換
	const std::size_t converted_str_len = converter_func(converted_str, logWorkBuff::MAX_MESSAGE_SIZE, str, GASHA_ strlen_fast(str));

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	makeLogPrintInfo(print_info, ope, level, category, converted_str, converted_str_len + 1, consoles_info);

	//ログキューをエンキュー
	result = enqueue(print_info);

	//ワークバッファを解放
	work_buff.free(converted_str);

	//ログキューモニターに通知
	if (result)
		notifyMonitor();

	//終了
	return result;
}
//※通常版
template<class CONVERTER_FUNC>
inline bool log::convPut(CONVERTER_FUNC converter_func, const log::level_type level, const log::category_type category, const char* str)
{
	return convPut(normalOpe, converter_func, dummyAddMessageFunctor(), level, category, str);
}
//※予約出力版
template<class CONVERTER_FUNC>
inline bool log::reservedConvPut(CONVERTER_FUNC converter_func, const char* str)
{
	//予約IDを確認
	if (m_reservedNum == 0)
		return false;
	return convPut(useReservedId, converter_func, dummyAddMessageFunctor(), m_reservedLevel, m_reservedCategory, str);
}

//ログ直接出力：書式付き出力
template<class PRINT_FUNC, class ADD_MESSAGE_FUNC, typename... Tx>
bool log::printDirect(const log::ope_type ope, PRINT_FUNC print_func, ADD_MESSAGE_FUNC add_message_func, const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	bool result = false;

	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask::consolesInfo_type consoles_info;
	if (!consolesInfo(result, consoles_info, level, category))
		return result;

	//ワークバッファ取得
	logWorkBuff work_buff;
	char* message = work_buff.alloc();
	if (!message)
		return false;

	//書式付き文字列を変換
	std::size_t message_len = 0;
	work_buff.spprintf(message, message_len, fmt, std::forward<Tx>(args)...);

	//メッセージ付加
	add_message_func(message, logWorkBuff::MAX_MESSAGE_SIZE, message_len);

	//メッセージ付加
	addMessage(ope, message, logWorkBuff::MAX_MESSAGE_SIZE, message_len);

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	makeLogPrintInfo(print_info, normalOpe, level, category, message, message_len + 1, consoles_info);

	//直接出力
	print_func(print_info);//出力

	//ワークバッファを解放
	work_buff.free(message);

	//終了
	return true;
}
//※通常版
template<class PRINT_FUNC, typename... Tx>
bool log::printDirect(PRINT_FUNC print_func, const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	return printDirect(normalOpe, print_func, dummyAddMessageFunctor(), level, category, fmt, std::forward<Tx>(args)...);
}
//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
template<typename... Tx>
inline bool log::printDirect(const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	return printDirect(normalOpe, GASHA_ stdLogPrint(), dummyAddMessageFunctor(), level, category, fmt, std::forward<Tx>(args)...);
}

//ログ直接出力：書式なし出力
template<class PRINT_FUNC, class ADD_MESSAGE_FUNC>
bool log::putDirect(const log::ope_type ope, PRINT_FUNC print_func, ADD_MESSAGE_FUNC add_message_func, const log::level_type level, const log::category_type category, const char* str)
{
	//メッセージ付加がある場合、書式付き出力に回してワークバッファを使用した出力を行う
	if (isAddMessage(ope, add_message_func))
		return printDirect(ope, print_func, add_message_func, level, category, str);

	bool result = false;

	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask::consolesInfo_type consoles_info;
	if (!consolesInfo(result, consoles_info, level, category))
		return result;

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	makeLogPrintInfo(print_info, normalOpe, level, category, str, 0, consoles_info);

	//直接出力
	print_func(print_info);//出力

	//終了
	return true;
}
//※通常版
template<class PRINT_FUNC>
bool log::putDirect( PRINT_FUNC print_func, const log::level_type level, const log::category_type category, const char* str)
{
	return putDirect(normalOpe, print_func, dummyAddMessageFunctor(), level, category, str);
}
//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
inline bool log::putDirect(const log::level_type level, const log::category_type category, const char* str)
{
	return putDirect(normalOpe, GASHA_ stdLogPrint(), dummyAddMessageFunctor(), level, category, str);
}

//ログ直接出力：書式付き出力
//※文字コード変換処理指定版
template<class PRINT_FUNC, class CONVERTER_FUNC, class ADD_MESSAGE_FUNC, typename... Tx>
bool log::convPrintDirect(const log::ope_type ope, PRINT_FUNC print_func, CONVERTER_FUNC converter_func, ADD_MESSAGE_FUNC add_message_func, const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	bool result = false;

	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask::consolesInfo_type consoles_info;
	if (!consolesInfo(result, consoles_info, level, category))
		return result;

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
	std::size_t converted_message_len = converter_func(converted_message, logWorkBuff::HALF_MESSAGE_SIZE, message, message_len);

	//メッセージ付加
	add_message_func(message, logWorkBuff::MAX_MESSAGE_SIZE, message_len);

	//メッセージ付加
	addMessage(ope, converted_message, logWorkBuff::MAX_MESSAGE_SIZE, converted_message_len);

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	makeLogPrintInfo(print_info, normalOpe, level, category, converted_message, converted_message_len + 1, consoles_info);

	//直接出力
	print_func(print_info);//出力

	//ワークバッファを解放
	work_buff.free(message);

	//終了
	return true;
}
//※通常版
template<class PRINT_FUNC, class CONVERTER_FUNC, typename... Tx>
bool log::convPrintDirect(PRINT_FUNC print_func, CONVERTER_FUNC converter_func, const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	return convPrintDirect(normalOpe, print_func, converter_func, dummyAddMessageFunctor(), level, category, fmt, std::forward<Tx>(args)...);
}
//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
template<class CONVERTER_FUNC, typename... Tx>
inline bool log::convPrintDirect(CONVERTER_FUNC converter_func, const log::level_type level, const log::category_type category, const char* fmt, Tx&&... args)
{
	return convPrintDirect(normalOpe, GASHA_ stdLogPrint(), converter_func, dummyAddMessageFunctor(), level, category, fmt, std::forward<Tx>(args)...);
}

//ログ直接出力：書式なし出力
//※文字コード変換処理指定版
template<class PRINT_FUNC, class CONVERTER_FUNC, class ADD_MESSAGE_FUNC>
bool log::convPutDirect(const log::ope_type ope, PRINT_FUNC print_func, CONVERTER_FUNC converter_func, ADD_MESSAGE_FUNC add_message_func, const log::level_type level, const log::category_type category, const char* str)
{
	//メッセージ付加がある場合、書式付き出力に回してワークバッファを使用した出力を行う
	if (isAddMessage(ope, add_message_func))
		return convPrintDirect(ope, print_func, converter_func, add_message_func, level, category, str);

	bool result = false;

	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask::consolesInfo_type consoles_info;
	if (!consolesInfo(result, consoles_info, level, category))
		return result;

	//ワークバッファ取得
	logWorkBuff work_buff;
	char* converted_str = work_buff.alloc();
	if (!converted_str)
		return false;

	//文字コード変換
	const std::size_t converted_str_len = converter_func(converted_str, logWorkBuff::MAX_MESSAGE_SIZE, str, GASHA_ strlen_fast(str));

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	makeLogPrintInfo(print_info, normalOpe, level, category, converted_str, converted_str_len + 1, consoles_info);

	//直接出力
	print_func(print_info);//出力

	//ワークバッファを解放
	work_buff.free(converted_str);

	//終了
	return true;
}
//※通常版
template<class PRINT_FUNC, class CONVERTER_FUNC>
bool log::convPutDirect(PRINT_FUNC print_func, CONVERTER_FUNC converter_func, const log::level_type level, const log::category_type category, const char* str)
{
	return convPutDirect(normalOpe, print_func, converter_func, dummyAddMessageFunctor(), level, category, str);
}
//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
template<class CONVERTER_FUNC>
inline bool log::convPutDirect(CONVERTER_FUNC converter_func, const log::level_type level, const log::category_type category, const char* str)
{
	return convPutDirect(normalOpe, GASHA_ stdLogPrint(), converter_func, dummyAddMessageFunctor(), level, category, str);
}

//メッセージ付加
inline bool log::addMessage(const log::ope_type ope, char* message, const std::size_t max_size, std::size_t& message_len)
{
	bool result = false;
	if (hasOpe(ope, addCPStack))
	{
		//コールポイントスタックを追加
		GASHA_ callPoint cp;
		message_len += cp.debugInfo(message + message_len, max_size);
		result = true;
	}
	return result;
}

//操作種別判定
inline bool log::hasOpe(const log::ope_type target_ope, const log::ope_type ope)
{
	return (static_cast<std::uint32_t>(target_ope)& static_cast<std::uint32_t>(ope)) != 0;
}

//メッセージ付加操作判定
template<class ADD_MESSAGE_FUNC>
inline bool log::isAddMessage(const log::ope_type ope, ADD_MESSAGE_FUNC add_message_func)
{
	return std::is_same<ADD_MESSAGE_FUNC, dummyAddMessageFunctor>::value || hasOpe(ope, addMessageMask);
}

//ログレベルマスク判定とコンソール取得
inline bool log::consolesInfo(bool& result, GASHA_ logMask::consolesInfo_type& consoles_info, const log::level_type level, const log::category_type category)
{
	GASHA_ logMask mask;
	mask.consolesInfo(consoles_info, level, category);
	if (consoles_info.m_cond == logMask::hasNotAvailableConsole)//利用可能なコンソールがない
	{
		result = false;
		return false;//処理続行不要
	}
	else if (consoles_info.m_cond == logMask::everyConsoleIsDummy)//ダミーコンソールしかない
	{
		result = true;//成功扱い
		return false;//処理続行不要
	}
	return true;//処理続行可能
}

//ログ出力情報を作成
inline void log::makeLogPrintInfo(GASHA_ logPrintInfo& print_info, const log::ope_type ope, const log::level_type level, const log::category_type category, const char* message, const std::size_t message_size, GASHA_ logMask::consolesInfo_type& consoles_info)
{
	//ログ出力情報作成
	print_info.setId(properId(ope));
	print_info.setTime(nowElapsedTime());
	print_info.setMessage(message);
	print_info.setMessageSize(message_size);
	print_info.setLevel(level);
	print_info.setCategory(category);
	print_info.setAttr(*GASHA_ logAttr());
	for (purpose_type purpose = 0; purpose < PURPOSE_NUM; ++purpose)
	{
		print_info.setConsole(purpose, consoles_info.m_consoles[purpose]);
		print_info.setColor(purpose, consoles_info.m_colors[purpose]);
	}
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

//予約IDの取得と更新
inline log::id_type log::reservedId()
{
	id_type id = 0;
	if (m_reservedNum > 0)
	{
		id = m_reservedId;
		--m_reservedNum;
		if (m_reservedNum == 0)
			m_reservedId = 0;
		else
			++m_reservedId;
	}
	return id;
}

//適切なIDの取得
inline log::id_type log::properId(const log::ope_type ope)
{
	return (ope & useReservedId) ? reservedId() : 0;
}

//ログキューをエンキュー
inline bool log::enqueue(GASHA_ logPrintInfo& print_info)
{
	GASHA_ logQueue queue;
	return queue.enqueue(print_info);
}

//ログキューモニターに通知
inline bool log::notifyMonitor()
{
	GASHA_ logQueueMonitor mon;
	return mon.notify();
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

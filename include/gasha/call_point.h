#pragma once
#ifndef GASHA_INCLUDED_CALL_POINT_H
#define GASHA_INCLUDED_CALL_POINT_H

//--------------------------------------------------------------------------------
// call_point.h
// コールポイント【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstdint>//std::uintptr_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//コールポイント
//--------------------------------------------------------------------------------

	//----------------------------------------
	//コールポイント用定数
	enum controlEnum : unsigned char//操作設定
	{
		recProfile = 0x00,//プロファイル記録
		noProfile = 0x01,//プロファイル不要
	};
	//----------------------------------------
	//コールポイントクラス
	class callPoint : public message
	{
	public:
		//型
		typedef std::uintptr_t key_t;//キー型
		typedef std::chrono::high_resolution_clock::time_point clock_t;//クロック型
		typedef double duration_t;//処理時間型
		typedef unsigned char control_t;//操作設定型
	public:
		//アクセッサ
		key_t getKey() const{ return m_key; }//キー
		clock_t getBegintime() const{ return m_beginTime; }//処理開始時間
	public:
		//メソッド
		//経過時間取得
		duration_t getElapsedTime() const
		{
			const clock_t end_time = std::chrono::high_resolution_clock::now();
			return static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end_time - m_beginTime).count()) / 1000000.;
		}
	public:
		//コンストラクタ
		callPoint(const level::value_t level_, const category::value_t category_, const char* name, const controlEnum control, const key_t key, const char* src_file_name, const char* func_name) :
			message(level_, category_, name, src_file_name, func_name),
			m_key(key),
			m_control(control),
			m_beginTime(std::chrono::high_resolution_clock::now())
		{
			pushCallPoint();//コールポイントをプッシュ
		}
		//デストラクタ
		~callPoint()
		{
			if (!(m_control & noProfile))
			{
				//スレッド名取得
				CThreadID thread_id;
				
				//パフォーマンス記録
				CProfiler profiler;
				profiler.addProfile(m_key, m_name, thread_id, getFileName(m_srcFileName), m_funcName, getElapsedTime());
			}
			//popCallPoint();//コールポイントをポップ　※ポップは親クラスが行うのでここでは不要
		}
	private:
		//フィールド
		const key_t m_key;//キー
		const control_t m_control;//操作設定
		const clock_t m_beginTime;//処理開始時間
	};
	template<level::value_t L>
	class callPointAsLevel : public callPoint
	{
	public:
		//定数
		static const level::value_t LEVEL = L;//レベル
	public:
		//コンストラクタ
		callPointAsLevel(const category::value_t category_, const char* name, const controlEnum control, const key_t key, const char* src_file_name, const char* func_name) :
			callPoint(LEVEL, category_, name, control, key, src_file_name, func_name)
		{}
		//デストラクタ
		~callPointAsLevel()
		{}
	};
#define declare_callPointClass(level_) using callPointAs##level_ = callPointAsLevel<as##level_>
	declare_callPointClass(Normal);//通常メッセージ
	declare_callPointClass(Verbose);//冗長メッセージ
	declare_callPointClass(Detail);//詳細メッセージ
	declare_callPointClass(Important);//重要メッセージ
	declare_callPointClass(Warning);//警告メッセージ
	declare_callPointClass(Critical);//重大（問題）メッセージ
	declare_callPointClass(Absolute);//絶対（必須）メッセージ
#define __CPARGS() reinterpret_cast<callPoint::key_t>(GET_CONCATENATED_SOURCE_FILE_NAME()), GET_CONCATENATED_SOURCE_FILE_NAME(), GET_FUNC_NAME()
	//----------------------------------------
	//関数
	//現在の表示属性を取得
	message::attr_t getAttr(){ return message::getAttrG(); }
	//現在の表示属性を変更
	//void setAttr(const message::attr_t attr){ message::setAttrG(attr); }
	message::attr_t addAttr(const message::attr_t attr){ return message::addAttrG(attr); }
	message::attr_t delAttr(const message::attr_t attr){ return message::delAttrG(attr); }
	//現在の表示属性をリセット
	void resetAttr(){ message::resetAttrG(); }
	//現在のログレベルを取得
	level::value_t getLogLevel(const category::value_t category_)
	{
		return message::getLogLevelG(category_);
	}
	//現在のログレベルを変更
	//※指定の値以上のレベルのメッセージのみをログ出力する
	level::value_t setLogLevel(const level::value_t level_, const category::value_t category_)
	{
		return message::setLogLevelG(level_, category_);
	}
	//現在のログレベルをリセット
	void resetLogLevel(const category::value_t category_)
	{
		message::resetLogLevelG(category_);
	}
	//現在のログレベルを全てリセット
	void resetLogLevelAll()
	{
		message::resetLogLevelAllG();
	}
	//現在の画面通知レベルを取得
	level::value_t getNoticeLevel(const category::value_t category_)
	{
		return message::getNoticeLevelG(category_);
	}
	//現在の画面通知レベルを変更
	//※指定の値以上のレベルのメッセージのみをログ出力する
	level::value_t setNoticeLevel(const level::value_t level_, const category::value_t category_)
	{
		return message::setNoticeLevelG(level_, category_);
	}
	//現在の画面通知レベルをリセット
	void resetNoticeLevel(const category::value_t category_)
	{
		message::resetNoticeLevelG(category_);
	}
	//現在の画面通知レベルを全てリセット
	void resetNoticeLevelAll()
	{
		message::resetNoticeLevelAllG();
	}
	//メッセージ出力／ログ出力／画面通知関数：レベルを引数指定
	//※vprint***/vlog***/vnotice***
	//※va_listを引数にとるバージョン
	static int vprint(const level::value_t level_, const category::value_t category_, const char* fmt, va_list args)
	{
		return message::vprintG(level_, category_, fmt, args);
	}
	static int vlog(const level::value_t level_, const category::value_t category_, const char* fmt, va_list args)
	{
		return message::vlogG(level_, category_, fmt, args);
	}
	static int vnotice(const level::value_t level_, const category::value_t category_, const char* fmt, va_list args)
	{
		return message::vnoticeG(level_, category_, fmt, args);
	}
	//メッセージ出力／ログ出力／画面通知関数：レベルを引数指定
	//※print***/log***/notice***
	//※可変長引数バージョン
	static int print(const level::value_t level_, const category::value_t category_, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		const int ret = message::vprintG(level_, category_, fmt, args);
		va_end(args);
		return ret;
	}
	static int log(const level::value_t level_, const category::value_t category_, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		const int ret = message::vlogG(level_, category_, fmt, args);
		va_end(args);
		return ret;
	}
	static int notice(const level::value_t level_, const category::value_t category_, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		const int ret = message::vnoticeG(level_, category_, fmt, args);
		va_end(args);
		return ret;
	}
	//メッセージ出力／ログ出力／画面通知関数定義マクロ
	//※vprint***/vlog***/vnotice***
	//※va_listを引数にとるバージョン
#define declare_vprintFuncs(level_) \
	static int vprintAs##level_(const category::value_t category_, const char* fmt, va_list args) \
	{ \
		return message::vprintAs##level_##G(category_, fmt, args); \
	} \
	static int vlogAs##level_(const category::value_t category_, const char* fmt, va_list args) \
	{ \
		return message::vlogAs##level_##G(category_, fmt, args); \
	} \
	static int vnoticeAs##level_(const category::value_t category_, const char* fmt, va_list args) \
	{ \
		return message::vnoticeAs##level_##G(category_, fmt, args); \
	}
	//メッセージ出力／ログ出力／画面通知関数定義マクロ
	//※print***/log***/notice***
	//※可変長引数バージョン
#define declare_printFuncs(level_) \
	static int printAs##level_(const category::value_t category_, const char* fmt, ...) \
	{ \
		va_list args; \
		va_start(args, fmt); \
		const int ret = message::vprintAs##level_##G(category_, fmt, args); \
		va_end(args); \
		return ret; \
	} \
	static int logAs##level_(const category::value_t category_, const char* fmt, ...) \
	{ \
		va_list args; \
		va_start(args, fmt); \
		const int ret = message::vlogAs##level_##G(category_, fmt, args); \
		va_end(args); \
		return ret; \
	} \
	static int noticeAs##level_(const category::value_t category_, const char* fmt, ...) \
	{ \
		va_list args; \
		va_start(args, fmt); \
		const int ret = message::vnoticeAs##level_##G(category_, fmt, args); \
		va_end(args); \
		return ret; \
	}
	//メッセージ出力／ログ出力／画面通知関数定義
	//※vprint***/vlog***/vnotice***
	//※va_listを引数にとるバージョン
	declare_vprintFuncs(Normal);//通常メッセージ
	declare_vprintFuncs(Verbose);//冗長メッセージ
	declare_vprintFuncs(Detail);//詳細メッセージ
	declare_vprintFuncs(Important);//重要メッセージ
	declare_vprintFuncs(Warning);//警告メッセージ
	declare_vprintFuncs(Critical);//重大（問題）メッセージ
	declare_vprintFuncs(Absolute);//絶対（必須）メッセージ
	//メッセージ出力／ログ出力／画面通知関数定義
	//※print***/log***/notice***
	//※可変長引数バージョン
	declare_printFuncs(Normal);//通常メッセージ
	declare_printFuncs(Verbose);//冗長メッセージ
	declare_printFuncs(Detail);//詳細メッセージ
	declare_printFuncs(Important);//重要メッセージ
	declare_printFuncs(Warning);//警告メッセージ
	declare_printFuncs(Critical);//重大（問題）メッセージ
	declare_printFuncs(Absolute);//絶対（必須）メッセージ
	//コールポイントスタック表示
	void printCPStack(const level::value_t level_, category::value_t category_, const char* name, const message::control_t control = simplePrint)
	{
		message::printCPStackG(level_, category_, name, control);
	}
#define declare_printCPFunc(level_) \
	static void printCPStackAs##level_(const category::value_t category_, const char* name, const message::control_t control = simplePrint) \
	{ \
		message::printCPStackAs##level_##G(category_, name, control); \
	}
	declare_printCPFunc(Normal);//通常メッセージ
	declare_printCPFunc(Verbose);//冗長メッセージ
	declare_printCPFunc(Detail);//詳細メッセージ
	declare_printCPFunc(Important);//重要メッセージ
	declare_printCPFunc(Warning);//警告メッセージ
	declare_printCPFunc(Critical);//重大（問題）メッセージ
	declare_printCPFunc(Absolute);//絶対（必須）メッセージ

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/call_point.inl>

#endif//GASHA_INCLUDED_CALL_POINT_H

// End of file

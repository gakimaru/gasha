#pragma once
#ifndef GASHA_INCLUDED_DEBUG_LOG_H
#define GASHA_INCLUDED_DEBUG_LOG_H

//--------------------------------------------------------------------------------
// debug_log.h
// ログ操作【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_print_info.h>//ログ出力情報
#include <gasha/log_purpose.h>//ログ用途
#include <gasha/log_level.h>//ログレベル
#include <gasha/log_category.h>//ログカテゴリ
#include <gasha/log_mask.h>//ログレベルマスク

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ操作
//--------------------------------------------------------------------------------
//【注意】UTF-16, UTF-32（通常文字の文字コードに'\0'が含まれるもの）には非対応
//【注意】文字列中の改行コード変換は行わない（'\r', '\n' はそのまま出力する）
//【注意】文字コード変換処理を指定した場合、かつ、書式付き出力時は、ワークバッファ（logWorkBuff）の半分のサイズの文字列しか扱えない
//--------------------------------------------------------------------------------

//----------------------------------------
//ログ操作
//※指定のログレベルとログカテゴリに応じて、マスクされていない時だけ実際にコンソールに出力される。
//※ログ出力時はログ出力属性が適用され、ログレベル名やログカテゴリ名などを付加することができる。
//※ログレベルマスクを変更したい場合は、別途 logMask を使用する。
//※ログ出力属性を変更したい場合は、別途 logAttr を使用する。
class debugLog
{
	friend class breakPoint;
public:
	//型
	typedef GASHA_ logPrintInfo::id_type id_type;//ログID
	typedef GASHA_ logPurpose::purpose_type purpose_type;//ログ用途の値
	typedef GASHA_ logLevel::level_type level_type;//ログレベルの値
	typedef GASHA_ logCategory::category_type category_type;//ログカテゴリの値
public:
	//定数
	static const std::size_t PURPOSE_NUM = GASHA_ logPurpose::NUM;//ログ用途の数

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

private:
	enum ope_type : std::uint32_t//ログ操作種別
	{
		normalOpe = 0x0000,//属性なし

		useReservedId = 0x0001,//予約IDを使用
		addCPStack = 0x0002,//コールポイントスタックを付加

		addMessageMask = (addCPStack),//メッセージ付加あり判定マスク（メッセージ付加がある場合、必ずワークバッファを使用してメッセ―ジ出力する）
	};

private:
	//ダミーメッセージ追加用関数オブジェクト
	struct dummyAddMessageFunctor{
		std::size_t operator()(char* message, const std::size_t max_size, std::size_t& pos){ return 0; }
	};
public:
	//メソッド
	
	//--------------------
	//ログ出力
	//※キューを使用して出力するため、マルチスレッドで出力が干渉することがない。
	//※キューからコンソールに出力するために、logQueueMonitor を専用スレッドで動作させておく必要がある。
	//※書式付き出力時は専用のワークバッファを使用するため、スタックサイズの小さなスレッドからでも問題なく出力可能。
	
private:
	//ログ出力：書式付き出力
	template<class ADD_MESSAGE_FUNC, typename... Tx>
	bool print(const ope_type ope, ADD_MESSAGE_FUNC add_message_func, const level_type level, const category_type category, const char* fmt, Tx&&... args);
public:
	//※通常版
	template<typename... Tx>
	inline bool print(const level_type level, const category_type category, const char* fmt, Tx&&... args);
	//※予約出力版
	template<typename... Tx>
	inline bool reservedPrint(const char* fmt, Tx&&... args);

private:
	//ログ出力：書式なし出力
	template<class ADD_MESSAGE_FUNC>
	bool put(const ope_type ope, ADD_MESSAGE_FUNC add_message_func, const level_type level, const category_type category, const char* str);
public:
	//※通常版
	inline bool put(const level_type level, const category_type category, const char* str);
	//※予約出力版
	inline bool reservedPut(const char* str);

	//----------
	//※文字コード変換処理指定版：max_dst_sizeは終端を含めた出力バッファサイズ、src_len は終端を含まない入力文字列長、戻り値は終端を含まない出力文字列長
	//　CONVERTER_FUNC のプロトタイプ：std::size_t converter_func(char* dst, const std::size_t max_dst_size, const char* src, const std::size_t src_len)
	
private:
	//ログ出力：書式付き出力
	//※文字コード変換処理指定版
	template<class CONVERTER_FUNC, class ADD_MESSAGE_FUNC, typename... Tx>
	inline bool convPrint(const ope_type ope, CONVERTER_FUNC converter_func, ADD_MESSAGE_FUNC add_message_func, const level_type level, const category_type category, const char* fmt, Tx&&... args);
public:
	//※通常版
	template<class CONVERTER_FUNC, typename... Tx>
	inline bool convPrint(CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* fmt, Tx&&... args);
	//※予約出力版
	template<class CONVERTER_FUNC, typename... Tx>
	inline bool reservedConvPrint(CONVERTER_FUNC converter_func, const char* fmt, Tx&&... args);

private:
	//ログ出力：書式なし出力
	//※文字コード変換処理指定版
	template<class CONVERTER_FUNC, class ADD_MESSAGE_FUNC>
	bool convPut(const ope_type ope, CONVERTER_FUNC converter_func, ADD_MESSAGE_FUNC add_message_func, const level_type level, const category_type category, const char* str);
public:
	//※通常版
	template<class CONVERTER_FUNC>
	inline bool convPut(CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* str);
	//※予約出力版
	template<class CONVERTER_FUNC>
	inline bool reservedConvPut(CONVERTER_FUNC converter_func, const char* str);

	//--------------------
	//ログ直接出力
	//※キューを使用せずに直接出力する。
	//※書式付き出力時は専用のワークバッファを使用するため、スタックサイズの小さなスレッドからでも問題なく出力可能。
	//※ログ出力処理数を指定する。（省略時は stdLogPrint を使用）
	//　PRINT_FUNC のプロトタイプ：void print_func(GASHA_ logPrintInfo& info)

private:
	//ログ直接出力：書式付き出力
	template<class PRINT_FUNC, class ADD_MESSAGE_FUNC, typename... Tx>
	bool printDirect(const ope_type ope, PRINT_FUNC print_func, ADD_MESSAGE_FUNC add_message_func, const level_type level, const category_type category, const char* fmt, Tx&&... args);
public:
	//※通常版
	template<class PRINT_FUNC, typename... Tx>
	bool printDirect(PRINT_FUNC print_func, const level_type level, const category_type category, const char* fmt, Tx&&... args);
	//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
	template<typename... Tx>
	inline bool printDirect(const level_type level, const category_type category, const char* fmt, Tx&&... args);
	
private:
	//ログ直接出力：書式なし出力
	template<class PRINT_FUNC, class ADD_MESSAGE_FUNC>
	bool putDirect(const ope_type ope, PRINT_FUNC print_func, ADD_MESSAGE_FUNC add_message_func, const level_type level, const category_type category, const char* str);
public:
	//※通常版
	template<class PRINT_FUNC>
	bool putDirect(PRINT_FUNC print_func, const level_type level, const category_type category, const char* str);
	//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
	inline bool putDirect(const level_type level, const category_type category, const char* str);
	
	//----------
	//※文字コード変換処理指定版：max_dst_sizeは終端を含めた出力バッファサイズ、src_len は終端を含まない入力文字列長、戻り値は終端を含まない出力文字列長
	//　CONVERTER_FUNC のプロトタイプ：std::size_t converter_func(char* dst, const std::size_t max_dst_size, const char* src, const std::size_t src_len)
	
private:
	//ログ直接出力：書式付き出力
	//※文字コード変換処理指定版
	template<class PRINT_FUNC, class CONVERTER_FUNC, class ADD_MESSAGE_FUNC, typename... Tx>
	bool convPrintDirect(const ope_type ope, PRINT_FUNC print_func, CONVERTER_FUNC converter_func, ADD_MESSAGE_FUNC add_message_func, const level_type level, const category_type category, const char* fmt, Tx&&... args);
public:
	//※通常版
	template<class PRINT_FUNC, class CONVERTER_FUNC, typename... Tx>
	bool convPrintDirect(PRINT_FUNC print_func, CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* fmt, Tx&&... args);
	//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
	template<class CONVERTER_FUNC, typename... Tx>
	inline bool convPrintDirect(CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* fmt, Tx&&... args);
	
private:
	//ログ直接出力：書式なし出力
	//※文字コード変換処理指定版
	template<class PRINT_FUNC, class CONVERTER_FUNC, class ADD_MESSAGE_FUNC>
	bool convPutDirect(const ope_type ope, PRINT_FUNC print_func, CONVERTER_FUNC converter_func, ADD_MESSAGE_FUNC add_message_func, const level_type level, const category_type category, const char* str);
public:
	//※通常版
	template<class PRINT_FUNC, class CONVERTER_FUNC>
	bool convPutDirect(PRINT_FUNC print_func, CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* str);
	//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
	template<class CONVERTER_FUNC>
	inline bool convPutDirect(CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* str);

	//--------------------

private:
	//メッセージ付加
	inline bool addMessage(const ope_type ope, char* message, const std::size_t max_size, std::size_t& message_len);

	//--------------------

private:
	//操作種別判定
	inline bool hasOpe(const ope_type target_ope, const ope_type ope);
	//メッセージ付加操作判定
	template<class ADD_MESSAGE_FUNC >
	inline bool isAddMessage(const ope_type ope, ADD_MESSAGE_FUNC add_message_func);
	//ログレベルマスク判定とコンソール取得
	//※戻り値が false を返したら続行不要と判定し、result を返して終了する
	inline bool consolesInfo(bool& result, GASHA_ logMask::consolesInfo_type& consoles_info, const level_type level, const category_type category);
	//ログ出力情報を作成
	inline void makeLogPrintInfo(GASHA_ logPrintInfo& print_info, const ope_type ope, const level_type level, const category_type category, const char* message, const std::size_t message_size, GASHA_ logMask::consolesInfo_type& consoles_info);
	
	//--------------------

public:
	//ログ出力予約
	//※キューイングを予約し、実際の出力が遅延しても予約順にコンソールに出力されることを保証する。
	//※予約後は reservedPrint(), reservedPut() を実行すると、予約分として出力される。
	//※あまりに遅延が長いと順序が無視される。（予約分の出力を待っているキューが先に出力されてしまう）
	//※複数回予約しても加算されず、前回の予約分が取り消される。
	inline bool reserve(const level_type level, const category_type category, const int num);

	//ログ出力予約を取り消す
	inline bool cancelToReserve();

private:
	//予約IDの取得と更新
	inline id_type reservedId();
	//適切なIDの取得
	inline id_type properId(const ope_type ope);

private:
	//ログキューをエンキュー
	inline bool enqueue(GASHA_ logPrintInfo& print_info);
	//ログキューモニターに通知
	inline bool notifyMonitor();

public:
	//【使用注意】ログ関係の処理を一括して初期化する
	//※ログレベル、ログカテゴリ、ログレベルマスク、ログ出力属性、ログワークバッファ、ログキュー、ログキューモニターをまとめて初期化
	//※プロファイラーは初期化しない
	void initialize();

	//【使用注意】ログキューモニターに溜まっているキューを全て出力する
	//※出力の完了を待機する。
	//※ログキューモニター起動状態もしくは中断済みじゃないと、待機状態から復帰できないので注意。誤って実行するとデッドロックになる。
	void flush();

	//【使用注意】ログ関係の処理を一括して中断する
	//※ログワークバッファとログキュー、ログキュモニターを中断する。
	//※中断後は初期化しないと再利用できないので注意。
	void abort();

	//【使用注意】ログ関係の処理を一括して一時停止する
	//※ログワークバッファとログキューを一時停止し、ログモニターをフラッシュする。
	//※一時停止中に print(), put(), printDirect() を実行すると待機状態になるので注意。誤って実行するとデッドロックになる。
	//　なお、putDirect() だけは使用可能。
	//※ブレークポイント時（ウォッチポインタ／アサーション違反時含む）に自動的に実行される。
	void pause();

	//【使用注意】ログ関係の処理を一括して一時停止解除する
	//※ログワークバッファとログキューの一時停止を解除する。
	void resume();

public:
	//コンストラクタ
	inline debugLog();
	//デストラクタ
	inline ~debugLog();

private:
	//フィールド
	id_type m_reservedId;//予約ID（現在値）
	id_type m_reservedNum;//予約IDの数（出力するごとに減算する）
	level_type m_reservedLevel;//予約レベル
	category_type m_reservedCategory;//予約カテゴリ

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//メソッド

	//ログ出力：書式付き出力
	template<typename... Tx>
	inline bool print(const level_type level, const category_type category, const char* fmt, Tx&&... args){ return true; }
	//※予約出力版
	template<typename... Tx>
	inline bool reservedPrint(const char* fmt, Tx&&... args){ return true; }
	//ログ出力：書式なし出力
	inline bool put(const level_type level, const category_type category, const char* str){ return true; }
	//※予約出力版
	inline bool reservedPut(const char* str){ return true; }
	//ログ出力：書式付き出力
	//※文字コード変換処理指定版
	template<class CONVERTER_FUNC, typename... Tx>
	inline bool convPrint(CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* fmt, Tx&&... args){ return true; }
	//※予約出力版
	template<class CONVERTER_FUNC, typename... Tx>
	inline bool reservedConvPrint(CONVERTER_FUNC converter_func, const char* fmt, Tx&&... args){ return true; }
	//ログ出力：書式なし出力
	template<class CONVERTER_FUNC>
	inline bool convPut(CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* str){ return true; }
	//※予約出力版
	template<class CONVERTER_FUNC>
	inline bool reservedConvPut(CONVERTER_FUNC converter_func, const char* str){ return true; }
	//ログ直接出力：書式付き出力
	template<class PRINT_FUNC, typename... Tx>
	inline bool printDirect(PRINT_FUNC print_func, const level_type level, const category_type category, const char* fmt, Tx&&... args){ return true; }
	//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
	template<typename... Tx>
	inline bool printDirect(const level_type level, const category_type category, const char* fmt, Tx&&... args){ return true; }
	//ログ直接出力：書式なし出力
	template<class PRINT_FUNC>
	inline bool putDirect(PRINT_FUNC print_func, const level_type level, const category_type category, const char* str){ return true; }
	//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
	inline bool putDirect(const level_type level, const category_type category, const char* str){ return true; }
	//ログ直接出力：書式付き出力
	template<class PRINT_FUNC, class CONVERTER_FUNC, typename... Tx>
	inline bool convPrintDirect(PRINT_FUNC print_func, CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* fmt, Tx&&... args){ return true; }
	//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
	template<class CONVERTER_FUNC, typename... Tx>
	inline bool convPrintDirect(CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* fmt, Tx&&... args){ return true; }
	//ログ直接出力：書式なし出力
	template<class PRINT_FUNC, class CONVERTER_FUNC>
	inline bool convPutDirect(PRINT_FUNC print_func, CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* str){ return true; }
	//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
	template<class CONVERTER_FUNC>
	inline bool convPutDirect(CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* str){ return true; }
	
	inline bool reserve(const level_type level, const category_type category, const int num){ return true; }//ログ出力予約
	inline bool cancelToReserve(){ return true; }//ログ出力予約を取り消す
	inline void initialize(){}//ログ関係の処理を一括して初期化する
	inline void flush(){}//ログキューモニターに溜まっているキューを全て出力する
	inline void abort(){}//ログ関係の処理を一括して中断する
	inline void pause(){}//ログ関係の処理を一括して一時停止する
	inline void resume(){}//ログ関係の処理を一括して一時停止解除する

public:
	inline debugLog(){}//コンストラクタ
	inline ~debugLog(){}//デストラクタ

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/debug_log.inl>

#endif//GASHA_INCLUDED_DEBUG_LOG_H

// End of file

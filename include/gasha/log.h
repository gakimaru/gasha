#pragma once
#ifndef GASHA_INCLUDED_LOG_H
#define GASHA_INCLUDED_LOG_H

//--------------------------------------------------------------------------------
// log.h
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

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ操作
//--------------------------------------------------------------------------------
//【注意】UTF-16, UTF-32（通常文字の文字コードに'\0'が含まれるもの）には非対応
//【注意】文字列中の改行コード変換は行わない（'\r', '\n' はそのまま出力する）
//【注意】文字コード変換処理を指定した場合、かつ、書式付き出力時は、ワークバッファ（logWorkBuff）の半分のサイズの文字列しか扱えない
//--------------------------------------------------------------------------------

//デバッグログ無効時は非inline関数として処理するものを指定
#ifdef GASHA_HAS_DEBUG_LOG
#define GASHA_INLINE
#else//GASHA_HAS_DEBUG_LOG
#define GASHA_INLINE inline
#endif//GASHA_HAS_DEBUG_LOG

//----------------------------------------
//ログ操作
//※指定のログレベルとログカテゴリに応じて、マスクされていない時だけ実際にコンソールに出力される。
//※ログ出力時はログ出力属性が適用され、ログレベル名やログカテゴリ名などを付加することができる。
//※ログレベルマスクを変更したい場合は、別途 logMask を使用する。
//※ログ出力属性を変更したい場合は、別途 logAttr を使用する。
class log
{
public:
#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化
	//型
	typedef GASHA_ logPrintInfo::id_type id_type;//ログID
	typedef GASHA_ logPurpose::purpose_type purpose_type;//ログ用途の値
	typedef GASHA_ logLevel::level_type level_type;//ログレベルの値
	typedef GASHA_ logCategory::category_type category_type;//ログカテゴリの値
	//定数
	static const std::size_t PURPOSE_NUM = GASHA_ logPurpose::NUM;//ログ用途の数
#else//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化
	typedef std::uint8_t purpose_type;//ログ用途の値
	typedef std::uint8_t level_type;//ログレベルの値
	typedef std::uint8_t category_type;//ログカテゴリの値
	//定数
	static const std::size_t PURPOSE_NUM = 2;//ログ用途の数
#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化
public:
	//--------------------
	//ログ出力
	//※キューを使用して出力するため、マルチスレッドで出力が干渉することがない。
	//※キューからコンソールに出力するために、logQueueMonitor を専用スレッドで動作させておく必要がある。
	//※書式付き出力時は専用のワークバッファを使用するため、スタックサイズの小さなスレッドからでも問題なく出力可能。
	
public:
	//ログ出力：書式付き出力
	template<typename... Tx>
	inline bool print(const level_type level, const category_type category, const char* fmt, Tx&&... args);
	//※予約出力版
	template<typename... Tx>
	inline bool reservedPrint(const char* fmt, Tx&&... args);
private:
	//※共通処理（プライベート）
	template<typename... Tx>
	bool print(const bool is_reserved, const level_type level, const category_type category, const char* fmt, Tx&&... args);

public:
	//ログ出力：書式なし出力
	inline bool put(const level_type level, const category_type category, const char* str);
	//※予約出力版
	inline bool reservedPut(const char* str);
private:
	//※共通処理（プライベート）
	bool put(const bool is_reserved, const level_type level, const category_type category, const char* str);

	//----------
	//※文字コード変換処理指定版：max_dst_sizeは終端を含めた出力バッファサイズ、src_len は終端を含まない入力文字列長、戻り値は終端を含まない出力文字列長
	//　CONVERTER_FUNC のプロトタイプ：std::size_t converter_func(char* dst, const std::size_t max_dst_size, const char* src, const std::size_t src_len)
	
public:
	//ログ出力：書式付き出力
	//※文字コード変換処理指定版
	template<class CONVERTER_FUNC, typename... Tx>
	inline bool convPrint(CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* fmt, Tx&&... args);
	//※予約出力版
	template<class CONVERTER_FUNC, typename... Tx>
	inline bool reservedConvPrint(CONVERTER_FUNC converter_func, const char* fmt, Tx&&... args);
private:
	//※共通処理（プライベート）
	template<class CONVERTER_FUNC, typename... Tx>
	inline bool convPrint(const bool is_reserved, CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* fmt, Tx&&... args);

public:
	//ログ出力：書式なし出力
	//※文字コード変換処理指定版
	template<class CONVERTER_FUNC>
	inline bool convPut(CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* str);
	//※予約出力版
	template<class CONVERTER_FUNC>
	inline bool reservedConvPut(CONVERTER_FUNC converter_func, const char* str);
private:
	//※共通処理（プライベート）
	template<class CONVERTER_FUNC>
	bool convPut(const bool is_reserved, CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* str);

public:
	//--------------------
	//ログ直接出力
	//※キューを使用せずに直接出力する。
	//※書式付き出力時は専用のワークバッファを使用するため、スタックサイズの小さなスレッドからでも問題なく出力可能。
	//※ログ出力処理数を指定する。（省略時は stdLogPrint を使用）
	//　PRINT_FUNC のプロトタイプ：void print_func(GASHA_ logPrintInfo& info)

	//ログ直接出力：書式付き出力
	template<class PRINT_FUNC, typename... Tx>
	GASHA_INLINE bool printDirect(PRINT_FUNC print_func, const level_type level, const category_type category, const char* fmt, Tx&&... args);
	//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
	template<typename... Tx>
	inline bool printDirect(const level_type level, const category_type category, const char* fmt, Tx&&... args);
	
	//ログ直接出力：書式なし出力
	template<class PRINT_FUNC>
	GASHA_INLINE bool putDirect(PRINT_FUNC print_func, const level_type level, const category_type category, const char* str);
	//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
	inline bool putDirect(const level_type level, const category_type category, const char* str);
	
	//----------
	//※文字コード変換処理指定版：max_dst_sizeは終端を含めた出力バッファサイズ、src_len は終端を含まない入力文字列長、戻り値は終端を含まない出力文字列長
	//　CONVERTER_FUNC のプロトタイプ：std::size_t converter_func(char* dst, const std::size_t max_dst_size, const char* src, const std::size_t src_len)
	
	//ログ直接出力：書式付き出力
	//※文字コード変換処理指定版
	template<class PRINT_FUNC, class CONVERTER_FUNC, typename... Tx>
	GASHA_INLINE bool convPrintDirect(PRINT_FUNC print_func, CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* fmt, Tx&&... args);
	//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
	template<class CONVERTER_FUNC, typename... Tx>
	inline bool convPrintDirect(CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* fmt, Tx&&... args);
	
	//ログ直接出力：書式なし出力
	//※文字コード変換処理指定版
	template<class PRINT_FUNC, class CONVERTER_FUNC>
	GASHA_INLINE bool convPutDirect(PRINT_FUNC print_func, CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* str);
	//※ログ出力処理省略版（標準ログ出力：stdLogPrint 使用）
	template<class CONVERTER_FUNC>
	inline bool convPutDirect(CONVERTER_FUNC converter_func, const level_type level, const category_type category, const char* str);

	//--------------------

	//ログ出力予約
	//※キューイングを予約し、実際の出力が遅延しても予約順にコンソールに出力されることを保証する。
	//※予約後は reservedPrint(), reservedPut() を実行すると、予約分として出力される。
	//※あまりに遅延が長いと順序が無視される。（予約分の出力を待っているキューが先に出力されてしまう）
	//※複数回予約しても加算されず、前回の予約分が取り消される。
	inline bool reserve(const level_type level, const category_type category, const int num);

	//ログ出力予約を取り消す
	inline bool cancelToReserve();

	//【使用注意】ログ関係の処理を一括して初期化する
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
	inline log();

	//デストラクタ
	inline ~log();

private:
	//フィールド
#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化
	id_type m_reservedId;//予約ID（現在値）
	id_type m_reservedNum;//予約IDの数（出力するごとに減算する）
	level_type m_reservedLevel;//予約レベル
	category_type m_reservedCategory;//予約カテゴリ
#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化
};

//マクロ削除
#undef GASHA_INLINE

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log.inl>

#endif//GASHA_INCLUDED_LOG_H

// End of file

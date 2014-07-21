#pragma once
#ifndef GASHA_INCLUDED_ASSERT_H
#define GASHA_INCLUDED_ASSERT_H

//--------------------------------------------------------------------------------
// assert.h
// アサーション／ブレークポイント／ウォッチポイント【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/print.h>//ログ出力

#include <gasha/i_debug_pause.h>//デバッグポーズインターフェース

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アサーション／ブレークポイント／ウォッチポイント
//--------------------------------------------------------------------------------

//※ GASHA_ASSERT(), GASHA_BREAKPOINT(), GASHA_WATCHPOINT() のいずれかのマクロを使用してアサーション／ブレークポイント／ウォッチポイントを指定する。
//※アサーションは、条件判定式の結果が false の時にアサーション違反となり、指定のメッセージをログ出力し、ブレークする。
//※ブレークポイントは、条件判定式を指定せず、無条件に指定のメッセージをログ出力し、ブレークする。
//※ウォッチポイントは、条件判定式の結果が true の時に、指定のメッセージをログ出力し、ブレークする。
//※メッセージのログ出力は、通常のログ出力の設定に基づく。出力先のコンソールやメッセージのカラー付加、ヘッダー付加など全て同じ。
//※メッセージの出力内容には、条件判定式（文字列）とそのソースファイル情報、および、コールポイントスタックを伴う。
//※ブレーク時は、デバッガへのブレークポイント割り込みと、（ユーザー定義の）ポーズ処理呼び出しを行う。
//※通常のログ出力と同様に、アサーション／ブレークポイント／ウォッチポイントを指定する際、「ログレベル」と「ログカテゴリ」を指定する。
//※通常のログ出力と同様に、「ログレベル」と「ログカテゴリ」に応じたマスク判定が行われる。
//　マスクされていると、ログ出力もブレークもしない。なお、マスク判定は「ログ出力用設定」に基づく（画面通知用設定は見ない）
//※ユーザー定義のポーズ処理を指定には、breakPoint クラスを使用する。
//　指定されたポーズ処理は、自スレッドに対してのみ有効となる。（メインスレッド以外はポーズできない可能性がある、もしくは、メインスレッドにポーズを要求する必要がある）

//※ビルド設定 GASHA_LOG_PRINT_USE_QUEUE が指定された場合、この関数はログキューを使用した出力を行う。（未指定時は直接出力する）
//※ログキューを使用した場合、ブレーク状態が解除されるまで、ログ出力を一時停止状態にする。（直接出力時は何もしない）
//　これにより、他のスレッドが出力処理を行おうとすると、待機状態になる。【注意】ポーズ中に自スレッドでログ出力を行うとデッドロックする。
//　なお、待機中は、defaultContexstSwitch を呼びながらループし続けるので、ポーズ中のCPU使用率が高すぎる場合は、
//　ビルド設定の GASHA_DEFAULT_CONTEXT_SWITH_IS_**** を設定することで、待機時間を長くし、負荷を軽減できる。

//----------------------------------------
//ブレークポイントクラス
//※アサーション／ブレークポイント／ウォッチポイントを兼ねる。
//※マクロから関数オブジェクトとして呼び出される。
//※現在のスレッド用のポーズ処理の置き換え処理に対応する。
class breakPoint
{
#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化
	
public:
	//関数オペレータ
	template<typename... Tx>
	bool operator()(const bool conditon, const GASHA_ debugLog::level_type level, const GASHA_ debugLog::category_type category, const char* expr, const char* file_name, const char* func_name, const char* message, Tx&&... args);

public:
	//デバッグポーズ処理の参照／変更
	inline iDebugPause* debugPause();//取得
	inline iDebugPause* changeDebugPause(iDebugPause* new_debug_pause);//変更 ※変更前のオブジェクトを返す
	inline iDebugPause* changeDebugPause(iDebugPause& new_debug_pause);//変更 ※変更前のオブジェクトを返す
	inline iDebugPause* resetDebugPause();//標準処理に変更 ※変更前のオブジェクトを返す

public:
	//デフォルトコンストラクタ
	inline breakPoint();
	//デストラクタ
	inline ~breakPoint();

private:
	//TLS静的フィールド
	static thread_local iDebugPause* m_tlsDebugPause;//TLSデバッグポーズ処理オブジェクト

#else//GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

public:
	//関数オペレータ
	template<typename... Tx>
	inline bool operator()(const bool conditon, const GASHA_ debugLog::level_type level, const GASHA_ debugLog::category_type category, const char* expr, const char* file_name, const char* func_name, const char* message, Tx&&... args){ return true; }//ブレークポイント

public:
	//デバッグポーズ処理の参照／変更
	inline iDebugPause* debugPause(){ return nullptr; }//取得
	inline iDebugPause* changeDebugPause(iDebugPause* new_debug_pause){ return nullptr; }//変更
	inline iDebugPause* changeDebugPause(iDebugPause& new_debug_pause){ return nullptr; }//変更
	inline iDebugPause* resetDebugPause(){ return nullptr; }//標準処理に変更
public:
	inline breakPoint(){}//デフォルトコンストラクタ
	inline ~breakPoint(){}//デストラクタ

#endif//GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//アサーション／ブレークポイント／ウォッチポイント用マクロ

#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション無効時は無効化

#define GASHA_ASSERT(level, category, expr, ...) GASHA_ breakPoint()(!(expr), level, category, "ASSERTION FAILURE!: " #expr "\n", GASHA_FILE_AND_FUNC, __VA_ARGS__)
#define GASHA_BREAKPOINT(level, category, ...) GASHA_ breakPoint()(true, level, category, "", GASHA_FILE_AND_FUNC, __VA_ARGS__)
#define GASHA_WATCHPOINT(level, category, expr, ...) GASHA_ breakPoint()((expr), level, category, "HIT!: " #expr "\n", GASHA_FILE_AND_FUNC, __VA_ARGS__)

#else//GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

#define GASHA_ASSERT(level, category, expr, ...)
#define GASHA_BREAKPOINT(level, category, ...)
#define GASHA_WATCHPOINT(level, category, expr, ...)

#endif//GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/assert.inl>

#endif//GASHA_INCLUDED_ASSERT_H

// End of file

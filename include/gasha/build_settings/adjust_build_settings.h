#pragma once
#ifndef GASHA_INCLUDED_ADJUST_BUILD_SETTINGS_H
#define GASHA_INCLUDED_ADJUST_BUILD_SETTINGS_H

//--------------------------------------------------------------------------------
// build_settings/adjust_build_settings.h
// ビルド設定調整
//
// 依存するヘッダー：project_first_settings.h//プロジェクト固有のビルド設定（先行設定）
//                   compiler_auto_settings.h//コンパイラ自動判別・設定
//                   platform_auto_settings.h//プラットフォーム自動判別・設定
//                   language_auto_settings.h//言語機能自動判別・設定
//                   build_configuration.h//ビルド構成
//                   project_last_settings.h//プロジェクト固有のビルド設定（最終設定）
//                   ※build_settings.h により、依存関係順にインクルード
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//【ビルド構成】

//アサーションが有効なら、必ずデバッグログも有効になる
#if !defined(GASHA_LOG_IS_ENABLED) && defined(GASHA_ASSERTION_IS_ENABLED)
	#define GASHA_LOG_IS_ENABLED
#endif//GASHA_LOG_IS_ENABLED

//コールポイントが有効なら、必ずデバッグログも有効になる
#if !defined(GASHA_LOG_IS_ENABLED) && defined(GASHA_CALLPOINT_IS_ENABLED)
	#define GASHA_LOG_IS_ENABLED
#endif//GASHA_LOG_IS_ENABLED

//--------------------------------------------------------------------------------
//【デバッガ用ブレークポイント割り込み】

//デバッガ用ブレークポイントは、ビルド構成でアサーションが有効でなければ、もしくは、開発ツールが有効でなければ無効化する
#if defined(GASHA_DEBUGGER_BREAK_IS_AVAILABLE) && (!defined(GASHA_ASSERTION_IS_ENABLED) || !defined(GASHA_DEV_TOOLS_IS_AVAILABLE))
	#undef GASHA_DEBUGGER_BREAK_IS_AVAILABLE
#endif//GASHA_DEBUGGER_BREAK_IS_AVAILABLE

//--------------------------------------------------------------------------------
//【ログ出力操作】

//デバッグログが無効なら、ログキューも無効
#if defined(GASHA_LOG_PRINT_USE_QUEUE) && !defined(GASHA_LOG_IS_ENABLED)
	#undef GASHA_LOG_PRINT_USE_QUEUE
#endif//GASHA_LOG_PRINT_USE_QUEUE

//--------------------------------------------------------------------------------
//【シンプルアサーション／ブレークポイント／ウォッチポイント】

//シンプルアサーション／ブレークポイント／ウォッチポイントの「メッセージの出力先」が未定義なら、stderrにする
#if !defined(GASHA_SIMPLE_ASSERT_STDOUT)
	#define GASHA_SIMPLE_ASSERT_STDOUT stderr
#endif//GASHA_SIMPLE_ASSERT_STDOUT

//--------------------------------------------------------------------------------
//【全体設定：TLS】

//MAC OS Xなど、TLSの初期化ができない環境では、TLSの初期化が不完全であることを明示する
//【注】現時点で、GASHAは MAX OS X に対応していないが、対応に備えた処理として記述しておく
#if !defined(GASHA_INCOMPLETE_TLS_INITIALIZER) && defined(GASHA_IS_MAXOSX)
	#define GASHA_INCOMPLETE_TLS_INITIALIZER
#endif//GASHA_INCOMPLETE_TLS_INITIALIZER

//--------------------------------------------------------------------------------
//【全体設定：標準入出力】

//標準入力が使用できない環境では、標準入力を無効化する
//#if defined(GASHA_STDIN_IS_AVAILABLE) && defined(***IS_GAME_PLATFORM***)
//	#undef GASHA_STDIN_IS_AVAILABLE
//#endif//GASHA_STDIN_IS_AVAILABLE

//--------------------------------------------------------------------------------
//【全体設定：SSE命令】

#ifdef GASHA_IS_X86//x86,x64系CPUの場合のみ設定可

	//指定のSSE命令の下位のSSE命令を自動的に有効化する
	
	#if !defined(GASHA_USE_SSE) && (defined(GASHA_USE_SSE2) || defined(GASHA_USE_SSE3) || defined(GASHA_USE_SSE4_1) || defined(GASHA_USE_SSE4_2) || defined(GASHA_USE_AVX) || defined(GASHA_USE_AVX2))
		#define GASHA_USE_SSE
	#endif//GASHA_USE_SSE

	#if !defined(GASHA_USE_SSE2) && (defined(GASHA_USE_SSE3) || defined(GASHA_USE_SSE4_1) || defined(GASHA_USE_SSE4_2) || defined(GASHA_USE_AVX) || defined(GASHA_USE_AVX2))
		#define GASHA_USE_SSE2
	#endif//GASHA_USE_SSE2

	#if !defined(GASHA_USE_SSE3) && (defined(GASHA_USE_SSE4_1) || defined(GASHA_USE_SSE4_2) || defined(GASHA_USE_AVX) || defined(GASHA_USE_AVX2))
		#define GASHA_USE_SSE3
	#endif//GASHA_USE_SSE3

	#if !defined(GASHA_USE_SSE4_1) && (defined(GASHA_USE_SSE4_2) || defined(GASHA_USE_AVX) || defined(GASHA_USE_AVX2))
		#define GASHA_USE_SSE4_1
	#endif//GASHA_USE_SSE4_1

	#if !defined(GASHA_USE_SSE4_2) && (defined(GASHA_USE_AVX) || defined(GASHA_USE_AVX2))
		#define GASHA_USE_SSE4_2
	#endif//GASHA_USE_SSE4_2

	#if !defined(GASHA_USE_AVX) && defined(GASHA_USE_AVX2)
		#define GASHA_USE_AVX
	#endif//GASHA_USE_AVX

#else//GASHA_IS_X86

	//x86系以外のCPUでは、SSE命令を無効化する

	#ifdef GASHA_USE_SSE
		#undef GASHA_USE_SSE
	#endif//GASHA_USE_SSE

	#ifdef GASHA_USE_SSE2
		#undef GASHA_USE_SSE2
	#endif//GASHA_USE_SSE2

	#ifdef GASHA_USE_SSE3
		#undef GASHA_USE_SSE3
	#endif//GASHA_USE_SSE3

	#ifdef GASHA_USE_SSE4A
		#undef GASHA_USE_SSE4A
	#endif//GASHA_USE_SSE4A

	#ifdef GASHA_USE_SSE4_1
		#undef GASHA_USE_SSE4_1
	#endif//GASHA_USE_SSE4_1

	#ifdef GASHA_USE_SSE4_2
		#undef GASHA_USE_SSE4_2
	#endif//GASHA_USE_SSE4_2

	#ifdef GASHA_USE_POPCNT
		#undef GASHA_USE_POPCNT
	#endif//GASHA_USE_POPCNT

	#ifdef GASHA_USE_AES
		#undef GASHA_USE_AES
	#endif//GASHA_USE_AES

	#ifdef GASHA_USE_AVX
		#undef GASHA_USE_AVX
	#endif//GASHA_USE_AVX

	#ifdef GASHA_USE_AVX2
		#undef GASHA_USE_AVX2
	#endif//GASHA_USE_AVX2

	#ifdef GASHA_USE_FMA4
		#undef GASHA_USE_FMA4
	#endif//GASHA_USE_FMA4

	#ifdef GASHA_USE_FMA3
		#undef GASHA_USE_FMA3
	#endif//GASHA_USE_FMA3

#endif//GASHA_IS_X86

//--------------------------------------------------------------------------------
//【算術：高速算術】

//SSE命令による高速化は、SSE命令が使えなければ無効化する
#if defined(GASHA_FAST_ARITH_USE_SSE) && !defined(GASHA_USE_SSE)
	#undef GASHA_FAST_ARITH_USE_SSE
#endif//GASHA_FAST_ARITH_USE_SSE

//SSE2命令による高速化は、SSE2命令が使えなければ無効化する
#if defined(GASHA_FAST_ARITH_USE_SSE2) && !defined(GASHA_USE_SSE2)
	#undef GASHA_FAST_ARITH_USE_SSE2
#endif//GASHA_FAST_ARITH_USE_SSE2

//SSE4.1命令による高速化は、SSE4.1命令が使えなければ無効化する
#if defined(GASHA_FAST_ARITH_USE_SSE4_1) && !defined(GASHA_USE_SSE4_1)
	#undef GASHA_FAST_ARITH_USE_SSE4_1
#endif//GASHA_FAST_ARITH_USE_SSE4_1

//AVX命令による高速化は、AVX命令が使えなければ無効化する
#if defined(GASHA_FAST_ARITH_USE_AVX) && !defined(GASHA_USE_AVX)
	#undef GASHA_FAST_ARITH_USE_AVX
#endif//GASHA_FAST_ARITH_USE_AVX

//逆数を用いた除算は、SSE命令による演算高速化が有効でなければ無効化する
#if defined(GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION) && !defined(GASHA_FAST_ARITH_USE_SSE)
//#if defined(GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION) && (!defined(GASHA_FAST_ARITH_USE_SSE) && !defined(GASHA_FAST_ARITH_USE_***))//その他のSIMD演算に対応するなら条件を追加する
	#undef GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION
#endif//GASHA_FAST_ARITH_USE_RECIPROCAL_FOR_DIVISION

//SSE4.1命令によるベクトル演算の高速化は、SSE4.1命令が使えなければ無効化する
#if defined(GASHA_VECTOR_OPERATION_ALWAYS_USE_SSE4_1) && !defined(GASHA_USE_SSE4_1)
	#undef GASHA_VECTOR_OPERATION_ALWAYS_USE_SSE4_1
#endif//GASHA_VECTOR_OPERATION_ALWAYS_USE_SSE4_1

//SSE4.1命令による行列演算の高速化は、SSE4.1命令が使えなければ無効化する
#if defined(GASHA_MATRIX_OPERATION_ALWAYS_USE_SSE4_1) && !defined(GASHA_USE_SSE4_1)
	#undef GASHA_MATRIX_OPERATION_ALWAYS_USE_SSE4_1
#endif//GASHA_MATRIX_OPERATION_ALWAYS_USE_SSE4_1

//--------------------------------------------------------------------------------
//【算術：CRC32】

//SSE4.2命令によるCRC計算の高速化は、SSE4.2命令が使えなければ無効化する
#if defined(GASHA_CRC32_USE_SSE) && (!defined(GASHA_CRC32_IS_CRC32C) || !defined(GASHA_USE_SSE4_2))
	#undef GASHA_CRC32_USE_SSE
#endif//GASHA_CRC32_USE_SSE

//--------------------------------------------------------------------------------
//【文字列：高速文字列】

//SSE4.2命令による文字列処理の高速化は、SSE4.2命令が使えなければ無効化する
#if defined(GASHA_FASE_STRING_USE_SSE4_2) && !defined(GASHA_USE_SSE4_2)
	#undef GASHA_FASE_STRING_USE_SSE4_2
#endif//GASHA_FASE_STRING_USE_SSE4_2

//SSE4.2命令によるstrlenの高速化は、SSE4.2命令による文字列処理高速化が有効でなければ無効化する
#if defined(GASHA_STRLEN_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRLEN_FAST_USE_SSE4_2
#endif//GASHA_STRLEN_FAST_USE_SSE4_2

//SSE4.2命令によるstrnlenの高速化は、SSE4.2命令による文字列処理高速化が有効でなければ無効化する
#if defined(GASHA_STRNLEN_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRNLEN_FAST_USE_SSE4_2
#endif//GASHA_STRNLEN_FAST_USE_SSE4_2

//SSE4.2命令によるstrcmpの高速化は、SSE4.2命令による文字列処理高速化が有効でなければ無効化する
#if defined(GASHA_STRCMP_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRCMP_FAST_USE_SSE4_2
#endif//GASHA_STRCMP_FAST_USE_SSE4_2

//SSE4.2命令によるstrncmpの高速化は、SSE4.2命令による文字列処理高速化が有効でなければ無効化する
#if defined(GASHA_STRNCMP_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRNCMP_FAST_USE_SSE4_2
#endif//GASHA_STRNCMP_FAST_USE_SSE4_2

//SSE4.2命令によるstrchrの高速化は、SSE4.2命令による文字列処理高速化が有効でなければ無効化する
#if defined(GASHA_STRCHR_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRCHR_FAST_USE_SSE4_2
#endif//GASHA_STRCHR_FAST_USE_SSE4_2

//SSE4.2命令によるstrrchrの高速化は、SSE4.2命令による文字列処理高速化が有効でなければ無効化する
#if defined(GASHA_STRRCHR_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRRCHR_FAST_USE_SSE4_2
#endif//GASHA_STRRCHR_FAST_USE_SSE4_2

//SSE4.2命令によるstrstrの高速化は、SSE4.2命令による文字列処理高速化が有効でなければ無効化する
#if defined(GASHA_STRSTR_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRSTR_FAST_USE_SSE4_2
#endif//GASHA_STRSTR_FAST_USE_SSE4_2

//SSE4.2命令によるstrstrbmの高速化は、SSE4.2命令による文字列処理高速化が有効でなければ無効化する
#if defined(GASHA_STRSTRBM_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRSTRBM_FAST_USE_SSE4_2
#endif//GASHA_STRSTRBM_FAST_USE_SSE4_2

//SSE4.2命令によるstrstr0の高速化は、SSE4.2命令による文字列処理高速化が有効でなければ無効化する
#if defined(GASHA_STRSTR0_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRSTR0_FAST_USE_SSE4_2
#endif//GASHA_STRSTR0_FAST_USE_SSE4_2

//SSE4.2命令によるstrcpyの高速化は、SSE4.2命令による文字列処理高速化が有効でなければ無効化する
#if defined(GASHA_STRCPY_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRCPY_FAST_USE_SSE4_2
#endif//GASHA_STRCPY_FAST_USE_SSE4_2

//SSE4.2命令によるstrncpyの高速化は、SSE4.2命令による文字列処理高速化が有効でなければ無効化する
#if defined(GASHA_STRNCPY_FAST_USE_SSE4_2) && !defined(GASHA_FASE_STRING_USE_SSE4_2)
	#undef GASHA_STRNCPY_FAST_USE_SSE4_2
#endif//GASHA_STRNCPY_FAST_USE_SSE4_2

//--------------------------------------------------------------------------------
//【時間処理ユーティリティ】

//経過時間計算用の「秒数型」が未定義なら、double にする
#if !defined(GASHA_TIME_TYPE)
	#define GASHA_TIME_TYPE double
#endif//GASHA_TIME_TYPE

//--------------------------------------------------------------------------------
//【スタックアロケータ】

//スタックアロケータのメモリ確保／破棄時のアサーションは、ビルド構成でアサーションが有効でなければ無効化する
#if defined(GASHA_STACK_ALLOCATOR_ENABLE_ASSERTION) && !defined(GASHA_ASSERTION_IS_ENABLED)
	#undef GASHA_STACK_ALLOCATOR_ENABLE_ASSERTION
#endif//GASHA_STACK_ALLOCATOR_ENABLE_ASSERTION

//--------------------------------------------------------------------------------
//【ロックフリースタックアロケータ】

//ロックフリースタックアロケータのメモリ確保／破棄時のアサーションは、ビルド構成でアサーションが有効でなければ無効化する
#if defined(GASHA_LF_STACK_ALLOCATOR_ENABLE_ASSERTION) && !defined(GASHA_ASSERTION_IS_ENABLED)
	#undef GASHA_LF_STACK_ALLOCATOR_ENABLE_ASSERTION
#endif//GASHA_LF_STACK_ALLOCATOR_ENABLE_ASSERTION

//--------------------------------------------------------------------------------
//【双方向スタックアロケータ】

//双方向スタックアロケータのメモリ確保／破棄時のアサーションは、ビルド構成でアサーションが有効でなければ無効化する
#if defined(GASHA_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION) && !defined(GASHA_ASSERTION_IS_ENABLED)
	#undef GASHA_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
#endif//GASHA_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION

//--------------------------------------------------------------------------------
//【ロックフリー双方向スタックアロケータ】

//ロックフリー双方向スタックアロケータのメモリ確保／破棄時のアサーションは、ビルド構成でアサーションが有効でなければ無効化する
#if defined(GASHA_LF_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION) && !defined(GASHA_ASSERTION_IS_ENABLED)
	#undef GASHA_LF_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION
#endif//GASHA_LF_DUAL_STACK_ALLOCATOR_ENABLE_ASSERTION

//--------------------------------------------------------------------------------
//【単一アロケータ】

//単一アロケータのメモリ確保／破棄時のアサーションは、ビルド構成でアサーションが有効でなければ無効化する
#if defined(GASHA_MONO_ALLOCATOR_ENABLE_ASSERTION) && !defined(GASHA_ASSERTION_IS_ENABLED)
	#undef GASHA_MONO_ALLOCATOR_ENABLE_ASSERTION
#endif//GASHA_MONO_ALLOCATOR_ENABLE_ASSERTION

//--------------------------------------------------------------------------------
//【ロックフリー単一アロケータ】

//ロックフリー単一アロケータのメモリ確保／破棄時のアサーションは、ビルド構成でアサーションが有効でなければ無効化する
#if defined(GASHA_LF_MONO_ALLOCATOR_ENABLE_ASSERTION) && !defined(GASHA_ASSERTION_IS_ENABLED)
	#undef GASHA_LF_MONO_ALLOCATOR_ENABLE_ASSERTION
#endif//GASHA_LF_MONO_ALLOCATOR_ENABLE_ASSERTION

//--------------------------------------------------------------------------------
//【プールアロケータ】

//プールアロケータのメモリ確保／破棄時のアサーションは、ビルド構成でアサーションが有効でなければ無効化する
#if defined(GASHA_POOL_ALLOCATOR_ENABLE_ASSERTION) && !defined(GASHA_ASSERTION_IS_ENABLED)
	#undef GASHA_POOL_ALLOCATOR_ENABLE_ASSERTION
#endif//GASHA_POOL_ALLOCATOR_ENABLE_ASSERTION

//--------------------------------------------------------------------------------
//【ロックフリープールアロケータ】

//ロックフリープールアロケータのメモリ確保／破棄時のアサーションは、ビルド構成でアサーションが有効でなければ無効化する
#if defined(GASHA_LF_POOL_ALLOCATOR_ENABLE_ASSERTION) && !defined(GASHA_ASSERTION_IS_ENABLED)
	#undef GASHA_LF_POOL_ALLOCATOR_ENABLE_ASSERTION
#endif//GASHA_LF_POOL_ALLOCATOR_ENABLE_ASSERTION

//--------------------------------------------------------------------------------
//【標準アロケータ】

//標準アロケータのメモリ確保／破棄時のアサーションは、ビルド構成でアサーションが有効でなければ無効化する
#if defined(GASHA_STD_ALLOCATOR_ENABLE_ASSERTION) && !defined(GASHA_ASSERTION_IS_ENABLED)
	#undef GASHA_STD_ALLOCATOR_ENABLE_ASSERTION
#endif//GASHA_STD_ALLOCATOR_ENABLE_ASSERTION

//--------------------------------------------------------------------------------
//【コンソール】

//Windowsコマンドプロンプトは、対象プラットフォームがWindowsでなければ無効化する
//※Windowsコマンドプロンプトクラスは、TTY端末クラスに自動的に変更される
#if defined(GASHA_USE_WINDOWS_CONSOLE) && !defined(GASHA_IS_WIN)
	#undef GASHA_USE_WINDOWS_CONSOLE
#endif//GASHA_USE_WINDOWS_CONSOLE

//Visual Studio出力ウインドウは、対象プラットフォームがWindowsかつデバッグツールが使用可能でなければ無効化する
//※Visual Studio出力ウインドウは、TTY端末クラスに自動的に変更される
#if defined(GASHA_USE_VS_CONSOLE) && (!defined(GASHA_IS_WIN) || !defined(GASHA_DEV_TOOLS_IS_AVAILABLE))
	#undef GASHA_USE_VS_CONSOLE
#endif//GASHA_USE_VS_CONSOLE

//--------------------------------------------------------------------------------
//【コンソールカラー】

//（TTY端末クラスの）エスケープシーケンス処理は、対象プラットフォームがWindowsなら無効化する
//#if defined(GASHA_USE_ESCAPE_SEQUENCE) && defined(GASHA_IS_WIN)
//	#undef GASHA_USE_ESCAPE_SEQUENCE
//#endif//GASHA_USE_ESCAPE_SEQUENCE

//Windowsコマンドプロンプトのコンソールカラー設定処理は、対象プラットフォームがWindowsでなければ無効化する
#if defined(GASHA_USE_WINDOWS_CONSOLE_COLOR) && !defined(GASHA_IS_WIN)
	#undef GASHA_USE_WINDOWS_CONSOLE_COLOR
#endif//GASHA_USE_WINDOWS_CONSOLE_COLOR

//--------------------------------------------------------------------------------
//【メモリコンソール】

//「標準メモリコンソールのバッファサイズ」が未定義なら、デフォルト値にする
#if !defined(GASHA_STD_MEM_CONSOLE_BUFF_SIZE)
	#define GASHA_STD_MEM_CONSOLE_BUFF_SIZE 4096
#endif//GASHA_STD_MEM_CONSOLE_BUFF_SIZE

//--------------------------------------------------------------------------------
//【ログカテゴリ】

//「ログカテゴリの数」が未定義なら、デフォルト値にする
#if !defined(GASHA_LOG_CATEGORY_NUM)
	#define GASHA_LOG_CATEGORY_NUM 64
#endif//GASHA_LOG_CATEGORY_NUM

//「ログカテゴリの数」が10未満なら、10に再定義する
#if GASHA_LOG_CATEGORY_NUM < 10
	#unde GASHA_LOG_CATEGORY_NUM
	#define GASHA_LOG_CATEGORY_NUM 10
#endif//GASHA_LOG_CATEGORY_NUM

//--------------------------------------------------------------------------------
//【ログレベルマスク】

//「デフォルトのログレベルマスク」が未定義なら、3（asNormal：通常メッセージ）にする
#if !defined(GASHA_DEFAULT_LOG_MASK_OF_LOG)
	#define GASHA_DEFAULT_LOG_MASK_OF_LOG 3
#endif//GASHA_DEFAULT_LOG_MASK_OF_LOG

//「デフォルトの画面通知ログレベルマスク」が未定義なら、9（asCritical：重大メッセージ）にする
#if !defined(GASHA_DEFAULT_LOG_MASK_OF_NOTICE)
	#define GASHA_DEFAULT_LOG_MASK_OF_NOTICE 9
#endif//GASHA_DEFAULT_LOG_MASK_OF_NOTICE

//--------------------------------------------------------------------------------
//【ログ属性】

//「デフォルトのログ属性」が未定義なら、0x0000（属性なし）にする
#if !defined(GASHA_DEFAULT_LOG_ATTR)
	#define GASHA_DEFAULT_LOG_ATTR 0x0000
#endif//GASHA_DEFAULT_LOG_ATTR

//--------------------------------------------------------------------------------
//【ログワークバッファ】

//「ログ出力用ワークバッファ一つ当たりのサイズ」が未定義なら、デフォルト値にする
#if !defined(GASHA_LOG_WORK_BUFF_BLOCK_SIZE)
	#define GASHA_LOG_WORK_BUFF_BLOCK_SIZE 4096
#endif//GASHA_LOG_WORK_BUFF_BLOCK_SIZE

//「ログ出力用ワークバッファのプール数」が未定義なら、デフォルト値にする
#if !defined(GASHA_LOG_WORK_BUFF_POOL_SIZE)
	#define GASHA_LOG_WORK_BUFF_POOL_SIZE 4
#endif//GASHA_LOG_WORK_BUFF_POOL_SIZE

//--------------------------------------------------------------------------------
//【ログキュー】

//「ログキューメッセージ用スタックバッファサイズ」が未定義なら、デフォルト値にする
#if !defined(GASHA_LOG_QUEUE_MESSAGE_STACK_SIZE)
	#define GASHA_LOG_QUEUE_MESSAGE_STACK_SIZE 32768
#endif//GASHA_LOG_QUEUE_MESSAGE_STACK_SIZE

//「ログキュー用のノード数」が未定義なら、デフォルト値にする
#if !defined(GASHA_LOG_QUEUE_NODE_SIZE)
	#define GASHA_LOG_QUEUE_NODE_SIZE 256
#endif//GASHA_LOG_QUEUE_NODE_SIZE

//--------------------------------------------------------------------------------
//【ログキューモニター】

//「次のIDのキューが来ない時にリトライ（待機）する最大回数」が未定義なら、デフォルト値にする
#if !defined(GASHA_LOG_QUEUE_MONITOR_MAX_RETRY_COUNT)
	#define GASHA_LOG_QUEUE_MONITOR_MAX_RETRY_COUNT 256
#endif//GASHA_LOG_QUEUE_MONITOR_MAX_RETRY_COUNT

//--------------------------------------------------------------------------------
//【プロファイラー】

//「文字列プールバッファサイズ（バイト数）」が未定義なら、デフォルト値にする
#if !defined(GASHA_PROFILER_STR_POOL_BUFF_SIZE)
	#define GASHA_PROFILER_STR_POOL_BUFF_SIZE 8192
#endif//GASHA_PROFILER_STR_POOL_BUFF_SIZE

//「文字列プールテーブルサイズ（個数）」が未定義なら、デフォルト値にする
#if !defined(GASHA_PROFILER_STR_POOL_TABLE_SIZE)
	#define GASHA_PROFILER_STR_POOL_TABLE_SIZE 1024
#endif//GASHA_PROFILER_STR_POOL_TABLE_SIZE

//「プロファイル情報プールサイズ（個数）」が未定義なら、デフォルト値にする
#if !defined(GASHA_PROFILER_PROFILE_INFO_POOL_SIZE)
	#define GASHA_PROFILER_PROFILE_INFO_POOL_SIZE 2048
#endif//GASHA_PROFILER_PROFILE_INFO_POOL_SIZE

//「スレッド情報テーブルサイズ（個数）」が未定義なら、デフォルト値にする
#if !defined(GASHA_PROFILER_THREAD_INFO_TABLE_SIZE)
	#define GASHA_PROFILER_THREAD_INFO_TABLE_SIZE 64
#endif//GASHA_PROFILER_THREAD_INFO_TABLE_SIZE

//--------------------------------------------------------------------------------
//【シングルトンデバッグ用処理】

//シングルトンのデバッグ情報収集機能は、ビルド構成でデバッグ機能が有効でなければ無効化する
#if defined(GASHA_SINGLETON_DEBUG_ENABLED) && !defined(GASHA_DEBUG_FEATURE_IS_ENABLED)
	#undef GASHA_SINGLETON_DEBUG_ENABLED
#endif//GASHA_SINGLETON_DEBUG_ENABLED

#endif//GASHA_INCLUDED_ADJUST_BUILD_SETTINGS_H

// End of file

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
#if !defined(GASHA_HAS_DEBUG_LOG) && defined(GASHA_ASSERTION_IS_ENABLED)
	#define GASHA_HAS_DEBUG_LOG
#endif//GASHA_HAS_DEBUG_LOG

//--------------------------------------------------------------------------------
//【全体設定：SSE命令設定】

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
//【算術設定：高速算術】

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
//【算術設定：CRC32】

//SSE4.2命令によるCRC計算の高速化は、SSE4.2命令が使えなければ無効化する
#if defined(GASHA_CRC32_USE_SSE) && (!defined(GASHA_CRC32_IS_CRC32C) || !defined(GASHA_USE_SSE4_2))
	#undef GASHA_CRC32_USE_SSE
#endif//GASHA_CRC32_USE_SSE

//--------------------------------------------------------------------------------
//【文字列設定：高速文字列】

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
//【ログカテゴリ】

//ログカテゴリの数が未定義なら、64にする
#if !defined(GASHA_LOG_CATEGORY_NUM)
	#define GASHA_LOG_CATEGORY_NUM 64
#endif//GASHA_LOG_CATEGORY_NUM

//ログカテゴリの数が8以下なら、8に再定義する
#if GASHA_LOG_CATEGORY_NUM < 8
	#unde GASHA_LOG_CATEGORY_NUM
	#define GASHA_LOG_CATEGORY_NUM 8
#endif//GASHA_LOG_CATEGORY_NUM

//--------------------------------------------------------------------------------
//【ログマスク】

//デフォルトのログマスクが未定義なら、1（asNormal：通常メッセージ）にする
#if !defined(GASHA_DEFAULT_LOG_MASK_OF_LOG)
	#define GASHA_DEFAULT_LOG_MASK_OF_LOG 1
#endif//GASHA_DEFAULT_LOG_MASK_OF_LOG

//デフォルトの画面通知ログマスクが未定義なら、4（asCritical：重大メッセージ）にする
#if !defined(GASHA_DEFAULT_LOG_MASK_OF_NOTICE)
	#define GASHA_DEFAULT_LOG_MASK_OF_NOTICE 4
#endif//GASHA_DEFAULT_LOG_MASK_OF_NOTICE

//--------------------------------------------------------------------------------
//【ログワークバッファ】

//ログ出力用ワークバッファ一つ当たりのサイズ
#if !defined(GASHA_LOG_WORK_BUFF_BLOCK_SIZE)
	#define GASHA_LOG_WORK_BUFF_BLOCK_SIZE 2048
#endif//GASHA_LOG_WORK_BUFF_BLOCK_SIZE

//ログ出力用ワークバッファのプール数
#if !defined(GASHA_LOG_WORK_BUFF_POOL_SIZE)
	#define GASHA_LOG_WORK_BUFF_POOL_SIZE 4
#endif//GASHA_LOG_WORK_BUFF_POOL_SIZE

//--------------------------------------------------------------------------------
//【ログキュー】
//※ライブラリの再ビルド必要

//ログキューメッセージ用スタックバッファサイズ
#if !defined(GASHA_LOG_QUEUE_MESSAGE_STACK_SIZE)
	#define GASHA_LOG_QUEUE_MESSAGE_STACK_SIZE 32768
#endif//GASHA_LOG_QUEUE_MESSAGE_STACK_SIZE

//ログキュー用のノード数
#if !defined(GASHA_LOG_QUEUE_NODE_SIZE)
	#define GASHA_LOG_QUEUE_NODE_SIZE 256
#endif//GASHA_LOG_QUEUE_NODE_SIZE

//--------------------------------------------------------------------------------
//【シングルトンデバッグ用処理】

//シングルトンのデバッグ情報収集機能は、ビルド構成でデバッグ機能が有効でなければ無効化する
#if defined(GASHA_SINGLETON_DEBUG_ENABLED) && !defined(GASHA_HAS_DEBUG_FEATURE)
	#undef GASHA_SINGLETON_DEBUG_ENABLED
#endif//GASHA_SINGLETON_DEBUG_ENABLED

#endif//GASHA_INCLUDED_ADJUST_BUILD_SETTINGS_H

// End of file

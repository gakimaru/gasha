#pragma once
#ifndef GASHA_INCLUDED_BUILD_CONFIGURATION_H
#define GASHA_INCLUDED_BUILD_CONFIGURATION_H

//--------------------------------------------------------------------------------
// build_configuration.h
// ビルド構成
//
// 依存するヘッダー：project/first_settings.h//プロジェクト固有のビルド設定（先行設定）
//                   compiler_auto_settings.h//コンパイラ自動判別・設定
//                   platform_auto_settings.h//プラットフォーム自動判別・設定
//                   language_auto_settings.h//言語機能自動判別・設定
//                   ※build_settings.h により、依存関係順にインクルード
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//ビルド構成

//----------------------------------------
//ビルド構成識別マクロ一覧
//#define GASHA_BUILD_CONFIG_IS_DEBUG//フルデバッグ設定
//#define GASHA_BUILD_CONFIG_IS_DEBUG_MODERATE//プログラム開発向け設定
//#define GASHA_BUILD_CONFIG_IS_DEBUG_OPT//コンテンツ制作・QA向け設定
//#define GASHA_BUILD_CONFIG_IS_REGRESSION_TEST//自動回帰テスト向け設定
//#define GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE//製品テスト向け設定
//#define GASHA_BUILD_CONFIG_ISRELEASE//製品向け設定

//----------------------------------------
//ビルド構成個別マクロ一覧
//#define GASHA_HAS_DEBUG_FEATURE//デバッグ機能有効化
//#define GASHA_HAS_VERBOSE_DEBUG//冗長デバッグ機能有効化
//#define GASHA_ASSERTION_IS_ENABLED//アサーション有効化
//#define GASHA_NO_OPTIMIZED//最適化なし
//#define GASHA_OPTIMIZED_MODERATELY//適度に最適化
//#define GASHA_OPTIMIZED//最大限の最適化
//#define GASHA_FILE_SYSTEM_IS_ROM//ROM専用ファイルシステム
//#define GASHA_FILE_SYSTEM_IS_FLEXIBLE//ローカル／ROM切り替えファイルシステム（ローカルデータで設定）
//#define GASHA_UNITE_TEST_ENABLED//ユニットテスト（の仕組みが）有効
//#define GASHA_IS_REGRESSION_TEST//回帰テストモード有効
//#define GASHA_HAS_SYNBOMS//シンボル情報あり
//#define GASHA_IS_STRIPPED_SYMBOLS//シンボル情報なし

//----------------------------------------
#ifdef GASHA_IS_VC
//Visual C++ の場合

	#ifdef _DEBUG
		#define GASHA_BUILD_CONFIG_FULL_DEBUG//DEBUG：フルデバッグ設定
	#else//_DEBUG
		#define GASHA_BUILD_CONFIG_IS_RELEASE//RELEASE：製品向け設定
	#endif//_DEBUG

#endif//GASHA_IS_VC

//----------------------------------------
#ifdef GASHA_IS_GCC
//GCC の場合

	#ifdef _DEBUG
		#define GASHA_BUILD_CONFIG_FULL_DEBUG//DEBUG：フルデバッグ設定
	#else//_DEBUG
		#define GASHA_BUILD_CONFIG_IS_RELEASE//RELEASE：製品向け設定
	#endif//_DEBUG

#endif//GASHA_IS_GCC

//----------------------------------------
#ifdef GASHA_BUILD_CONFIG_FULL_DEBUG//フルデバッグ設定
	#define GASHA_BUILD_CONFIG_NAME "FULL_DEBUG"
	#define GASHA_HAS_DEBUG_FEATURE//デバッグ機能有効化
	#define GASHA_HAS_VERBOSE_DEBUG//冗長デバッグ機能有効化
	#define GASHA_ASSERTION_IS_ENABLED//アサーション有効化
	#define GASHA_NO_OPTIMIZED//最適化なし
	#define GASHA_FILE_SYSTEM_IS_FLEXIBLE//ローカル／ROM切り替えファイルシステム（ローカルデータで設定）
	#define GASHA_UNITE_TEST_ENABLED//ユニットテスト（の仕組みが）有効
	#define GASHA_HAS_SYNBOMS//シンボル情報あり
#endif//GASHA_BUILD_CONFIG_IS_DEBUG

//----------------------------------------
#ifdef GASHA_BUILD_CONFIG_IS_DEBUG_MODERATE//プログラム開発向け設定
	#define GASHA_BUILD_CONFIG_NAME "DEBUG_MODERATE"
	#define GASHA_HAS_DEBUG_FEATURE//デバッグ機能有効化
	#define GASHA_HAS_VERBOSE_DEBUG//冗長デバッグ機能有効化
	#define GASHA_ASSERTION_IS_ENABLED//アサーション有効化
	#define GASHA_OPTIMIZED_MODERATELY//適度に最適化
	#define GASHA_FILE_SYSTEM_IS_FLEXIBLE//ローカル／ROM切り替えファイルシステム（ローカルデータで設定）
	#define GASHA_UNITE_TEST_ENABLED//ユニットテスト（の仕組みが）有効
	#define GASHA_HAS_SYNBOMS//シンボル情報あり
#endif//GASHA_BUILD_CONFIG_IS_DEBUG_MODERATE

//----------------------------------------
#ifdef GASHA_BUILD_CONFIG_IS_DEBUG_OPT//コンテンツ制作・QA向け設定
	#define GASHA_BUILD_CONFIG_NAME "DEBUG_OPT"
	#define GASHA_HAS_DEBUG_FEATURE//デバッグ機能有効化
	#define GASHA_ASSERTION_IS_ENABLED//アサーション有効化
	#define GASHA_OPTIMIZED//最大限の最適化
	#define GASHA_FILE_SYSTEM_IS_FLEXIBLE//ローカル／ROM切り替えファイルシステム（ローカルデータで設定）
	#define GASHA_UNITE_TEST_ENABLED//ユニットテスト（の仕組みが）有効
	#define GASHA_HAS_SYNBOMS//シンボル情報あり
#endif//GASHA_BUILD_CONFIG_IS_DEBUG_OPT

//----------------------------------------
#ifdef GASHA_BUILD_CONFIG_IS_REGRESSION_TEST//自動回帰テスト向け設定
	#define GASHA_BUILD_CONFIG_NAME "REGRESSON_TEST"
	#define GASHA_HAS_DEBUG_FEATURE//デバッグ機能有効化
	#define GASHA_ASSERTION_IS_ENABLED//アサーション有効化
	#define GASHA_OPTIMIZED//最大限の最適化
	#define GASHA_FILE_SYSTEM_IS_FLEXIBLE//ローカル／ROM切り替えファイルシステム（ローカルデータで設定）
	#define GASHA_UNITE_TEST_ENABLED//ユニットテスト（の仕組みが）有効
	#define GASHA_IS_REGRESSION_TEST//回帰テストモード有効
	#define GASHA_HAS_SYNBOMS//シンボル情報あり
#endif//GASHA_BUILD_CONFIG_IS_REGRESSION_TEST

//----------------------------------------
#ifdef GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE//製品テスト向け設定
	#define GASHA_BUILD_CONFIG_NAME "LOCAL_RELEASE"
	#define GASHA_OPTIMIZED//最大限の最適化
	#define GASHA_FILE_SYSTEM_IS_FLEXIBLE//ローカル／ROM切り替えファイルシステム（ローカルデータで設定）
	#define GASHA_HAS_SYNBOMS//シンボル情報あり
#endif//GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE

//----------------------------------------
#ifdef GASHA_BUILD_CONFIG_IS_RELEASE//製品向け設定
	#define GASHA_BUILD_CONFIG_NAME "RELEASE"
	#define GASHA_OPTIMIZED//最大限の最適化
	#define GASHA_FILE_SYSTEM_IS_ROM//ROM専用ファイルシステム
	#define GASHA_IS_STRIPPED_SYMBOLS//シンボル情報なし
#endif//GASHA_BUILD_CONFIG_IS_RELEASE

#endif//GASHA_INCLUDED_BUILD_CONFIGURATION_H

// End of file

#pragma once
#ifndef GASHA_INCLUDED_BUILD_CONFIGURATION_H
#define GASHA_INCLUDED_BUILD_CONFIGURATION_H

//--------------------------------------------------------------------------------
// build_settings/build_configuration.h
// ビルド構成
//
// 依存するヘッダー：project_first_settings.h//プロジェクト固有のビルド設定（先行設定）
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
//※ユーザー側で外部定義するマクロ（コンパイラオプションにて定義）
//※いずれの指定も内場合は、（可能な限り）自動的に該当する構成を選択する
//※複数指定された場合、デバッグレベルの高いものが優先される（下記の並び順に優先度が高い）
//#define GASHA_BUILD_CONFIG_IS_DEBUG//フルデバッグ設定
//#define GASHA_BUILD_CONFIG_IS_DEBUG_MODERATE//プログラム開発向け設定
//#define GASHA_BUILD_CONFIG_IS_DEBUG_OPT//コンテンツ制作・QA向け設定
//#define GASHA_BUILD_CONFIG_IS_REGRESSION_TEST//自動回帰テスト向け設定
//#define GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE//製品テスト向け設定
//#define GASHA_BUILD_CONFIG_IS_RELEASE//製品向け設定

//----------------------------------------
//ビルド構成識別マクロ補助
//※ユーザー側で外部定義するマクロ（コンパイラオプションにて定義）
//#define GASHA_BUILD_FOR_LIB//ライブラリファイル用のビルド

//----------------------------------------
//ビルド構成個別マクロ一覧
//#define GASHA_DEBUG_FEATURE_IS_ENABLED//デバッグ機能有効化
//#define GASHA_VERBOSE_DEBUG_IS_ENABLED//冗長デバッグ機能有効化
//#define GASHA_LOG_IS_ENABLED//デバッグログ有効化
//#define GASHA_ASSERTION_IS_ENABLED//アサーション／ブレークポイント／ウォッチポイント有効化 ※【注】アサーション有効化時は、デバッグログを自動的に有効化する
//#define GASHA_CALLPOINT_IS_ENABLED//コールポイント機能有効化 ※【注】コールポイント有効化時は、デバッグログを自動的に有効化する
//#define GASHA_PROFILE_IS_AVAILABLE//プロファイル機能利用可能
//#define GASHA_DEV_TOOLS_IS_AVAILABLE//開発ツール利用可能
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
//ビルド構成識別マクロ判定

#ifdef GASHA_BUILD_CONFIG_EXIST
	#undef GASHA_BUILD_CONFIG_EXIST//一旦このマクロは削除
#endif//GASHA_BUILD_CONFIG_EXIST

//フルデバッグ設定
#ifdef GASHA_BUILD_CONFIG_IS_DEBUG

	#define GASHA_BUILD_CONFIG_EXIST

	#ifdef GASHA_BUILD_CONFIG_IS_DEBUG_MODERATE
		#undef GASHA_BUILD_CONFIG_IS_DEBUG_MODERATE
	#endif//GASHA_BUILD_CONFIG_IS_DEBUG_MODERATE

	#ifdef GASHA_BUILD_CONFIG_IS_DEBUG_OPT
		#undef GASHA_BUILD_CONFIG_IS_DEBUG_OPT
	#endif//GASHA_BUILD_CONFIG_IS_DEBUG_OPT

	#ifdef GASHA_BUILD_CONFIG_IS_REGRESSION_TEST
		#undef GASHA_BUILD_CONFIG_IS_REGRESSION_TEST
	#endif//GASHA_BUILD_CONFIG_IS_REGRESSION_TEST

	#ifdef GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE
		#undef GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE
	#endif//GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE

	#ifdef GASHA_BUILD_CONFIG_IS_RELEASE
		#undef GASHA_BUILD_CONFIG_IS_RELEASE
	#endif//GASHA_BUILD_CONFIG_IS_RELEASE

#endif//GASHA_BUILD_CONFIG_IS_DEBUG

//プログラム開発向け設定
#ifdef GASHA_BUILD_CONFIG_IS_DEBUG_MODERATE

	#define GASHA_BUILD_CONFIG_EXIST

	#ifdef GASHA_BUILD_CONFIG_IS_DEBUG_OPT
		#undef GASHA_BUILD_CONFIG_IS_DEBUG_OPT
	#endif//GASHA_BUILD_CONFIG_IS_DEBUG_OPT

	#ifdef GASHA_BUILD_CONFIG_IS_REGRESSION_TEST
		#undef GASHA_BUILD_CONFIG_IS_REGRESSION_TEST
	#endif//GASHA_BUILD_CONFIG_IS_REGRESSION_TEST

	#ifdef GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE
		#undef GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE
	#endif//GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE

	#ifdef GASHA_BUILD_CONFIG_IS_RELEASE
		#undef GASHA_BUILD_CONFIG_IS_RELEASE
	#endif//GASHA_BUILD_CONFIG_IS_RELEASE

#endif//GASHA_BUILD_CONFIG_IS_DEBUG_MODERATE

//コンテンツ制作・QA向け設定
#ifdef GASHA_BUILD_CONFIG_IS_DEBUG_OPT

	#define GASHA_BUILD_CONFIG_EXIST

	#ifdef GASHA_BUILD_CONFIG_IS_REGRESSION_TEST
		#undef GASHA_BUILD_CONFIG_IS_REGRESSION_TEST
	#endif//GASHA_BUILD_CONFIG_IS_REGRESSION_TEST

	#ifdef GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE
		#undef GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE
	#endif//GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE

	#ifdef GASHA_BUILD_CONFIG_IS_RELEASE
		#undef GASHA_BUILD_CONFIG_IS_RELEASE
	#endif//GASHA_BUILD_CONFIG_IS_RELEASE

#endif//GASHA_BUILD_CONFIG_IS_DEBUG_OPT

//自動回帰テスト向け設定
#ifdef GASHA_BUILD_CONFIG_IS_REGRESSION_TEST

	#define GASHA_BUILD_CONFIG_EXIST

	#ifdef GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE
		#undef GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE
	#endif//GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE

	#ifdef GASHA_BUILD_CONFIG_IS_RELEASE
		#undef GASHA_BUILD_CONFIG_IS_RELEASE
	#endif//GASHA_BUILD_CONFIG_IS_RELEASE

#endif//GASHA_BUILD_CONFIG_IS_REGRESSION_TEST

//製品テスト向け設定
#ifdef GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE

	#define GASHA_BUILD_CONFIG_EXIST

	#ifdef GASHA_BUILD_CONFIG_IS_RELEASE
		#undef GASHA_BUILD_CONFIG_IS_RELEASE
	#endif//GASHA_BUILD_CONFIG_IS_RELEASE

#endif//GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE

//製品向け設定
#ifdef GASHA_BUILD_CONFIG_IS_RELEASE

	#define GASHA_BUILD_CONFIG_EXIST

	#ifdef GASHA_BUILD_CONFIG_IS_RELEASE
		#undef GASHA_BUILD_CONFIG_IS_RELEASE
	#endif//GASHA_BUILD_CONFIG_IS_RELEASE

#endif//GASHA_BUILD_CONFIG_IS_RELEASE

//----------------------------------------
//ビルド構成識別マクロがなかった時の自動判定

#ifndef GASHA_BUILD_CONFIG_EXIST

	//--------------------
	#ifdef GASHA_IS_VC
	//Visual C++ の場合

		#ifdef _DEBUG
			#define GASHA_BUILD_CONFIG_FULL_DEBUG//DEBUG：フルデバッグ設定
			#define GASHA_BUILD_CONFIG_EXIST
		#else//_DEBUG
			#define GASHA_BUILD_CONFIG_IS_RELEASE//RELEASE：製品向け設定
			#define GASHA_BUILD_CONFIG_EXIST
		#endif//_DEBUG

	#endif//GASHA_IS_VC

	//----------------------------------------
	#ifdef GASHA_IS_GCC
	//GCC の場合

		#ifdef _DEBUG
			#define GASHA_BUILD_CONFIG_FULL_DEBUG//DEBUG：フルデバッグ設定
			#define GASHA_BUILD_CONFIG_EXIST
		#else//_DEBUG
			#define GASHA_BUILD_CONFIG_IS_RELEASE//RELEASE：製品向け設定
			#define GASHA_BUILD_CONFIG_EXIST
		#endif//_DEBUG

	#endif//GASHA_IS_GCC

#endif//GASHA_BUILD_CONFIG_TYPE

//----------------------------------------
#ifdef GASHA_BUILD_CONFIG_FULL_DEBUG//フルデバッグ設定
	#define GASHA_BUILD_CONFIG_LEVEL 1
	#define GASHA_BUILD_CONFIG_NAME "FULL_DEBUG"
	#define GASHA_DEBUG_FEATURE_IS_ENABLED//デバッグ機能有効化
	#define GASHA_LOG_IS_ENABLED//デバッグログ有効化
	#define GASHA_VERBOSE_DEBUG_IS_ENABLED//冗長デバッグ機能有効化
	#define GASHA_ASSERTION_IS_ENABLED//アサーション／ブレークポイント／ウォッチポイント有効化
	#define GASHA_CALLPOINT_IS_ENABLED//コールポイント機能有効化
	#define GASHA_PROFILE_IS_AVAILABLE//プロファイル機能利用可能
	#define GASHA_DEV_TOOLS_IS_AVAILABLE//開発ツール利用可能
	#define GASHA_NO_OPTIMIZED//最適化なし
	#define GASHA_FILE_SYSTEM_IS_FLEXIBLE//ローカル／ROM切り替えファイルシステム（ローカルデータで設定）
	#define GASHA_UNITE_TEST_ENABLED//ユニットテスト（の仕組みが）有効
	#define GASHA_HAS_SYNBOMS//シンボル情報あり
#endif//GASHA_BUILD_CONFIG_IS_DEBUG

//----------------------------------------
#ifdef GASHA_BUILD_CONFIG_IS_DEBUG_MODERATE//プログラム開発向け設定
	#define GASHA_BUILD_CONFIG_LEVEL 2
	#define GASHA_BUILD_CONFIG_NAME "DEBUG_MODERATE"
	#define GASHA_DEBUG_FEATURE_IS_ENABLED//デバッグ機能有効化
	#define GASHA_VERBOSE_DEBUG_IS_ENABLED//冗長デバッグ機能有効化
	#define GASHA_LOG_IS_ENABLED//デバッグログ有効化
	#define GASHA_ASSERTION_IS_ENABLED//アサーション／ブレークポイント／ウォッチポイント有効化
	#define GASHA_CALLPOINT_IS_ENABLED//コールポイント機能有効化
	#define GASHA_PROFILE_IS_AVAILABLE//プロファイル機能利用可能
	#define GASHA_DEV_TOOLS_IS_AVAILABLE//開発ツール利用可能
	#define GASHA_OPTIMIZED_MODERATELY//適度に最適化
	#define GASHA_FILE_SYSTEM_IS_FLEXIBLE//ローカル／ROM切り替えファイルシステム（ローカルデータで設定）
	#define GASHA_UNITE_TEST_ENABLED//ユニットテスト（の仕組みが）有効
	#define GASHA_HAS_SYNBOMS//シンボル情報あり
#endif//GASHA_BUILD_CONFIG_IS_DEBUG_MODERATE

//----------------------------------------
#ifdef GASHA_BUILD_CONFIG_IS_DEBUG_OPT//コンテンツ制作・QA向け設定
	#define GASHA_BUILD_CONFIG_LEVEL 3
	#define GASHA_BUILD_CONFIG_NAME "DEBUG_OPT"
	#define GASHA_DEBUG_FEATURE_IS_ENABLED//デバッグ機能有効化
	#define GASHA_LOG_IS_ENABLED//デバッグログ有効化
	#define GASHA_ASSERTION_IS_ENABLED//アサーション／ブレークポイント／ウォッチポイント有効化
	#define GASHA_CALLPOINT_IS_ENABLED//コールポイント機能有効化
	#define GASHA_PROFILE_IS_AVAILABLE//プロファイル機能利用可能
	#define GASHA_OPTIMIZED//最大限の最適化
	#define GASHA_FILE_SYSTEM_IS_FLEXIBLE//ローカル／ROM切り替えファイルシステム（ローカルデータで設定）
	#define GASHA_UNITE_TEST_ENABLED//ユニットテスト（の仕組みが）有効
	#define GASHA_HAS_SYNBOMS//シンボル情報あり
#endif//GASHA_BUILD_CONFIG_IS_DEBUG_OPT

//----------------------------------------
#ifdef GASHA_BUILD_CONFIG_IS_REGRESSION_TEST//自動回帰テスト向け設定
	#define GASHA_BUILD_CONFIG_LEVEL 4
	#define GASHA_BUILD_CONFIG_NAME "REGRESSON_TEST"
	#define GASHA_DEBUG_FEATURE_IS_ENABLED//デバッグ機能有効化
	#define GASHA_LOG_IS_ENABLED//デバッグログ有効化
	#define GASHA_ASSERTION_IS_ENABLED//アサーション／ブレークポイント／ウォッチポイント有効化
	#define GASHA_CALLPOINT_IS_ENABLED//コールポイント機能有効化
	#define GASHA_PROFILE_IS_AVAILABLE//プロファイル機能利用可能
	#define GASHA_OPTIMIZED//最大限の最適化
	#define GASHA_FILE_SYSTEM_IS_FLEXIBLE//ローカル／ROM切り替えファイルシステム（ローカルデータで設定）
	#define GASHA_UNITE_TEST_ENABLED//ユニットテスト（の仕組みが）有効
	#define GASHA_IS_REGRESSION_TEST//回帰テストモード有効
	#define GASHA_HAS_SYNBOMS//シンボル情報あり
#endif//GASHA_BUILD_CONFIG_IS_REGRESSION_TEST

//----------------------------------------
#ifdef GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE//製品テスト向け設定
	#define GASHA_BUILD_CONFIG_LEVEL 5
	#define GASHA_BUILD_CONFIG_NAME "LOCAL_RELEASE"
	#define GASHA_OPTIMIZED//最大限の最適化
	#define GASHA_FILE_SYSTEM_IS_FLEXIBLE//ローカル／ROM切り替えファイルシステム（ローカルデータで設定）
	#define GASHA_HAS_SYNBOMS//シンボル情報あり
#endif//GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE

//----------------------------------------
#ifdef GASHA_BUILD_CONFIG_IS_RELEASE//製品向け設定
	#define GASHA_BUILD_CONFIG_LEVEL 6
	#define GASHA_BUILD_CONFIG_NAME "RELEASE"
	#define GASHA_OPTIMIZED//最大限の最適化
	#define GASHA_FILE_SYSTEM_IS_ROM//ROM専用ファイルシステム
	#define GASHA_IS_STRIPPED_SYMBOLS//シンボル情報なし
#endif//GASHA_BUILD_CONFIG_IS_RELEASE

#endif//GASHA_INCLUDED_BUILD_CONFIGURATION_H

// End of file

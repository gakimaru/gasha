#pragma once
#ifndef GASHA_INCLUDED_CHRONO_H
#define GASHA_INCLUDED_CHRONO_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// chrono.h
// 時間処理系ユーティリティ【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <chrono>//C++11 std::chrono

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//時間処理
//--------------------------------------------------------------------------------

//----------------------------------------
//型
typedef GASHA_TIME_TYPE sec_t;//秒型

//----------------------------------------
//経過時間を算出
//※秒を返す
inline std::chrono::system_clock::time_point nowTime();
inline sec_t calcElapsedTime(const std::chrono::system_clock::time_point begin);
inline sec_t calcElapsedTime(const std::chrono::system_clock::time_point begin, const std::chrono::system_clock::time_point end);

//----------------------------------------
//経過時間を文字列化
//※文字列の長さ（終端を含まない）を返す。
enum timeStrFormat_type
{
	timeStr_S,//"秒"（例："0"）
	timeStr_S_MILLI,//"秒.ミリ秒"（例："0.000"）
	timeStr_S_MICRO,//"秒.マイクロ秒"（例："0.000000"）
	timeStr_S_NANO,//"秒.ナノ秒"（例："0.000000000"）
	
	timeStr_MMSS,//"分:秒"（例："00:00"）
	timeStr_MMSS_MILLI,//"分:秒.ミリ秒"（例："00:00.000"）
	timeStr_MMSS_MICRO,//"分:秒.マイクロ秒"（例："00:00.000000"）
	timeStr_MMSS_NANO,//"分:秒.ナノ秒"（例："00:00.000000000"）

	timeStr_HHMMSS,//"時:分:秒"（例："00:00:00"）
	timeStr_HHMMSS_MILLI,//"時:分:秒.ミリ秒"（例："00:00:00.000"）
	timeStr_HHMMSS_MICRO,//"時:分:秒.マイクロ秒"（例："00:00:00.000000"）
	timeStr_HHMMSS_NANO,//"時:分:秒.ナノ秒"（例："00:00:00.000000000"）
};
std::size_t timeToStr(char* buff, const sec_t time, const timeStrFormat_type format_type);

//----------------------------------------
//経過時間クラス
class elapsedTime
{
public:
	//開始時間を取得
	inline sec_t now();
	//開始時間をリセット
	//※開始時間を現在時間にリセット
	inline void reset();
public:
	//ムーブオペレータ
	inline elapsedTime& operator=(elapsedTime&& rhs);
	//コピーオペレータ
	inline elapsedTime& operator=(const elapsedTime& rhs);
public:
	//ムーブコンストラクタ
	inline elapsedTime(elapsedTime&& obj);
	//コピーコンストラクタ
	inline elapsedTime(const elapsedTime& obj);
	//コンストラクタ
	inline elapsedTime();
private:
	std::chrono::system_clock::time_point m_beginTime;//開始時間
};

//----------------------------------------
//プログラム経過時間

//プログラム経過時間取得
inline sec_t nowElapsedTime();

//プログラム経過時間リセット
//※計測開始時間を現在時間にリセット
inline void resetElapsedTime();

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/chrono.inl>

#endif//GASHA_INCLUDED_CHRONO_H

// End of file

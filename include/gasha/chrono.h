#pragma once
#ifndef GASHA_INCLUDED_CHRONO_H
#define GASHA_INCLUDED_CHRONO_H

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// chrono.h
// 時間処理系ユーティリティ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
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
//経過時間を算出
//※秒を返す
inline std::chrono::system_clock::time_point nowTime();
inline double calcElapsedTime(const std::chrono::system_clock::time_point begin);
inline double calcElapsedTime(const std::chrono::system_clock::time_point begin, const std::chrono::system_clock::time_point end);

//----------------------------------------
//経過時間クラス
class elapsedTime
{
public:
	//開始時間を取得
	inline double now();
	//開始時間をリセット
	//※開始時間を現在時間にリセット
	inline void reset();
public:
	//コンストラクタ
	inline elapsedTime();
private:
	std::chrono::system_clock::time_point m_beginTime;//開始時間
};

//----------------------------------------
//プログラム経過時間

//プログラム経過時間取得
inline double nowElapsedTime();

//プログラム経過時間リセット
//※計測開始時間を現在時間にリセット
inline void resetElapsedTime();

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/chrono.inl>

#endif//GASHA_INCLUDED_CHRONO_H

// End of file

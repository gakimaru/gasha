#pragma once
#ifndef GASHA_INCLUDED_CHRONO_INL
#define GASHA_INCLUDED_CHRONO_INL

//--------------------------------------------------------------------------------
//【テンプレートライブラリ含む】
// chrono.inl
// 時間処理系ユーティリティ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/chrono.h>//時間処理ユーティリティ【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//時間処理
//--------------------------------------------------------------------------------

//----------------------------------------
//経過時間を算出
inline std::chrono::system_clock::time_point nowTime()
{
	return std::chrono::system_clock::now();
}
inline time_type calcElapsedTime(const std::chrono::system_clock::time_point begin, const std::chrono::system_clock::time_point end)
{
	const auto duration = end - begin;
	const auto duration_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
	const auto duration_count = duration_nano.count();
	const time_type second = static_cast<time_type>(duration_count) / static_cast<time_type>(1000000000.);
	return second;
}
inline time_type calcElapsedTime(const std::chrono::system_clock::time_point begin)
{
	return calcElapsedTime(begin, nowTime());
}

//----------------------------------------
//経過時間クラス

//開始時間を取得
inline time_type elapsedTime::now()
{
	return calcElapsedTime(m_beginTime);
}

//開始時間をリセット
inline void elapsedTime::reset()
{
	m_beginTime = nowTime();
}

//ムーブオペレータ
inline elapsedTime& elapsedTime::operator=(elapsedTime&& rhs)
{
	m_beginTime = rhs.m_beginTime;
	return *this;
}

//コピーオペレータ
inline elapsedTime& elapsedTime::operator=(const elapsedTime& rhs)
{
	m_beginTime = rhs.m_beginTime;
	return *this;
}

//ムーブコンストラクタ
inline elapsedTime::elapsedTime(elapsedTime&& obj) :
	m_beginTime(obj.m_beginTime)
{}

//コピーコンストラクタ
inline elapsedTime::elapsedTime(const elapsedTime& obj) :
	m_beginTime(obj.m_beginTime)
{}

//コンストラクタ
inline elapsedTime::elapsedTime() :
	m_beginTime(nowTime())
{}

//----------------------------------------
//プログラム経過時間

//プログラム経過時間取得
inline time_type nowElapsedTime()
{
	extern elapsedTime g_elapsedTime;
	return g_elapsedTime.now();
}

//プログラム経過時間リセット
inline void resetElapsedTime()
{
	extern elapsedTime g_elapsedTime;
	g_elapsedTime.reset();
}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_CHRONO_H

// End of file

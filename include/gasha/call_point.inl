#pragma once
#ifndef GASHA_INCLUDED_CALL_POINT_INL
#define GASHA_INCLUDED_CALL_POINT_INL

//--------------------------------------------------------------------------------
// call_point.inl
// コールポイント【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/call_point.h>//コールポイント【宣言部】

#include <utility>//C++11 std::move

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//コールポイント
//--------------------------------------------------------------------------------

#ifdef GASHA_CALLPOINT_IS_ENABLED//デバッグログ無効時はまるごと無効化

//コールポイント種別
inline callPoint::callpoint_type callPoint::type() const
{
	return m_type;
}
//カテゴリ
inline callPoint::category_type callPoint::category() const
{
	return m_category;
}
//コールポイント名
inline const char* callPoint::name() const
{
	return m_name;
}
//コールポイント開始時間
inline GASHA_ time_type callPoint::beginTime() const
{
	return m_beginTime;
}
//経過時間
inline const GASHA_ elapsedTime callPoint::elapsedTime() const
{
	return m_elapsedTime;
}
//呼び出し元ソースファイル名
inline const char* callPoint::fileName() const
{
	return m_fileName;
}
//呼び出し元関数名
inline const char* callPoint::funcName() const
{
	return m_funcName;
}
//自動プロファイリング種別
inline callPoint::autoProfiling_type callPoint::autoProfiling() const
{
	return m_autoProfiling;
}
//コールスタック（前のコールポイント）
inline callPoint* callPoint::prevCallStack() const
{
	return m_prevCallStack;
}

//直近のコールポイントを検索
inline const callPoint* callPoint::find() const
{
	return m_callStackTop;
}

//自動プロファイリングを有効化／無効化
inline void callPoint::setAutoProfiling(const bool enabled)
{
	m_autoProfiling = enabled ? useAutoProfiling : noProfiling;
}

//ムーブオペレータ
inline callPoint& callPoint::operator=(callPoint&& rhs)
{
	m_prevCallStack = rhs.m_prevCallStack;
	m_name = rhs.m_name;
	m_fileName = rhs.m_fileName;
	m_funcName = rhs.m_funcName;
	m_beginTime = rhs.m_beginTime;
	m_elapsedTime = std::move(rhs.m_elapsedTime);
	m_category = rhs.m_category;
	m_type = rhs.m_type;
	m_autoProfiling = rhs.m_autoProfiling;

	rhs.m_type = isReference;
	rhs.m_prevCallStack = nullptr;
	rhs.m_autoProfiling = noProfiling;
	if (m_callStackTop == &rhs)
		m_callStackTop = this;
	return *this;
}

//ムーブコンストラクタ
inline callPoint::callPoint(callPoint&& obj) :
	m_prevCallStack(obj.m_prevCallStack),
	m_name(obj.m_name),
	m_fileName(obj.m_fileName),
	m_funcName(obj.m_funcName),
	m_beginTime(obj.m_beginTime),
	m_elapsedTime(std::move(obj.m_elapsedTime)),
	m_category(obj.m_category),
	m_type(obj.m_type),
	m_autoProfiling(obj.m_autoProfiling)
{
	obj.m_type = isReference;
	obj.m_prevCallStack = nullptr;
	obj.m_autoProfiling = noProfiling;
	if (m_callStackTop == &obj)
		m_callStackTop = this;
}

//コンストラクタ
inline callPoint::callPoint(const callPoint::category_type category, const char* name, const char* file_name, const char* func_name) :
	callPoint(isNormal, category, name, noProfiling, file_name, func_name)//委譲コンストラクタ
{}
inline callPoint::callPoint(const callPoint::category_type category, const char* name, const autoProfiling_type auto_profiling, const char* file_name, const char* func_name) :
	callPoint(isNormal, category, name, auto_profiling, file_name, func_name)//委譲コンストラクタ
{}
inline callPoint::callPoint(const callPoint::callpoint_type type, const callPoint::category_type category, const char* name, const autoProfiling_type auto_profiling, const char* file_name, const char* func_name) :
	m_prevCallStack(m_callStackTop),
	m_name(name),
	m_fileName(file_name),
	m_funcName(func_name),
	m_beginTime(GASHA_ nowElapsedTime()),
	m_elapsedTime(),
	m_category(category),
	m_type(type),
	m_autoProfiling(auto_profiling)
{
	m_callStackTop = this;
}

//デフォルトコンストラクタ
inline callPoint::callPoint() :
	m_prevCallStack(nullptr),
	m_name(nullptr),
	m_fileName(nullptr),
	m_funcName(nullptr),
	m_beginTime(static_cast<GASHA_ time_type>(0.)),
	m_elapsedTime(),
	m_category(0),
	m_type(isReference),
	m_autoProfiling(noProfiling)
{}

//デストラクタ
inline callPoint::~callPoint()
{
	//自動プロファイリング
	if (m_autoProfiling == useAutoProfiling)
		profiling();

	//コールポイントスタックを戻す
	if (m_type != isReference)
		m_callStackTop = m_prevCallStack;
}

//----------------------------------------
//重大コールポイント

//ムーブオペレータ
inline criticalCallPoint& criticalCallPoint::operator=(criticalCallPoint&& rhs)
{
	*static_cast<callPoint*>(this) = std::move(*static_cast<callPoint*>(&rhs));
	return *this;
}

//ムーブコンストラクタ
inline criticalCallPoint::criticalCallPoint(criticalCallPoint&& obj) :
	callPoint(std::move(*static_cast<callPoint*>(&obj)))
{}

//コンストラクタ
inline criticalCallPoint::criticalCallPoint(const callPoint::category_type category, const char* name, const char* file_name, const char* func_name) :
	callPoint(isCritical, category, name, noProfiling, file_name, func_name)
{}

inline criticalCallPoint::criticalCallPoint(const callPoint::category_type category, const char* name, const callPoint::autoProfiling_type auto_profiling, const char* file_name, const char* func_name) :
	callPoint(isCritical, category, name, auto_profiling, file_name, func_name)
{}

//デストラクタ
inline criticalCallPoint::~criticalCallPoint()
{}

#endif//GASHA_CALLPOINT_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_CALL_POINT_INL

// End of file

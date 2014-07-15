#pragma once
#ifndef GASHA_INCLUDED_LOG_MASK_INL
#define GASHA_INCLUDED_LOG_MASK_INL

//--------------------------------------------------------------------------------
// log_mask.inl
// ログマスク【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_mask.h>//ログマスク【宣言部】

#include <cstring>//memcpy()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログマスク
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//--------------------
//イテレータ

//コンソール
inline const GASHA_ IConsole* logMask::iterator::console(const logMask::purpose_type purpose, const logMask::level_type level) const
{
	return m_logMask->console(purpose, level, m_logCategory);
}
inline GASHA_ IConsole* logMask::iterator::console(const logMask::purpose_type purpose, const logMask::level_type level)
{
	return m_logMask->console(purpose, level, m_logCategory);
}

//コンソールカラー
inline const GASHA_ consoleColor* logMask::iterator::color(const logMask::purpose_type purpose, const logMask::level_type level) const
{
	return m_logMask->color(purpose, level, m_logCategory);
}

//ムーブオペレータ
inline logMask::iterator& logMask::iterator::operator=(logMask::iterator&& rhs)
{
	m_logMask = rhs.m_logMask;
	m_logCategory = rhs.m_logCategory;
	return *this;
}

//コピーオペレータ
inline logMask::iterator& logMask::iterator::operator=(const logMask::iterator& rhs)
{
	m_logMask = rhs.m_logMask;
	m_logCategory = rhs.m_logCategory;
	return *this;
}

//ムーブコンストラクタ
inline logMask::iterator::iterator(logMask::iterator&& ite) :
	m_logMask(ite.m_logMask),
	m_logCategory(ite.m_logCategory)
{}

//コピーコンストラクタ
inline logMask::iterator::iterator(const logMask::iterator& ite) :
	m_logMask(ite.m_logMask),
	m_logCategory(ite.m_logCategory)
{}

//デフォルトコンストラクタ
inline logMask::iterator::iterator() :
	m_logMask(nullptr),
	m_logCategory()
{}

//デストラクタ
inline logMask::iterator::~iterator()
{}

//--------------------
//リバースイテレータ

//コンソール
inline const GASHA_ IConsole* logMask::reverse_iterator::console(const logMask::purpose_type purpose, const logMask::level_type level) const
{
	return m_logMask->console(purpose, level, m_logCategory);
}
inline GASHA_ IConsole* logMask::reverse_iterator::console(const logMask::purpose_type purpose, const logMask::level_type level)
{
	return m_logMask->console(purpose, level, m_logCategory);
}

//コンソールカラー
inline const GASHA_ consoleColor* logMask::reverse_iterator::color(const logMask::purpose_type purpose, const logMask::level_type level) const
{
	return m_logMask->color(purpose, level, m_logCategory);
}

//ベースイテレータを取得
inline logMask::iterator logMask::reverse_iterator::base() const
{
	GASHA_ logCategoryContainer::iterator category(m_logCategory.base());
	logMask::iterator ite(m_logMask, category);
	return ite;
}

//ムーブオペレータ
inline logMask::reverse_iterator& logMask::reverse_iterator::operator=(logMask::reverse_iterator&& rhs)
{
	m_logMask = rhs.m_logMask;
	m_logCategory = rhs.m_logCategory;
	return *this;
}

//コピーオペレータ
inline logMask::reverse_iterator& logMask::reverse_iterator::operator=(const logMask::reverse_iterator& rhs)
{
	m_logMask = rhs.m_logMask;
	m_logCategory = rhs.m_logCategory;
	return *this;
}

//ムーブコンストラクタ
inline logMask::reverse_iterator::reverse_iterator(logMask::reverse_iterator&& ite) :
	m_logMask(ite.m_logMask),
	m_logCategory(ite.m_logCategory)
{}

//コピーコンストラクタ
inline logMask::reverse_iterator::reverse_iterator(const logMask::reverse_iterator& ite) :
	m_logMask(ite.m_logMask),
	m_logCategory(ite.m_logCategory)
{}

//デフォルトコンストラクタ
inline logMask::reverse_iterator::reverse_iterator() :
	m_logMask(nullptr),
	m_logCategory()
{}

//デストラクタ
inline logMask::reverse_iterator::~reverse_iterator()
{}

//----------------------------------------
//ログマスク

//ログレベルマスクを取得
inline logMask::level_type logMask::level(const logMask::purpose_type purpose, const logMask::category_type category) const
{
	if (category < CATEGORY_MIN || category > CATEGORY_MAX)
		return LEVEL_MIN;//無効なカテゴリの場合、最も低いログレベルを返す
	return m_refMask->m_level[purpose][category];
}

//出力可能なログレベルか？
inline bool logMask::isEnableLevel(const logMask::purpose_type purpose, const GASHA_ logLevel& require_level, const logMask::category_type category) const
{
	GASHA_ logLevel mask_level(level(purpose, category));
	return mask_level.isExist() && require_level >= mask_level;
}

//出力可能なログレベルか？
inline bool logMask::isEnableLevel(const logMask::purpose_type purpose, const logMask::level_type require_level, const logMask::category_type category) const
{
	GASHA_ logLevel require_level_obj(require_level);
	return isEnableLevel(purpose, require_level_obj, category);
}

//コンソール取得
inline const GASHA_ IConsole* logMask::console(const logMask::purpose_type purpose, const GASHA_ logLevel& require_level, const logMask::category_type category) const
{
	if (!isEnableLevel(purpose, require_level, category))
		return nullptr;
	GASHA_ logCategory category_obj(category);
	const GASHA_ IConsole* console_obj = category_obj.console(purpose);
	if (!console_obj)
		console_obj = require_level.console(purpose);
	return console_obj;
}
inline const GASHA_ IConsole* logMask::console(const logMask::purpose_type purpose, const logMask::level_type require_level, const logMask::category_type category) const
{
	GASHA_ logLevel require_level_obj(require_level);
	return console(purpose, require_level_obj, category);
}
inline GASHA_ IConsole* logMask::console(const logMask::purpose_type purpose, const GASHA_ logLevel& require_level, const logMask::category_type category)
{
	GASHA_ logLevel require_level_obj(require_level);
	return const_cast<GASHA_ IConsole*>(const_cast<const logMask*>(this)->console(purpose, require_level_obj, category));
}
inline GASHA_ IConsole* logMask::console(const logMask::purpose_type purpose, const logMask::level_type require_level, const logMask::category_type category)
{
	GASHA_ logLevel require_level_obj(require_level);
	return const_cast<GASHA_ IConsole*>(const_cast<const logMask*>(this)->console(purpose, require_level_obj, category));
}

//コンソールカラー取得 
inline const GASHA_ consoleColor* logMask::color(const logMask::purpose_type purpose, const GASHA_ logLevel& require_level, const logMask::category_type category) const
{
	if (!isEnableLevel(purpose, require_level, category))
		return nullptr;
	return &require_level.color(purpose);
}
inline const GASHA_ consoleColor* logMask::color(const logMask::purpose_type purpose, const logMask::level_type require_level, const logMask::category_type category) const
{
	GASHA_ logLevel require_level_obj(require_level);
	return color(purpose, require_level_obj, category);
}

//ムーブオペレータ
inline logMask& logMask::operator=(logMask&& rhs)
{
	m_refType = rhs.m_refType;
	m_refMask = rhs.m_refMask;
	m_prevTlsMask = rhs.m_prevTlsMask;//変更前のTLSログレベルマスクはコピーし、
	rhs.m_prevTlsMask = nullptr;      //ムーブ元からは削除（ムーブ元はデストラクタで復元しなくなる）
	if (m_refType == isLocal)//ローカルログレベルマスクは、現在の種別がローカルの時だけコピー
		m_localMask = rhs.m_localMask;
	return *this;
}

//コピーオペレータ
inline logMask& logMask::operator=(const logMask& rhs)
{
	m_refType = rhs.m_refType;
	m_refMask = rhs.m_refMask;
	m_prevTlsMask = nullptr;//変更前のTLSログレベルマスクはコピーしない（デストラクタで復元しない）
	if (m_refType == isLocal)//ローカルログレベルマスクは、現在の種別がローカルの時だけコピー
		m_localMask = rhs.m_localMask;
	return *this;
}

//ムーブコンストラクタ
inline logMask::logMask(logMask&& obj) :
	m_refType(obj.m_refType),
	m_refMask(obj.m_refMask),
	m_prevTlsMask(obj.m_prevTlsMask)//変更前のTLSログレベルマスクはコピーし、
	                                //ムーブ元からは削除（ムーブ元はデストラクタで復元しなくなる）
{
	obj.m_prevTlsMask = nullptr;//ムーブ元無効化
	if (m_refType == isLocal)//ローカルログレベルマスクは、現在の種別がローカルの時だけコピー
		m_localMask = obj.m_localMask;
}

//コピーコンストラクタ
inline logMask::logMask(const logMask& obj) :
	m_refType(obj.m_refType),
	m_refMask(obj.m_refMask),
	m_prevTlsMask(nullptr)//変更前のTLSログレベルマスクはコピーしない（デストラクタで復元しない）
{
	if (m_refType == isLocal)//ローカルログレベルマスクは、現在の種別がローカルの時だけコピー
		m_localMask = obj.m_localMask;
}

//明示的な初期化用コンストラクタ
inline logMask::logMask(const explicitInitialize_t&)
{
	std::call_once(m_initialized, initializeOnce);//グローバルログマスク初期化（一回限り）
}

//デストラクタ
inline logMask::~logMask()
{
	if (m_prevTlsMask)//TLSログレベルマスクを復元
		m_tlsMaskRef = m_prevTlsMask;
}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_MASK_INL

// End of file

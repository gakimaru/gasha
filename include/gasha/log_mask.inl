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
inline const GASHA_ IConsole* logMask::iterator::console(const GASHA_ logPurpose purpose, const level_type level) const
{
	return m_logMask->console(purpose, m_logCategory, level);
}

//コンソール
inline GASHA_ IConsole* logMask::iterator::console(const GASHA_ logPurpose purpose, const level_type level)
{
	return m_logMask->console(purpose, m_logCategory, level);
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
inline const GASHA_ IConsole* logMask::reverse_iterator::console(const GASHA_ logPurpose purpose, const level_type level) const
{
	return m_logMask->console(purpose, m_logCategory, level);
}

//コンソール
inline GASHA_ IConsole* logMask::reverse_iterator::console(const GASHA_ logPurpose purpose, const level_type level)
{
	return m_logMask->console(purpose, m_logCategory, level);
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

//ログレベルを取得
inline logMask::level_type logMask::levelMask(const logPurpose purpose, const category_type category) const
{
	if (category < CATEGORY_MIN || category > CATEGORY_MAX)
		return LEVEL_MIN;//無効なカテゴリの場合、最も低いログレベルを返す
	if (m_localLogLevelIsEnabled)
		return m_localLogLevel[purpose][category];
	return m_globalLogLevel[purpose][category];
}

//出力可能なログレベルか？
inline bool logMask::isEnableLevel(const logPurpose purpose, const logMask::category_type category, const GASHA_ logLevel& require_level) const
{
	GASHA_ logLevel level_mask(this->levelMask(purpose, category));
	return level_mask.isExist() && require_level >= level_mask;
}

//出力可能なログレベルか？
inline bool logMask::isEnableLevel(const logPurpose purpose, const logMask::category_type category, const level_type require_level) const
{
	GASHA_ logLevel require_level_obj(require_level);
	return isEnableLevel(purpose, category, require_level_obj);
}

//コンソール取得
inline const GASHA_ IConsole* logMask::console(const logPurpose purpose, const category_type category, const GASHA_ logLevel& require_level) const
{
	if (!isEnableLevel(purpose, category, require_level))
		return nullptr;
	GASHA_ logCategory category_obj(category);
	const GASHA_ IConsole* console_obj = category_obj.console(purpose);
	if (!console_obj)
		console_obj = require_level.console(purpose);
	return console_obj;
}
inline const GASHA_ IConsole* logMask::console(const logPurpose purpose, const category_type category, const level_type require_level) const
{
	GASHA_ logLevel require_level_obj(require_level);
	return console(purpose, category, require_level_obj);
}

//コンソール取得 
inline GASHA_ IConsole* logMask::console(const logPurpose purpose, const category_type category, const GASHA_ logLevel& require_level)
{
	const GASHA_ IConsole* console_obj = reinterpret_cast<const logMask*>(this)->console(purpose, category, require_level);
	return const_cast<GASHA_ IConsole*>(console_obj);
}
inline GASHA_ IConsole* logMask::console(const logPurpose purpose, const category_type category, const level_type require_level)
{
	GASHA_ logLevel require_level_obj(require_level);
	return console(purpose, category, require_level_obj);
}

//ローカルログレベルを有効にする
inline void logMask::enableLocalLogLevel()
{
	if (!m_localLogLevelIsEnabled)
	{
		m_localLogLevelIsEnabled = true;
		m_localLogLevelIsCopied = true;
		memcpy(m_localLogLevel, m_globalLogLevel, sizeof(m_localLogLevel));
	}
}

//ローカルログレベルを無効にする
inline void logMask::disableLocalLogLevel()
{
	m_localLogLevelIsEnabled = false;
}

//ローカルログレベルを再度有効にする
inline bool logMask::resumeLocalLogLevel()
{
	if (!m_localLogLevelIsCopied)
		return false;
	m_localLogLevelIsEnabled = true;
	return true;
}

//ムーブオペレータ
inline logMask& logMask::operator=(logMask&& rhs)
{
	m_localLogLevelIsEnabled = rhs.m_localLogLevelIsEnabled;
	m_localLogLevelIsCopied = rhs.m_localLogLevelIsCopied;
	if (m_localLogLevelIsCopied)
		memcpy(m_localLogLevel, rhs.m_localLogLevel, sizeof(m_localLogLevel));
	//rhs.m_localLogLevelIsEnabled = false;//ムーブ元を無効化
	return *this;
}

//コピーオペレータ
inline logMask& logMask::operator=(const logMask& rhs)
{
	m_localLogLevelIsEnabled = rhs.m_localLogLevelIsEnabled;
	m_localLogLevelIsCopied = rhs.m_localLogLevelIsCopied;
	if (m_localLogLevelIsCopied)
		memcpy(m_localLogLevel, rhs.m_localLogLevel, sizeof(m_localLogLevel));
	return *this;
}

//ムーブコンストラクタ
inline logMask::logMask(logMask&& obj) :
	m_localLogLevelIsEnabled(obj.m_localLogLevelIsEnabled),
	m_localLogLevelIsCopied(obj.m_localLogLevelIsCopied)
{
	if (m_localLogLevelIsCopied)
		memcpy(m_localLogLevel, obj.m_localLogLevel, sizeof(m_localLogLevel));
	//obj.m_localLogLevelIsEnabled = false;//ムーブ元を無効化
}

//コピーコンストラクタ
inline logMask::logMask(const logMask& obj) :
	m_localLogLevelIsEnabled(obj.m_localLogLevelIsEnabled),
	m_localLogLevelIsCopied(obj.m_localLogLevelIsCopied)
{
	if (m_localLogLevelIsCopied)
		memcpy(m_localLogLevel, obj.m_localLogLevel, sizeof(m_localLogLevel));
}

//明示的な初期化用コンストラクタ
inline logMask::logMask(const explicitInitialize_t&)
{
	std::call_once(m_initialized, initializeOnce);//グローバルログマスク初期化（一回限り）
}

//コンストラクタ
inline logMask::logMask(const bool enable_local) :
	m_localLogLevelIsEnabled(false),
	m_localLogLevelIsCopied(false)
{
#ifdef GASHA_LOG_MASK_SECURE_INITIALIZE
	std::call_once(m_initialized, initializeOnce);//グローバルログマスク初期化（一回限り）
#endif//GASHA_LOG_MASK_SECURE_INITIALIZE
	if (enable_local)
		enableLocalLogLevel();
}

//デフォルトコンストラクタ
inline logMask::logMask() :
	m_localLogLevelIsEnabled(false),
	m_localLogLevelIsCopied(false)
{
#ifdef GASHA_LOG_MASK_SECURE_INITIALIZE
	std::call_once(m_initialized, initializeOnce);//グローバルログマスク初期化（一回限り）
#endif//GASHA_LOG_MASK_SECURE_INITIALIZE
	//memset(m_localLogLevel, 0, sizeof(m_localLogLevel));//初期化しない
}

//デストラクタ
inline logMask::~logMask()
{}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_MASK_INL

// End of file

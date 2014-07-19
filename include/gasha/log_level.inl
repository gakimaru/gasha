#pragma once
#ifndef GASHA_INCLUDED_LOG_LEVEL_INL
#define GASHA_INCLUDED_LOG_LEVEL_INL

//--------------------------------------------------------------------------------
// log_level.inl
// ログレベル【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_level.h>//ログレベル【宣言部】

#include <cstddef>//std::size_t
#include <cstring>//std::memcpy()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログレベル
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログレベル

//ムーブオペレータ
inline logLevel& logLevel::operator=(logLevel&& rhs)
{
	m_info = rhs.m_info;
	return *this;
}

//コピーオペレータ
inline logLevel& logLevel::operator=(const logLevel& rhs)
{
	m_info = rhs.m_info;
	return *this;
}

//ムーブコンストラクタ
inline logLevel::logLevel(logLevel&& obj) :
	m_info(obj.m_info)
{}

//コピーコンストラクタ
inline logLevel::logLevel(const logLevel& obj) :
	m_info(obj.m_info)
{}

//コンテナ登録済みインスタンス取得用コンストラクタ
inline logLevel::logLevel(const logLevel::level_type value)
{
#ifdef GASHA_LOG_LEVEL_CONTAINER_SECURE_INITIALIZE
	logLevelContainer con;//コンテナをインスタンス化することで、確実に初期化を実行する
#endif//GASHA_LOG_LEVEL_CONTAINER_SECURE_INITIALIZE
	m_info = logLevelContainer::getInfo(value);//登録済みのログレベル情報を取得
}

//デフォルトコンストラクタ
inline logLevel::logLevel() :
	m_info(nullptr)
{}

//デストラクタ
inline logLevel::~logLevel()
{}

//（プライベート）コピーオペレータ
inline logLevel& logLevel::operator=(info*info)
{
	m_info = info;
	return *this;
}

//（プライベート）コンストラクタ
inline logLevel::logLevel(info* info) :
	m_info(info)
{}

//--------------------
//イテレータ

//ムーブオペレータ
inline logLevelContainer::iterator& logLevelContainer::iterator::operator=(logLevelContainer::iterator&& rhs)
{
	m_value = rhs.m_value;
	m_logLevel = rhs.m_logLevel;
	m_isEnd = rhs.m_isEnd;
	return *this;
}

//コピーオペレータ
inline logLevelContainer::iterator& logLevelContainer::iterator::operator=(const logLevelContainer::iterator& rhs)
{
	m_value = rhs.m_value;
	m_logLevel = rhs.m_logLevel;
	m_isEnd = rhs.m_isEnd;
	return *this;
}

//ムーブコンストラクタ
inline logLevelContainer::iterator::iterator(logLevelContainer::iterator&& ite) :
	m_value(ite.m_value),
	m_logLevel(ite.m_logLevel),
	m_isEnd(ite.m_isEnd)
{}

//コピーコンストラクタ
inline logLevelContainer::iterator::iterator(const logLevelContainer::iterator& ite) :
	m_value(ite.m_value),
	m_logLevel(ite.m_logLevel),
	m_isEnd(ite.m_isEnd)
{}

//デフォルトコンストラクタ
inline logLevelContainer::iterator::iterator() :
	m_value(logLevel::END),
	m_logLevel(nullptr),
	m_isEnd(true)
{}

//デストラクタ
inline logLevelContainer::iterator::~iterator()
{}

//--------------------
//リバースイテレータ

//ベースイテレータを取得
inline logLevelContainer::iterator logLevelContainer::reverse_iterator::base() const
{
	logLevelContainer::iterator ite(m_value);
	return ite;
}

//ムーブオペレータ
inline logLevelContainer::reverse_iterator& logLevelContainer::reverse_iterator::operator=(logLevelContainer::reverse_iterator&& rhs)
{
	m_value = rhs.m_value;
	m_logLevel = rhs.m_logLevel;
	m_isEnd = rhs.m_isEnd;
	return *this;
}

//コピーオペレータ
inline logLevelContainer::reverse_iterator& logLevelContainer::reverse_iterator::operator=(const logLevelContainer::reverse_iterator& rhs)
{
	m_value = rhs.m_value;
	m_logLevel = rhs.m_logLevel;
	m_isEnd = rhs.m_isEnd;
	return *this;
}

//ムーブコンストラクタ
inline logLevelContainer::reverse_iterator::reverse_iterator(logLevelContainer::reverse_iterator&& ite) :
	m_value(ite.m_value),
	m_logLevel(ite.m_logLevel),
	m_isEnd(ite.m_isEnd)
{}

//コピーコンストラクタ
inline logLevelContainer::reverse_iterator::reverse_iterator(const logLevelContainer::reverse_iterator& ite) :
	m_value(ite.m_value),
	m_logLevel(ite.m_logLevel),
	m_isEnd(ite.m_isEnd)
{}

//デフォルトコンストラクタ
inline logLevelContainer::reverse_iterator::reverse_iterator() :
	m_value(logLevel::BEGIN),
	m_logLevel(nullptr),
	m_isEnd(true)
{}

//デストラクタ
inline logLevelContainer::reverse_iterator::~reverse_iterator()
{}

//--------------------
//コンテナ（イテレータ用）

//要素を取得
inline logLevel::info* logLevelContainer::getInfo(const logLevel::level_type value)
{
	if (value < logLevel::MIN || value > logLevel::MAX || !m_isAlreadyPool[value])
		return nullptr;
	return &m_pool[value];
}

//明示的な初期化用コンストラクタ
inline logLevelContainer::logLevelContainer(const explicitInit_type&)
{
	std::call_once(m_initialized, initializeOnce);//コンテナ初期化（一回限り）
}

//デフォルトコンストラクタ
inline logLevelContainer::logLevelContainer()
{
#ifdef GASHA_LOG_LEVEL_CONTAINER_SECURE_INITIALIZE
	std::call_once(m_initialized, initializeOnce);//コンテナ初期化（一回限り）
#endif//GASHA_LOG_LEVEL_CONTAINER_SECURE_INITIALIZE
}

//デストラクタ
inline logLevelContainer::~logLevelContainer()
{}

//----------------------------------------
//ログレベル登録テンプレートクラス

//コンストラクタ
template<unsigned char _LEVEL>
inline bool regLogLevel<_LEVEL>::operator()(const char* name, GASHA_ iConsole* (&consoles)[logLevel::PURPOSE_NUM], GASHA_ consoleColor (&colors)[logLevel::PURPOSE_NUM])
{
	logLevel::info info =
	{
		name,
		LEVEL
	};
	for (logLevel::purpose_type purpose = 0; purpose < logLevel::PURPOSE_NUM; ++purpose)
	{
		info.m_consoles[purpose] = consoles[purpose];
		info.m_colors[purpose] = colors[purpose];
	}
	logLevelContainer con;//コンテナ初期化のためのインスタンス化
	return logLevelContainer::regist(info);
}

namespace _private
{
	//----------------------------------------
	//ログレベル登録用テンプレートクラス（コンテナ初期化処理専用）
	template<unsigned char _LEVEL>
	class regLogLevel
	{
	public:
		//定数
		static const logLevel::level_type LEVEL = _LEVEL;//レベルの値
		static_assert(LEVEL >= logLevel::NORMAL_MIN && LEVEL <= logLevel::NORMAL_MAX, "Out of range of normal-log-level");//値の範囲チェック
	public:
		//関数オペレータ
		inline bool operator()(const char* name, GASHA_ iConsole* (&consoles)[logLevel::PURPOSE_NUM], GASHA_ consoleColor* (&colors)[logLevel::PURPOSE_NUM])
		{
			logLevel::info info =
			{
				name,
				LEVEL
			};
			for (logLevel::purpose_type purpose = 0; purpose < logLevel::PURPOSE_NUM; ++purpose)
			{
				info.m_consoles[purpose] = consoles[purpose];
				if (colors[purpose])
					info.m_colors[purpose] = *colors[purpose];
				else
					info.m_colors[purpose] = consoleColor(consoleColor::stdColor);
			}
			return logLevelContainer::regist(info);
		}
	};
	
	//----------------------------------------
	//レベル登録用テンプレートクラス（コンテナ初期化処理＆特殊ログレベル専用）
	template<unsigned char _LEVEL>
	class regSpecialLogLevel
	{
	public:
		//定数
		static const logLevel::level_type LEVEL = _LEVEL;//レベルの値
		static_assert(LEVEL >= logLevel::SPECIAL_MIN && LEVEL <= logLevel::SPECIAL_MAX, "Out of range of special-log-level");//値の範囲チェック
	public:
		//関数オペレータ
		inline bool operator()(const char* name)
		{
			logLevel::info info =
			{
				name,
				LEVEL
			};
			for (logLevel::purpose_type purpose = 0; purpose < logLevel::PURPOSE_NUM; ++purpose)
			{
				info.m_consoles[purpose] = nullptr;
				info.m_colors[purpose] = GASHA_ consoleColor(GASHA_ consoleColor::stdColor);
			}
			return logLevelContainer::regist(info);
		}
	};
}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_LEVEL_INL

// End of file

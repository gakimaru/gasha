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

#include <cstring>//std::memcpy()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログレベル
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログレベルクラス

//コピーオペレータ
inline logLevel& logLevel::operator = (const logLevel& rhs)
{
	std::memcpy(this, &rhs, sizeof(*this));//強制更新
	return *this;
}

//前のレベルを取得
inline logLevel* logLevel::prev() const
{
	iterator ite(m_value);
	--ite;
	return ite.m_ref;
}

//次のレベルを取得
inline logLevel* logLevel::next() const
{
	iterator ite(m_value);
	++ite;
	return ite.m_ref;
}

	//ムーブコンストラクタ
inline logLevel::logLevel(logLevel&& obj) :
	m_name(obj.m_name),
	m_value(obj.m_value),
	m_forLog(obj.m_forLog),
	m_forNotice(obj.m_forNotice),
	m_forMask(obj.m_forMask),
	m_console(obj.m_console),
	m_consoleForNotice(obj.m_consoleForNotice),
	m_color(std::move(obj.m_color)),
	m_colorForNotice(std::move(obj.m_colorForNotice))
{}

//コピーコンストラクタ
inline logLevel::logLevel(const logLevel& obj) :
	m_name(obj.m_name),
	m_value(obj.m_value),
	m_forLog(obj.m_forLog),
	m_forNotice(obj.m_forNotice),
	m_forMask(obj.m_forMask),
	m_console(obj.m_console),
	m_consoleForNotice(obj.m_consoleForNotice),
	m_color(obj.m_color),
	m_colorForNotice(obj.m_colorForNotice)
{}

//コンストラクタ
inline logLevel::logLevel(const logLevel::level_type value, const char* name, const bool for_log, const bool for_notice, const bool for_mask, GASHA_ IConsole* console, GASHA_ IConsole* console_for_notice, GASHA_ consoleColor&& color, GASHA_ consoleColor&& color_for_notice) :
	m_name(name),
	m_value(value),
	m_forLog(for_log),
	m_forNotice(for_notice),
	m_forMask(for_mask),
	m_console(console),
	m_consoleForNotice(console_for_notice),
	m_color(std::move(color)),
	m_colorForNotice(std::move(color_for_notice))
{
	assert(value >= BEGIN && value <= END);
	container::update(m_value, *this);//コンテナに登録
}

//コンテナ登録済みインスタンス取得用コンストラクタ
inline logLevel::logLevel(const logLevel::level_type value) :
	m_name(nullptr),
	m_value(value),
	m_forLog(false),
	m_forNotice(false),
	m_forMask(false),
	m_console(nullptr),
	m_consoleForNotice(nullptr)
{
	assert(value >= BEGIN && value <= END);
	logLevel* obj = container::_at(m_value);//コンテナから取得して自身にコピー
	if (obj)
		*this = *obj;
}

//デフォルトコンストラクタ
inline logLevel::logLevel() :
	m_name(nullptr),
	m_value(0),
	m_forLog(false),
	m_forNotice(false),
	m_forMask(false),
	m_console(nullptr),
	m_consoleForNotice(nullptr)
{}

//デストラクタ
inline logLevel::~logLevel()
{}

//--------------------
//イテレータ

//ムーブコンストラクタ
inline logLevel::iterator::iterator(logLevel::iterator&& ite) :
	m_value(ite.m_value),
	m_ref(ite.m_ref),
	m_isEnd(ite.m_isEnd)
{}

//コピーコンストラクタ
inline logLevel::iterator::iterator(const logLevel::iterator& ite) :
	m_value(ite.m_value),
	m_ref(ite.m_ref),
	m_isEnd(ite.m_isEnd)
{}

//コンストラクタ
inline logLevel::iterator::iterator(const logLevel::level_type value) :
	m_value(value),
	m_ref(container::_at(value)),
	m_isEnd(value == container::endValue())
{
	if (!m_isEnd && !m_ref)
		m_value = container::beginValue();
}

//デフォルトコンストラクタ
inline logLevel::iterator::iterator() :
	m_value(container::endValue()),
	m_ref(nullptr),
	m_isEnd(true)
{}

//デストラクタ
inline logLevel::iterator::~iterator()
{}

//--------------------
//リバースイテレータ

//ベースイテレータを取得
inline logLevel::iterator logLevel::reverse_iterator::base() const
{
	logLevel::iterator ite(m_value);
	return ite;
}

//ムーブコンストラクタ
inline logLevel::reverse_iterator::reverse_iterator(logLevel::reverse_iterator&& ite) :
	m_value(ite.m_value),
	m_ref(ite.m_ref),
	m_isEnd(ite.m_isEnd)
{}

//コピーコンストラクタ
inline logLevel::reverse_iterator::reverse_iterator(const logLevel::reverse_iterator& ite) :
	m_value(ite.m_value),
	m_ref(ite.m_ref),
	m_isEnd(ite.m_isEnd)
{}

//コンストラクタ
inline logLevel::reverse_iterator::reverse_iterator(const logLevel::level_type value) :
	m_value(value),
	m_ref(container::_at(value - 1)),
	m_isEnd(value == container::endValue())
{
	if (!m_isEnd && !m_ref)
		m_value = container::beginValue();
}

//デフォルトコンストラクタ
inline logLevel::reverse_iterator::reverse_iterator() :
	m_value(container::endValue()),
	m_ref(nullptr),
	m_isEnd(true)
{}

//デストラクタ
inline logLevel::reverse_iterator::~reverse_iterator()
{}

//--------------------
//コンテナ（イテレータ用）

//要素を取得
inline logLevel* logLevel::container::_at(const logLevel::level_type value)
{
	if (value < MIN || value > MAX || !m_isAlreadyPool[value])
		return nullptr;
	return &m_poolPtr[value];
}

//コンストラクタ
inline logLevel::container::container()
{
	container::initializeOnce();//コンテナ初期化（一回限り）
}

//----------------------------------------
//レベル定義用テンプレートクラス：通常レベル用

//コンストラクタ
template<unsigned char V, bool _FOR_LOG, bool _FOR_NOTICE>
inline normalLogLevel<V, _FOR_LOG, _FOR_NOTICE>::normalLogLevel(const char* name, GASHA_ consoleColor&& color, GASHA_ consoleColor&& color_for_notice) :
	logLevel(VALUE, name, FOR_LOG, FOR_NOTICE, FOR_MASK, &GASHA_ stdConsole::instance(), &GASHA_ stdConsoleForNotice::instance(), std::move(color), std::move(color_for_notice))
{}

//----------------------------------------
//レベル定義用テンプレートクラス：特殊レベル用

//コンストラクタ
template<unsigned char V>
inline specialLogLevel<V>::specialLogLevel(const char* name) :
	logLevel(VALUE, name, FOR_LOG, FOR_NOTICE, FOR_MASK, &GASHA_ stdConsole::instance(), &GASHA_ stdConsoleForNotice::instance(), std::move(consoleColor()), std::move(consoleColor()))
{}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_LEVEL_INL

// End of file

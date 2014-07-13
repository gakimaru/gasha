#pragma once
#ifndef GASHA_INCLUDED_LOG_CATEGORY_INL
#define GASHA_INCLUDED_LOG_CATEGORY_INL

//--------------------------------------------------------------------------------
// log_category.inl
// ログカテゴリ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_category.h>//ログカテゴリ【宣言部】

#include <cstring>//memcpy()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログカテゴリ
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログカテゴリクラス

//指定のログカテゴリを取得
inline logCategory* logCategory::at(const category_type value)
{
	container con;
	return con.at(value);
}

//全てのログカテゴリのコンソールを変更
inline void logCategory::setAllConsole(IConsole* console)
{
	container con;
	con.setAllConsole(console);
}

//全てのログカテゴリの画面通知用コンソールを変更
inline void logCategory::setAllConsoleForNotice(IConsole* console)
{
	container con;
	con.setAllConsoleForNotice(console);
}

//コピーオペレータ
inline logCategory& logCategory::operator=(const logCategory& rhs)
{
	std::memcpy(this, &rhs, sizeof(*this));//強制更新
	return *this;
}

//前のカテゴリを取得
inline logCategory* logCategory::prev() const
{
	const category_type prev = fromOutputCategory(outputCategory() - 1);
	if (prev < NORMAL_MIN || prev > NORMAL_MAX)
		return nullptr;
	return at(prev);
}

//次のカテゴリを取得
inline logCategory* logCategory::next() const
{
	const category_type prev = fromOutputCategory(outputCategory() + 1);
	if (prev < NORMAL_MIN || prev > NORMAL_MAX)
		return nullptr;
	return at(prev);
}

//ムーブコンストラクタ
inline logCategory::logCategory(logCategory&& obj) :
m_name(obj.m_name),
m_value(obj.m_value),
m_console(obj.m_console),
m_consoleForNotice(obj.m_consoleForNotice),
m_color(std::move(obj.m_color)),
m_colorForNotice(std::move(obj.m_colorForNotice))
{}

//コピーコンストラクタ
inline logCategory::logCategory(const logCategory& obj) :
m_name(obj.m_name),
m_value(obj.m_value),
m_console(obj.m_console),
m_consoleForNotice(obj.m_consoleForNotice),
m_color(obj.m_color),
m_colorForNotice(obj.m_colorForNotice)
{}

//ログカテゴリ登録用コンストラクタ
inline logCategory::logCategory(const logCategory::category_type value, const char* name, GASHA_ IConsole* console, GASHA_ IConsole* console_for_notice, GASHA_ consoleColor&& color, GASHA_ consoleColor&& color_for_notice) :
m_name(name),
m_value(value),
m_console(console),
m_consoleForNotice(console_for_notice),
m_color(std::move(color)),
m_colorForNotice(std::move(color_for_notice))
{
	assert(value >= NORMAL_MIN && value <= NORMAL_MAX);
	//コンテナに登録
	container con;
	con.update(m_value, *this);
}

//特殊ログカテゴリ登録用コンストラクタ
inline logCategory::logCategory(const logCategory::category_type value, const char* name) :
m_name(name),
m_value(value),
m_console(nullptr),
m_consoleForNotice(nullptr)
{
	assert(value >= SPECIAL_MIN && value <= SPECIAL_MAX);
	//コンテナに登録
	container con;
	con.update(m_value, *this);
}

//コンテナ登録済みインスタンス取得用コンストラクタ
inline logCategory::logCategory(const logCategory::category_type value) :
m_name(nullptr),
m_value(value),
m_console(nullptr),
m_consoleForNotice(nullptr)
{
	assert(value >= BEGIN && value <= END);
	container con;
	logCategory* obj = con.at(m_value);//コンテナから取得して自身にコピー
	if (obj)
		*this = *obj;
}

//デフォルトコンストラクタ
inline logCategory::logCategory() :
m_name(nullptr),
m_value(0),
m_console(nullptr),
m_consoleForNotice(nullptr)
{}

//デストラクタ
inline logCategory::~logCategory()
{}

//--------------------
//イテレータ

//ムーブコンストラクタ
inline logCategory::iterator::iterator(logCategory::iterator&& ite) :
m_value(ite.m_value),
m_ref(ite.m_ref),
m_isEnd(ite.m_isEnd)
{}

//コピーコンストラクタ
inline logCategory::iterator::iterator(const logCategory::iterator& ite) :
m_value(ite.m_value),
m_ref(ite.m_ref),
m_isEnd(ite.m_isEnd)
{}

//コンストラクタ
inline logCategory::iterator::iterator(const logCategory::category_type value) :
m_value(value),
m_ref(container::_at(value)),
m_isEnd(value == END)
{
	if (!m_isEnd && !m_ref)
		m_value = INVALID;
}

//デフォルトコンストラクタ
inline logCategory::iterator::iterator() :
m_value(END),
m_ref(nullptr),
m_isEnd(true)
{}

//デストラクタ
inline logCategory::iterator::~iterator()
{}

//--------------------
//リバースイテレータ

//ベースイテレータを取得
inline logCategory::iterator logCategory::reverse_iterator::base() const
{
	logCategory::iterator ite(m_value);
	return ite;
}

//ムーブコンストラクタ
inline logCategory::reverse_iterator::reverse_iterator(logCategory::reverse_iterator&& ite) :
m_value(ite.m_value),
m_ref(ite.m_ref),
m_isEnd(ite.m_isEnd)
{}

//コピーコンストラクタ
inline logCategory::reverse_iterator::reverse_iterator(const logCategory::reverse_iterator& ite) :
m_value(ite.m_value),
m_ref(ite.m_ref),
m_isEnd(ite.m_isEnd)
{}

//コンストラクタ
inline logCategory::reverse_iterator::reverse_iterator(const logCategory::category_type value) :
m_value(value),
m_ref(container::_at(value - 1)),
m_isEnd(value == BEGIN)
{
	if (!m_isEnd && !m_ref)
		m_value = INVALID;
}

//デフォルトコンストラクタ
inline logCategory::reverse_iterator::reverse_iterator() :
m_value(BEGIN),
m_ref(nullptr),
m_isEnd(true)
{}

//デストラクタ
inline logCategory::reverse_iterator::~reverse_iterator()
{}

//--------------------
//コンテナ（イテレータ用）

//要素を取得
inline logCategory* logCategory::container::_at(const logCategory::category_type value)
{
	if (value < MIN || value > MAX || !m_isAlreadyPool[value])
		return nullptr;
	return &m_poolPtr[value];
}

//コンストラクタ
inline logCategory::container::container()
{
	std::call_once(m_initialized, container::initializeOnce);//コンテナ初期化（一回限り）
}

//----------------------------------------
//ログカテゴリ登録テンプレートクラス

//コンストラクタ
template<unsigned char V, bool FOR_LOG, bool FOR_NOTICE>
inline regLogCategory<V, FOR_LOG, FOR_NOTICE>::regLogCategory(const char* name, GASHA_ consoleColor&& color, GASHA_ consoleColor&& color_for_notice) :
logCategory(VALUE, name, FOR_LOG ? &GASHA_ stdConsole::instance() : nullptr, FOR_NOTICE ? &GASHA_ stdConsoleForNotice::instance() : nullptr, std::move(color), std::move(color_for_notice))
{}

//----------------------------------------
//特殊ログカテゴリ登録テンプレートクラス
namespace _private
{
	template<unsigned char V>
	class regSpecialLogCategory : public logCategory
	{
	public:
		//定数
		static const category_type VALUE = V;//値（カテゴリ）
		static_assert(VALUE >= SPECIAL_MIN && VALUE <= SPECIAL_MAX, "out of range of special-log-category");//値の範囲チェック
	public:
		//コンストラクタ
		inline regSpecialLogCategory(const char* name);
	};

	//コンストラクタ
	template<unsigned char V>
	inline regSpecialLogCategory<V>::regSpecialLogCategory(const char* name) :
		logCategory(VALUE, name)
	{}
}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_CATEGORY_INL

// End of file

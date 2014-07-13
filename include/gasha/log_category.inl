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
//ログカテゴリ

//ムーブオペレータ
inline logCategory& logCategory::operator=(logCategory&& rhs)
{
	m_info = rhs.m_info;
	return *this;
}

//コピーオペレータ
inline logCategory& logCategory::operator=(const logCategory& rhs)
{
	m_info = rhs.m_info;
	return *this;
}

//ムーブコンストラクタ
inline logCategory::logCategory(logCategory&& obj) :
	m_info(obj.m_info)
{}

//コピーコンストラクタ
inline logCategory::logCategory(const logCategory& obj) :
	m_info(obj.m_info)
{}

//コンテナ登録済みインスタンス取得用コンストラクタ
inline logCategory::logCategory(const logCategory::category_type value)
{
#ifdef GASHA_LOG_CATEGORY_CONTAINER_SECURE_INITIALIZE
	logCategoryContainer con;//コンテナをインスタンス化することで、確実に初期化を実行する
#endif//GASHA_LOG_CATEGORY_CONTAINER_SECURE_INITIALIZE
	m_info = logCategoryContainer::getInfo(value);//登録済みのログカテゴリ情報を取得
}

//デフォルトコンストラクタ
inline logCategory::logCategory() :
	m_info(nullptr)
{}

//デストラクタ
inline logCategory::~logCategory()
{}

//（プライベート）コピーオペレータ
inline logCategory& logCategory::operator=(info*info)
{
	m_info = info;
	return *this;
}

//（プライベート）コンストラクタ
inline logCategory::logCategory(info* info) :
	m_info(info)
{}

//--------------------
//イテレータ

//ムーブオペレータ
inline logCategoryContainer::iterator& logCategoryContainer::iterator::operator = (logCategoryContainer::iterator&& rhs)
{
	m_value = rhs.m_value;
	m_logCategory = rhs.m_logCategory;
	m_isEnd = rhs.m_isEnd;
	return *this;
}

//コピーオペレータ
inline logCategoryContainer::iterator& logCategoryContainer::iterator::operator=(const logCategoryContainer::iterator& rhs)
{
	m_value = rhs.m_value;
	m_logCategory = rhs.m_logCategory;
	m_isEnd = rhs.m_isEnd;
	return *this;
}

//ムーブコンストラクタ
inline logCategoryContainer::iterator::iterator(logCategoryContainer::iterator&& ite) :
	m_value(ite.m_value),
	m_logCategory(ite.m_logCategory),
	m_isEnd(ite.m_isEnd)
{}

//コピーコンストラクタ
inline logCategoryContainer::iterator::iterator(const logCategoryContainer::iterator& ite) :
	m_value(ite.m_value),
	m_logCategory(ite.m_logCategory),
	m_isEnd(ite.m_isEnd)
{}

//デフォルトコンストラクタ
inline logCategoryContainer::iterator::iterator() :
	m_value(logCategory::END),
	m_logCategory(nullptr),
	m_isEnd(true)
{}

//デストラクタ
inline logCategoryContainer::iterator::~iterator()
{}

//--------------------
//リバースイテレータ

//ベースイテレータを取得
inline logCategoryContainer::iterator logCategoryContainer::reverse_iterator::base() const
{
	logCategoryContainer::iterator ite(m_value);
	return ite;
}

//ムーブオペレータ
inline logCategoryContainer::reverse_iterator& logCategoryContainer::reverse_iterator::operator=(logCategoryContainer::reverse_iterator&& rhs)
{
	m_value = rhs.m_value;
	m_logCategory = rhs.m_logCategory;
	m_isEnd = rhs.m_isEnd;
	return *this;
}

//コピーオペレータ
inline logCategoryContainer::reverse_iterator& logCategoryContainer::reverse_iterator::operator=(const logCategoryContainer::reverse_iterator& rhs)
{
	m_value = rhs.m_value;
	m_logCategory = rhs.m_logCategory;
	m_isEnd = rhs.m_isEnd;
	return *this;
}

//ムーブコンストラクタ
inline logCategoryContainer::reverse_iterator::reverse_iterator(logCategoryContainer::reverse_iterator&& ite) :
	m_value(ite.m_value),
	m_logCategory(ite.m_logCategory),
	m_isEnd(ite.m_isEnd)
{}

//コピーコンストラクタ
inline logCategoryContainer::reverse_iterator::reverse_iterator(const logCategoryContainer::reverse_iterator& ite) :
	m_value(ite.m_value),
	m_logCategory(ite.m_logCategory),
	m_isEnd(ite.m_isEnd)
{}

//デフォルトコンストラクタ
inline logCategoryContainer::reverse_iterator::reverse_iterator() :
m_value(logCategory::BEGIN),
m_logCategory(nullptr),
	m_isEnd(true)
{}

//デストラクタ
inline logCategoryContainer::reverse_iterator::~reverse_iterator()
{}

//--------------------
//コンテナ（イテレータ用）

//要素を取得
inline logCategory::info* logCategoryContainer::getInfo(const logCategory::category_type value)
{
	if (value < logCategory::MIN || value > logCategory::MAX || !m_isAlreadyPool[value])
		return nullptr;
	return &m_pool[value];
}

//コンストラクタ
inline logCategoryContainer::logCategoryContainer()
{
	std::call_once(m_initialized, initializeOnce);//コンテナ初期化（一回限り）
}

//デストラクタ
inline logCategoryContainer::~logCategoryContainer()
{}

//----------------------------------------
//ログカテゴリ登録テンプレートクラス

//コンストラクタ
template<unsigned char _CATEGORY>
inline bool regLogCategory<_CATEGORY>::operator()(const char* name, GASHA_ IConsole* console, GASHA_ IConsole* console_for_notice)
{
	logCategory::info info =
	{
		name,
		CATEGORY,
		console,
		console_for_notice
	};
	return logCategoryContainer::regist(info);
}

namespace _private
{
	//----------------------------------------
	//特殊ログカテゴリ登録用テンプレートクラス
	//※テンプレートクラスにより、有効範囲内の値かコンパイル時にチェックする。
	template<unsigned char _CATEGORY>
	class regSpecialLogCategory
	{
	public:
		//定数
		static const logCategory::category_type CATEGORY = _CATEGORY;//値（カテゴリ）
		static_assert(CATEGORY >= logCategory::SPECIAL_MIN && CATEGORY <= logCategory::SPECIAL_MAX, "Out of range of special-log-category");//値の範囲チェック
	public:
		//関数オペレータ
		inline bool operator()(const char* name)
		{
			logCategory::info info =
			{
				name,
				CATEGORY,
				nullptr,
				nullptr
			};
			return logCategoryContainer::regist(info);
		}
	};
}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_CATEGORY_INL

// End of file

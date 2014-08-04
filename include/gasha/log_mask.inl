#pragma once
#ifndef GASHA_INCLUDED_LOG_MASK_INL
#define GASHA_INCLUDED_LOG_MASK_INL

//--------------------------------------------------------------------------------
// log_mask.inl
// ログレベルマスク【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_mask.h>//ログレベルマスク【宣言部】

#include <gasha/call_point.h>//コールポイント

#include <cstring>//std::memcpy()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログレベルマスク
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//--------------------
//イテレータ

//コンソール
inline const GASHA_ iConsole* logMask::iterator::console(const logMask::purpose_type purpose, const logMask::level_type level) const
{
	return m_logMask->console(purpose, level, m_logCategory);
}
inline GASHA_ iConsole* logMask::iterator::console(const logMask::purpose_type purpose, const logMask::level_type level)
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
inline const GASHA_ iConsole* logMask::reverse_iterator::console(const logMask::purpose_type purpose, const logMask::level_type level) const
{
	return m_logMask->console(purpose, level, m_logCategory);
}
inline GASHA_ iConsole* logMask::reverse_iterator::console(const logMask::purpose_type purpose, const logMask::level_type level)
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
//ログレベルマスク

//ログレベルマスクを取得
inline logMask::level_type logMask::level(const logMask::purpose_type purpose, const logMask::category_type category) const
{
	GASHA_ callPoint cp;
	const logMask::category_type _category = cp.properCategory(category);
	return m_maskRef->m_level[purpose][_category];
}

//出力可能なログレベルか？
inline bool logMask::isEnableLevel(const logMask::purpose_type purpose, const GASHA_ logLevel& require_level, const logMask::category_type category) const
{
	GASHA_ logLevel mask_level(level(purpose, category));
	return mask_level.isExist() && require_level >= mask_level;
}
inline bool logMask::isEnableLevel(const logMask::purpose_type purpose, const logMask::level_type require_level, const logMask::category_type category) const
{
	GASHA_ logLevel require_level_obj(require_level);
	return isEnableLevel(purpose, require_level_obj, category);
}

//コンソール取得
inline GASHA_ iConsole* logMask::console(const logMask::purpose_type purpose, const GASHA_ logLevel& level, const GASHA_ logCategory& category) const
{
	if (!level.isExist() || !category.isExist())
		return nullptr;
	GASHA_ iConsole* console_obj = category.console(purpose);
	if (console_obj)
		return console_obj;
	return level.console(purpose);
}
inline GASHA_ iConsole* logMask::console(const logMask::purpose_type purpose, const logMask::level_type level, const logMask::category_type category) const
{
	GASHA_ logLevel level_obj(level);
	GASHA_ logCategory category_obj(category);
	return console(purpose, level_obj, category_obj);
}
inline GASHA_ iConsole* logMask::console(const logMask::purpose_type purpose, const GASHA_ logLevel& level, const GASHA_ logCategory& category)
{
	return const_cast<GASHA_ iConsole*>(const_cast<const logMask*>(this)->console(purpose, level, category));
}
inline GASHA_ iConsole* logMask::console(const logMask::purpose_type purpose, const logMask::level_type level, const logMask::category_type category)
{
	return const_cast<GASHA_ iConsole*>(const_cast<const logMask*>(this)->console(purpose, level, category));
}

//コンソールカラー取得 
inline const GASHA_ consoleColor* logMask::color(const logMask::purpose_type purpose, const GASHA_ logLevel& level, const GASHA_ logCategory& category) const
{
	const GASHA_ iConsole* console_obj = console(purpose, level, category);
	if (!console_obj)
		return nullptr;
	return &level.color(purpose);
}
inline const GASHA_ consoleColor* logMask::color(const logMask::purpose_type purpose, const logMask::level_type level, const logMask::category_type category) const
{
	GASHA_ logLevel level_obj(level);
	GASHA_ logCategory category_obj(category);
	return color(purpose, level_obj, category_obj);
}

//ログレベルマスクのセーブ／ロード

//シリアライズに必要なサイズを取得
inline std::size_t logMask::serializeSize() const
{
	return sizeof(mask_type);
}

//シリアライズ（セーブ用）
inline bool logMask::serialize(void* dst, const std::size_t dst_size) const
{
	if (dst_size < serializeSize())
		return false;
	std::memcpy(dst, m_maskRef, serializeSize());
	return true;
}

//デシリアライズ（ロード用）
inline bool logMask::deserialize(const void* src, const std::size_t src_size)
{
	if (src_size != serializeSize())
		return false;
	std::memcpy(m_maskRef, src, serializeSize());
	return true;
}

//ログレベルマスクをリセット
//※現在参照しているログレベルを初期設定にする
inline void logMask::reset()
{
	reset(m_maskRef);
}

//ムーブオペレータ
inline logMask& logMask::operator=(logMask&& rhs)
{
	m_refType = rhs.m_refType;
	m_maskRef = rhs.m_maskRef;
	m_prevTlsMask = rhs.m_prevTlsMask;//変更前のTLSログレベルマスクはコピーし、
	rhs.m_prevTlsMask = nullptr;      //ムーブ元からは削除（ムーブ元はデストラクタで復元しなくなる）
	rhs.m_refType = isGlobal;         //（同上）
	rhs.m_maskRef = &m_globalMask;    //（同上）
	if (m_refType == isLocal)//ローカルログレベルマスクは、現在の種別がローカルの時だけコピー
	{
		m_localMask = rhs.m_localMask;
		m_maskRef = &m_localMask;
		if (m_tlsMaskRef == &rhs.m_localMask)//末端以外のオブジェクトをムーブすると、ここで不整合を起こすので注意
			m_tlsMaskRef = &m_localMask;
	}
	return *this;
}

//ムーブコンストラクタ
inline logMask::logMask(logMask&& obj) :
	m_refType(obj.m_refType),
	m_maskRef(obj.m_maskRef),
	m_prevTlsMask(obj.m_prevTlsMask)//変更前のTLSログレベルマスクはコピーし、
	                                //ムーブ元からは削除（ムーブ元はデストラクタで復元しなくなる）
{
	obj.m_prevTlsMask = nullptr;  //ムーブ元を無効化
	obj.m_refType = isGlobal;     //（同上）
	obj.m_maskRef = &m_globalMask;//（同上）
	if (m_refType == isLocal)//ローカルログレベルマスクは、現在の種別がローカルの時だけコピー
	{
		m_localMask = obj.m_localMask;
		m_maskRef = &m_localMask;
		if (m_tlsMaskRef == &obj.m_localMask)//末端以外のオブジェクトをムーブすると、ここで不整合を起こすので注意
			m_tlsMaskRef = &m_localMask;
	}
}

//明示的な初期化用コンストラクタ
inline logMask::logMask(const explicitInit_tag&) :
	m_refType(isGlobal),
	m_maskRef(&m_globalMask),
	m_prevTlsMask(nullptr)
{
	std::call_once(m_initialized, initializeOnce);//グローバルログレベルマスク初期化（一回限り）
}

//デストラクタ
inline logMask::~logMask()
{
	if (m_refType == isLocal || m_prevTlsMask)//TLSログレベルマスクを復元
		m_tlsMaskRef = m_prevTlsMask;
}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LOG_MASK_INL

// End of file

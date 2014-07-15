#pragma once
#ifndef GASHA_INCLUDED_LOG_ATTR_H
#define GASHA_INCLUDED_LOG_ATTR_H

//--------------------------------------------------------------------------------
// log_attr.h
// ログ属性【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstdint>//std::uint32_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ属性
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログ属性の定数
enum logAttrEnum : std::uint32_t
{
	noLogAttr = 0x00,//属性なし

	logWithoutCr = 0x01,//改行なし属性
	logWithLevel = 0x02,//ログレベル名を表示
	logWithCategory = 0x04,//ログカテゴリ名を表示
};

//----------------------------------------
//ログ属性
struct logAttr
{
	typedef unsigned char attr_type;//ログ属性の値
	attr_type m_attr;//属性

	//属性判定
	inline bool hasAttr(const attr_type attr) const;

	//属性付与
	inline attr_type addAttr(const attr_type attr);
	
	//属性破棄
	inline attr_type removeAttr(const attr_type attr);

	//クリア
	inline attr_type clear();

	//オペレータ
	inline logAttr& operator=(logAttr&& attr);
	inline logAttr& operator=(const logAttr& attr);
	inline logAttr& operator=(attr_type&& attr);
	inline logAttr& operator=(const attr_type& attr);

	//コンストラクタ
	inline logAttr(logAttr&& attr);
	inline logAttr(const logAttr& attr);
	inline logAttr(attr_type&& attr);
	inline logAttr(const attr_type& attr);

	//デフォルトコンストラクタ
	inline logAttr();

	//デストラクタ
	inline ~logAttr();
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_attr.inl>

#endif//GASHA_INCLUDED_LOG_ATTR_H

// End of file

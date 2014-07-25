#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_VERIOSN_INL
#define GASHA_INCLUDED_SERIALIZATION_VERIOSN_INL

//--------------------------------------------------------------------------------
// serialization/version.inl
// シリアライズ/バージョン【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/version.h>//シリアライズ/バージョン【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/バージョン
	//--------------------------------------------------------------------------------

	//比較
	inline version::compareResult_type version::compare(version& rhs) const
	{
		return	m_major < rhs.m_major ?
					LT :
					m_major > rhs.m_major ?
						GT :
						m_minor < rhs.m_minor ?
							MINOR_LT :
							m_minor > rhs.m_minor ?
								MINOR_GT :
								EQ;
	}
	
	//バージョンからジャーバージョンとマイナーバージョンを算出
	inline void version::calcFromVer() const
	{
		*const_cast<GASHA_ serialization::majorVer_type*>(&m_major) = m_version / GASHA_ serialization::VERSION_FIGURE;
		*const_cast<GASHA_ serialization::minorVer_type*>(&m_minor) = m_version % GASHA_ serialization::VERSION_FIGURE;
	}

	//デフォルトコンストラクタ
	inline version::version() :
		m_major(0),
		m_minor(0),
		m_version(0)
	{}

	//コンストラクタ
	inline version::version(const GASHA_ serialization::majorVer_type major, const minorVer_type minor) :
		m_major(major),
		m_minor(minor),
		m_version(major * GASHA_ serialization::VERSION_FIGURE + minor)
	{}

	//テンプレートコンストラクタ
	template<GASHA_ serialization::majorVer_type _MAJOR, GASHA_ serialization::minorVer_type _MINOR>
	inline version::version(GASHA_ serialization::versionDefBase<_MAJOR, _MINOR>) :
		version(_MAJOR, _MINOR)
	{}

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SERIALIZATION_VERIOSN_INL

// End of file

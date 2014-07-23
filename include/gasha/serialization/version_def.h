#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_VERIOSN_DEF_H
#define GASHA_INCLUDED_SERIALIZATION_VERIOSN_DEF_H

//--------------------------------------------------------------------------------
// serialization/version_def.h
// シリアライズ/バージョン定義【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstdint>//C++11 std::int**_t, std::uint**_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/バージョン定義
	//--------------------------------------------------------------------------------

	//--------------------
	//バージョン用基本型と定数
	
	//型
	typedef std::uint32_t version_type;//合成バージョン型
	typedef std::uint16_t majorVer_type;//メジャーバージョン型
	typedef std::uint16_t minorVer_type;//メジャーバージョン型
	
	//定数
	static const version_type VERSION_FIGURE = 1000000;//合成バージョン計算用桁上げ数

	//--------------------
	//バージョン定義テンプレート基底クラス
	template<majorVer_type _MAJOR, minorVer_type _MINOR>
	class versionDefBase
	{
	public:
		//定数
		static const majorVer_type MAJOR = _MAJOR;//メジャーバージョン
		static const minorVer_type MINOR = _MINOR;//マイナーバージョン
		static const version_type VERSION = MAJOR * VERSION_FIGURE + MINOR;//合成バージョン
	public:
		//アクセッサ
		inline majorVer_type major() const { return MAJOR; };//メジャーバージョン
		inline minorVer_type minor() const { return MINOR; };//マイナーバージョン
		inline version_type value() const { return VERSION; };//合成バージョン
	};
	//--------------------
	//バージョン定義テンプレートクラス
	template<class T>
	class versionDef : public versionDefBase<0, 0>{};//規定では0.0

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

//--------------------
//データクラス用のバージョン定義マクロ
#define GASHA_SERIALIZATION_VERSION_DEF(T, _MAJOR, _MINOR) \
	GASHA_NAMESPACE_BEGIN \
		namespace serialization { \
			template<> \
			class versionDef<T> : public versionDefBase<_MAJOR, _MINOR>{}; \
		} \
	GASHA_NAMESPACE_END

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/serialization/version_def.inl>

#endif//GASHA_INCLUDED_SERIALIZATION_VERIOSN_DEF_H

// End of file

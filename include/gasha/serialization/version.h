#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_VERIOSN_H
#define GASHA_INCLUDED_SERIALIZATION_VERIOSN_H

//--------------------------------------------------------------------------------
// serialization/version.h
// シリアライズ/バージョン【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/version_def.h>//シリアライズ/バージョン定義

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/バージョン
	//--------------------------------------------------------------------------------

	//--------------------
	//バージョンクラス
	class version
	{
	public:
		//定数
		enum compareResult_type : int
		{
			EQ = 0,//==:EQual：同じ
			LT = -1,//<:Less Than：小さい
			GT = 1,//>:Greater Than：大きい
			MINOR_LT = -2,//マイナーバージョンのみ小さい（メジャーバージョンは一致）
			MINOR_GT = 2,//マイナーバージョンのみ大きい（メジャーバージョンは一致）
		};
	public:
		//アクセッサ
		inline GASHA_ serialization::majorVer_type majorVer() const { return m_major; };//メジャーバージョン ※本当は major(), minor() にしたかったが、GCCでマクロ名と競合するので、やむなく変更
		inline GASHA_ serialization::minorVer_type minorVer() const { return m_minor; };//マイナーバージョン
		inline GASHA_ serialization::version_type value()const { return m_version; };//合成バージョン
		inline const version_type* refVersion()const { return &m_version; };//合成バージョンのポインタ
		inline std::size_t size() const { return sizeof(m_version); };//合成バージョンのサイズ
	public:
		//オペレータ
		inline bool operator==(const version& rhs) const { return m_version == rhs.m_version; }
		inline bool operator!=(const version& rhs) const { return m_version != rhs.m_version; }
		inline bool operator<(const version& rhs) const { return m_version < rhs.m_version; }
		inline bool operator<=(const version& rhs) const { return m_version <= rhs.m_version; }
		inline bool operator>(const version& rhs) const { return m_version > rhs.m_version; }
		inline bool operator>=(const version& rhs) const { return m_version >= rhs.m_version; }
	public:
		//キャストオペレータ
		inline operator version_type() const { return m_version; }
	public:
		//メソッド
		//比較
		inline compareResult_type compare(version& rhs) const;
		//バージョンからジャーバージョンとマイナーバージョンを算出
		inline void calcFromVer() const;
	public:
		//デフォルトコンストラクタ
		inline version();
		//コンストラクタ
		inline version(const GASHA_ serialization::majorVer_type major, const GASHA_ serialization::minorVer_type minor);
		//テンプレートコンストラクタ
		template<GASHA_ serialization::majorVer_type _MAJOR, GASHA_ serialization::minorVer_type _MINOR>
		inline version(GASHA_ serialization::versionDefBase<_MAJOR, _MINOR>);
	private:
		//フィールド
		const GASHA_ serialization::majorVer_type m_major;//メジャーバージョン
		const GASHA_ serialization::minorVer_type m_minor;//マイナーバージョン
		const GASHA_ serialization::version_type m_version;//バージョン
	};

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/serialization/version.inl>

#endif//GASHA_INCLUDED_SERIALIZATION_VERIOSN_H

// End of file

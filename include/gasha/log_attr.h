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

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <mutex> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <mutex>//C++11 std::call_once

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ属性
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログ属性の定数
enum logAttrEnum : std::uint32_t
{
	noLogAttr = 0x0000,//属性なし

	//ログ系属性
	logWithoutCr = 0x0001,//改行なし属性 ※デフォルトは改行する
	logWithID = 0x0002,//メッセージヘッダー：IDを付加
	logWithLevel = 0x0004,//メッセージヘッダー：ログレベル名を付加
	logWithCategory = 0x0008,//メッセージヘッダ：ログカテゴリ名を付加
	logWithCPName = 0x0010,//メッセージヘッダ：直近のコールポイント名を付加
	logWithCriticalCPName = 0x0020,//メッセージヘッダ：直近のクリティカルコールポイント名を付加
	logHeaderOnlyColored = 0x0040,//カラーの反映をヘッダーのみにする

	//画面通知系属性
	bitShiftOfNotice = 16,//画面通知系属性のためのビットシフト数
	noticeWithCr = 0x0001 << bitShiftOfNotice,//改行付き属性 ※デフォルトは改行しない（デフォルトの挙動がログ系と異なる点に注意）
	noticeWithID = logWithID << bitShiftOfNotice,//メッセージヘッダー：IDを付加
	noticeWithLevel = logWithLevel << bitShiftOfNotice,//メッセージヘッダー：ログレベル名を付加
	noticeWithCategory = logWithCategory << bitShiftOfNotice,//メッセージヘッダ：ログカテゴリ名を付加
	noticeWithCPName = logWithCPName << bitShiftOfNotice,//メッセージヘッダ：直近のコールポイント名を付加
	noticeWithCriticalCPName = logWithCriticalCPName << bitShiftOfNotice,//メッセージヘッダ：直近のクリティカルコールポイント名を付加
	noticeHeaderOnlyColored = logHeaderOnlyColored << bitShiftOfNotice,//カラーの反映をヘッダーのみにする
};

//----------------------------------------
//ログ属性
class logAttr
{
public:
	//型
	typedef std::uint32_t attr_type;//ログ属性の値
	struct explicitInitialize_t{};//明示的な初期化用構造体
public:
	//定数
	static const attr_type DEFAULT_ATTR = GASHA_DEFAULT_LOG_ATTR;//デフォルト属性
	enum ref_type//ログ属性参照種別
	{
		isGlobal = 0,//グローバルログ属性
		isTls,//TLSログ属性（他のローカルログ属性の参照）
		isLocal,//ローカルログ属性
	};

public:
	//キャストオペレータ
	inline operator int() const { return static_cast<int>(*m_attrRef); }//ログ属性の値
	inline operator attr_type() const { return *m_attrRef; }//ログ属性の値

public:
	//アクセッサ
	inline ref_type refType() const { return m_refType; }//参照しているログ属性の種別
	inline const attr_type& attr() const { return *m_attrRef; }//現在参照しているログ属性
public:
	//メソッド
	inline static bool has(const attr_type target_attr, const attr_type attr);//属性判定（静的メソッド）
	inline bool has(const attr_type attr) const;//属性判定
	inline attr_type add(const attr_type attr);//属性付与
	inline attr_type remove(const attr_type attr);//属性破棄
	inline attr_type set(const attr_type attr);//属性変更
	inline attr_type reset();//属性リセット
	
	//参照するログ属性を変更
	//※ローカルログ属性に関する操作
	//　ローカルログ属性に変更ると、その時点のグローバルログレベルマスクもしくは
	//　TLSログ属性をコピーする。さらに、TLSログ属性に適用する。
	//※ローカルログ属性がTLSログ属性に適用されると、以降の処理にローカルログ属性が反映される。
	//※【注意】ローカルの状態で再度ローカルを指定すると、改めて元の値をコピーし直すことに注意。
	void changeRef(const ref_type type);

private:
	//初期化メソッド（一回限り）
	static void initializeOnce();

public:
	//ムーブオペレータ
	inline logAttr& operator=(logAttr&& attr);
	//コピーオペレータ
	inline logAttr& operator=(const logAttr& attr) = delete;

public:
	//ムーブコンストラクタ
	inline logAttr(logAttr&& attr);
	//コピーコンストラクタ
	inline logAttr(const logAttr& attr) = delete;
	//明示的な初期化用コンストラクタ
	inline logAttr(const explicitInitialize_t&);
	//デフォルトコンストラクタ
	logAttr();
	//デストラクタ
	inline ~logAttr();

public:
	//静的フィールド
	static const explicitInitialize_t explicitInitialize;//明示的な初期化指定用
private:
	//フィールド
	ref_type m_refType;//ログ属性種別
	attr_type* m_attrRef;//現在参照しているログ属性
	attr_type* m_prevTlsAttr;//変更前のTLSログ属性（ローカルログ属性作成時に記録し、デストラクタで元に戻す）
	attr_type m_localAttr;//ローカルログ属性
private:
	//静的フィールド
	static std::once_flag m_initialized;//グローバルログ属性初期化済み
	static attr_type m_globalAttr;//グローバルログ属性
private:
	//TLS静的フィールド
	static thread_local attr_type* m_tlsAttrRef;//TLSログ属性（ローカルログ属性の参照）
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_attr.inl>

#endif//GASHA_INCLUDED_LOG_ATTR_H

// End of file

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

#include <gasha/log_purpose.h>//ログ用途

#include <cstdint>//C++11 std::uint32_t

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <mutex>//C++11 std::call_once
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ属性
//--------------------------------------------------------------------------------

//----------------------------------------
//ログ属性の定数
enum logAttrConst : std::uint32_t
{
	noLogAttr = 0x00000000,//属性なし

	//基本属性
	//reserved1 = 0x0001,//（予約１）
	//reserved2 = 0x0001,//（予約２）
	//reserved3 = 0x0001,//（予約３）
	//reserved4 = 0x0001,//（予約４）
	//reserved5 = 0x0001,//（予約５）
	//reserved6 = 0x0001,//（予約６）
	//reserved7 = 0x0001,//（予約７）
	//reserved8 = 0x0001,//（予約８）

	//共通属性（全12bit）
	//※ビットシフトしてログ系と画面通知系の両方で使用する
	logPurposeWithID = 0x0002,//メッセージヘッダー：IDを付加
	logPurposeWithTime = 0x0004,//メッセージヘッダー：時間を付加
	logPurposeWithLevel = 0x0008,//メッセージヘッダー：ログレベル名を付加
	logPurposeWithCategory = 0x0010,//メッセージヘッダ：ログカテゴリ名を付加
	//※ログキューを通すと正しいコールポイントを得られない
	//logPurposeWithCPName = 0x0020,//メッセージヘッダ：直近のコールポイント名を付加
	//logPurposeWithCriticalCPName = 0x0040,//メッセージヘッダ：直近のクリティカルコールポイント名を付加
	//logPurposeWithAnyHeader = logPurposeWithID | logPurposeWithTime | logPurposeWithLevel | logPurposeWithCategory | logPurposeWithCPName | logPurposeWithCriticalCPName,//メッセージヘッダー：全ビット
	logPurposeWithAnyHeader = logPurposeWithID | logPurposeWithTime | logPurposeWithLevel | logPurposeWithCategory,//メッセージヘッダー：全ビット
	logPurposeHeaderOnlyColored = 0x0080,//カラーの反映をヘッダーのみにする
	logPurposeWithoutColor = 0x0100,//カラー表示なし ※logPurposeHeaderOnlyColored より優先
	//reserved1 = 0x0200,//（予約１）
	//reserved2 = 0x0400,//（予約２）
	//reserved3 = 0x0800,//（予約３）

	//ログ系属性
	logShiftBits = 8,//ログ系属性のためのビットシフト数
	logWithoutCr = 0x0001 << logShiftBits,//改行なし属性 ※デフォルトは改行する
	logWithID = logPurposeWithID << logShiftBits,//メッセージヘッダー：IDを付加
	logWithTime = logPurposeWithTime << logShiftBits,//メッセージヘッダー：時間を付加
	logWithLevel = logPurposeWithLevel << logShiftBits,//メッセージヘッダー：ログレベル名を付加
	logWithCategory = logPurposeWithCategory << logShiftBits,//メッセージヘッダ：ログカテゴリ名を付加
	//logWithCPName = logPurposeWithCPName << logShiftBits,//メッセージヘッダ：直近のコールポイント名を付加
	//logWithCriticalCPName = logPurposeWithCriticalCPName << logShiftBits,//メッセージヘッダ：直近のクリティカルコールポイント名を付加
	logWithAnyHeader = logPurposeWithAnyHeader << logShiftBits,//メッセージヘッダー：全ビット
	logHeaderOnlyColored = logPurposeHeaderOnlyColored << logShiftBits,//カラーの反映をヘッダーのみにする
	logWithoutColor = logPurposeWithoutColor << logShiftBits,//カラー表示なし ※logPurposeHeaderOnlyColored より優先

	//画面通知系属性
	noticeShiftBits = 8 + 12,//画面通知系属性のためのビットシフト数
	noticeWithCr = 0x0001 << noticeShiftBits,//改行付き属性 ※デフォルトは改行しない（通常の画面通知処理は、end() 時に改行処理（と同等の処理）が行われる）
	noticeWithID = logPurposeWithID << noticeShiftBits,//メッセージヘッダー：IDを付加
	noticeWithTime = logPurposeWithTime << noticeShiftBits,//メッセージヘッダー：時間を付加
	noticeWithLevel = logPurposeWithLevel << noticeShiftBits,//メッセージヘッダー：ログレベル名を付加
	noticeWithCategory = logPurposeWithCategory << noticeShiftBits,//メッセージヘッダ：ログカテゴリ名を付加
	//noticeWithCPName = logPurposeWithCPName << noticeShiftBits,//メッセージヘッダ：直近のコールポイント名を付加
	//noticeWithCriticalCPName = logPurposeWithCriticalCPName << noticeShiftBits,//メッセージヘッダ：直近のクリティカルコールポイント名を付加
	noticeWithAnyHeader = logPurposeWithAnyHeader << noticeShiftBits,//メッセージヘッダー：全ビット
	noticeHeaderOnlyColored = logPurposeHeaderOnlyColored << noticeShiftBits,//カラーの反映をヘッダーのみにする
	noticeWithoutColor = logPurposeWithoutColor << noticeShiftBits,//カラー表示なし ※logPurposeHeaderOnlyColored より優先
};

//----------------------------------------
//ログ属性
class logAttr
{
public:
	//型
	typedef std::uint32_t attr_value_type;//ログ属性の値
	typedef GASHA_ logPurpose::purpose_type purpose_type;//ログ用途の値
	struct explicitInit_type{};//明示的な初期化用構造体
public:
	//定数
	static const attr_value_type DEFAULT_ATTR = GASHA_DEFAULT_LOG_ATTR;//デフォルト属性
	enum ref_type//ログ属性参照種別
	{
		isGlobal = 0,//グローバルログ属性
		isTls,//TLSログ属性（他のローカルログ属性の参照）
		isLocal,//ローカルログ属性
	};
public:
	//ログ属性型
	struct attr_type
	{
	#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
		inline operator attr_value_type() const { return m_attr; }
		inline operator attr_value_type(){ return m_attr; }
		inline attr_value_type operator*() const { return m_attr; }
		inline attr_value_type operator*(){ return m_attr; }
		inline attr_type(const attr_value_type attr):m_attr(attr){}//コンストラクタ
		inline attr_type(){}//コンストラクタ
		attr_value_type m_attr;//ログ属性
	#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
		inline operator attr_value_type() const { return 0; }
		inline operator attr_value_type(){ return 0; }
		inline attr_value_type operator*() const { return 0; }
		inline attr_value_type operator*(){ return 0; }
		inline attr_type(const attr_value_type attr){}//コンストラクタ
		inline attr_type(){}//コンストラクタ
	#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
	};

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

public:
	//基本オペレータ
	inline const attr_type* operator->() const { return ref(); }//現在参照しているログ属性
	inline attr_type* operator->(){ return ref(); }//現在参照しているログ属性
	inline const attr_type& operator*() const { return attr(); }//現在参照しているログ属性
	inline attr_type& operator*(){ return attr(); }//現在参照しているログ属性
public:
	//キャストオペレータ
	inline operator attr_value_type() const { return attrValue(); }//現在参照しているログ属性
	inline operator const attr_type&() const { return attr(); }//現在参照しているログ属性
	inline operator attr_type&(){ return attr(); }//現在参照しているログ属性
	inline operator const attr_type*() const { return ref(); }//現在参照しているログ属性
	inline operator attr_type*(){ return ref(); }//現在参照しているログ属性

public:
	//アクセッサ
	inline ref_type refType() const { return m_refType; }//現在参照しているログ属性の種別
	inline const attr_type* ref() const { return m_attrRef; }//現在参照しているログ属性
	inline attr_type* ref(){ return m_attrRef; }//現在参照しているログ属性
	inline const attr_type& attr() const { return *m_attrRef; }//現在参照しているログ属性
	inline attr_type& attr(){ return *m_attrRef; }//現在参照しているログ属性
	inline attr_value_type attrValue() const { return m_attrRef->m_attr; }//現在参照しているログ属性
	inline static int shiftBits(const purpose_type purpose);//ログ用途に応じたビットシフト数

public:
	//メソッド
	inline static bool has(const attr_value_type target_attr, const attr_value_type attr);//属性判定（静的メソッド）
	inline static bool has(const attr_value_type target_attr, const purpose_type purpose, const attr_value_type attr);//属性判定（静的メソッド）※用途別の判定
	inline bool has(const attr_value_type attr) const;//属性判定
	inline bool has(const purpose_type purpose, const attr_value_type attr) const;//属性判定
	inline attr_type add(const attr_value_type attr);//属性付与
	inline attr_type add(const purpose_type purpose, const attr_value_type attr);//属性付与
	inline attr_type remove(const attr_value_type attr);//属性破棄
	inline attr_type remove(const purpose_type purpose, const attr_value_type attr);//属性破棄
	inline attr_type get() const;//属性取得 ※attr() と同じ
	inline attr_type set(const attr_value_type attr);//属性変更
	inline attr_type reset();//属性リセット
	
	//参照するログ属性を変更
	//※ローカルログ属性に関する操作
	//　ローカルログ属性に変更ると、その時点のグローバルログレベルマスクもしくは
	//　TLSログ属性をコピーする。さらに、TLSログ属性に適用する。
	//※ローカルログ属性がTLSログ属性に適用されると、以降の処理にローカルログ属性が反映される。
	//※【注意】ローカルの状態で再度ローカルを指定すると、改めて元の値をコピーし直すことに注意。
	void changeRef(const ref_type type);

	//ログ属性のセーブ／ロード
	//※【注意】現在参照しているログ属性に対して操作する点に注意
	inline std::size_t serializeSize() const;//シリアライズに必要なサイズを取得
	inline bool serialize(void* dst, const std::size_t dst_size) const;//シリアライズ（セーブ用）※dst に出力
	inline bool deserialize(const void* src, const std::size_t dst_size);//デシリアライズ（ロード用）※src から復元

private:
	//初期化メソッド（一回限り）
	static void initializeOnce();

public:
	//ムーブオペレータ
	//※【注意】末端以外のオブジェクトをムーブすると、TLSに不整合が生じるので注意
	inline logAttr& operator=(logAttr&& rhs);
	//コピーオペレータ
	inline logAttr& operator=(const logAttr& rhs) = delete;

public:
	//ムーブコンストラクタ
	//※【注意】末端以外のオブジェクトをムーブすると、TLSに不整合が生じるので注意
	inline logAttr(logAttr&& obj);
	//コピーコンストラクタ
	inline logAttr(const logAttr& obj) = delete;
	//明示的な初期化用コンストラクタ
	inline logAttr(const explicitInit_type&);
	//デフォルトコンストラクタ
	logAttr();
	//デストラクタ
	inline ~logAttr();

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

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

public:
	//基本オペレータ
	inline const attr_type* operator->() const { return ref(); }//現在参照しているログ属性
	inline attr_type* operator->(){ return ref(); }//現在参照しているログ属性
	inline const attr_type& operator*() const { return attr(); }//現在参照しているログ属性
	inline attr_type& operator*(){ return attr(); }//現在参照しているログ属性
public:
	//キャストオペレータ
	inline operator attr_value_type() const { return attrValue(); }//現在参照しているログ属性
	inline operator const attr_type&() const { return attr(); }//現在参照しているログ属性
	inline operator attr_type&(){ return attr(); }//現在参照しているログ属性
	inline operator const attr_type*() const { return ref(); }//現在参照しているログ属性
	inline operator attr_type*(){ return ref(); }//現在参照しているログ属性
public:
	//アクセッサ
	inline ref_type refType() const { return isGlobal; }//現在参照しているログ属性の種別
	inline const attr_type* ref() const { return &m_attr; }//現在参照しているログ属性
	inline attr_type* ref(){ return &m_attr; }//現在参照しているログ属性
	inline const attr_type& attr() const { return m_attr; }//現在参照しているログ属性
	inline attr_type& attr(){ return m_attr; }//現在参照しているログ属性
	inline attr_value_type attrValue() const { return 0; }//現在参照しているログ属性
	inline static int shiftBits(const purpose_type purpose){ return 0; };//ログ用途に応じたビットシフト数
public:
	//メソッド
	inline static bool has(const attr_value_type target_attr, const attr_value_type attr){ return false; }//属性判定（静的メソッド）
	inline static bool has(const attr_value_type target_attr, const purpose_type purpose, const attr_value_type attr){ return false; }//属性判定（静的メソッド）※用途別の判定
	inline bool has(const attr_value_type attr) const{ return false; }//属性判定
	inline bool has(const purpose_type purpose, const attr_value_type attr) const{ return false; }//属性判定
	inline attr_type add(const attr_value_type attr){ return attr_type(); }//属性付与
	inline attr_type add(const purpose_type purpose, const attr_value_type attr){ return attr_type(); }//属性付与
	inline attr_type remove(const attr_value_type attr){ return attr_type(); }//属性破棄
	inline attr_type remove(const purpose_type purpose, const attr_value_type attr){ return attr_type(); }//属性破棄
	inline attr_type get() const{ return attr_type(); }//属性取得 ※attr() と同じ
	inline attr_type set(const attr_value_type attr){ return attr_type(); }//属性変更
	inline attr_type reset(){ return attr_type(); }//属性リセット
	inline void changeRef(const ref_type type){}//参照するログ属性を変更
	inline std::size_t serializeSize() const{ return 0; }//シリアライズに必要なサイズを取得
	inline bool serialize(void* dst, const std::size_t dst_size) const{ return true; }//シリアライズ（セーブ用）※dst に出力
	inline bool deserialize(const void* src, const std::size_t dst_size){ return true; }//デシリアライズ（ロード用）※src から復元
public:
	inline logAttr& operator=(logAttr&& rhs){ return *this; }//ムーブオペレータ
	inline logAttr& operator=(const logAttr& rhs) = delete;//コピーオペレータ
public:
	inline logAttr(logAttr&& obj){}//ムーブコンストラクタ
	inline logAttr(const logAttr& obj) = delete;//コピーコンストラクタ
	inline logAttr(const explicitInit_type&){}//明示的な初期化用コンストラクタ
	inline logAttr(){}//デフォルトコンストラクタ
	inline ~logAttr(){}//デストラクタ
private:
	//フィールド
	attr_type m_attr;//ダミー

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

public:
	//静的フィールド
	static const explicitInit_type explicitInit;//明示的な初期化指定用
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_attr.inl>

#endif//GASHA_INCLUDED_LOG_ATTR_H

// End of file

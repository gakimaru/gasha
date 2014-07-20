#pragma once
#ifndef GASHA_INCLUDED_CALL_POINT_H
#define GASHA_INCLUDED_CALL_POINT_H

//--------------------------------------------------------------------------------
// call_point.h
// コールポイント【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_category.h>//ログカテゴリ

#include <gasha/chrono.h>//時間処理ユーティリティ：sec_t, elapsedTime

#include <cstddef>//std::size_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//クラス宣言
class criticalCallPoint;

//----------------------------------------
//コールポイント
class callPoint
{
public:
	//型
	typedef GASHA_ logCategory::category_type category_type;//ログカテゴリの値
public:
	//定数
	enum callpoint_type : unsigned char//コールポイント種別
	{
		isReference = 0,//コールポイント参照用
		isNormal,//通常コールポイント
		isCritical,//重大コールポイント
	};
	enum autoProfiling_type : unsigned char//自動プロファイリング種別
	{
		useAutoProfiling = 0,//自動プロファイリング使用
		noProfiling,//自動プロファイリングなし
	};

#ifdef GASHA_CALLPOINT_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//アクセッサ
	inline callpoint_type type() const;//コールポイント種別
	inline category_type category() const;//カテゴリ
	inline const char* name() const;//コールポイント名
	inline GASHA_ sec_t beginTime() const;//コールポイント開始時間
	inline const GASHA_ elapsedTime elapsedTime() const;//経過時間
	inline const char* fileName() const;//呼び出し元ソースファイル名
	inline const char* funcName() const;//呼び出し元関数名
	inline autoProfiling_type autoProfiling() const;//自動プロファイリング種別
	inline callPoint* prevCallStack() const;//コールスタック（前のコールポイント）

public:
	//メソッド
	inline const callPoint* find() const;//直近のコールポイントを検索
	const criticalCallPoint* findCritical() const;//直近の重大コールポイントを検索
	inline void setAutoProfiling(const bool enabled);//自動プロファイリングを有効化／無効化
	void profiling();//プロファイリングする ※現時点の経過時間をプロファイラーに記録する
	//デバッグ情報作成
	std::size_t debugInfo(char* message, const std::size_t max_size) const;

public:
	//ムーブオペレータ
	//※【注意】末端以外のオブジェクトをムーブすると、TLSに不整合が生じるので注意
	inline callPoint& operator=(callPoint&& rhs);
	inline callPoint& operator=(criticalCallPoint&& rhs) = delete;
	//コピーオペレータ
	inline callPoint& operator=(const callPoint& rhs) = delete;
	inline callPoint& operator=(const criticalCallPoint& rhs) = delete;

public:
	//ムーブコンストラクタ
	//※【注意】末端以外のオブジェクトをムーブすると、TLSに不整合が生じるので注意
	inline callPoint(callPoint&& obj);
	inline callPoint(criticalCallPoint&& obj) = delete;
	//コピーコンストラクタ
	inline callPoint(const callPoint& obj) = delete;
	inline callPoint(const criticalCallPoint& obj) = delete;
	//コンストラクタ
	inline callPoint(const category_type category, const char* name, const char* file_name, const char* func_name);
	inline callPoint(const category_type category, const char* name, const autoProfiling_type auto_profiling, const char* file_name, const char* func_name);
protected:
	inline callPoint(const callpoint_type type, const category_type category, const char* name, const autoProfiling_type auto_profiling, const char* file_name, const char* func_name);
public:
	//デフォルトコンストラクタ
	//※コールポイントは作らず、コールポイントスタックなどの情報を確認するために使用
	inline callPoint();
	//デストラクタ
	inline ~callPoint();

private:
	//フィールド
	callPoint* m_prevCallStack;//コールスタック連結
	const char* m_name;//コールポイント名
	const char* m_fileName;//呼び出し元ソースファイル名
	const char* m_funcName;//呼び出し元関数名
	GASHA_ sec_t m_beginTime;//コールポイント開始時間
	GASHA_ elapsedTime m_elapsedTime;//経過時間
	category_type m_category;//カテゴリ
	callpoint_type m_type;//コールポイント種別
	autoProfiling_type m_autoProfiling;//自動プロファイリング種別
private:
	//TLS静的フィールド
	static thread_local callPoint* m_callStackTop;//コールスタックの先頭

#else//GASHA_CALLPOINT_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//アクセッサ
	inline callpoint_type type() const{ return isReference; }//コールポイント種別
	inline category_type category() const{ return 0; }//カテゴリ
	inline const char* name() const{ return ""; }//コールポイント名
	inline GASHA_ sec_t beginTime() const{ return static_cast<GASHA_ sec_t>(0.); }//コールポイント開始時間
	inline const GASHA_ elapsedTime elapsedTime() const{ return GASHA_ elapsedTime(); }//経過時間
	inline const char* fileName() const{ return ""; }//呼び出し元ソースファイル名
	inline const char* funcName() const{ return ""; }//呼び出し元関数名
	inline autoProfiling_type autoProfiling() const{ return noProfiling; }//自動プロファイリング種別
	inline callPoint* prevCallStack() const{ return nullptr; }//コールスタック（前のコールポイント）
public:
	//メソッド
	inline const callPoint* find() const{ return nullptr; }//直近のコールポイントを検索
	inline const criticalCallPoint* findCritical() const{ return nullptr; }//直近の重大コールポイントを検索
	inline void setAutoProfiling(const bool enabled){}//自動プロファイリングを有効化／無効化
	inline void profiling(){}//プロファイリングする
	inline std::size_t debugInfo(char* message, const std::size_t max_size) const{ return 0; }//デバッグ情報作成
public:
	inline callPoint& operator=(callPoint&& rhs){ return *this; }//ムーブオペレータ
	inline callPoint& operator=(criticalCallPoint&& rhs) = delete;//ムーブオペレータ
	inline callPoint& operator=(const callPoint& rhs) = delete;//コピーオペレータ
	inline callPoint& operator=(const criticalCallPoint& rhs) = delete;//コピーオペレータ
public:
	inline callPoint(callPoint&& obj){}//ムーブコンストラクタ
	inline callPoint(criticalCallPoint&& obj) = delete;//ムーブコンストラクタ
	inline callPoint(const callPoint& obj) = delete;//コピーコンストラクタ
	inline callPoint(const criticalCallPoint& obj) = delete;//コピーコンストラクタ
	inline callPoint(const category_type category, const char* name, const char* file_name, const char* func_name){}//コンストラクタ
	inline callPoint(const category_type category, const char* name, const autoProfiling_type auto_profiling, const char* file_name, const char* func_name){}//コンストラクタ
public:
	inline callPoint(){}//デフォルトコンストラクタ
	inline ~callPoint(){}//デストラクタ

#endif//GASHA_CALLPOINT_IS_ENABLED//デバッグログ無効時はまるごと無効化
};

//----------------------------------------
//重大コールポイント
class criticalCallPoint: public callPoint
{
#ifdef GASHA_CALLPOINT_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//ムーブオペレータ
	//※【注意】末端以外のオブジェクトをムーブすると、TLSに不整合が生じるので注意
	inline criticalCallPoint& operator=(callPoint&& rhs) = delete;
	inline criticalCallPoint& operator=(criticalCallPoint&& rhs);
	//コピーオペレータ
	inline criticalCallPoint& operator=(const callPoint& rhs) = delete;
	inline criticalCallPoint& operator=(const criticalCallPoint& rhs) = delete;

public:
	//ムーブコンストラクタ
	//※【注意】末端以外のオブジェクトをムーブすると、TLSに不整合が生じるので注意
	inline criticalCallPoint(callPoint&& obj) = delete;
	inline criticalCallPoint(criticalCallPoint&& obj);
	//コピーコンストラクタ
	inline criticalCallPoint(const callPoint& obj) = delete;
	inline criticalCallPoint(const criticalCallPoint& obj) = delete;
	//コンストラクタ
	inline criticalCallPoint(const category_type category, const char* name, const char* file_name, const char* func_name);
	inline criticalCallPoint(const category_type category, const char* name, const autoProfiling_type auto_profiling, const char* file_name, const char* func_name);
	//デフォルトコンスタラクタ
	inline criticalCallPoint() = delete;
	//デストラクタ
	inline ~criticalCallPoint();

#else//GASHA_CALLPOINT_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	inline criticalCallPoint& operator=(callPoint&& rhs) = delete;//ムーブオペレータ
	inline criticalCallPoint& operator=(criticalCallPoint&& rhs){ return *this; }//ムーブオペレータ
	inline criticalCallPoint& operator=(const callPoint& rhs) = delete;//コピーオペレータ
	inline criticalCallPoint& operator=(const criticalCallPoint& rhs) = delete;//コピーオペレータ
public:
	inline criticalCallPoint(callPoint&& obj) = delete;//ムーブコンストラクタ
	inline criticalCallPoint(criticalCallPoint&& obj){}//ムーブコンストラクタ
	inline criticalCallPoint(const callPoint& obj) = delete;//コピーコンストラクタ
	inline criticalCallPoint(const criticalCallPoint& obj) = delete;//コピーコンストラクタ
	inline criticalCallPoint(const category_type category, const char* name, const char* file_name, const char* func_name){}//コンストラクタ
	inline criticalCallPoint(const category_type category, const char* name, const autoProfiling_type auto_profiling, const char* file_name, const char* func_name){}//コンストラクタ
	inline criticalCallPoint() = delete;//デフォルトコンスタラクタ
	inline ~criticalCallPoint(){}//デストラクタ

#endif//GASHA_CALLPOINT_IS_ENABLED//デバッグログ無効時はまるごと無効化
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/call_point.inl>

//コールポイントのコンストラクタ引数用マクロ
//※ソースファイル名と関数名
#ifdef GASHA_CALLPOINT_IS_ENABLED//デバッグログ無効時は無効化

	#define GASHA_CP_ARGS GASHA_FILE_AND_FUNC//コールポイントのコンストラクタ引数用マクロ
	#define GASHA_CP(var, category, name) GASHA_ callPoint var(category, name, GASHA_CP_ARGS)//コールポイント設定用マクロ
	#define GASHA_CP_AUTOPRO(var, category, name) GASHA_ callPoint var(category, name, GASHA_ callPoint::useAutoProfiling, GASHA_CP_ARGS)//コールポイント設定用マクロ ※自動プロファイリング付き
	#define GASHA_CRITICAL_CP(var, category, name) GASHA_ criticalCallPoint var(category, name, GASHA_CP_ARGS)//重大コールポイント設定用マクロ
	#define GASHA_CRITICAL_CP_AUTOPRO(var, category, name) GASHA_ criticalCallPoint var(category, name, GASHA_ callPoint::useAutoProfiling, GASHA_CP_ARGS)//重大コールポイント設定用マクロ ※自動プロファイリング付き

#else//GASHA_CALLPOINT_IS_ENABLED

	#define GASHA_CP_ARGS nullptr, nullptr//コールポイントのコンストラクタ引数用マクロ
	#define GASHA_CP(var, category, name) GASHA_ callPoint var(category, name, GASHA_CP_ARGS)//コールポイント設定用マクロ
	#define GASHA_CP_AUTOPRO(var, category, name) GASHA_ callPoint var(category, name, GASHA_CP_ARGS)//コールポイント設定用マクロ ※自動プロファイリング付き
	#define GASHA_CRITICAL_CP(var, category, name) GASHA_ criticalCallPoint var(category, name, GASHA_CP_ARGS)//重大コールポイント設定用マクロ
	#define GASHA_CRITICAL_CP_AUTOPRO(var, category, name) GASHA_ criticalCallPoint var(category, name, GASHA_CP_ARGS)//重大コールポイント設定用マクロ ※自動プロファイリング付き

#endif//GASHA_CALLPOINT_IS_ENABLED

#endif//GASHA_INCLUDED_CALL_POINT_H

// End of file

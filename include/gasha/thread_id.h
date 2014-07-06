#pragma once
#ifndef GASHA_INCLUDED_THREAD_ID_H
#define GASHA_INCLUDED_THREAD_ID_H

//--------------------------------------------------------------------------------
// thread_id.h
// スレッドID【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/limits.h>//限界値

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <thread> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <thread>//C++11 std::thread::id

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//スレッドID
//--------------------------------------------------------------------------------

//----------------------------------------
//スレッドIDクラス
//※現在のスレッドのスレッドID を std::this_thread::get_id() から取得していると遅いため、それを高速化するためのクラス。
//※std::hash<std::thread::id> をTLSに記憶し、静的メンバー関数で現在のスレッドのIDを高速に取得可能。
//※同時にスレッド名も扱う。
//※スレッドIDをメンバーに保持し、比較・コピーに対応。
class threadId
{
public:
	//型
	typedef std::hash<std::thread::id>::result_type id_t;//ID型
public:
	//定数
	static const id_t INVALID_ID = GASHA_ numeric_limits<id_t>::MAX;//無向なID
public:
	//アクセッサ
	inline id_t id() const { return *m_id; }//スレッドIDを取得
	inline const char* name() const { return *m_name; }//スレッド名を取得
	inline bool isThisThread() const { return *m_id == m_thisId; }//現在のスレッドと同じスレッドか判定
public:
	//静的フィールドのアクセッサ
	inline id_t thisId() const { return m_thisId; }//現在のスレッドのスレッドIDを取得
	inline const char* thisName() const { return m_thisName; }//現在のスレッドのスレッド名を取得
public:
	//比較オペレータ
	inline bool operator==(const threadId& rhs) const { return id() == rhs.id(); }
	inline bool operator!=(const threadId& rhs) const { return id() != rhs.id(); }
public:
	//キャストオペレータ
	inline operator id_t() const { return *m_id; }//スレッドID
	inline operator const char*() const { return *m_name; }//スレッド名
public:
	//基本オペレータ
	inline id_t operator*() const { return *m_id; }//スレッドID
public:
	//メソッド
	//現在のスレッドのスレッドIDをセット
	void setThisId() const;
	//現在のスレッドのスレッド名をセット
	void setThisName(const char* name) const;
public:
	//ムーブオペレータ
	inline threadId& operator=(const threadId&& rhs);//※定義しない（暗黙的なコンストラクタに任せる）
	//コピーオペレータ
	inline threadId& operator=(const threadId& rhs);//※定義しない（暗黙的なコンストラクタに任せる）
public:
	//ムーブコンストラクタ
	inline threadId(const threadId&& obj);
	//コピーコンストラクタ
	inline threadId(const threadId& obj);
	//コンストラクタ
	//※現在のスレッドのスレッド名をセットしつつ、現在のスレッドIDを保持
	inline threadId(const char* name);
	//デフォルトコンストラクタ
	//※現在のスレッドIDを保持
	inline threadId();
	//デストラクタ
	inline ~threadId();
private:
	//フィールド
	id_t* m_id;//スレッドID
	const char** m_name;//スレッド名
private:
	//静的フィールド
	static thread_local bool m_thisIdIsInitialized;//現在のスレッドのスレッドIDセット済みフラグ
	static thread_local id_t m_thisId;//現在のスレッドのスレッドID
	static thread_local const char* m_thisName;//現在のスレッドのスレッド名
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/thread_id.inl>

#endif//GASHA_INCLUDED_THREAD_ID_H

// End of file

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

#include <gasha/crc32.h>//CRC32
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

//--------------------------------------------------------------------------------
//【注意】対策済みだが、TLSが初期化できない環境（0初期化しかできない環境）には注意。
//        ⇒懸念点：m_thisId の初期値が 0 ではない。
//--------------------------------------------------------------------------------

//----------------------------------------
//スレッドIDクラス
//※現在のスレッドのスレッドID を std::this_thread::get_id() から取得していると遅いため、それを高速化するためのクラス。
//※std::hash<std::thread::id> をTLSに記憶し、静的メンバー関数で現在のスレッドのIDを高速に取得可能。
//※同時にスレッド名とそのCRCも扱う。
//※スレッドIDをメンバーに保持し、比較・コピーに対応。
//※スレッド名のCRCは、スレッドのパフォーマンス統計などに用いることを想定。
//　例えば、同じスレッドを何度も生成・破棄するようなケースでは、スレッドIDが毎回変わるので、
//　同じスレッドとして集計するようなことができないが、名前（CRC）に基づけば集計することができる。
class threadId
{
public:
	//型
	typedef std::hash<std::thread::id>::result_type id_type;//ID型
public:
	//定数
	static const id_type INITIAL_ID = GASHA_ numeric_limits<id_type>::MAX;//初期ID
public:
	//アクセッサ
	inline id_type id() const { return *m_id; }//スレッドIDを取得
	inline const char* name() const { return *m_name; }//スレッド名を取得
	inline GASHA_ crc32_t nameCrc() const { return m_nameCrc; }//スレッド名のCRCを取得
	inline bool isThisThread() const { return *m_id == m_thisId; }//現在のスレッドと同じスレッドか判定
public:
	//静的フィールドのアクセッサ
	inline id_type thisId() const { return m_thisId; }//現在のスレッドのスレッドIDを取得
	inline const char* thisName() const { return m_thisName; }//現在のスレッドのスレッド名を取得
	inline GASHA_ crc32_t thisNameCrc() const { return m_thisNameCrc; }//現在のスレッドのスレッド名のCRCを取得
public:
	//比較オペレータ
	inline bool operator==(const threadId& rhs) const { return id() == rhs.id(); }
	inline bool operator!=(const threadId& rhs) const { return id() != rhs.id(); }
public:
	//キャストオペレータ
	inline operator id_type() const { return *m_id; }//スレッドID
	inline operator const char*() const { return *m_name; }//スレッド名
	//inline operator GASHA_ crc32_t() const { return m_nameCrc; }//スレッド名のCRC
public:
	//基本オペレータ
	inline id_type operator*() const { return *m_id; }//スレッドID
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
	id_type* m_id;//スレッドID
	const char** m_name;//スレッド名
	GASHA_ crc32_t m_nameCrc;//スレッド名のCRC
private:
	//静的フィールド
	static thread_local bool m_thisIdIsInitialized;//現在のスレッドのスレッドIDセット済みフラグ
	static thread_local id_type m_thisId;//現在のスレッドのスレッドID
	static thread_local const char* m_thisName;//現在のスレッドのスレッド名
	static thread_local GASHA_ crc32_t m_thisNameCrc;//現在のスレッドのスレッド名のCRC
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/thread_id.inl>

#endif//GASHA_INCLUDED_THREAD_ID_H

// End of file

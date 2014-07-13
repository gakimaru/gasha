#pragma once
#ifndef GASHA_INCLUDED_LOG_CATEGORY_H
#define GASHA_INCLUDED_LOG_CATEGORY_H

//--------------------------------------------------------------------------------
// log_category.h
// ログカテゴリ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_console.h>//コンソールインターフェース
#include <gasha/std_console.h>//標準コンソール

#include <utility>//C++11 std::move
#include <cassert>//assert()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <iterator> <bitset> <mutex> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <iterator>//std::iterator
#include <bitset>//std::bitset
#include <mutex>//C++11 std::call_once

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログカテゴリ
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログカテゴリ登録用クラス宣言
template<unsigned char V, bool FOR_LOG, bool FOR_NOTICE>
class regLogCategory;
namespace _private
{
	template<unsigned char V>
	class regSpecialLogCategory;
}

//----------------------------------------
//ログカテゴリクラス
//※ログ出力カテゴリと出力先（コンソール）、および、カラーを扱う。
//※出力先、カラーはいつでも変更可能。
//※ログカテゴリを登録する時は、ログカテゴリ登録用クラス regLogCategory クラスのインスタンスを生成する。
class logCategory
{
	template<unsigned char V, bool FOR_LOG, bool FOR_NOTICE>
	friend class regLogCategory;
	template<unsigned char V>
	friend class _private::regSpecialLogCategory;
public:
	//型
	typedef unsigned char category_type;//値（カテゴリ）
	typedef unsigned char byte;//バッファ用
private:
	//定数計算用マクロ（conexprが使えない代わりに）
#define TO_OUTPUT_CATEGORY(value) ((value) >> 1)//値を出力カテゴリに変換
#define FROM_OUTPUT_CATEGORY(value) ((value) << 1)//出力カテゴリを値に変換
public:
	//定数
	static const category_type NORMAL_NUM = 11;//通常カテゴリ数
	static const category_type SPECIAL_NUM = 2;//特殊カテゴリ数
	static const category_type NUM = NORMAL_NUM + SPECIAL_NUM;//カテゴリ総数
	static const category_type MIN = 0;//カテゴリ最小値
	static const category_type MAX = NUM - 1;//カテゴリ最大値
	static const category_type NORMAL_MIN = MIN;//通常カテゴリ最小値
	static const category_type NORMAL_MAX = NORMAL_MIN + NORMAL_NUM - 1;//通常カテゴリ最大値
	static const category_type SPECIAL_MIN = NORMAL_MAX + 1;//特殊カテゴリ最小値
	static const category_type SPECIAL_MAX = SPECIAL_MIN + SPECIAL_NUM - 1;//特殊カテゴリ最大値
	static const category_type BEGIN = MIN;//カテゴリ開始値（イテレータ用）
	static const category_type END = NUM;//カテゴリ終端値（イテレータ用）
	static const category_type INVALID = NUM + 1;//無効なカテゴリ（イテレータ用）
	static const category_type POOL_NUM = NUM;//カテゴリ記録数
	static const category_type OUTPUT_CATEGORY_MIN = TO_OUTPUT_CATEGORY(NORMAL_MIN);//出力カテゴリ最小値
	static const category_type OUTPUT_CATEGORY_MAX = TO_OUTPUT_CATEGORY(NORMAL_MAX);//出力カテゴリ最大値
	static_assert(SPECIAL_MAX == MAX, "Invalid category-constants.");//定数チェック
public:
	//--------------------
	//イテレータ宣言
	class iterator;
	class reverse_iterator;
	typedef const iterator const_iterator;
	typedef const reverse_iterator const_reverse_iterator;
	//--------------------
	//イテレータ
	class iterator : public std::iterator<std::bidirectional_iterator_tag, logCategory>//双方向イテレータとして実装
	{
		friend logCategory;
	public:
		//オペレータ
		inline bool isExist() const { return m_ref != nullptr; }
		inline const logCategory* operator->() const { return m_ref; }
		inline logCategory* operator->(){ return m_ref; }
		inline const logCategory& operator*() const { return *m_ref; }
		inline logCategory& operator*(){ return *m_ref; }
		inline bool operator==(const iterator& rhs) const { return m_value == rhs.m_value; }
		inline bool operator!=(const iterator& rhs) const { return m_value != rhs.m_value; }
		inline const iterator& operator++() const { inc(); return *this; }
		inline iterator& operator++(){ inc(); return *this; }
		inline iterator operator++(int) const { iterator ite(m_value); inc(); return ite; }
		inline const iterator& operator--() const { dec(); return *this; }
		inline iterator& operator--(){ dec(); return *this; }
		inline iterator operator--(int) const { iterator ite(m_value); dec(); return ite; }
	private:
		//インクリメント
		void inc() const;
		//デクリメント
		void dec() const;
	public:
		//キャストオペレータ
		inline operator bool() const { return isExist(); }
		inline operator const logCategory&() const { return *m_ref; }//値（カテゴリ）
		inline operator logCategory&(){ return *m_ref; }//値（カテゴリ）
	public:
		//ムーブコンストラクタ
		inline iterator(iterator&& ite);
		//コピーコンストラクタ
		inline iterator(const iterator& ite);
		//コンストラクタ
		inline iterator(const category_type value);
		//デフォルトコンストラクタ
		inline iterator();
		//デストラクタ
		inline ~iterator();
	private:
		//フィールド
		mutable category_type m_value;//値（カテゴリ）
		mutable logCategory* m_ref;//ログカテゴリの参照
		mutable bool m_isEnd;//終端
	};
	//--------------------
	//リバースイテレータ
	class reverse_iterator : public std::iterator<std::bidirectional_iterator_tag, logCategory>//双方向イテレータとして実装
	{
		friend logCategory;
	public:
		//オペレータ
		inline bool isExist() const { return m_ref != nullptr; }
		inline const logCategory* operator->() const { return m_ref; }
		inline logCategory* operator->(){ return m_ref; }
		inline const logCategory& operator*() const { return *m_ref; }
		inline logCategory& operator*(){ return *m_ref; }
		inline bool operator==(const reverse_iterator& rhs) const { return m_value == rhs.m_value; }
		inline bool operator!=(const reverse_iterator& rhs) const { return m_value != rhs.m_value; }
		inline const reverse_iterator& operator++() const { inc(); return *this; }
		inline reverse_iterator& operator++(){ inc(); return *this; }
		inline reverse_iterator operator++(int) const { reverse_iterator ite(m_value); inc(); return ite; }
		inline const reverse_iterator& operator--() const { dec(); return *this; }
		inline reverse_iterator& operator--(){ dec(); return *this; }
		inline reverse_iterator operator--(int) const { reverse_iterator ite(m_value); dec(); return ite; }
	private:
		//インクリメント
		void inc() const;
		//デクリメント
		void dec() const;
	public:
		//キャストオペレータ
		inline operator bool() const { return isExist(); }
		inline operator const logCategory&() const { return *m_ref; }//値（カテゴリ）
		inline operator logCategory&(){ return *m_ref; }//値（カテゴリ）
	public:
		//ベースイテレータを取得
		inline logCategory::iterator base() const;
	public:
		//ムーブコンストラクタ
		inline reverse_iterator(reverse_iterator&& ite);
		//コピーコンストラクタ
		inline reverse_iterator(const reverse_iterator& ite);
		//コンストラクタ
		inline reverse_iterator(const category_type value);
		//デフォルトコンストラクタ
		inline reverse_iterator();
		//デストラクタ
		inline ~reverse_iterator();
	private:
		//フィールド
		mutable category_type m_value;//値（カテゴリ）
		mutable logCategory* m_ref;//ログカテゴリの参照
		mutable bool m_isEnd;//終端
	};
	//--------------------
	//コンテナ
	//※静的メンバーに既定のログカテゴリを登録
	//※イテレータで全ログカテゴリを列挙可能
	class container
	{
		friend class logCategory;
		friend class logCategory::iterator;
		friend class logCategory::reverse_iterator;
	public:
		//型
		typedef logCategory::iterator iterator;
		typedef logCategory::const_iterator const_iterator;
		typedef logCategory::reverse_iterator reverse_iterator;
		typedef logCategory::const_reverse_iterator const_reverse_iterator;
	public:
		//アクセッサ
		inline const logCategory* at(const category_type value) const { return _at(value); }
		inline logCategory* at(const category_type value){ return _at(value); }
		inline const logCategory* operator[](const category_type value) const { return _at(value); }
		inline logCategory* operator[](const category_type value){ return _at(value); }
	private:
		//静的メソッド
		inline static logCategory* _at(const category_type value);//要素を取得
		static bool update(const category_type value, const logCategory& obj);//要素を更新
	public:
		//通常メソッド
		//イテレータ取得
		inline const iterator begin() const { return iterator(BEGIN); }//開始イテレータを取得
		inline const iterator end() const { return iterator(END); }//終端イテレータを取得
		inline iterator begin(){ return iterator(BEGIN); }//開始イテレータを取得
		inline iterator end(){ return iterator(END); }//終端イテレータを取得
		inline const_iterator cbegin() const { return iterator(BEGIN); }//開始constイテレータを取得
		inline const_iterator cend() const { return iterator(END); }//終端constイテレータを取得
		//リバースイテレータ取得
		inline const reverse_iterator rbegin() const { return reverse_iterator(END); }//開始イテレータを取得
		inline const reverse_iterator rend() const { return reverse_iterator(BEGIN); }//終端イテレータを取得
		inline reverse_iterator rbegin(){ return reverse_iterator(END); }//開始イテレータを取得
		inline reverse_iterator rend(){ return reverse_iterator(BEGIN); }//終端イテレータを取得
		inline const_reverse_iterator crbegin() const { return reverse_iterator(END); }//開始constイテレータを取得
		inline const_reverse_iterator crend() const { return reverse_iterator(BEGIN); }//終端constイテレータを取得
		//全てのログカテゴリのコンソールを変更
		void setAllConsole(IConsole* console);
		//全てのログカテゴリの画面通知用コンソールを変更
		void setAllConsoleForNotice(IConsole* console);
	private:
		//初期化メソッド（一回限り）
		static void initializeOnce();
	public:
		//コンストラクタ
		inline container();
	private:
		//フィールド
		static std::once_flag m_initialized;//初期化済み
		static logCategory* m_poolPtr;//要素のプール（ポインタ）
		static byte m_pool[];//要素のプール（バッファ）※バッファとポインタを分けているのは、コンストラクタの実行を防止するため
		static std::bitset<POOL_NUM> m_isAlreadyPool;//要素の初期化済みフラグ
	};
public:
	//オペレータ
	//※出力カテゴリで比較する
	inline bool operator ==(const logCategory& rhs) const { return outputCategory() == rhs.outputCategory(); }
	inline bool operator !=(const logCategory& rhs) const { return outputCategory() != rhs.outputCategory(); }
	inline bool operator >(const logCategory& rhs) const { return outputCategory() > rhs.outputCategory(); }
	inline bool operator >=(const logCategory& rhs) const { return outputCategory() >= rhs.outputCategory(); }
	inline bool operator <(const logCategory& rhs) const { return outputCategory() < rhs.outputCategory(); }
	inline bool operator <=(const logCategory& rhs) const { return outputCategory() <= rhs.outputCategory(); }
private:
	//コピーオペレータ
	inline logCategory& operator=(const logCategory& rhs);
public:
	//キャストオペレータ
	inline operator int() const { return static_cast<int>(m_value); }//値（カテゴリ）
	inline operator category_type() const { return m_value; }//値（カテゴリ）
	inline operator const char*() const { return m_name; }//名前
private:
	//静的メソッド（アクセッサ補助要）
	inline static category_type toOutputCategory(const category_type value){ return TO_OUTPUT_CATEGORY(value); }//出力カテゴリ計算
	inline static category_type fromOutputCategory(const category_type value){ return FROM_OUTPUT_CATEGORY(value); }//出力カテゴリからカテゴリ計算
public:
	//アクセッサ
	inline bool isSpecial() const { return m_value >= SPECIAL_MIN && m_value <= SPECIAL_MAX; }//特殊カテゴリか？
	bool isRegistered() const;//登録済みか？
	inline category_type value() const { return m_value; }//値（カテゴリ）取得
	inline const char* name() const { return m_name; }//名前取得
	inline category_type outputCategory() const { return toOutputCategory(m_value); }//出力カテゴリ取得
	inline const GASHA_ IConsole* console() const { return m_console; }//コンソール
	inline GASHA_ IConsole*& console(){ return m_console; }//コンソール
	inline const GASHA_ IConsole* consoleForNotice() const { return m_consoleForNotice; }//画面通知用コンソール
	inline GASHA_ IConsole*& consoleForNotice(){ return m_consoleForNotice; }//画面通知用コンソール
	inline const consoleColor& color() const { return m_color; }//カラー取得
	inline consoleColor& color(){ return m_color; }//カラー取得
	inline const consoleColor& colorForNotice() const { return m_colorForNotice; }//カラー取得（画面通知用）
	inline consoleColor& colorForNotice(){ return m_colorForNotice; }//カラー取得（画面通知用）
public:
	//静的アクセッサ（ショートカット用）
	inline static logCategory* at(const category_type value);//指定のログカテゴリを取得
	inline static void setAllConsole(IConsole* console);//全てのログカテゴリのコンソールを変更
	inline static void setAllConsoleForNotice(IConsole* console);//全てのログカテゴリの画面通知用コンソールを変更
public:
	//メソッド
	//前のカテゴリを取得
	inline logCategory* prev() const;
	//次のカテゴリを取得
	inline logCategory* next() const;
public:
	//ムーブコンストラクタ
	inline logCategory(logCategory&& obj);
	//コピーコンストラクタ
	inline logCategory(const logCategory& obj);
	//コンテナ登録済みインスタンス取得用コンストラクタ
	inline logCategory(const category_type value);
private:
	//ログカテゴリ登録用コンストラクタ
	inline logCategory(const category_type value, const char* name, GASHA_ IConsole* console, GASHA_ IConsole* console_for_notice, GASHA_ consoleColor&& color, GASHA_ consoleColor&& color_for_notice);
	//特殊ログカテゴリ登録用コンストラクタ
	inline logCategory(const category_type value, const char* name);
public:
	//デフォルトコンストラクタ
	inline logCategory();
	//デストラクタ
	inline ~logCategory();
private:
	//フィールド
	const char* m_name;//名前
	const category_type m_value;//値（カテゴリ）
	GASHA_ IConsole* m_console;//出力先コンソール
	GASHA_ IConsole* m_consoleForNotice;//画面通知先コンソール
	GASHA_ consoleColor m_color;//カラー
	GASHA_ consoleColor m_colorForNotice;//画面通知カラー
	//マクロ消去
#undef TO_OUTPUT_CATEGORY
#undef FROM_OUTPUT_CATEGORY
};

//----------------------------------------
//既定のログカテゴリ用定数
#define MAKE_LOG_CATEGORY_VALUE(output_category, sub) (logCategory::NORMAL_MIN + output_category * 2 + sub)
#define MAKE_SPECIAL_LOG_CATEGORY_VALUE(value) (logCategory::SPECIAL_MIN + value)
enum categoryEnum : logCategory::category_type
{
	asNormal = MAKE_LOG_CATEGORY_VALUE(1, 0),//通常メッセージ
	asVerbose = MAKE_LOG_CATEGORY_VALUE(0, 0),//冗長メッセージ
	asDetail = MAKE_LOG_CATEGORY_VALUE(0, 1),//詳細メッセージ
	asImportant = MAKE_LOG_CATEGORY_VALUE(2, 0),//重要メッセージ
	asWarning = MAKE_LOG_CATEGORY_VALUE(3, 0),//警告メッセージ
	asCritical = MAKE_LOG_CATEGORY_VALUE(4, 0),//重大メッセージ
	asAbsolute = MAKE_LOG_CATEGORY_VALUE(5, 0),//絶対メッセージ（ログカテゴリに関係なく出力したいメッセージ）
	//以下、ログカテゴリ／画面通知カテゴリ変更用
	asSilent = MAKE_SPECIAL_LOG_CATEGORY_VALUE(0),//静寂（絶対メッセージ以外出力しない）
	asSilentAbsolutely = MAKE_SPECIAL_LOG_CATEGORY_VALUE(1),//絶対静寂（全てのメッセージを出力しない）
};

//----------------------------------------
//ログカテゴリ登録用テンプレートクラス
template<unsigned char V, bool FOR_LOG, bool FOR_NOTICE>
class regLogCategory : public logCategory
{
public:
	//定数
	static const category_type VALUE = V;//値（カテゴリ）
	static_assert(VALUE >= NORMAL_MIN && VALUE <= NORMAL_MAX, "out of range of normal-log-category");//値の範囲チェック
public:
	//コンストラクタ
	inline regLogCategory(const char* name, GASHA_ consoleColor&& color, GASHA_ consoleColor&& color_for_notice);
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_category.inl>

#endif//GASHA_INCLUDED_LOG_CATEGORY_H

// End of file

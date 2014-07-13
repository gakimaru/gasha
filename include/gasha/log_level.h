#pragma once
#ifndef GASHA_INCLUDED_LOG_LEVEL_H
#define GASHA_INCLUDED_LOG_LEVEL_H

//--------------------------------------------------------------------------------
// log_level.h
// ログレベル【宣言部】
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

//【VC++】例外を無効化した状態で <iterator> <bitset> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <iterator>//std::iterator
#include <bitset>//std::bitset

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログレベル
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログレベルクラス
//※ログ出力レベルと出力先（コンソール）、および、カラーを扱う。
//※出力先、カラーはいつでも変更可能。
class logLevel
{
public:
	//型
	typedef unsigned char level_type;//値（レベル）
	typedef unsigned char byte;//バッファ用
private:
	//定数計算用マクロ（conexprが使えない代わりに）
	#define CALC_TO_OUTPUT_LEVEL(value) ((value) >> 1)//値を出力レベルに変換
	#define CALC_FROM_OUTPUT_LEVEL(value) ((value) << 1)//出力レベルを値に変換
public:
	//定数
	static const level_type NORMAL_NUM = 11;//通常レベル数
	static const level_type SPECIAL_NUM = 2;//特殊レベル数
	static const level_type NUM = NORMAL_NUM + SPECIAL_NUM;//レベル総数
	static const level_type MIN = 0;//レベル最小値
	static const level_type MAX = NUM - 1;//レベル最大値
	static const level_type NORMAL_MIN = MIN;//通常レベル最小値
	static const level_type NORMAL_MAX = NORMAL_MIN + NORMAL_NUM - 1;//通常レベル最大値
	static const level_type SPECIAL_MIN = NORMAL_MAX + 1;//特殊レベル最小値
	static const level_type SPECIAL_MAX = SPECIAL_MIN + SPECIAL_NUM - 1;//特殊レベル最大値
	static const level_type BEGIN = MIN;//レベル開始値（イテレータ用）
	static const level_type END = NUM;//レベル終端値（イテレータ用）
	static const level_type INVALID = NUM + 1;//無効なレベル（イテレータ用）
	static const level_type POOL_NUM = NUM;//レベル記録数
	static const level_type OUTPUT_LEVEL_MIN = CALC_TO_OUTPUT_LEVEL(NORMAL_MIN);//出力レベル最小値
	static const level_type OUTPUT_LEVEL_MAX = CALC_TO_OUTPUT_LEVEL(NORMAL_MAX);//出力レベル最大値
	static_assert(SPECIAL_MAX == MAX, "Invalid level-constants.");//定数チェック
public:
	//型
	//--------------------
	//イテレータ宣言
	class iterator;
	class reverse_iterator;
	typedef const iterator const_iterator;
	typedef const reverse_iterator const_reverse_iterator;
	//--------------------
	//イテレータ
	class iterator : public std::iterator<std::bidirectional_iterator_tag, logLevel>//双方向イテレータとして実装
	{
		friend logLevel;
	public:
		//オペレータ
		inline const logLevel* operator->() const { return m_ref; }
		inline logLevel* operator->(){ return m_ref; }
		inline const logLevel& operator*() const { return *m_ref; }
		inline logLevel& operator*(){ return *m_ref; }
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
		operator const logLevel&() const { return *m_ref; }//値（レベル）
		operator logLevel&(){ return *m_ref; }//値（レベル）
	public:
		//ムーブコンストラクタ
		inline iterator(iterator&& ite);
		//コピーコンストラクタ
		inline iterator(const iterator& ite);
		//コンストラクタ
		inline iterator(const level_type value);
		//デフォルトコンストラクタ
		inline iterator();
		//デストラクタ
		inline ~iterator();
	private:
		//フィールド
		mutable level_type m_value;//値（レベル）
		mutable logLevel* m_ref;//ログレベルの参照
		mutable bool m_isEnd;//終端
	};
	//--------------------
	//リバースイテレータ
	class reverse_iterator : public std::iterator<std::bidirectional_iterator_tag, logLevel>//双方向イテレータとして実装
	{
		friend logLevel;
	public:
		//オペレータ
		inline const logLevel* operator->() const { return m_ref; }
		inline logLevel* operator->(){ return m_ref; }
		inline const logLevel& operator*() const { return *m_ref; }
		inline logLevel& operator*(){ return *m_ref; }
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
		operator const logLevel&() const { return *m_ref; }//値（レベル）
		operator logLevel&(){ return *m_ref; }//値（レベル）
	public:
		//ベースイテレータを取得
		inline logLevel::iterator base() const;
	public:
		//ムーブコンストラクタ
		inline reverse_iterator(reverse_iterator&& ite);
		//コピーコンストラクタ
		inline reverse_iterator(const reverse_iterator& ite);
		//コンストラクタ
		inline reverse_iterator(const level_type value);
		//デフォルトコンストラクタ
		inline reverse_iterator();
		//デストラクタ
		inline ~reverse_iterator();
	private:
		//フィールド
		mutable level_type m_value;//値（レベル）
		mutable logLevel* m_ref;//ログレベルの参照
		mutable bool m_isEnd;//終端
	};
	//--------------------
	//コンテナ
	//※静的メンバーに既定のログレベルを登録
	//※イテレータで全ログレベルを列挙可能
	class container
	{
		friend class logLevel;
		friend class iterator;
	public:
		//型
		typedef logLevel::iterator iterator;
		typedef logLevel::const_iterator const_iterator;
		typedef logLevel::reverse_iterator reverse_iterator;
		typedef logLevel::const_reverse_iterator const_reverse_iterator;
	public:
		//アクセッサ
		inline const logLevel* at(const level_type value) const { return _at(value); }
		inline logLevel* at(const level_type value){ return _at(value); }
		inline const logLevel* operator[](const level_type value) const { return _at(value); }
		inline logLevel* operator[](const level_type value){ return _at(value); }
	private:
		//静的メソッド
		inline static const level_type beginValue(){ return BEGIN; }//開始値取得
		inline static const level_type endValue(){ return END; }//終端値取得
		inline static const level_type invalidValue(){ return INVALID; }//無効な値取得
		inline static logLevel* _at(const level_type value);//要素を取得
		static bool update(const level_type value, const logLevel& obj);//要素を更新
	public:
		//通常メソッド
		//イテレータ取得
		inline const iterator begin() const { return iterator(beginValue()); }//開始イテレータを取得
		inline const iterator end() const { return iterator(endValue()); }//終端イテレータを取得
		inline iterator begin(){ return iterator(beginValue()); }//開始イテレータを取得
		inline iterator end(){ return iterator(endValue()); }//終端イテレータを取得
		inline const_iterator cbegin() const { return iterator(beginValue()); }//開始constイテレータを取得
		inline const_iterator cend() const { return iterator(endValue()); }//終端constイテレータを取得
		//リバースイテレータ取得
		inline const reverse_iterator rbegin() const { return reverse_iterator(endValue()); }//開始イテレータを取得
		inline const reverse_iterator rend() const { return reverse_iterator(beginValue()); }//終端イテレータを取得
		inline reverse_iterator rbegin(){ return reverse_iterator(endValue()); }//開始イテレータを取得
		inline reverse_iterator rend(){ return reverse_iterator(beginValue()); }//終端イテレータを取得
		inline const_reverse_iterator crbegin() const { return reverse_iterator(endValue()); }//開始constイテレータを取得
		inline const_reverse_iterator crend() const { return reverse_iterator(beginValue()); }//終端constイテレータを取得
	public:
		//静的メソッド
		//全てのログレベルのコンソールを変更
		static void setAllConsole(IConsole* console);
		//全てのログレベルの画面通知用コンソールを変更
		static void setAllConsoleForNotice(IConsole* console);
		//初期化メソッド（一回限り）
		static void initializeOnce();
	public:
		//コンストラクタ
		inline container();
	private:
		//フィールド
		static bool m_isInitialized;//初期化済み
		static logLevel* m_poolPtr;//要素のプール（ポインタ）
		static byte m_pool[];//要素のプール（バッファ）※バッファとポインタを分けているのは、コンストラクタの実行を防止するため
		static std::bitset<POOL_NUM> m_isAlreadyPool;//要素の初期化済みフラグ
	};
public:
	//オペレータ
	//※出力レベルで比較する
	inline bool operator ==(const logLevel& rhs) const { return outputLevel() == rhs.outputLevel(); }
	inline bool operator !=(const logLevel& rhs) const { return outputLevel() != rhs.outputLevel(); }
	inline bool operator >(const logLevel& rhs) const { return outputLevel() > rhs.outputLevel(); }
	inline bool operator >=(const logLevel& rhs) const { return outputLevel() >= rhs.outputLevel(); }
	inline bool operator <(const logLevel& rhs) const { return outputLevel() < rhs.outputLevel(); }
	inline bool operator <=(const logLevel& rhs) const { return outputLevel() <= rhs.outputLevel(); }
private:
	//コピーオペレータ
	inline logLevel& operator=(const logLevel& rhs);
public:
	//キャストオペレータ
	inline operator int() const { return static_cast<int>(m_value); }//値（レベル）
	inline operator level_type() const { return m_value; }//値（レベル）
	inline operator const char*() const { return m_name; }//名前
private:
	//静的メソッド（アクセッサ補助要）
	inline static level_type calcToOutputLevel(const level_type value){ return CALC_TO_OUTPUT_LEVEL(value); }//出力レベル計算
public:
	//アクセッサ
	inline level_type value() const { return m_value; }//値（レベル）取得
	inline const char* name() const { return m_name; }//名前取得
	inline level_type outputLevel() const { return CALC_TO_OUTPUT_LEVEL(m_value); }//出力レベル取得
	inline bool forLog() const { return m_forLog; }//ログレベルとして使用可能か？
	inline bool forNotice() const { return m_forNotice; }//画面通知レベルとして使用可能か？
	inline bool forMask() const { return m_forMask; }//出力レベルマスクとして使用可能か？
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
	inline static logLevel* at(const level_type value){ return container::_at(value); }//指定のログレベルを取得
	inline static void setAllConsole(IConsole* console){ container::setAllConsole(console); }//全てのログレベルのコンソールを変更
	inline static void setAllConsoleForNotice(IConsole* console){ container::setAllConsoleForNotice(console); }//全てのログレベルの画面通知用コンソールを変更
public:
	//メソッド
	//前のレベルを取得
	inline logLevel* prev() const;
	//次のレベルを取得
	inline logLevel* next() const;
public:
	//ムーブコンストラクタ
	inline logLevel(logLevel&& obj);
	//コピーコンストラクタ
	inline logLevel(const logLevel& obj);
	//コンストラクタ
	inline logLevel(const level_type value, const char* name, const bool for_log, const bool for_notice, const bool for_mask, GASHA_ IConsole* console, GASHA_ IConsole* console_for_notice, GASHA_ consoleColor&& color, GASHA_ consoleColor&& color_for_notice);
	//コンテナ登録済みインスタンス取得用コンストラクタ
	inline logLevel(const level_type value);
	//デフォルトコンストラクタ
	inline logLevel();
	//デストラクタ
	inline ~logLevel();
private:
	//フィールド
	const char* m_name;//名前
	const level_type m_value;//値（レベル）
	const bool m_forLog;//ログ出力レベルとして使用可能か？
	const bool m_forNotice;//画面通知レベルとして使用可能か？
	const bool m_forMask;//出力レベルマスクとして使用可能か？
	GASHA_ IConsole* m_console;//出力先コンソール
	GASHA_ IConsole* m_consoleForNotice;//画面通知先コンソール
	GASHA_ consoleColor m_color;//カラー
	GASHA_ consoleColor m_colorForNotice;//画面通知カラー
	//マクロ消去
	#undef CALC_TO_OUTPUT_LEVEL
	#undef CALC_FROM_OUTPUT_LEVEL
};

//----------------------------------------
//レベル定義用テンプレートクラス：通常レベル用
template<unsigned char V, bool _FOR_LOG, bool _FOR_NOTICE>
class normalLogLevel : public logLevel
{
public:
	//定数
	static const level_type VALUE = V;//値（レベル）
	static_assert(VALUE >= NORMAL_MIN && VALUE <= NORMAL_MAX, "out of range of logLevel");//値の範囲チェック
	static const bool FOR_LOG = _FOR_LOG;//ログレベルとして使用可能か？
	static const bool FOR_NOTICE = _FOR_NOTICE;//画面通知レベルとして使用可能か？
	static const bool FOR_MASK = true;//出力レベルマスクとして使用可能か？
public:
	//コンストラクタ
	inline normalLogLevel(const char* name, GASHA_ consoleColor&& color, GASHA_ consoleColor&& color_for_notice);
};

//----------------------------------------
//レベル定義用テンプレートクラス：特殊レベル用
template<unsigned char V>
class specialLogLevel : public logLevel
{
public:
	//定数
	static const level_type VALUE = V;//値（レベル）
	static_assert(VALUE >= SPECIAL_MIN && VALUE <= SPECIAL_MAX, "out of range of logLevel");//値の範囲チェック
	static const bool FOR_LOG = false;//ログレベルとして使用可能か？
	static const bool FOR_NOTICE = false;//画面通知レベルとして使用可能か？
	static const bool FOR_MASK = true;//出力レベルマスクとして使用可能か？
public:
	//コンストラクタ
	inline specialLogLevel(const char* name);
};

//----------------------------------------
//既定のログレベル用定数
#define MAKE_LOG_LEVEL_VALUE(output_level, sub) (logLevel::NORMAL_MIN + output_level * 2 + sub)
#define MAKE_SPECIAL_LOG_LEVEL_VALUE(value) (logLevel::SPECIAL_MIN + value)
enum levelEnum : logLevel::level_type
{
	asNormal = MAKE_LOG_LEVEL_VALUE(1, 0),//通常メッセージ
	asVerbose = MAKE_LOG_LEVEL_VALUE(0, 0),//冗長メッセージ
	asDetail = MAKE_LOG_LEVEL_VALUE(0, 1),//詳細メッセージ
	asImportant = MAKE_LOG_LEVEL_VALUE(2, 0),//重要メッセージ
	asWarning = MAKE_LOG_LEVEL_VALUE(3, 0),//警告メッセージ
	asCritical = MAKE_LOG_LEVEL_VALUE(4, 0),//重大メッセージ
	asAbsolute = MAKE_LOG_LEVEL_VALUE(5, 0),//絶対メッセージ（ログレベルに関係なく出力したいメッセージ）
	//以下、ログレベル／画面通知レベル変更用
	asSilent = MAKE_SPECIAL_LOG_LEVEL_VALUE(0),//絶対メッセージ（ログレベルに関係なく出力したいメッセージ）
	asSilentAbsolutely = MAKE_SPECIAL_LOG_LEVEL_VALUE(1),//絶対静寂（全てのメッセージを出力しない）
};

//----------------------------------------
//既定のログレベル定義
#define DECLARE_LOG_LEVEL(value, for_log, for_notice, fore_color, back_color, fore_color_for_notice, back_color_for_notice) struct level_##value : public normalLogLevel<value, for_log, for_notice>{ level_##value () : normalLogLevel<value, for_log, for_notice>(#value, GASHA_ consoleColor(GASHA_ consoleColor::fore_color, GASHA_ consoleColor::back_color), GASHA_ consoleColor(GASHA_ consoleColor::fore_color_for_notice, GASHA_ consoleColor::back_color_for_notice)){} }
#define DECLARE_SPECIAL_LOG_LEVEL(value) struct level_##value : public specialLogLevel<value>{ level_##value () : specialLogLevel<value>(#value){} }

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_level.inl>

#endif//GASHA_INCLUDED_LOG_LEVEL_H

// End of file

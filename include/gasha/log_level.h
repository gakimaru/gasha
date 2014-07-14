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
#include <gasha/console_color.h>//コンソールカラー

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
//ログレベル
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//クラス宣言
class logLevelContainer;
template<unsigned char _LEVEL>
class regLogLevel;
namespace _private
{
	template<unsigned char _LEVEL>
	class regLogLevel;
	template<unsigned char _LEVEL>
	class regSpecialLogLevel;
}

//----------------------------------------
//ログレベル
//※ログ出力レベルと出力先（コンソール）、および、カラーを扱う。
//※コンストラクタにログレベルを指定すると、コンテナに登録済みのログレベル情報を扱える。
//※一度登録したものは取り消せないが、出力先、カラーをいつでも変更することができる。
//※コンテナへの登録には regLogLevel 関数オブジェクトを用いる。（テンプレートクラスにより、有効範囲内の値かコンパイル時にチェックする）
class logLevel
{
	friend class logLevelContainer;
	template<unsigned char _LEVEL>
	friend class regLogLevel;
	template<unsigned char _LEVEL>
	friend class _private::regLogLevel;
	template<unsigned char _LEVEL>
	friend class _private::regSpecialLogLevel;
public:
	//型
	typedef unsigned char level_type;//値（レベル）
private:
	//ログレベル情報
	struct info
	{
		const char* m_name;//名前
		level_type m_value;//値（レベル）
		GASHA_ IConsole* m_console;//出力先コンソール
		GASHA_ IConsole* m_consoleForNotice;//画面通知先コンソール
		GASHA_ consoleColor m_color;//カラー
		GASHA_ consoleColor m_colorForNotice;//画面通知カラー
	};
private:
	//定数計算用マクロ／関数
	#define TO_OUTPUT_LEVEL(value) ((value) >> 1)//値を出力レベルに変換
	#define FROM_OUTPUT_LEVEL(value) ((value) << 1)//出力レベルを値に変換
	inline static constexpr level_type toOutputLevel(const level_type value){ return TO_OUTPUT_LEVEL(value); }//出力レベル計算
	inline static constexpr level_type fromOutputLevel(const level_type value){ return FROM_OUTPUT_LEVEL(value); }//出力レベルからレベル計算
public:
	//定数
	static const level_type NORMAL_NUM = 11;//通常ログレベル数
	static const level_type SPECIAL_NUM = 2;//特殊ログレベル数
	static const level_type NUM = NORMAL_NUM + SPECIAL_NUM;//ログレベル総数
	static const level_type MIN = 0;//ログレベル最小値
	static const level_type MAX = NUM - 1;//ログレベル最大値
	static const level_type NORMAL_MIN = MIN;//通常ログレベル最小値
	static const level_type NORMAL_MAX = NORMAL_MIN + NORMAL_NUM - 1;//通常ログレベル最大値
	static const level_type SPECIAL_MIN = NORMAL_MAX + 1;//特殊ログレベル最小値
	static const level_type SPECIAL_MAX = SPECIAL_MIN + SPECIAL_NUM - 1;//特殊ログレベル最大値
	static const level_type BEGIN = MIN;//ログレベル開始値（イテレータ用）
	static const level_type END = NUM;//ログレベル終端値（イテレータ用）
	static const level_type INVALID = NUM + 1;//無効なログレベル（イテレータ用）
	static const level_type POOL_NUM = NUM;//ログレベル記録数
	static const level_type OUTPUT_LEVEL_MIN = TO_OUTPUT_LEVEL(NORMAL_MIN);//出力レベル最小値
	static const level_type OUTPUT_LEVEL_MAX = TO_OUTPUT_LEVEL(NORMAL_MAX);//出力レベル最大値
	static_assert(SPECIAL_MAX == MAX, "Invalid level-constants.");//定数チェック
	//マクロ消去
	#undef TO_OUTPUT_LEVEL
	#undef FROM_OUTPUT_LEVEL
public:
	//比較オペレータ
	//※出力レベルで比較する
	inline bool operator ==(const logLevel& rhs) const { return outputLevel() == rhs.outputLevel(); }
	inline bool operator !=(const logLevel& rhs) const { return outputLevel() != rhs.outputLevel(); }
	inline bool operator >(const logLevel& rhs) const { return outputLevel() > rhs.outputLevel(); }
	inline bool operator >=(const logLevel& rhs) const { return outputLevel() >= rhs.outputLevel(); }
	inline bool operator <(const logLevel& rhs) const { return outputLevel() < rhs.outputLevel(); }
	inline bool operator <=(const logLevel& rhs) const { return outputLevel() <= rhs.outputLevel(); }
public:
	//キャストオペレータ
	inline operator bool() const { return isExist(); }//正しいログレベルか？
	inline operator int() const { return static_cast<int>(m_info->m_value); }//値（レベル）
	inline operator level_type() const { return m_info->m_value; }//値（レベル）
	inline operator const char*() const { return m_info->m_name; }//名前
public:
	//アクセッサ
	inline bool isExist() const { return m_info != nullptr; }//正しいログレベルか？
	inline bool isSpecial() const { return m_info->m_value >= SPECIAL_MIN && m_info->m_value <= SPECIAL_MAX; }//特殊レベルか？
	inline level_type value() const { return m_info->m_value; }//値（レベル）取得
	inline const char* name() const { return m_info->m_name; }//名前取得
	inline level_type outputLevel() const { return toOutputLevel(m_info->m_value); }//出力レベル取得
	inline const GASHA_ IConsole* console() const { return m_info->m_console; }//コンソール
	inline GASHA_ IConsole*& console(){ return m_info->m_console; }//コンソール
	inline const GASHA_ IConsole* consoleForNotice() const { return m_info->m_consoleForNotice; }//画面通知用コンソール
	inline GASHA_ IConsole*& consoleForNotice(){ return m_info->m_consoleForNotice; }//画面通知用コンソール
	inline const consoleColor& color() const { return m_info->m_color; }//カラー取得
	inline consoleColor& color(){ return m_info->m_color; }//カラー取得
	inline const consoleColor& colorForNotice() const { return m_info->m_colorForNotice; }//カラー取得（画面通知用）
	inline consoleColor& colorForNotice(){ return m_info->m_colorForNotice; }//カラー取得（画面通知用）
public:
	//メソッド
	//前後のログレベルを取得
	//※末端では不正なログレベルを返すので、isExist()でチェック必須
	logLevel prev() const;
	logLevel next() const;
public:
	//ムーブオペレータ
	inline logLevel& operator=(logLevel&& rhs);
	//コピーオペレータ
	inline logLevel& operator=(const logLevel& rhs);
public:
	//ムーブコンストラクタ
	inline logLevel(logLevel&& obj);
	//コピーコンストラクタ
	inline logLevel(const logLevel& obj);
	//コンストラクタ
	inline logLevel(const level_type value);
	//デフォルトコンストラクタ
	inline logLevel();
	//デストラクタ
	inline ~logLevel();
private:
	//コピーオペレータ
	inline logLevel& operator=(info*info);
	//コンストラクタ
	inline logLevel(info* info);
private:
	//フィールド
	info* m_info;//ログレベル情報
};

//--------------------
//ログレベルコンテナ
//※イテレータで全ログレベルを列挙可能
class logLevelContainer
{
	friend class logLevel;
	template<unsigned char _LEVEL>
	friend class regLogLevel;
	template<unsigned char _LEVEL>
	friend class _private::regLogLevel;
	template<unsigned char _LEVEL>
	friend class _private::regSpecialLogLevel;
public:
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
		inline bool isExist() const { return m_logLevel.isExist(); }
		inline const logLevel* operator->() const { return &m_logLevel; }
		inline logLevel* operator->(){ return &m_logLevel; }
		inline const logLevel& operator*() const { return m_logLevel; }
		inline logLevel& operator*(){ return m_logLevel; }
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
		inline operator const logLevel&() const { return m_logLevel; }//値（レベル）
		inline operator logLevel&(){ return m_logLevel; }//値（レベル）
	public:
		//ムーブオペレータ
		inline iterator& operator=(iterator&& rhs);
		//コピーオペレータ
		inline iterator& operator=(const iterator& rhs);
	public:
		//ムーブコンストラクタ
		inline iterator(iterator&& ite);
		//コピーコンストラクタ
		inline iterator(const iterator& ite);
		//コンストラクタ
		iterator(const logLevel::level_type value);
		//デフォルトコンストラクタ
		inline iterator();
		//デストラクタ
		inline ~iterator();
	private:
		//フィールド
		mutable logLevel::level_type m_value;//値（レベル）
		mutable logLevel m_logLevel;//ログレベル
		mutable bool m_isEnd;//終端か？
	};
	//--------------------
	//リバースイテレータ
	class reverse_iterator : public std::iterator<std::bidirectional_iterator_tag, logLevel>//双方向イテレータとして実装
	{
		friend logLevel;
	public:
		//オペレータ
		inline bool isExist() const { return m_logLevel.isExist(); }
		inline const logLevel* operator->() const { return &m_logLevel; }
		inline logLevel* operator->(){ return &m_logLevel; }
		inline const logLevel& operator*() const { return m_logLevel; }
		inline logLevel& operator*(){ return m_logLevel; }
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
		inline operator const logLevel&() const { return m_logLevel; }//値（レベル）
		inline operator logLevel&(){ return m_logLevel; }//値（レベル）
	public:
		//ムーブオペレータ
		inline reverse_iterator& operator=(reverse_iterator&& rhs);
		//コピーオペレータ
		inline reverse_iterator& operator=(const reverse_iterator& rhs);
	public:
		//ベースイテレータを取得
		inline logLevelContainer::iterator base() const;
	public:
		//ムーブコンストラクタ
		inline reverse_iterator(reverse_iterator&& ite);
		//コピーコンストラクタ
		inline reverse_iterator(const reverse_iterator& ite);
		//コンストラクタ
		reverse_iterator(const logLevel::level_type value);
		//デフォルトコンストラクタ
		inline reverse_iterator();
		//デストラクタ
		inline ~reverse_iterator();
	private:
		//フィールド
		mutable logLevel::level_type m_value;//値（レベル）
		mutable logLevel m_logLevel;//ログレベル
		mutable bool m_isEnd;//終端か？
	};
public:
	//アクセッサ
	inline const logLevel at(const logLevel::level_type value) const { return logLevel(getInfo(value)); }
	inline logLevel at(const logLevel::level_type value){ return logLevel(getInfo(value)); }
	inline const logLevel operator[](const logLevel::level_type value) const { return logLevel(getInfo(value)); }
	inline logLevel operator[](const logLevel::level_type value){ return logLevel(getInfo(value)); }
private:
	//静的メソッド
	inline static logLevel::info* getInfo(const logLevel::level_type value);//要素を取得
	static bool regist(const logLevel::info& info);//要素を登録
public:
	//通常メソッド
	//イテレータ取得
	inline const iterator begin() const { return iterator(logLevel::BEGIN); }//開始イテレータを取得
	inline const iterator end() const { return iterator(logLevel::END); }//終端イテレータを取得
	inline iterator begin(){ return iterator(logLevel::BEGIN); }//開始イテレータを取得
	inline iterator end(){ return iterator(logLevel::END); }//終端イテレータを取得
	inline const_iterator cbegin() const { return iterator(logLevel::BEGIN); }//開始constイテレータを取得
	inline const_iterator cend() const { return iterator(logLevel::END); }//終端constイテレータを取得
	//リバースイテレータ取得
	inline const reverse_iterator rbegin() const { return reverse_iterator(logLevel::END); }//開始イテレータを取得
	inline const reverse_iterator rend() const { return reverse_iterator(logLevel::BEGIN); }//終端イテレータを取得
	inline reverse_iterator rbegin(){ return reverse_iterator(logLevel::END); }//開始イテレータを取得
	inline reverse_iterator rend(){ return reverse_iterator(logLevel::BEGIN); }//終端イテレータを取得
	inline const_reverse_iterator crbegin() const { return reverse_iterator(logLevel::END); }//開始constイテレータを取得
	inline const_reverse_iterator crend() const { return reverse_iterator(logLevel::BEGIN); }//終端constイテレータを取得
	//全てのログレベルのコンソールを変更
	void setAllConsole(IConsole* console);
	//全てのログレベルの画面通知用コンソールを変更
	void setAllConsoleForNotice(IConsole* console);
private:
	//初期化メソッド（一回限り）
	static void initializeOnce();
public:
	//コンストラクタ
	inline logLevelContainer();
	//デストラクタ
	inline ~logLevelContainer();
private:
	//フィールド
	static std::once_flag m_initialized;//初期化済み
	static logLevel::info m_pool[logLevel::POOL_NUM];//要素のプール（バッファ）
	static std::bitset<logLevel::POOL_NUM> m_isAlreadyPool;//要素の初期化済みフラグ
};

//----------------------------------------
//ログレベル登録用テンプレートクラス
//※テンプレートクラスにより、有効範囲内の値かコンパイル時にチェックする。
template<unsigned char _LEVEL>
class regLogLevel
{
public:
	//定数
	static const logLevel::level_type LEVEL = _LEVEL;//値（レベル）
	static_assert(LEVEL >= logLevel::NORMAL_MIN && LEVEL <= logLevel::NORMAL_MAX, "Out of range of normal-log-level");//値の範囲チェック
public:
	//関数オペレータ
	inline bool operator()(const char* name, IConsole* console, IConsole* console_for_notice, GASHA_ consoleColor&& color, GASHA_ consoleColor&& color_for_notice);
};

//----------------------------------------
//既定のログレベル用定数
#define MAKE_LOG_LEVEL_VALUE(OUTPUT_LEVEL, SUB_VALIE) (logLevel::NORMAL_MIN + OUTPUT_LEVEL * 2 + SUB_VALIE)
#define MAKE_SPECIAL_LOG_LEVEL_VALUE(VALUE) (logLevel::SPECIAL_MIN + VALUE)
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
	asSilent = MAKE_SPECIAL_LOG_LEVEL_VALUE(0),//静寂（絶対メッセージ以外出力しない）
	asSilentAbsolutely = MAKE_SPECIAL_LOG_LEVEL_VALUE(1),//絶対静寂（全てのメッセージを出力しない）
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_level.inl>

#endif//GASHA_INCLUDED_LOG_LEVEL_H

// End of file

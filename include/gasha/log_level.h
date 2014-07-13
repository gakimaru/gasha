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
#include <gasha/console_std.h>//標準コンソールー

#include <utility>//C++11 std::move
#include <memory.h>//memcpy()
#include <assert.h>//assert()

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

//----------------------------------------
//ログレベルクラス
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
	static_assert(SPECIAL_MAX == MAX, "invalid category numbers.");//定数チェック
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
		void inc() const
		{
			if (m_value == container::invalidValue())
				return;
			else if (m_value == container::endValue())
			{
				m_value = container::invalidValue();
				m_ref = nullptr;
				m_isEnd = false;
				return;
			}
			while (m_value != container::endValue())
			{
				++m_value;
				m_ref = container::staticAt(m_value);
				if (m_ref)
					return;
			}
			m_ref = nullptr;
			m_isEnd = true;
		}
		//デクリメント
		void dec() const
		{
			m_isEnd = false;
			if (m_value == container::invalidValue())
				return;
			else if (m_value == container::beginValue())
			{
				m_value = container::invalidValue();
				m_ref = nullptr;
				return;
			}
			while (m_value != container::beginValue())
			{
				--m_value;
				m_ref = container::staticAt(m_value);
				if (m_ref)
					return;
			}
			m_ref = container::staticAt(m_value);
			if (!m_ref)
				m_value = container::invalidValue();
			return;
		}
	public:
		//キャストオペレータ
		operator const logLevel&() const { return *m_ref; }//値（レベル）
		operator logLevel&(){ return *m_ref; }//値（レベル）
	public:
		//ムーブコンストラクタ
		iterator(iterator&& ite) :
			m_value(ite.m_value),
			m_ref(ite.m_ref),
			m_isEnd(ite.m_isEnd)
		{}
		//コピーコンストラクタ
		iterator(const iterator& ite) :
			m_value(ite.m_value),
			m_ref(ite.m_ref),
			m_isEnd(ite.m_isEnd)
		{}
		//コンストラクタ
		iterator(const level_type value) :
			m_value(value),
			m_ref(container::staticAt(value)),
			m_isEnd(value == container::endValue())
		{
			if (!m_isEnd && !m_ref)
				m_value = container::beginValue();
		}
		//デフォルトコンストラクタ
		iterator() :
			m_value(container::endValue()),
			m_ref(nullptr),
			m_isEnd(true)
		{}
		//デストラクタ
		~iterator()
		{}
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
		void inc() const
		{
			if (m_value == container::invalidValue())
				return;
			else if (m_value == container::beginValue())
			{
				m_value = container::invalidValue();
				m_ref = nullptr;
				m_isEnd = false;
				return;
			}
			while (m_value != container::beginValue())
			{
				--m_value;
				m_ref = container::staticAt(m_value - 1);
				if (m_ref)
					return;
			}
			m_ref = nullptr;
			m_isEnd = true;
		}
		//デクリメント
		void dec() const
		{
			m_isEnd = false;
			if (m_value == container::invalidValue())
				return;
			else if (m_value == container::endValue())
			{
				m_value = container::invalidValue();
				m_ref = nullptr;
				return;
			}
			while (m_value != container::endValue())
			{
				++m_value;
				m_ref = container::staticAt(m_value - 1);
				if (m_ref)
					return;
			}
			m_ref = container::staticAt(m_value - 1);
			if (!m_ref)
				m_value = container::invalidValue();
			return;
		}
	public:
		//キャストオペレータ
		operator const logLevel&() const { return *m_ref; }//値（レベル）
		operator logLevel&(){ return *m_ref; }//値（レベル）
	public:
		logLevel::iterator base() const
		{
			logLevel::iterator ite(m_value);
			return ite;
		}
	public:
		//ムーブコンストラクタ
		reverse_iterator(reverse_iterator&& ite) :
			m_value(ite.m_value),
			m_ref(ite.m_ref),
			m_isEnd(ite.m_isEnd)
		{}
		//コピーコンストラクタ
		reverse_iterator(const reverse_iterator& ite) :
			m_value(ite.m_value),
			m_ref(ite.m_ref),
			m_isEnd(ite.m_isEnd)
		{}
		//コンストラクタ
		reverse_iterator(const level_type value) :
			m_value(value),
			m_ref(container::staticAt(value - 1)),
			m_isEnd(value == container::endValue())
		{
			if (!m_isEnd && !m_ref)
				m_value = container::beginValue();
		}
		//デフォルトコンストラクタ
		reverse_iterator() :
			m_value(container::endValue()),
			m_ref(nullptr),
			m_isEnd(true)
		{}
		//デストラクタ
		~reverse_iterator()
		{}
	private:
		//フィールド
		mutable level_type m_value;//値（レベル）
		mutable logLevel* m_ref;//ログレベルの参照
		mutable bool m_isEnd;//終端
	};
	//--------------------
	//コンテナ（イテレータ用）
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
		const logLevel* at(const level_type value) const { return staticAt(value); }
		logLevel* at(const level_type value){ return staticAt(value); }
		const logLevel* operator[](const level_type value) const { return staticAt(value); }
		logLevel* operator[](const level_type value){ return staticAt(value); }
	private:
		//メソッド
		static const level_type beginValue(){ return BEGIN; }//開始値取得
		static const level_type endValue(){ return END; }//終端値取得
		static const level_type invalidValue(){ return INVALID; }//無効な値取得
		static logLevel* staticAt(const level_type value){ if (value < MIN || value > MAX || !m_isAlreadyPool[value]) return nullptr; return &m_poolPtr[value]; }//要素を取得
		static bool update(const level_type value, const logLevel& obj)//要素を更新
		{
			if (value >= MIN && value <= MAX && !container::m_isAlreadyPool[value])
			{
				container::m_poolPtr[value] = obj;
				container::m_isAlreadyPool[value] = true;
				return true;
			}
			return false;
		}
	public:
		//イテレータ取得
		const iterator begin() const { return iterator(beginValue()); }//開始イテレータを取得
		const iterator end() const { return iterator(endValue()); }//終端イテレータを取得
		iterator begin(){ return iterator(beginValue()); }//開始イテレータを取得
		iterator end(){ return iterator(endValue()); }//終端イテレータを取得
		const_iterator cbegin() const { return iterator(beginValue()); }//開始constイテレータを取得
		const_iterator cend() const { return iterator(endValue()); }//終端constイテレータを取得
		//リバースイテレータ取得
		const reverse_iterator rbegin() const { return reverse_iterator(endValue()); }//開始イテレータを取得
		const reverse_iterator rend() const { return reverse_iterator(beginValue()); }//終端イテレータを取得
		reverse_iterator rbegin(){ return reverse_iterator(endValue()); }//開始イテレータを取得
		reverse_iterator rend(){ return reverse_iterator(beginValue()); }//終端イテレータを取得
		const_reverse_iterator crbegin() const { return reverse_iterator(endValue()); }//開始constイテレータを取得
		const_reverse_iterator crend() const { return reverse_iterator(beginValue()); }//終端constイテレータを取得
		//初期化メソッド（一回限り）
		static void initializeOnce();
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
	bool operator ==(const logLevel& rhs) const { return outputLevel() == rhs.outputLevel(); }
	bool operator !=(const logLevel& rhs) const { return outputLevel() != rhs.outputLevel(); }
	bool operator >(const logLevel& rhs) const { return outputLevel() > rhs.outputLevel(); }
	bool operator >=(const logLevel& rhs) const { return outputLevel() >= rhs.outputLevel(); }
	bool operator <(const logLevel& rhs) const { return outputLevel() < rhs.outputLevel(); }
	bool operator <=(const logLevel& rhs) const { return outputLevel() <= rhs.outputLevel(); }
private:
	//コピーオペレータ
	logLevel& operator=(const logLevel& rhs)
	{
		memcpy(this, &rhs, sizeof(*this));//強制更新
		return *this;
	}
public:
	//キャストオペレータ
	operator int() const { return static_cast<int>(m_value); }//値（レベル）
	operator level_type() const { return m_value; }//値（レベル）
	operator const char*() const { return m_name; }//名前
public:
	//アクセッサ
	level_type value() const { return m_value; }//値（レベル）取得
	const char* name() const { return m_name; }//名前取得
	static level_type calcToOutputLevel(const level_type value){ return CALC_TO_OUTPUT_LEVEL(value); }//出力レベル計算
	level_type outputLevel() const { return CALC_TO_OUTPUT_LEVEL(m_value); }//出力レベル取得
	bool forLog() const { return m_forLog; }//ログレベルとして使用可能か？
	bool forNotice() const { return m_forNotice; }//画面通知レベルとして使用可能か？
	bool forMask() const { return m_forMask; }//出力レベルマスクとして使用可能か？
	const GASHA_ IConsole* console() const { return m_console; }//コンソール
	GASHA_ IConsole* console(){ return m_console; }//コンソール
	const GASHA_ IConsole* consoleForNotice() const { return m_consoleForNotice; }//画面通知用コンソール
	GASHA_ IConsole* consoleForNotice(){ return m_consoleForNotice; }//画面通知用コンソール
	const consoleColor& color() const { return m_color; }//カラー取得
	consoleColor& color(){ return m_color; }//カラー取得
	const consoleColor& colorForNotice() const { return m_colorForNotice; }//カラー取得（画面通知用）
	consoleColor& colorForNotice(){ return m_colorForNotice; }//カラー取得（画面通知用）
public:
	//メソッド
	//コンテナ要素を取得（ショートカット用）
	static const logLevel* at(const level_type value){ return container::staticAt(value); }
	//前のレベルを取得
	logLevel* prev() const
	{
		iterator ite(m_value);
		--ite;
		return ite.m_ref;
	}
	//次のレベルを取得
	logLevel* next() const
	{
		iterator ite(m_value);
		++ite;
		return ite.m_ref;
	}
public:
	//ムーブコンストラクタ
	logLevel(logLevel&& obj) :
		m_name(obj.m_name),
		m_value(obj.m_value),
		m_forLog(obj.m_forLog),
		m_forNotice(obj.m_forNotice),
		m_forMask(obj.m_forMask),
		m_console(obj.m_console),
		m_consoleForNotice(obj.m_consoleForNotice),
		m_color(std::move(obj.m_color)),
		m_colorForNotice(std::move(obj.m_colorForNotice))
	{}
	//コピーコンストラクタ
	logLevel(const logLevel& obj) :
		m_name(obj.m_name),
		m_value(obj.m_value),
		m_forLog(obj.m_forLog),
		m_forNotice(obj.m_forNotice),
		m_forMask(obj.m_forMask),
		m_console(obj.m_console),
		m_consoleForNotice(obj.m_consoleForNotice),
		m_color(obj.m_color),
		m_colorForNotice(obj.m_colorForNotice)
	{}
	//コンストラクタ
	logLevel(const level_type value, const char* name, const bool for_log, const bool for_notice, const bool for_mask, GASHA_ IConsole* console, GASHA_ IConsole* console_for_notice, GASHA_ consoleColor&& color, GASHA_ consoleColor&& color_for_notice) :
		m_name(name),
		m_value(value),
		m_forLog(for_log),
		m_forNotice(for_notice),
		m_forMask(for_mask),
		m_console(console),
		m_consoleForNotice(console_for_notice),
		m_color(std::move(color)),
		m_colorForNotice(std::move(color_for_notice))
	{
		assert(value >= BEGIN && value <= END);
		container::update(m_value, *this);//コンテナに登録
	}
	//コンテナから取得用コンストラクタ
	logLevel(const level_type value) :
		m_name(nullptr),
		m_value(value),
		m_forLog(false),
		m_forNotice(false),
		m_forMask(false),
		m_console(nullptr),
		m_consoleForNotice(nullptr)
	{
		assert(value >= BEGIN && value <= END);
		logLevel* obj = container::staticAt(m_value);//コンテナから取得して自身にコピー
		if (obj)
			*this = *obj;
	}
	//デフォルトコンストラクタ
	logLevel() :
		m_name(nullptr),
		m_value(0),
		m_forLog(false),
		m_forNotice(false),
		m_forMask(false),
		m_console(nullptr),
		m_consoleForNotice(nullptr)
	{
		container::initializeOnce();//コンテナ初期化（一回限り）
	}
	//デストラクタ
	~logLevel()
	{}
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
template<unsigned char V, bool for_log, bool for_notice>
class normalLogLevel : public logLevel
{
public:
	//定数
	static const level_type VALUE = V;//値（レベル）
	static_assert(VALUE >= NORMAL_MIN && VALUE <= NORMAL_MAX, "out of range of logLevel");//値の範囲チェック
	static const bool FOR_LOG = for_log;//ログレベルとして使用可能か？
	static const bool FOR_NOTICE = for_notice;//画面通知レベルとして使用可能か？
	static const bool FOR_MASK = true;//出力レベルマスクとして使用可能か？
public:
	//コンストラクタ
	normalLogLevel(const char* name, GASHA_ consoleColor&& color, GASHA_ consoleColor&& color_for_notice) :
		logLevel(VALUE, name, FOR_LOG, FOR_NOTICE, FOR_MASK, &GASHA_ g_consoleStd, &GASHA_ g_consoleStdForNotice, std::move(color), std::move(color_for_notice))
	{}
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
	specialLogLevel(const char* name) :
		logLevel(VALUE, name, FOR_LOG, FOR_NOTICE, FOR_MASK, &GASHA_ g_consoleStd, &GASHA_ g_consoleStdForNotice, std::move(consoleColor()), std::move(consoleColor()))
	{}
};
//----------------------------------------
//レベル定数
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
//レベル定義
#define DECLARE_LOG_LEVEL(value, for_log, for_notice, fore_color, back_color, fore_color_for_notice, back_color_for_notice) struct level_##value : public normalLogLevel<value, for_log, for_notice>{ level_##value () : normalLogLevel<value, for_log, for_notice>(#value, GASHA_ consoleColor(GASHA_ consoleColor::fore_color, GASHA_ consoleColor::back_color), GASHA_ consoleColor(GASHA_ consoleColor::fore_color_for_notice, GASHA_ consoleColor::back_color_for_notice)){} }
#define DECLARE_SPECIAL_LOG_LEVEL(value) struct level_##value : public specialLogLevel<value>{ level_##value () : specialLogLevel<value>(#value){} }
//※以下、ヘッダーで公開する必要なし
DECLARE_LOG_LEVEL(asNormal, true, true, STANDARD, STANDARD, BLACK, iWHITE);//通常メッセージ
DECLARE_LOG_LEVEL(asVerbose, true, false, iBLACK, STANDARD, iBLACK, iWHITE);//冗長メッセージ
DECLARE_LOG_LEVEL(asDetail, true, false, iBLACK, STANDARD, iBLACK, iWHITE);//詳細メッセージ
DECLARE_LOG_LEVEL(asImportant, true, true, iBLUE, STANDARD, iBLUE, iWHITE);//重要メッセージ
DECLARE_LOG_LEVEL(asWarning, true, true, iMAGENTA, STANDARD, BLACK, iMAGENTA);//警告メッセージ
DECLARE_LOG_LEVEL(asCritical, true, true, iRED, STANDARD, iYELLOW, iRED);//重大メッセージ
DECLARE_LOG_LEVEL(asAbsolute, true, false, STANDARD, STANDARD, STANDARD, STANDARD);//絶対メッセージ（ログレベルに関係なく出力したいメッセージ）
//以下、ログレベル／画面通知レベル変更用
DECLARE_SPECIAL_LOG_LEVEL(asSilent);//静寂（絶対メッセ―ジ以外出力しない）
DECLARE_SPECIAL_LOG_LEVEL(asSilentAbsolutely);//絶対静寂（全てのメッセージを出力しない）
//----------------------------------------
//レベルコンテナの静的変数をインスタンス化
bool logLevel::container::m_isInitialized = false;
logLevel* logLevel::container::m_poolPtr = nullptr;
logLevel::byte logLevel::container::m_pool[(POOL_NUM)* sizeof(logLevel)];
std::bitset<logLevel::POOL_NUM> logLevel::container::m_isAlreadyPool;
//----------------------------------------
//レベルコンテナ初期化（一回限り）
void logLevel::container::initializeOnce()
{
	//初期化済みチェック
	if (m_isInitialized)
		return;
	//静的変数を初期化
	m_isInitialized = true;
	m_isAlreadyPool.reset();
	memset(m_pool, 0, sizeof(m_pool));
	m_poolPtr = reinterpret_cast<logLevel*>(m_pool);
	//要素を初期化
	for (logLevel::level_type value = 0; value < logLevel::NUM; ++value)
	{
		logLevel(value, "(undefined)", false, false, false, nullptr, nullptr, GASHA_ consoleColor(), GASHA_ consoleColor());
		m_isAlreadyPool[value] = false;
	}
	//割り当て済みレベルを設定（コンストラクタで要素を登録）
	level_asNormal();//通常メッセージ
	level_asVerbose();//冗長メッセージ
	level_asDetail();//詳細メッセージ
	level_asImportant();//重要メッセージ
	level_asWarning();//警告メッセージ
	level_asCritical();//重大メッセージ
	level_asAbsolute();//絶対メッセージ（ログレベルに関係なく出力したいメッセージ）
	level_asSilent();//静寂（絶対メッセ―ジ以外出力しない）
	level_asSilentAbsolutely();//絶対静寂（全てのメッセージを出力しない）
}
//----------------------------------------
//レベル用変数
static logLevel s_levelForInitialize;//初期化処理実行のためのインスタンス

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_level.inl>

#endif//GASHA_INCLUDED_LOG_LEVEL_H

// End of file

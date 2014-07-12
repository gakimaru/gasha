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

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログレベル
//--------------------------------------------------------------------------------

//----------------------------------------
//レベルクラス
class level
{
public:
	//型
	typedef unsigned char value_t;//値（レベル）
	typedef unsigned char byte;//バッファ用
private:
	//定数（計算用）
#define calcAsOutput(value) ((value) >> 1)//値を出力レベルに変換
#define calcAsValue(value) ((value) << 1)//出力レベルを値に変換
public:
	//定数
	static const value_t NORMAL_NUM = 11;//通常レベル数
	static const value_t SPECIAL_NUM = 2;//特殊レベル数
	static const value_t NUM = NORMAL_NUM + SPECIAL_NUM;//レベル総数
	static const value_t MIN = 0;//レベル最小値
	static const value_t MAX = NUM - 1;//レベル最大値
	static const value_t NORMAL_MIN = MIN;//通常レベル最小値
	static const value_t NORMAL_MAX = NORMAL_MIN + NORMAL_NUM - 1;//通常レベル最大値
	static const value_t SPECIAL_MIN = NORMAL_MAX + 1;//特殊レベル最小値
	static const value_t SPECIAL_MAX = SPECIAL_MIN + SPECIAL_NUM - 1;//特殊レベル最大値
	static const value_t BEGIN = MIN;//レベル開始値（イテレータ用）
	static const value_t END = NUM;//レベル終端値（イテレータ用）
	static const value_t POOL_NUM = NUM + 1;//レベル記録数
	static const value_t OUTPUT_LEVEL_MIN = calcAsOutput(NORMAL_MIN);//出力レベル最小値
	static const value_t OUTPUT_LEVEL_MAX = calcAsOutput(NORMAL_MAX);//出力レベル最大値
	static_assert(SPECIAL_MAX == MAX, "invalid category numbers.");//定数チェック
public:
	//型
	//--------------------
	//イテレータ
	class iterator : public std::iterator<std::bidirectional_iterator_tag, level>//双方向イテレータとして実装
	{
	public:
		//オペレータ
		const level* operator->() const { return &container::get(m_value); }
		const level& operator*() const { return container::get(m_value); }
		bool operator==(const iterator rhs) const { return m_value == rhs.m_value; }
		bool operator!=(const iterator rhs) const { return m_value != rhs.m_value; }
		const level& operator++() const { return m_value >= container::endValue() ? container::getEnd() : container::get(++m_value); }
		const level& operator++(int) const { return m_value >= container::endValue() ? container::getEnd() : container::get(m_value++); }
		const level& operator--() const { if (m_value == container::beginValue()){ m_value = container::endValue(); return container::getEnd(); } return container::get(--m_value); }
		const level& operator--(int) const { if (m_value == container::beginValue()){ m_value = container::endValue(); return container::getBegin(); } return container::get(m_value--); }
	public:
		//キャストオペレータ
		operator const level&() const { return container::get(m_value); }//値（レベル）
	public:
		//コンストラクタ
		iterator(const value_t value) :
			m_value(value)
		{}
		iterator(const value_t value, int) :
			m_value(container::endValue())
		{}
		iterator(const level& obj) :
			m_value(obj.value())
		{}
		iterator(const level& obj, int) :
			m_value(container::endValue())
		{}
		iterator() :
			m_value(container::endValue())
		{}
		//デストラクタ
		~iterator()
		{}
	private:
		//フィールド
		value_t mutable m_value;//値（レベル）
	};
	//--------------------
	//constイテレータ
	typedef const iterator const_iterator;
	//--------------------
	//コンテナ（イテレータ用）
	class container
	{
		friend class level;
	public:
		//メソッド
		static const value_t beginValue(){ return BEGIN; }//開始値取得
		static const value_t endValue(){ return END; }//終端値取得
		static const level& get(const value_t value){ return m_poolPtr[value]; }//要素を取得
		static const level& getBegin(){ return m_poolPtr[beginValue()]; }//開始要素を取得
		static const level& getEnd(){ return m_poolPtr[endValue()]; }//終端要素を取得
	private:
		static void set(const value_t value, const level& obj)//要素を更新
		{
			if (!container::m_isAlreadyPool[value])
			{
				container::m_poolPtr[value] = obj;
				container::m_isAlreadyPool[value] = true;
			}
		}
	public:
		static const iterator begin(){ return iterator(beginValue()); }//開始イテレータを取得
		static const iterator end()	{ return iterator(endValue()); }//終端イテレータを取得
		static const_iterator cbegin(){ return const_iterator(beginValue()); }//開始constイテレータを取得
		static const_iterator cend(){ return const_iterator(endValue()); }//終端constイテレータを取得
		//※reverse_iterator非対応
		//メソッド
		//初期化（一回限り）
		static void initializeOnce();
	private:
		//フィールド
		static bool m_isInitialized;//初期化済み
		static level* m_poolPtr;//要素のプール（ポインタ）
		static byte m_pool[];//要素のプール（バッファ）※バッファとポインタを分けているのは、コンストラクタの実行を防止するため
		static std::bitset<POOL_NUM> m_isAlreadyPool;//要素の初期化済みフラグ
	};
public:
	//オペレータ
	//※出力レベルで比較する
	bool operator ==(const level& rhs) const { return valueAsOutput() == rhs.valueAsOutput(); }
	bool operator !=(const level& rhs) const { return valueAsOutput() != rhs.valueAsOutput(); }
	bool operator >(const level& rhs) const { return valueAsOutput() > rhs.valueAsOutput(); }
	bool operator >=(const level& rhs) const { return valueAsOutput() >= rhs.valueAsOutput(); }
	bool operator <(const level& rhs) const { return valueAsOutput() < rhs.valueAsOutput(); }
	bool operator <=(const level& rhs) const { return valueAsOutput() <= rhs.valueAsOutput(); }
private:
	//コピーオペレータ
	level& operator=(const level& rhs)
	{
		memcpy(this, &rhs, sizeof(*this));
		return *this;
	}
public:
	//キャストオペレータ
	operator int() const { return static_cast<int>(m_value); }//値（レベル）
	operator value_t() const { return m_value; }//値（レベル）
	operator const char*() const { return m_name; }//名前
public:
	//アクセッサ
	value_t value() const { return m_value; }//値（レベル）取得
	const char* name() const { return m_name; }//名前取得
	static value_t calcValueAsOutput(const value_t value){ return calcAsOutput(value); }//出力レベル計算
	value_t valueAsOutput() const { return calcAsOutput(m_value); }//出力レベル取得
	bool forLog() const { return m_forLog; }//ログレベルとして使用可能か？
	bool forNotice() const { return m_forNotice; }//画面通知レベルとして使用可能か？
	bool forMask() const { return m_forMask; }//出力レベルマスクとして使用可能か？
	color getColor() const { return std::move(changeColor(true)); }//カラー取得
	color getColorForNotice() const { return std::move(changeColorForNotice(true)); }//カラー取得（画面通知用）
public:
	//メソッド
	//カラー変更
	//※戻り値受け取り必須
	//　戻り値を受け取らないとすぐにデストラクタが実行されて元のカラーに戻るので実際に反映されない
	color changeColor(const bool is_only_get = false) const
	{
		return std::move(color(color::stdOut, m_foreColor, m_backColor, !is_only_get));
	}
	//カラー変更（画面通知用）
	//※戻り値受け取り必須
	//　戻り値を受け取らないとすぐにデストラクタが実行されて元のカラーに戻るので実際に反映されない
	color changeColorForNotice(const bool is_only_get = false) const
	{
		return std::move(color(color::stdErr, m_foreColorForNotice, m_backColorForNotice, !is_only_get));
	}
	//コンテナ要素を取得（ショートカット用）
	static const level& get(const value_t value){ return container::get(value); }
	//前のレベルを取得
	const level& prev() const
	{
		if (valueAsOutput() <= OUTPUT_LEVEL_MIN || valueAsOutput() > OUTPUT_LEVEL_MAX || !(m_forLog || m_forNotice))
			return container::m_poolPtr[m_value];
		return container::m_poolPtr[calcAsValue(calcAsOutput(m_value) - 1)];
	}
	//次のレベルを取得
	const level& next() const
	{
		if (valueAsOutput() < OUTPUT_LEVEL_MIN || valueAsOutput() >= OUTPUT_LEVEL_MAX || !(m_forLog || m_forNotice))
			return container::m_poolPtr[m_value];
		return container::m_poolPtr[calcAsValue(calcAsOutput(m_value) + 1)];
	}
public:
	//デフォルトコンストラクタ
	level() :
		m_name(nullptr),
		m_value(UCHAR_MAX),
		m_forLog(false),
		m_forNotice(false),
		m_forMask(false),
		m_foreColor(color::through),
		m_backColor(color::through),
		m_foreColorForNotice(color::through),
		m_backColorForNotice(color::through)
	{
		container::initializeOnce();//コンテナ初期化（一回限り）
	}
	//コピーコンストラクタ
	level(const level& src) :
		m_name(src.m_name),
		m_value(src.m_value),
		m_forLog(src.m_forLog),
		m_forNotice(src.m_forNotice),
		m_forMask(src.m_forMask),
		m_foreColor(src.m_foreColor),
		m_backColor(src.m_backColor),
		m_foreColorForNotice(src.m_foreColorForNotice),
		m_backColorForNotice(src.m_backColorForNotice)
	{
	}
	//コンストラクタ
	level(const value_t value, const char* name, const bool for_log, const bool for_notice, const bool for_mask, const color::color_t fore_color, const color::color_t back_color, const color::color_t fore_color_for_notice, const color::color_t back_color_for_notice) :
		m_name(name),
		m_value(value),
		m_forLog(for_log),
		m_forNotice(for_notice),
		m_forMask(for_mask),
		m_foreColor(fore_color),
		m_backColor(back_color),
		m_foreColorForNotice(fore_color_for_notice),
		m_backColorForNotice(back_color_for_notice)
	{
		assert(value >= BEGIN && value <= END);
		container::set(m_value, *this);//コンテナに登録
	}
	level(const value_t value) :
		m_name(nullptr),
		m_value(value),
		m_forLog(false),
		m_forNotice(false),
		m_forMask(false),
		m_foreColor(color::through),
		m_backColor(color::through),
		m_foreColorForNotice(color::through),
		m_backColorForNotice(color::through)
	{
		assert(value >= BEGIN && value <= END);
		*this = container::get(m_value);//コンテナから取得して自身にコピー
	}
	//デストラクタ
	~level()
	{}
private:
	//フィールド
	const char* m_name;//名前
	const value_t m_value;//値（レベル）
	const bool m_forLog;//ログ出力レベルとして使用可能か？
	const bool m_forNotice;//画面通知レベルとして使用可能か？
	const bool m_forMask;//出力レベルマスクとして使用可能か？
	const color::color_t m_foreColor;//カラー：前面
	const color::color_t m_backColor;//カラー：背面
	const color::color_t m_foreColorForNotice;//カラー：前面（画面通知用）
	const color::color_t m_backColorForNotice;//カラー：背面（画面通知用）
private:
	//マクロ消去
#undef calcAsOutput
#undef calcAsValue
};
//----------------------------------------
	//レベル定義用テンプレートクラス：通常レベル用
	template<unsigned char V, bool for_log, bool for_notice, color::color_t fore_color, color::color_t back_color, color::color_t fore_color_for_notice, color::color_t back_color_for_notice>
	class level_normal : public level
	{
	public:
		//定数
		static const value_t VALUE = V;//値（レベル）
		static_assert(VALUE >= NORMAL_MIN && VALUE <= NORMAL_MAX, "out of range of level");//値の範囲チェック
		static const bool FOR_LOG = for_log;//ログレベルとして使用可能か？
		static const bool FOR_NOTICE = for_notice;//画面通知レベルとして使用可能か？
		static const bool FOR_MASK = true;//出力レベルマスクとして使用可能か？
		static const color::color_t FORE_COLOR = fore_color;//カラー：前面
		static const color::color_t BACK_COLOR = back_color;//カラー：背面
		static const color::color_t FORE_COLOR_FOR_NOTICE = fore_color_for_notice;//カラー：前面（画面通知用）
		static const color::color_t BACK_COLOR_FOR_NOTICE = back_color_for_notice;//カラー：背面（画面通知用）
	public:
		//コンストラクタ
		level_normal(const char* name) :
			level(VALUE, name, FOR_LOG, FOR_NOTICE, FOR_MASK, FORE_COLOR, BACK_COLOR, FORE_COLOR_FOR_NOTICE, BACK_COLOR_FOR_NOTICE)
		{}
	};
	//----------------------------------------
	//レベル定義用テンプレートクラス：特殊レベル用
	template<unsigned char V>
	class level_special : public level
	{
	public:
		//定数
		static const value_t VALUE = V;//値（レベル）
		static_assert(VALUE >= SPECIAL_MIN && VALUE <= SPECIAL_MAX, "out of range of level");//値の範囲チェック
		static const bool FOR_LOG = false;//ログレベルとして使用可能か？
		static const bool FOR_NOTICE = false;//画面通知レベルとして使用可能か？
		static const bool FOR_MASK = true;//出力レベルマスクとして使用可能か？
	public:
		//コンストラクタ
		level_special(const char* name) :
			level(VALUE, name, FOR_LOG, FOR_NOTICE, FOR_MASK, color::through, color::through, color::through, color::through)
		{}
	};
	//----------------------------------------
	//レベル定義クラス：終端用
	class level_end : public level
	{
	public:
		//定数
		static const value_t VALUE = END;//値（レベル）
		static const bool FOR_LOG = false;//ログレベルとして使用可能か？
		static const bool FOR_NOTICE = false;//画面通知レベルとして使用可能か？
		static const bool FOR_MASK = false;//出力レベルマスクとして使用可能か？
	public:
		//コンストラクタ
		level_end() :
			level(VALUE, "(END)", FOR_LOG, FOR_NOTICE, FOR_MASK, color::through, color::through, color::through, color::through)
		{}
	};
	//----------------------------------------
	//レベル定数
#define define_normalLevel(print_level, sub) (level::NORMAL_MIN + print_level * 2 + sub)
#define define_specialLevel(value) (level::SPECIAL_MIN + value)
	enum levelEnum : level::value_t
	{
		asNormal = define_normalLevel(1, 0),//通常メッセージ
		asVerbose = define_normalLevel(0, 0),//冗長メッセージ
		asDetail = define_normalLevel(0, 1),//詳細メッセージ
		asImportant = define_normalLevel(2, 0),//重要メッセージ
		asWarning = define_normalLevel(3, 0),//警告メッセージ
		asCritical = define_normalLevel(4, 0),//重大メッセージ
		asAbsolute = define_normalLevel(5, 0),//絶対メッセージ（ログレベルに関係なく出力したいメッセージ）
		//以下、ログレベル／画面通知レベル変更用
		asSilent = define_specialLevel(0),//絶対メッセージ（ログレベルに関係なく出力したいメッセージ）
		asSilentAbsolutely = define_specialLevel(1),//絶対静寂（全てのメッセージを出力しない）
	};
	//----------------------------------------
	//レベル定義
#define declare_normalLevel(value, for_log, for_notice, fore_color, back_color, fore_color_for_notice, back_color_for_notice) struct level_##value : public level_normal<value, for_log, for_notice, fore_color, back_color, fore_color_for_notice, back_color_for_notice>{ level_##value () :level_normal<value, for_log, for_notice, fore_color, back_color, fore_color_for_notice, back_color_for_notice>(#value){} }
#define declare_specialLevel(value) struct level_##value : public level_special<value>{ level_##value () :level_special<value>(#value){} }
	//※以下、ヘッダーで公開する必要なし
	declare_normalLevel(asNormal, true, true, color::reset, color::reset, color::black, color::iWhite);//通常メッセージ
	declare_normalLevel(asVerbose, true, false, color::iBlack, color::black, color::iBlack, color::iWhite);//冗長メッセージ
	declare_normalLevel(asDetail, true, false, color::iBlack, color::black, color::iBlack, color::iWhite);//詳細メッセージ
	declare_normalLevel(asImportant, true, true, color::iBlue, color::black, color::iBlue, color::iWhite);//重要メッセージ
	declare_normalLevel(asWarning, true, true, color::iMagenta, color::black, color::black, color::iMagenta);//警告メッセージ
	declare_normalLevel(asCritical, true, true, color::iRed, color::black, color::iYellow, color::iRed);//重大メッセージ
	declare_normalLevel(asAbsolute, true, false, color::through, color::through, color::through, color::through);//絶対メッセージ（ログレベルに関係なく出力したいメッセージ）
	//以下、ログレベル／画面通知レベル変更用
	declare_specialLevel(asSilent);//静寂（絶対メッセ―ジ以外出力しない）
	declare_specialLevel(asSilentAbsolutely);//絶対静寂（全てのメッセージを出力しない）
	//----------------------------------------
	//レベルコンテナの静的変数をインスタンス化
	bool level::container::m_isInitialized = false;
	level* level::container::m_poolPtr = nullptr;
	level::byte level::container::m_pool[(POOL_NUM)* sizeof(level)];
	std::bitset<level::POOL_NUM> level::container::m_isAlreadyPool;
	//----------------------------------------
	//レベルコンテナ初期化（一回限り）
	void level::container::initializeOnce()
	{
		//初期化済みチェック
		if (m_isInitialized)
			return;
		//静的変数を初期化
		m_isInitialized = true;
		m_isAlreadyPool.reset();
		memset(m_pool, 0, sizeof(m_pool));
		m_poolPtr = reinterpret_cast<level*>(m_pool);
		//要素を初期化
		for (level::value_t value = 0; value < level::NUM; ++value)
		{
			level(value, "(undefined)", false, false, false, color::through, color::through, color::through, color::through);
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
		level_end();//終端
	}
	//----------------------------------------
	//レベル用変数
	static level s_levelForInitialize;//初期化処理実行のためのインスタンス
	//----------------------------------------
	//レベルコンテナ列挙
	void printLevelAll()
	{
		for (auto& obj : level::container())//C++11スタイル
		//for (auto ite = level::container::begin(); ite != level::container::end(); ++ite)//旧来のスタイル
		//for (auto ite = level::container::cbegin(); ite != level::container::cend(); ++ite)//旧来のスタイル
		{
			//const level& obj = ite;//イテレータを変換（イテレータのままでもアロー演算子で直接値操作可能）
			color col(obj.changeColor());
			fprintf(stdout, "level=%d, name=\"%s\", valueAsOutput=%d, forLog=%d, forNotice=%d, forMask=%d\n", obj.value(), obj.name(), obj.valueAsOutput(), obj.forLog(), obj.forNotice(), obj.forMask());
			auto& prev = obj.prev();
			auto& next = obj.next();
			fprintf(stdout, "           prev=%s(%d)\n", prev.name(), prev.value());
			fprintf(stdout, "           next=%s(%d)\n", next.name(), next.value());
		}
	}

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_level.inl>

#endif//GASHA_INCLUDED_LOG_LEVEL_H

// End of file

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

#include <gasha/log_purpose.h>//ログ用途
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
	typedef GASHA_ logPurpose::purpose_type purpose_type;//ログ用途の値
	typedef unsigned char level_type;//ログレベルの値
public:
	//定数
	static const purpose_type PURPOSE_NUM = GASHA_ logPurpose::NUM;
private:
	//ログレベル情報
	struct info
	{
		const char* m_name;//名前
		level_type m_value;//レベルの値
		GASHA_ IConsole* m_consoles[PURPOSE_NUM];//出力先コンソール
		GASHA_ consoleColor m_colors[PURPOSE_NUM];//カラー
	};
public:
	//定数計算用マクロ／関数
	#define TO_OUTPUT_LEVEL(value) ((value) >> 1)//値を出力レベルに変換
	#define FROM_OUTPUT_LEVEL(value) ((value) << 1)//出力レベルを値に変換
	inline static constexpr level_type toOutputLevel(const level_type value){ return TO_OUTPUT_LEVEL(value); }//出力レベル計算
	inline static constexpr level_type fromOutputLevel(const level_type value){ return FROM_OUTPUT_LEVEL(value); }//出力レベルからレベル計算
public:
	//定数
	static const level_type NORMAL_NUM = 11;//通常ログレベル数 ※奇数である点に注意：次の値（最初の特殊ログレベル）は、「出力ログレベル」が通常ログレベルの最大と一致する
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
	inline bool operator==(const logLevel& rhs) const { return outputLevel() == rhs.outputLevel(); }
	inline bool operator!=(const logLevel& rhs) const { return outputLevel() != rhs.outputLevel(); }
	inline bool operator>(const logLevel& rhs) const { return outputLevel() > rhs.outputLevel(); }
	inline bool operator>=(const logLevel& rhs) const { return outputLevel() >= rhs.outputLevel(); }
	inline bool operator<(const logLevel& rhs) const { return outputLevel() < rhs.outputLevel(); }
	inline bool operator<=(const logLevel& rhs) const { return outputLevel() <= rhs.outputLevel(); }
public:
	//キャストオペレータ
	inline operator bool() const { return isExist(); }//正しいログレベルか？
	inline operator int() const { return static_cast<int>(m_info->m_value); }//レベルの値
	inline operator level_type() const { return m_info->m_value; }//レベルの値
	inline operator const char*() const { return m_info->m_name; }//名前
public:
	//アクセッサ
	inline bool isExist() const { return m_info != nullptr; }//正しいログレベルか？
	inline bool isSpecial() const { return m_info->m_value >= SPECIAL_MIN && m_info->m_value <= SPECIAL_MAX; }//特殊レベルか？
	inline level_type value() const { return m_info->m_value; }//レベルの値取得
	inline const char* name() const { return m_info->m_name; }//名前取得
	inline level_type outputLevel() const { return toOutputLevel(m_info->m_value); }//出力レベル取得
	inline const GASHA_ IConsole* console(const purpose_type purpose) const { return m_info->m_consoles[purpose]; }//コンソール
	inline GASHA_ IConsole*& console(const purpose_type purpose){ return m_info->m_consoles[purpose]; }//コンソール
	inline const consoleColor& color(const purpose_type purpose) const { return m_info->m_colors[purpose]; }//カラー取得
	inline consoleColor& color(const purpose_type purpose){ return m_info->m_colors[purpose]; }//カラー取得
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
	//型
	struct explicitInitialize_t{};//明示的な初期化用構造体
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
		inline bool isSpecial() const { return m_logLevel.isSpecial(); }
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
		inline operator logLevel::level_type() const { return m_value; }
		inline operator const logLevel*() const { return &m_logLevel; }//レベルの値
		inline operator logLevel*(){ return &m_logLevel; }//レベルの値
		inline operator const logLevel&() const { return m_logLevel; }//レベルの値
		inline operator logLevel&(){ return m_logLevel; }//レベルの値
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
		mutable logLevel::level_type m_value;//レベルの値
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
		inline bool isSpecial() const { return m_logLevel.isSpecial(); }
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
		inline operator logLevel::level_type() const { return m_value == logLevel::INVALID ? logLevel::INVALID : m_logLevel.m_info ? m_logLevel.m_info->m_value : logLevel::END; }
		inline operator const logLevel*() const { return &m_logLevel; }//レベルの値
		inline operator logLevel*(){ return &m_logLevel; }//レベルの値
		inline operator const logLevel&() const { return m_logLevel; }//レベルの値
		inline operator logLevel&(){ return m_logLevel; }//レベルの値
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
		mutable logLevel::level_type m_value;//レベルの値
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
public:
	//全てのログレベルのコンソール／画面通知用コンソールを変更
	//※nullptrが設定されているものは変更しない
	void replaceEachConsole(const logLevel::purpose_type purpose, IConsole* new_console);
	void replaceEachConsole(const logLevel::purpose_type purpose, IConsole* src_console, IConsole* new_console);//置き換え元のコンソールを指定する場合
private:
	//初期化メソッド（一回限り）
	static void initializeOnce();
public:
	//明示的な初期化用コンストラクタ
	inline logLevelContainer(const explicitInitialize_t&);
	//デフォルトコンストラクタ
	inline logLevelContainer();
	//デストラクタ
	inline ~logLevelContainer();
public:
	//静的フィールド
	static const explicitInitialize_t explicitInitialize;//明示的な初期化指定用
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
	static const logLevel::level_type LEVEL = _LEVEL;//レベルの値
	static_assert(LEVEL >= logLevel::NORMAL_MIN && LEVEL <= logLevel::NORMAL_MAX, "Out of range of normal-log-level");//値の範囲チェック
public:
	//関数オペレータ
	inline bool operator()(const char* name, IConsole* (&&console)[logLevel::PURPOSE_NUM], GASHA_ consoleColor (&&color)[logLevel::PURPOSE_NUM]);
};

//----------------------------------------
//ログレベル登録用補助マクロ
#define MAKE_LOG_LEVEL_VALUE(OUTPUT_LEVEL, SUB_VALIE) (logLevel::NORMAL_MIN + OUTPUT_LEVEL * 2 + SUB_VALIE)//ログレベル定数計算用マクロ
#define MAKE_SPECIAL_LOG_LEVEL_VALUE(VALUE) (logLevel::SPECIAL_MIN + VALUE)//特殊ログレベル定数計算用マクロ

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_level.inl>

//利便性のためにいっしょにインクルード
#include <gasha/default_log_level.h>//既定のログレベル

#endif//GASHA_INCLUDED_LOG_LEVEL_H

// End of file

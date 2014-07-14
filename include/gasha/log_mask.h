#pragma once
#ifndef GASHA_INCLUDED_LOG_MASK_H
#define GASHA_INCLUDED_LOG_MASK_H

//--------------------------------------------------------------------------------
// log_mask.h
// ログマスク【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_common.h>//ログ共通設定

#include <gasha/log_level.h>//ログレベル
#include <gasha/log_category.h>//ログカテゴリ
#include <gasha/i_console.h>//コンソールインターフェース
#include <gasha/console_color.h>//コンソールカラー

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <ioterator> <mutex> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <iterator>//std::iterator
#include <mutex>//C++11 std::call_once

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログマスク
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ログマスク
//※コンテナ
class logMask
{
public:
	//型
	typedef GASHA_ logLevel::level_type level_type;//ログレベル
	typedef GASHA_ logCategory::category_type category_type;//ログカテゴリ
	struct explicitInitialize_t{};//明示的な初期化用構造体
public:
	//定数
	enum puroise_type : unsigned char
	{
		PURPUSE_OF_LOG = 0,//用途：ログ出力
		PURPUSE_OF_NOTICE = 1,//用途：画面通知
		PURPOSE_NUM = 2//用途数
	};
	static const level_type LEVEL_MIN = GASHA_ logLevel::NORMAL_MIN;//ログレベル最小値
	static const level_type LEVEL_MAX = GASHA_ logLevel::SPECIAL_MAX;//ログレベル最大値
	static const category_type CATEGORY_MIN = GASHA_ logCategory::NORMAL_MIN;//ログカテゴリ最小値
	static const category_type CATEGORY_MAX = GASHA_ logCategory::NORMAL_MAX;//ログカテゴリ最大値
	static const category_type CATEGORY_NUM = GASHA_ logCategory::NORMAL_NUM;//ログカテゴリ数
	static const category_type CATEGORY_FOR_EVERY = GASHA_ logCategory::FOR_EVERY;//全体変更用ログカテゴリ
	static const level_type DEFAULT_LOG_MASK_OF_LOG = GASHA_DEFAULT_LOG_MASK_OF_LOG;//デフォルトのログマスク
	static const level_type DEFAULT_LOG_MASK_OF_NOTICE = GASHA_DEFAULT_LOG_MASK_OF_NOTICE;//デフォルトの画面通知ログマスク
public:
	//--------------------
	//イテレータ宣言
	class iterator;
	class reverse_iterator;
	typedef const iterator const_iterator;
	typedef const reverse_iterator const_reverse_iterator;
	//--------------------
	//イテレータ
	class iterator : public std::iterator<std::bidirectional_iterator_tag, GASHA_ logCategory>//双方向イテレータとして実装
	{
	public:
		//オペレータ
		inline bool isExist() const { return m_logCategory.isExist(); }
		inline const iterator* operator->() const { return this; }
		inline iterator* operator->(){ return this; }
		inline const iterator& operator*() const { return *this; }
		inline iterator& operator*(){ return *this; }
		inline bool operator==(const iterator& rhs) const { return m_logMask == rhs.m_logMask && m_logCategory == rhs.m_logCategory; }
		inline bool operator!=(const iterator& rhs) const { return m_logMask == rhs.m_logMask && m_logCategory != rhs.m_logCategory; }
		inline const iterator& operator++() const { inc(); return *this; }
		inline iterator& operator++(){ inc(); return *this; }
		inline iterator operator++(int) const { iterator ite(m_logMask, m_logCategory); inc(); return ite; }
		inline const iterator& operator--() const { dec(); return *this; }
		inline iterator& operator--(){ dec(); return *this; }
		inline iterator operator--(int) const { iterator ite(m_logMask, m_logCategory); dec(); return ite; }
	private:
		//インクリメント
		void inc() const;
		//デクリメント
		void dec() const;
	public:
		//キャストオペレータ
		inline operator bool() const { return isExist(); }
		inline operator category_type() const { return static_cast<GASHA_ logCategory::category_type>(m_logCategory); }
		inline operator const GASHA_ logCategory*() const { return static_cast<const GASHA_ logCategory*>(m_logCategory); }//値（カテゴリ）
		inline operator const GASHA_ logCategory&() const { return m_logCategory; }//値（カテゴリ）
	public:
		//アクセッサ
		inline const GASHA_ logCategory& category() const { return m_logCategory; }//値（カテゴリ）
		inline const GASHA_ logLevel levelMask(const GASHA_ logPurpose purpose) const { return logLevel(m_logMask->levelMask(purpose, m_logCategory)); }//ログレベルマスク
		inline const GASHA_ IConsole* console(const GASHA_ logPurpose purpose, const level_type level) const;//コンソール取得
		inline GASHA_ IConsole* console(const GASHA_ logPurpose purpose, const level_type level);//コンソール取得
		inline const GASHA_ consoleColor* color(const GASHA_ logPurpose purpose, const level_type level) const;//コンソールカラー取得
	public:
		//メソッド
		void changeLevel(const GASHA_ logPurpose purpose, const level_type level);//ログレベルを変更
		void upLevel(const GASHA_ logPurpose purpose);//ログレベルを一レベル上に変更
		void downLevel(const GASHA_ logPurpose purpose);//ログレベルを一レベル下に変更
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
		iterator(const logMask* mask, const category_type category);
		iterator(const logMask* mask, const GASHA_ logCategoryContainer::iterator& category);
		//デフォルトコンストラクタ
		inline iterator();
		//デストラクタ
		inline ~iterator();
	private:
		//フィールド
		mutable logMask* m_logMask;//ログマスク
		mutable GASHA_ logCategoryContainer::iterator m_logCategory;//ログカテゴリのイテレータ
	};
	//--------------------
	//リバースイテレータ
	class reverse_iterator : public std::iterator<std::bidirectional_iterator_tag, GASHA_ logCategory>//双方向イテレータとして実装
	{
	public:
		//オペレータ
		inline bool isExist() const { return m_logCategory.isExist(); }
		inline const reverse_iterator* operator->() const { return this; }
		inline reverse_iterator* operator->(){ return this; }
		inline const reverse_iterator& operator*() const { return *this; }
		inline reverse_iterator& operator*(){ return *this; }
		inline bool operator==(const reverse_iterator& rhs) const { return m_logMask == rhs.m_logMask && m_logCategory == rhs.m_logCategory; }
		inline bool operator!=(const reverse_iterator& rhs) const { return m_logMask == rhs.m_logMask && m_logCategory != rhs.m_logCategory; }
		inline const reverse_iterator& operator++() const { inc(); return *this; }
		inline reverse_iterator& operator++(){ inc(); return *this; }
		inline reverse_iterator operator++(int) const { reverse_iterator ite(m_logMask, m_logCategory); inc(); return ite; }
		inline const reverse_iterator& operator--() const { dec(); return *this; }
		inline reverse_iterator& operator--(){ dec(); return *this; }
		inline reverse_iterator operator--(int) const { reverse_iterator ite(m_logMask, m_logCategory); dec(); return ite; }
	private:
		//インクリメント
		void inc() const;
		//デクリメント
		void dec() const;
	public:
		//キャストオペレータ
		inline operator bool() const { return isExist(); }
		inline operator category_type() const { return static_cast<GASHA_ logCategory::category_type>(m_logCategory); }
		inline operator const GASHA_ logCategory*() const { return static_cast<const GASHA_ logCategory*>(m_logCategory); }//値（カテゴリ）
		inline operator const GASHA_ logCategory&() const { return m_logCategory; }//値（カテゴリ）
	public:
		//アクセッサ
		inline const GASHA_ logCategory& category() const { return m_logCategory; }//値（カテゴリ）
		inline const GASHA_ logLevel levelMask(const GASHA_ logPurpose purpose) const { return m_logMask->levelMask(purpose, m_logCategory); }//ログレベルマスク
		inline const GASHA_ IConsole* console(const GASHA_ logPurpose purpose, const level_type level) const;//コンソール取得
		inline GASHA_ IConsole* console(const GASHA_ logPurpose purpose, const level_type level);//コンソール取得
		inline const GASHA_ consoleColor* color(const GASHA_ logPurpose purpose, const level_type level) const;//コンソールカラー取得
	public:
		//メソッド
		void changeLevel(const GASHA_ logPurpose purpose, const level_type level);//ログレベルを変更
		void upLevel(const GASHA_ logPurpose purpose);//ログレベルを一レベル上に変更
		void downLevel(const GASHA_ logPurpose purpose);//ログレベルを一レベル下に変更
	public:
		//ムーブオペレータ
		inline reverse_iterator& operator=(reverse_iterator&& rhs);
		//コピーオペレータ
		inline reverse_iterator& operator=(const reverse_iterator& rhs);
	public:
		//ベースイテレータを取得
		inline logMask::iterator base() const;
	public:
		//ムーブコンストラクタ
		inline reverse_iterator(reverse_iterator&& ite);
		//コピーコンストラクタ
		inline reverse_iterator(const reverse_iterator& ite);
		//コンストラクタ
		reverse_iterator(const logMask* mask, const category_type category);
		reverse_iterator(const logMask* mask, const GASHA_ logCategoryContainer::reverse_iterator& category);
		//デフォルトコンストラクタ
		inline reverse_iterator();
		//デストラクタ
		inline ~reverse_iterator();
	private:
		//フィールド
		mutable logMask* m_logMask;//ログマスク
		mutable GASHA_ logCategoryContainer::reverse_iterator m_logCategory;//ログカテゴリのリバースイテレータ
	};
public:
	//アクセッサ
	inline level_type levelMask(const GASHA_ logPurpose purpose, const category_type category) const;//ログレベルマスクを取得
	inline bool isEnableLevel(const GASHA_ logPurpose purpose, const category_type category, const GASHA_ logLevel& require_level) const;//出力可能なログレベルか？
	inline bool isEnableLevel(const GASHA_ logPurpose purpose, const category_type category, const level_type require_level) const;//出力可能なログレベルか？
	bool isLocal() const{ return m_localLogLevelIsEnabled; }//ローカルログレベルが有効か？
	bool isGlobal() const{ return !m_localLogLevelIsEnabled; }//グローバルログレベルが有効か？
	//※以下のコンソール取得は、出力可能なログレベルでなければ nullptr を返す
	//※ログカテゴリもしくはログレベルの設定から取得する（ログカテゴリ優先）
	inline const GASHA_ IConsole* console(const GASHA_ logPurpose purpose, const category_type category, const GASHA_ logLevel& require_level) const;//コンソール取得
	inline const GASHA_ IConsole* console(const GASHA_ logPurpose purpose, const category_type category, const level_type require_level) const;//コンソール取得
	inline GASHA_ IConsole* console(const GASHA_ logPurpose purpose, const category_type category, const GASHA_ logLevel& require_level);//コンソール取得
	inline GASHA_ IConsole* console(const GASHA_ logPurpose purpose, const category_type category, const level_type require_level);//コンソール取得
	inline const GASHA_ consoleColor* color(const GASHA_ logPurpose purpose, const category_type category, const GASHA_ logLevel& require_level) const;//コンソール取得
	inline const GASHA_ consoleColor* color(const GASHA_ logPurpose purpose, const category_type category, const level_type require_level) const;//コンソール取得
public:
	//イテレータ取得
	inline const iterator begin() const { return iterator(this, GASHA_ logCategory::BEGIN); }//開始イテレータを取得
	inline const iterator end() const { return iterator(this, GASHA_ logCategory::END); }//終端イテレータを取得
	inline iterator begin(){ return iterator(this, GASHA_ logCategory::BEGIN); }//開始イテレータを取得
	inline iterator end(){ return iterator(this, GASHA_ logCategory::END); }//終端イテレータを取得
	inline const_iterator cbegin() const { return iterator(this, GASHA_ logCategory::BEGIN); }//開始constイテレータを取得
	inline const_iterator cend() const { return iterator(this, GASHA_ logCategory::END); }//終端constイテレータを取得
	//リバースイテレータ取得
	inline const reverse_iterator rbegin() const { return reverse_iterator(this, GASHA_ logCategory::END); }//開始イテレータを取得
	inline const reverse_iterator rend() const { return reverse_iterator(this, GASHA_ logCategory::BEGIN); }//終端イテレータを取得
	inline reverse_iterator rbegin(){ return reverse_iterator(this, GASHA_ logCategory::END); }//開始イテレータを取得
	inline reverse_iterator rend(){ return reverse_iterator(this, GASHA_ logCategory::BEGIN); }//終端イテレータを取得
	inline const_reverse_iterator crbegin() const { return reverse_iterator(this, GASHA_ logCategory::END); }//開始constイテレータを取得
	inline const_reverse_iterator crend() const { return reverse_iterator(this, GASHA_ logCategory::BEGIN); }//終端constイテレータを取得
	//イテレータ取得
	const iterator find(const category_type category) const;//指定のカテゴリのイテレータを取得
	iterator find(const category_type category);//指定のカテゴリのイテレータを取得
public:
	//メソッド
	void changeLevel(const GASHA_ logPurpose purpose, const level_type level, const category_type category);//ログレベルを変更
	void changeEveryLevel(const GASHA_ logPurpose purpose, const level_type level);//全ログレベルを変更 ※changeLogLevel に forEvery 指定時と同じ
	void upLevel(const GASHA_ logPurpose purpose, const category_type category);//ログレベルを一レベル上に変更 ※forEvery は無視する
	void downLevel(const GASHA_ logPurpose purpose, const category_type category);//ログレベルを一レベル下に変更 ※forEvery は無視する
	//※以下、ローカルログレベルに関する操作
	//　ローカルログレベルが有効になると、その時点のグローバルログレベルをコピーし、
	//　優先的に使用するようになる
	inline void enableLocalLogLevel();//ローカルログレベルを有効にする ※すでに有効な時は何もしない
	inline void disableLocalLogLevel();//ローカルログレベルを無効にする
	inline bool resumeLocalLogLevel();//ローカルログレベルを再度有効にする ※コピー済みの時だけ可。グローバルログレベルのコピーを行わない。再開できたら（もしくは既に有効なら） true を返す。
private:
	//初期化メソッド（一回限り）
	static void initializeOnce();
public:
	//ムーブオペレータ
	inline logMask& operator=(logMask&& rhs);
	//コピーオペレータ
	inline logMask& operator=(const logMask& rhs);
public:
	//ムーブコンストラクタ
	inline logMask(logMask&& obj);
	//コピーコンストラクタ
	inline logMask(const logMask& obj);
	//明示的な初期化用コンストラクタ
	inline logMask(const explicitInitialize_t&);
	//コンストラクタ
	inline logMask(const bool enable_local);
	//デフォルトコンストラクタ
	inline logMask();
	//デストラクタ
	inline ~logMask();
public:
	//静的フィールド
	static const explicitInitialize_t explicitInitialize;//明示的な初期化指定用
private:
	//フィールド
	level_type m_localLogLevel[PURPOSE_NUM][CATEGORY_NUM];//カテゴリ別ログレベル情報（ローカル）
	bool m_localLogLevelIsEnabled;//ローカルログレベルが有効か？
	bool m_localLogLevelIsCopied;//ローカルログレベルコピー済みか？
	//静的フィールド
	static std::once_flag m_initialized;//初期化済み
	static level_type m_globalLogLevel[PURPOSE_NUM][CATEGORY_NUM];//カテゴリ別ログレベル情報（グローバル）
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_mask.inl>

#endif//GASHA_INCLUDED_LOG_MASK_H

// End of file

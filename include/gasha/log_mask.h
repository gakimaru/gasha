#pragma once
#ifndef GASHA_INCLUDED_LOG_MASK_H
#define GASHA_INCLUDED_LOG_MASK_H

//--------------------------------------------------------------------------------
// log_mask.h
// ログレベルマスク【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_purpose.h>//ログ用途
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
//ログレベルマスク
//--------------------------------------------------------------------------------

//----------------------------------------
//ログレベルマスク
//※カテゴリ別のログレベルマスクを扱う。
//※グローバルログレベルマスクの他、そのコピーとして、ローカルログレベルマスクを扱うことができる。
//※ローカルログレベルマスクは、TLSにその参照を保持し、処理ブロック（スコープ）を抜けるまで、以降の処理に適用される。
//※コンテナクラスを兼ねる。
class logMask
{
public:
	//型
	typedef GASHA_ logPurpose::purpose_type purpose_type;//ログ用途の値
	typedef GASHA_ logLevel::level_type level_type;//ログレベルの値
	typedef GASHA_ logCategory::category_type category_type;//ログカテゴリの値
	struct explicitInit_type{};//明示的な初期化用構造体
public:
	//定数
	static const purpose_type PURPOSE_NUM = GASHA_ logPurpose::NUM;//ログ用途数
	static const level_type LEVEL_MIN = GASHA_ logLevel::NORMAL_MIN;//ログレベル最小値
	static const level_type LEVEL_MAX = GASHA_ logLevel::SPECIAL_MAX;//ログレベル最大値
	static const category_type CATEGORY_MIN = GASHA_ logCategory::NORMAL_MIN;//ログカテゴリ最小値
	static const category_type CATEGORY_MAX = GASHA_ logCategory::NORMAL_MAX;//ログカテゴリ最大値
	static const category_type CATEGORY_NUM = GASHA_ logCategory::NORMAL_NUM;//ログカテゴリ数
	static const category_type CATEGORY_FOR_EVERY = GASHA_ logCategory::FOR_EVERY;//全体変更用ログカテゴリ
	static const level_type DEFAULT_LOG_MASK_OF_LOG = GASHA_DEFAULT_LOG_MASK_OF_LOG;//デフォルトのログレベルマスク
	static const level_type DEFAULT_LOG_MASK_OF_NOTICE = GASHA_DEFAULT_LOG_MASK_OF_NOTICE;//デフォルトの画面通知ログレベルマスク
	enum ref_type//ログレベルマスク参照種別
	{
		isGlobal = 0,//グローバルログレベルマスク
		isTls,//TLSログレベルマスク（他のローカルログレベルの参照）
		isLocal,//ローカルログレベルマスク
	};
	enum consolesCondition_type//コンソール状態
	{
		hasNotAvailableConsole = 0,//利用可能なコンソールがない
		everyConsoleIsDummy = -1,//ダミーコンソールのみ（利用可能なコンソールがない）
		hasAvailableConsoles = 1,//利用可能なコンソールがある
	};
public:
	//ログレベルマスク型
	struct mask_type
	{
	#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
		level_type m_level[PURPOSE_NUM][CATEGORY_NUM];//カテゴリごとのログレベルマスク値
	#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
	};
	//コンソール情報取得用構造体
	struct consolesInfo_type
	{
	#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
		GASHA_ logLevel m_level;//ログレベル
		GASHA_ logCategory m_category;//ログカテゴリ
		GASHA_ IConsole* m_consoles[PURPOSE_NUM];//コンソール
		const GASHA_ consoleColor* m_colors[PURPOSE_NUM];//コンソールカラー
		consolesCondition_type m_cond;//コンソール状態
	#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
	};
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
	#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
	
	public:
		//基本オペレータ
		inline const iterator* operator->() const { return this; }
		inline iterator* operator->(){ return this; }
		inline const iterator& operator*() const { return *this; }
		inline iterator& operator*(){ return *this; }
	public:
		//比較オペレータ
		inline bool operator==(const iterator& rhs) const { return m_logMask == rhs.m_logMask && m_logCategory == rhs.m_logCategory; }
		inline bool operator!=(const iterator& rhs) const { return m_logMask == rhs.m_logMask && m_logCategory != rhs.m_logCategory; }
	public:
		//算術オペレータ
		inline const iterator& operator++() const { inc(); return *this; }
		inline iterator& operator++(){ inc(); return *this; }
		inline iterator operator++(int) const { iterator ite(m_logMask, m_logCategory); inc(); return ite; }
		inline const iterator& operator--() const { dec(); return *this; }
		inline iterator& operator--(){ dec(); return *this; }
		inline iterator operator--(int) const { iterator ite(m_logMask, m_logCategory); dec(); return ite; }
	public:
		//キャストオペレータ
		inline operator bool() const { return isExist(); }
		inline operator category_type() const { return static_cast<GASHA_ logCategory::category_type>(m_logCategory); }
		inline operator const GASHA_ logCategory*() const { return static_cast<const GASHA_ logCategory*>(m_logCategory); }//カテゴリの値
		inline operator const GASHA_ logCategory&() const { return m_logCategory; }//カテゴリの値
	public:
		//アクセッサ
		inline bool isExist() const { return m_logCategory.isExist(); }
		inline const GASHA_ logCategory& category() const { return m_logCategory; }//カテゴリの値
		inline const GASHA_ logLevel level(const purpose_type purpose) const { return logLevel(m_logMask->level(purpose, m_logCategory)); }//ログレベルマスク
		inline const GASHA_ IConsole* console(const purpose_type purpose, const level_type level) const;//コンソール取得
		inline GASHA_ IConsole* console(const purpose_type purpose, const level_type level);//コンソール取得
		inline const GASHA_ consoleColor* color(const purpose_type purpose, const level_type level) const;//コンソールカラー取得
	private:
		//インクリメント
		void inc() const;
		//デクリメント
		void dec() const;
	public:
		//メソッド
		void changeLevel(const purpose_type purpose, const level_type level);//ログレベルを変更
		void upLevel(const purpose_type purpose);//ログレベルを一レベル上に変更
		void downLevel(const purpose_type purpose);//ログレベルを一レベル下に変更
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
		mutable logMask* m_logMask;//ログレベルマスク
		mutable GASHA_ logCategoryContainer::iterator m_logCategory;//ログカテゴリのイテレータ
	
	#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
	
	public:
		//基本オペレータ
		inline const iterator* operator->() const { return this; }
		inline iterator* operator->(){ return this; }
		inline const iterator& operator*() const { return *this; }
		inline iterator& operator*(){ return *this; }
	public:
		//比較オペレータ
		inline bool operator==(const iterator& rhs) const { return true; }
		inline bool operator!=(const iterator& rhs) const { return false; }
	public:
		//算術オペレータ
		inline const iterator& operator++() const { return *this; }
		inline iterator& operator++(){ return *this; }
		inline iterator operator++(int) const { return iterator(); }
		inline const iterator& operator--() const { return *this; }
		inline iterator& operator--(){ return *this; }
		inline iterator operator--(int) const { return iterator(); }
	public:
		//キャストオペレータ
		inline operator bool() const { return isExist(); }
		inline operator category_type() const { return 0; }
		inline operator const GASHA_ logCategory*() const { return &m_logCategory; }//カテゴリの値
		inline operator const GASHA_ logCategory&() const { return m_logCategory; }//カテゴリの値
	public:
		//アクセッサ
		inline bool isExist() const { return false; }
		inline const GASHA_ logCategory& category() const { return m_logCategory; }//カテゴリの値
		inline const GASHA_ logLevel level(const purpose_type purpose) const { return GASHA_ logLevel(); }//ログレベルマスク
		inline const GASHA_ IConsole* console(const purpose_type purpose, const level_type level) const{ return nullptr; }//コンソール取得
		inline GASHA_ IConsole* console(const purpose_type purpose, const level_type level){ return nullptr; }//コンソール取得
		inline const GASHA_ consoleColor* color(const purpose_type purpose, const level_type level) const{ return nullptr; }//コンソールカラー取得
	public:
		//メソッド
		inline void changeLevel(const purpose_type purpose, const level_type level){}//ログレベルを変更
		inline void upLevel(const purpose_type purpose){}//ログレベルを一レベル上に変更
		inline void downLevel(const purpose_type purpose){}//ログレベルを一レベル下に変更
	public:
		inline iterator& operator=(iterator&& rhs){ return *this; }//ムーブオペレータ
		inline iterator& operator=(const iterator& rhs){ return *this; }//コピーオペレータ
	public:
		inline iterator(iterator&& ite){}//ムーブコンストラクタ
		inline iterator(const iterator& ite){}//コピーコンストラクタ
		inline iterator(const logMask* mask, const category_type category){}//コンストラクタ}
		inline iterator(const logMask* mask, const GASHA_ logCategoryContainer::iterator& category){}//コンストラクタ
		inline iterator(){}//デフォルトコンストラクタ
		inline ~iterator(){}//デストラクタ
	private:
		//フィールド
		mutable GASHA_ logCategory m_logCategory;//ログカテゴリ（ダミー）
	
	#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
	};
	//--------------------
	//リバースイテレータ
	class reverse_iterator : public std::iterator<std::bidirectional_iterator_tag, GASHA_ logCategory>//双方向イテレータとして実装
	{
	#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

	public:
		//基本オペレータ
		inline const reverse_iterator* operator->() const { return this; }
		inline reverse_iterator* operator->(){ return this; }
		inline const reverse_iterator& operator*() const { return *this; }
		inline reverse_iterator& operator*(){ return *this; }
	public:
		//比較オペレータ
		inline bool operator==(const reverse_iterator& rhs) const { return m_logMask == rhs.m_logMask && m_logCategory == rhs.m_logCategory; }
		inline bool operator!=(const reverse_iterator& rhs) const { return m_logMask == rhs.m_logMask && m_logCategory != rhs.m_logCategory; }
	public:
		//算術オペレータ
		inline const reverse_iterator& operator++() const { inc(); return *this; }
		inline reverse_iterator& operator++(){ inc(); return *this; }
		inline reverse_iterator operator++(int) const { reverse_iterator ite(m_logMask, m_logCategory); inc(); return ite; }
		inline const reverse_iterator& operator--() const { dec(); return *this; }
		inline reverse_iterator& operator--(){ dec(); return *this; }
		inline reverse_iterator operator--(int) const { reverse_iterator ite(m_logMask, m_logCategory); dec(); return ite; }
	public:
		//キャストオペレータ
		inline operator bool() const { return isExist(); }
		inline operator category_type() const { return static_cast<GASHA_ logCategory::category_type>(m_logCategory); }
		inline operator const GASHA_ logCategory*() const { return static_cast<const GASHA_ logCategory*>(m_logCategory); }//カテゴリの値
		inline operator const GASHA_ logCategory&() const { return m_logCategory; }//カテゴリの値
	public:
		//アクセッサ
		inline bool isExist() const { return m_logCategory.isExist(); }
		inline const GASHA_ logCategory& category() const { return m_logCategory; }//カテゴリの値
		inline const GASHA_ logLevel level(const purpose_type purpose) const { return m_logMask->level(purpose, m_logCategory); }//ログレベルマスク
		inline const GASHA_ IConsole* console(const purpose_type purpose, const level_type level) const;//コンソール取得
		inline GASHA_ IConsole* console(const purpose_type purpose, const level_type level);//コンソール取得
		inline const GASHA_ consoleColor* color(const purpose_type purpose, const level_type level) const;//コンソールカラー取得
	private:
		//メソッド
		//インクリメント
		void inc() const;
		//デクリメント
		void dec() const;
	public:
		//メソッド
		void changeLevel(const purpose_type purpose, const level_type level);//ログレベルを変更
		void upLevel(const purpose_type purpose);//ログレベルを一レベル上に変更
		void downLevel(const purpose_type purpose);//ログレベルを一レベル下に変更
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
		mutable logMask* m_logMask;//ログレベルマスク
		mutable GASHA_ logCategoryContainer::reverse_iterator m_logCategory;//ログカテゴリのリバースイテレータ
	
	#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
	
	public:
		//基本オペレータ
		inline const reverse_iterator* operator->() const { return this; }
		inline reverse_iterator* operator->(){ return this; }
		inline const reverse_iterator& operator*() const { return *this; }
		inline reverse_iterator& operator*(){ return *this; }
	public:
		//比較オペレータ
		inline bool operator==(const reverse_iterator& rhs) const { return true; }
		inline bool operator!=(const reverse_iterator& rhs) const { return false; }
	public:
		//算術オペレータ
		inline const reverse_iterator& operator++() const { return *this; }
		inline reverse_iterator& operator++(){ return *this; }
		inline reverse_iterator operator++(int) const { return reverse_iterator(); }
		inline const reverse_iterator& operator--() const { return *this; }
		inline reverse_iterator& operator--(){ return *this; }
		inline reverse_iterator operator--(int) const { return reverse_iterator(); }
	public:
		//キャストオペレータ
		inline operator bool() const { return isExist(); }
		inline operator category_type() const { return 0; }
		inline operator const GASHA_ logCategory*() const { return &m_logCategory; }//カテゴリの値
		inline operator const GASHA_ logCategory&() const { return m_logCategory; }//カテゴリの値
	public:
		//アクセッサ
		inline bool isExist() const { return m_logCategory.isExist(); }
		inline const GASHA_ logCategory& category() const { return m_logCategory; }//カテゴリの値
		inline const GASHA_ logLevel level(const purpose_type purpose) const { return GASHA_ logLevel(); }//ログレベルマスク
		inline const GASHA_ IConsole* console(const purpose_type purpose, const level_type level) const{ return nullptr; }//コンソール取得
		inline GASHA_ IConsole* console(const purpose_type purpose, const level_type level){ return nullptr; }//コンソール取得
		inline const GASHA_ consoleColor* color(const purpose_type purpose, const level_type level) const{ return nullptr; }//コンソールカラー取得
	public:
		//メソッド
		inline void changeLevel(const purpose_type purpose, const level_type level){}//ログレベルを変更
		inline void upLevel(const purpose_type purpose){}//ログレベルを一レベル上に変更
		inline void downLevel(const purpose_type purpose){}//ログレベルを一レベル下に変更
	public:
		inline reverse_iterator& operator=(reverse_iterator&& rhs){ return *this; }//ムーブオペレータ
		inline reverse_iterator& operator=(const reverse_iterator& rhs){ return *this; }//コピーオペレータ
	public:
		inline logMask::iterator base() const{ return logMask::iterator(); }//ベースイテレータを取得
	public:
		inline reverse_iterator(reverse_iterator&& ite){}//ムーブコンストラクタ
		inline reverse_iterator(const reverse_iterator& ite){}//コピーコンストラクタ
		inline reverse_iterator(const logMask* mask, const category_type category){}//コンストラクタ
		inline reverse_iterator(const logMask* mask, const GASHA_ logCategoryContainer::reverse_iterator& category){}//コンストラクタ
		inline reverse_iterator(){}//デフォルトコンストラクタ
		inline ~reverse_iterator(){}//デストラクタ
	private:
		mutable GASHA_ logCategory m_logCategory;//ログカテゴリ（ダミー）
	
	#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化
	};

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

public:
	//基本オペレータ
	inline const mask_type* operator->() const { return ref(); }//現在参照しているログレベルマスク
	inline mask_type* operator->(){ return ref(); }//現在参照しているログレベルマスク
	inline const mask_type& operator*() const { return mask(); }//現在参照しているログレベルマスク
	inline mask_type& operator*(){ return mask(); }//現在参照しているログレベルマスク
public:
	//キャストオペレータ
	inline operator const mask_type&() const { return mask(); }//現在参照しているログレベルマスク
	inline operator mask_type&(){ return mask(); }//現在参照しているログレベルマスク
	inline operator const mask_type*() const { return ref(); }//現在参照しているログレベルマスク
	inline operator mask_type*(){ return ref(); }//現在参照しているログレベルマスク

public:
	//アクセッサ
	inline ref_type refType() const { return m_refType; }//現在参照しているログレベルマスクの種別
	inline const mask_type* ref() const { return m_maskRef; }//現在参照しているログレベルマスク
	inline mask_type* ref() { return m_maskRef; }//現在参照しているログレベルマスク
	inline const mask_type& mask() const { return *m_maskRef; }//現在参照しているログレベルマスク
	inline mask_type& mask() { return *m_maskRef; }//現在参照しているログレベルマスク
	inline level_type level(const purpose_type purpose, const category_type category) const;//ログレベルマスクを取得
	inline bool isEnableLevel(const purpose_type purpose, const GASHA_ logLevel& require_level, const category_type category) const;//出力可能なログレベルか？
	inline bool isEnableLevel(const purpose_type purpose, const level_type require_level, const category_type category) const;//出力可能なログレベルか？
	//コンソール／コンソールカラー取得
	//※ログカテゴリもしくはログレベルの設定から取得する（ログカテゴリ優先）
	//※【注意】ログレベルマスクの判定は行っていないので注意
	inline GASHA_ IConsole* console(const purpose_type purpose, const GASHA_ logLevel& level, const GASHA_ logCategory& category) const;//コンソール取得
	inline GASHA_ IConsole* console(const purpose_type purpose, const level_type level, const category_type category) const;//コンソール取得
	inline GASHA_ IConsole* console(const purpose_type purpose, const GASHA_ logLevel& level, const GASHA_ logCategory& category);//コンソール取得
	inline GASHA_ IConsole* console(const purpose_type purpose, const level_type level, const category_type category);//コンソール取得
	inline const GASHA_ consoleColor* color(const purpose_type purpose, const GASHA_ logLevel& level, const GASHA_ logCategory& category) const;//コンソールカラー取得
	inline const GASHA_ consoleColor* color(const purpose_type purpose, const level_type level, const category_type category) const;//コンソールカラー取得
	//コンソールとコンソールカラー、ログレベルオブジェクト、ログカテゴリオブジェクトをまとめて取得
	//※ログレベルマスクの判定も行う
	//※出力可能なコンソールがない場合（全てログレベルマスクされたかダミーコンソールであった場合）、戻り値に false を返す
	const consolesCondition_type consolesInfo(consolesInfo_type& info, const level_type require_level, const category_type category) const;
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
	void changeLevel(const purpose_type purpose, const level_type level, const category_type category);//ログレベルマスクを変更
	void changeEveryLevel(const purpose_type purpose, const level_type level);//全ログレベルマスクを変更 ※changeLogLevel に forEvery 指定時と同じ
	void upLevel(const purpose_type purpose, const category_type category);//ログレベルマスクを一レベル上に変更 ※forEvery は無視する
	void downLevel(const purpose_type purpose, const category_type category);//ログレベルマスクを一レベル下に変更 ※forEvery は無視する
	
	//参照するログレベルマスクを変更
	//　ローカルログレベルマスクに変更すると、その時点のグローバルログレベルマスクもしくは
	//　TLSログレベルマスクをコピーする。さらに、TLSログレベルマスクに適用する。
	//※ローカルログレベルマスクがTLSログレベルマスクに適用されると、以降の処理にローカルログレベルマスクが反映される。
	//※【注意】ローカルの状態で再度ローカルを指定すると、改めて元の値をコピーし直すことに注意。
	void changeRef(const ref_type type);

	//ログレベルマスクのセーブ／ロード
	//※【注意】現在参照しているログレベルマスクに対して操作する点に注意
	inline std::size_t serializeSize() const;//シリアライズに必要なサイズを取得
	inline bool serialize(void* dst, const std::size_t dst_size) const;//シリアライズ（セーブ用）※dst に出力
	inline bool deserialize(const void* src, const std::size_t dst_size);//デシリアライズ（ロード用）※src から復元

private:
	//初期化メソッド（一回限り）
	static void initializeOnce();

public:
	//ムーブオペレータ
	//※【注意】末端以外のオブジェクトをムーブすると、TLSに不整合が生じるので注意
	inline logMask& operator=(logMask&& rhs);
	//コピーオペレータ
	inline logMask& operator=(const logMask& rhs) = delete;

public:
	//ムーブコンストラクタ
	//※【注意】末端以外のオブジェクトをムーブすると、TLSに不整合が生じるので注意
	inline logMask(logMask&& obj);
	//コピーコンストラクタ
	inline logMask(const logMask& obj) = delete;
	//明示的な初期化用コンストラクタ
	inline logMask(const explicitInit_type&);
	//デフォルトコンストラクタ
	logMask();
	//デストラクタ
	inline ~logMask();

private:
	//フィールド
	ref_type m_refType;//ログレベルマスク種別
	mask_type* m_maskRef;//現在参照しているログレベルマスク
	mask_type* m_prevTlsMask;//変更前のTLSログレベルマスク（ローカルログレベルマスク作成時に記録し、デストラクタで元に戻す）
	mask_type m_localMask;//ローカルログレベルマスク
private:
	//静的フィールド
	static std::once_flag m_initialized;//グローバルログレベルマスク初期化済み
	static mask_type m_globalMask;//グローバルログレベルマスク
private:
	//TLS静的フィールド
	static thread_local mask_type* m_tlsMaskRef;//TLSログレベルマスク（ローカルログレベルマスクの参照）

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

public:
	//基本オペレータ
	inline const mask_type* operator->() const { return ref(); }//現在参照しているログレベルマスク
	inline mask_type* operator->(){ return ref(); }//現在参照しているログレベルマスク
	inline const mask_type& operator*() const { return mask(); }//現在参照しているログレベルマスク
	inline mask_type& operator*(){ return mask(); }//現在参照しているログレベルマスク
public:
	//キャストオペレータ
	inline operator const mask_type&() const { return mask(); }//現在参照しているログレベルマスク
	inline operator mask_type&(){ return mask(); }//現在参照しているログレベルマスク
	inline operator const mask_type*() const { return ref(); }//現在参照しているログレベルマスク
	inline operator mask_type*(){ return ref(); }//現在参照しているログレベルマスク
public:
	//アクセッサ
	inline ref_type refType() const { return isGlobal; }//現在参照しているログレベルマスクの種別
	inline const mask_type* ref() const { return &m_mask; }//現在参照しているログレベルマスク
	inline mask_type* ref() { return &m_mask; }//現在参照しているログレベルマスク
	inline const mask_type& mask() const { return m_mask; }//現在参照しているログレベルマスク
	inline mask_type& mask() { return m_mask; }//現在参照しているログレベルマスク
	inline level_type level(const purpose_type purpose, const category_type category) const{ return 0; }//ログレベルマスクを取得
	inline bool isEnableLevel(const purpose_type purpose, const GASHA_ logLevel& require_level, const category_type category) const{ return false; }//出力可能なログレベルか？
	inline bool isEnableLevel(const purpose_type purpose, const level_type require_level, const category_type category) const{ return false; }//出力可能なログレベルか？
	//コンソール／コンソールカラー取得
	inline GASHA_ IConsole* console(const purpose_type purpose, const GASHA_ logLevel& level, const GASHA_ logCategory& category) const{ return nullptr; }//コンソール取得
	inline GASHA_ IConsole* console(const purpose_type purpose, const level_type level, const category_type category) const{ return nullptr; }//コンソール取得
	inline GASHA_ IConsole* console(const purpose_type purpose, const GASHA_ logLevel& level, const GASHA_ logCategory& category){ return nullptr; }//コンソール取得
	inline GASHA_ IConsole* console(const purpose_type purpose, const level_type level, const category_type category){ return nullptr; }//コンソール取得
	inline const GASHA_ consoleColor* color(const purpose_type purpose, const GASHA_ logLevel& level, const GASHA_ logCategory& category) const{ return nullptr; }//コンソールカラー取得
	inline const GASHA_ consoleColor* color(const purpose_type purpose, const level_type level, const category_type category) const{ return nullptr; }//コンソールカラー取得
	//コンソールとコンソールカラー、ログレベルオブジェクト、ログカテゴリオブジェクトをまとめて取得
	inline const consolesCondition_type consolesInfo(consolesInfo_type& info, const level_type require_level, const category_type category) const{ return hasNotAvailableConsole; }
public:
	//イテレータ取得
	inline const iterator begin() const { return iterator(); }//開始イテレータを取得
	inline const iterator end() const { return iterator(); }//終端イテレータを取得
	inline iterator begin(){ return iterator(); }//開始イテレータを取得
	inline iterator end(){ return iterator(); }//終端イテレータを取得
	inline const_iterator cbegin() const { return iterator(); }//開始constイテレータを取得
	inline const_iterator cend() const { return iterator(); }//終端constイテレータを取得
	//リバースイテレータ取得
	inline const reverse_iterator rbegin() const { return reverse_iterator(); }//開始イテレータを取得
	inline const reverse_iterator rend() const { return reverse_iterator(); }//終端イテレータを取得
	inline reverse_iterator rbegin(){ return reverse_iterator(); }//開始イテレータを取得
	inline reverse_iterator rend(){ return reverse_iterator(); }//終端イテレータを取得
	inline const_reverse_iterator crbegin() const { return reverse_iterator(); }//開始constイテレータを取得
	inline const_reverse_iterator crend() const { return reverse_iterator(); }//終端constイテレータを取得
	//イテレータ取得
	inline const iterator find(const category_type category) const{ return iterator(); }//指定のカテゴリのイテレータを取得
	inline iterator find(const category_type category){ return iterator(); }//指定のカテゴリのイテレータを取得
public:
	//メソッド
	inline void changeLevel(const purpose_type purpose, const level_type level, const category_type category){}//ログレベルマスクを変更
	inline void changeEveryLevel(const purpose_type purpose, const level_type level){}//全ログレベルマスクを変更
	inline void upLevel(const purpose_type purpose, const category_type category){}//ログレベルマスクを一レベル上に変更
	inline void downLevel(const purpose_type purpose, const category_type category){}//ログレベルマスクを一レベル下に変更
	inline void changeRef(const ref_type type){}//参照するログレベルマスクを変更
	inline std::size_t serializeSize() const{ return 0; }//シリアライズに必要なサイズを取得
	inline bool serialize(void* dst, const std::size_t dst_size) const{ return true; }//シリアライズ（セーブ用）※dst に出力
	inline bool deserialize(const void* src, const std::size_t dst_size){ return true; }//デシリアライズ（ロード用）※src から復元
public:
	inline logMask& operator=(logMask&& rhs){ return *this; }//ムーブオペレータ
	inline logMask& operator=(const logMask& rhs) = delete;//コピーオペレータ
public:
	inline logMask(logMask&& obj){}//ムーブコンストラクタ
	inline logMask(const logMask& obj) = delete;//コピーコンストラクタ
	inline logMask(const explicitInit_type&){}//明示的な初期化用コンストラクタ
	inline logMask(){}//デフォルトコンストラクタ
	inline ~logMask(){}//デストラクタ
private:
	//フィールド
	mask_type m_mask;//ダミー

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時は無効化

public:
	//静的フィールド
	static const explicitInit_type explicitInit;//明示的な初期化指定用
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_mask.inl>

#endif//GASHA_INCLUDED_LOG_MASK_H

// End of file

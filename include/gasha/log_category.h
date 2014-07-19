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

#include <gasha/log_purpose.h>//ログ用途
#include <gasha/i_console.h>//コンソールインターフェース
#include <gasha/console_color.h>//コンソールカラー

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint8_t
#include <utility>//C++11 std::move

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

//----------------------------------------
//クラス宣言
class logCategoryContainer;
template<unsigned char _CATEGORY>
class regLogCategory;
namespace _private
{
	template<unsigned char _CATEGORY>
	class regLogCategory;
	template<unsigned char _CATEGORY>
	class regSpecialLogCategory;
}

//----------------------------------------
//ログカテゴリ
//※ログ出力カテゴリと出力先（コンソール）を扱う。（出力先には通常 nullptr を指定し、ログレベルの設定に基づいて出力を行う。ログカテゴリに出力先が指定されると、優先的に使用される。）
//※コンストラクタにログカテゴリを指定すると、コンテナに登録済みのログカテゴリ情報を扱える。
//※一度登録したものは取り消せないが、出力先をいつでも変更することができる。
//※コンテナへの登録には regLogCategory 関数オブジェクトを用いる。（テンプレートクラスにより、有効範囲内の値かコンパイル時にチェックする）
class logCategory
{
	friend class logCategoryContainer;
	template<unsigned char _CATEGORY>
	friend class regLogCategory;
	template<unsigned char _CATEGORY>
	friend class _private::regLogCategory;
	template<unsigned char _CATEGORY>
	friend class _private::regSpecialLogCategory;
public:
	//型
	typedef GASHA_ logPurpose::purpose_type purpose_type;//ログ用途の値
	typedef std::uint8_t category_type;//ログカテゴリの値
public:
	//定数
	static const purpose_type PURPOSE_NUM = GASHA_ logPurpose::NUM;
private:
	//ログカテゴリ情報
	struct info
	{
		const char* m_name;//名前
		category_type m_value;//カテゴリの値
		GASHA_ iConsole* m_consoles[PURPOSE_NUM];//出力先コンソール
	};
public:
	//定数
	static const category_type NORMAL_NUM = GASHA_LOG_CATEGORY_NUM;//通常ログカテゴリ数
	static const category_type SPECIAL_NUM = 3;//特殊ログカテゴリ数
	static const category_type NUM = NORMAL_NUM + SPECIAL_NUM;//ログカテゴリ総数
	static const category_type MIN = 0;//ログカテゴリ最小値
	static const category_type MAX = NUM - 1;//ログカテゴリ最大値
	static const category_type NORMAL_MIN = MIN;//通常ログカテゴリ最小値
	static const category_type NORMAL_MAX = NORMAL_MIN + NORMAL_NUM - 1;//通常ログカテゴリ最大値
	static const category_type SPECIAL_MIN = NORMAL_MAX + 1;//特殊ログカテゴリ最小値
	static const category_type SPECIAL_MAX = SPECIAL_MIN + SPECIAL_NUM - 1;//特殊ログカテゴリ最大値
	static const category_type FOR_EVERY = SPECIAL_MIN;//全体マスク用ログカテゴリ
	static const category_type BEGIN = MIN;//ログカテゴリ開始値（イテレータ用）
	static const category_type END = NUM;//ログカテゴリ終端値（イテレータ用）
	static const category_type INVALID = NUM + 1;//無効なログカテゴリ（イテレータ用）
	static const category_type POOL_NUM = NUM;//ログカテゴリ記録数
	static_assert(SPECIAL_MAX == MAX, "Invalid category-constants.");//定数チェック

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//比較オペレータ
	inline bool operator==(const logCategory& rhs) const { return value() == rhs.value(); }
	inline bool operator!=(const logCategory& rhs) const { return value() != rhs.value(); }
public:
	//キャストオペレータ
	inline operator bool() const { return isExist(); }//正しいログカテゴリか？
	inline operator int() const { return static_cast<int>(value()); }//ログカテゴリの値
	inline operator category_type() const { return value(); }//ログカテゴリの値
	inline operator const char*() const { return name(); }//名前
public:
	//アクセッサ
	inline bool isExist() const { return m_info != nullptr; }//正しいログカテゴリか？
	inline bool isSpecial() const { return m_info->m_value >= SPECIAL_MIN && m_info->m_value <= SPECIAL_MAX; }//特殊ログカテゴリか？
	inline bool isAllowMask() const { return !isSpecial() || m_info->m_value == FOR_EVERY; }//マスク操作可能なログカテゴリか？（通常カテゴリ＋全体マスク操作用カテゴリ）
	inline category_type value() const { return m_info->m_value; }//ログカテゴリの値取得
	inline const char* name() const { return m_info->m_name; }//名前取得
	inline GASHA_ iConsole* console(const purpose_type purpose) const { return m_info->m_consoles[purpose]; }//コンソール取得
	inline void setConsole(const purpose_type purpose, GASHA_ iConsole* console){ m_info->m_consoles[purpose] = console; }//コンソール変更
	inline void setConsole(const purpose_type purpose, GASHA_ iConsole& console){ m_info->m_consoles[purpose] = &console; }//コンソール変更
public:
	//ムーブオペレータ
	inline logCategory& operator=(logCategory&& rhs);
	//コピーオペレータ
	inline logCategory& operator=(const logCategory& rhs);
public:
	//ムーブコンストラクタ
	inline logCategory(logCategory&& obj);
	//コピーコンストラクタ
	inline logCategory(const logCategory& obj);
	//コンストラクタ
	inline logCategory(const category_type value);
	//デフォルトコンストラクタ
	inline logCategory();
	//デストラクタ
	inline ~logCategory();
private:
	//コピーオペレータ
	inline logCategory& operator=(info*info);
	//コンストラクタ
	inline logCategory(info* info);
private:
	//フィールド
	info* m_info;//ログカテゴリ情報

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//比較オペレータ
	inline bool operator==(const logCategory& rhs) const { return true; }
	inline bool operator!=(const logCategory& rhs) const { return false; }
public:
	//キャストオペレータ
	inline operator bool() const { return isExist(); }//正しいログカテゴリか？
	inline operator int() const { return static_cast<int>(value()); }//ログカテゴリの値
	inline operator category_type() const { return value(); }//ログカテゴリの値
	inline operator const char*() const { return name(); }//名前
public:
	//アクセッサ
	inline bool isExist() const { return false; }//正しいログカテゴリか？
	inline bool isSpecial() const { return false; }//特殊ログカテゴリか？
	inline bool isAllowMask() const { return false; }//マスク操作可能なログカテゴリか？（通常カテゴリ＋全体マスク操作用カテゴリ）
	inline category_type value() const { return 0; }//ログカテゴリの値取得
	inline const char* name() const { return ""; }//名前取得
	inline GASHA_ iConsole* console(const purpose_type purpose) const { return const_cast<GASHA_ iConsole*>(&m_console); }//コンソール取得
	inline void setConsole(const purpose_type purpose, GASHA_ iConsole* console){}//コンソール変更
	inline void setConsole(const purpose_type purpose, GASHA_ iConsole& console){}//コンソール変更
public:
	inline logCategory& operator=(logCategory&& rhs){ return *this; }//ムーブオペレータ
	inline logCategory& operator=(const logCategory& rhs){ return *this; }//コピーオペレータ
public:
	inline logCategory(logCategory&& obj){}//ムーブコンストラクタ
	inline logCategory(const logCategory& obj){}//コピーコンストラクタ
	inline logCategory(const category_type value){}//コンストラクタ
	inline logCategory(){}//デフォルトコンストラクタ
	inline ~logCategory(){}//デストラクタ
private:
	iConsole m_console;//ダミーコンソール

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
};

//--------------------
//ログカテゴリコンテナ
//※イテレータで全ログカテゴリを列挙可能
class logCategoryContainer
{
	friend class logCategory;
	template<unsigned char _CATEGORY>
	friend class regLogCategory;
	template<unsigned char _CATEGORY>
	friend class _private::regLogCategory;
	template<unsigned char _CATEGORY>
	friend class _private::regSpecialLogCategory;
public:
	//型
	struct explicitInit_type{};//明示的な初期化用構造体
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

	#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

	public:
		//基本オペレータ
		inline const logCategory* operator->() const { return &category(); }
		inline logCategory* operator->(){ return &category(); }
		inline const logCategory& operator*() const { return category(); }
		inline logCategory& operator*(){ return category(); }
	public:
		//比較オペレータ
		inline bool operator==(const iterator& rhs) const { return value() == rhs.value(); }
		inline bool operator!=(const iterator& rhs) const { return value() != rhs.value(); }
	public:
		//算術オペレータ
		inline const iterator& operator++() const { inc(); return *this; }
		inline iterator& operator++(){ inc(); return *this; }
		inline iterator operator++(int) const { iterator ite(value()); inc(); return ite; }
		inline const iterator& operator--() const { dec(); return *this; }
		inline iterator& operator--(){ dec(); return *this; }
		inline iterator operator--(int) const { iterator ite(value()); dec(); return ite; }
	public:
		//キャストオペレータ
		inline operator bool() const { return isExist(); }
		inline operator logCategory::category_type() const { return value(); }//カテゴリの値
		inline operator const logCategory*() const { return &category(); }
		inline operator logCategory*(){ return &category(); }
		inline operator const logCategory&() const { return category(); }
		inline operator logCategory&(){ return category(); }
	public:
		//アクセッサ
		inline bool isExist() const { return m_logCategory.isExist(); }
		inline bool isSpecial() const { return m_logCategory.isSpecial(); }
		inline bool isAllowMask() const { return m_logCategory.isAllowMask(); }
		inline logCategory::category_type value() const { return m_value; }//カテゴリの値
		inline const logCategory& category() const { return m_logCategory; }
		inline logCategory& category(){ return m_logCategory; }
	private:
		//メソッド
		void inc() const;//インクリメント
		void dec() const;//デクリメント
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
		iterator(const logCategory::category_type value);
		//デフォルトコンストラクタ
		inline iterator();
		//デストラクタ
		inline ~iterator();
	private:
		//フィールド
		mutable logCategory::category_type m_value;//カテゴリの値
		mutable logCategory m_logCategory;//ログカテゴリ
		mutable bool m_isEnd;//終端か？

	#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
	
	public:
		//基本オペレータ
		inline const logCategory* operator->() const { return &category(); }
		inline logCategory* operator->(){ return &category(); }
		inline const logCategory& operator*() const { return category(); }
		inline logCategory& operator*(){ return category(); }
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
		inline operator logCategory::category_type() const { return value(); }//カテゴリの値
		inline operator const logCategory*() const { return &category(); }
		inline operator logCategory*(){ return &category(); }
		inline operator const logCategory&() const { return category(); }
		inline operator logCategory&(){ return category(); }
	public:
		//アクセッサ
		inline bool isExist() const { return false; }
		inline bool isSpecial() const { return false; }
		inline bool isAllowMask() const { return false; }
		inline logCategory::category_type value() const { return 0; }//カテゴリの値
		inline const logCategory& category() const { return m_logCategory; }
		inline logCategory& category(){ return m_logCategory; }
	public:
		inline iterator& operator=(iterator&& rhs){ return *this; }//ムーブオペレータ
		inline iterator& operator=(const iterator& rhs){ return *this; }//コピーオペレータ
	public:
		inline iterator(iterator&& ite){}//ムーブコンストラクタ
		inline iterator(const iterator& ite){}//コピーコンストラクタ
		inline iterator(const logCategory::category_type value){}//コンストラクタ
		inline iterator(){}//デフォルトコンストラクタ
		inline ~iterator(){}//デストラクタ
	private:
		//フィールド
		mutable logCategory m_logCategory;//ログカテゴリ（ダミー）
	
	#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
	};
	//--------------------
	//リバースイテレータ
	class reverse_iterator : public std::iterator<std::bidirectional_iterator_tag, logCategory>//双方向イテレータとして実装
	{
		friend logCategory;

	#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

	public:
		//基本オペレータ
		inline const logCategory* operator->() const { return &category(); }
		inline logCategory* operator->(){ return &category(); }
		inline const logCategory& operator*() const { return category(); }
		inline logCategory& operator*(){ return category(); }
	public:
		//比較オペレータ
		inline bool operator==(const reverse_iterator& rhs) const { return value() == rhs.value(); }
		inline bool operator!=(const reverse_iterator& rhs) const { return value() != rhs.value(); }
	public:
		//算術オペレータ
		inline const reverse_iterator& operator++() const { inc(); return *this; }
		inline reverse_iterator& operator++(){ inc(); return *this; }
		inline reverse_iterator operator++(int) const { reverse_iterator ite(value()); inc(); return ite; }
		inline const reverse_iterator& operator--() const { dec(); return *this; }
		inline reverse_iterator& operator--(){ dec(); return *this; }
		inline reverse_iterator operator--(int) const { reverse_iterator ite(value()); dec(); return ite; }
	public:
		//キャストオペレータ
		inline operator bool() const { return isExist(); }
		inline operator logCategory::category_type() const { return baseValue(); }//ベースカテゴリの値
		inline operator const logCategory*() const { return &category(); }//カテゴリの値
		inline operator logCategory*(){ return &category(); }//カテゴリの値
		inline operator const logCategory&() const { return category(); }//カテゴリの値
		inline operator logCategory&(){ return category(); }//カテゴリの値
	public:
		//アクセッサ
		inline bool isExist() const { return m_logCategory.isExist(); }
		inline bool isSpecial() const { return m_logCategory.isSpecial(); }
		inline bool isAllowMask() const { return m_logCategory.isAllowMask(); }
		inline logCategory::category_type value() const { return m_value; }//カテゴリの値
		inline logCategory::category_type baseValue() const { return m_value == logCategory::INVALID ? logCategory::INVALID : m_logCategory.m_info ? m_logCategory.m_info->m_value : logCategory::END; }//ベースカテゴリの値
		inline const logCategory& category() const { return m_logCategory; }
		inline logCategory& category(){ return m_logCategory; }
	private:
		//メソッド
		void inc() const;//インクリメント
		void dec() const;//デクリメント
	public:
		//ムーブオペレータ
		inline reverse_iterator& operator=(reverse_iterator&& rhs);
		//コピーオペレータ
		inline reverse_iterator& operator=(const reverse_iterator& rhs);
	public:
		//ベースイテレータを取得
		inline logCategoryContainer::iterator base() const;
	public:
		//ムーブコンストラクタ
		inline reverse_iterator(reverse_iterator&& ite);
		//コピーコンストラクタ
		inline reverse_iterator(const reverse_iterator& ite);
		//コンストラクタ
		reverse_iterator(const logCategory::category_type value);
		//デフォルトコンストラクタ
		inline reverse_iterator();
		//デストラクタ
		inline ~reverse_iterator();
	private:
		//フィールド
		mutable logCategory::category_type m_value;//カテゴリの値
		mutable logCategory m_logCategory;//ログカテゴリ
		mutable bool m_isEnd;//終端か？

	#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
	
	public:
		//基本オペレータ
		inline const logCategory* operator->() const { return &category(); }
		inline logCategory* operator->(){ return &category(); }
		inline const logCategory& operator*() const { return category(); }
		inline logCategory& operator*(){ return category(); }
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
		inline operator logCategory::category_type() const { return baseValue(); }//ベースカテゴリの値
		inline operator const logCategory*() const { return &category(); }//カテゴリの値
		inline operator logCategory*(){ return &category(); }//カテゴリの値
		inline operator const logCategory&() const { return category(); }//カテゴリの値
		inline operator logCategory&(){ return category(); }//カテゴリの値
	public:
		//アクセッサ
		inline bool isExist() const { return false; }
		inline bool isSpecial() const { return false; }
		inline bool isAllowMask() const { return false; }
		inline logCategory::category_type value() const { return 0; }//カテゴリの値
		inline logCategory::category_type baseValue() const { return 0; }//ベースカテゴリの値
		inline const logCategory& category() const { return m_logCategory; }
		inline logCategory& category(){ return m_logCategory; }
	public:
		inline reverse_iterator& operator=(reverse_iterator&& rhs){ return *this; }//ムーブオペレータ
		inline reverse_iterator& operator=(const reverse_iterator& rhs){ return *this; }//コピーオペレータ
	public:
		inline logCategoryContainer::iterator base() const{ return logCategoryContainer::iterator(); }//ベースイテレータを取得
	public:
		inline reverse_iterator(reverse_iterator&& ite){}//ムーブコンストラクタ
		inline reverse_iterator(const reverse_iterator& ite){}//コピーコンストラクタ
		inline reverse_iterator(const logCategory::category_type value){}//コンストラクタ
		inline reverse_iterator(){}//デフォルトコンストラクタ
		inline ~reverse_iterator(){}//デストラクタ
	private:
		//フィールド
		mutable logCategory m_logCategory;//ログカテゴリ（ダミー）
	
	#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
	};

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//アクセッサ
	inline const logCategory at(const logCategory::category_type value) const { return logCategory(getInfo(value)); }
	inline logCategory at(const logCategory::category_type value){ return logCategory(getInfo(value)); }
	inline const logCategory operator[](const logCategory::category_type value) const { return logCategory(getInfo(value)); }
	inline logCategory operator[](const logCategory::category_type value){ return logCategory(getInfo(value)); }
private:
	//静的メソッド
	inline static logCategory::info* getInfo(const logCategory::category_type value);//要素を取得
	static bool regist(const logCategory::info& info);//要素を登録
public:
	//イテレータ取得
	inline const iterator begin() const { return iterator(logCategory::BEGIN); }//開始イテレータを取得
	inline const iterator end() const { return iterator(logCategory::END); }//終端イテレータを取得
	inline iterator begin(){ return iterator(logCategory::BEGIN); }//開始イテレータを取得
	inline iterator end(){ return iterator(logCategory::END); }//終端イテレータを取得
	inline const_iterator cbegin() const { return iterator(logCategory::BEGIN); }//開始constイテレータを取得
	inline const_iterator cend() const { return iterator(logCategory::END); }//終端constイテレータを取得
	//リバースイテレータ取得
	inline const reverse_iterator rbegin() const { return reverse_iterator(logCategory::END); }//開始イテレータを取得
	inline const reverse_iterator rend() const { return reverse_iterator(logCategory::BEGIN); }//終端イテレータを取得
	inline reverse_iterator rbegin(){ return reverse_iterator(logCategory::END); }//開始イテレータを取得
	inline reverse_iterator rend(){ return reverse_iterator(logCategory::BEGIN); }//終端イテレータを取得
	inline const_reverse_iterator crbegin() const { return reverse_iterator(logCategory::END); }//開始constイテレータを取得
	inline const_reverse_iterator crend() const { return reverse_iterator(logCategory::BEGIN); }//終端constイテレータを取得
private:
	//初期化メソッド（一回限り）
	static void initializeOnce();
public:
	//明示的な初期化用コンストラクタ
	inline logCategoryContainer(const explicitInit_type&);
	//デフォルトコンストラクタ
	inline logCategoryContainer();
	//デストラクタ
	inline ~logCategoryContainer();
private:
	//フィールド
	static std::once_flag m_initialized;//初期化済み
	static logCategory::info m_pool[logCategory::POOL_NUM];//要素のプール（バッファ）
	static std::bitset<logCategory::POOL_NUM> m_isAlreadyPool;//要素の初期化済みフラグ

#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//アクセッサ
	inline const logCategory at(const logCategory::category_type value) const { return logCategory(); }
	inline logCategory at(const logCategory::category_type value){ return logCategory(); }
	inline const logCategory operator[](const logCategory::category_type value) const { return logCategory(); }
	inline logCategory operator[](const logCategory::category_type value){ return logCategory(); }
private:
	//静的メソッド
	inline static logCategory::info* getInfo(const logCategory::category_type value){ return nullptr; }//要素を取得
	inline static bool regist(const logCategory::info& info){ return true; }//要素を登録
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
public:
	inline logCategoryContainer(const explicitInit_type&){}//明示的な初期化用コンストラクタ
	inline logCategoryContainer(){}//デフォルトコンストラクタ
	inline ~logCategoryContainer(){}//デストラクタ

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

public:
	//静的フィールド
	static const explicitInit_type explicitInit;//明示的な初期化指定用
};

//----------------------------------------
//ログカテゴリ登録用テンプレートクラス
//※テンプレートクラスにより、有効範囲内の値かコンパイル時にチェックする。
template<unsigned char _CATEGORY>
class regLogCategory
{
public:
	//定数
	static const logCategory::category_type CATEGORY = _CATEGORY;//カテゴリの値
	static_assert(CATEGORY >= logCategory::NORMAL_MIN && CATEGORY <= logCategory::NORMAL_MAX, "Out of range of normal-log-category");//値の範囲チェック
public:
	//関数オペレータ
#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
	inline bool operator()(const char* name);
	inline bool operator()(const char* name, iConsole* (&consoles)[logCategory::PURPOSE_NUM]);
#else//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
	inline bool operator()(const char* name){ return true; }
	inline bool operator()(const char* name, iConsole* (&consoles)[logCategory::PURPOSE_NUM]){ return true; }
#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化
};

//----------------------------------------
//ログカテゴリ登録用補助マクロ
#define MAKE_LOG_CATEGORY_VALUE(VALUE) (logCategory::NORMAL_MIN + VALUE)//ログカテゴリ定数計算用マクロ
#define MAKE_SPECIAL_LOG_CATEGORY_VALUE(VALUE) (logCategory::SPECIAL_MIN + VALUE)//特殊ログカテゴリ定数計算用マクロ

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_category.inl>

//利便性のためにいっしょにインクルード
#include <gasha/default_log_category.h>//既定のログカテゴリ

#endif//GASHA_INCLUDED_LOG_CATEGORY_H

// End of file

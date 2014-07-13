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
//ログカテゴリ
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//クラス宣言
class logCategoryContainer;
template<unsigned char _CATEGORY>
class regLogCategory;
namespace _private
{
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
	friend class _private::regSpecialLogCategory;
public:
	//型
	typedef unsigned char category_type;//値（カテゴリ）
private:
	//ログカテゴリ情報
	struct info
	{
		const char* m_name;//名前
		category_type m_value;//値（カテゴリ）
		GASHA_ IConsole* m_console;//出力先コンソール
		GASHA_ IConsole* m_consoleForNotice;//画面通知先コンソール
	};
public:
	//定数
	static const category_type NORMAL_NUM = 64;//通常ログカテゴリ数
	static const category_type SPECIAL_NUM = 8;//特殊ログカテゴリ数
	static const category_type NUM = NORMAL_NUM + SPECIAL_NUM;//ログカテゴリ総数
	static const category_type MIN = 0;//ログカテゴリ最小値
	static const category_type MAX = NUM - 1;//ログカテゴリ最大値
	static const category_type NORMAL_MIN = MIN;//通常ログカテゴリ最小値
	static const category_type NORMAL_MAX = NORMAL_MIN + NORMAL_NUM - 1;//通常ログカテゴリ最大値
	static const category_type SPECIAL_MIN = NORMAL_MAX + 1;//特殊ログカテゴリ最小値
	static const category_type SPECIAL_MAX = SPECIAL_MIN + SPECIAL_NUM - 1;//特殊ログカテゴリ最大値
	static const category_type FOR_ALL_MASK = SPECIAL_MIN;//全体マスク用ログカテゴリ
	static const category_type BEGIN = MIN;//ログカテゴリ開始値（イテレータ用）
	static const category_type END = NUM;//ログカテゴリ終端値（イテレータ用）
	static const category_type INVALID = NUM + 1;//無効なログカテゴリ（イテレータ用）
	static const category_type POOL_NUM = NUM;//ログカテゴリ記録数
	static_assert(SPECIAL_MAX == MAX, "Invalid category-constants.");//定数チェック
	//マクロ消去
	#undef TO_OUTPUT_CATEGORY
	#undef FROM_OUTPUT_CATEGORY
public:
	//比較オペレータ
	inline bool operator ==(const logCategory& rhs) const { return value() == rhs.value(); }
	inline bool operator !=(const logCategory& rhs) const { return value() != rhs.value(); }
public:
	//キャストオペレータ
	inline operator bool() const { return isExist(); }//正しいログカテゴリか？
	inline operator int() const { return static_cast<int>(m_info->m_value); }//値（カテゴリ）
	inline operator category_type() const { return m_info->m_value; }//値（カテゴリ）
	inline operator const char*() const { return m_info->m_name; }//名前
public:
	//アクセッサ
	inline bool isExist() const { return m_info != nullptr; }//正しいログカテゴリか？
	inline bool isSpecial() const { return m_info->m_value >= SPECIAL_MIN && m_info->m_value <= SPECIAL_MAX; }//特殊カテゴリか？
	inline bool isAllowMask() const { return !isSpecial() || m_info->m_value == FOR_ALL_MASK; }//マスク操作可能なカテゴリか？（通常カテゴリ＋全体マスク操作用カテゴリ）
	inline category_type value() const { return m_info->m_value; }//値（カテゴリ）取得
	inline const char* name() const { return m_info->m_name; }//名前取得
	inline const GASHA_ IConsole* console() const { return m_info->m_console; }//コンソール
	inline GASHA_ IConsole*& console(){ return m_info->m_console; }//コンソール
	inline const GASHA_ IConsole* consoleForNotice() const { return m_info->m_consoleForNotice; }//画面通知用コンソール
	inline GASHA_ IConsole*& consoleForNotice(){ return m_info->m_consoleForNotice; }//画面通知用コンソール
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
	friend class _private::regSpecialLogCategory;
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
		inline bool isExist() const { return m_logCategory.isExist(); }
		inline const logCategory* operator->() const { return &m_logCategory; }
		inline logCategory* operator->(){ return &m_logCategory; }
		inline const logCategory& operator*() const { return m_logCategory; }
		inline logCategory& operator*(){ return m_logCategory; }
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
		inline operator const logCategory&() const { return m_logCategory; }//値（カテゴリ）
		inline operator logCategory&(){ return m_logCategory; }//値（カテゴリ）
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
		mutable logCategory::category_type m_value;//値（カテゴリ）
		mutable logCategory m_logCategory;//ログカテゴリ
		mutable bool m_isEnd;//終端か？
	};
	//--------------------
	//リバースイテレータ
	class reverse_iterator : public std::iterator<std::bidirectional_iterator_tag, logCategory>//双方向イテレータとして実装
	{
		friend logCategory;
	public:
		//オペレータ
		inline bool isExist() const { return m_logCategory.isExist(); }
		inline const logCategory* operator->() const { return &m_logCategory; }
		inline logCategory* operator->(){ return &m_logCategory; }
		inline const logCategory& operator*() const { return m_logCategory; }
		inline logCategory& operator*(){ return m_logCategory; }
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
		inline operator const logCategory&() const { return m_logCategory; }//値（カテゴリ）
		inline operator logCategory&(){ return m_logCategory; }//値（カテゴリ）
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
		mutable logCategory::category_type m_value;//値（カテゴリ）
		mutable logCategory m_logCategory;//ログカテゴリ
		mutable bool m_isEnd;//終端か？
	};
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
	//通常メソッド
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
	//全てのログカテゴリのコンソールを変更
	void setAllConsole(IConsole* console);
	//全てのログカテゴリの画面通知用コンソールを変更
	void setAllConsoleForNotice(IConsole* console);
private:
	//初期化メソッド（一回限り）
	static void initializeOnce();
public:
	//コンストラクタ
	inline logCategoryContainer();
	//デストラクタ
	inline ~logCategoryContainer();
private:
	//フィールド
	static std::once_flag m_initialized;//初期化済み
	static logCategory::info m_pool[logCategory::POOL_NUM];//要素のプール（バッファ）
	static std::bitset<logCategory::POOL_NUM> m_isAlreadyPool;//要素の初期化済みフラグ
};

//----------------------------------------
//ログカテゴリ登録用テンプレートクラス
//※テンプレートクラスにより、有効範囲内の値かコンパイル時にチェックする。
template<unsigned char _CATEGORY>
class regLogCategory
{
public:
	//定数
	static const logCategory::category_type CATEGORY = _CATEGORY;//値（カテゴリ）
	static_assert(CATEGORY >= logCategory::NORMAL_MIN && CATEGORY <= logCategory::NORMAL_MAX, "Out of range of normal-log-category");//値の範囲チェック
public:
	//関数オペレータ
	inline bool operator()(const char* name, IConsole* console, IConsole* console_for_notice);
};

//----------------------------------------
//既定のログカテゴリ用定数
#define MAKE_LOG_CATEGORY_VALUE(VALUE) (logCategory::NORMAL_MIN + VALUE)
#define MAKE_SPECIAL_LOG_CATEGORY_VALUE(VALUE) (logCategory::SPECIAL_MIN + VALUE)
enum categoryEnum : logCategory::category_type
{
	forAny = MAKE_LOG_CATEGORY_VALUE(0),//なんでも（カテゴリなし）
	forFileSystem = MAKE_LOG_CATEGORY_VALUE(1),//ファイルシステム関係
	forResource = MAKE_LOG_CATEGORY_VALUE(2),//リソース関係
	for3D = MAKE_LOG_CATEGORY_VALUE(3),//3Dグラフィックス関係
	for2D = MAKE_LOG_CATEGORY_VALUE(4),//2Dグラフィックス関係
	forSound = MAKE_LOG_CATEGORY_VALUE(5),//サウンド関係
	//ログレベル／画面通知レベル変更用
	forEvery = MAKE_SPECIAL_LOG_CATEGORY_VALUE(0),//全部まとめて変更
	//特殊なカテゴリ（プリント時専用）
	forCallPoint = MAKE_SPECIAL_LOG_CATEGORY_VALUE(1),//直近のコールポイントのカテゴリに合わせる（なければforAny扱い）
	forCriticalCallPoint = MAKE_SPECIAL_LOG_CATEGORY_VALUE(2),//直近の重大コールポイントのカテゴリに合わせる（なければforAny扱い）
};

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_category.inl>

#endif//GASHA_INCLUDED_LOG_CATEGORY_H

// End of file

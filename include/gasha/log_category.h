#pragma once
#ifndef GASHA_INCLUDED_LOG_CATEGORY_H
#define GASHA_INCLUDED_LOG_CATEGORY_H

//--------------------------------------------------------------------------------
// log_category.h
// プロファイラ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//プロファイラ
//--------------------------------------------------------------------------------

	//----------------------------------------
	//カテゴリクラス
	class category
	{
	public:
		//型
		typedef unsigned char value_t;//値（カテゴリ）
		typedef unsigned char byte;//バッファ用
	public:
		//定数
		static const value_t NORMAL_NUM = 64;//通常カテゴリ数
		static const value_t ASSIGNED_NUM = 8;//割り当て済みカテゴリ数
		static const value_t RESERVED_NUM = NORMAL_NUM - ASSIGNED_NUM;//予約カテゴリ数
		static const value_t SPECIAL_NUM = 3;//特殊カテゴリ数
		static const value_t NUM = NORMAL_NUM + SPECIAL_NUM;//カテゴリ総数
		static const value_t MIN = 0;//カテゴリ最小値
		static const value_t MAX = NUM - 1;//カテゴリ最大値
		static const value_t NORMAL_MIN = MIN;//通常カテゴリ最小値
		static const value_t NORMAL_MAX = NORMAL_MIN + NORMAL_NUM - 1;//通常カテゴリ最大値
		static const value_t ASSIGNED_MIN = NORMAL_MIN;//割り当て済みカテゴリ最小値
		static const value_t ASSIGNED_MAX = ASSIGNED_MIN + ASSIGNED_NUM - 1;//割り当て済みカテゴリ最大値
		static const value_t RESERVED_MIN = ASSIGNED_MAX + 1;//予約カテゴリ最小値
		static const value_t RESERVED_MAX = RESERVED_MIN + RESERVED_NUM - 1;//予約カテゴリ最大値
		static const value_t SPECIAL_MIN = NORMAL_MAX + 1;//特殊カテゴリ最小値
		static const value_t SPECIAL_MAX = SPECIAL_MIN + SPECIAL_NUM - 1;//特殊カテゴリ最大値
		static const value_t BEGIN = MIN;//カテゴリ開始値（イテレータ用）
		static const value_t END = NUM;//カテゴリ終端値（イテレータ用）
		static const value_t POOL_NUM = NUM + 1;//カテゴリ記録数
		static_assert(NORMAL_MAX == RESERVED_MAX, "invalid category numbers.");//定数チェック
		static_assert(SPECIAL_MAX == MAX, "invalid category numbers.");//定数チェック
	public:
		//型
		//--------------------
		//イテレータ
		class iterator : public std::iterator<std::bidirectional_iterator_tag, category>//双方向イテレータとして実装
		{
		public:
			//オペレータ
			const category* operator->() const { return &container::get(m_value); }
			const category& operator*() const { return container::get(m_value); }
			bool operator==(const iterator rhs) const { return m_value == rhs.m_value; }
			bool operator!=(const iterator rhs) const { return m_value != rhs.m_value; }
			const category& operator++() const { return m_value >= container::endValue() ? container::getEnd() : container::get(++m_value); }
			const category& operator++(int) const { return m_value >= container::endValue() ? container::getEnd() : container::get(m_value++); }
			const category& operator--() const { if (m_value == container::beginValue()){ m_value = container::endValue(); return container::getEnd(); } return container::get(--m_value); }
			const category& operator--(int) const { if (m_value == container::beginValue()){ m_value = container::endValue(); return container::getBegin(); } return container::get(m_value--); }
		public:
			//キャストオペレータ
			operator const category&() const { return container::get(m_value); }//値（カテゴリ）
		public:
			//コンストラクタ
			iterator(const value_t value) :
				m_value(value)
			{}
			iterator(const value_t value, int) :
				m_value(container::endValue())
			{}
			iterator(const category& obj) :
				m_value(obj.value())
			{}
			iterator(const category& obj, int) :
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
			value_t mutable m_value;//値（カテゴリ）
		};
		//--------------------
		//constイテレータ
		typedef const iterator const_iterator;
		//--------------------
		//コンテナ（イテレータ用）
		class container
		{
			friend class category;
		public:
			//メソッド
			static const value_t beginValue(){ return BEGIN; }//開始値取得
			static const value_t endValue(){ return END; }//終端値取得
			static const category& get(const value_t value){ return m_poolPtr[value]; }//要素を取得
			static const category& getBegin(){ return m_poolPtr[beginValue()]; }//開始要素を取得
			static const category& getEnd(){ return m_poolPtr[endValue()]; }//終端要素を取得
		private:
			static void set(const value_t value, const category& obj)//要素を更新
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
			static category* m_poolPtr;//要素のプール（ポインタ）
			static byte m_pool[];//要素のプール（バッファ）※バッファとポインタを分けているのは、コンストラクタの実行を防止するため
			static std::bitset<POOL_NUM> m_isAlreadyPool;//要素の初期化済みフラグ
		};
	public:
		//オペレータ
		bool operator ==(const category& rhs) const { return m_value == rhs.m_value; }
		bool operator !=(const category& rhs) const { return m_value != rhs.m_value; }
	private:
		//コピーオペレータ
		category& operator=(const category& rhs)
		{
			memcpy(this, &rhs, sizeof(*this));
			return *this;
		}
	public:
		//キャストオペレータ
		operator int() const { return static_cast<int>(m_value); }//値（カテゴリ）
		operator value_t() const { return m_value; }//値（カテゴリ）
		operator const char*() const { return m_name; }//名前
	public:
		//アクセッサ
		value_t value() const { return m_value; }//値（カテゴリ）取得
		const char* name() const { return m_name; }//名前取得
		bool isAssigned() const { return m_isAssigned; }//割り当て済みカテゴリか？
		bool isReserved() const { return !m_isAssigned; }//予約カテゴリか？
		bool forLog() const { return m_forLog; }//ログ出力可能か？
		bool forNotice() const { return m_forNotice; }//画面通知可能か？
		bool forMask() const { return m_forMask; }//出力レベルマスク可能か？
	public:
		//メソッド
		//コンテナ要素を取得（ショートカット用）
		static const category& get(const value_t value){ return container::get(value); }
	public:
		//デフォルトコンストラクタ
		category() :
			m_name(nullptr),
			m_value(UCHAR_MAX),
			m_isAssigned(false),
			m_forLog(false),
			m_forNotice(false),
			m_forMask(false)
		{
			container::initializeOnce();//コンテナ初期化（一回限り）
		}
		//コピーコンストラクタ
		category(const category& src) :
			m_name(src.m_name),
			m_value(src.m_value),
			m_isAssigned(src.m_isAssigned),
			m_forLog(src.m_forLog),
			m_forNotice(src.m_forNotice),
			m_forMask(src.m_forMask)
		{
		}
		//コンストラクタ
		category(const value_t value, const char* name, const bool is_assigned, const bool for_log, const bool for_notice, const bool for_mask) :
			m_name(name),
			m_value(value),
			m_isAssigned(is_assigned),
			m_forLog(for_log),
			m_forNotice(for_notice),
			m_forMask(for_mask)
		{
			assert(value >= BEGIN && value <= END);
			container::set(m_value, *this);//コンテナに登録
		}
		category(const value_t value) :
			m_name(nullptr),
			m_value(value),
			m_isAssigned(false),
			m_forLog(false),
			m_forNotice(false),
			m_forMask(false)
		{
			assert(value >= BEGIN && value <= END);
			*this = container::get(m_value);//コンテナから取得して自身にコピー
		}
		//デストラクタ
		~category()
		{}
	private:
		//フィールド
		const char* m_name;//名前
		const value_t m_value;//値（カテゴリ）
		const bool m_isAssigned;//割り当て済みカテゴリか？
		const bool m_forLog;//ログ出力可能か？
		const bool m_forNotice;//画面通知可能か？
		const bool m_forMask;//出力レベルマスク可能か？
	};
	//----------------------------------------
	//カテゴリ定義用テンプレートクラス：割り当て済みカテゴリ用
	template<unsigned char V, bool for_log, bool for_notice>
	class category_assigned : public category
	{
	public:
		//定数
		static const value_t VALUE = V;//値（カテゴリ）
		static_assert(VALUE >= ASSIGNED_MIN && VALUE <= ASSIGNED_MAX, "out of range of category");//値の範囲チェック
		static const bool IS_ASSIGNED = true;//割り当て済みカテゴリか？
		static const bool FOR_LOG = for_log;//ログ出力可能か？
		static const bool FOR_NOTICE = for_notice;//画面通知可能か？
		static const bool FOR_MASK = true;//出力レベルマスク可能か？
	public:
		//コンストラクタ
		category_assigned(const char* name) :
			category(VALUE, name, IS_ASSIGNED, FOR_LOG, FOR_NOTICE, FOR_MASK)
		{}
	};
	//----------------------------------------
	//カテゴリ定義用テンプレートクラス：予約カテゴリ用
	template<unsigned char V, bool for_log, bool for_notice>
	class category_reserved: public category
	{
	public:
		//定数
		static const value_t VALUE = V;//値（カテゴリ）
		static_assert(VALUE >= RESERVED_MIN && VALUE <= RESERVED_MAX, "out of range of category");//値の範囲チェック
		static const bool IS_ASSIGNED = false;//割り当て済みカテゴリか？
		static const bool FOR_LOG = for_log;//ログ出力可能か？
		static const bool FOR_NOTICE = for_notice;//画面通知可能か？
		static const bool FOR_MASK = true;//出力レベルマスク可能か？
	public:
		//コンストラクタ
		category_reserved(const char* name) :
			category(VALUE, name, IS_ASSIGNED, FOR_LOG, FOR_NOTICE, FOR_MASK)
		{}
	};
	//----------------------------------------
	//カテゴリ定義用テンプレートクラス：特殊カテゴリ用
	template<unsigned char V, bool for_log, bool for_notice, bool for_mask>
	class category_special : public category
	{
	public:
		//定数
		static const value_t VALUE = V;//値（カテゴリ）
		static_assert(VALUE >= SPECIAL_MIN && VALUE <= SPECIAL_MAX, "out of range of category");//値の範囲チェック
		static const bool IS_ASSIGNED = true;//割り当て済みカテゴリか？
		static const bool FOR_LOG = for_log;//ログ出力可能か？
		static const bool FOR_NOTICE = for_notice;//画面通知可能か？
		static const bool FOR_MASK = for_mask;//出力レベルマスク可能か？
	public:
		//コンストラクタ
		category_special(const char* name) :
			category(VALUE, name, IS_ASSIGNED, FOR_LOG, FOR_NOTICE, FOR_MASK)
		{}
	};
	//----------------------------------------
	//カテゴリ定義クラス：終端用
	class category_end : public category
	{
	public:
		//定数
		static const value_t VALUE = END;//値（カテゴリ）
		static const bool IS_ASSIGNED = true;//割り当て済みカテゴリか？
		static const bool FOR_LOG = false;//ログ出力可能か？
		static const bool FOR_NOTICE = false;//画面通知可能か？
		static const bool FOR_MASK = false;//出力レベルマスク可能か？
	public:
		//コンストラクタ
		category_end() :
			category(VALUE, "(END)", IS_ASSIGNED, FOR_LOG, FOR_NOTICE, FOR_MASK)
		{}
	};
	//----------------------------------------
	//カテゴリ定数
#define define_assignedCategory(value) (category::ASSIGNED_MIN + value)
#define define_revervedCategory(value) (category::RESERVED_MIN + value)
#define define_specialCategory(value) (category::SPECIAL_MIN + value)
	enum categoryEnum : category::value_t
	{
		forAny = define_assignedCategory(0),//なんでも（カテゴリなし）
		forLogic = define_assignedCategory(1),//プログラム関係
		forResource = define_assignedCategory(2),//リソース関係
		for3D = define_assignedCategory(3),//3Dグラフィックス関係
		for2D = define_assignedCategory(4),//2Dグラフィックス関係
		forSound = define_assignedCategory(5),//サウンド関係
		forScript = define_assignedCategory(6),//スクリプト関係
		forGameData = define_assignedCategory(7),//ゲームデータ関係
		//ログレベル／画面通知レベル変更用
		forEvery = define_specialCategory(0),//全部まとめて変更
		//特殊なカテゴリ（プリント時専用）
		forCallPoint = define_specialCategory(1),//直近のコールポイントのカテゴリに合わせる（なければforAny扱い）
		forCriticalCallPoint = define_specialCategory(2),//直近の重大コールポイントのカテゴリに合わせる（なければforAny扱い）
	};
	//----------------------------------------
	//カテゴリ定義
#define declare_assignedCategory(value, for_log, for_notice) struct category_##value : public category_assigned<value, for_log, for_notice>{ category_##value () :category_assigned<value, for_log, for_notice>(#value){} }
#define declare_reservedCategory(value, for_log, for_notice) struct category_##value : public category_reserved<value, for_log, for_notice>{ category_##value () :category_reserved<value, for_log, for_notice>(#value){} }
#define declare_specialCategory(value, for_log, for_notice, for_mask) struct category_##value : public category_special<value, for_log, for_notice, for_mask>{ category_##value () :category_special<value, for_log, for_notice, for_mask>(#value){} }
	//※以下、ヘッダーで公開する必要なし
	declare_assignedCategory(forAny, true, true);//なんでも（カテゴリなし）
	declare_assignedCategory(forLogic, true, true);//プログラム関係
	declare_assignedCategory(forResource, true, true);//リソース関係
	declare_assignedCategory(for3D, true, true);//3Dグラフィックス関係
	declare_assignedCategory(for2D, true, true);//2Dグラフィックス関係
	declare_assignedCategory(forSound, true, true);//サウンド関係
	declare_assignedCategory(forScript, true, true);//スクリプト関係
	declare_assignedCategory(forGameData, true, true);//ゲームデータ関係
	//ログレベル／画面通知レベル変更用
	declare_specialCategory(forEvery, false, false, true);//全部まとめて変更
	//特殊なカテゴリ（プリント時専用）
	declare_specialCategory(forCallPoint, true, true, false);//直近のコールポイントのカテゴリに合わせる（なければforAny扱い）
	declare_specialCategory(forCriticalCallPoint, true, true, false);//直近の重大コールポイントのカテゴリに合わせる（なければforAny扱い）
	//----------------------------------------
	//カテゴリコンテナの静的変数をインスタンス化
	bool category::container::m_isInitialized = false;
	category* category::container::m_poolPtr = nullptr;
	category::byte category::container::m_pool[(POOL_NUM)* sizeof(category)];
	std::bitset<category::POOL_NUM> category::container::m_isAlreadyPool;
	//----------------------------------------
	//カテゴリコンテナ初期化（一回限り）
	void category::container::initializeOnce()
	{
		//初期化済みチェック
		if (m_isInitialized)
			return;
		//静的変数を初期化
		m_isInitialized = true;
		m_isAlreadyPool.reset();
		memset(m_pool, 0, sizeof(m_pool));
		m_poolPtr = reinterpret_cast<category*>(m_pool);
		//要素を初期化
		for (category::value_t value = 0; value < category::NUM; ++value)
		{
			category(value, "(undefined)", false, false, false, false);
			m_isAlreadyPool[value] = false;
		}
		//割り当て済みカテゴリを設定（コンストラクタで要素を登録）
		category_forAny();//なんでも（カテゴリなし）
		category_forLogic();//プログラム関係
		category_forResource();//リソース関係
		category_for3D();//3Dグラフィックス関係
		category_for2D();//2Dグラフィックス関係
		category_forSound();//サウンド関係
		category_forScript();//スクリプト関係
		category_forGameData();//ゲームデータ関係
		//ログレベル／画面通知レベル変更用
		category_forEvery();//全部まとめて変更
		//特殊なカテゴリ（プリント時専用）
		category_forCallPoint();//直近のコールポイントのカテゴリに合わせる（なければforAny扱い）
		category_forCriticalCallPoint();//直近の重大コールポイントのカテゴリに合わせる（なければforAny扱い）
		category_end();//終端
	}
	//----------------------------------------
	//カテゴリ用変数
	static category s_categoryForInitialize;//初期化処理実行のためのインスタンス
	//----------------------------------------
	//カテゴリコンテナ列挙
	void printCategoryAll()
	{
		for (auto& obj : category::container())//C++11スタイル
		//for (auto ite = category::container::begin(); ite != category::container::end(); ++ite)//旧来のスタイル
		//for (auto ite = category::container::cbegin(); ite != category::container::cend(); ++ite)//旧来のスタイル
		{
			//const category& obj = ite;//イテレータを変換（イテレータのままでもアロー演算子で直接値操作可能）
			printf("category=%d, name=\"%s\", isAssigned=%d, isReserved=%d, forLog=%d, forNotice=%d, forMask=%d\n", obj.value(), obj.name(), obj.isAssigned(), obj.isReserved(), obj.forLog(), obj.forNotice(), obj.forMask());
		}
	}

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_category.inl>

#endif//GASHA_INCLUDED_LOG_CATEGORY_H

// End of file

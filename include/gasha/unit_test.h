#pragma once
#ifndef GASHA_INCLUDED_UNIT_TEST_H
#define GASHA_INCLUDED_UNIT_TEST_H

//--------------------------------------------------------------------------------
// unit_test.h
// ユニットテスト【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#ifdef GASHA_UNIT_TEST_ENABLED//ユニットテスト無効化時はまるごと無効化

#include <gasha/simple_assert.h>//シンプルアサーション
#include <gasha/chrono.h>//時間系ユーティリティ
#include <gasha/type_traits.h>//型特性ユーティリティ
#include <gasha/i_console.h>//コンソールインターフェース
#include <gasha/stack_allocator.h>//スタックアロケータ
#include <gasha/scoped_stack_allocator.h>//スコープスタックアロケータ
#include <gasha/str_pool.h>//文字列プール

#include <cstdint>//C++11 std::uint32_t
#include <cstddef>//std::size_t
#include <exception>//std::exception

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ユニットテスト
//--------------------------------------------------------------------------------

namespace ut
{
	//ユニットテスト実行属性
	typedef std::uint32_t attr_type;
	enum attrEnum : attr_type
	{
		ATTR_NONE = 0x00000000, //属性無し
		ATTR_ANY = 0xffffffff,  //全て
		ATTR_AUTO = 0x00000001, //自動実行用
		ATTR_MANUAL = 0x00000002, //手動実行用
	};
	
	//ユニットテスト情報
	static const std::size_t UNIT_TEST_REGIST_NUM_MAX = GASHA_UNIT_TEST_REGIST_NUM_MAX;
	extern std::size_t g_registeredNum;//登録件数
	extern std::size_t g_lastPassedTotal;//成功件数
	extern std::size_t g_lastMissedTotal;//失敗件数
	extern std::size_t g_minStackRemain;//最小スタック残量
	extern std::size_t g_minStrRemain;//最小文字列登録数残量
	extern std::size_t g_minStrBuffRemain;//最小文字列バッファ残量

	//ユニットテスト比較演算子指定用定数
	typedef int ope_type;
	enum opeEnum : ope_type
	{
		UNKNOWN_OPE = 0, //??
		EQ_OPE, //== 
		NE_OPE, //!=
		GT_OPE, //>
		GE_OPE, //>=
		LT_OPE, //<
		LE_OPE, //<=
		OPE_NUM,
	};

	//ユニットテスト比較演算子用文字列
	extern const char* g_opeEnumStr[OPE_NUM];

	//ユニットテスト比較演算子指定用タグ
	struct unknownOpe_tag{};
	struct EQ_tag{};
	struct NE_tag{};
	struct GT_tag{};
	struct GE_tag{};
	struct LT_tag{};
	struct LE_tag{};
	extern const unknownOpe_tag unknownOpe;
	extern const EQ_tag EQ;
	extern const NE_tag NE;
	extern const GT_tag GT;
	extern const GE_tag GE;
	extern const LT_tag LT;
	extern const LE_tag LE;

	//出力先コンソール
	extern iConsole* g_console;

	//スタックアロケータ
	static const std::size_t UNIT_TEST_ALLOCATOR_SIZE = GASHA_UNIT_TEST_ALLOCATOR_SIZE;
	extern GASHA_ stackAllocator_withBuff<UNIT_TEST_ALLOCATOR_SIZE> g_allocator;
	extern GASHA_ scopedStackAllocator<GASHA_ stackAllocator<>>* s_scopeAllocator;

	//文字列プール
	static const std::size_t UNIT_TEST_STR_POOL_SIZE = GASHA_UNIT_TEST_STR_POOL_BUFF_SIZE;
	static const std::size_t UNIT_TEST_STR_POOL_TABLE_SIZE = GASHA_UNIT_TEST_STR_POOL_TABLE_SIZE;
	extern strPool<UNIT_TEST_ALLOCATOR_SIZE, UNIT_TEST_STR_POOL_TABLE_SIZE> g_strPool;

	//出力用ワークバッファ
	static const std::size_t UNIT_TEST_WORK_BUFF_SIZE = GASHA_UNIT_TEST_WORK_BUFF_SIZE;
	extern char g_workBuff[UNIT_TEST_WORK_BUFF_SIZE];

	//----------------------------------------
	//ユニットテスト結果基底クラス
	class resultBase
	{
	public:
		//アクセッサ
		inline void setResult(const bool result);
		inline bool result() const;
		inline void setHasResult(const bool has_result);
		inline bool hasResult() const;
		void setException(std::exception const& e);
		inline bool hasException() const;
		inline bool hasExprStr() const;
		inline bool hasValueStr() const;
		inline bool hasOpeStr() const;
		inline bool hasExpectStr() const;
		void setExprStr(const char* expr);
		inline const char* exprStr() const;
		void setValueStr(const char* value);
		inline const char* valueStr() const;
		void setOpeStr(const char* ope);
		const char* setOpeStrFromId(const opeEnum ope);
		inline const char* opeStr() const;
		void setExpectStr(const char* expect);
		inline const char* expectStr() const;
		inline const char* exceptionStr() const;
	public:
		//キャストオペレータ
		inline operator bool() const;
	public:
		//ムーブオペレータ
		resultBase& operator=(resultBase&& rhs) = delete;
		//コピーオペレータ
		resultBase& operator=(const resultBase& rhs) = delete;
	public:
		//ムーブコンストラクタ
		resultBase(resultBase&& obj) = delete;
		//コピーコンストラクタ
		resultBase(const resultBase& obj) = delete;
		//デフォルトコンストラクタ
		resultBase();
		//デストラクタ
		~resultBase();
	private:
		//フィールド
		bool m_result;
		bool m_hasResult;
		bool m_hasException;
		bool m_hasExprStr;
		bool m_hasValueStr;
		bool m_hasOpeStr;
		bool m_hasExpectStr;
		const char* m_exprStr;
		const char* m_valuieStr;
		const char* m_opeStr;
		const char* m_expectStr;
		const char* m_exceptionStr;
	};
	
	//----------------------------------------
	//ユニットテスト結果クラス
	template<typename T>
	class result : public resultBase
	{
	public:
		//アクセッサ
		void setValue(const T value);
		inline T value() const;
		void setExpect(const T expect);
		inline T expect() const;
		inline void setOpeStrFromType(const unknownOpe_tag&);
		inline void setOpeStrFromType(const EQ_tag&);
		inline void setOpeStrFromType(const NE_tag&);
		inline void setOpeStrFromType(const GT_tag&);
		inline void setOpeStrFromType(const GE_tag&);
		inline void setOpeStrFromType(const LT_tag&);
		inline void setOpeStrFromType(const LE_tag&);
		void evaluate(const unknownOpe_tag&);
		void evaluate(const EQ_tag& ope);
		void evaluate(const NE_tag& ope);
		void evaluate(const GT_tag& ope);
		void evaluate(const GE_tag& ope);
		void evaluate(const LT_tag& ope);
		void evaluate(const LE_tag& ope);
	
	public:
		//コンストラクタ
		inline result(const T value, const T expect);
		inline result(const T value);
		//デフォルトコンストラクタ
		inline result();
		//デストラクタ
		inline ~result();

	private:
		//フィールド
		T m_value;
		T m_expect;
		bool m_hasValue;
		bool m_hasExpect;
	};
	
	//----------------------------------------
	//ユニットテストコンテナ
	class container
	{
	public:
		//型
		typedef int(*testFunc)(std::size_t& passed, std::size_t& missed);
		struct funcInfo
		{
			container::testFunc m_func;
			const char* m_moduleName;
			int m_groupId;
			attr_type m_attr;
			std::size_t passed;
			std::size_t missed;
			GASHA_ sec_t elapsed_time;
		};
	public:
		//ムーブコンストラクタ
		container(container&&) = delete;
		//コピーコンストラクタ
		container(const container&) = delete;
		//デフォルトコンストラクタ
		inline container(){}
		//デストラクタ
		inline ~container(){}

	public:
		//アクセッサ
		static std::size_t size();
		static const funcInfo* head();
		static const funcInfo* at(const int index);
		static const funcInfo* at(const char* module_name);
		
		//前回の結果
		static int lastPassedCount();
		static int lastMissedCount();

	public:
		//前回の実行結果をリセット
		static void resetLastResult();

	public:
		//ユニットテスト実行
		inline static int run(const attr_type target_attr = ATTR_ANY);
		inline static int runByModule(const char* target_module_name, const attr_type target_attr = ATTR_ANY);
		inline static int runByGroup(const int target_group_id, const attr_type target_attr = ATTR_ANY);
		static int runByModuleAndGroup(const char* target_module_name, const int target_group_id, const attr_type target_attr = ATTR_ANY);

		//ユニットテスト登録
		static bool add(testFunc func, const char* module_name, const int group_id, const attr_type attr);

		//ユニットテスト結果登録
		template<typename T, typename OPE>
		static result<T>* makeResultWithValueAndExpect(const T value, T expect, const OPE& ope, const char* expr_str, const char* expect_str);
		template<typename OPE>
		static result<int>* makeResultWithExpect(const OPE& ope, const char* expr_str, const char* expect_str);
		template<typename T>
		static result<T>* makeResultWithValue(const T value, const char* expr_str);
		static result<int>* makeResultWithoutValueAndExpect(const char* expr_str);
		template<typename T>
		static void removeResult(result<T>* result);
		
		//ユニットテスト結果表示
		static void setConsole(GASHA_ iConsole& console);
		static void put(const char* str);
		template<typename... Tx>
		static std::size_t printf(const char* fmt, Tx&&... args);
		static void outputBegin(const char* target_module_name, const int target_group_id, const attr_type target_attr);
		static void outputEnd(const char* target_module_name, const int target_group_id, const attr_type target_attr, const std::size_t total_passed, const std::size_t total_missed, const GASHA_ sec_t elapsed_time);
		static void outputModuleBegin(const char* module_name, const int group_id, const attr_type attr);
		static void outputModuleEnd(const char* module_name, const int group_id, const attr_type attr, const std::size_t passed, const std::size_t missed, const GASHA_ sec_t elapsed_time);
		static void outputResult(const bool is_child, std::size_t* passed, std::size_t* missed, const GASHA_ sec_t elapsed_time_sec, resultBase* result);
	};
}//namespace ut

GASHA_NAMESPACE_END;//ネームスペース：終了

//--------------------------------------------------------------------------------
//ユニットテスト用マクロ
//--------------------------------------------------------------------------------

//ユニットテスト用モジュールの開始マクロ
#define GASHA_UT_BEGIN(_module_name, _group_id, _attr) \
	GASHA_NAMESPACE_BEGIN; \
	namespace ut \
	{ \
		namespace module_##_module_name \
		{ \
			struct module \
			{ \
			module() \
				{ \
					GASHA_ ut::container::add(run, #_module_name, _group_id, _attr); \
				} \
			public: \
				static int run(std::size_t& passed, std::size_t& missed) \
				{ \
					const char* module_name = #_module_name; \
					const int group_id = _group_id; \
					GASHA_ elapsedTime elapsed_time; \
					const GASHA_ ut::attr_type attr = _attr; \
					passed = 0; \
					missed = 0; \
					GASHA_ ut::container::outputModuleBegin(module_name, group_id, attr);

//ユニットテスト用モジュールの終了マクロ
#define GASHA_UT_END() \
					const GASHA_ sec_t elapsed_time_sec = elapsed_time.now(); \
					GASHA_ ut::container::outputModuleEnd(module_name, group_id, attr, passed, missed, elapsed_time_sec); \
					return static_cast<int>(missed); \
				} \
			}; \
			static module auto_regist; \
		}; \
	}; \
	GASHA_NAMESPACE_END;

//ユニットテストに登録：式と式の結果、判定用の演算子、判定用の値を記録
//※直接使用しない
#define GASHA_UT_PRINT_EXPR_AND_RESULT(is_child, expr, ope, expect) \
	{ \
		try \
		{ \
			GASHA_ elapsedTime elapsed_time; \
			auto result = GASHA_ ut::container::makeResultWithValueAndExpect(expr, expect, ope, #expr, #expect); \
			GASHA_ ut::container::outputResult(is_child, &passed, &missed, elapsed_time.now(), result); \
			GASHA_ ut::container::removeResult(result); \
		} \
		catch(std::exception const &e) \
		{ \
			auto result = GASHA_ ut::container::makeResultWithExpect(ope, #expr, #expect); \
			result->setException(e); \
			GASHA_ ut::container::outputResult(is_child, nullptr, &missed, static_cast<GASHA_ sec_t>(0.), result); \
			GASHA_ ut::container::removeResult(result); \
		} \
	}

//ユニットテストに登録：式と式の結果を記録
//※直接使用しない
#define GASHA_UT_PRINT_EXPR_AND_VALUE(is_child, expr) \
	{ \
		try \
		{ \
			GASHA_ elapsedTime elapsed_time; \
			auto result = GASHA_ ut::container::makeResultWithValue(expr, #expr); \
			result->setResult(true); \
			GASHA_ ut::container::outputResult(is_child, nullptr, nullptr, elapsed_time.now(), result); \
			GASHA_ ut::container::removeResult(result); \
		} \
		catch(std::exception const &e) \
		{ \
			auto result = GASHA_ ut::container::makeResultWithoutValueAndExpect(#expr); \
			result->setException(e); \
			GASHA_ ut::container::outputResult(is_child, nullptr, &missed, static_cast<GASHA_ sec_t>(0.), result); \
			GASHA_ ut::container::removeResult(result); \
		} \
	}

//ユニットテストに登録：式のみを記録
//※直接使用しない
#define GASHA_UT_PRINT_EXPR(is_child, expr) \
	{ \
		auto result = GASHA_ ut::container::makeResultWithoutValueAndExpect(#expr); \
		result->setResult(true); \
		GASHA_ ut::container::outputResult(is_child, nullptr, nullptr, static_cast<GASHA_ sec_t>(0.), result); \
		GASHA_ ut::container::removeResult(result); \
	}

//ユニットテスト登録
#define GASHA_UT_EXPECT(expr, ope, expect)       GASHA_UT_PRINT_EXPR_AND_RESULT(false, expr, ope, expect)//式と判定用の演算子と判定用の値を指定（式の結果とその判定結果を表示）
#define GASHA_UT_EXPECT_CHILD(expr, ope, expect) GASHA_UT_PRINT_EXPR_AND_RESULT(true,  expr, ope, expect)//式と判定用の演算子と判定用の値を指定（インデント付きで表示）
#define GASHA_UT_EXPR_WITH_RET(expr)             GASHA_UT_PRINT_EXPR_AND_VALUE(false, expr)//式のみを指定（式の結果を表示するが判定は行わない）
#define GASHA_UT_EXPR_WITH_RET_CHILD(expr)       GASHA_UT_PRINT_EXPR_AND_VALUE(true,  expr)//式のみを指定（インデント付きで表示）
#define GASHA_UT_EXPR(expr)                      GASHA_UT_PRINT_EXPR(false, expr)//式のみを指定（式の結果を扱わない）
#define GASHA_UT_EXPR_CHILD(expr)                GASHA_UT_PRINT_EXPR(true, expr)//式のみを指定（インデント付きで表示）
#define GASHA_UT_EXPECT_EQ(expr, expect)       GASHA_UT_EXPECT(      expr, GASHA_ ut::EQ, expect)//式と == 判定用の値を指定
#define GASHA_UT_EXPECT_EQ_CHILD(expr, expect) GASHA_UT_EXPECT_CHILD(expr, GASHA_ ut::EQ, expect)//式と == 判定用の値を指定（インデント付きで表示）
#define GASHA_UT_EXPECT_NE(expr, expect)       GASHA_UT_EXPECT(      expr, GASHA_ ut::NE, expect)//式と != 判定用の値を指定
#define GASHA_UT_EXPECT_NE_CHILD(expr, expect) GASHA_UT_EXPECT_CHILD(expr, GASHA_ ut::NE, expect)//式と != 判定用の値を指定（インデント付きで表示）
#define GASHA_UT_EXPECT_GT(expr, expect)       GASHA_UT_EXPECT(      expr, GASHA_ ut::GT, expect)//式と > 判定用の値を指定
#define GASHA_UT_EXPECT_GT_CHILD(expr, expect) GASHA_UT_EXPECT_CHILD(expr, GASHA_ ut::GT, expect)//式と > 判定用の値を指定（インデント付きで表示）
#define GASHA_UT_EXPECT_GE(expr, expect)       GASHA_UT_EXPECT(      expr, GASHA_ ut::GE, expect)//式と >= 判定用の値を指定
#define GASHA_UT_EXPECT_GE_CHILD(expr, expect) GASHA_UT_EXPECT_CHILD(expr, GASHA_ ut::GE, expect)//式と >= 判定用の値を指定（インデント付きで表示）
#define GASHA_UT_EXPECT_LT(expr, expect)       GASHA_UT_EXPECT(      expr, GASHA_ ut::LT, expect)//式と < 判定用の値を指定
#define GASHA_UT_EXPECT_LT_CHILD(expr, expect) GASHA_UT_EXPECT_CHILD(expr, GASHA_ ut::LT, expect)//式と < 判定用の値を指定（インデント付きで表示）
#define GASHA_UT_EXPECT_LE(expr, expect)       GASHA_UT_EXPECT(      expr, GASHA_ ut::LE, expect)//式と <= 判定用の値を指定
#define GASHA_UT_EXPECT_LE_CHILD(expr, expect) GASHA_UT_EXPECT_CHILD(expr, GASHA_ ut::LE, expect)//式と <= 判定用の値を指定（インデント付きで表示）

//ユニットテスト自動実行
#ifdef GASHA_UNIT_TEST_AUTO_RUN_ENABLED
#define GASHA_UT_AUTO_RUN() {GASHA_ ut::container::run(ut::ATTR_AUTO); GASHA_UT_RETURN();}
#define GASHA_UT_AUTO_RUN_AND_GET_RESULT(result) {GASHA_ ut::container::run(ut::ATTR_AUTO); result = GASHA_ ut::container::lastMissedCount(); GASHA_UT_RETURN();}
#else//GASHA_UNIT_TEST_AUTO_RUN_ENABLED
#define GASHA_UT_AUTO_RUN()
#define GASHA_UT_AUTO_RUN_AND_GET_RESULT(result)
#endif//GASHA_UNIT_TEST_AUTO_RUN_ENABLED

//ユニットテスト手動実行
#define GASHA_UT_MANUAL_RUN(attr) GASHA_ ut::container::run(attr)
#define GASHA_UT_MANUAL_RUN_BY_MODULE(module_name, attr) GASHA_ ut::container::runByModule(module_name, attr)
#define GASHA_UT_MANUAL_RUN_BY_GROUP(group_id, attr) GASHA_ ut::container::runByGroup(group_id, attr)
#define GASHA_UT_MANUAL_RUN_BY_MODULE_AND_GROUP(module_name, group_id, attr) GASHA_ ut::container::runByModuleAndGroup(module_name, group_id, attr)

//ユニットテスト結果操作処理
#define GASHA_UT_RETURN() {return GASHA_ ut::container::lastMissedCount();}//無条件に return を実行（戻り値として失敗件数を返す）
#define GASHA_UT_RETURN_WHEN_MISSED() {if(GASHA_ ut::container::lastMissedCount() > 0) return GASHA_ ut::container::lastMissedCount();}//失敗件数が 0 より大きかった時だけ return
#define GASHA_UT_EXIT_WHEN_MISSED() {if(GASHA_ ut::container::lastMissedCount() > 0) exit(GASHA_ ut::container::lastMissedCount());}//失敗件数が 0 より大きかった時だけ exit
#define GASHA_UT_ABORT_WHEN_MISSED() {if(GASHA_ ut::container::lastMissedCount() > 0) abort();}//失敗件数が 0 より大きかった時だけ abort
#define GASHA_UT_ASSERT_WHEN_MISSED() GASHA_SIMPLE_ASSERT(GASHA_ ut::container::lastMissedCount() == 0, "Unit-test is failed.")//失敗件数が 0 より大きかった時にアサーション違反
#define GASHA_UT_RESET_LAST_RESULT() GASHA_ ut::container::resetLastResult()//ユニットテストの結果をリセット

//ユニットテスト結果取得
#define GASHA_UT_LAST_PASSED_TOTAL() GASHA_ ut::container::lastPassedCount()//成功件数を取得
#define GASHA_UT_LAST_MISSED_TOTAL() GASHA_ ut::container::lastMissedCount()//失敗件数を取得

//ユニットテスト補助操作
//※出力先は標準コンソールがデフォルト。GASHA_UT_SET_CONSOLE() を使用して変更することが可能。
//※ユニットテストに付随してメッセージを表示したい場合に、GASHA_UT_PUT(), GASHA_UT_PRINTF() を使用。
#define GASHA_UT_SET_CONSOLE(console) GASHA_ ut::container::setConsole(console)
#define GASHA_UT_PUT(str) GASHA_ ut::container::put(str)
#define GASHA_UT_PRINTF(fmt, ...) GASHA_ ut::container::printf(fmt, __VA_ARGS__)

#else//GASHA_UNIT_TEST_ENABLED//ユニットテスト無効化時はまるごと無効化

//ユニットテスト用モジュールの開始マクロ
//※処理を無効化するために、ダミー関数化する。
//※未使用関数としてコンパイラが警告を発しないように、（インスタンス化されない）テンプレート関数とする。
#define GASHA_UT_BEGIN(_module_name, _group_id, attr) \
	GASHA_NAMESPACE_BEGIN; \
	namespace ut \
	{ \
		namespace module_##_module_name \
		{ \
			struct module \
			{ \
				template<class T> \
				void dummy(int& passed, int& missed, T dummy) \
				{

//ユニットテスト用モジュールの終了マクロ
#define GASHA_UT_END() \
				} \
			}; \
		}; \
	}; \
	GASHA_NAMESPACE_END;

//ユニットテスト登録
#define GASHA_UT_EXPECT(expr, ope, expect)
#define GASHA_UT_EXPECT_CHILD(expr, ope, expect)
#define GASHA_UT_EXPR(expr)
#define GASHA_UT_EXPR_CHILD(expr)
#define GASHA_UT_EXPR_WITH_RET(expr)
#define GASHA_UT_EXPR_WITH_RET_CHILD(expr)
#define GASHA_UT_EXPECT_EQ(expr, expect)
#define GASHA_UT_EXPECT_EQ_CHILD(expr, expect)
#define GASHA_UT_EXPECT_NE(expr, expect)
#define GASHA_UT_EXPECT_NE_CHILD(expr, expect)
#define GASHA_UT_EXPECT_GT(expr, expect)
#define GASHA_UT_EXPECT_GT_CHILD(expr, expect)
#define GASHA_UT_EXPECT_GE(expr, expect)
#define GASHA_UT_EXPECT_GE_CHILD(expr, expect)
#define GASHA_UT_EXPECT_LT(expr, expect)
#define GASHA_UT_EXPECT_LT_CHILD(expr, expect)
#define GASHA_UT_EXPECT_LE(expr, expect)
#define GASHA_UT_EXPECT_LE_CHILD(expr, expect)
//ユニットテスト自動実行
#define GASHA_UT_AUTO_RUN()
#define GASHA_UT_AUTO_RUN_AND_GET_RESULT(result)
//ユニットテスト手動実行
#define GASHA_UT_MANUAL_RUN(attr)
#define GASHA_UT_MANUAL_RUN_BY_MODULE(module_name, attr)
#define GASHA_UT_MANUAL_RUN_BY_GROUP(group_id, attr)
#define GASHA_UT_MANUAL_RUN_BY_MODULE_AND_GROUP(module_name, group_id, attr)
//ユニットテスト結果操作処理
#define GASHA_UT_RETURN()
#define GASHA_UT_RETURN_WHEN_MISSED()
#define GASHA_UT_EXIT_WHEN_MISSED()
#define GASHA_UT_ABORT_WHEN_MISSED()
#define GASHA_UT_ASSERT_WHEN_MISSED()
#define GASHA_UT_RESET_LAST_RESULT()
//ユニットテスト結果取得
#define GASHA_UT_LAST_PASSED_TOTAL() 0
#define GASHA_UT_LAST_MISSED_TOTAL() 0
//ユニットテスト補助操作
#define GASHA_UT_SET_CONSOLE(console)
#define GASHA_UT_PUT(msg)
#define GASHA_UT_PRINTF(fmt, ...)

#endif//GASHA_UNIT_TEST_ENABLED//ユニットテスト無効化時はまるごと無効化

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/unit_test.inl>

#endif//GASHA_INCLUDED_UNIT_TEST_H

// End of file

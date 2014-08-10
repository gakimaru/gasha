#pragma once
#ifndef GASHA_INCLUDED_UNIT_TEST_INL
#define GASHA_INCLUDED_UNIT_TEST_INL

//--------------------------------------------------------------------------------
// unit_test.inl
// ユニットテスト【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/unit_test.h>//ユニットテスト【宣言部】

#ifdef GASHA_UNIT_TEST_ENABLED//ユニットテスト無効化時はまるごと無効化

#include <gasha/string.h>//文字列処理

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ユニットテスト
//--------------------------------------------------------------------------------

namespace ut
{
	//----------------------------------------
	//ユニットテスト結果基底クラス

	//アクセッサ
	inline void resultBase::setResult(const bool result)
	{
		m_result = result;
	}
	inline bool resultBase::result() const
	{
		return m_result;
	}
	inline void resultBase::setHasResult(const bool has_result)
	{
		m_hasResult = has_result;
	}
	inline bool resultBase::hasResult() const
	{
		return m_hasResult;
	}
	inline bool resultBase::hasException() const
	{
		return m_hasException;
	}
	inline bool resultBase::hasExprStr() const
	{
		return m_hasExprStr;
	}
	inline bool resultBase::hasValueStr() const
	{
		return m_hasValueStr;
	}
	inline bool resultBase::hasOpeStr() const
	{
		return m_hasOpeStr;
	}
	inline bool resultBase::hasExpectStr() const
	{
		return m_hasExpectStr;
	}
	inline const char* resultBase::exprStr() const
	{
		return m_exprStr;
	}
	inline const char* resultBase::valueStr() const
	{
		return m_valuieStr;
	}
	inline const char* resultBase::opeStr() const
	{
		return m_opeStr;
	}
	inline const char* resultBase::expectStr() const
	{
		return m_expectStr;
	}
	inline const char* resultBase::exceptionStr() const
	{
		return m_exceptionStr;
	}

	//キャストオペレータ
	inline resultBase::operator bool() const
	{
		return m_hasResult ? m_result : true;
	}

	//コンストラクタ
	inline resultBase::resultBase() :
		m_result(true),
		m_hasResult(false),
		m_hasException(false),
		m_hasExprStr(false),
		m_hasValueStr(false),
		m_hasOpeStr(false),
		m_hasExpectStr(false),
		m_exprStr(),
		m_valuieStr(),
		m_opeStr(),
		m_expectStr(),
		m_exceptionStr()
	{}

	//デストラクタ
	inline resultBase::~resultBase()
	{}

	//----------------------------------------
	//ユニットテスト結果クラス

	//アクセッサ
	template<typename T>
	void result<T>::setValue(const T value)
	{
		m_value = value;
		m_hasValue = true;
		char buff[64];
		GASHA_ toStr<T>(buff, value);
		setValueStr(buff);
	}
	template<typename T>
	inline T result<T>::value() const
	{
		return m_value;
	}
	template<typename T>
	void result<T>::setExpect(const T expect)
	{
		m_expect = expect;
		m_hasExpect = true;
	}
	template<typename T>
	inline T result<T>::expect() const
	{
		return m_expectStr;
	}
	template<typename T>
	inline void result<T>::setOpeStrFromType(const unknownOpe_tag&)
	{
		setOpeStrFromId(UNKNOWN_OPE);
	}
	template<typename T>
	inline void result<T>::setOpeStrFromType(const EQ_tag&)
	{
		setOpeStrFromId(EQ_OPE);
	}
	template<typename T>
	inline void result<T>::setOpeStrFromType(const NE_tag&)
	{
		setOpeStrFromId(NE_OPE);
	}
	template<typename T>
	inline void result<T>::setOpeStrFromType(const GT_tag&)
	{
		setOpeStrFromId(GT_OPE);
	}
	template<typename T>
	inline void result<T>::setOpeStrFromType(const GE_tag&)
	{
		setOpeStrFromId(GE_OPE);
	}
	template<typename T>
	inline void result<T>::setOpeStrFromType(const LT_tag&)
	{
		setOpeStrFromId(LT_OPE);
	}
	template<typename T>
	inline void result<T>::setOpeStrFromType(const LE_tag&)
	{
		setOpeStrFromId(LE_OPE);
	}
	template<typename T>
	void result<T>::evaluate(const unknownOpe_tag& ope)
	{
		setOpeStrFromType(ope);
		setResult(false);
		setHasResult(false);
	}
	template<typename T>
	void result<T>::evaluate(const EQ_tag& ope)
	{
		setOpeStrFromType(ope);
		setResult(m_value == m_expect);
		setHasResult(true);
	}
	template<typename T>
	void result<T>::evaluate(const NE_tag& ope)
	{
		setOpeStrFromType(ope);
		setResult(m_value != m_expect);
		setHasResult(true);
	}
	template<typename T>
	void result<T>::evaluate(const GT_tag& ope)
	{
		setOpeStrFromType(ope);
		setResult(m_value > m_expect);
		setHasResult(true);
	}
	template<typename T>
	void result<T>::evaluate(const GE_tag& ope)
	{
		setOpeStrFromType(ope);
		setResult(m_value >= m_expect);
		setHasResult(true);
	}
	template<typename T>
	void result<T>::evaluate(const LT_tag& ope)
	{
		setOpeStrFromType(ope);
		setResult(m_value < m_expect);
		setHasResult(true);
	}
	template<typename T>
	void result<T>::evaluate(const LE_tag& ope)
	{
		setOpeStrFromType(ope);
		setResult(m_value <= m_expect);
		setHasResult(true);
	}

	//コンストラクタ
	template<typename T>
	inline result<T>::result(const T value, const T expect) :
		resultBase(),
		m_value(value),
		m_expect(expect),
		m_hasValue(true),
		m_hasExpect(true)
	{}
	template<typename T>
	inline result<T>::result(const T value) :
		resultBase(),
		m_value(0),
		m_expect(0),
		m_hasValue(false),
		m_hasExpect(false)
	{
		setValue(value);
	}
	//デフォルトコンストラクタ
	template<typename T>
	inline result<T>::result() :
		resultBase(),
		m_value(0),
		m_expect(0),
		m_hasValue(false),
		m_hasExpect(false)
	{}
	//デストラクタ
	template<typename T>
	inline result<T>::~result()
	{}

	//----------------------------------------
	//ユニットテストメインクラス

	//ユニットテスト結果登録
	template<typename T, typename OPE>
	result<T>* container::makeResultWithValueAndExpect(const T value, T expect, const OPE& ope, const char* expr_str, const char* expect_str)
	{
		result<T>* obj = s_scopeAllocator->newObj<result<T>>(value, expect);
		obj->setValue(value);
		obj->setExpect(expect);
		obj->setExprStr(expr_str);
		obj->setExpectStr(expect_str);
		obj->evaluate(ope);
		return obj;
	}
	template<typename OPE>
	result<int>* container::makeResultWithExpect(const OPE& ope, const char* expr_str, const char* expect_str)
	{
		result<int>* obj = s_scopeAllocator->newObj<result<int>>();
		obj->setExprStr(expr_str);
		obj->setExpectStr(expect_str);
		obj->setOpeStrFromType(ope);
		obj->setResult(false);
		obj->setHasResult(false);
		return obj;
	}
	template<typename T>
	result<T>* container::makeResultWithValue(const T value, const char* expr_str)
	{
		result<T>* obj = s_scopeAllocator->newObj<result<T>>(value);
		obj->setExprStr(expr_str);
		obj->setResult(false);
		obj->setHasResult(false);
		return obj;
	}
	template<typename T>
	void container::removeResult(result<T>* result)
	{
		if (result)
			s_scopeAllocator->deleteObj(result);
	}
	
	//ユニットテスト実行
	inline int container::run(const attr_type target_attr)
	{
		return runByModuleAndGroup(GASHA_UNIT_TEST_AUTO_RUN_TARGET_MODULE, GASHA_UNIT_TEST_AUTO_RUN_TARGET_GROUP, target_attr);
	}
	inline int container::runByModule(const char* target_module_name, const attr_type target_attr)
	{
		return runByModuleAndGroup(target_module_name, 0, target_attr);
	}
	inline int container::runByGroup(const int target_group_id, const attr_type target_attr)
	{
		return runByModuleAndGroup(nullptr, target_group_id, target_attr);
	}
	
	//ユニットテスト結果表示
	inline void container::put(const char* str)
	{
		if (!g_console)
			return;
		g_console->put(str);
	}
	template<typename... Tx>
	std::size_t container::printf(const char* fmt, Tx&&... args)
	{
		if (!g_console)
			return 0;
		std::size_t pos = 0;
		std::size_t ret = GASHA_ spprintf(g_workBuff, sizeof(g_workBuff), pos, fmt, std::forward<Tx>(args)...);
		g_console->put(g_workBuff);
		return ret;
	}
}//namespace ut

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_UNIT_TEST_ENABLED//ユニットテスト無効化時はまるごと無効化

#endif//GASHA_INCLUDED_UNIT_TEST_INL

// End of file

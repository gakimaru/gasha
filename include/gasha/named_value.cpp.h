#pragma	 once
#ifndef GASHA_INCLUDED_NAMED_VALUE_CPP_H
#define GASHA_INCLUDED_NAMED_VALUE_CPP_H

//--------------------------------------------------------------------------------
// named_value.cpp.h
// 名前付き定数【定数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/named_value.inl>//名前付き定数【インライン定数／テンプレート定数定義部】

#include <gasha/hash_table.cpp.h>//ハッシュテーブル【定数／実体定義部】
#include <gasha/linked_list.cpp.h>//双方向連結リスト【定数／実体定義部】
#include <gasha/str_pool.cpp.h>//双方向連結リスト【定数／実体定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//名前付き定数
//--------------------------------------------------------------------------------

namespace named_value
{
	//----------------------------------------
	//名前付き定数クラス

	//定数取得
	//※次の定数を取得
	template<class OPE_TYPE>
	GASHA_ crc32_t table<OPE_TYPE>::next(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, const int step) const
	{
		if (step < 0)
			return prev(group_name_crc, name_crc, -step);
		auto* group_info = findGroup(group_name_crc);
		GASHA_SIMPLE_ASSERT(group_info != nullptr, "group:0x%08x is not registered.", group_name_crc);
		if (!group_info)
			return 0;
		int _step = step;
		typename valueList_type::iterator ite = group_info->m_valueList.begin();
		if (name_crc != 0)
		{
			ite = group_info->m_valueList.findValue(name_crc);
			if (!ite.isExist())
				return 0;
			++_step;
		}
		GASHA_ crc32_t next_name_crc = 0;
		for (int i = 0; i < _step && ite.isExist(); ++i)
		{
			next_name_crc = ite->m_nameCrc;
			++ite;
		}
		return next_name_crc;
	}
	template<class OPE_TYPE>
	GASHA_ crc32_t table<OPE_TYPE>::next(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, const wraparound_tag&, const int step) const
	{
		if (step < 0)
			return prev(group_name_crc, name_crc, wraparound, -step);
		auto* group_info = findGroup(group_name_crc);
		GASHA_SIMPLE_ASSERT(group_info != nullptr, "group:0x%08x is not registered.", group_name_crc);
		if (!group_info)
			return 0;
		int _step = step;
		typename valueList_type::iterator ite = group_info->m_valueList.begin();
		if (name_crc != 0)
		{
			ite = group_info->m_valueList.findValue(name_crc);
			if (!ite.isExist())
				return 0;
			++_step;
		}
		GASHA_ crc32_t next_name_crc = 0;
		for (int i = 0; i < _step && ite.isExist(); ++i)
		{
			next_name_crc = ite->m_nameCrc;
			++ite;
			if (!ite.isExist())
				ite = group_info->m_valueList.begin();
		}
		return next_name_crc;
	}
	//※前の定数を取得
	template<class OPE_TYPE>
	GASHA_ crc32_t table<OPE_TYPE>::prev(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, const int step) const
	{
		if (step < 0)
			return next(group_name_crc, name_crc, -step);
		auto* group_info = findGroup(group_name_crc);
		GASHA_SIMPLE_ASSERT(group_info != nullptr, "group:0x%08x is not registered.", group_name_crc);
		if (!group_info)
			return 0;
		int _step = step;
		typename valueList_type::reverse_iterator rite = group_info->m_valueList.rbegin();
		if (name_crc != 0)
		{
			typename valueList_type::iterator ite = group_info->m_valueList.findValue(name_crc);
			if (!ite.isExist())
				return 0;
			++ite;
			rite = ite;
			++_step;
		}
		GASHA_ crc32_t prev_name_crc = 0;
		for (int i = 0; i < _step && rite.isExist(); ++i)
		{
			prev_name_crc = rite->m_nameCrc;
			++rite;
		}
		return prev_name_crc;
	}
	template<class OPE_TYPE>
	GASHA_ crc32_t table<OPE_TYPE>::prev(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, const wraparound_tag&, const int step) const
	{
		if (step < 0)
			return next(group_name_crc, name_crc, wraparound, -step);
		auto* group_info = findGroup(group_name_crc);
		GASHA_SIMPLE_ASSERT(group_info != nullptr, "group:0x%08x is not registered.", group_name_crc);
		if (!group_info)
			return 0;
		int _step = step;
		typename valueList_type::reverse_iterator rite = group_info->m_valueList.rbegin();
		if (name_crc != 0)
		{
			typename valueList_type::iterator ite = group_info->m_valueList.findValue(name_crc);
			if (!ite.isExist())
				return 0;
			++ite;
			rite = ite;
			++_step;
		}
		GASHA_ crc32_t prev_name_crc = 0;
		for (int i = 0; i < _step && rite.isExist(); ++i)
		{
			prev_name_crc = rite->m_nameCrc;
			++rite;
			if (!rite.isExist())
				rite = group_info->m_valueList.rbegin();
		}
		return prev_name_crc;
	}

	//定数グループ登録
	template<class OPE_TYPE>
	typename table<OPE_TYPE>::groupInfo* table<OPE_TYPE>::registAndGetGroup(const GASHA_ crc32_t group_name_crc)
	{
		groupInfo* group_info = m_groupTable->at(group_name_crc);
		if (!group_info)
			group_info = m_groupTable->emplace(group_name_crc, group_name_crc);
		if (!group_info)
			group_info = m_groupTable->at(group_name_crc);
		return group_info;
	}

	//定数登録解除
	template<class OPE_TYPE>
	bool table<OPE_TYPE>::unregist(const GASHA_ crc32_t name_crc)
	{
		valueInfo* value_info = const_cast<valueInfo*>(findValue(name_crc));
		if (!value_info)
			return false;
		groupInfo* group_info = m_groupTable->at(value_info->m_groupNameCrc);
		{
			auto lock = m_groupTable->lockScoped();
			if (group_info)
				group_info->m_valueList.remove(*value_info);
			m_valueTable->erase(name_crc);
		}
		return true;
	}

	//初期化メソッド（一回限り）
	template<class OPE_TYPE>
	void table<OPE_TYPE>::initializeOnce()
	{
		GASHA_ callConstructor<groupTable_type>(&m_groupTableBuff);
		GASHA_ callConstructor<valueTable_type>(&m_valueTableBuff);
	}

	//静的フィールド

	//ハッシュテーブル初期化済み
	template<class OPE_TYPE>
	std::once_flag table<OPE_TYPE>::m_initialized;
	//ハッシュテーブル（バッファ）
	//※アラインメントサイズ分バッファサイズを余計に確保する
	template<class OPE_TYPE>
	char table<OPE_TYPE>::m_groupTableBuff[table<OPE_TYPE>::GROUP_TABLE_BUFF_SIZE];
	template<class OPE_TYPE>
	char table<OPE_TYPE>::m_valueTableBuff[table<OPE_TYPE>::VALUE_TABLE_BUFF_SIZE];

	//明示的な初期化指定用
	template<class OPE_TYPE>
	const typename table<OPE_TYPE>::explicitInit_tag table<OPE_TYPE>::explicitInit;
	//ラップアラウンド演算指定用
	template<class OPE_TYPE>
	const typename table<OPE_TYPE>::wraparound_tag table<OPE_TYPE>::wraparound;
	//飽和演算指定用
	template<class OPE_TYPE>
	const typename table<OPE_TYPE>::saturation_tag table<OPE_TYPE>::saturation;

}//namespace named_value

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//名前付き定数の明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_namedValue(OPE_TYPE) \
	template class GASHA_ hash_table::container<typename GASHA_ named_value::table<OPE_TYPE>::groupTableOpe>; \
	template class GASHA_ hash_table::container<typename GASHA_ named_value::table<OPE_TYPE>::valueTableOpe>; \
	template class GASHA_ linked_list::container<typename GASHA_ named_value::table<OPE_TYPE>::valueListOpe>; \
	template class GASHA_ named_value::table<OPE_TYPE>;

//--------------------------------------------------------------------------------
//【注】明示的インスタンス化に失敗する場合
// ※このコメントは、「明示的なインスタンス化マクロ」が定義されている全てのソースコードに
// 　同じ内容のものをコピーしています。
//--------------------------------------------------------------------------------
//【原因①】
// 　対象クラスに必要なインターフェースが実装されていない。
//
// 　例えば、ソート処理に必要な「bool operator<(const value_type&) const」か「friend bool operator<(const value_type&, const value_type&)」や、
// 　探索処理に必要な「bool operator==(const key_type&) const」か「friend bool operator==(const value_type&, const key_type&)」。
//
// 　明示的なインスタンス化を行う場合、実際に使用しない定数のためのインターフェースも確実に実装する必要がある。
// 　逆に言えば、明示的なインスタンス化を行わない場合、使用しない定数のためのインターフェースを実装する必要がない。
//
//【対策１】
// 　インターフェースをきちんと実装する。
// 　（無難だが、手間がかかる。）
//
//【対策２】
// 　明示的なインスタンス化を行わずに、.cpp.h をテンプレート使用前にインクルードする。
// 　（手間がかからないが、コンパイル時の依存ファイルが増えるので、コンパイルが遅くなる可能性がある。）
//
//--------------------------------------------------------------------------------
//【原因②】
// 　同じ型のインスタンスが複数作成されている。
//
// 　通常、テンプレートクラス／定数の同じ型のインスタンスが複数作られても、リンク時に一つにまとめられるため問題がない。
// 　しかし、一つのソースファイルの中で複数のインスタンスが生成されると、コンパイラによってはエラーになる。
//   GCCの場合のエラーメッセージ例：（VC++ではエラーにならない）
// 　  source_file.cpp.h:114:17: エラー: duplicate explicit instantiation of ‘class templateClass<>’ [-fpermissive]
//
//【対策１】
// 　別のファイルに分けてインスタンス化する。
// 　（コンパイルへの影響が少なく、良い方法だが、無駄にファイル数が増える可能性がある。）
//
//【対策２】
// 　明示的なインスタンス化を行わずに、.cpp.h をテンプレート使用前にインクルードする。
// 　（手間がかからないが、コンパイル時の依存ファイルが増えるので、コンパイルが遅くなる可能性がある。）
//
//【対策３】
// 　GCCのコンパイラオプションに、 -fpermissive を指定し、エラーを警告に格下げする。
// 　（最も手間がかからないが、常時多数の警告が出る状態になりかねないので注意。）
//--------------------------------------------------------------------------------

#endif//GASHA_INCLUDED_NAMED_VALUE_CPP_H

// End of file

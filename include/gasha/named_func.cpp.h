#pragma	 once
#ifndef GASHA_INCLUDED_NAMED_FUNC_CPP_H
#define GASHA_INCLUDED_NAMED_FUNC_CPP_H

//--------------------------------------------------------------------------------
// named_func.cpp.h
// 名前付き関数【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/named_func.inl>//名前付き関数【インライン関数／テンプレート関数定義部】

#include <gasha/hash_table.cpp.h>//ハッシュテーブル【関数／実体定義部】
#include <gasha/singly_linked_list.cpp.h>//片方向連結リスト【関数／実体定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//名前付き関数
//--------------------------------------------------------------------------------

namespace named_func
{
	//----------------------------------------
	//名前付き関数クラス

	//関数グループ登録
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

	//関数登録解除
	template<class OPE_TYPE>
	bool table<OPE_TYPE>::unregist(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc)
	{
		funcInfo* func_info = const_cast<funcInfo*>(findFunc(group_name_crc, name_crc));
		if (!func_info)
			return false;
		groupInfo* group_info = const_cast<groupInfo*>(findGroup(group_name_crc));
		{
			auto lock = m_groupTable->lockScoped();
			if (group_info)
				group_info->m_funcList.remove(*func_info);
			m_funcTable->erase(func_info->m_key.m_key);
		}
		return true;
	}

	//初期化メソッド（一回限り）
	template<class OPE_TYPE>
	void table<OPE_TYPE>::initializeOnce()
	{
		GASHA_ callConstructor<groupTable_type>(&m_groupTableBuff);
		GASHA_ callConstructor<funcTable_type>(&m_funcTableBuff);
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
	char table<OPE_TYPE>::m_funcTableBuff[table<OPE_TYPE>::FUNC_TABLE_BUFF_SIZE];

	//明示的な初期化指定用
	template<class OPE_TYPE>
	const typename table<OPE_TYPE>::explicitInit_tag table<OPE_TYPE>::explicitInit;
	//const関数指定用
	template<class OPE_TYPE>
	const typename table<OPE_TYPE>::constFunc_tag table<OPE_TYPE>::constFunc;
	//書き込み可能関数指定用
	template<class OPE_TYPE>
	const typename table<OPE_TYPE>::writableFunc_tag table<OPE_TYPE>::writableFunc;

}//namespace named_func

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//名前付き関数の明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_namedFunc(OPE_TYPE) \
	template class GASHA_ hash_table::container<typename GASHA_ named_func::table<OPE_TYPE>::groupTableOpe>; \
	template class GASHA_ hash_table::container<typename GASHA_ named_func::table<OPE_TYPE>::funcTableOpe>; \
	template class GASHA_ singly_linked_list::container<typename GASHA_ named_func::table<OPE_TYPE>::funcListOpe>; \
	template class GASHA_ named_func::table<OPE_TYPE>;

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
// 　明示的なインスタンス化を行う場合、実際に使用しない関数のためのインターフェースも確実に実装する必要がある。
// 　逆に言えば、明示的なインスタンス化を行わない場合、使用しない関数のためのインターフェースを実装する必要がない。
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
// 　通常、テンプレートクラス／関数の同じ型のインスタンスが複数作られても、リンク時に一つにまとめられるため問題がない。
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

#endif//GASHA_INCLUDED_NAMED_FUNC_CPP_H

// End of file

#pragma	 once
#ifndef GASHA_INCLUDED_NAMED_REF_CPP_H
#define GASHA_INCLUDED_NAMED_REF_CPP_H

//--------------------------------------------------------------------------------
// named_ref.cpp.h
// 名前付きデータ参照【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/named_ref.inl>//名前付きデータ参照【インライン関数／テンプレート関数定義部】

#include <gasha/hash_table.cpp.h>//ハッシュテーブル【関数／実体定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//名前付きデータ参照
//--------------------------------------------------------------------------------

//----------------------------------------
//名前付きデータ参照クラス

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//開番地法ハッシュテーブルコンテナの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_namedRef(IDENTIFIER_TYPE, _TABLE_SIZE) \
	template class GASHA_ hash_table::container<typename GASHA_ namedRef<IDENTIFIER_TYPE, _TABLE_SIZE>::tableOpe, _TABLE_SIZE>; \
	template class GASHA_ namedRef<IDENTIFIER_TYPE, _TABLE_SIZE>;

#define GASHA_INSTANCING_namedRef_withLock(IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY) \
	template class GASHA_ hash_table::container<typename GASHA_ namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::tableOpe, _TABLE_SIZE>; \
	template class GASHA_ namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>;

//--------------------------------------------------------------------------------
//【注】明示的インスタンス化に失敗する場合
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
// ※このコメントは、「明示的なインスタンス化マクロ」が定義されている全てのソースコードに
// 　同じ内容のものをコピーしています。

#endif//GASHA_INCLUDED_NAMED_REF_CPP_H

// End of file

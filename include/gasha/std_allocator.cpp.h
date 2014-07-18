#pragma once
#ifndef GASHA_INCLUDED_STD_ALLOCATOR_CPP_H
#define GASHA_INCLUDED_STD_ALLOCATOR_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// std_allocator.cpp.h
// 標準アロケータ【関数定義部】
//
// ※クラスのインスタンス化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。
// 　（コンパイル・リンク時間への影響を気にしないならOK）
// ※明示的なインスタンス化を避けたい場合は、ヘッダーファイルと共にインクルード。
// 　（この場合、実際に使用するメンバー関数しかインスタンス化されないので、対象クラスに不要なインターフェースを実装しなくても良い）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/std_allocator.inl>//標準アロケータ【インライン関数／テンプレート関数定義部】

#include <gasha/string.h>//文字列処理：spprintf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準アロケータクラス

//デバッグ情報作成
template<class LOCK_TYPE, class IMPL>
std::size_t stdAllocator<LOCK_TYPE, IMPL>::debugInfo(char* message) const
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	std::size_t size = 0;
	GASHA_ spprintf(message, size, "----- Debug-info for stdAllocator -----\n");
	GASHA_ spprintf(message, size, "maxSize=%d, size=%d, remain=%d\n", maxSize(), this->size(), remain());
	GASHA_ spprintf(message, size, "---------------------------------------\n");
	return size;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//----------------------------------------
//明示的なインスタンス化

//標準アロケータの明示的なインスタンス化用マクロ

//※アラインメントなし＋ロックなし版
#define GASHA_INSTANCING_stdAllocator() \
	template class GASHA_ stdAllocator<GASHA_ dummyLock, GASHA_ stdAllocatorImpl_NoAlign>;
//※アラインメントなし＋ロック指定版
#define GASHA_INSTANCING_stdAllocator_withLock(LOCK_TYPE) \
	template class GASHA_ stdAllocator<LOCK_TYPE, GASHA_ stdAllocatorImpl_NoAlign>;

//※アラインメントあり＋ロックなし版
#define GASHA_INSTANCING_stdAlignAllocator() \
	template class GASHA_ stdAllocator<GASHA_ dummyLock, GASHA_ stdAllocatorImpl_Align>;
//※アラインメントあり＋ロック指定版
#define GASHA_INSTANCING_stdAlignAllocator_withLock(LOCK_TYPE) \
	template class GASHA_ stdAllocator<LOCK_TYPE, GASHA_ stdAllocatorImpl_Align>;

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
// 　  source_file.cpp.h:114:17: エラー: duplicate explicit instantiation of ‘class gasha::templateClass<>’ [-fpermissive]
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

#endif//GASHA_INCLUDED_STD_ALLOCATOR_CPP_H

// End of file

#pragma once
#ifndef GASHA_INCLUDED_SINGLETON_CPP_H
#define GASHA_INCLUDED_SINGLETON_CPP_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// singleton.cpp.h
// シングルトン【関数／実体定義部】
//
// ※クラスのインスタンス化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。
// 　（コンパイル・リンク時間への影響を気にしないならOK）
// ※明示的なインスタンス化を避けたい場合は、ヘッダーファイルと共にインクルード。
// 　（この場合、実際に使用するメンバー関数しかインスタンス化されないので、対象クラスに不要なインターフェースを実装しなくても良い）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/singleton.inl>//シングルトン【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シングルトン
//--------------------------------------------------------------------------------

//----------------------------------------
//固定バッファシングルトン
namespace _private
{
	//静的フィールド：静的シングルトンインスタンス用バッファ
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	unsigned char singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::m_staticInstanceBuff[sizeof(typename singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::class_type)];

	//静的フィールド：静的シングルトンインスタンス参照
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	typename singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::class_type* singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::m_staticInstanceRef = nullptr;

	//静的フィールド：インスタンス生成済み
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	std::atomic<bool> singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::m_staticInstanceIsCreated(false);

	//静的フィールド：静的ロックオブジェクト
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	typename singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::lock_type singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::m_staticLock;

	//静的フィールド：デバッグ用オブジェクト
	template<class TARGET_CLASS, class LOCK_POLICY, class DEBUG_POLICY>
	typename singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::debug_type singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>::m_staticDebug;
}//namepace _private

//----------------------------------------
//明示的なインスタンス化

//通常シングルトンの明示的なインスタンス化用マクロ
//※ロック制御クラスとデバッグ処理クラスはクラス TARGET_CLASS の中で定義する。
//　（例１）
//　    typedef sharedSpinLock spilock_type;//ロック制御クラス：共有スピンロック
//      typedef singletonDebug debug_type;//シングルトンデバッグ用処理クラス
//　（例２）
//　    typedef dummySharedLock spilock_type;//ロック制御クラス：ダミー共有ロック（ロックしない）
//      typedef dummySingletonDebug debug_type;//シングルトンデバッグ用ダミー処理クラス（デバッグしない）
#define GASHA_INSTANCING_singleton(TARGET_CLASS) \
	template class GASHA_ _private::singleton<TARGET_CLASS, TARGET_CLASS::lock_type, TARGET_CLASS::debug_type>;

//シンプルシングルトンの明示的なインスタンス化用マクロ
//※ロックなし、デバッグなし版
#define GASHA_INSTANCING_simpleSingleton(TARGET_CLASS) \
	template class GASHA_ _private::singleton<TARGET_CLASS, GASHA_ dummySharedLock, GASHA_ dummySingletonDebug>;
//※ロック指定、デバッグなし版
#define GASHA_INSTANCING_simpleSingleton_withLock(TARGET_CLASS, LOCK_POLICY) \
	template class GASHA_ _private::singleton<TARGET_CLASS, LOCK_POLICY, GASHA_ dummySingletonDebug>;
//※ロックなし、デバッグ指定版
#define GASHA_INSTANCING_simpleSingleton_withDebug(TARGET_CLASS, DEBUG_POLICY) \
	template class GASHA_ _private::singleton<TARGET_CLASS, GASHA_ dummySharedLock, DEBUG_POLICY>;
//※ロック指定、デバッグ指定版
#define GASHA_INSTANCING_simpleSingleton_withLock_withDebug(TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY) \
	template class GASHA_ _private::singleton<TARGET_CLASS, LOCK_POLICY, DEBUG_POLICY>;

//※別途、必要に応じてシングルトンデバッグ用処理の明示的なインスタンス化も必要
//    GASHA_INSTANCING_singletonDebug(_MAX_RECORDS);//ロックなし版
//    GASHA_INSTANCING_singletonDebug_withLock(_MAX_RECORDS, LOCK_POLICY);//ロック指定版

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

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SINGLETON_CPP_H

// End of file

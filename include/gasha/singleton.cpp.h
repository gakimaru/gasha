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
// Gakimaru's researched and standard library for C++ - GASHA
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
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	unsigned char singleton<T, LOCK_TYPE, DEBUG_TYPE>::m_staticInstanceBuff[sizeof(typename singleton<T, LOCK_TYPE, DEBUG_TYPE>::class_type)];

	//静的フィールド：静的シングルトンインスタンス参照
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	T* singleton<T, LOCK_TYPE, DEBUG_TYPE>::m_staticInstanceRef = nullptr;

	//静的フィールド：インスタンス生成済み
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	std::atomic<bool> singleton<T, LOCK_TYPE, DEBUG_TYPE>::m_staticInstanceIsCreated(false);

	//静的フィールド：静的ロックオブジェクト
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	typename singleton<T, LOCK_TYPE, DEBUG_TYPE>::lock_type singleton<T, LOCK_TYPE, DEBUG_TYPE>::m_staticLock;

	//静的フィールド：デバッグ用オブジェクト
	template<class T, class LOCK_TYPE, class DEBUG_TYPE>
	typename singleton<T, LOCK_TYPE, DEBUG_TYPE>::debug_type singleton<T, LOCK_TYPE, DEBUG_TYPE>::m_staticDebug;
}//namepace _private

//----------------------------------------
//明示的なインスタンス化

//シングルトンの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_singleton(T) \
	template class _private::singleton<T, T::lock_type, T::debug_type>;

//シンプルシングルトンの明示的なインスタンス化用マクロ
#define GASHA_INSTANCING_simpleSingleton(T) \
	template class _private::singleton<T, GASHA_ dummySharedLock, GASHA_ dummySingletonDebug>;

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SINGLETON_CPP_H

// End of file

#pragma once
#ifndef GASHA_INCLUDED_SHARED_POOL_ALLOCATOR_INL
#define GASHA_INCLUDED_SHARED_POOL_ALLOCATOR_INL

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_allocator.h
// マルチスレッド共有プールアロケータ【インライン関数／テンプレート関数定義部】
//
// ※クラスの操作が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shared_pool_allocator.h>//マルチスレッド共有プールアロケータ【宣言部】

#include <utility>//C++11 std::forward

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//メモリ確保とコンストラクタ呼び出し
//※既定の型
template<class T, std::size_t _POOL_SIZE, class LOCK_TYPE>
template<typename...Tx>
typename sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::value_type* sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::newObj(Tx&&... args)
{
	void* p = alloc();
	if (!p)
		return nullptr;
	return new(p)value_type(std::forward<Tx>(args)...);
}
//※型指定
template<class T, std::size_t _POOL_SIZE, class LOCK_TYPE>
template<typename ObjType, typename...Tx>
ObjType* sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::newObj(Tx&&... args)
{
	static_assert(sizeof(ObjType) <= VALUE_SIZE, "sizeof(ObjType) is too large.");
	void* p = alloc();
	if (!p)
		return nullptr;
	return new(p)ObjType(std::forward<Tx>(args)...);
}

//メモリ解放とデストラクタ呼び出し
template<class T, std::size_t _POOL_SIZE, class LOCK_TYPE>
template<typename ObjType>
bool sharedPoolAllocator<T, _POOL_SIZE, LOCK_TYPE>::deleteObj(ObjType* p)
{
	GASHA_ lock_guard<lock_type> lock(m_lock);//ロック（スコープロック）
	const std::size_t index = ptrToIndex(p);//ポインタをインデックスに変換
	if (index == INVALID_INDEX)
		return false;
	p->~ObjType();//デストラクタ呼び出し
	operator delete(p, p);//（作法として）deleteオペレータ呼び出し
	return free(p, index);
}

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

#endif//GASHA_INCLUDED_SHARED_POOL_ALLOCATOR_INL

// End of file

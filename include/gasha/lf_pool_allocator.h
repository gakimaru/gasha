#pragma once
#ifndef __LOCKFREE_POOL_ALLOCATOR_H_
#define __LOCKFREE_POOL_ALLOCATOR_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_pool_allocator.h
// ロックフリープールアロケータ【宣言部】
//
// ※クラスをインスタンス化する際は、別途下記のファイルをインクルードする必要あり
//
//   ・lf_pool_allocator.inl   ... 【インライン関数／テンプレート関数定義部】
//                                 クラスの操作が必要な場所でインクルード。
//   ・lf_pool_allocator.cpp.h ... 【関数定義部】
//                                 クラスの実体化が必要な場所でインクルード。
//
// ※面倒なら三つまとめてインクルードして使用しても良いが、分けた方が、
// 　コンパイル・リンク時間の短縮、および、クラス修正時の影響範囲の抑制になる。
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <cstddef>//std::size_t
#include <cstdint>//++11 std::uint32_t
#include <atomic>//C++11 std::atomic

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <functional> <bitset> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <functional>//std::function

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリープールアロケータクラス
//※ABA問題対策あり（プールの再利用インデックスに8ビットのタグを付けて管理）
//※プール可能な要素数の最大は2^(32-8)-2 = 16,777,214個（2^(32-8)-1だと、値がINVALID_INDEXになる可能性があるのでNG）
template<class T, std::size_t _POOL_SIZE>
class lfPoolAllocator
{
	static_assert(sizeof(T) >= 4, "sizeof(T) is too small.");
	static_assert(_POOL_SIZE < 0x00ffffff, "POOL is too large.");

public:
	//型
	typedef T value_type;//値型
	typedef std::uint32_t index_type;//インデックス型

	//再利用プール型
	struct recycable_t
	{
		std::atomic<std::uint32_t> m_next_index;//再利用プール連結インデックス
	};

public:
	//定数
	static const std::size_t POOL_SIZE = _POOL_SIZE;//プールサイズ（プールする個数）
	static const std::size_t VALUE_SIZE = sizeof(value_type);//値のサイズ
	static const index_type INVALID_INDEX = 0xffffffff;//無効なインデックス
	static const index_type DIRTY_INDEX = 0xfefefefe;//再利用プール連結インデックス削除用

public:
	//メソッド
	
	//メモリ確保
	void* alloc();

private:
	//メモリ解放（共通処理）
	bool free(void* p, const index_type index);
	
	//ポインタをインデックスに変換
	index_type ptrToIndex(void* p);

public:
	//メモリ解放
	bool free(void* p);

	//メモリ確保とコンストラクタ呼び出し
	//※既定の型
	template<typename...Tx>
	value_type* newObj(Tx&&... args);
	//※型指定
	template<typename ObjType, typename...Tx>
	ObjType* newObj(Tx&&... args);
	//※ムーブ／コピー
	value_type* newMoveObj(value_type&& org);//※ムーブ版
	value_type* newCopyObj(const value_type& org);//※コピー版

	//メモリ解放とデストラクタ呼び出し
	//※型指定
	template<typename ObjType>
	bool deleteObj(ObjType* p);

	//デバッグ情報表示
	void printDebugInfo(std::function<void(const value_type& value)> print_node);

public:
	//コンストラクタ
	lfPoolAllocator();
	//デストラクタ
	~lfPoolAllocator();

private:
	//フィールド
	char m_pool[POOL_SIZE][VALUE_SIZE];//プールバッファ ※先頭に配置してクラスのアライメントと一致させる
	std::atomic<index_type> m_vacantHead;//空きプールの先頭インデックス
	std::atomic<index_type> m_recyclableHead;//再利用プールの先頭インデックス
	std::atomic<unsigned char> m_tag;//ABA問題対策用のタグ
	std::atomic<char> m_using[POOL_SIZE];//使用中インデックス（二重解放判定＆保険の排他制御用）  ※std::bitset使用不可
	//std::atomic<std::size_t> m_usingCount;//使用中の数（デバッグ用）※必須の情報ではない
	//std::atomic<std::size_t> m_allocCount[POOL_SIZE];//アロケート回数（デバッグ用）※必須の情報ではない
	//std::atomic<std::size_t> m_freeCount[POOL_SIZE];//フリー回数（デバッグ用）※必須の情報ではない
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルードする場合
#ifdef GASHA_LF_POOL_ALLOCATOR_ALLWAYS_TOGETHER_INL
#include <gasha/lf_pool_allocator.inl>
#endif//GASHA_LF_POOL_ALLOCATOR_ALLWAYS_TOGETHER_INL

//.hファイルのインクルードに伴い、常に.cp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_LF_POOL_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H
#include <gasha/lf_pool_allocator.cpp.h>
#endif//GASHA_LF_POOL_ALLOCATOR_ALLWAYS_TOGETHER_CPP_H

#endif//__LOCKFREE_POOL_ALLOCATOR_H_

// End of file

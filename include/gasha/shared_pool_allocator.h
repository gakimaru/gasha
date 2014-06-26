#pragma once
#ifndef __SHARED_POOL_ALLOCATOR_H_
#define __SHARED_POOL_ALLOCATOR_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_allocator.h
// マルチスレッド共有プールアロケータ【宣言部】
//
// ※クラスをインスタンス化する際は、別途下記のファイルをインクルードする必要あり
//
//   ・shared_pool_allocator.inl   ... 【インライン関数／テンプレート関数定義部】
//                                     クラスの操作が必要な場所でインクルード。
//   ・shared_pool_allocator.cpp.h ... 【関数定義部】
//                                     クラスの実体化が必要な場所でインクルード。
//
// ※面倒なら三つまとめてインクルードして使用しても良いが、分けた方が、
// 　コンパイル・リンク時間の短縮、および、クラス修正時の影響範囲の抑制になる。
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/spin_lock.h>//スピンロック

#include <cstddef>//std::size_t

//【VC++】例外を無効化した状態で <functional> <bitset> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <functional>//std::function
#include <bitset>//std::bitset

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//マルチスレッド共有プールアロケータクラス
//※デフォルトでは排他制御しないので、必要に応じてテンプレートパラメータに
//　spinLock や std::mutex などの適切な同期オブジェクト型の指定が必要。
//※排他制御が不要か滅多に必要がない場合は、ロックフリー版よりも速い。
template<class T, std::size_t _POOL_SIZE, class LOCK_TYPE = GASHA_ spinLock>
class sharedPoolAllocator
{
	static_assert(sizeof(T) >= 4, "sizeof(T) is too small.");

public:
	//型
	typedef T value_type;//値型
	typedef LOCK_TYPE lock_type;//ロック型

	//再利用プール型
	struct recycable_t
	{
		std::size_t m_next_index;//再利用プール連結インデックス
	};

public:
	//定数
	static const std::size_t POOL_SIZE = _POOL_SIZE;//プールサイズ（プールする個数）
	static const std::size_t VALUE_SIZE = sizeof(value_type);//値のサイズ
	static const std::size_t INVALID_INDEX = 0xffffffff;//無効なインデックス
	static const std::size_t DIRTY_INDEX = 0xfefefefe;//再利用プール連結インデックス削除用

public:
	//メソッド

	//メモリ確保
	void* alloc();

private:
	//メモリ解放（共通処理）
	//※ロック取得は呼び出し元で行う
	bool free(void* p, const std::size_t index);
	
	//ポインタをインデックスに変換
	std::size_t ptrToIndex(void* p);

public:
	//メモリ解放
	bool free(void* p);
	
	//メモリ確保とコンストラクタ呼び出し
	//※既定の型
	template<typename...Tx>
	value_type* newObj(Tx... args);
	//※型指定
	template<typename ObjType, typename...Tx>
	ObjType* newObj(Tx... args);
	//※ムーブ／コピー
	value_type* newMoveObj(value_type&& org);//※ムーブ版
	value_type* newCopyObj(value_type& org);//※コピー版

	//メモリ解放とデストラクタ呼び出し
	//※型指定
	template<typename ObjType>
	bool deleteObj(ObjType* p);

	//デバッグ情報表示
	void printDebugInfo(std::function<void(const value_type& value)> print_node);

public:
	//コンストラクタ
	sharedPoolAllocator();
	//デストラクタ
	~sharedPoolAllocator();

private:
	//フィールド
	char m_pool[POOL_SIZE][VALUE_SIZE];//プールバッファ ※先頭に配置してクラスのアライメントと一致させる
	std::size_t m_vacantHead;//空きプールの先頭インデックス
	std::size_t m_recyclableHead;//再利用プールの先頭インデックス
	std::bitset<POOL_SIZE> m_using;//使用中インデックス（二重解放判定用）
	//std::size_t m_usingCount;//使用中の数（デバッグ用）※必須の情報ではない
	lock_type m_lock;//ロックオブジェクト
};

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__SHARED_POOL_ALLOCATOR_H_

// End of file

#pragma once
#ifndef GASHA_INCLUDED_SHARED_STACK_H
#define GASHA_INCLUDED_SHARED_STACK_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// shared_stack.h
// マルチスレッド共有スタック【宣言部】
//
// ※クラスをインスタンス化する際は、別途下記のファイルをインクルードする必要あり
//
//   ・shared_stack.inl   ... 【インライン関数／テンプレート関数定義部】
//                            クラスの操作が必要な場所でインクルード。
//   ・shared_stack.cpp.h ... 【関数定義部】
//                            クラスの実体化が必要な場所でインクルード。
//
// ※面倒なら三つまとめてインクルードして使用しても良いが、分けた方が、
// 　コンパイル・リンク時間の短縮、および、クラス修正時の影響範囲の抑制になる。
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shared_pool_allocator.h>//マルチスレッド共有プールアロケータ
#include <gasha/spin_lock.h>//スピンロック
#include <gasha/dummy_lock.h>//ダミーロック

#include <utility>//C++11 std::move
#include <cstddef>//std::size_t

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <functional> <bitset> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <functional>//std::function

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//マルチスレッド共有スタッククラス
template<class T, std::size_t POOL_SIZE, class LOCK_TYPE = GASHA_ spinLock>
class sharedStack
{
public:
	//型
	typedef T value_type;//値型
	typedef LOCK_TYPE lock_type;//ロック型

	//スタック型
	struct stack_t
	{
		stack_t* m_next;//次のノード
		value_type m_value;//値
		//コンストラクタ
		inline stack_t(value_type&& value) :
			m_value(std::move(value))
		{}
		inline stack_t(value_type& value) :
			m_value(value)
		{}
	};

public:
	//メソッド

	//プッシュ
private:
	inline bool _push(stack_t* new_node);
public:
	bool push(value_type&& value);//※ムーブ版
	bool push(value_type& value);//※コピー版

	//ポップ
	bool pop(value_type& value);

	//デバッグ情報表示
	void printDebugInfo(std::function<void(const value_type& value)> print_node);

private:
	//初期化
	void initialize();
	//終了
	void finalize();

public:
	//コンストラクタ
	sharedStack();
	//デストラクタ
	~sharedStack();
private:
	//フィールド
	sharedPoolAllocator<stack_t, POOL_SIZE, GASHA_ dummyLock> m_allocator;//プールアロケータ（プールアロケータ自体はロック制御しない）
	stack_t* m_head;//スタックの先頭
	lock_type m_lock;//ロックオブジェクト
};

//明示的なインスタンス化用マクロ
#define INSTANCING_sharedStack(T, _POOL_SIZE) \
	template class sharedStack<T, _POOL_SIZE>;
#define INSTANCING_sharedStack_withLock(T, _POOL_SIZE, LOCK_TYPE) \
	template class sharedStack<T, _POOL_SIZE, LOCK_TYPE>;

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルードする場合
#ifdef GASHA_SHARED_STACK_ALLWAYS_TOGETHER_INL
#include <gasha/shared_stack.inl>
#endif//GASHA_SHARED_STACK_ALLWAYS_TOGETHER_INL

//.hファイルのインクルードに伴い、常に.cp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_SHARED_STACK_ALLWAYS_TOGETHER_CPP_H
#include <gasha/shared_stack.cpp.h>
#endif//GASHA_SHARED_STACK_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_SHARED_STACK_H

// End of file

#pragma once
#ifndef GASHA_INCLUDED_LOCKFREE_STACK_H
#define GASHA_INCLUDED_LOCKFREE_STACK_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lockfree_stack.h
// ロックフリースタック【宣言部】
//
// ※クラスをインスタンス化する際は、別途 .cpp.h ファイルをインクルードする必要あり。
// ※明示的なインスタンス化を避けたい場合は、ヘッダーファイルと共にインクルード。
// 　（この場合、実際に使用するメンバー関数しかインスタンス化されないので、対象クラスに不要なインターフェースを実装しなくても良い）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_pool_allocator.h>//ロックフリープールアロケータ
#include <gasha/tagged_ptr.h>//タグ付きポインタ
#include <gasha/basic_math.h>//基本算術：calcStaticMSB()

#include <utility>//C++11 std::move
#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t, std::uint64_t
#include <atomic>//C++11 std::atomic

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <functional> <bitset> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <functional>//std::function

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリースタッククラス
//※ABA問題対策あり（タグ付きポインタ型使用）
//
//【テンプレート引数の説明】
//・T ... スタックのデータ型
//・_POOL_SIZE ... 同時にスタック可能なデータの最大個数
//※以下、ABA問題対策のためのタグ付きポインタのパラメータ（詳しい説明は tagged_ptr.h 参照）
//・_TAGGED_PTR_TAG_BITS  ... taggedPtr クラスのテンプレート引数 _TAG_BITS　※0でデータ型 T のアラインメントサイズに合わせる（デフォルト）
//・_TAGGED_PTR_TAG_SHIFT ... taggedPtr クラスのテンプレート引数 _TAG_SHIFT
//・TAGGED_PTR_VALUE_TYPE ... taggedPtr クラスのテンプレート引数 VALUE_TYPE
//・TAGGED_PTR_TAG_TYPE   ... taggedPtr クラスのテンプレート引数 TAG_TYPE
//※デフォルトでは、データ型 T のアライメントの隙間にタグを挿入するので、タグのサイズは2ビット程度になる。
//　スレッドが混み合う場面では、ABA問題の対策不十分で、データ破壊が起こる可能性がある。（さほど混み合わないなら十分）
//　推奨設定は tagged_ptr.h 参照。
//
template<class T, std::size_t _POOL_SIZE, std::size_t _TAGGED_PTR_TAG_BITS = 0, int _TAGGED_PTR_TAG_SHIFT = 0, typename TAGGED_PTR_VALUE_TYPE = std::uint64_t, typename TAGGED_PTR_TAG_TYPE = std::uint32_t>
class lfStack
{
public:
	//定数
	static const std::size_t POOL_SIZE = _POOL_SIZE;//プールアロケータのプールサイズ（プールする個数）
	static const std::size_t TAGGED_PTR_TAG_BITS = _TAGGED_PTR_TAG_BITS == 0 ? GASHA_ calcStaticMSB<alignof(T)>::value : _TAGGED_PTR_TAG_BITS;//タグ付きポインタのタグのビット長
	static const int TAGGED_PTR_TAG_SHIFT = _TAGGED_PTR_TAG_SHIFT;//タグ付きポインタのタグの位置

public:
	//型
	typedef T value_type;//値型

	//スタック型用タグ付きポインタ型
	struct stack_t;
	typedef TAGGED_PTR_VALUE_TYPE tagged_ptr_value_type;//タグ付きポインタの値の型
	typedef TAGGED_PTR_TAG_TYPE tagged_ptr_tag_type;//タグ付きポインタのタグの型
	typedef GASHA_ taggedPtr<stack_t, TAGGED_PTR_TAG_BITS, TAGGED_PTR_TAG_SHIFT, TAGGED_PTR_VALUE_TYPE, TAGGED_PTR_TAG_TYPE> stack_ptr_t;
	
	//アロケータ型
	typedef GASHA_ lfPoolAllocator_withType<stack_t, POOL_SIZE> allocator_type;//ロックフリープールアロケータ

	//スタック型
	struct stack_t
	{
		value_type m_value;//値
		std::atomic<stack_ptr_t> m_next;//次のノード
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

	//デバッグ情報作成
	//※十分なサイズのバッファを渡す必要あり。
	//※使用したバッファのサイズを返す。
	//※作成中、他のスレッドで操作が発生すると、不整合が生じる可能性がある点に注意
	std::size_t debugInfo(char* message, std::function<std::size_t(char* message, const value_type& value)> print_node);

private:
	//初期化
	void initialize();
	//終了
	void finalize();

public:
	//コンストラクタ
	lfStack();
	//デストラクタ
	~lfStack();
private:
	//フィールド
	allocator_type m_allocator;//アロケータ
	std::atomic<stack_ptr_t> m_head;//スタックの先頭　※タグ付きポインタ
	std::atomic<typename stack_ptr_t::tag_type> m_tag;//ABA問題対策用のタグ
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/lf_stack.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_LF_STACK_ALLWAYS_TOGETHER_CPP_H
#include <gasha/lf_stack.cpp.h>
#endif//GASHA_LF_STACK_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_LOCKFREE_STACK_H

// End of file

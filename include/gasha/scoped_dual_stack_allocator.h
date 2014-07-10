#pragma once
#ifndef GASHA_INCLUDED_SCOPED_DUAL_STACK_ALLOCATOR_H
#define GASHA_INCLUDED_SCOPED_DUAL_STACK_ALLOCATOR_H

//--------------------------------------------------------------------------------
// scoped_dual_stack_allocator.h
// スコープ双方向スタックアロケータ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/allocator_common.h>//メモリアロケータ共通設定

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//スコープ双方向スタックアロケータクラス
//※既存の双方向スタックアロケータをコンストラクタで受け渡して使用する。
//※スコープの範囲内でのみ有効な双方向スタックアロケータ。スコープを抜けると、使用していたメモリを全て解放する。
//※メモリのクリア／リワインドもスコープの範囲内に制限される。
//※【注】マルチスレッド非対応。他のスレッドとアロケータを共有している時には使えない。
template<class ALLOCATOR>
class scopedDualStackAllocator
{
public:
	//型
	typedef ALLOCATOR allocator_type;//アロケータ型
	typedef std::uint32_t size_type;//サイズ型

public:
	//アクセッサ
	inline const char* name() const;//名前
	inline const char* mode() const;//実装モード
	inline size_type maxSize() const;//バッファの全体サイズ（バイト数）
	inline size_type size() const;//使用中のサイズ（バイト数）
	inline size_type sizeAsc() const;//正順で使用中のサイズ（バイト数）
	inline size_type sizeDesc() const;//逆順で使用中のサイズ（バイト数）
	inline size_type remain() const;//残りサイズ（バイト数）
	inline size_type count() const;//アロケート中の数
	inline size_type countAsc() const;//正順でアロケート中の数
	inline size_type countDesc() const;//逆順でアロケート中の数
	inline allocateOrder_t allocateOrder() const;//現在のアロケート方向
	inline void setAllocateOrder(const allocateOrder_t order);//現在のアロケート方向を変更
	inline void reversewAllocateOrder();//現在のアロケート方向を逆にする

public:
	//メソッド
	//メモリ確保
	inline void* alloc(const std::size_t size, const std::size_t align = GASHA_ DEFAULT_ALIGN);
	//※アロケート方向指定版
	inline  void* allocOrdinal(const allocateOrder_t order, const std::size_t size, const std::size_t align = GASHA_ DEFAULT_ALIGN);

	//メモリ解放
	inline bool free(void* p);

	//メモリ確保とコンストラクタ呼び出し
	template<typename T, typename...Tx>
	inline T* newObj(Tx&&... args);
	//※アロケート方向指定版
	template<typename T, typename...Tx>
	T* newObjOrdinal(const allocateOrder_t order, Tx&&... args);
	//※配列用
	template<typename T, typename...Tx>
	inline T* newArray(const std::size_t num, Tx&&... args);
	//※配列用アロケート方向指定版
	template<typename T, typename...Tx>
	T* newArrayOrdinal(const allocateOrder_t order, const std::size_t num, Tx&&... args);

	//メモリ解放とデストラクタ呼び出し
	template<typename T>
	inline bool deleteObj(T* p);
	//※配列用（要素数の指定が必要な点に注意）
	template<typename T>
	inline bool deleteArray(T* p, const std::size_t num);

	//使用中のサイズを指定位置に戻す
	inline bool rewind(const size_type pos);
	//※位置指定とアロケート方向指定版
	inline bool rewindOrdinal(const allocateOrder_t order, const size_type pos);
	//※ポインタ指定版（スコープの範囲内に制限しないので注意）
	inline bool rewind(void* p);

	//メモリクリア
	//※全て初期状態にする（アロケート方向も初期状態に戻るので注意）
	inline void clearAll();
	//※現在のアロケート方向のみ
	inline void clear();
	//※アロケート方向指定
	inline void clearOrdinal(const allocateOrder_t order);
	
	//デバッグ情報作成
	std::size_t debugInfo(char* message);

public:
	//コンストラクタ
	inline scopedDualStackAllocator(allocator_type& allocator);
	//デストラクタ
	inline ~scopedDualStackAllocator();
private:
	//フィールド
	allocator_type& m_allocator;//アロケータ
	allocateOrder_t m_initAllocateOrder;//初期アロケート方向
	size_type m_initSizeAsc;//初期正順使用サイズ
	size_type m_initSizeDesc;//初期逆順使用サイズ
	size_type m_initCountAsc;//初期正順アロケート数
	size_type m_initCountDesc;//初期逆順アロケート数
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/scoped_dual_stack_allocator.inl>

#endif//GASHA_INCLUDED_SCOPED_DUAL_STACK_ALLOCATOR_H

// End of file

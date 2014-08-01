#pragma once
#ifndef GASHA_INCLUDED_NAMED_REF_H
#define GASHA_INCLUDED_NAMED_REF_H

//--------------------------------------------------------------------------------
// named_ref.h
// 名前付きデータ参照【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/crc32.h>//CRC32操作
#include <gasha/hash_table.h>//ハッシュテーブル
#include <gasha/dummy_shared_lock.h>//ダミー共有ロック
#include <gasha/unique_shared_lock.h>//単一共有ロック

#include <cstddef>//std::size_t
#include <typeinfo>//std::type_info

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <mutex>//C++11 std::call_once
#include <functional>//C++11 std::function
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//名前付きデータ参照
//--------------------------------------------------------------------------------

//----------------------------------------
//名前付きデータ参照クラス
//※シングルトンとして振る舞う為、複数のインスタンスを扱うために、IDENTIFIER_TYPE（空の構造体で良い）を与えて区別する。
template<class IDENTIFIER_TYPE, std::size_t _TABLE_SIZE, class LOCK_POLICY = GASHA_ dummySharedLock>
class namedRef
{
public:
	//定数
	static const std::size_t TABLE_SIZE = _TABLE_SIZE;
public:
	//型
	typedef IDENTIFIER_TYPE identifier_type;//インスタンス識別型
	typedef LOCK_POLICY lock_type;//ロック型
	struct explicitInit_type{};//明示的な初期化用構造体
	//--------------------
	//参照情報
	struct refInfo
	{
		GASHA_ crc32_t m_nameCrc;//名前のCRC
		const std::type_info* m_typeInfo;//型情報
		const void* m_loadRef;//読み込み用参照先
		void* m_storeRef;//書き込み用参照先
		mutable lock_type m_lock;//ロック型
		
		//コンストラクタ
		template<typename T>
		inline refInfo(const GASHA_ crc32_t name_crc, const T& ref);
		template<typename T>
		inline refInfo(const GASHA_ crc32_t name_crc, T& ref);
		//デフォルトコンストラクタ
		inline refInfo();
		//デストラクタ
		inline ~refInfo();
	};
	//--------------------
	//参照情報プロキシー
	//※スコープロックを伴う
	template<typename T>
	class refProxy
	{
	public:
		//型
		typedef typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lock_type lock_type;
	public:
		//オペレータ
		inline const T* operator->() const;
		inline T* operator->();
		inline const T& operator*() const;
		inline T& operator*();
		//ムーブオペレータ
		refProxy& operator=(refProxy&& obj) = delete;
		//コピーオペレータ
		refProxy& operator=(const refProxy& obj) = delete;
		//ムーブコンストラクタ
		inline refProxy(typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T>&& obj);
		//コピーコンストラクタ
		refProxy(const refProxy& obj) = delete;
		//コンストラクタ
		inline refProxy(const refInfo& ref_info, GASHA_ unique_shared_lock<lock_type>&& lock);
		//デフォルトコンストラクタ
		//inline refProxy();
		//デストラクタ
		inline ~refProxy();
	private:
		//フィールド
		const refInfo& m_refInfo;//参照情報
		mutable GASHA_ unique_shared_lock<lock_type> m_lock;//単一共有ロック
	};
	//--------------------
	//読み取り専用参照情報プロキシー
	//※スコープロックを伴う
	template<typename T>
	class constRefProxy
	{
	public:
		//型
		typedef typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lock_type lock_type;
	public:
		//オペレータ
		inline const T* operator->() const;
		inline const T& operator*() const;
		//ムーブオペレータ
		constRefProxy& operator=(constRefProxy&& obj) = delete;
		//コピーオペレータ
		constRefProxy& operator=(const constRefProxy& obj) = delete;
		//ムーブコンストラクタ
		inline constRefProxy(const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template constRefProxy<T>&& obj);
		//コピーコンストラクタ
		constRefProxy(const constRefProxy& obj) = delete;
		//コンストラクタ
		inline constRefProxy(const refInfo& ref_info, GASHA_ unique_shared_lock<lock_type>&& lock);
		//デフォルトコンストラクタ
		//inline constRefProxy();
		//デストラクタ
		inline ~constRefProxy();
	private:
		//フィールド
		const refInfo& m_refInfo;//参照情報
		mutable GASHA_ unique_shared_lock<lock_type> m_lock;//単一共有ロック
	};
	//ハッシュテーブル操作型
	struct tableOpe : public GASHA_ hash_table::baseOpe<tableOpe, refInfo, GASHA_ crc32_t>
	{
		typedef typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::lock_type lock_type;//ロック型
	};
	//ハッシュテーブル型
	typedef GASHA_ hash_table::container<tableOpe, TABLE_SIZE> table_type;
	//イテレータ型
	typedef typename table_type::iterator iterator;
	typedef typename table_type::const_iterator const_iterator;
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	typedef typename table_type::reverse_iterator reverse_iterator;
	typedef typename table_type::const_reverse_iterator const_reverse_iterator;
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
public:
	//定数
	static const std::size_t TABLE_BUFF_SIZE = sizeof(table_type) + alignof(table_type);//ハッシュテーブルバッファサイズ
public:
	//アクセッサ
	inline bool isRegistered(const GASHA_ crc32_t name_crc) const;//登録済みチェック
	inline bool isRegistered(const char* name) const;//登録済みチェック
	template<typename T>
	inline bool isReadOnly(const GASHA_ crc32_t name_crc) const;//読み取り専用チェック ※未登録時は false
	template<typename T>
	inline bool isReadOnly(const char* name) const;//読み取り専用チェック ※未登録時は false
	template<typename T>
	inline bool isReadable(const GASHA_ crc32_t name_crc) const;//読み取り可能チェック ※未登録時は false
	template<typename T>
	inline bool isReadable(const char* name) const;//読み取り可能チェック ※未登録時は false
	template<typename T>
	inline bool isWritable(const GASHA_ crc32_t name_crc) const;//書き込み可能チェック ※未登録時は false
	template<typename T>
	inline bool isWritable(const char* name) const;//書き込み可能チェック ※未登録時は false
public:
	//データ参照
	//※読み取り専用参照
	//※戻り値のスコープを抜けるまで、共有ロックを取得する
	template<typename T>
	inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template constRefProxy<T> ref(const char* name) const;
	template<typename T>
	inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template constRefProxy<T> ref(const GASHA_ crc32_t name_crc) const;
	//※読み書き参照
	//※戻り値のスコープを抜けるまで、排他ロックを取得する
	template<typename T>
	inline typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T> ref(const char* name);
	template<typename T>
	inline typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template refProxy<T> ref(const GASHA_ crc32_t name_crc);
	template<typename T>
	//※読み取り専用参照
	//※戻り値のスコープを抜けるまで、共有ロックを取得する
	inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template constRefProxy<T> cref(const char* name) const;
	template<typename T>
	inline const typename namedRef<IDENTIFIER_TYPE, _TABLE_SIZE, LOCK_POLICY>::template constRefProxy<T> cref(const GASHA_ crc32_t name_crc) const;
public:
	//データ取得
	//※一時的に共有ロックを取得する（関数を抜けた時点でロックは解除されている）
	template<typename T>
	inline T load(const char* name) const;
	template<typename T>
	inline T load(const GASHA_ crc32_t name_crc) const;
public:
	//データ更新
	//※一時的に排他ロックを取得する（関数を抜けた時点でロックは解除されている）
	template<typename T>
	inline void store(const char* name, const T value);
	template<typename T>
	inline void store(const GASHA_ crc32_t name_crc, const T value);
public:
	//データ交換
	//※一時的に排他ロックを取得する（関数を抜けた時点でロックは解除されている）
	//※戻り値は演算前の値
	template<typename T>
	inline T exchange(const GASHA_ crc32_t name_crc, const T value);
	template<typename T>
	inline T exchange(const char* name, const T value);
private:
	//演算用共通処理
	template<typename T, class OPERATOR_POLICY>
	inline T calcurate(const GASHA_ crc32_t name_crc, OPERATOR_POLICY ope, const T rhs, const T max, const T min);
public:
	//演算
	//※一時的に排他ロックを取得する（関数を抜けた時点でロックは解除されている）
	//※戻り値は演算後の値
	//※浮動小数点はラップアラウンド演算に対応せず、指定の範囲を超えた結果となる
	//※浮動小数点はビット演算は常に何もせず現在値を返すのみ
	template<typename T>
	inline T inc(const GASHA_ crc32_t name_crc);//インクリメント
	template<typename T>
	inline T inc(const char* name);
	template<typename T>
	inline T incWA(const GASHA_ crc32_t name_crc, const T max, const T min = static_cast<T>(0));//インクリメント ※ラップアラウンド演算（wrap-around）：演算結果が範囲を超えたらループする
	template<typename T>
	inline T incWA(const char* name, const T max, const T min = static_cast<T>(0));
	template<typename T>
	inline T incSA(const GASHA_ crc32_t name_crc, const T max, const T min = static_cast<T>(0));//インクリメント ※飽和演算（saturation）：演算結果が範囲を超えたら限界値を返す
	template<typename T>
	inline T incSA(const char* name, const T max, const T min = static_cast<T>(0));
	template<typename T>
	inline T dec(const GASHA_ crc32_t name_crc);//デクリメント
	template<typename T>
	inline T dec(const char* name);
	template<typename T>
	inline T decWA(const GASHA_ crc32_t name_crc, const T max, const T min = static_cast<T>(0));//デクリメント ※ラップアラウンド演算（wrap-around）：演算結果が範囲を超えたらループする
	template<typename T>
	inline T decWA(const char* name, const T max, const T min = static_cast<T>(0));
	template<typename T>
	inline T decSA(const GASHA_ crc32_t name_crc, const T max, const T min = static_cast<T>(0));//デクリメント ※飽和演算（saturation）：演算結果が範囲を超えたら限界値を返す
	template<typename T>
	inline T decSA(const char* name, const T max, const T min = static_cast<T>(0));
	template<typename T>
	inline T add(const GASHA_ crc32_t name_crc, const T value);//加算
	template<typename T>
	inline T add(const char* name, const T rhs);
	template<typename T>
	inline T addWA(const GASHA_ crc32_t name_crc, const T rhs, const T max, const T min = static_cast<T>(0));//加算 ※ラップアラウンド演算（wrap-around）：演算結果が範囲を超えたらループする
	template<typename T>
	inline T addWA(const char* name, const T rhs, const T max, const T min = static_cast<T>(0));
	template<typename T>
	inline T addSA(const GASHA_ crc32_t name_crc, const T rhs, const T max, const T min = static_cast<T>(0));//加算 ※飽和演算（saturation）：演算結果が範囲を超えたら限界値を返す
	template<typename T>
	inline T addSA(const char* name, const T rhs, const T max, const T min = static_cast<T>(0));
	template<typename T>
	inline T sub(const GASHA_ crc32_t name_crc, const T rhs);//減算
	template<typename T>
	inline T sub(const char* name, const T rhs);
	template<typename T>
	inline T subWA(const GASHA_ crc32_t name_crc, const T rhs, const T max, const T min = static_cast<T>(0));//減算 ※ラップアラウンド演算（wrap-around）：演算結果が範囲を超えたらループする
	template<typename T>
	inline T subWA(const char* name, const T rhs, const T max, const T min = static_cast<T>(0));
	template<typename T>
	inline T subSA(const GASHA_ crc32_t name_crc, const T rhs, const T max, const T min = static_cast<T>(0));//減算 ※飽和演算（saturation）：演算結果が範囲を超えたら限界値を返す
	template<typename T>
	inline T subSA(const char* name, const T rhs, const T max, const T min = static_cast<T>(0));
	template<typename T>
	inline T mul(const GASHA_ crc32_t name_crc, const T rhs);//乗算
	template<typename T>
	inline T mul(const char* name, const T rhs);
	template<typename T>
	inline T mulWA(const GASHA_ crc32_t name_crc, const T rhs, const T max, const T min = static_cast<T>(0));//乗算 ※ラップアラウンド演算（wrap-around）：演算結果が範囲を超えたらループする
	template<typename T>
	inline T mulWA(const char* name, const T rhs, const T max, const T min = static_cast<T>(0));
	template<typename T>
	inline T mulSA(const GASHA_ crc32_t name_crc, const T rhs, const T max, const T min = static_cast<T>(0));//乗算 ※飽和演算（saturation）：演算結果が範囲を超えたら限界値を返す
	template<typename T>
	inline T mulSA(const char* name, const T rhs, const T max, const T min = static_cast<T>(0));
	template<typename T>
	inline T div(const GASHA_ crc32_t name_crc, const T rhs);//除算
	template<typename T>
	inline T div(const char* name, const T rhs);
	template<typename T>
	inline T mod(const GASHA_ crc32_t name_crc, const T rhs);//剰余算
	template<typename T>
	inline T mod(const char* name, const T rhs);
	template<typename T>
	inline T bitAnd(const GASHA_ crc32_t name_crc, const T rhs);//ビット論理積
	template<typename T>
	inline T bitAnd(const char* name, const T rhs);
	template<typename T>
	inline T bitOr(const GASHA_ crc32_t name_crc, const T rhs);//ビット論理和
	template<typename T>
	inline T bitOr(const char* name, const T rhs);
	template<typename T>
	inline T bitXor(const GASHA_ crc32_t name_crc, const T rhs);//ビット排他的論理和
	template<typename T>
	inline T bitXor(const char* name, const T rhs);
	template<typename T>
	inline T bitNot(const GASHA_ crc32_t name_crc);//ビット反転
	template<typename T>
	inline T bitNot(const char* name);
	template<typename T>
	inline T lShift(const GASHA_ crc32_t name_crc, const int rhs);//左シフト
	template<typename T>
	inline T lShift(const char* name, const int rhs);
	template<typename T>
	inline T rShift(const GASHA_ crc32_t name_crc, const int rhs);//右シフト
	template<typename T>
	inline T rShift(const char* name, const int rhs);
	template<typename T>
	inline T bitOn(const GASHA_ crc32_t name_crc, const T rhs);//ビットオン（論理和）
	template<typename T>
	inline T bitOn(const char* name, const T rhs);
	template<typename T>
	inline T bitOff(const GASHA_ crc32_t name_crc, const T rhs);//ビットオフ（ビット反転して論理積）
	template<typename T>
	inline T bitOff(const char* name, const T rhs);
private:
	//比較用共通処理
	template<typename T, class OPERATOR_POLICY>
	inline bool compare(const GASHA_ crc32_t name_crc, OPERATOR_POLICY ope, const T rhs);
public:
	//比較
	//※一時的に共有ロックを取得する（関数を抜けた時点でロックは解除されている）
	//※戻り値は判定結果
	//※浮動小数点のビット判定は常に false を返す
	template<typename T>
	inline bool eq(const GASHA_ crc32_t name_crc, const T rhs);//比較 ==
	template<typename T>
	inline bool eq(const char* name, const T rhs);
	template<typename T>
	inline bool ne(const GASHA_ crc32_t name_crc, const T rhs);//比較 !=
	template<typename T>
	inline bool ne(const char* name, const T rhs);
	template<typename T>
	inline bool gt(const GASHA_ crc32_t name_crc, const T rhs);//比較 >
	template<typename T>
	inline bool gt(const char* name, const T rhs);
	template<typename T>
	inline bool ge(const GASHA_ crc32_t name_crc, const T rhs);//比較 >=
	template<typename T>
	inline bool ge(const char* name, const T rhs);
	template<typename T>
	inline bool lt(const GASHA_ crc32_t name_crc, const T rhs);//比較 <
	template<typename T>
	inline bool lt(const char* name, const T rhs);
	template<typename T>
	inline bool le(const GASHA_ crc32_t name_crc, const T rhs);//比較 <=
	template<typename T>
	inline bool le(const char* name, const T rhs);
	template<typename T>
	inline bool isOn(const GASHA_ crc32_t name_crc, const T rhs);//ビットが立っているか？
	template<typename T>
	inline bool isOn(const char* name, const T rhs);
	template<typename T>
	inline bool isOff(const GASHA_ crc32_t name_crc, const T rhs);//ビットが立っているか？
	template<typename T>
	inline bool isOff(const char* name, const T rhs);
	template<typename T>
	inline bool logicalAnd(const GASHA_ crc32_t name_crc, const T rhs);//比較 &&
	template<typename T>
	inline bool logicalAnd(const char* name, const T rhs);
	template<typename T>
	inline bool logicalOr(const GASHA_ crc32_t name_crc, const T rhs);//比較 ||
	template<typename T>
	inline bool logicalOr(const char* name, const T rhs);
	template<typename T>
	inline bool isTrue(const GASHA_ crc32_t name_crc);//真か？（0以外か？）
	template<typename T>
	inline bool isTrue(const char* name);
	template<typename T>
	inline bool isFalse(const GASHA_ crc32_t name_crc);//偽か？（0か？）
	template<typename T>
	inline bool isFalse(const char* name);
public:
	//データ参照登録
	//※読み書き可能データ登録
	//※登録できた場合は true を返し、既に登録済みなどの理由で登録できなかった場合は false を返す。
	template<typename T>
	inline bool regist(const GASHA_ crc32_t name_crc, T& ref);
	template<typename T>
	inline bool regist(const char* name, T& ref);
	//※読み取り専用データ登録
	template<typename T>
	inline bool regist(const GASHA_ crc32_t name_crc, const T& ref);
	template<typename T>
	inline bool regist(const char* name, const T& ref);
	//データ登録解除
	//※登録解除できた場合は true を返し、未登録などの理由で登録解除できなかった場合は false を返す。
	inline bool unregist(const GASHA_ crc32_t name_crc);
	inline bool unregist(const char* name);
public:
	//クリア
	inline void clear();
	//初期状態にする
	//※明示的に実行する必要あり
	//※先着一つだけが初期化を実行し、以後の呼び出しでは何もしない
	inline void initialize();
	//明示的な再初期化
	//※初期化済みでも再初期化する
	//※スレッドセーフじゃないので注意
	inline void explicitInitialize();
private:
	//初期化メソッド（一回限り）
	static void initializeOnce();
public:
	//ハッシュテーブルアクセス：アクセッサ
	inline std::size_t max_size() const;
	inline std::size_t size() const;
	inline std::size_t remain() const;
	inline bool empty() const;
	inline bool full() const;
public:
	//ハッシュテーブルアクセス：ロック取得系メソッド
	//単一ロック取得
	inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ with_lock_shared_t&) const;
	inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ try_to_lock_shared_t&) const;
	//スコープロック取得
	inline GASHA_ shared_lock_guard<lock_type> lockSharedScoped() const;
public:
	//ハッシュテーブルアクセス：イテレータ取得系メソッド
	//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
	//　一連の処理ブロックの前後で共有ロック（リードロック）または
	//　排他ロック（ライトロック）の取得と解放を行う必要がある
	//イテレータを取得
	inline const iterator cbegin() const;
	inline const iterator cend() const;
	inline const iterator begin() const;
	inline const iterator end() const;
#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	//リバースイテレータを取得
	inline const reverse_iterator crbegin() const;
	inline const reverse_iterator crend() const;
	inline const reverse_iterator rbegin() const;
	inline const reverse_iterator rend() const;
#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
public:
	//明示的な初期化用コンストラクタ
	inline namedRef(const explicitInit_type&);
	//デフォルトコンストラクタ
	inline namedRef();
	//デストラクタ
	inline ~namedRef();
private:
	//フィールド
	table_type* m_refTable;//ハッシュテーブル（参照）
private:
	//静的フィールド
	static std::once_flag m_initialized;//ハッシュテーブル初期化済み
	static char m_refTableBuff[];//ハッシュテーブル（バッファ）
public:
	//静的フィールド
	static const explicitInit_type explicitInit;//明示的な初期化指定用
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/named_ref.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_NAMED_REF_ALLWAYS_TOGETHER_CPP_H
#include <gasha/named_ref.cpp.h>
#endif//GASHA_NAMED_REF_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_NAMED_REF_H

// End of file

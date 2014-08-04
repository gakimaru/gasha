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
#include <gasha/limits.h>//限界値

#include <cstddef>//std::size_t
#include <typeinfo>//std::type_info

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <mutex>//C++11 std::call_once
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//名前付きデータ参照
//--------------------------------------------------------------------------------

namespace named_ref
{
	//--------------------
	//名前付きデータ参照操作用テンプレート構造体
	//※CRTPを活用し、下記のような派生構造体を作成して使用する
	//  //struct 派生構造体名 : public named_ref::baseOpe<派生構造体名, テーブルサイズ>
	//	struct ope : public named_ref::baseOpe<ope, TABLE_SIZE>
	//	{
	//		//ロックポリシー ※必要に応じて定義
	//		//※共有ロック（リード・ライトロック）でテーブル操作をスレッドセーフにしたい場合は、
	//		//　有効な共有ロック型（sharedSpinLockなど）を lock_type 型として定義する。
	//		typedef sharedSpinLock lock_type;//ロックオブジェクト型
	//	};
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	struct baseOpe
	{
		//型
		typedef OPE_TYPE ope_type;//データ操作型

		//定数
		static const std::size_t TABLE_SIZE = _TABLE_SIZE;//テーブルサイズ（元の指定サイズ）※扱えるデータの要素数
		static const std::size_t AUTO_REHASH_RATIO = 25;//自動リハッシュ実行の基準割合(0～100) ※削除済み件数が全体サイズの一定割合以上になったら自動リハッシュ ※0で自動リハッシュなし

		//ロックポリシー
		typedef GASHA_ dummySharedLock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※共有ロック（リード・ライトロック）でテーブル操作をスレッドセーフにしたい場合は、
		//　baseOpeの派生クラスにて、有効な共有ロック型（sharedSpinLock など）を
		//　lock_type 型として再定義する。
	};

	//----------------------------------------
	//名前付きデータ参照クラス
	template<class OPE_TYPE>
	class table
	{
	public:
		//型
		typedef OPE_TYPE ope_type;
		typedef typename ope_type::lock_type lock_type;
	public:
		//定数
		static const std::size_t TABLE_SIZE = ope_type::TABLE_SIZE;//テーブルサイズ
		static const std::size_t AUTO_REHASH_RATIO = ope_type::AUTO_REHASH_RATIO;//自動リハッシュ実行の基準割合
	public:
		//型
		struct explicitInit_tag{};//明示的な初期化用構造体
		typedef GASHA_ wraparound_tag wraparound_tag;//ラップアラウンド演算指定用構造体
		typedef GASHA_ saturation_tag saturation_tag;//飽和演算指定用構造体
		struct readonly_tag{};//読み取り専用指定用構造体
		//--------------------
		//参照情報
		enum refAccess_type//参照情報へのアクセス種別
		{
			READ_ONLY = 0,//読み取り専用
			WRITABLE,//書き込み可能
			WRITABLE_WRAPAROUND,//書き込み可能：ラップアラウンド演算
			WRITABLE_SATURATION//書き込み可能：飽和演算
		};
		struct refInfo
		{
			GASHA_ crc32_t m_nameCrc;//名前のCRC
			const std::type_info* m_typeInfo;//型情報
			refAccess_type m_accessType;//アクセス種別
			const void* m_ref;//参照先
			const void* m_refMax;//最大値参照先
			const void* m_refMin;//最小値参照先
			mutable lock_type m_lock;//ロック型

			//アクセッサ
			template<typename T>
			inline const T& max() const;
			template<typename T>
			inline const T& min() const;

		private:
			//メソッド
			template<typename T>
			inline void setMinMax(const T& max, const T& min);

		public:
			//コンストラクタ
			template<typename T>
			inline refInfo(const GASHA_ crc32_t name_crc, T& value);
			template<typename T>
			inline refInfo(const GASHA_ crc32_t name_crc, T& value, const wraparound_tag&, const T& max, const T& min);
			template<typename T>
			inline refInfo(const GASHA_ crc32_t name_crc, T& value, const saturation_tag&, const T& max, const T& min);
			template<typename T>
			inline refInfo(const GASHA_ crc32_t name_crc, const T& value);
			template<typename T>
			inline refInfo(const GASHA_ crc32_t name_crc, const T& value, const readonly_tag&);
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
			typedef typename table<OPE_TYPE>::lock_type lock_type;
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
			inline refProxy(typename table<OPE_TYPE>::template refProxy<T>&& obj);
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
			typedef typename table<OPE_TYPE>::lock_type lock_type;
		public:
			//オペレータ
			inline const T* operator->() const;
			inline const T& operator*() const;
			//ムーブオペレータ
			constRefProxy& operator=(constRefProxy&& obj) = delete;
			//コピーオペレータ
			constRefProxy& operator=(const constRefProxy& obj) = delete;
			//ムーブコンストラクタ
			inline constRefProxy(const typename table<OPE_TYPE>::template constRefProxy<T>&& obj);
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
		struct tableOpe : public GASHA_ hash_table::baseOpe<tableOpe, TABLE_SIZE, refInfo, GASHA_ crc32_t>
		{
			typedef typename table<OPE_TYPE>::lock_type lock_type;//ロック型
			static const std::size_t AUTO_REHASH_RATIO = table<OPE_TYPE>::AUTO_REHASH_RATIO;//自動リハッシュ実行の基準割合
		};
		//ハッシュテーブル型
		typedef GASHA_ hash_table::container<tableOpe> table_type;
		//イテレータ型
		typedef typename table_type::iterator iterator;
		typedef typename table_type::const_iterator const_iterator;
	#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
		typedef typename table_type::reverse_iterator reverse_iterator;
		typedef typename table_type::const_reverse_iterator const_reverse_iterator;
	#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	public:
		//定数
		static const std::size_t TABLE_BUFF_SIZE = sizeof(table_type)+alignof(table_type);//ハッシュテーブルバッファサイズ
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
		template<typename T>
		inline bool isWritableRanged(const GASHA_ crc32_t name_crc) const;//ラップアラウンド演算／飽和演算で書き込み可能チェック ※未登録時は false
		template<typename T>
		inline bool isWritableRanged(const char* name) const;//ラップアラウンド演算／飽和演算で書き込み可能チェック ※未登録時は false
		template<typename T>
		inline bool isWritableWraparound(const GASHA_ crc32_t name_crc) const;//ラップアラウンド演算で書き込み可能チェック ※未登録時は false
		template<typename T>
		inline bool isWritableWraparound(const char* name) const;//ラップアラウンド演算で書き込み可能チェック ※未登録時は false
		template<typename T>
		inline bool isWritableSaturation(const GASHA_ crc32_t name_crc) const;//飽和演算で書き込み可能チェック ※未登録時は false
		template<typename T>
		inline bool isWritableSaturation(const char* name) const;//飽和演算で書き込み可能チェック ※未登録時は false
	public:
		//データ参照
		//※読み取り専用参照
		//※戻り値のスコープを抜けるまで、共有ロックを取得する
		template<typename T>
		inline const typename table<OPE_TYPE>::template constRefProxy<T> ref(const GASHA_ crc32_t name_crc) const;
		template<typename T>
		inline const typename table<OPE_TYPE>::template constRefProxy<T> ref(const char* name) const;
		//※読み書き参照
		//※戻り値のスコープを抜けるまで、排他ロックを取得する
		template<typename T>
		inline typename table<OPE_TYPE>::template refProxy<T> ref(const GASHA_ crc32_t name_crc);
		template<typename T>
		inline typename table<OPE_TYPE>::template refProxy<T> ref(const char* name);
		//※読み取り専用参照
		//※戻り値のスコープを抜けるまで、共有ロックを取得する
		template<typename T>
		inline const typename table<OPE_TYPE>::template constRefProxy<T> cref(const GASHA_ crc32_t name_crc) const;
		template<typename T>
		inline const typename table<OPE_TYPE>::template constRefProxy<T> cref(const char* name) const;
	public:
		//ラップアラウンド演算／飽和演算用の最大値取得
		//※ロック操作は行わない
		template<typename T>
		inline const T max(const GASHA_ crc32_t name_crc) const;
		template<typename T>
		inline const T max(const char* name) const;
		//ラップアラウンド演算／飽和演算用の最小値取得
		//※ロック操作は行わない
		template<typename T>
		inline const T min(const GASHA_ crc32_t name_crc) const;
		template<typename T>
		inline const T min(const char* name) const;
	public:
		//データ取得
		//※一時的に共有ロックを取得する（関数を抜けた時点でロックは解除されている）
		template<typename T>
		inline T load(const GASHA_ crc32_t name_crc) const;
		template<typename T>
		inline T load(const char* name) const;
	public:
		//データ更新
		//※一時的に排他ロックを取得する（関数を抜けた時点でロックは解除されている）
		//※ラップアラウンド演算（wraparound）および飽和演算（saturation）に対応せず、範囲外の値になり得る。
		template<typename T>
		inline void store(const GASHA_ crc32_t name_crc, const T value);
		template<typename T>
		inline void store(const char* name, const T value);
	public:
		//データ交換
		//※一時的に排他ロックを取得する（関数を抜けた時点でロックは解除されている）
		//※戻り値は演算前の値
		//※ラップアラウンド演算（wraparound）および飽和演算（saturation）に対応せず、範囲外の値になり得る。
		template<typename T>
		inline T exchange(const GASHA_ crc32_t name_crc, const T value);
		template<typename T>
		inline T exchange(const char* name, const T value);
	private:
		//演算用共通処理
		template<typename T, typename T2, class OPERATOR_POLICY, class OPERATOR_WRAPAROUND_POLICY, class OPERATOR_SATURATION_POLICY>
		inline T calcurate(const GASHA_ crc32_t name_crc, OPERATOR_POLICY ope, OPERATOR_WRAPAROUND_POLICY ope_wraparound, OPERATOR_SATURATION_POLICY ope_saturation, const T2 rhs);
	public:
		//演算
		//※一時的に排他ロックを取得する（関数を抜けた時点でロックは解除されている）
		//※戻り値は演算後の値
		//※inc(), dec(), add(), sub(), mul() のみ、ラップアラウンド演算（wraparound）および飽和演算（saturation）に対応する。他の演算結果は範囲外の値になり得る。
		template<typename T>
		inline T inc(const GASHA_ crc32_t name_crc);//インクリメント
		template<typename T>
		inline T inc(const char* name);
		template<typename T>
		inline T dec(const GASHA_ crc32_t name_crc);//デクリメント
		template<typename T>
		inline T dec(const char* name);
		template<typename T>
		inline T add(const GASHA_ crc32_t name_crc, const T value);//加算
		template<typename T>
		inline T add(const char* name, const T rhs);
		template<typename T>
		inline T sub(const GASHA_ crc32_t name_crc, const T rhs);//減算
		template<typename T>
		inline T sub(const char* name, const T rhs);
		template<typename T>
		inline T mul(const GASHA_ crc32_t name_crc, const T rhs);//乗算
		template<typename T>
		inline T mul(const char* name, const T rhs);
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
		//※ラップアラウンド演算用
		//※max, min は、sizeof(T) <= sizeof(void*) なら内部に保持し、sizeof(T) > sizeof(void*) なら参照を保持する点に注意。
		template<typename T>
		inline bool regist(const GASHA_ crc32_t name_crc, T& ref, const wraparound_tag&, const T&& max, const T&& min);
		template<typename T>
		inline bool regist(const GASHA_ crc32_t name_crc, T& ref, const wraparound_tag&, const T& max, const T& min);
		template<typename T>
		inline bool regist(const char* name, T& ref, const wraparound_tag&, const T&& max, const T&& min);
		template<typename T>
		inline bool regist(const char* name, T& ref, const wraparound_tag&, const T& max, const T& min);
		//※飽和演算用
		//※max, min は、sizeof(T) <= sizeof(void*) なら内部に保持し、sizeof(T) > sizeof(void*) なら参照を保持する点に注意。
		template<typename T>
		inline bool regist(const GASHA_ crc32_t name_crc, T& ref, const saturation_tag&, const T&& max, const T&& min);
		template<typename T>
		inline bool regist(const GASHA_ crc32_t name_crc, T& ref, const saturation_tag&, const T& max, const T& min);
		template<typename T>
		inline bool regist(const char* name, T& ref, const saturation_tag&, const T&& max, const T&& min);
		template<typename T>
		inline bool regist(const char* name, T& ref, const saturation_tag&, const T& max, const T& min);
		//※読み取り専用
		template<typename T>
		inline bool regist(const GASHA_ crc32_t name_crc, const T& ref);
		template<typename T>
		inline bool regist(const char* name, const T& ref);
		//※読み取り専用
		template<typename T>
		inline bool regist(const GASHA_ crc32_t name_crc, const T& ref, const readonly_tag&);
		template<typename T>
		inline bool regist(const char* name, const T& ref, const readonly_tag&);
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
		inline table(const explicitInit_tag&);
		//デフォルトコンストラクタ
		inline table();
		//デストラクタ
		inline ~table();
	private:
		//フィールド
		table_type* m_refTable;//ハッシュテーブル（参照）
	private:
		//静的フィールド
		static std::once_flag m_initialized;//テーブル初期化済み
		static char m_refTableBuff[];//ハッシュテーブル（バッファ）
	public:
		//静的フィールド
		static const explicitInit_tag explicitInit;//明示的な初期化指定用
		static const wraparound_tag wraparound;//ラップアラウンド演算指定用
		static const saturation_tag saturation;//飽和演算指定用
		static const readonly_tag readonly;//読み取り専用指定用
	};
}//namespace named_ref

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/named_ref.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_NAMED_REF_ALLWAYS_TOGETHER_CPP_H
#include <gasha/named_ref.cpp.h>
#endif//GASHA_NAMED_REF_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_NAMED_REF_H

// End of file

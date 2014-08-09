#pragma once
#ifndef GASHA_INCLUDED_NAMED_VALUE_H
#define GASHA_INCLUDED_NAMED_VALUE_H

//--------------------------------------------------------------------------------
// named_value.h
// 名前付き定数【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/crc32.h>//CRC32操作
#include <gasha/hash_table.h>//ハッシュテーブル
#include <gasha/str_pool.h>//文字列プール
#include <gasha/linked_list.h>//双方向連結リスト
#include <gasha/dummy_shared_lock.h>//ダミー共有ロック
#include <gasha/unique_shared_lock.h>//単一共有ロック

#include <cstddef>//std::size_t
#include <typeinfo>//std::type_info

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <mutex>//C++11 std::call_once
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//名前付き定数
//※定数に対して、名前を付けて管理。
//※名前付き定数（テーブル）自体はシングルトンとして振る舞う。
//※名前と定数の型さえ分かっていれば、extern や include なしに、どこからでも呼び出し可能。
//※インスタンスごとの共有ロック／排他ロックが可能。
//※操作用構造体を複数用意することにより、複数のインスタンスを作成可能。ロック制御の有無はインスタンスごとに設定。
//※グループ設定により、特定グループの情報の列挙が可能。
//※デバッグ機能の作成に便利。文字列で指定されたオプションを数値化する場合など。
//--------------------------------------------------------------------------------

namespace named_value
{
	//--------------------
	//名前付き定数操作用テンプレート構造体
	//※CRTPを活用し、下記のような派生構造体を作成して使用する
	//  //struct 派生構造体名 : public named_value::baseOpe<派生構造体名, 定数グループテーブルサイズ, 定数テーブルサイズ>
	//	struct ope : public named_value::baseOpe<ope, GROUP_TABLE_SIZE, FUNC_TABLE_SIZE>
	//	{
	//		//ロックポリシー ※必要に応じて定義
	//		//※共有ロック（リード・ライトロック）でテーブル操作をスレッドセーフにしたい場合は、
	//		//　有効な共有ロック型（sharedSpinLockなど）を lock_type 型として定義する。
	//		typedef sharedSpinLock lock_type;//ロックオブジェクト型
	//	};
	template<class OPE_TYPE, std::size_t _GROUP_TABLE_SIZE, std::size_t _FUNC_TABLE_SIZE>
	struct baseOpe
	{
		//型
		typedef OPE_TYPE ope_type;//データ操作型

		//定数
		static const std::size_t GROUP_TABLE_SIZE = _GROUP_TABLE_SIZE;//定数グループテーブルサイズ（元の指定サイズ）※扱えるデータの要素数
		static const std::size_t FUNC_TABLE_SIZE = _FUNC_TABLE_SIZE;//定数テーブルサイズ（元の指定サイズ）※扱えるデータの要素数

		//ロックポリシー
		typedef GASHA_ dummySharedLock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※共有ロック（リード・ライトロック）でテーブル操作をスレッドセーフにしたい場合は、
		//　baseOpeの派生クラスにて、有効な共有ロック型（sharedSpinLock など）を
		//　lock_type 型として再定義する。
	};

	//----------------------------------------
	//名前付き定数クラス
	template<class OPE_TYPE>
	class table
	{
	public:
		//型
		typedef OPE_TYPE ope_type;
		typedef typename ope_type::lock_type lock_type;
		typedef std::uint64_t infoKey_type;//定数情報キー型
	public:
		//定数
		static const std::size_t GROUP_TABLE_SIZE = ope_type::GROUP_TABLE_SIZE;//定数グループテーブルサイズ
		static const std::size_t FUNC_TABLE_SIZE = ope_type::FUNC_TABLE_SIZE;//定数テーブルサイズ
	public:
		//型
		struct explicitInit_tag{};//明示的な初期化用構造体
		typedef GASHA_ wraparound_tag wraparound_tag;//ラップアラウンド演算指定用構造体
		typedef GASHA_ saturation_tag saturation_tag;//飽和演算指定用構造体
		//--------------------
		//定数情報
		struct groupInfo;
		struct valueInfo
		{
			//フィールド
			GASHA_ crc32_t m_nameCrc;//名前のCRC
			GASHA_ crc32_t m_groupNameCrc;//グループ名のCRC
			const std::type_info* m_typeInfo;//型情報
			void* m_value;//定数
			const mutable valueInfo* m_next;//連結ノード（後）
			const mutable valueInfo* m_prev;//連結ノード（前）

			//比較オペレータ
			inline bool operator<(const valueInfo& rhs) const;
			inline bool operator==(const valueInfo& rhs) const;
			inline bool operator!=(const valueInfo& rhs) const;
			inline bool operator==(const GASHA_ crc32_t rhs) const;

			//コンストラクタ
			template<typename T>
			inline valueInfo(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, const T& value);
			//デフォルトコンストラクタ
			inline valueInfo();
			//デストラクタ
			inline ~valueInfo();
		};
		//ハッシュテーブル操作型
		//※テーブル操作時は、定数グループテーブルのロックを使用
		struct valueTableOpe : public GASHA_ hash_table::baseOpe<valueTableOpe, FUNC_TABLE_SIZE, valueInfo, GASHA_ crc32_t>
		{
			static const std::size_t AUTO_REHASH_RATIO = 0;//自動リハッシュなし ※リハッシュすると、連結リストに問題が生じるので禁止
		};
		//ハッシュテーブル型
		typedef GASHA_ hash_table::container<valueTableOpe> valueTable_type;
		//双方向連結リスト操作型
		//※リスト操作時は、定数グループテーブルのロックを使用
		struct valueListOpe : public GASHA_ linked_list::baseOpe<valueListOpe, valueInfo>
		{
			typedef typename GASHA_ linked_list::baseOpe<valueListOpe, valueInfo>::node_type node_type;//型があいまいになってコンパイルエラーになることがあるので型を再定義
			
			//前ノードを取得
			inline static const node_type* getPrev(const node_type& node){ return node.m_prev; }
			//前ノードを変更
			inline static void setPrev(node_type& node, const node_type* prev){ node.m_prev = prev; }
			
			//次ノードを取得
			inline static const node_type* getNext(const node_type& node){ return node.m_next; }
			//次ノードを変更
			inline static void setNext(node_type& node, const node_type* next){ node.m_next = next; }
		};
		//双方向連結リスト型
		typedef GASHA_ linked_list::container<valueListOpe> valueList_type;
		//--------------------
		//定数グループ情報
		struct groupInfo
		{
			//フィールド
			GASHA_ crc32_t m_groupNameCrc;//グループ名のCRC
			mutable lock_type m_lock;//ロック型
			valueList_type m_valueList;//定数リスト

			//定数リストイテレータを取得
			inline const typename valueList_type::iterator cbegin() const;
			inline const typename valueList_type::iterator cend() const;
			inline const typename valueList_type::iterator begin() const;
			inline const typename valueList_type::iterator end() const;
			inline const typename valueList_type::reverse_iterator crbegin() const;
			inline const typename valueList_type::reverse_iterator crend() const;
			inline const typename valueList_type::reverse_iterator rbegin() const;
			inline const typename valueList_type::reverse_iterator rend() const;

			//定数リスト情報を取得
			inline std::size_t size() const;

			//コンストラクタ
			inline groupInfo(const GASHA_ crc32_t group_name_crc);
			//デフォルトコンストラクタ
			inline groupInfo();
			//デストラクタ
			inline ~groupInfo();
		};
		//ハッシュテーブル操作型
		struct groupTableOpe : public GASHA_ hash_table::baseOpe<groupTableOpe, GROUP_TABLE_SIZE, groupInfo, GASHA_ crc32_t>
		{
			typedef typename table<OPE_TYPE>::lock_type lock_type;//ロック型
			static const std::size_t AUTO_REHASH_RATIO = 0;//自動リハッシュなし ※削除しないのでリハッシュ不要
		};
		//ハッシュテーブル型
		typedef GASHA_ hash_table::container<groupTableOpe> groupTable_type;
		//イテレータ型
		typedef typename groupTable_type::iterator iterator;
		typedef typename groupTable_type::const_iterator const_iterator;
	#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
		typedef typename groupTable_type::reverse_iterator reverse_iterator;
		typedef typename groupTable_type::const_reverse_iterator const_reverse_iterator;
	#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	public:
		//定数
		static const std::size_t GROUP_TABLE_BUFF_SIZE = sizeof(groupTable_type) + alignof(groupTable_type);//ハッシュテーブルバッファサイズ
		static const std::size_t VALUE_TABLE_BUFF_SIZE = sizeof(valueTable_type) + alignof(valueTable_type);//ハッシュテーブルバッファサイズ
	public:
		//アクセッサ
		inline bool isRegistered(const GASHA_ crc32_t name_crc) const;//登録済みチェック
		inline bool isRegistered(const char* name) const;//登録済みチェック
		template<typename T>
		inline bool isType(const GASHA_ crc32_t name_crc) const;//型チェック ※未登録時は false
		template<typename T>
		inline bool isType(const char* name) const;//型チェック ※未登録時は false
	public:
		//定数取得
		//※値が存在しない場合は、アサーション違反となった上で、0を返す。
		template<typename T>
		T value(const GASHA_ crc32_t name_crc) const;
		template<typename T>
		inline T value(const char* name) const;
		//※次の定数を取得
		//※登録順に次の定数のCRCを返す。
		//※グループに属していない場合は、アサーション違反となった上で、0 を返す。
		//※名前が 0 または nullptr の場合は、グループの最初の値（最小値）の名前（CRC）を返す。
		//※飽和演算（デフォルト）の場合、末端に達したら末端の名前（CRC）を返す。
		//※ラップアラウンド演算の場合、末端に達したら先頭の名前（CRC）を返す。
		GASHA_ crc32_t next(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, const int step = 1) const;
		inline GASHA_ crc32_t next(const char* group_name, const char* name, const int step = 1) const;
		GASHA_ crc32_t next(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, const wraparound_tag&, const int step = 1) const;
		inline GASHA_ crc32_t next(const char* group_name, const char* name, const wraparound_tag&, const int step = 1) const;
		//※前の定数を取得
		//※登録順に前の定数のCRCを返す。
		//※グループに属していない場合は、アサーション違反となった上で、0 を返す。
		//※名前が 0 または nullptr の場合は、グループの末端の値（最大値）の名前（CRC）を返す。
		//※飽和演算（デフォルト）の場合、末端に達したら末端の名前（CRC）を返す。
		//※ラップアラウンド演算の場合、末端に達したら先頭の名前（CRC）を返す。
		GASHA_ crc32_t prev(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, const int step = 1) const;
		inline GASHA_ crc32_t prev(const char* group_name, const char* name, const int step = 1) const;
		GASHA_ crc32_t prev(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, const wraparound_tag&, const int step = 1) const;
		inline GASHA_ crc32_t prev(const char* group_name, const char* name, const wraparound_tag&, const int step = 1) const;
	private:
		//定数グループ登録
		groupInfo* registAndGetGroup(const GASHA_ crc32_t group_name_crc);
	public:
		//定数登録
		//※登録できた場合は true を返し、既に登録済みなどの理由で登録できなかった場合は false を返す。
		//※名前は全グループを通して一意である必要がある点に注意（名前付き関数と異なる）。
		//※グループは定数の列挙に使用する。
		//※グループ不要時は 0 または nullptr を指定する。この場合、next(), prev() が使用できないので注意。
		template<typename T>
		bool regist(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, const T& value);
		template<typename T>
		inline bool regist(const char* group_name, const char* name, const T& value);
		template<typename T>
		inline bool regist(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, const T&& value);
		template<typename T>
		inline bool regist(const char* group_name, const char* name, const T&& value);
		//定数登録解除
		//※登録解除できた場合は true を返し、未登録などの理由で登録解除できなかった場合は false を返す。
		//※登録解除してもグループは消えないので注意。
		bool unregist(const GASHA_ crc32_t name_crc);
		inline bool unregist(const char* name);
	public:
		////クリア
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
		//※定数情報
		inline std::size_t max_size() const;
		inline std::size_t size() const;
		inline std::size_t remain() const;
		inline bool empty() const;
		inline bool full() const;
		//※グループ情報
		inline std::size_t groupMaxSize() const;
		inline std::size_t groupSize() const;
		inline std::size_t groupRemain() const;
		inline bool groupIsEmpty() const;
		inline bool groupIsFull() const;
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
		//ハッシュテーブルアクセス：情報取得系メソッド
		inline const groupInfo* findGroup(const GASHA_ crc32_t group_name_crc) const;
		inline const groupInfo* findGroup(const char* group_name) const;
		inline const valueInfo* findValue(const GASHA_ crc32_t name_crc) const;
		inline const valueInfo* findValue(const char* name) const;
	public:
		//明示的な初期化用コンストラクタ
		inline table(const explicitInit_tag&);
		//デフォルトコンストラクタ
		inline table();
		//デストラクタ
		inline ~table();
	private:
		//フィールド
		groupTable_type* m_groupTable;//定数グループハッシュテーブル（参照）
		valueTable_type* m_valueTable;//定数ハッシュテーブル（参照）
	private:
		//静的フィールド
		static std::once_flag m_initialized;//テーブル初期化済み
		static char m_groupTableBuff[];//定数グループハッシュテーブル（バッファ）
		static char m_valueTableBuff[];//定数ハッシュテーブル（バッファ）
	public:
		//静的フィールド
		static const explicitInit_tag explicitInit;//明示的な初期化指定用
		static const wraparound_tag wraparound;//ラップアラウンド演算指定用
		static const saturation_tag saturation;//飽和演算指定用
	};
}//namespace named_value

//--------------------
//クラスの別名
//※ネームスペースの指定を省略してクラスを使用するための別名

//名前付き定数操作用テンプレート構造体
template<class OPE_TYPE, std::size_t _GROUP_TABLE_SIZE, std::size_t _TABLE_SIZE>
using namedValue_baseOpe = named_value::baseOpe<OPE_TYPE, _GROUP_TABLE_SIZE, _TABLE_SIZE>;

//名前付き定数
template<class OPE_TYPE>
using namedValue = named_value::table<OPE_TYPE>;

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/named_value.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_NAMED_VALUE_ALLWAYS_TOGETHER_CPP_H
#include <gasha/named_value.cpp.h>
#endif//GASHA_NAMED_VALUE_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_NAMED_VALUE_H

// End of file

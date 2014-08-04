#pragma once
#ifndef GASHA_INCLUDED_NAMED_FUNC_H
#define GASHA_INCLUDED_NAMED_FUNC_H

//--------------------------------------------------------------------------------
// named_func.h
// 名前付き関数【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/crc32.h>//CRC32操作
#include <gasha/hash_table.h>//ハッシュテーブル
#include <gasha/singly_linked_list.h>//片方向連結リスト
#include <gasha/dummy_shared_lock.h>//ダミー共有ロック
#include <gasha/unique_shared_lock.h>//単一共有ロック

#include <cstddef>//std::size_t
#include <cstdint>//std::uint64_t
#include <typeinfo>//std::type_info

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <mutex>//C++11 std::call_once
#include <functional>//C++11 std::function, std::mem_fn
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//名前付き関数
//--------------------------------------------------------------------------------

namespace named_func
{
	//--------------------
	//名前付き関数操作用テンプレート構造体
	//※CRTPを活用し、下記のような派生構造体を作成して使用する
	//  //struct 派生構造体名 : public named_func::baseOpe<派生構造体名, 関数グループテーブルサイズ, 関数テーブルサイズ>
	//	struct ope : public named_func::baseOpe<ope, GROUP_TABLE_SIZE, FUNC_TABLE_SIZE>
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
		static const std::size_t GROUP_TABLE_SIZE = _GROUP_TABLE_SIZE;//関数グループテーブルサイズ（元の指定サイズ）※扱えるデータの要素数
		static const std::size_t FUNC_TABLE_SIZE = _FUNC_TABLE_SIZE;//関数テーブルサイズ（元の指定サイズ）※扱えるデータの要素数

		//ロックポリシー
		typedef GASHA_ dummySharedLock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※共有ロック（リード・ライトロック）でテーブル操作をスレッドセーフにしたい場合は、
		//　baseOpeの派生クラスにて、有効な共有ロック型（sharedSpinLock など）を
		//　lock_type 型として再定義する。
	};

	//----------------------------------------
	//名前付き関数クラス
	template<class OPE_TYPE>
	class table
	{
	public:
		//型
		typedef OPE_TYPE ope_type;
		typedef typename ope_type::lock_type lock_type;
		typedef std::uint64_t infoKey_type;//関数情報キー型
	public:
		//定数
		static const std::size_t GROUP_TABLE_SIZE = ope_type::GROUP_TABLE_SIZE;//関数グループテーブルサイズ
		static const std::size_t FUNC_TABLE_SIZE = ope_type::FUNC_TABLE_SIZE;//関数テーブルサイズ
	public:
		//型
		struct explicitInit_tag{};//明示的な初期化用構造体
		struct constFunc_tag{};//const関数指定（読み取り専用関数指定）用構造体
		struct writableFunc_tag{};//書き込み可能関数指定用構造体
		//--------------------
		//関数情報
		enum func_type//関数種別
		{
			CONST_FUNCTION = 0,//const関数（読み取り専用関数）
			WRITABLE_FUNCTION,//書き込み可能関数
		};
		//関数情報
		struct groupInfo;
		struct funcInfoKey
		{
			//フィールド
			union
			{
				struct
				{
					GASHA_ crc32_t m_groupNameCrc;//グループ名のCRC
					GASHA_ crc32_t m_nameCrc;//名前のCRC
				};
				infoKey_type m_key;//ハッシュキー
			};

			//キャストオペレータ
			inline operator infoKey_type() const { return m_key; }
			//比較オペレータ
			inline bool operator<(const funcInfoKey& rhs) const;
			inline bool operator==(const funcInfoKey& rhs) const;
			inline bool operator!=(const funcInfoKey& rhs) const;
			inline bool operator==(const infoKey_type& rhs) const;
			//ムーブ／コピーオペレータ
			inline funcInfoKey& operator=(funcInfoKey&& rhs);
			inline funcInfoKey& operator=(const funcInfoKey& rhs);
			//ムーブ／コピーコンストラクタ
			inline funcInfoKey(funcInfoKey&& obj);
			inline funcInfoKey(const funcInfoKey& obj);
			//コンストラクタ
			inline funcInfoKey(GASHA_ crc32_t group_name_crc, GASHA_ crc32_t name_crc);
		};
		struct funcInfo
		{
			//フィールド
			funcInfoKey m_key;//ハッシュキー
			const std::type_info* m_retTypeInfo;//型情報 ※戻り値の型
			const std::type_info* m_objTypeInfo;//型情報 ※オブジェクトの型 ※nullptrならオブジェクト用ではない
			func_type m_funcType;//関数種別
			std::function<void()> m_func;//関数
			const mutable funcInfo* m_next;//関数情報の連結リスト

			//比較オペレータ
			inline bool operator<(const funcInfo& rhs) const;
			inline bool operator==(const funcInfo& rhs) const;
			inline bool operator!=(const funcInfo& rhs) const;
			inline bool operator==(const funcInfoKey& rhs) const;
			inline bool operator==(const infoKey_type& rhs) const;

			//コンストラクタ
			template<typename RET, typename... Tx>
			inline funcInfo(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, std::function<RET(Tx...)> function, const writableFunc_tag&);
			template<typename RET, typename... Tx>
			inline funcInfo(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, std::function<RET(Tx...)> function, const constFunc_tag&);
			template<typename RET, class OBJ, typename... Tx>
			inline funcInfo(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, OBJ*, std::function<RET(OBJ&, Tx...)> function, const writableFunc_tag&);
			template<typename RET, class OBJ, typename... Tx>
			inline funcInfo(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, OBJ*, std::function<RET(OBJ&, Tx...)> function, const constFunc_tag&);
			//デフォルトコンストラクタ
			inline funcInfo();
			//デストラクタ
			inline ~funcInfo();
		};
		//ハッシュテーブル操作型
		//※テーブル操作時は、関数グループテーブルのロックを使用
		struct funcTableOpe : public GASHA_ hash_table::baseOpe<funcTableOpe, FUNC_TABLE_SIZE, funcInfo, infoKey_type>
		{
			static const std::size_t AUTO_REHASH_RATIO = 0;//自動リハッシュなし ※リハッシュすると、オブジェクトのポインタがずれて、連結リストに問題が生じる可能性がある
		};
		//ハッシュテーブル型
		typedef GASHA_ hash_table::container<funcTableOpe> funcTable_type;
		//片方向連結リスト操作型
		//※リスト操作時は、関数グループテーブルのロックを使用
		struct funcListOpe : public GASHA_ singly_linked_list::baseOpe<funcListOpe, funcInfo>
		{
			typedef typename GASHA_ singly_linked_list::baseOpe<funcListOpe, funcInfo>::node_type node_type;//型があいまいになってコンパイルエラーになることがあるので型を再定義

			//次ノードを取得
			inline static const node_type* getNext(const node_type& node){ return node.m_next; }
			//次ノードを変更
			inline static void setNext(node_type& node, const node_type* next){ node.m_next = next; }
		};
		//片方向連結リスト型
		typedef GASHA_ singly_linked_list::container<funcListOpe> funcList_type;
		//--------------------
		//関数グループ情報
		struct groupInfo
		{
			//フィールド
			GASHA_ crc32_t m_groupNameCrc;//グループ名のCRC
			mutable lock_type m_lock;//ロック型
			funcList_type m_funcList;//関数リスト

			//関数リストイテレータを取得
			//※リバースイテレータ非対応
			inline const typename funcList_type::iterator cbegin() const;
			inline const typename funcList_type::iterator cend() const;
			inline const typename funcList_type::iterator begin() const;
			inline const typename funcList_type::iterator end() const;
			
			//関数リスト情報を取得
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
		static const std::size_t FUNC_TABLE_BUFF_SIZE = sizeof(funcTable_type) + alignof(funcTable_type);//ハッシュテーブルバッファサイズ
	public:
		//アクセッサ
		inline bool isRegistered(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc) const;//登録済みチェック
		inline bool isRegistered(const char* group_name, const char* name) const;//登録済みチェック
		template<typename RET>
		inline bool isConstType(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc) const;//const関数チェック ※未登録時は false
		template<typename RET>
		inline bool isConstType(const char* group_name, const char* name) const;//const関数チェック ※未登録時は false
		template<typename RET, class OBJ>
		inline bool isConstObj(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc) const;//const関数チェック ※未登録時は false
		template<typename RET, class OBJ>
		inline bool isConstObj(const char* group_name, const char* name) const;//const関数チェック ※未登録時は false
		template<typename RET>
		inline bool isWritable(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc) const;//書き込み可能関数チェック ※未登録時は false
		template<typename RET>
		inline bool isWritable(const char* group_name, const char* name) const;//書き込み可能関数チェック ※未登録時は false
		template<typename RET, class OBJ>
		inline bool isWritableObj(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc) const;//書き込み可能関数チェック ※未登録時は false
		template<typename RET, class OBJ>
		inline bool isWritableObj(const char* group_name, const char* name) const;//書き込み可能関数チェック ※未登録時は false
	private:
		//関数呼び出し（共通処理）
		//※戻り値あり
		//※const関数
		template<typename RET, class LOCK_TYPE, typename... Tx>
		inline RET _func(const std::function<void()>& func, const constFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args) const;
		//※書き込み可能関数
		template<typename RET, class LOCK_TYPE, typename... Tx>
		inline RET _func(const std::function<void()>& func, const writableFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args);
		//※戻り値なし
		//※const関数
		template<class LOCK_TYPE, typename... Tx>
		inline void _proc(const std::function<void()>& func, const constFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args) const;
		//※書き込み可能関数
		template<class LOCK_TYPE, typename... Tx>
		inline void _proc(const std::function<void()>& func, const writableFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args);
		//オブジェクトメンバー関数呼び出し（共通処理）
		//※戻り値あり
		//※const関数
		template<typename RET, class OBJ, class LOCK_TYPE, typename... Tx>
		inline RET _objFunc(const OBJ& obj, const std::function<void()>& func, const constFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args) const;
		//※書き込み可能関数
		template<typename RET, class OBJ, class LOCK_TYPE, typename... Tx>
		inline RET _objFunc(OBJ& obj, const std::function<void()>& func, const writableFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args);
		//※戻り値なし
		//※const関数
		template<class OBJ, class LOCK_TYPE, typename... Tx>
		inline void _objProc(const OBJ& obj, const std::function<void()>& func, const constFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args) const;
		//※書き込み可能関数
		template<class OBJ, class LOCK_TYPE, typename... Tx>
		inline void _objProc(OBJ& obj, const std::function<void()>& func, const writableFunc_tag&, LOCK_TYPE&& lock_obj, Tx&&... args);
	public:
		//関数呼び出し
		//※戻り値のスコープを抜けるまで、const関数なら共有ロックを、書き込み可能関数なら排他ロックを取得する
		//※戻り値あり
		//※const関数
		template<typename RET, typename... Tx>
		RET func(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args) const;
		template<typename RET, typename... Tx>
		inline RET func(const char* group_name, const char* name, Tx&&... args) const;
		//※書き込み可能関数
		template<typename RET, typename... Tx>
		RET func(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args);
		template<typename RET, typename... Tx>
		inline RET func(const char* group_name, const char* name, Tx&&... args);
		//※戻り値なし
		//※const関数
		template<typename... Tx>
		void proc(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args) const;
		template<typename... Tx>
		inline void proc(const char* group_name, const char* name, Tx&&... args) const;
		//※書き込み可能関数
		template<typename... Tx>
		void proc(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args);
		template<typename... Tx>
		inline void proc(const char* group_name, const char* name, Tx&&... args);
		//オブジェクトメンバー関数呼び出し
		//※オブジェクトを引数指定する
		//※戻り値のスコープを抜けるまで、const関数なら共有ロックを、書き込み可能関数なら排他ロックを取得する
		//※戻り値あり
		//※const関数
		template<typename RET, class OBJ, typename... Tx>
		RET func(const OBJ& obj, const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args) const;
		template<typename RET, class OBJ, typename... Tx>
		inline RET func(const OBJ& obj, const char* group_name, const char* name, Tx&&... args) const;
		//※書き込み可能関数
		template<typename RET, class OBJ, typename... Tx>
		RET func(OBJ& obj, const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args);
		template<typename RET, class OBJ, typename... Tx>
		inline RET func(OBJ& obj, const char* group_name, const char* name, Tx&&... args);
		//※戻り値なし
		//※const関数
		template<class OBJ, typename... Tx>
		void proc(const OBJ& obj, const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args) const;
		template<class OBJ, typename... Tx>
		inline void proc(const OBJ& obj, const char* group_name, const char* name, Tx&&... args) const;
		//※書き込み可能関数
		template<class OBJ, typename... Tx>
		void proc(OBJ& obj, const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, Tx&&... args);
		template<class OBJ, typename... Tx>
		inline void proc(OBJ& obj, const char* group_name, const char* name, Tx&&... args);
	private:
		//関数グループ登録
		groupInfo* registAndGetGroup(const GASHA_ crc32_t group_name_crc);
		//関数登録用構造体
		template<class>
		struct funcSpec;
		template<typename RET, typename... Tx>
		struct funcSpec<RET(Tx...)>{
			typedef RET return_type;
			std::function<RET(Tx...)> m_func;
			template<class FUNCTION>
			inline funcSpec(FUNCTION func) : m_func(func) {}
		};
		//関数登録用構造体（オブジェクト用）
		template<class>
		struct memberFuncSpec;
		template<typename RET, class OBJ, typename... Tx>
		struct memberFuncSpec<RET(OBJ&, Tx...)>{
			typedef RET return_type;
			typedef OBJ object_type;
			std::function<RET(OBJ&, Tx...)> m_func;
			template<class FUNCTION>
			inline memberFuncSpec(FUNCTION func) : m_func(std::mem_fn(func)) {}
		};
		//関数登録用構造体（const オブジェクト用）
		template<class>
		struct cmemberFuncSpec;
		template<typename RET, class OBJ, typename... Tx>
		struct cmemberFuncSpec<RET(OBJ&, Tx...)>{
			typedef RET return_type;
			typedef const OBJ object_type;
			std::function<RET(const OBJ&, Tx...)> m_func;
			template<class FUNCTION>
			inline cmemberFuncSpec(FUNCTION func) : m_func(std::mem_fn(func)) {}
		};
	public:
		//関数登録
		//※登録できた場合は true を返し、既に登録済みなどの理由で登録できなかった場合は false を返す。
		template<class FUNC_SPEC, class FUNCTION>
		bool regist(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, FUNCTION function);
		template<class FUNC_SPEC, class FUNCTION>
		inline bool regist(const char* group_name, const char* name, FUNCTION function);
		//※const関数
		template<class FUNC_SPEC, class FUNCTION>
		bool regist(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, FUNCTION function, const constFunc_tag&);
		template<class FUNC_SPEC, class FUNCTION>
		inline bool regist(const char* group_name, const char* name, FUNCTION function, const constFunc_tag&);
		//オブジェクトメンバー関数登録
		template<class FUNC_SPEC, class FUNCTION>
		bool registObj(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, FUNCTION function);
		template<class FUNC_SPEC, class FUNCTION>
		inline bool registObj(const char* group_name, const char* name, FUNCTION function);
		//※const関数
		template<class FUNC_SPEC, class FUNCTION>
		bool registObj(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc, FUNCTION function, const constFunc_tag&);
		template<class FUNC_SPEC, class FUNCTION>
		inline bool registObj(const char* group_name, const char* name, FUNCTION function, const constFunc_tag&);
		//関数登録解除
		//※登録解除できた場合は true を返し、未登録などの理由で登録解除できなかった場合は false を返す。
		//※登録解除してもグループは消えないので注意。
		bool unregist(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc);
		inline bool unregist(const char* group_name, const char* name);
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
		//※関数情報
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
		inline const funcInfo* findFunc(const GASHA_ crc32_t group_name_crc, const GASHA_ crc32_t name_crc) const;
		inline const funcInfo* findFunc(const char* group_name, const char* name) const;
	public:
		//明示的な初期化用コンストラクタ
		inline table(const explicitInit_tag&);
		//デフォルトコンストラクタ
		inline table();
		//デストラクタ
		inline ~table();
	private:
		//フィールド
		groupTable_type* m_groupTable;//関数グループハッシュテーブル（参照）
		funcTable_type* m_funcTable;//関数ハッシュテーブル（参照）
	private:
		//静的フィールド
		static std::once_flag m_initialized;//テーブル初期化済み
		static char m_groupTableBuff[];//関数グループハッシュテーブル（バッファ）
		static char m_funcTableBuff[];//関数ハッシュテーブル（バッファ）
	public:
		//静的フィールド
		static const explicitInit_tag explicitInit;//明示的な初期化指定用
		static const constFunc_tag constFunc;//const関数指定用
		static const writableFunc_tag writableFunc;//書き込み可能関数指定用
	};
}//namespace named_func

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/named_func.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_NAMED_FUNC_ALLWAYS_TOGETHER_CPP_H
#include <gasha/named_func.cpp.h>
#endif//GASHA_NAMED_FUNC_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_NAMED_FUNC_H

// End of file

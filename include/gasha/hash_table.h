﻿#pragma once
#ifndef __HASH_TABLE_H_
#define __HASH_TABLE_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// hash_table.h
// ハッシュテーブルコンテナ【宣言部】※データのメモリ管理を行わない擬似コンテナ
//
// ※コンテナをインスタンス化する際は、別途下記のファイルをインクルードする必要あり
//
//   ・hash_table.inl   ... 【インライン関数／テンプレート関数定義部】
//                            コンテナクラスの操作が必要な場所でインクルード。
//   ・hash_table.cpp.h ... 【関数定義部】
//                            コンテナクラスの実体化が必要な場所でインクルード。
//
// ※面倒なら三つまとめてインクルードして使用しても良いが、分けた方が、
// 　コンパイル・リンク時間の短縮、および、クラス修正時の影響範囲の抑制になる。
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dummy_shared_lock.h>//ダミー共有ロック
#include <gasha/lock_guard.h>//スコープロック
#include <gasha/shared_lock_guard.h>//スコープ共有ロック
#include <gasha/unique_shared_lock.h>//単一共有ロック

#include <gasha/basic_math.h>//基本算術（素数計算）
#include <gasha/crc32.h>//CRC32

#include <cstddef>//std::size_t, std::ptrdiff_t

//【VC++】例外を無効化した状態で <iterator> <bitset> <string> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <iterator>//std::iterator用
#include <bitset>//std::bitset
#include <string>//std::string


//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//開番地法ハッシュテーブル
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//データ構造とアルゴリズム
//--------------------------------------------------------------------------------
//【特徴】
//・キーからハッシュキーを算出して配列要素にアクセスするアルゴリズムにより、
//  O(1) に近い探索性能が得られる。
//・データ登録時にハッシュキーが衝突した際、ダブルハッシュアルゴリズムによる
//  二次キーの算出により、再衝突の頻度を抑え、良好な探索性能を極力維持する。
//--------------------------------------------------------------------------------
//【利点】
//・テーブルの要素の探索がほぼO(1)で行える。
//・テーブルへの要素の追加がほぼO(1)で行える。
//・テーブルの要素の削除がほぼO(1)で行える。
//--------------------------------------------------------------------------------
//【欠点】
//・要素の昇順・降順アクセスができない。（最小値・最大値探索もできない）
//・要素ごとのメモリオーバーヘッドはないが、十分なパフォーマンスを得るために、
//  余分な未使用要素を多数必要とする。
//・未使用要素が少なくなると、探索時間がO(n)まで悪化する可能性がある。
//・要素の追加と削除を繰り返すと、削除済みデータがテーブルを圧迫し、探索性能の
//  劣化を招く。（それを防ぐために、リハッシュの機能を用意している。）
//・イテレータは用意しているが、ハッシュキー順の配列であるため、見た目には
//  ランダムな順序となる。
//--------------------------------------------------------------------------------
//【本プログラムにおける実装要件】
//・固定配列のコンテナとし、最大要素数の自動拡張を行わない。
//・ノードのキーと型をユーザー定義可能とする。
//　コンテナ操作用のユーザー定義構造体に、両方の型を定義して扱うものとする。
//・キーの重複を許容しない。
//・文字列キー（std::string/char*）をサポートしない。
//  文字列キーの代わりに、文字列のcrc32値を扱う。（文字列は保持しない）
//・コンテナは、STLの std::unordered_map (C++11)をモデルとしたインターフェースを実装する。
//・STL（std::unordered_map）との主な違いは下記のとおり。
//    - 固定長のため、自動拡張を行わない。
//    - リハッシュは実装するが、配列の拡張は行わず、削除済み要素のパージのみを行う。
//    - 開番地法ハッシュテーブルのため、十分に空きがある時の処理性能がSTLよりも良い。
//      STLは連鎖法ハッシュテーブルであり、空きがない状況では圧倒的に処理性能が良い。
//    - 重複キーを許容しない。（std::unordered_multimapのようには使えない。）
//      補足：開番地法では重複キーを（効率的に）処理できない。
//    - 必ずキーと値を扱う。（std::unordered_set/unordered_multisetのようには使えない。）
//    - 例外を扱わない。そのため、イテレータへの isExist() メソッドの追加や、
//      at()メソッドが返す情報がポインターなど、インターフェースの相違点がある。
//    - （他のコンテナと同様に）コンテナ操作対象・方法を設定した
//      構造体をユーザー定義して用いる。
//--------------------------------------------------------------------------------
//【想定する具的的な用途】
//・あまり追加・削除を頻繁に行わず、高速な探索性能が求められるデータ。
//・スクリプト用のコールバック関数。
//--------------------------------------------------------------------------------

namespace hash_table
{
	//データ置換属性
	enum replaceAttr_t
	{
		NEVER_REPLACE,//キーが重複するデータは登録できない（置換しない）
		REPLACE,//キーが重複するデータは置換して登録する
	};
	
	//--------------------
	//開番地法ハッシュテーブル操作用テンプレート構造体
	//※CRTPを活用し、下記のような派生構造体を作成して使用する
	//  //struct 派生構造体名 : public hash_table::baseOpe_t<派生構造体名, 要素の型, キーの型, キーの最小値= 0u, キーの最大値 = 0xffffffffu, 不正なキー = 0xffffffffu>
	//	//※文字列キーを扱いたい場合は、キー型に crc32_t を指定すること
	//	struct ope_t : public hash_table::baseOpe_t<ope_t, data_t, crc32_t, 500>
	//	{
	//		//データ置換属性 ※必要に応じて定義
	//		static const replaceAttr_t REPLACE_ATTR = REPLACE;//キーが重複するデータは置換して登録する
	//
	//		//キーを取得 ※必要に応じて定義
	//		inline static key_type getKey(const value_type& value){ return ???; }
	//		
	//		//ロック型 ※必要に応じて定義
	//		//※共有ロック（リード・ライトロック）でコンテナ操作をスレッドセーフにしたい場合は、
	//		//　有効な共有ロック型（shared_spin_lockなど）を lock_type 型として定義する。
	//		typedef shared_spin_lock lock_type;//ロックオブジェクト型
	//	};
	template<class OPE_TYPE, typename VALUE_TYPE, typename KEY_TYPE, KEY_TYPE _KEY_MIN = 0u, KEY_TYPE _KEY_MAX = 0xffffffffu, KEY_TYPE _INVALID_KEY = 0xffffffffu>
	struct baseOpe_t
	{
		//定数
		static const KEY_TYPE KEY_MIN = _KEY_MIN;//キーの最小値
		static const KEY_TYPE KEY_MAX = _KEY_MAX;//キーの最大値
		static const KEY_TYPE INVALID_KEY = _INVALID_KEY;//不正なキー
		static const std::size_t AUTO_REHASH_RATIO = 25;//自動リハッシュ実行の基準割合 ※削除済み件数が全体サイズの一定割合以上になったら自動リハッシュ ※0で自動リハッシュなし
		static const std::size_t FINDING_CYCLE_LIMIT = 0;//検索時の巡回回数の制限 ※0で無制限
		static const std::size_t INDEX_STEP_BASE = 5;//検索巡回時のインデックスのス歩幅の基準値 ※必ず素数でなければならない

		static const replaceAttr_t REPLACE_ATTR = NEVER_REPLACE;//キーが重複するデータは登録できない（置換しない）

		//型
		typedef OPE_TYPE ope_type;//データ操作型
		typedef VALUE_TYPE value_type;//値型
		typedef KEY_TYPE key_type;//キー型

		//ロック型
		typedef dummySharedLock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※共有ロック（リード・ライトロック）でコンテナ操作をスレッドセーフにしたい場合は、
		//　baseOpe_tの派生クラスにて、有効な共有ロック型（sharedSpinLock など）を
		//　lock_type 型として再定義する。
		//【補足①】コンテナには、あらかじめロック制御のための仕組みがソースコードレベルで
		//　　　　　仕込んであるが、有効な型を与えない限りは、実行時のオーバーヘッドは一切ない。
		//【補足②】スレッドセーフ化した場合、書き込み時の排他ロックは行われるようになるが、
		//　　　　　読み込み時の共有ロックは行っていない。読み込み時のロックは局所的なロックで
		//　　　　　済まないため、ユーザーが任意に対応しなければならない。
		//　　　　　（例）
		//　　　　　    {
		//　　　　　        auto lock = container.lockSharedScoped();//コンテナの共有ロック取得（スコープロック）
		//　　　　　        //...
		//　　　　　        //一連のイテレータ操作など
		//　　　　　        //...
		//　　　　　    }//スコープを抜ける時に自動的にロック解放

		//デストラクタ呼び出し
		static void callDestructor(value_type* obj){ obj->~VALUE_TYPE(); }
	};
	
	//--------------------
	//基本型定義マクロ
	#define DECLARE_OPE_TYPES(OPE_TYPE) \
		typedef OPE_TYPE ope_type; \
		typedef typename ope_type::value_type value_type; \
		typedef typename ope_type::key_type key_type; \
		typedef value_type& reference; \
		typedef const value_type& const_reference; \
		typedef value_type* pointer; \
		typedef const value_type* const_pointer; \
		typedef std::ptrdiff_t difference_type; \
		typedef std::size_t size_type; \
		typedef std::size_t index_type; \
		typedef typename ope_type::lock_type lock_type;
	
	//----------------------------------------
	//ハッシュテーブルコンテナ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	class container
	{
	public:
		//型
		DECLARE_OPE_TYPES(OPE_TYPE);
	public:
		//定数
		static const size_type ORIGINAL_TABLE_SIZE = _TABLE_SIZE;//テーブルサイズ（元々指定されたサイズ）
		static const size_type TABLE_SIZE = GASHA_ makeStaticPrimeGE<ORIGINAL_TABLE_SIZE>::value;//テーブルサイズ（指定サイズと同じか、それより大きい素数）
		static const size_type TABLE_SIZE_EXTENDED = TABLE_SIZE - ORIGINAL_TABLE_SIZE;//指定サイズから拡張したサイズ
		static const size_type  AUTO_REHASH_RATIO = static_cast<size_type>(ope_type::AUTO_REHASH_RATIO);//自動リハッシュ実行の基準割合 ※削除済み件数が全体サイズの一定割合以上になったら自動リハッシュ ※0で自動リハッシュなし
		static const size_type  AUTO_REHASH_SIZE = AUTO_REHASH_RATIO == 0 ? 0 : TABLE_SIZE * AUTO_REHASH_RATIO / 100;//自動リハッシュ実行の基準サイズ ※割合とテーブルサイズから計算
		static const size_type FINDING_CYCLE_LIMIT = static_cast<size_type>(ope_type::FINDING_CYCLE_LIMIT);//検索時の巡回回数の制限 ※0で無制限
		static const key_type KEY_MIN = ope_type::KEY_MIN;//キーの最小値
		static const key_type KEY_MAX = ope_type::KEY_MAX;//キーの最大値
		static const index_type INDEX_STEP_BASE = static_cast<index_type>(ope_type::INDEX_STEP_BASE);//検索巡回時のインデックスのス歩幅の基準値 ※必ず素数でなければならない
		static const index_type INVALID_INDEX = 0xffffffffu;//無効なインデックス
	public:
		//メタ関数
		//キー範囲定数計算（２バリエーション）
		template <bool COND, typename size_type, typename key_type, key_type KEY_MIN, key_type KEY_MAX>
		struct calcKeyRangeImpl{
			static const key_type value = KEY_MAX - KEY_MIN + 1;
		};
		template <typename size_type, typename key_type, key_type KEY_MIN, key_type KEY_MAX>
		struct calcKeyRangeImpl<true, size_type, key_type, KEY_MIN, KEY_MAX>{
			static const key_type value = 0;
		};
		//インデックス計算関数（２バリエーション）
		template <bool COND, typename size_type, typename index_type, typename key_type, size_type TABLE_SIZE, key_type KEY_MIN, key_type KEY_RANGE>
		struct calcIndexImpl{
			inline static index_type calc(const key_type key){ return (key - KEY_MIN) % TABLE_SIZE; }//キーからインデックスを計算 ※キーの範囲がテーブルサイズより大きい場合
		};
		template <typename size_type, typename index_type, typename key_type, size_type TABLE_SIZE, key_type KEY_MIN, key_type KEY_RANGE>
		struct calcIndexImpl<true, size_type, index_type, key_type, TABLE_SIZE, KEY_MIN, KEY_RANGE>{
			inline static index_type calc(const key_type key){ return (key - KEY_MIN) * (TABLE_SIZE / KEY_RANGE) % TABLE_SIZE; }//キーからインデックスを計算 ※キーの範囲がテーブルサイズ以下の場合
		};
	public:
		//定数
		static const key_type KEY_RANGE = calcKeyRangeImpl<((KEY_MIN == 0u && KEY_MAX == 0xffffffffu) || KEY_MIN >= KEY_MAX), size_type, key_type, KEY_MIN, KEY_MAX>::value;//キーの範囲
		//静的アサーション
		static_assert(TABLE_SIZE > INDEX_STEP_BASE, "hash_table::container: TABLE_SIZE is required larger than INDEX_STEP_BASE.");
		static_assert(GASHA_ isStaticPrime<INDEX_STEP_BASE>::value == true, "hash_table::container: INDEX_STEP_BASE is required prime.");
	public:
		//--------------------
		//イテレータ用の型
		struct set
		{
			//フィールド
			index_type m_index;//（実際の）インデックス
			index_type m_primaryIndex;//本来のインデックス
			key_type m_key;//現在のキー
			value_type* m_value;//現在の値
			bool m_isDeleted;//削除済み

			//オペレータ
			inline const_reference operator*() const { return m_value; }
			inline reference operator*(){ return m_value; }
			inline const_pointer operator->() const { return m_value; }
			inline pointer operator->(){ return m_value; }

			//実際のインデックスは、本来のインデックスと一致するか？
			inline bool isPrimaryIndex() const { return m_index == m_primaryIndex; }
			//参照を更新
			void update(const index_type index, const index_type primary_index, const key_type key, const value_type* value, const bool is_deleted);

			//ムーブオペレータ
			set& operator=(const set&& rhs);
			//コピーオペレータ
			set& operator=(const set& rhs);;
			//ムーブコンストラクタ
			set(const set&& obj);
			//コピーコンストラクタ
			set(const set& obj);
			//コストラクタ
			set(const index_type index, const index_type primary_index, const key_type key, const value_type* value, const bool is_deleted);
			//デフォルトコンストラクタ
			set() = delete;
			//デストラクタ
			inline ~set()
			{}
		};
	public:
		//--------------------
		//イテレータ宣言
		class iterator;
		class reverse_iterator;
		typedef const iterator const_iterator;
		typedef const reverse_iterator const_reverse_iterator;
		//--------------------
		//イテレータ
		class iterator : public std::iterator<std::forward_iterator_tag, value_type>
		{
			friend class container;
			friend class reverse_iterator;
		public:
			//※コンパイラによって優先して参照する型があいまいになることを避けるための定義
			typedef typename container::value_type value_type;
			typedef typename container::reverse_iterator reverse_iterator;
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const set&() const { return getSet(); }
			inline operator set&(){ return getSet(); }
			inline operator const_reference() const { return *getValue(); }
			inline operator reference(){ return *getValue(); }
			inline operator const_pointer() const { return getValue(); }
			inline operator pointer(){ return getValue(); }
			inline operator key_type() const { return getKey(); }
		public:
			//基本オペレータ
			inline const set& operator*() const { return getSet(); }
			inline set& operator*(){ return getSet(); }
			inline const_pointer operator->() const { return getValue(); }
			inline pointer operator->(){ return getValue(); }
		#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
			inline const iterator operator[](const int index) const;
			inline iterator operator[](const int index);
		#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
			//比較オペレータ
			inline bool operator==(const iterator& rhs) const;
			inline bool operator!=(const iterator& rhs) const;
		#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
			inline bool operator>(const iterator& rhs) const;
			inline bool operator>=(const iterator& rhs) const;
			inline bool operator<(const iterator& rhs) const;
			inline bool operator<=(const iterator& rhs) const;
		#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//演算オペレータ
			inline const iterator& operator++() const;
		#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
			inline const iterator& operator--() const;
		#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
			inline iterator& operator++();
		#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
			inline iterator& operator--();
		#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
			inline const iterator operator++(int) const;
		#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
			inline const iterator operator--(int) const;
		#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
			inline iterator operator++(int);
		#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
			inline iterator operator--(int);
		#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
		#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE//std::forward_iterator_tag には本来必要ではない
			inline const iterator& operator+=(const int rhs) const;
			inline const iterator& operator+=(const std::size_t rhs) const { return operator+=(static_cast<int>(rhs)); }
		#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
			inline const iterator& operator-=(const int rhs) const;
			inline const iterator& operator-=(const std::size_t rhs) const { return operator-=(static_cast<int>(rhs)); }
		#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
			inline iterator& operator+=(const int rhs);
			inline iterator& operator+=(const std::size_t rhs) { return operator+=(static_cast<int>(rhs)); }
		#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
			inline iterator& operator-=(const int rhs);
			inline iterator& operator-=(const std::size_t rhs) { return operator-=(static_cast<int>(rhs)); }
		#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
			inline const iterator operator+(const int rhs) const;
			inline const iterator operator+(const std::size_t rhs) const { return operator+(static_cast<int>(rhs)); }
		#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
			inline const iterator operator-(const int rhs) const;
			inline const iterator operator-(const std::size_t rhs) const { return operator-(static_cast<int>(rhs)); }
		#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
			inline iterator operator+(const int rhs);
			inline iterator operator+(const std::size_t rhs) { return operator+(static_cast<int>(rhs)); }
		#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
			inline iterator operator-(const int rhs);
			inline iterator operator-(const std::size_t rhs) { return operator-(static_cast<int>(rhs)); }
		#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
			//inline int operator-(const iterator& rhs) const;
		#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//アクセッサ
			inline bool isExist() const;
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const;
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const;//終端か？
			inline const set& getSet() const;//現在のセット
			inline set& getSet();// 現在のセット
			inline index_type getIndex() const;//（実際の）インデックス
			inline index_type getPrimaryIndex() const;//本来のインデックス
			inline key_type getKey() const;//現在のキー
			inline const value_type* getValue() const;//現在の値
			inline value_type* getValue();//現在の値
			inline bool isDeleted() const;//削除済み
			inline bool isPrimaryIndex() const;//本来のインデックスか？
		private:
			//参照を更新
			index_type update(const index_type index) const;
			void updateNext() const;
			void updatePrev() const;
			void updateForward(const int step) const;
			void updateBackward(const int step) const;
		public:
			//ムーブオペレータ
			iterator& operator=(const iterator&& rhs);
		#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
			iterator& operator=(const reverse_iterator&& rhs);
		#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
			//コピーオペレータ
			iterator& operator=(const iterator& rhs);
		#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
			iterator& operator=(const reverse_iterator& rhs);
		#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
		public:
			//ムーブコンストラクタ
			iterator(const iterator&& obj);
		#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
			iterator(const reverse_iterator&& obj);
		#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
			//コピーコンストラクタ
			inline iterator(const iterator& obj);
		#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
			iterator(const reverse_iterator& obj);
		#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
			//コンストラクタ
			inline iterator(const container& con, const bool is_end);
			//デフォルトコンストラクタ
			inline iterator() :
				m_con(nullptr),
				m_set(INVALID_INDEX, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false),
				m_isEnd(false)
			{}
			//デストラクタ
			inline ~iterator()
			{}
		protected:
			//フィールド
			const container* m_con;//コンテナ
			mutable set m_set;//現在のキー/値/削除済みフラグ
			mutable bool m_isEnd;//終端か？
		};
	#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR//std::forward_iterator_tag には本来必要ではない
		//--------------------
		//リバースイテレータ
		//class reverse_iterator : public std::reverse_iterator<iterator>
		class reverse_iterator : public std::iterator<std::forward_iterator_tag, value_type>
		{
			friend class container;
			friend class iterator;
		public:
			//※コンパイラによって優先して参照する型があいまいになることを避けるための定義
			typedef typename container::value_type value_type;
			typedef typename container::iterator iterator;
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const set&() const { return getSet(); }
			inline operator set&(){ return getSet(); }
			inline operator const_reference() const { return *getValue(); }
			inline operator reference(){ return *getValue(); }
			inline operator const_pointer() const { return getValue(); }
			inline operator pointer(){ return getValue(); }
			inline operator key_type() const { return getKey(); }
		public:
			//基本オペレータ
			inline const set& operator*() const { return getSet(); }
			inline set& operator*(){ return getSet(); }
			inline const_pointer operator->() const { return getValue(); }
			inline pointer operator->(){ return getValue(); }
		#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
			inline const reverse_iterator operator[](const int index) const;
			inline reverse_iterator operator[](const int index);
		#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//比較オペレータ
			inline bool operator==(const reverse_iterator& rhs) const;
			inline bool operator!=(const reverse_iterator& rhs) const;
		#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
			inline bool operator>(const reverse_iterator& rhs) const;
			inline bool operator>=(const reverse_iterator& rhs) const;
			inline bool operator<(const reverse_iterator& rhs) const;
			inline bool operator<=(const reverse_iterator& rhs) const;
		#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//演算オペレータ
			inline const reverse_iterator& operator++() const;
			inline const reverse_iterator& operator--() const;
			inline reverse_iterator& operator++();
			inline reverse_iterator& operator--();
			inline const reverse_iterator operator++(int) const;
			inline const reverse_iterator operator--(int) const;
			inline reverse_iterator operator++(int);
			inline reverse_iterator operator--(int);
		#ifdef GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
			inline const reverse_iterator& operator+=(const int rhs) const;
			inline const reverse_iterator& operator+=(const std::size_t rhs) const { return operator+=(static_cast<int>(rhs)); }
			inline const reverse_iterator& operator-=(const int rhs) const;
			inline const reverse_iterator& operator-=(const std::size_t rhs) const { return operator-=(static_cast<int>(rhs)); }
			inline reverse_iterator& operator+=(const int rhs);
			inline reverse_iterator& operator+=(const std::size_t rhs) { return operator+=(static_cast<int>(rhs)); }
			inline reverse_iterator& operator-=(const int rhs);
			inline reverse_iterator& operator-=(const std::size_t rhs) { return operator-=(static_cast<int>(rhs)); }
			inline const reverse_iterator operator+(const int rhs) const;
			inline const reverse_iterator operator+(const std::size_t rhs) const { return operator+(static_cast<int>(rhs)); }
			inline const reverse_iterator operator-(const int rhs) const;
			inline const reverse_iterator operator-(const std::size_t rhs) const { return operator-(static_cast<int>(rhs)); }
			inline reverse_iterator operator+(const int rhs);
			inline reverse_iterator operator+(const std::size_t rhs) { return operator+(static_cast<int>(rhs)); }
			inline reverse_iterator operator-(const int rhs);
			inline reverse_iterator operator-(const std::size_t rhs) { return operator-(static_cast<int>(rhs)); }
			//inline int operator-(const reverse_iterator& rhs) const;
		#endif//GASHA_HASH_TABLE_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//アクセッサ
			inline bool isExist() const;
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const;
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const;//終端か？
			inline const set& getSet() const;//現在のセット
			inline set& getSet();//現在のセット
			inline index_type getIndex() const;//（実際の）インデックス
			inline index_type getPrimaryIndex() const;//本来のインデックス
			inline key_type getKey() const;//現在のキー
			inline const value_type* getValue() const;//現在の値
			inline value_type* getValue();//現在の値
			inline bool isDeleted() const;//削除済み
			inline bool isPrimaryIndex() const;//本来のインデックスか？
		private:
			//参照を更新
			index_type update(const index_type index) const;
			void updateNext() const;
			void updatePrev() const;
			void updateForward(const int step) const;
			void updateBackward(const int step) const;
		public:
			//ベースを取得
			inline const iterator base() const;
			inline iterator base();
		public:
			//ムーブオペレータ
			reverse_iterator& operator=(const reverse_iterator&& rhs);
			reverse_iterator& operator=(const iterator&& rhs);
			//コピーオペレータ
			reverse_iterator& operator=(const reverse_iterator& rhs);
			reverse_iterator& operator=(const iterator& rhs);
		public:
			//ムーブコンストラクタ
			reverse_iterator(const reverse_iterator&& obj);
			reverse_iterator(const iterator&& obj);
			//コピーコンストラクタ
			reverse_iterator(const reverse_iterator& obj);
			reverse_iterator(const iterator& obj);
			//コンストラクタ
			reverse_iterator(const container& con, const bool is_end);
			//デフォルトコンストラクタ
			inline reverse_iterator() :
				m_con(nullptr),
				m_set(INVALID_INDEX, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false),
				m_isEnd(false)
			{}
			//デストラクタ
			inline ~reverse_iterator()
			{}
		protected:
			//フィールド
			const container* m_con;//コンテナ
			mutable set m_set;//現在のキー/値/削除済みフラグ
			mutable bool m_isEnd;//終端か？
		};
	#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	public:
		//アクセッサ
		//※マルチスレッドで処理する際は、一連の処理ブロック全体の前後で
		//　リードロックの取得を行うようにすること。
		value_type* at(const key_type key){ return findValue(key); }
		const value_type* at(const key_type key) const { return findValue(key); }
		value_type* operator[](const key_type key){ return findValue(key); }
		const value_type* operator[](const key_type key) const { return findValue(key); }
		//※文字列キーはCRC32に変換して扱う
		value_type* at(const char* key){ return findValue(key); }
		const value_type* at(const char* key) const { return findValue(key); }
		value_type* operator[](const char* key){ return findValue(key); }
		const value_type* operator[](const char* key) const { return findValue(key); }
		//※std::stringの場合も同様
		value_type* at(const std::string& key){ return findValue(key); }
		const value_type* at(const std::string& key) const { return findValue(key); }
		value_type* operator[](const std::string& key){ return findValue(key); }
		const value_type* operator[](const std::string& key) const { return findValue(key); }
		//※ノード型からキーを取得して探索することにも対応
		value_type* at(const value_type& value){ return findValue(value); }
		const value_type* at(const value_type& value) const { return findValue(value); }
		value_type* operator[](const value_type& value){ return findValue(value); }
		const value_type* operator[](const value_type& value) const { return findValue(value); }
	public:
		//キャストオペレータ
		inline operator lock_type&(){ return m_lock; }//共有ロックオブジェクト
		inline operator lock_type&() const { return m_lock; }//共有ロックオブジェクト ※mutable
	public:
		//メソッド：ロック取得系
		//単一ロック取得
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(){ GASHA_ unique_shared_lock<lock_type> lock(*this); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ with_lock_t){ GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ with_lock); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ with_lock_shared_t){ GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ with_lock_shared); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ try_lock_t){ GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ try_lock); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ try_lock_shared_t){ GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ try_lock_shared); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ adopt_lock_t){ GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ adopt_lock); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ adopt_shared_lock_t){ GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ adopt_shared_lock); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ defer_lock_t){ GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ defer_lock); return lock; }
		//スコープロック取得
		inline GASHA_ lock_guard<lock_type> lockScoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT){ GASHA_ lock_guard<lock_type> lock(*this); return lock; }
		inline GASHA_ shared_lock_guard<lock_type> lockSharedScoped(const int spin_count = GASHA_ DEFAULT_SPIN_COUNT){ GASHA_ shared_lock_guard<lock_type> lock(*this); return lock; }
	public:
		//メソッド：イテレータ取得系
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		//イテレータ取得
		inline const iterator cbegin() const { iterator ite(*this, false); return ite; }
		inline const iterator cend() const { iterator ite(*this, true); return ite; }
		inline const iterator begin() const { iterator ite(*this, false); return ite; }
		inline const iterator end() const { iterator ite(*this, true); return ite; }
		inline iterator begin() { iterator ite(*this, false); return ite; }
		inline iterator end() { iterator ite(*this, true); return ite; }
	#ifdef GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
		//リバースイテレータを取得
		inline const reverse_iterator crbegin() const { reverse_iterator ite(*this, false); return ite; }
		inline const reverse_iterator crend() const { reverse_iterator ite(*this, true); return ite; }
		inline const reverse_iterator rbegin() const { reverse_iterator ite(*this, false); return ite; }
		inline const reverse_iterator rend() const { reverse_iterator ite(*this, true); return ite; }
		inline reverse_iterator rbegin() { reverse_iterator ite(*this, false); return ite; }
		inline reverse_iterator rend() { reverse_iterator ite(*this, true); return ite; }
	#endif//GASHA_HASH_TABLE_ENABLE_REVERSE_ITERATOR
	public:
		//メソッド：基本情報系
		inline size_type max_size() const { return TABLE_SIZE; }//最大要素数を取得
		inline size_type capacity() const { return TABLE_SIZE; }//最大要素数を取得
		inline size_type size() const { return m_usingCount - m_deletedCount; }//使用中の要素数を取得
		inline size_type remain() const { return TABLE_SIZE - size(); }//残りの要素数を取得
		inline bool empty() const { return size() == 0; }//空か？
		inline bool full() const { return size() == TABLE_SIZE; }//満杯か？
		inline size_type bucket_count() const { return TABLE_SIZE; }//最大要素数を取得
		inline size_type max_bucket_count() const { return TABLE_SIZE; }//最大要素数を取得
		inline size_type bucket(const key_type key) const { return _findIndex(key); }//キーに対応するインデックスを取得
		inline size_type bucket(const char* key) const { return _findIndex(key); }//キーに対応するインデックスを取得
		inline size_type bucket(const std::string key) const { return _findIndex(key); }//キーに対応するインデックスを取得
		inline size_type bucket(const value_type& value) const { return _findIndex(value); }//キーに対応するインデックスを取得
		inline size_type bucket_size(const index_type index) const { return m_using[index] && !m_deleted[index] ? 1 : 0; }//特定バケット内の要素数を取得
	public:
		//メソッド：基本情報系（拡張）
		inline size_type getOriginalTableSize() const { return ORIGINAL_TABLE_SIZE; }//指定されたテーブルサイズを取得
		inline size_type getTableSize() const { return TABLE_SIZE; }//（実際の）テーブルサイズを取得
		inline size_type getTableSizeExtended() const { return TABLE_SIZE_EXTENDED; }//指定のテーブルサイズからの増分を取得
		inline size_type getAutoRehashRatio() const { return AUTO_REHASH_RATIO; }//自動リハッシュ実行の基準割合
		inline size_type getAutoRehashSize() const { return AUTO_REHASH_SIZE; }//自動リハッシュ実行の基準サイズ
		inline int getFindingCycleLimit() const { return FINDING_CYCLE_LIMIT; }//検索時の巡回回数の制限を取得
		inline key_type getKeyMin() const { return KEY_MIN; }//キーの最小値を取得
		inline key_type getKeyMax() const { return KEY_MAX; }//キーの最大値を取得
		inline key_type getKeyRange() const { return KEY_RANGE; }//キーの範囲を取得
		inline index_type getIndexStepBase() const { return INDEX_STEP_BASE; }//検索巡回時のインデックスの歩幅の基準値（実際のステップ数は、この値を係数にキーから算出する）
		inline int getUsingCount() const { return m_usingCount; }//使用中データ数を取得
		inline int getDeletedCount() const { return m_deletedCount; }//削除済みデータ数を取得
		inline int getMaxFindingCycle() const { return m_maxFindingCycle; }//検索時の最大巡回回数を取得
		int getNotOptimizedCount() const//最適化されていないデータ件数を取得 ※検索の際に2回以上の巡回が必要なデータ = 本来のインデックスと実際のインデックスが異なるデータ
		{
			int count = 0;
			for (index_type index = 0; index < TABLE_SIZE; ++index)
			{
				if (!m_using[index] || m_deleted[index])
					continue;
				if (index != calcIndex(m_keyTable[index]))
					++count;
			}
			return count;
		}
	public:
		//メソッド：インデックス計算
		inline index_type calcIndexStep(const key_type key) const { return INDEX_STEP_BASE - key % INDEX_STEP_BASE; }//キーからインデックスの歩幅（第二ハッシュ）を計算
		inline index_type calcIndex(const key_type key) const { return calcIndexImpl<(TABLE_SIZE >= KEY_RANGE && KEY_RANGE > 0), size_type, index_type, key_type, TABLE_SIZE, KEY_MIN, KEY_RANGE >::calc(key); }//キーからインデックス（第一ハッシュ）を計算
		inline index_type calcNextIndex(const key_type key, const index_type index) const { return (index + calcIndexStep(key)) % TABLE_SIZE; }//次のインデックスを計算（指定のインデックスに歩幅を加算）
	public:
		//メソッド：インデックスを取得
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロック全体の前後で共有ロック（リードロック）の取得と解放を行う必要がある
		index_type getFirstIndex() const
		{
			if (m_usingCount == 0 || m_deletedCount == m_usingCount)
				return INVALID_INDEX;
			for (index_type index = 0; index < TABLE_SIZE; ++index)
			{
				if (m_using[index])
					return index;
			}
			return INVALID_INDEX;
		}
		index_type getLastIndex() const
		{
			if (m_usingCount == 0 || m_deletedCount == m_usingCount)
				return INVALID_INDEX;
			for (index_type index = TABLE_SIZE; index > 0; --index)
			{
				if (m_using[index - 1])
					return index - 1;
			}
			return INVALID_INDEX;
		}
		index_type getNextIndex(const index_type index) const
		{
			index_type next_index = index == INVALID_INDEX ? 0 : index + 1;
			if (index < 0 || index >= TABLE_SIZE - 1 || m_usingCount == 0 || m_deletedCount == m_usingCount)
				return INVALID_INDEX;
			for (; next_index < TABLE_SIZE; ++next_index)
			{
				if (m_using[next_index])
					return next_index;
			}
			return INVALID_INDEX;
		}
		index_type getPrevIndex(const index_type index) const
		{
			index_type now_index = index == INVALID_INDEX ? TABLE_SIZE : index;
			if (index <= 0 || index >= TABLE_SIZE || m_usingCount == 0 || m_deletedCount == m_usingCount)
				return INVALID_INDEX;
			for (; now_index > 0; --now_index)
			{
				if (m_using[now_index - 1])
					return now_index - 1;
			}
			return INVALID_INDEX;
		}
	private:
		//キーで検索してインデックスを取得（共通）
		index_type _findIndexCommon(const key_type key) const
		{
			if (m_usingCount == 0 || m_deletedCount == m_usingCount)
				return INVALID_INDEX;
			const index_type index_first = calcIndex(key);//キーからインデックス（ハッシュ）を計算
			index_type index = index_first;
			do
			{
				if (!m_using[index])//未使用インデックスなら検索失敗
					break;
				if (!m_deleted[index] && m_keyTable[index] == key)//キーが一致するインデックスなら検索成功
					return index;
				index = calcNextIndex(key, index);//次のインデックスへ
			} while (index != index_first);//最初のインデックスに戻ったら終了（検索失敗）
			return INVALID_INDEX;
		}
		//キーで検索してインデックスを取得
		inline index_type _findIndex(const key_type key) const{ return _findIndexCommon(key); }
		inline index_type _findIndex(const char* key) const{ return _findIndexCommon(calcCRC32(key)); }
		inline index_type _findIndex(const std::string& key) const{ return _findIndexCommon(key.c_str()); }
		inline index_type _findIndex(const value_type& value) const{ return _findIndexCommon(ope_type::getKey(value)); }
	private:
		//キーで検索して値を取得（本体）
		inline const value_type* _findValue(const key_type key) const
		{
			const index_type index = _findIndex(key);//検索してインデックスを取得
			if (index == INVALID_INDEX)
				return nullptr;
			return reinterpret_cast<const value_type*>(&m_table[index]);
		}
	public:
		//キーで検索して値を取得
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロック全体の前後で共有ロック（リードロック）の取得と解放を行う必要がある
		inline const value_type* findValue(const key_type key) const { return _findValue(key); }
		inline const value_type* findValue(const char* key) const { return _findValue(calcCRC32(key)); }
		inline const value_type* findValue(const std::string& key) const { return _findValue(key.c_str()); }
		inline const value_type* findValue(const value_type& value) const { return _findValue(ope_type::getKey(value)); }
		inline value_type* findValue(const key_type key){ return const_cast<value_type*>(_findValue(key)); }
		inline value_type* findValue(const char* key){ return const_cast<value_type*>(_findValue(calcCRC32(key))); }
		inline value_type* findValue(const std::string& key){ return const_cast<value_type*>(_findValue(key.c_str())); }
		inline value_type* findValue(const value_type& value){ return const_cast<value_type*>(_findValue(ope_type::getKey(value))); }
	public:
		//キーで検索してイテレータを取得
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロック全体の前後で共有ロック（リードロック）の取得と解放を行う必要がある
		const iterator find(const key_type key) const
		{
			const index_type index = _findIndex(key);
			if (index == INVALID_INDEX)
				return iterator(*this, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false);
			return iterator(*this, index, m_keyTable[index], reinterpret_cast<const value_type*>(m_table[index]), m_deleted[index]);
		}
		inline const iterator find(const char* key) const { return find(calcCRC32(key)); }
		inline const iterator find(const std::string& key) const { return find(key.c_str()); }
		inline const iterator find(const value_type& value) const { return find(ope_type::getKey(value)); }
		inline iterator find(const key_type key){ return const_cast<const container*>(this)->find(key); }
		inline iterator find(const char* key){ return const_cast<const container*>(this)->find(key); }
		inline iterator find(const std::string& key){ return const_cast<const container*>(this)->find(key); }
		inline iterator find(const value_type value){ return const_cast<const container*>(this)->find(value); }
	private:
		//キー割り当て（本体）
		//※割り当てた配列要素（データテーブル）のポインタを返す
		value_type* _assign(const key_type key)
		{
			if (m_usingCount == TABLE_SIZE && m_deletedCount == 0)
				return nullptr;
			index_type index = _findIndexCommon(key);
			if (ope_type::REPLACE_ATTR == ope_type::NEVER_REPLACE && index != INVALID_INDEX)//同じキーが既に割り当て済みなら割り当て失敗
				return nullptr;
			int find_cycle = 0;
			if (index != INVALID_INDEX)
			{
				//置換
				_eraseByIndex(index);//デストラクタの呼び出しあり
			}
			else
			{
				//新規登録
				const index_type index_first = calcIndex(key);//キーからインデックス（ハッシュ）を計算
				index = index_first;
				do
				{
					++find_cycle;
					if (!m_using[index] || m_deleted[index])//未使用／削除済みインデックスなら割り当て成功
						break;
					if (FINDING_CYCLE_LIMIT > 0 && find_cycle == FINDING_CYCLE_LIMIT)//巡回回数が制限に達したら割り当て失敗
						return nullptr;
					index = calcNextIndex(key, index);//次のインデックスへ
				} while (index != index_first);//最初のインデックスに戻ったら終了（割り当て失敗）
			}
			m_keyTable[index] = key;//キーテーブルにキー登録
			if (!m_using[index])//未使用インデックスの割り当てなら使用中数を調整
			{
				m_using[index] = true;//使用中フラグをセット
				++m_usingCount;//使用中数をカウントアップ
			}
			if (m_deleted[index])//削除済みインデックスの再割り当てなら削除数を調整
			{
				m_deleted[index] = false;//削除済みフラグをリセット
				--m_deletedCount;//削除済み数をカウントダウン
			}
			m_maxFindingCycle = m_maxFindingCycle >= find_cycle ? m_maxFindingCycle : find_cycle;//最大巡回回数を更新
			return reinterpret_cast<value_type*>(&m_table[index]);
		}
	public:
		//キー割り当て
		//※割り当てた配列要素（データテーブル）のポインタを返す
		//※処理中、排他ロック（ライトロック）を取得する
		inline value_type* assign(const key_type key)
		{
			lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
			return _assign(key);
		}
		inline value_type* assign(const char* key){ return assign(calcCRC32(key)); }
		inline value_type* assign(const std::string& key){ return assign(key.c_str()); }
		inline value_type* assign(const value_type& value){ return assign(ope_type::getKey(value)); }
	private:
		//キー割り当てして値を挿入（コピー）（本体）
		//※オブジェクトのコピーが発生する点に注意
		value_type* _insert(const key_type key, const value_type& value)
		{
			value_type* assigned_value = _assign(key);
			if (!assigned_value)
				return nullptr;
			*assigned_value = value;
			return assigned_value;
		}
	public:
		//キー割り当てして値を挿入（コピー）
		//※オブジェクトのコピーが発生する点に注意
		//※処理中、排他ロック（ライトロック）を取得する
		inline value_type* insert(const key_type key, const value_type& value)
		{
			lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
			return _insert(key, value);
		}
		inline value_type* insert(const char* key, const value_type& value){ return insert(calcCRC32(key), value); }
		inline value_type* insert(const std::string& key, const value_type& value){ return insert(key.c_str(), value); }
		//値を挿入（コピー）し、キーは自動割り当て
		//※操作用クラス baseOpe_t の派生クラスで、getKey() を実装する必要あり
		//※オブジェクトのコピーが発生する点に注意
		inline value_type* insertAuto(const value_type& value){ return insert(ope_type::getKey(value), value); }
	private:
		//キー割り当てして値を初期化（本体）
		//※コンストラクタが呼び出される
		template<typename... Tx>
		value_type* _emplace(const key_type key, Tx... args)//const Tx&... args とも書ける
		{
			value_type* assigned_value = _assign(key);
			if (!assigned_value)
				return nullptr;
			assigned_value = new(assigned_value) value_type(args...);//コンストラクタ呼び出し
			return assigned_value;
		}
	public:
		//キー割り当てして値を初期化
		//※コンストラクタが呼び出される
		//※処理中、排他ロック（ライトロック）を取得する
		template<typename... Tx>
		inline value_type* emplace(const key_type key, Tx... args)//const Tx&... args とも書ける
		{
			lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
			return _emplace(key, args...);
		}
		template<typename... Tx>
		inline value_type* emplace(const char* key, Tx... args){ return emplace(calcCRC32(key), args...); }
		template<typename... Tx>
		inline value_type* emplace(const std::string& key, Tx... args){ return emplace(key.c_str(), args...); }
		//値を初期化して自動的にキー割り当て
		//※オブジェクトのコピーが発生する点に注意
		//※操作用クラス baseOpe_t の派生クラスで、getKey() を実装する必要あり
		//※処理中、ライトロックを取得する
		template<typename... Tx>
		inline value_type* emplaceAuto(Tx... args)
		{
			value_type value(args...);
			return insertAuto(value);
		}
	private:
		//インデックスを指定して削除
		void _eraseByIndex(const index_type index)
		{
			value_type* data_p = reinterpret_cast<value_type*>(&m_table[index]);
			ope_type::callDestructor(data_p);//デストラクタ呼び出し
			operator delete(data_p, data_p);//（作法として）deleteオペレータ呼び出し
			m_deleted[index] = true;//削除済みインデックスを更新
			++m_deletedCount;//削除済み数をカウントアップ
		}
		//キーを削除（本体）
		bool _erase(const key_type key)
		{
			const index_type index = _findIndex(key);//検索してインデックスを取得
			if (index == INVALID_INDEX)//検索失敗なら削除失敗
				return false;
			_eraseByIndex(index);
			if (m_usingCount == m_deletedCount || m_deletedCount == AUTO_REHASH_SIZE)//自動リハッシュ
				_rehash();
			return true;
		}
	public:
		//キーを削除
		//※処理中、排他ロック（ライトロック）を取得する
		inline bool erase(const key_type key)
		{
			lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
			return _erase(key);
		}
		inline bool erase(const char* key){ return erase(calcCRC32(key)); }
		inline bool erase(const std::string& key){ return erase(key.c_str()); }
		//キーを削除
		//※操作用クラス baseOpe_t の派生クラスで、getKey() を実装する必要あり
		inline bool eraseAuto(const value_type& value){ return erase(ope_type::getKey(value)); }
	private:
		//リハッシュ（本体）
		bool _rehash()
		{
			if (m_usingCount == 0 || m_deletedCount == 0)
				return false;
			if (m_usingCount == m_deletedCount)
			{
				_clear();
				return true;
			}
			m_maxFindingCycle = 1;//最大巡回回数を1にリセット
			//値の移動
			for (index_type index = 0; index < TABLE_SIZE; ++index)
			{
				if (!m_using[index] || m_deleted[index])//未使用インデックスまたは削除済みインデックスは処理をスキップ
					continue;
				const key_type key = m_keyTable[index];//キーを取得
				if (calcIndex(key) == index)//キーが本来のインデックスと異なる場合、移動を試みる
					continue;
				value_type* value = reinterpret_cast<value_type*>(m_table[index]);//現在の値を記憶
				m_deleted[index] = true;//一旦削除済みにする
				++m_deletedCount;       //（同上）
				value_type* value_new = _assign(key);//改めてキーを割り当て
				if (value_new != value)//インデックスが違っていたなら値を移動
				{
				#if 1
					*value_new = std::move(*value);//ムーブ演算子で移動
				#else
					memcpy(value_new, value, sizeof(value_type));//memcpyで移動
				#endif
				}
			}
			//削除済みインデックスのクリア
			for (index_type index = 0; index < TABLE_SIZE; ++index)
			{
				if (!m_using[index] || !m_deleted[index])//未使用インデックスまたは未削除インデックスは処理をスキップ
					continue;
				m_deleted[index] = false;//削除済みを解消する
				--m_deletedCount;       //（同上）
				m_using[index] = false;//使用中を解消する
				--m_usingCount;        //（同上）
			}
			return true;
		}
	public:
		//リハッシュ
		//※テーブルを拡大・再構築するのではなく、削除済みデータを完全に削除するだけ。
		//　そのために、削除済みデータの位置に移動可能なデータを移動する。
		//※処理中、排他ロック（ライトロック）を取得する
		inline bool rehash()
		{
			lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
			return _rehash();
		}
	private:
		//クリア（本体）
		void _clear()
		{
			for (index_type index = 0; index < TABLE_SIZE; ++index)
			{
				if (m_using[index] && !m_deleted[index])//使用中データはデストラクタ呼び出し
					_eraseByIndex(index);
			}
			m_using.reset();
			m_deleted.reset();
			m_usingCount = 0;
			m_deletedCount = 0;
			m_maxFindingCycle = 0;
		}
	public:
		//クリア
		//※処理中、排他ロック（ライトロック）を取得する
		inline void clear()
		{
			lock_guard<lock_type> lock(m_lock);//排他ロック（ライトロック）取得（関数を抜ける時に自動開放）
			_clear();
		}
	public:
		//コンストラクタ
		container() :
			m_using(),
			m_deleted(),
			m_usingCount(0),
			m_deletedCount(0),
			m_maxFindingCycle(0)
		{}
		//デストラクタ
		~container()
		{}
	private:
		unsigned char m_table[TABLE_SIZE][sizeof(value_type)];//データテーブル（バケット）
		key_type m_keyTable[TABLE_SIZE];//キーテーブル
		std::bitset<TABLE_SIZE> m_using;//キー設定済みフラグ ※登録を削除してもfalseにならない（リハッシュ時には調整される）
		std::bitset<TABLE_SIZE> m_deleted;//削除済みフラグ
		int m_usingCount;//使用中データ数 ※登録を削除しても減らない（リハッシュ時には調整される）
		int m_deletedCount;//削除済みデータ数
		int m_maxFindingCycle;//検索時の最大巡回回数 ※登録を削除しても減らない（リハッシュ時には調整される）
		mutable lock_type m_lock;//ロックオブジェクト
	};
	
	//--------------------
	//基本型定義マクロ消去
	#undef DECLARE_OPE_TYPES
}//namespace hash_table

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__HASH_TABLE_H_

// End of file

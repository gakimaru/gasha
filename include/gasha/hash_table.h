#pragma once
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

#include <cstddef>//std::size_t, std::ptrdiff_t用
//#include <cstdint>//std::intptr_t用
#include <gasha/sort_basic.h>//ソート処理基本
#include <gasha/search_basic.h>//探索処理基本

//【VC++】例外を無効化した状態で <iterator> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <iterator>//std::iterator用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ハッシュテーブル
//--------------------------------------------------------------------------------
//データ構造とアルゴリズム
//【特徴】
//・ダブルハッシュアルゴリズムを採用。
//　  - ハッシュ① ... 最初のインデックス。キーに応じて生成。
//　  - ハッシュ② ... 衝突の際のインデックスの歩幅。キーに応じて生成。
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//【本プログラムにおける実装要件】
//・固定配列で実装し、一切メモリ確保・削除をしない。
//・キー重複を許容しないアルゴリズムとする。
//・文字列（std::string/char*）のキーをサポートしない。
//・代わりに、文字列キーは自動的にcrc32変換を行う。（文字列は保持しない）
//・処理効率化のために、データの削除時は実際に削除せず、
//　削除済みフラグを用いるものとする。
//　※検索処理は、削除済みのデータを検出しても打ち切らないことで効率化する。
//　※このため、頻繁に登録／削除を繰り返すと、未登録データ検索時の
//　　速度が劣化する可能性がある。
//・STL（std::unodered_map）との違いは下記の通り
//    - 固定長配列である。（STLは自動拡張する）
//    - 衝突時は開番地方を用いて処理し、メモリ操作を行わない。
//        ※STLは（おそらく）連鎖法。STLの方が速いが、より多くのメモリを必要とする。
//    - リハッシュは削除済みデータを掃除するのみ。
//    - キーと値のペア（std::pair）で扱わず、基本的にキーと値を直接扱う。
//      その代わり、イテレータにはインデックスやキーなどの情報を含む。
//　　- insert/erase時のイテレータ指定に対応しない。
//    - 赤黒木コンテナ（rb_tree）の実装と合わせた構造にしており、
//　　  操作用テンプレート構造体を用いる。
//--------------------------------------------------------------------------------

//#include <bitset>//std::bitset用
//#include <cstddef>//srd::size_t用
//#include <iterator>//std::iterator用
//#include <utility>//std::move用
//#include <string>//std::string用

#ifdef USE_GCC
//#include "../StaticCRC32_unix/constexpr.h"
#else//USE_GCC
//#include "../../StaticCRC32/StaticCRC32/src/constexpr/constexpr.h"
#endif//USE_GCC

namespace hash_table
{
#if 0
	//--------------------
	//ハッシュテーブルデータ操作用テンプレート構造体
	//※CRTPを活用し、下記のような派生構造体を作成して使用する
	//  //template<class OPE_TYPE, typename KEY_TYPE, typename VALUE_TYPE, KEY_TYPE _KEY_MIN = 0u, KEY_TYPE _KEY_MAX = 0xffffffffu, KEY_TYPE _INVALID_KEY = 0xffffffffu>
	//  //struct baseOpe_t;
	//  //struct 派生構造体名 : public hash_table::baseOpe_t<派生構造体, キー型, 値型, キーの最小値= 0u, キーの最大値 = 0xffffffffu, 不正なキー = 0xffffffffu>
	//	struct ope_t : public hash_table::baseOpe_t<ope_t, crc32_t, data_t, 500>
	//	{
	//		//データ置換属性 ※必要に応じて定義
	//		static const replace_attr_t REPLACE_ATTR = REPLACE;//キーが重複するデータは置換して登録する
	//
	//		//キーを取得 ※必要に応じて定義
	//		inline static key_type getKey(const value_type& value){ return ???; }
	//		
	//		//ロック型 ※必要に応じて定義
	//		//※共有ロック（リード・ライトロック）でコンテナ操作をスレッドセーフにしたい場合は、
	//		//　有効な共有ロック型（shared_spin_lockなど）を lock_type 型として定義する。
	//		typedef shared_spin_lock lock_type;//ロックオブジェクト型
	//	};
	template<class OPE_TYPE, typename KEY_TYPE, typename VALUE_TYPE, KEY_TYPE _KEY_MIN = 0u, KEY_TYPE _KEY_MAX = 0xffffffffu, KEY_TYPE _INVALID_KEY = 0xffffffffu>
	struct baseOpe_t
	{
		//定数
		static const KEY_TYPE KEY_MIN = _KEY_MIN;//キーの最小値
		static const KEY_TYPE KEY_MAX = _KEY_MAX;//キーの最大値
		static const KEY_TYPE INVALID_KEY = _INVALID_KEY;//不正なキー
		
		//データ置換属性
		enum replace_attr_t
		{
			NEVER_REPLACE,//キーが重複するデータは登録できない（置換しない）
			REPLACE,//キーが重複するデータは置換して登録する
		};
		static const replace_attr_t REPLACE_ATTR = NEVER_REPLACE;//キーが重複するデータは登録できない（置換しない）

		//型
		typedef OPE_TYPE ope_type;//データ操作型
		typedef VALUE_TYPE value_type;//値型
		typedef KEY_TYPE key_type;//キー型

		//ロック型
		typedef dummy_shared_lock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※共有ロック（リード・ライトロック）でコンテナ操作をスレッドセーフにしたい場合は、
		//　baseOpe_tの派生クラスにて、有効な共有ロック型（shared_spin_lock など）を
		//　lock_type 型として再定義する。
		//【補足①】コンテナには、あらかじめロック制御のための仕組みがソースコードレベルで
		//　　　　　仕込んであるが、有効な型を与えない限りは、実行時のオーバーヘッドは一切ない。
		//【補足②】スレッドセーフ化した場合、書き込み時の排他ロックは行われるようになるが、
		//　　　　　読み込み時の共有ロックは行っていない。読み込み時のロックは局所的なロックで
		//　　　　　済まないため、ユーザーが任意に対応しなければならない。
		//　　　　　（例）
		//　　　　　    {
		//　　　　　        shared_lock_guard lock(container);//コンテナオブジェクトを渡して共有ロック
		//　　　　　                                          //※コンテナオブジェクトはロック
		//　　　　　                                          //　オブジェクト（lock_type）として振る舞える
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
		typedef std::size_t size_type; \
		typedef std::size_t index_type; \
		typedef typename ope_type::lock_type lock_type;
	//----------------------------------------
	//ハッシュテーブルコンテナ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, std::size_t _AUTO_REHASH_RATIO = 25, int _FINDING_CYCLE_LIMIT = 0, std::size_t _INDEX_STEP_BASE = 5>
	class container
	{
	public:
		//型
		DECLARE_OPE_TYPES(OPE_TYPE);
	public:
		//定数
		static const size_type ORIGINAL_TABLE_SIZE = _TABLE_SIZE;//テーブルサイズ（元々指定されたサイズ）
		static const size_type TABLE_SIZE = makeStaticPrimeGE<ORIGINAL_TABLE_SIZE>::value;//テーブルサイズ（指定サイズと同じか、それより大きい素数）
		static const size_type TABLE_SIZE_EXTENDED = TABLE_SIZE - ORIGINAL_TABLE_SIZE;//指定サイズから拡張したサイズ
		static const size_type  AUTO_REHASH_RATIO = _AUTO_REHASH_RATIO;//自動リハッシュ実行の基準割合 ※削除済み件数が全体サイズの一定割合以上になったら自動リハッシュ ※0で自動リハッシュなし
		static const size_type  AUTO_REHASH_SIZE = AUTO_REHASH_RATIO == 0 ? 0 : TABLE_SIZE * AUTO_REHASH_RATIO / 100;//自動リハッシュ実行の基準サイズ ※割合とテーブルサイズから計算
		static const int FINDING_CYCLE_LIMIT = _FINDING_CYCLE_LIMIT;//検索時の巡回回数の制限 ※0で無制限
		static const key_type KEY_MIN = ope_type::KEY_MIN;//キーの最小値
		static const key_type KEY_MAX = ope_type::KEY_MAX;//キーの最大値
		static const index_type INDEX_STEP_BASE = _INDEX_STEP_BASE;//検索巡回時のインデックスのス歩幅の基準値 ※必ず素数でなければならない
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
		static_assert(isStaticPrime<INDEX_STEP_BASE>::value == true, "hash_table::container: INDEX_STEP_BASE is required prime.");
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
			inline const value_type& operator*() const { return m_value; }
			inline value_type& operator*(){ return m_value; }
			inline const value_type* operator->() const { return m_value; }
			inline value_type* operator->(){ return m_value; }

			//ムーブオペレータ
			inline set& operator=(const set&& rhs)
			{
				m_index = rhs.m_index;
				m_primaryIndex = rhs.m_primaryIndex;
				m_key = rhs.m_key;
				m_value = rhs.m_value;
				m_isDeleted = rhs.m_isDeleted;
				return *this;
			}
			//コピーオペレータ
			inline set& operator=(const set& rhs)
			{
				//return operator=(std::move(rhs));
				m_index = rhs.m_index;
				m_primaryIndex = rhs.m_primaryIndex;
				m_key = rhs.m_key;
				m_value = rhs.m_value;
				m_isDeleted = rhs.m_isDeleted;
				return *this;
			}
			
			//メソッド
			inline bool isPrimaryIndex() const { return m_index == m_primaryIndex; }//実際のインデックスは、本来のインデックスと一致するか？
			inline void update(const index_type index, const index_type primary_index, const key_type key, const value_type* value, const bool is_deleted)
			{
				m_index = index;
				m_primaryIndex = primary_index;
				m_key = key;
				m_value = const_cast<value_type*>(value);
				m_isDeleted = is_deleted;
			}

			//ムーブコンストラクタ
			inline set(const set&& obj) :
				m_index(obj.m_index),
				m_primaryIndex(obj.m_primaryIndex),
				m_key(obj.m_key),
				m_value(obj.m_value),
				m_isDeleted(obj.m_isDeleted)
			{}
			//コピーコンストラクタ
			inline set(const set& obj) :
				m_index(obj.m_index),
				m_primaryIndex(obj.m_primaryIndex),
				m_key(obj.m_key),
				m_value(obj.m_value),
				m_isDeleted(obj.m_isDeleted)
			{}
			//コストラクタ
			inline set(const index_type index, const index_type primary_index, const key_type key, const value_type* value, const bool is_deleted) :
				m_index(index),
				m_primaryIndex(primary_index),
				m_key(key),
				m_value(const_cast<value_type*>(value)),
				m_isDeleted(is_deleted)
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
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const set&() const { return getSet(); }
			inline operator set&(){ return getSet(); }
			inline operator const value_type&() const { return *getValue(); }
			inline operator value_type&(){ return *getValue(); }
			inline operator const value_type*() const { return getValue(); }
			inline operator value_type*(){ return getValue(); }
			inline operator key_type() const { return getKey(); }
		public:
			//オペレータ
			inline const set& operator*() const { return getSet(); }
			inline set& operator*(){ return getSet(); }
			inline const_pointer operator->() const { return getValue(); }
			inline pointer operator->(){ return getValue(); }
		#if 1//std::forward_iterator_tag には本来必要ではない
			inline const_iterator operator[](const int index) const
			{
				iterator ite(*m_con, false);
				ite += index;
				return std::move(ite);
			}
			inline iterator operator[](const int index)
			{
				iterator ite(*m_con, false);
				ite += index;
				return std::move(ite);
			}
		#endif
			//比較オペレータ
			inline bool operator==(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_isEnd && rhs.m_isEnd ? true :
				       m_isEnd || rhs.m_isEnd ? false :
				       m_set.m_index == rhs.index;
			}
			inline bool operator!=(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_isEnd && rhs.m_isEnd ? false :
				       m_isEnd || rhs.m_isEnd ? true :
				       m_set.m_index != rhs.m_set.m_index;
			}
		#if 1//std::forward_iterator_tag には本来必要ではない
			inline bool operator>(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_isEnd && !rhs.m_isEnd ? true :
				       m_isEnd || rhs.m_isEnd ? false :
				       m_set.m_index > rhs.m_set.m_index;
			}
			inline bool operator>=(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_isEnd && rhs.m_isEnd ? true :
				       m_isEnd && !rhs.m_isEnd ? true :
				       m_isEnd || rhs.m_isEnd ? false :
				       m_set.m_index >= rhs.m_set.m_index;
			}
			inline bool operator<(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       !m_isEnd && rhs.m_isEnd ? true :
				       m_isEnd || rhs.m_isEnd ? false :
				       m_set.m_index < rhs.m_set.m_index;
			}
			inline bool operator<=(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_isEnd && rhs.m_isEnd ? true :
				       !m_isEnd && rhs.m_isEnd ? true :
				       m_isEnd || rhs.m_isEnd ? false :
				       m_set.m_index <= rhs.m_set.m_index;
			}
		#endif
			//演算オペレータ
			inline const_iterator& operator++() const
			{
				updateNext();
				return *this;
			}
		#if 1//std::forward_iterator_tag には本来必要ではない

			inline const_iterator& operator--() const
			{
				updatePrev();
				return *this;
			}
		#endif
			inline iterator& operator++()
			{
				updateNext();
				return *this;
			}
		#if 1//std::forward_iterator_tag には本来必要ではない
			inline iterator& operator--()
			{
				updatePrev();
				return *this;
			}
		#endif
			inline const_iterator operator++(int) const
			{
				iterator ite(*this);
				++(*this);
				return std::move(ite);
			}
		#if 1//std::forward_iterator_tag には本来必要ではない
			inline const_iterator operator--(int) const
			{
				iterator ite(*this);
				--(*this);
				return std::move(ite);
			}
		#endif
			inline iterator operator++(int)
			{
				iterator ite(*this);
				++(*this);
				return std::move(ite);
			}
		#if 1//std::forward_iterator_tag には本来必要ではない
			inline iterator operator--(int)
			{
				iterator ite(*this);
				--(*this);
				return std::move(ite);
			}
		#endif
		#if 1//std::forward_iterator_tag には本来必要ではない
			inline const_iterator& operator+=(const typename iterator::difference_type rhs) const
			{
				updateForward(rhs);
				return *this;
			}
			inline const_iterator& operator+=(const std::size_t rhs) const
			{
				return operator+=(static_cast<typename iterator::difference_type>(rhs));
			}
			inline const_iterator& operator-=(const typename iterator::difference_type rhs) const
			{
				updateBackward(rhs);
				return *this;
			}
			inline const_iterator& operator-=(const std::size_t rhs) const
			{
				return operator-=(static_cast<typename iterator::difference_type>(rhs));
			}
			inline iterator& operator+=(const typename iterator::difference_type rhs)
			{
				updateForward(rhs);
				return *this;
			}
			inline iterator& operator+=(const std::size_t rhs)
			{
				return operator+=(static_cast<typename iterator::difference_type>(rhs));
			}
			inline iterator& operator-=(const typename iterator::difference_type rhs)
			{
				updateBackward(rhs);
				return *this;
			}
			inline iterator& operator-=(const std::size_t rhs)
			{
				return operator-=(static_cast<typename iterator::difference_type>(rhs));
			}
			inline const_iterator operator+(const typename iterator::difference_type rhs) const
			{
				iterator ite(*this);
				ite += rhs;
				return std::move(ite);
			}
			inline const_iterator operator+(const std::size_t rhs) const
			{
				return std::move(operator+(static_cast<typename iterator::difference_type>(rhs)));
			}
			inline const_iterator operator-(const typename iterator::difference_type rhs) const
			{
				iterator ite(*this);
				ite -= rhs;
				return std::move(ite);
			}
			inline const_iterator operator-(const std::size_t rhs) const
			{
				return std::move(operator-(static_cast<typename iterator::difference_type>(rhs)));
			}
			inline iterator operator+(const typename iterator::difference_type rhs)
			{
				iterator ite(*this);
				ite += rhs;
				return std::move(ite);
			}
			inline iterator operator+(const std::size_t rhs)
			{
				return std::move(operator+(static_cast<typename iterator::difference_type>(rhs)));
			}
			inline iterator operator-(const typename iterator::difference_type rhs)
			{
				iterator ite(*this);
				ite -= rhs;
				return std::move(ite);
			}
			inline iterator operator-(const std::size_t rhs)
			{
				return std::move(operator-(static_cast<typename iterator::difference_type>(rhs)));
			}
			//inline typename iterator::difference_type operator-(const iterator rhs)
			//{
			//	return ???;
			//}
		#endif
		public:
			//ムーブオペレータ
			inline iterator& operator=(const_iterator&& rhs)
			{
				m_con = rhs.m_con;
				m_set = std::move(rhs.m_set);
				m_isEnd = rhs.m_isEnd;
				return *this;
			}
		#if 1//std::forward_iterator_tag には本来必要ではない
			iterator& operator=(const_reverse_iterator&& rhs);
		#endif
			//コピーオペレータ
			inline iterator& operator=(const_iterator& rhs)
			{
				m_con = rhs.m_con;
				m_set = rhs.m_set;
				m_isEnd = rhs.m_isEnd;
				return *this;
			}
		#if 1//std::forward_iterator_tag には本来必要ではない
			iterator& operator=(const_reverse_iterator& rhs);
		#endif
		public:
			//アクセッサ
			inline bool isExist() const { return m_set.m_index != INVALID_INDEX; }
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const { return m_set.m_index != INVALID_INDEX || m_isEnd; }
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const { return m_isEnd; }//終端か？
			inline const set& getSet() const { return m_set; }//現在のセット
			inline set& getSet(){ return m_set; }//現在のセット
			inline index_type getIndex() const { return m_set.m_index; }//（実際の）インデックス
			inline index_type getPrimaryIndex() const { return m_set.m_primaryIndex; }//本来のインデックス
			inline key_type getKey() const { return m_set.m_key; }//現在のキー
			inline const value_type* getValue() const { return m_set.m_value; }//現在の値
			inline value_type* getValue(){ return m_set.m_value; }//現在の値
			inline bool isDeleted() const { return m_set.m_isDeleted; }//削除済み
			inline bool isPrimaryIndex() const { return m_set.isPrimaryIndex(); }//本来のインデックスか？
		private:
			//メソッド
			index_type update(const index_type index) const
			{
				//if (index == INVALID_INDEX || index < 0 || index >= static_cast<index_type>(m_con->m_size))
				if (index >= static_cast<index_type>(TABLE_SIZE))
					m_set.update(INVALID_INDEX, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false);
				else
					m_set.update(index, m_con->calcIndex(m_con->m_keyTable[index]), m_con->m_keyTable[index], reinterpret_cast<const value_type*>(m_con->m_table[index]), m_con->m_deleted[index]);
				return m_set.m_index;
			}
			inline void updateNext() const
			{
				const index_type prev_index = m_set.m_index;
				const index_type index = update(m_con->getNextIndex(prev_index));
				m_isEnd = (prev_index != INVALID_INDEX && index == INVALID_INDEX);
			}
			inline void updatePrev() const
			{
				if (m_isEnd)
				{
					update(m_con->getLastIndex());
					m_isEnd = false;
					return;
				}
				update(m_con->getPrevIndex(m_set.m_index));
				m_isEnd = false;
			}
			void updateForward(const int step) const
			{
				int _step = step;
				const index_type prev_index = m_set.m_index;
				index_type index = prev_index;
				while (_step > 0 && index != INVALID_INDEX)
				{
					index = m_con->getNextIndex(index);
					--_step;
				}
				update(index);
				m_isEnd = (index != INVALID_INDEX && index == INVALID_INDEX && _step == 0);
			}
			void updateBackward(const int step) const
			{
				int _step = step;
				index_type index = m_set.m_index;
				if (_step > 0 && m_isEnd)
				{
					index = m_con->getLastIndex();
					--_step;
				}
				while (_step > 0 && index != INVALID_INDEX)
				{
					index = m_con->getPrevIndex(index);
					--_step;
				}
				update(index);
				m_isEnd = false;
			}
		public:
			//ムーブコンストラクタ
			iterator(const_iterator&& obj) :
				m_con(obj.m_con),
				m_set(std::move(obj.m_set)),
				m_isEnd(obj.m_isEnd)
			{}
		#if 1//std::forward_iterator_tag には本来必要ではない
			iterator(const_reverse_iterator&& obj);
		#endif
			//コピーコンストラクタ
			inline iterator(const_iterator& obj) :
				m_con(obj.m_con),
				m_set(obj.m_set),
				m_isEnd(obj.m_isEnd)
			{}
		#if 1//std::forward_iterator_tag には本来必要ではない
			iterator(const_reverse_iterator& obj);
		#endif
			//コンストラクタ
			inline iterator(const container& con, const bool is_end) :
				m_con(&con),
				m_set(INVALID_INDEX, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false),
				m_isEnd(is_end)
			{
				if (!is_end)
				{
					update(m_con->getFirstIndex());//先頭インデックス
					if (!m_set.m_value)
						m_isEnd = true;
				}
			}
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
	#if 1//std::forward_iterator_tag には本来必要ではない
		//--------------------
		//リバースイテレータ
		//class reverse_iterator : public std::reverse_iterator<iterator>
		class reverse_iterator : public std::iterator<std::forward_iterator_tag, value_type>
		{
			friend class container;
			friend class iterator;
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const set&() const { return getSet(); }
			inline operator set&(){ return getSet(); }
			inline operator const value_type&() const { return *getValue(); }
			inline operator value_type&(){ return *getValue(); }
			inline operator const value_type*() const { return getValue(); }
			inline operator value_type*(){ return getValue(); }
			inline operator key_type() const { return getKey(); }
		public:
			//オペレータ
			inline const set& operator*() const { return getSet(); }
			inline set& operator*(){ return getSet(); }
			inline const_pointer operator->() const { return getValue(); }
			inline pointer operator->(){ return getValue(); }
			inline const_reverse_iterator operator[](const int index) const
			{
				reverse_iterator ite(*m_con, false);
				ite += index;
				return std::move(ite);
			}
			inline reverse_iterator operator[](const int index)
			{
				reverse_iterator ite(*m_con, false);
				ite += index;
				return std::move(ite);
			}
		public:
			//比較オペレータ
			inline bool operator==(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_isEnd && m_isEnd ? true :
				       rhs.m_isEnd || m_isEnd ? false :
				       rhs.m_set.m_index == m_set.m_index;
			}
			inline bool operator!=(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_isEnd && m_isEnd ? false :
				       rhs.m_isEnd || m_isEnd ? true :
				       rhs.m_set.m_index != m_set.m_index;
			}
			inline bool operator>(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_isEnd && !m_isEnd ? true :
				       rhs.m_isEnd || m_isEnd ? false :
				       rhs.m_set.m_index > m_set.m_index;
			}
			inline bool operator>=(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_isEnd && m_isEnd ? true :
				       rhs.m_isEnd && !m_isEnd ? true :
				       rhs.m_isEnd || m_isEnd ? false :
				       rhs.m_set.m_index >= m_set.m_index;
			}
			inline bool operator<(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       !rhs.m_isEnd && m_isEnd ? true :
				       rhs.m_isEnd || m_isEnd ? false :
				       rhs.m_set.m_index < m_set.m_index;
			}
			inline bool operator<=(const_reverse_iterator& rhs) const
			{
				return !isEnabled() || !isEnabled() ? false :
				       rhs.m_isEnd && m_isEnd ? true :
				       !rhs.m_isEnd && m_isEnd ? true :
				       rhs.m_isEnd || m_isEnd ? false :
				       rhs.m_set.m_index <= m_set.m_index;
			}
			//演算オペレータ
			inline const_reverse_iterator& operator++() const
			{
				updateNext();
				return *this;
			}
			inline const_reverse_iterator& operator--() const
			{
				updatePrev();
				return *this;
			}
			inline reverse_iterator& operator++()
			{
				updateNext();
				return *this;
			}
			inline reverse_iterator& operator--()
			{
				updatePrev();
				return *this;
			}
			inline const_reverse_iterator operator++(int) const
			{
				reverse_iterator ite(*this);
				++(*this);
				return std::move(ite);
			}
			inline const_reverse_iterator operator--(int) const
			{
				reverse_iterator ite(*this);
				--(*this);
				return std::move(ite);
			}
			inline reverse_iterator operator++(int)
			{
				reverse_iterator ite(*this);
				++(*this);
				return std::move(ite);
			}
			inline reverse_iterator operator--(int)
			{
				reverse_iterator ite(*this);
				--(*this);
				return std::move(ite);
			}
			inline const_reverse_iterator& operator+=(const typename reverse_iterator::difference_type rhs) const
			{
				updateForward(rhs);
				return *this;
			}
			inline const_reverse_iterator& operator+=(const std::size_t rhs) const
			{
				return operator+=(static_cast<typename reverse_iterator::difference_type>(rhs));
			}
			inline const_reverse_iterator& operator-=(const typename reverse_iterator::difference_type rhs) const
			{
				updateBackward(rhs);
				return *this;
			}
			inline const_reverse_iterator& operator-=(const std::size_t rhs) const
			{
				return operator-=(static_cast<typename reverse_iterator::difference_type>(rhs));
			}
			inline reverse_iterator& operator+=(const typename reverse_iterator::difference_type rhs)
			{
				updateForward(rhs);
				return *this;
			}
			inline reverse_iterator& operator+=(const std::size_t rhs)
			{
				return operator+=(static_cast<typename reverse_iterator::difference_type>(rhs));
			}
			inline reverse_iterator& operator-=(const typename reverse_iterator::difference_type rhs)
			{
				updateBackward(rhs);
				return *this;
			}
			inline reverse_iterator& operator-=(const std::size_t rhs)
			{
				return operator-=(static_cast<typename reverse_iterator::difference_type>(rhs));
			}
			inline const_reverse_iterator operator+(const typename reverse_iterator::difference_type rhs) const
			{
				reverse_iterator ite(*this);
				ite += rhs;
				return std::move(ite);
			}
			inline const_reverse_iterator operator+(const std::size_t rhs) const
			{
				return std::move(operator+(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			inline const_reverse_iterator operator-(const typename reverse_iterator::difference_type rhs) const
			{
				reverse_iterator ite(*this);
				ite -= rhs;
				return std::move(ite);
			}
			inline const_reverse_iterator operator-(const std::size_t rhs) const
			{
				return std::move(operator-(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			inline reverse_iterator operator+(const typename reverse_iterator::difference_type rhs)
			{
				reverse_iterator ite(*this);
				ite += rhs;
				return std::move(ite);
			}
			inline reverse_iterator operator+(const std::size_t rhs)
			{
				return std::move(operator+(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			inline reverse_iterator operator-(const typename reverse_iterator::difference_type rhs)
			{
				reverse_iterator ite(*this);
				ite -= rhs;
				return std::move(ite);
			}
			inline reverse_iterator operator-(const std::size_t rhs)
			{
				return std::move(operator-(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			//inline typename reverse_iterator::difference_type operator-(const reverse_iterator rhs)
			//{
			//	return ???;
			//}
		public:
			//ムーブオペレータ
			inline reverse_iterator& operator=(const_reverse_iterator&& rhs)
			{
				m_con = rhs.m_con;
				m_set = std::move(rhs.m_set);
				m_isEnd = rhs.m_isEnd;
				return *this;
			}
			inline reverse_iterator& operator=(const_iterator&& rhs)
			{
				m_con = rhs.m_con;
				m_set = std::move(rhs.m_set);
				m_isEnd = false;
				if (m_set.m_index != INVALID_INDEX)
					++(*this);
				else
				{
					if (rhs.m_isEnd)
						update(m_con->getLastIndex());//末尾インデックス
				}
				return *this;
			}
			//コピーオペレータ
			inline reverse_iterator& operator=(const_reverse_iterator& rhs)
			{
				m_con = rhs.m_con;
				m_set = rhs.m_set;
				m_isEnd = rhs.m_isEnd;
				return *this;
			}
			inline reverse_iterator& operator=(const_iterator& rhs)
			{
				m_con = rhs.m_con;
				m_set = rhs.m_set;
				m_isEnd = false;
				if (m_set.m_index != INVALID_INDEX)
					++(*this);
				else
				{
					if (rhs.m_isEnd)
						update(m_con->getLastIndex());//末尾インデックス
				}
				return *this;
			}
		public:
			//アクセッサ
			inline bool isExist() const { return m_set.m_index != INVALID_INDEX; }
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const { return m_set.m_index != INVALID_INDEX || m_isEnd; }
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const { return m_isEnd; }//終端か？
			inline const set& getSet() const { return m_set; }//現在のセット
			inline set& getSet(){ return m_set; }//現在のセット
			inline index_type getIndex() const { return m_set.m_index; }//（実際の）インデックス
			inline index_type getPrimaryIndex() const { return m_set.m_primaryIndex; }//本来のインデックス
			inline key_type getKey() const { return m_set.m_key; }//現在のキー
			inline const value_type* getValue() const { return m_set.m_value; }//現在の値
			inline value_type* getValue(){ return m_set.m_value; }//現在の値
			inline bool isDeleted() const { return m_set.m_isDeleted; }//削除済み
			inline bool isPrimaryIndex() const { return m_set.isPrimaryIndex(); }//本来のインデックスか？
		private:
			//メソッド
			index_type update(const index_type index) const
			{
				//if (index == INVALID_INDEX || index < 0 || index >= static_cast<index_type>(m_con->m_size))
				if (index >= static_cast<index_type>(TABLE_SIZE))
					m_set.update(INVALID_INDEX, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false);
				else
					m_set.update(index, m_con->calcIndex(m_con->m_keyTable[index]), m_con->m_keyTable[index], reinterpret_cast<const value_type*>(m_con->m_table[index]), m_con->m_deleted[index]);
				return m_set.m_index;
			}
			inline void updateNext() const
			{
				const index_type prev_index = m_set.m_index;
				const index_type index = update(m_con->getPrevIndex(prev_index));
				m_isEnd = (prev_index != INVALID_INDEX && index == INVALID_INDEX);
			}
			inline void updatePrev() const
			{
				if (m_isEnd)
				{
					update(m_con->getFirstIndex());
					m_isEnd = false;
					return;
				}
				update(m_con->getNextIndex(m_set.m_index));
				m_isEnd = false;
			}
			void updateForward(const int step) const
			{
				int _step = step;
				const index_type prev_index = m_set.m_index;
				index_type index = prev_index;
				while (_step > 0 && index != INVALID_INDEX)
				{
					index = m_con->getPrevIndex(index);
					--_step;
				}
				update(index);
				m_isEnd = (index != INVALID_INDEX && index == INVALID_INDEX && _step == 0);
			}
			void updateBackward(const int step) const
			{
				int _step = step;
				index_type index = m_set.m_index;
				if (_step > 0 && m_isEnd)
				{
					index = m_con->getFirstIndex();
					--_step;
				}
				while (_step > 0 && index != INVALID_INDEX)
				{
					index = m_con->getNextIndex(index);
					--_step;
				}
				update(index);
				m_isEnd = false;
			}
		public:
			//ベースを取得
			inline const_iterator base() const
			{
				iterator ite(*this);
				return std::move(ite);
			}
			inline iterator base()
			{
				iterator ite(*this);
				return std::move(ite);
			}
		public:
			//ムーブコンストラクタ
			inline reverse_iterator(const_reverse_iterator&& obj) :
				m_con(obj.m_con),
				m_set(std::move(obj.m_set)),
				m_isEnd(obj.m_isEnd)
			{}
			inline reverse_iterator(const_iterator&& obj) :
				m_con(obj.m_con),
				m_set(std::move(obj.m_set)),
				m_isEnd(false)
			{
				if (m_set.m_index != INVALID_INDEX)
					++(*this);
				else
				{
					if (obj.m_isEnd)
						update(m_con->getLastIndex());//末尾インデックス
				}
			}
			//コピーコンストラクタ
			inline reverse_iterator(const_reverse_iterator& obj) :
				m_con(obj.m_con),
				m_set(obj.m_set),
				m_isEnd(obj.m_isEnd)
			{}
			inline reverse_iterator(const_iterator& obj) :
				m_con(obj.m_con),
				m_set(obj.m_set),
				m_isEnd(false)
			{
				if (m_set.m_index != INVALID_INDEX)
					++(*this);
				else
				{
					if (obj.m_isEnd)
						update(m_con->getLastIndex());//末尾インデックス
				}
			}
			//コンストラクタ
			inline reverse_iterator(const container& con, const bool is_end) :
				m_con(&con),
				m_set(INVALID_INDEX, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false),
				m_isEnd(is_end)
			{
				if (!is_end)
				{
					update(m_con->getLastIndex());//末尾インデックス
					if (!m_set.m_value)
						m_isEnd = true;
				}
			}
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
	#endif
	public:
		//アクセッサ
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
		//検索系アクセッサ：キーで検索して値を返す
		//※マルチスレッドで処理する際は、一連の処理ブロック全体の前後で
		//　リードロックの取得を行うようにすること。
		value_type* at(const key_type key){ return findValue(key); }
		value_type* at(const char* key){ return findValue(key); }
		value_type* at(const std::string& key){ return findValue(key); }
		value_type* at(const value_type& value){ return findValue(value); }
		const value_type* at(const key_type key) const { return findValue(key); }
		const value_type* at(const char* key) const { return findValue(key); }
		const value_type* at(const std::string& key) const { return findValue(key); }
		const value_type* at(const value_type& value) const { return findValue(value); }
		value_type* operator[](const key_type key){ return findValue(key); }
		value_type* operator[](const char* key){ return findValue(key); }
		value_type* operator[](const std::string& key){ return findValue(key); }
		value_type* operator[](const value_type& value){ return findValue(value); }
		const value_type* operator[](const key_type key) const { return findValue(key); }
		const value_type* operator[](const char* key) const { return findValue(key); }
		const value_type* operator[](const std::string& key) const { return findValue(key); }
		const value_type* operator[](const value_type& value) const { return findValue(value); }
	public:
		//キャストオペレータ
		inline operator lock_type&(){ return m_lock; }//共有ロックオブジェクト
		inline operator lock_type&() const { return m_lock; }//共有ロックオブジェクト ※mutable
	public:
		//メソッド
		inline index_type calcIndexStep(const key_type key) const { return INDEX_STEP_BASE - key % INDEX_STEP_BASE; }//キーからインデックスの歩幅（第二ハッシュ）を計算
		inline index_type calcIndex(const key_type key) const { return calcIndexImpl<(TABLE_SIZE >= KEY_RANGE && KEY_RANGE > 0), size_type, index_type, key_type, TABLE_SIZE, KEY_MIN, KEY_RANGE >::calc(key); }//キーからインデックス（第一ハッシュ）を計算
		inline index_type calcNextIndex(const key_type key, const index_type index) const { return (index + calcIndexStep(key)) % TABLE_SIZE; }//次のインデックスを計算（指定のインデックスに歩幅を加算）
	public:
		//インデックスを取得
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
		//イテレータを取得
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロック全体の前後で共有ロック（リードロック）の取得と解放を行う必要がある
		inline const_iterator cbegin() const
		{
			iterator ite(*this, false);
			return std::move(ite);
		}
		inline const_iterator cend() const
		{
			iterator ite(*this, true);
			return std::move(ite);
		}
		inline const_iterator begin() const
		{
			iterator ite(*this, false);
			return std::move(ite);
		}
		inline const_iterator end() const
		{
			iterator ite(*this, true);
			return std::move(ite);
		}
		inline iterator begin()
		{
			iterator ite(*this, false);
			return std::move(ite);
		}
		inline iterator end()
		{
			iterator ite(*this, true);
			return std::move(ite);
		}
	#if 1//std::forward_iterator_tag には本来必要ではない
		//リバースイテレータを取得
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロック全体の前後で共有ロック（リードロック）の取得と解放を行う必要がある
		inline const_reverse_iterator crbegin() const
		{
			reverse_iterator ite(*this, false);
			return std::move(ite);
		}
		inline const_reverse_iterator crend() const
		{
			reverse_iterator ite(*this, true);
			return std::move(ite);
		}
		inline const_reverse_iterator rbegin() const
		{
			reverse_iterator ite(*this, false);
			return std::move(ite);
		}
		inline const_reverse_iterator rend() const
		{
			reverse_iterator ite(*this, true);
			return std::move(ite);
		}
		inline reverse_iterator rbegin()
		{
			reverse_iterator ite(*this, false);
			return std::move(ite);
		}
		inline reverse_iterator rend()
		{
			reverse_iterator ite(*this, true);
			return std::move(ite);
		}
	#endif
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
		//キーで検索してイテレータを取得
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロック全体の前後で共有ロック（リードロック）の取得と解放を行う必要がある
		const_iterator find(const key_type key) const
		{
			const index_type index = _findIndex(key);
			if (index == INVALID_INDEX)
				return iterator(*this, INVALID_INDEX, ope_type::INVALID_KEY, nullptr, false);
			return iterator(*this, index, m_keyTable[index], reinterpret_cast<const value_type*>(m_table[index]), m_deleted[index]);
		}
		inline const_iterator find(const char* key) const { return find(calcCRC32(key)); }
		inline const_iterator find(const std::string& key) const { return find(key.c_str()); }
		inline const_iterator find(const value_type& value) const { return find(ope_type::getKey(value)); }
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
#if 1//std::forward_iterator_tag には本来必要ではない
	//イテレータのムーブオペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, std::size_t _AUTO_REHASH_RATIO, int _FINDING_CYCLE_LIMIT, std::size_t _INDEX_STEP_BASE>
	//typename container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::iterator& container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::iterator::operator=(typename container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::const_reverse_iterator&& rhs)//GCCはOK, VC++はNG
	typename container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::iterator& container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::reverse_iterator&& rhs)//VC++もOK
	{
		m_con = rhs.m_con;
		m_set = std::move(rhs.m_set);
		m_isEnd = false;
		if (m_set.m_index != INVALID_INDEX)
			++(*this);
		else
		{
			if (rhs.m_isEnd)
				update(m_con->getFirstIndex());//先頭インデックス
		}
		return *this;
	}
	//イテレータのコピーオペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, std::size_t _AUTO_REHASH_RATIO, int _FINDING_CYCLE_LIMIT, std::size_t _INDEX_STEP_BASE>
	//typename container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::iterator& container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::iterator::operator=(typename container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::const_reverse_iterator& rhs)//GCCはOK, VC++はNG
	typename container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::iterator& container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::reverse_iterator& rhs)//VC++もOK
	{
		m_con = rhs.m_con;
		m_set = rhs.m_set;
		m_isEnd = false;
		if (m_set.m_index != INVALID_INDEX)
			++(*this);
		else
		{
			if (rhs.m_isEnd)
				update(m_con->getFirstIndex());//先頭インデックス
		}
		return *this;
	}
	//イテレータのムーブコンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, std::size_t _AUTO_REHASH_RATIO, int _FINDING_CYCLE_LIMIT, std::size_t _INDEX_STEP_BASE>
	//container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::iterator::iterator(typename container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::const_reverse_iterator&& obj) ://GCCはOK, VC++はNG
	container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::iterator::iterator(const typename container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::reverse_iterator&& obj) ://VC++もOK
		m_con(obj.m_con),
		m_set(std::move(obj.m_set)),
		m_isEnd(false)
	{
		if (m_set.m_index != INVALID_INDEX)
			++(*this);
		else
		{
			if (obj.m_isEnd)
				update(m_con->getFirstIndex());//先頭インデックス
		}
	}
	//イテレータのコピーコンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE, std::size_t _AUTO_REHASH_RATIO, int _FINDING_CYCLE_LIMIT, std::size_t _INDEX_STEP_BASE>
	//container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::iterator::iterator(typename container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::const_reverse_iterator& obj) ://GCCはOK, VC++はNG
	container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::iterator::iterator(const typename container<OPE_TYPE, _TABLE_SIZE, _AUTO_REHASH_RATIO, _FINDING_CYCLE_LIMIT, _INDEX_STEP_BASE>::reverse_iterator& obj) ://VC++もOK
		m_con(obj.m_con),
		m_set(obj.m_set),
		m_isEnd(false)
	{
		if (m_set.m_index != INVALID_INDEX)
			++(*this);
		else
		{
			if (obj.m_isEnd)
				update(m_con->getFirstIndex());//先頭インデックス
		}
	}
#endif
	//--------------------
	//基本型定義マクロ消去
	#undef DECLARE_OPE_TYPES
#endif
}//namespace hash_table

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__HASH_TABLE_H_

// End of file

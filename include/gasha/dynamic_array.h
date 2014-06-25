#pragma once
#ifndef __DYNAMIC_ARRAY_H_
#define __DYNAMIC_ARRAY_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// dynamic_array.h
// 動的配列コンテナ【宣言部】※データのメモリ管理を行わない擬似コンテナ
//
// ※コンテナをインスタンス化する際は、別途下記のファイルをインクルードする必要あり
//
//   ・dynamic_array.inl   ... 【インライン関数／テンプレート関数実装部】
//                             コンテナクラスの操作が必要な場所でインクルード。
//   ・dynamic_array.cpp.h ... 【関数実装部】
//                             コンテナクラスの実体化が必要な場所でインクルード。
//
// ※面倒なら三つまとめてインクルードして使用しても良いが、分けた方が、
// 　コンパイルへの影響やコンパイル速度を抑えることができる。
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

//例外を無効化した状態で <iterator> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <iterator>//std::iterator用

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//動的配列
//--------------------------------------------------------------------------------
//データ構造とアルゴリズム
//【特徴】
//・単純な一次配列である。
//・配列の要素数を超えない範囲で、有効要素数を動的に変化させて管理する。
//・有効要素の増減に伴い、コンストラクタ／デストラクタの呼び出しを行う。
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//【本プログラムにおける実装要件】
//・アルゴリズムとデータ本体を分離したコンテナとする。
//・コンテナ自体は要素の実体を持たずメモリ確保も行わない。
//・コンストラクタで受けとった配列の参照を扱う。
//・STL（std::vector）との違いは下記の通り
//    - 固定長配列である。
//    - 他のコンテナ（rb_tree）の実装と合わせた構造にしており、
//      操作用テンプレート構造体を用いる。
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//【具体的な活用の想定】
//・コンテナクラスと無関係の配列を、一時的にコンテナクラス化して操作する。
//--------------------------------------------------------------------------------

namespace dynamic_array
{
	//--------------------
	//動的配列操作用テンプレート構造体
	//※CRTPを活用し、下記のような派生構造体を作成して使用する
	//  //template<class OPE_TYPE, typename VALUE_TYPE>
	//  //struct base_ope_t;
	//  //struct 派生構造体名 : public dynamic_array::base_ope_t<派生構造体, 要素型>
	//	struct ope_t : public dynamic_array::base_ope_t<ope_t, data_t>
	//	{
	//		//ソート用プレディケート関数オブジェクト
	//		//※必要に応じて実装する
	//		struct sort_predicate{
	//			inline bool operator()(const value_type& lhs, const value_type& rhs) const { return lhs.??? < rhs.???; }
	//		};
	//
	//		//探索用プレディケート関数オブジェクト
	//		//※必要に応じて実装する
	//		struct find_predicate{
	//			inline bool operator()(const value_type& lhs, const ???& rhs) const { return lhs.??? == rhs; }
	//		};
	//		
	//		//探索用比較関数オブジェクト
	//		//※必要に応じて実装する
	//		struct search_comparison{
	//			inline int operator()(const value_type& lhs, const ???& rhs) const { return rhs - lhs.???; }
	//		};
	//		
	//		//ロック型 ※必要に応じて定義
	//		//※ロックでコンテナ操作をスレッドセーフにしたい場合は、
	//		//　有効なロック型（spin_lockなど）を lock_type 型として定義する。
	//		typedef sharedSpinLock lock_type;//ロックオブジェクト型
	//
	//		//デストラクタ呼び出し ※必要に応じて実装
	//		//※デストラクタ呼び出しを抑制したい場合などに定義する。
	//		inline static void callDestructor(value_type* obj)
	//		{
	//			//なにもしない
	//		}
	//	};
	template<class OPE_TYPE, typename VALUE_TYPE>
	struct base_ope_t
	{
		//型
		typedef OPE_TYPE ope_type;//要素操作型
		typedef VALUE_TYPE value_type;//要素型

		//ロック型
		typedef GASHA_ dummySharedLock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※共有ロック（リード・ライトロック）でコンテナ操作をスレッドセーフにしたい場合は、
		//　base_ope_tの派生クラスにて、有効なロック型（sharedSpinLock など）を
		//　lock_type 型として再定義する。

		//デストラクタ呼び出し
		inline static void callDestructor(value_type* obj){ obj->~VALUE_TYPE(); }
		//※デストラクタの呼び出しを禁止したい場合、
		//　base_ope_tの派生クラスにて、なにもしない
		//　callDestructor メソッドを再定義する。

		//ソート用プレディケート関数オブジェクト
		//※trueでlhsの方が小さい（並び順が正しい）
		struct sort_predicate{
			inline bool operator()(const value_type& lhs, const value_type& rhs) const { return GASHA_ less<value_type>()(lhs, rhs); }
		};

		//探索用プレディケート関数オブジェクト
		//※trueで一致（探索成功）
		struct find_predicate{
			template<typename V>
			inline bool operator()(const value_type& lhs, const V& rhs) const { return GASHA_ equal_to<value_type>()(lhs, rhs); }
		};

		//探索用比較関数オブジェクト
		//※0で一致（探索成功）、1以上でlhsの方が大きい、-1以下でrhsの方が大きい
		struct search_comparison{
			template<typename V>
			inline int operator()(const value_type& lhs, const V& rhs) const { return GASHA_ compare_to<value_type>()(lhs, rhs); }
		};
	};
	//--------------------
	//基本型定義マクロ
	#define DECLARE_OPE_TYPES(OPE_TYPE) \
		typedef OPE_TYPE ope_type; \
		typedef typename ope_type::value_type value_type; \
		typedef value_type& reference; \
		typedef const value_type& const_reference; \
		typedef value_type* pointer; \
		typedef const value_type* const_pointer; \
		typedef std::size_t size_type; \
		typedef std::size_t index_type; \
		typedef typename ope_type::lock_type lock_type;
	//----------------------------------------
	//コンテナ破棄時の要素の自動クリア属性
	enum auto_clear_attr_t
	{
		NEVER_CLEAR,//自動クリアしない（デフォルト）
		AUTO_CLEAR,//自動クリアし、残っている要素のデストラクタを呼び出す
	};
	//----------------------------------------
	//動的配列コンテナ
	template<class OPE_TYPE>
	class container
	{
	public:
		//型
		DECLARE_OPE_TYPES(OPE_TYPE);
	public:
		//定数
		static const index_type INVALID_INDEX = 0xffffffffu;//無効なインデックス
	public:
		//--------------------
		//イテレータ宣言
		class iterator;
		class reverse_iterator;
		typedef const iterator const_iterator;
		typedef const reverse_iterator const_reverse_iterator;
		//--------------------
		//イテレータ
		class iterator : public std::iterator<std::random_access_iterator_tag, value_type>
		{
			friend class container;
			friend class reverse_iterator;
		public:
			typedef typename container::reverse_iterator reverse_iterator;
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const value_type&() const { return *getValue(); }
			inline operator value_type&(){ return *getValue(); }
			inline operator const value_type*() const { return getValue(); }
			inline operator value_type*(){ return getValue(); }
		public:
			//オペレータ
			inline const value_type& operator*() const { return *getValue(); }
			inline value_type& operator*(){ return *getValue(); }
			inline const_pointer operator->() const { return getValue(); }
			inline pointer operator->(){ return getValue(); }
			inline const iterator operator[](const int index) const
			{
				iterator ite(*m_con, false);
				ite.update(index);
				return ite;
			}
			inline iterator operator[](const int index)
			{
				iterator ite(*m_con, false);
				ite.update(index);
				return ite;
			}
			//比較オペレータ
			inline bool operator==(const iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_index == rhs.m_index;
			}
			inline bool operator!=(const iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_index != rhs.m_index;
			}
			inline bool operator>(const iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_index > rhs.m_index;
			}
			inline bool operator>=(const iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_index >= rhs.m_index;
			}
			inline bool operator<(const iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_index < rhs.m_index;
			}
			inline bool operator<=(const iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_index <= rhs.m_index;
			}
			//演算オペレータ
			inline const iterator& operator++() const
			{
				addIndexAndUpdate(1);
				return *this;
			}
			inline const iterator& operator--() const
			{
				addIndexAndUpdate(-1);
				return *this;
			}
			inline iterator& operator++()
			{
				addIndexAndUpdate(1);
				return *this;
			}
			inline iterator& operator--()
			{
				addIndexAndUpdate(-1);
				return *this;
			}
			inline const iterator operator++(int) const
			{
				iterator ite(*this);
				++(*this);
				return ite;
			}
			inline const iterator operator--(int) const
			{
				iterator ite(*this);
				--(*this);
				return ite;
			}
			inline iterator operator++(int)
			{
				iterator ite(*this);
				++(*this);
				return ite;
			}
			inline iterator operator--(int)
			{
				iterator ite(*this);
				--(*this);
				return ite;
			}
			inline const iterator& operator+=(const int rhs) const
			{
				addIndexAndUpdate(rhs);
				return *this;
			}
			inline const iterator& operator+=(const unsigned int rhs) const
			{
				return operator+=(static_cast<int>(rhs));
			}
			inline const iterator& operator-=(const int rhs) const
			{
				addIndexAndUpdate(-rhs);
				return *this;
			}
			inline const iterator& operator-=(const unsigned int rhs) const
			{
				return operator-=(static_cast<int>(rhs));
			}
			inline iterator& operator+=(const int rhs)
			{
				addIndexAndUpdate(rhs);
				return *this;
			}
			inline iterator& operator+=(const unsigned int rhs)
			{
				return operator+=(static_cast<int>(rhs));
			}
			inline iterator& operator-=(const int rhs)
			{
				addIndexAndUpdate(-rhs);
				return *this;
			}
			inline iterator& operator-=(const unsigned int rhs)
			{
				return operator-=(static_cast<int>(rhs));
			}
			inline const iterator operator+(const int rhs) const
			{
				iterator ite(*this);
				ite += rhs;
				return ite;
			}
			inline const iterator operator+(const unsigned int rhs) const
			{
				return operator+(static_cast<int>(rhs));
			}
			inline const iterator operator-(const int rhs) const
			{
				iterator ite(*this);
				ite -= rhs;
				return ite;
			}
			inline const iterator operator-(const unsigned int rhs) const
			{
				return operator-(static_cast<int>(rhs));
			}
			inline iterator operator+(const int rhs)
			{
				iterator ite(*this);
				ite += rhs;
				return ite;
			}
			inline iterator operator+(const unsigned int rhs)
			{
				return operator+(static_cast<int>(rhs));
			}
			inline iterator operator-(const int rhs)
			{
				iterator ite(*this);
				ite -= rhs;
				return ite;
			}
			inline iterator operator-(const unsigned int rhs)
			{
				return operator-(static_cast<int>(rhs));
			}
			inline typename iterator::difference_type operator-(const iterator rhs)
			{
				if (m_index == INVALID_INDEX || rhs.m_index == INVALID_INDEX || m_index < rhs.m_index)
					return 0;
				return m_index - rhs.m_index;
			}
		public:
			//ムーブオペレータ
			inline iterator& operator=(const iterator&& rhs);
			inline iterator& operator=(const reverse_iterator&& rhs);
			//コピーオペレータ
			inline iterator& operator=(const iterator& rhs);
			inline iterator& operator=(const reverse_iterator& rhs);
		public:
			//アクセッサ
			inline bool isExist() const { return m_index != INVALID_INDEX && m_index < m_con->m_size; }
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const { return m_index != INVALID_INDEX; }
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const { return m_index == m_con->m_size; }//終端か？
			inline index_type getIndex() const { return m_index; }//インデックス
			inline const value_type* getValue() const { return m_value; }//現在の値
			inline value_type* getValue(){ return m_value; }//現在の値
		private:
			//メソッド
			//参照を更新
			void update(const index_type index) const;
			inline void addIndexAndUpdate(const int add) const
			{
				update(m_index + add);
			}
		public:
			//ムーブコンストラクタ
			inline iterator(const iterator&& obj);
			inline iterator(const reverse_iterator&& obj);
			//コピーコンストラクタ
			inline iterator(const iterator& obj);
			inline iterator(const reverse_iterator& obj);
			//コンストラクタ
			inline iterator(const container& con, const bool is_end);
			inline iterator(const container& con, const index_type index);
			inline iterator() :
				m_con(nullptr),
				m_index(INVALID_INDEX),
				m_value(nullptr)
			{}
			//デストラクタ
			inline ~iterator()
			{}
		private:
			//フィールド
			const container* m_con;//コンテナ
			mutable index_type m_index;//現在のインデックス
			mutable value_type* m_value;//現在の値
		};
		//--------------------
		//リバースイテレータ
		//class reverse_iterator : public std::reverse_iterator<iterator>
		class reverse_iterator : public std::iterator<std::random_access_iterator_tag, value_type>
		{
			friend class container;
			friend class iterator;
		public:
			typedef typename container::iterator iterator;
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const value_type&() const { return *getValue(); }
			inline operator value_type&(){ return *getValue(); }
			inline operator const value_type*() const { return getValue(); }
			inline operator value_type*(){ return getValue(); }
		public:
			//オペレータ
			inline const value_type& operator*() const { return *getValue(); }
			inline value_type& operator*(){ return *getValue(); }
			inline const_pointer operator->() const { return getValue(); }
			inline pointer operator->(){ return getValue(); }
			inline const reverse_iterator operator[](const int index) const
			{
				reverse_iterator ite(*m_con, false);
				ite.update(m_con->m_size - index);
				return ite;
			}
			inline reverse_iterator operator[](const int index)
			{
				reverse_iterator ite(*m_con, false);
				ite.update(m_con->m_size - index);
				return ite;
			}
		public:
			//比較オペレータ
			inline bool operator==(const reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_index == m_index;
			}
			inline bool operator!=(const reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_index != m_index;
			}
			inline bool operator>(const reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_index > m_index;
			}
			inline bool operator>=(const reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_index >= m_index;
			}
			inline bool operator<(const reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_index < m_index;
			}
			inline bool operator<=(const reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_index <= m_index;
			}
			//演算オペレータ
			inline const reverse_iterator& operator++() const
			{
				addIndexAndUpdate(1);
				return *this;
			}
			inline const reverse_iterator& operator--() const
			{
				addIndexAndUpdate(-1);
				return *this;
			}
			inline reverse_iterator& operator++()
			{
				addIndexAndUpdate(1);
				return *this;
			}
			inline reverse_iterator& operator--()
			{
				addIndexAndUpdate(-1);
				return *this;
			}
			inline const reverse_iterator operator++(int) const
			{
				reverse_iterator ite(*this);
				++(*this);
				return ite;
			}
			inline const reverse_iterator operator--(int) const
			{
				reverse_iterator ite(*this);
				--(*this);
				return ite;
			}
			inline reverse_iterator operator++(int)
			{
				reverse_iterator ite(*this);
				++(*this);
				return ite;
			}
			inline reverse_iterator operator--(int)
			{
				reverse_iterator ite(*this);
				--(*this);
				return ite;
			}
			inline const reverse_iterator& operator+=(const int rhs) const
			{
				addIndexAndUpdate(rhs);
				return *this;
			}
			inline const reverse_iterator& operator+=(const unsigned int rhs) const
			{
				return operator+=(static_cast<int>(rhs));
			}
			inline const reverse_iterator& operator-=(const int rhs) const
			{
				addIndexAndUpdate(-rhs);
				return *this;
			}
			inline const reverse_iterator& operator-=(const unsigned int rhs) const
			{
				return operator-=(static_cast<int>(rhs));
			}
			inline reverse_iterator& operator+=(const int rhs)
			{
				addIndexAndUpdate(rhs);
				return *this;
			}
			inline reverse_iterator& operator+=(const unsigned int rhs)
			{
				return operator+=(static_cast<int>(rhs));
			}
			inline reverse_iterator& operator-=(const int rhs)
			{
				addIndexAndUpdate(-rhs);
				return *this;
			}
			inline reverse_iterator& operator-=(const unsigned int rhs)
			{
				return operator-=(static_cast<int>(rhs));
			}
			inline const reverse_iterator operator+(const int rhs) const
			{
				reverse_iterator ite(*this);
				ite += rhs;
				return ite;
			}
			inline const reverse_iterator operator+(const unsigned int rhs) const
			{
				return operator+(static_cast<int>(rhs));
			}
			inline const reverse_iterator operator-(const int rhs) const
			{
				reverse_iterator ite(*this);
				ite -= rhs;
				return ite;
			}
			inline const reverse_iterator operator-(const unsigned int rhs) const
			{
				return operator-(static_cast<int>(rhs));
			}
			inline reverse_iterator operator+(const int rhs)
			{
				reverse_iterator ite(*this);
				ite += rhs;
				return ite;
			}
			inline reverse_iterator operator+(const unsigned int rhs)
			{
				return operator+(static_cast<int>(rhs));
			}
			inline reverse_iterator operator-(const int rhs)
			{
				reverse_iterator ite(*this);
				ite -= rhs;
				return ite;
			}
			inline reverse_iterator operator-(const unsigned int rhs)
			{
				return operator-(static_cast<int>(rhs));
			}
			inline typename reverse_iterator::difference_type operator-(const reverse_iterator rhs)
			{
				if (m_index == INVALID_INDEX || rhs.m_index == INVALID_INDEX || rhs.m_index < m_index)
					return 0;
				return rhs.m_index - m_index;
			}
		public:
			//ムーブオペレータ
			inline reverse_iterator& operator=(const reverse_iterator&& rhs);
			inline reverse_iterator& operator=(const iterator&& rhs);
			//コピーオペレータ
			inline reverse_iterator& operator=(const reverse_iterator& rhs);
			inline reverse_iterator& operator=(const iterator& rhs);
		public:
			//アクセッサ
			inline bool isExist() const { return m_index != INVALID_INDEX && m_index > 0; }
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const { return m_index != INVALID_INDEX; }
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const { return m_index == 0; }//終端か？
			inline index_type getIndex() const { return m_index - 1; }//インデックス
			inline const value_type* getValue() const { return m_value; }//現在の値
			inline value_type* getValue(){ return m_value; }//現在の値
		private:
			//メソッド
			//参照を更新
			void update(const index_type index) const;
			inline void addIndexAndUpdate(const int add) const
			{
				update(m_index - add);
			}
		public:
			//ベースを取得
			inline const iterator base() const
			{
				iterator ite(*this);
				return ite;
			}
			inline iterator base()
			{
				iterator ite(*this);
				return ite;
			}
		public:
			//ムーブコンストラクタ
			inline reverse_iterator(const reverse_iterator&& obj);
			inline reverse_iterator(const iterator&& obj);
			//コピーコンストラクタ
			inline reverse_iterator(const reverse_iterator& obj);
			inline reverse_iterator(const iterator& obj);
			//コンストラクタ
			inline reverse_iterator(const container& con, const bool is_end);
			inline reverse_iterator(const container& con, const index_type index);
			inline reverse_iterator() :
				m_con(nullptr),
				m_index(INVALID_INDEX),
				m_value(nullptr)
			{}
			//デストラクタ
			inline ~reverse_iterator()
			{}
		private:
			//フィールド
			const container* m_con;//コンテナ
			mutable index_type m_index;//現在のインデックス
			mutable value_type* m_value;//現在の値
		};
	public:
		//アクセッサ
		inline const value_type* at(const int index) const { return refElement(index); }
		inline value_type* at(const int index){ return refElement(index); }
		inline const value_type* operator[](const int index) const { return refElement(index); }
		inline value_type* operator[](const int index){ return refElement(index); }
		auto_clear_attr_t getAutoClearAttr() const { return m_autoClearAttr; }//コンテナ破棄時に残っている要素の自動クリア属性を取得
		void setAutoClearAttr(const auto_clear_attr_t attr){ m_autoClearAttr = attr; }//コンテナ破棄時に残っている要素の自動クリア属性を変更
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
		inline const_iterator cbegin() const { iterator ite(*this, false); return ite; }
		inline const_iterator cend() const { iterator ite(*this, true); return ite; }
		inline const iterator begin() const { iterator ite(*this, false); return ite; }
		inline const iterator end() const { iterator ite(*this, true); return ite; }
		inline iterator begin() { iterator ite(*this, false); return ite; }
		inline iterator end() { iterator ite(*this, true); return ite; }
		//リバースイテレータを取得
		inline const_reverse_iterator crbegin() const { reverse_iterator ite(*this, false); return ite; }
		inline const_reverse_iterator crend() const { reverse_iterator ite(*this, true); return ite; }
		inline const reverse_iterator rbegin() const { reverse_iterator ite(*this, false); return ite; }
		inline const reverse_iterator rend() const { reverse_iterator ite(*this, true); return ite; }
		inline reverse_iterator rbegin() { reverse_iterator ite(*this, false); return ite; }
		inline reverse_iterator rend() { reverse_iterator ite(*this, true); return ite; }
	private:
		//メソッド：要素アクセス系
		//※範囲チェックなし（非公開）
		inline const value_type* _refElement(const index_type index) const { return &m_array[index]; }//要素参照
		inline const value_type* _refFront() const { return _refElement(0); }//先頭要素参照
		inline const value_type* _refBack() const { return _refElement(m_size - 1); }//末尾要素参照
		inline const value_type* _refNew() const { return _refElement(m_size); }//新規要素参照
		inline value_type* _refElement(const index_type index){ return &m_array[index]; }//要素参照
		inline value_type* _refFront(){ return _refElement(0); }//先頭要素参照
		inline value_type* _refBack(){ return _refElement(m_size - 1); }//末尾要素参照
		inline value_type* _refNew(){ return _refElement(m_size); }//新規要素参照
		//inline index_type _adjIndex(const index_type index) const { return index >= 0 && index < m_maxSize ? index : INVALID_INDEX; }//インデックスを範囲内に補正
		inline index_type _adjIndex(const index_type index) const { return index < m_maxSize ? index : INVALID_INDEX; }//インデックスを範囲内に補正
		inline index_type _refIndex(const value_type* node) const{ return node - _refFront(); }//要素をインデックスに変換 ※範囲チェックなし
	public:
		//メソッド：要素アクセス系
		//※範囲チェックあり
		//inline const value_type* refElement(const index_type index) const { return index >= 0 && index < m_size ? _refElement(index) : nullptr; }//要素参照
		inline const value_type* refElement(const index_type index) const { return index < m_size ? _refElement(index) : nullptr; }//要素参照
		inline const value_type* refFront() const { return m_size == 0 ? nullptr : _refFront(); }//先頭要素参照
		inline const value_type* refBack() const { return m_size == 0 ? nullptr : _refBack(); }//末尾要素参照
		inline const value_type* refNew() const { return m_size == m_maxSize ? nullptr : _refNew(); }//新規要素参照
		inline value_type* refElement(const index_type index){ return  const_cast<value_type*>(const_cast<const container*>(this)->refElement(index)); }//要素参照
		inline value_type* refFront(){ return const_cast<value_type*>(const_cast<const container*>(this)->refFront()); }//先頭要素参照
		inline value_type* refBack(){ return const_cast<value_type*>(const_cast<const container*>(this)->refBack()); }//末尾要素参照
		inline value_type* refNew(){ return const_cast<value_type*>(const_cast<const container*>(this)->refNew()); }//新規要素参照
		inline index_type refIndex(const value_type* node) const{ return _adjIndex(_refIndex(node)); }//要素をインデックスに変換
	public:
		//一般メソッド：基本情報系
		inline size_type max_sizeReal() const { return m_maxSizeReal; }//最大要素数（実際の最大要素数）を取得
		inline size_type max_size() const { return m_maxSize; }//最大要素数を取得
		inline size_type capacity() const { return m_maxSize; }//最大要素数を取得
		inline size_type size() const { return m_size; }//使用中の要素数を取得
		inline size_type remain() const { return m_maxSize - m_size; }//残りの要素数を取得
		inline bool empty() const { return m_size == 0; }//空か？
		inline bool full() const { return m_size == m_maxSize; }//満杯か？
	public:
		//一般メソッド：要素アクセス系
		inline const value_type* front() const { return refFront(); }//先頭要素参照
		inline value_type* front(){ return refFront(); }//先頭要素参照
		inline const value_type* back() const { return refBack(); }//末尾要素参照
		inline value_type* back(){ return refBack(); }//末尾要素参照
	public:
		//配列の再割り当て
		//※コンテナ生成時のコンストラクタで配列を指定できなかった時に使用する。
		//※初期状態で使用中の要素数を指定する（-1で全要素を使用中にする）
		//※要素の初期化は行わない（必要なら size に 0 を指定して、後で resize() を呼び出す）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		template<std::size_t N>
		inline void assignArray(value_type(&array)[N], const int size = 0);
		//※ポインタと配列要素数指定版
		void assignArray(value_type* array, const size_type max_size, const int size = 0);
		//※voidポインタとバッファサイズ数指定版
		void assignArray(void* buff_ptr, const size_type buff_size, const int size = 0);
		
		//最大要素数の変更（新しい最大要素数を返す）
		//※名前に反して拡大・縮小が可能
		//※使用中の要素数以下にはできない
		//※実際の最大要素数以上にはできない
		//※実際の配列の範囲を超えないことは保証しない
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		size_type reserve(const size_type max_size);
		
		//最大要素数の変更（新しい最大要素数を返す）
		//※名前に反して拡大・縮小が可能
		//※使用中の要素数以下にもできる（削除された要素はデストラクタを呼び出す）
		//※実際の配列の範囲を超えないことは保証しない
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		size_type shrink(const size_type max_size);
		
		//最大要素数の変更（新しい最大要素数を返す）
		//※現在使用中のサイズに合わせる
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		size_type shrink_to_fit();
		
		//使用中のサイズを変更（新しいサイズを返す）
		//※新しい要素にはnew_valueをセットし、削除された要素はデストラクタを呼び出す
		//※最大要素数を超えるサイズにはできない
		//※指定数が -1 なら最大要素数に変更
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		size_type resize(const int size, const value_type& new_value);
		//※コンストラクタ呼び出し版
		template<typename... Tx>
		size_type resize(const int size, Tx... args);
		
		//使用中のサイズを変更（新しいサイズを返す）
		//※新しい値の代入も削除された要素のデストラクタ呼び出しも行わず、
		//　使用中のサイズだけを変更する
		//※最大要素数を超えるサイズにはできない
		//※指定数が -1 なら最大要素数に変更
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		size_type resizeSilent(const int size);
		
		//先頭から指定数の要素にデータを割り当てる
		//※new_valueで要素を上書きする
		//※既存の要素を上書きする際は、コピーオペレータを使用（デストラクタは呼び出さない）
		//※指定数が -1 なら最大素数を対象にする
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		size_type assign(const int size, const value_type& new_value);
		//※コンストラクタ呼び出し版
		//※既存の要素を上書きする際は、先にデストラクタを呼び出す
		template<typename... Tx>
		size_type assign(const int size, Tx... args);
		
		//末尾に要素を追加
		//※オブジェクト渡し
		//※オブジェクトのコピーが発生する点に注意（少し遅くなる）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		value_type* push_back(const value_type&& src);//ムーブ版
		value_type* push_back(const value_type& src);//コピー版
		
		//末尾に要素を追加
		//※パラメータ渡し
		//※オブジェクトのコンストラクタが呼び出される
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		template<typename... Tx>
		value_type* push_back(Tx... args);
		
		//末尾の要素を削除
		//※オブジェクトのデストラクタが呼び出される
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		bool pop_back();
		//※オブジェクトの値を受け取る
		bool pop_back(value_type& value);
	
	public:
		//クリア
		//※デストラクタを呼び出す
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		void clear();
	
	private:
		//要素の移動（昇順）
		void move_asc(const index_type dst_pos, const index_type src_pos, const size_type num);
		
		//要素の移動（降順）
		void move_desc(const index_type dst_pos, const index_type src_pos, const size_type num);
	
	public:
		//要素の挿入
		//※numの挿入によって最大要素数を超える場合は、可能なサイズに縮小して返す
		//※挿入した位置（先頭）のイテレータを返す
		//※指定数が -1 なら可能な限りの数を挿入
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		iterator insert(iterator pos, const int num, value_type& value);
		//※コンストラクタ呼び出し版
		template<typename... Tx>
		iterator insert(iterator pos, const int num, Tx... args);
	
	private:
		//要素の削除
		void _erase(const index_type index, const size_type num);
	
	public:
		//要素の削除
		//※デストラクタを呼び出す
		//※削除された直後の要素のイテレータを返す
		//※指定数が -1 なら以降の全要素を削除
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		iterator erase(iterator pos, const int num = 1);
		
		//※範囲指定版
		iterator erase(iterator start, iterator end);
	
	public:
		//ソート
		//※イントロソートを使用
		//※ope_type::sort_predicate() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		void sort();
		//※プレディケート関数指定版
		template<class PREDICATE>
		void sort(PREDICATE predicate);
		
		//安定ソート
		//※挿入ソートを使用
		//※ope_type::sort_predicate() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		void stable_sort();
		//※プレディケート関数指定版
		template<class PREDICATE>
		void stable_sort(PREDICATE predicate);
		
		//ソート済み状態チェック
		//※ope_type::sort_predicate() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		bool is_ordered() const;
		//※プレディケート関数指定版
		template<class PREDICATE>
		bool is_ordered(PREDICATE predicate) const;
	
	public:
		//線形探索
		//※探索値指定版
		//※ope_type::find_predicate() を使用して探索（標準では、データ型の operator==() に従って探索）
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		template<typename V>
		iterator find_value(const V& value);
		//※比較関数＋値指定版
		template<typename V, class PREDICATE>
		iterator find_value(const V& value, PREDICATE predicate);
		//※比較関数指定版
		//※値の指定は関数に含んでおく（クロ―ジャを用いるなどする）
		template<class PREDICATE>
		iterator find(PREDICATE predicate);
		
		//二分探索
		//※探索値指定版
		//※ope_type::search_comparison() を使用して探索（標準では、データ型の operator==() と operator<() に従って探索）
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		template<typename V>
		iterator binary_search_value(const V& value);
		//※比較関数＋値指定版
		template<typename V, class COMPARISON>
		iterator binary_search_value(const V& value, COMPARISON comparison);
		//※比較関数指定版
		//※値の指定は関数に含んでおく（クロ―ジャを用いるなどする）
		template<class COMPARISON>
		iterator binary_search(COMPARISON comparison);
	
	public:
		//コンストラクタ
		//※初期状態で使用中の要素数を指定する（-1で全要素を使用中にする）
		//※要素の初期化は行わない（必要なら size に 0 を指定して、後で resize() を呼び出す）
		template<std::size_t N>
		container(value_type(&array)[N], const int size = 0, const auto_clear_attr_t auto_clear_attr = NEVER_CLEAR);
		//※ポインタと配列要素数指定版
		container(value_type* array, const size_type max_size, const int size = 0, const auto_clear_attr_t auto_clear_attr = NEVER_CLEAR);
		//※voidポインタとバッファサイズ数指定版
		container(void* buff_ptr, const size_type buff_size, const int size = 0, const auto_clear_attr_t auto_clear_attr = NEVER_CLEAR);
		//デフォルトコンストラクタ
		inline container() :
			m_array(nullptr),
			m_maxSizeReal(0),
			m_maxSize(0),
			m_size(0),
			m_autoClearAttr(NEVER_CLEAR)
		{}
		//デストラクタ
		~container();
	
	private:
		//フィールド
		value_type* m_array;//配列の先頭
		size_type m_maxSizeReal;//実際（初期）の最大要素数
		size_type m_maxSize;//最大要素数（後から変更可能なサイズ）
		size_type m_size;//使用中の要素数
		auto_clear_attr_t m_autoClearAttr;//コンテナ破棄時に残っている要素の自動クリア属性
		mutable lock_type m_lock;//ロックオブジェクト
	};
	//--------------------
	//基本型定義マクロ消去
	#undef DECLARE_OPE_TYPES
}//namespace dynamic_array

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__DYNAMIC_ARRAY_H_

// End of file

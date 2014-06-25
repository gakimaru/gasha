#pragma once
#ifndef __RING_BUFFER_H_
#define __RING_BUFFER_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// ring_buffer.h
// リングバッファコンテナ【宣言部】※データのメモリ管理を行わない擬似コンテナ
//
// ※コンテナをインスタンス化する際は、別途下記のファイルをインクルードする必要あり
//
//   ・ring_buffer.inl   ... 【インライン関数／テンプレート関数実装部】
//                            コンテナクラスの操作が必要な場所でインクルード。
//   ・ring_buffer.cpp.h ... 【関数実装部】
//                            コンテナクラスの実体化が必要な場所でインクルード。
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
//リングバッファ
//--------------------------------------------------------------------------------
//データ構造とアルゴリズム
//【特徴】
//・単純な一次配列をリングバッファとして管理する。
//・配列の要素数を超えない範囲で、有効要素数を動的に変化させて管理する。
//・有効要素の増減に伴い、コンストラクタ／デストラクタの呼び出しを行う。
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//【本プログラムにおける実装要件】
//・アルゴリズムとデータ本体を分離したコンテナとする。
//・コンテナ自体は要素の実体を持たずメモリ確保も行わない。
//・コンストラクタで受けとった配列の参照を扱う。
//・STL（std::deque）との違いは下記の通り
//    - 固定長配列である。
//    - 赤黒木コンテナ（rb_tree）の実装と合わせた構造にしており、
//　　  操作用テンプレート構造体を用いる。
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//【具体的な活用の想定】
//・コンテナクラスと無関係の配列を、一時的にコンテナクラス化して操作する。
//--------------------------------------------------------------------------------

//#include <cstddef>//std::size_t, std::ptrdiff_t用
////#include <cstdint>//std::intptr_t用
//#include <iterator>//std::iterator用
//#include <new>//配置new,配置delete用
//#include <algorithm>//C++11 std::move用

namespace ring_buffer
{
#if 0
	//--------------------
	//リングバッファ操作用テンプレート構造体
	//※CRTPを活用し、下記のような派生構造体を作成して使用する
	//  //template<class OPE_TYPE, typename VALUE_TYPE>
	//  //struct base_ope_t;
	//  //struct 派生構造体名 : public ring_buffer::base_ope_t<派生構造体, 要素型>
	//	struct ope_t : public ring_buffer::base_ope_t<ope_t, data_t>
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
	//		typedef shared_spin_lock lock_type;//ロックオブジェクト型
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
		typedef dummy_shared_lock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※共有ロック（リード・ライトロック）でコンテナ操作をスレッドセーフにしたい場合は、
		//　base_ope_tの派生クラスにて、有効なロック型（shared_spin_lock など）を
		//　lock_type 型として再定義する。

		//デストラクタ呼び出し
		inline static void callDestructor(value_type* obj){ obj->~VALUE_TYPE(); }
		//※デストラクタの呼び出しを禁止したい場合、
		//　base_ope_tの派生クラスにて、なにもしない
		//　callDestructor メソッドを再定義する。

		//ソート用プレディケート関数オブジェクト
		//※trueでlhsの方が小さい（並び順が正しい）
		struct sort_predicate{
			inline bool operator()(const value_type& lhs, const value_type& rhs) const { return less<value_type>()(lhs, rhs); }
		};

		//探索用プレディケート関数オブジェクト
		//※trueで一致（探索成功）
		struct find_predicate{
			template<typename V>
			inline bool operator()(const value_type& lhs, const V& rhs) const { return equal_to<value_type>()(lhs, rhs); }
		};

		//探索用比較関数オブジェクト
		//※0で一致（探索成功）、1以上でlhsの方が大きい、-1以下でrhsの方が大きい
		struct search_comparison{
			template<typename V>
			inline int operator()(const value_type& lhs, const V& rhs) const { return compare_to<value_type>()(lhs, rhs); }
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
	//リングバッファコンテナ
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
			inline const_iterator operator[](const int logical_index) const
			{
				iterator ite(*m_con, false);
				ite.update(logical_index);
				return std::move(ite);
			}
			inline iterator operator[](const int logical_index)
			{
				iterator ite(*m_con, false);
				ite.update(logical_index);
				return std::move(ite);
			}
	public:
			//比較オペレータ
			inline bool operator==(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_logicalIndex == rhs.m_logicalIndex;
			}
			inline bool operator!=(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_logicalIndex != rhs.m_logicalIndex;
			}
			inline bool operator>(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_logicalIndex > rhs.m_logicalIndex;
			}
			inline bool operator>=(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_logicalIndex >= rhs.m_logicalIndex;
			}
			inline bool operator<(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_logicalIndex < rhs.m_logicalIndex;
			}
			inline bool operator<=(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_logicalIndex <= rhs.m_logicalIndex;
			}
			//演算オペレータ
			inline const_iterator& operator++() const
			{
				addIndexAndUpdate(1);
				return *this;
			}
			inline const_iterator& operator--() const
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
			inline const_iterator operator++(int) const
			{
				iterator ite(*this);
				++(*this);
				return std::move(ite);
			}
			inline const_iterator operator--(int) const
			{
				iterator ite(*this);
				--(*this);
				return std::move(ite);
			}
			inline iterator operator++(int)
			{
				iterator ite(*this);
				++(*this);
				return std::move(ite);
			}
			inline iterator operator--(int)
			{
				iterator ite(*this);
				--(*this);
				return std::move(ite);
			}
			inline const_iterator& operator+=(const typename iterator::difference_type rhs) const
			{
				addIndexAndUpdate(rhs);
				return *this;
			}
			inline const_iterator& operator+=(const std::size_t rhs) const
			{
				return operator+=(static_cast<typename iterator::difference_type>(rhs));
			}
			inline const_iterator& operator-=(const typename iterator::difference_type rhs) const
			{
				addIndexAndUpdate(-rhs);
				return *this;
			}
			inline const_iterator& operator-=(const std::size_t rhs) const
			{
				return operator-=(static_cast<typename iterator::difference_type>(rhs));
			}
			inline iterator& operator+=(const typename iterator::difference_type rhs)
			{
				addIndexAndUpdate(rhs);
				return *this;
			}
			inline iterator& operator+=(const std::size_t rhs)
			{
				return operator+=(static_cast<typename iterator::difference_type>(rhs));
			}
			inline iterator& operator-=(const typename iterator::difference_type rhs)
			{
				addIndexAndUpdate(-rhs);
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
			inline typename iterator::difference_type operator-(const iterator rhs)
			{
				if (m_logicalIndex == INVALID_INDEX || rhs.m_logicalIndex == INVALID_INDEX || m_logicalIndex < rhs.m_logicalIndex)
					return 0;
				return m_logicalIndex - rhs.m_logicalIndex;
			}
		public:
			//ムーブオペレータ
			inline iterator& operator=(const_iterator&& rhs)
			{
				m_con = rhs.m_con;
				m_logicalIndex = rhs.m_logicalIndex;
				m_value = rhs.m_value;
				return *this;
			}
			iterator& operator=(const_reverse_iterator&& rhs);
			//コピーオペレータ
			inline iterator& operator=(const_iterator& rhs)
			{
				m_con = rhs.m_con;
				m_logicalIndex = rhs.m_logicalIndex;
				m_value = rhs.m_value;
				return *this;
			}
			iterator& operator=(const_reverse_iterator& rhs);
		public:
			//アクセッサ
			inline bool isExist() const { return m_logicalIndex != INVALID_INDEX && m_logicalIndex < m_con->m_size; }
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const { return m_logicalIndex != INVALID_INDEX; }
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const { return m_logicalIndex == m_con->m_size; }//終端か？
			inline index_type getRealIndex() const { return m_logicalIndex == INVALID_INDEX ? INVALID_INDEX : m_con->_to_real_index(m_logicalIndex); }//物理インデックス
			inline index_type getIndex() const { return m_logicalIndex; }//論理インデックス
			inline const value_type* getValue() const { return m_value; }//現在の値
			inline value_type* getValue(){ return m_value; }//現在の値
		private:
			//メソッド
			void update(const index_type logical_index) const
			{
				//if (logical_index == INVALID_INDEX || logical_index < 0 || logical_index > static_cast<index_type>(m_con->m_size))
				if (logical_index > static_cast<index_type>(m_con->m_size))
				{
					m_logicalIndex = INVALID_INDEX;
					m_value = nullptr;
				}
				else
				{
					m_logicalIndex = logical_index;
					const index_type real_index = m_con->_to_real_index(m_logicalIndex);
					m_value = const_cast<value_type*>(m_con->_ref_real_element(real_index));
				}
			}
			inline void addIndexAndUpdate(const int add) const
			{
				update(m_logicalIndex + add);
			}
		public:
			//ムーブコンストラクタ
			iterator(const_iterator&& obj) :
				m_con(obj.m_con),
				m_logicalIndex(obj.m_logicalIndex),
				m_value(obj.m_value)
			{}
			iterator(const_reverse_iterator&& obj);
			//コピーコンストラクタ
			inline iterator(const_iterator& obj) :
				m_con(obj.m_con),
				m_logicalIndex(obj.m_logicalIndex),
				m_value(obj.m_value)
			{}
			iterator(const_reverse_iterator& obj);
			//コンストラクタ
			inline iterator(const container& con, const bool is_end) :
				m_con(&con),
				m_logicalIndex(INVALID_INDEX),
				m_value(nullptr)
			{
				if (!is_end)
					update(0);//先頭データ
				else
					update(m_con->m_size);//末尾データ
			}
			inline iterator(const container& con, const index_type logical_index) :
				m_con(&con),
				m_logicalIndex(INVALID_INDEX),
				m_value(nullptr)
			{
				update(logical_index);
			}
			inline iterator() :
				m_con(nullptr),
				m_logicalIndex(INVALID_INDEX),
				m_value(nullptr)
			{}
			//デストラクタ
			inline ~iterator()
			{}
		protected:
			//フィールド
			const container* m_con;//コンテナ
			mutable index_type m_logicalIndex;//現在の論理インデックス
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
			inline const_reverse_iterator operator[](const int logical_index) const
			{
				reverse_iterator ite(*m_con, false);
				ite.update(m_con->m_size - logical_index);
				return std::move(ite);
			}
			inline reverse_iterator operator[](const int logical_index)
			{
				reverse_iterator ite(*m_con, false);
				ite.update(m_con->m_size - logical_index);
				return std::move(ite);
			}
		public:
			//比較オペレータ
			inline bool operator==(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_logicalIndex == m_logicalIndex;
			}
			inline bool operator!=(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_logicalIndex != m_logicalIndex;
			}
			inline bool operator>(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_logicalIndex > m_logicalIndex;
			}
			inline bool operator>=(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_logicalIndex >= m_logicalIndex;
			}
			inline bool operator<(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_logicalIndex < m_logicalIndex;
			}
			inline bool operator<=(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_logicalIndex <= m_logicalIndex;
			}
			//演算オペレータ
			inline const_reverse_iterator& operator++() const
			{
				addIndexAndUpdate(1);
				return *this;
			}
			inline const_reverse_iterator& operator--() const
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
				addIndexAndUpdate(rhs);
				return *this;
			}
			inline const_reverse_iterator& operator+=(const std::size_t rhs) const
			{
				return operator+=(static_cast<typename reverse_iterator::difference_type>(rhs));
			}
			inline const_reverse_iterator& operator-=(const typename reverse_iterator::difference_type rhs) const
			{
				addIndexAndUpdate(-rhs);
				return *this;
			}
			inline const_reverse_iterator& operator-=(const std::size_t rhs) const
			{
				return operator-=(static_cast<typename reverse_iterator::difference_type>(rhs));
			}
			inline reverse_iterator& operator+=(const typename reverse_iterator::difference_type rhs)
			{
				addIndexAndUpdate(rhs);
				return *this;
			}
			inline reverse_iterator& operator+=(const std::size_t rhs)
			{
				return operator+=(static_cast<typename reverse_iterator::difference_type>(rhs));
			}
			inline reverse_iterator& operator-=(const typename reverse_iterator::difference_type rhs)
			{
				addIndexAndUpdate(-rhs);
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
			inline typename reverse_iterator::difference_type operator-(const reverse_iterator rhs)
			{
				if (m_logicalIndex == INVALID_INDEX || rhs.m_logicalIndex == INVALID_INDEX || rhs.m_logicalIndex < m_logicalIndex)
					return 0;
				return rhs.m_logicalIndex - m_logicalIndex;
			}
		public:
			//ムーブオペレータ
			inline reverse_iterator& operator=(const_reverse_iterator&& rhs)
			{
				m_con = rhs.m_con;
				m_logicalIndex = rhs.m_logicalIndex;
				m_value = rhs.m_value;
				return *this;
			}
			inline reverse_iterator& operator=(const_iterator&& rhs)
			{
				m_con = rhs.m_con;
				m_logicalIndex = rhs.m_logicalIndex;
				update(m_logicalIndex);
				return *this;
			}
			//コピーオペレータ
			inline reverse_iterator& operator=(const_reverse_iterator& rhs)
			{
				m_con = rhs.m_con;
				m_logicalIndex = rhs.m_logicalIndex;
				m_value = rhs.m_value;
				return *this;
			}
			inline reverse_iterator& operator=(const_iterator& rhs)
			{
				m_con = rhs.m_con;
				m_logicalIndex = rhs.m_logicalIndex;
				update(m_logicalIndex);
				return *this;
			}
		public:
			//アクセッサ
			inline bool isExist() const { return m_logicalIndex != INVALID_INDEX && m_logicalIndex > 0; }
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const { return m_logicalIndex != INVALID_INDEX; }
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const { return m_logicalIndex == 0; }//終端か？
			inline index_type getRealIndex() const { return m_logicalIndex == INVALID_INDEX ? INVALID_INDEX : m_con->_to_real_index(m_logicalIndex); }//物理インデックス
			inline index_type getIndex() const { return m_logicalIndex - 1; }//論理インデックス
			inline const value_type* getValue() const { return m_value; }//現在の値
			inline value_type* getValue(){ return m_value; }//現在の値
		private:
			//メソッド
			void update(const index_type logical_index) const
			{
				//if (logical_index == INVALID_INDEX || logical_index < 0 || index > static_cast<index_type>(m_con->m_size))
				if (logical_index > static_cast<index_type>(m_con->m_size))
				{
					m_logicalIndex = INVALID_INDEX;
					m_value = nullptr;
				}
				else
				{
					m_logicalIndex = logical_index;
					const index_type real_index = m_con->_to_real_index(m_logicalIndex);
					m_value = real_index == 0 ? const_cast<value_type*>(m_con->_ref_real_element(m_con->m_maxSize - 1)) : const_cast<value_type*>(m_con->_ref_real_element(real_index - 1));
				}
			}
			inline void addIndexAndUpdate(const int add) const
			{
				update(m_logicalIndex - add);
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
				m_logicalIndex(obj.m_logicalIndex),
				m_value(obj.m_value)
			{}
			inline reverse_iterator(const_iterator&& obj) :
				m_con(obj.m_con),
				m_logicalIndex(obj.m_logicalIndex),
				m_value(nullptr)
			{
				update(m_logicalIndex);
			}
			//コピーコンストラクタ
			inline reverse_iterator(const_reverse_iterator& obj) :
				m_con(obj.m_con),
				m_logicalIndex(obj.m_logicalIndex),
				m_value(obj.m_value)
			{}
			inline reverse_iterator(const_iterator& obj) :
				m_con(obj.m_con),
				m_logicalIndex(obj.m_logicalIndex),
				m_value(nullptr)
			{
				update(m_logicalIndex);
			}
			//コンストラクタ
			inline reverse_iterator(const container& con, const bool is_end) :
				m_con(&con),
				m_logicalIndex(INVALID_INDEX),
				m_value(nullptr)
			{
				if (!is_end)
					update(m_con->m_size);//末尾データ
				else
					update(0);//先頭データ
			}
			inline reverse_iterator(const container& con, const index_type logical_index) :
				m_con(&con),
				m_logicalIndex(INVALID_INDEX),
				m_value(nullptr)
			{
				update(logical_index);
			}
			inline reverse_iterator() :
				m_con(nullptr),
				m_logicalIndex(INVALID_INDEX),
				m_value(nullptr)
			{}
			//デストラクタ
			inline ~reverse_iterator()
			{}
		protected:
			//フィールド
			const container* m_con;//コンテナ
			mutable index_type m_logicalIndex;//現在の物理インデックス
			mutable value_type* m_value;//現在の値
		};
	public:
		//アクセッサ
		inline const value_type* at(const int logical_index) const { return ref_element(logical_index); }
		inline value_type* at(const int logical_index){ return ref_element(logical_index); }
		inline const value_type* operator[](const int logical_index) const { return ref_element(logical_index); }
		inline value_type* operator[](const int logical_index){ return ref_element(logical_index); }
		auto_clear_attr_t getAutoClearAttr() const { return m_autoClearAttr; }//コンテナ破棄時に残っている要素の自動クリア属性を取得
		void setAutoClearAttr(const auto_clear_attr_t attr){ m_autoClearAttr = attr; }//コンテナ破棄時に残っている要素の自動クリア属性を変更
	public:
		//キャストオペレータ
		inline operator lock_type&(){ return m_lock; }//共有ロックオブジェクト
		inline operator lock_type&() const { return m_lock; }//共有ロックオブジェクト ※mutable
	public:
		//メソッド
		inline size_type max_size() const { return m_maxSize; }//最大要素数を取得
		inline size_type capacity() const { return m_maxSize; }//最大要素数を取得
		inline size_type size() const { return m_size; }//使用中の要素数を取得
		inline size_type remain() const { return m_maxSize - m_size; }//残りの要素数を取得
		inline bool empty() const { return m_size == 0; }//空か？
		inline bool full() const { return m_size == m_maxSize; }//満杯か？
		inline index_type offset() const { return m_offset; }//有効要素の先頭インデックス（オフセット）
	private:
		//※範囲チェックなし
		inline index_type _to_real_index(const index_type logical_index) const//論理インデックスを物理インデックスに変換
		{
			const index_type real_index = m_offset + logical_index;
			return real_index < m_maxSize ? real_index : real_index - m_maxSize;
		}
		inline index_type _to_logical_index(const index_type real_index) const//物理インデックスを論理インデックスに変換
		{
			return real_index > m_offset ? real_index - m_offset : real_index + m_maxSize - m_offset;
		}
		inline size_type _front_new_real_index() const//先頭の新規インデックス
		{
			return m_offset == 0 ? m_maxSize - 1 : m_offset - 1;
		}
		inline size_type _back_new_real_index() const//末尾の新規インデックス
		{
			const index_type new_real_index = m_offset + m_size;
			return new_real_index < m_maxSize ? new_real_index : new_real_index - m_maxSize;
		}
		inline const value_type* _ref_real_element(const index_type real_index) const { return &m_array[real_index]; }//要素参照
		inline const value_type* _ref_element(const index_type logical_index) const { return &m_array[_to_real_index(logical_index)]; }//要素参照
		inline const value_type* _ref_front() const { return _ref_element(0); }//先頭要素参照
		inline const value_type* _ref_back() const { return _ref_element(m_size - 1); }//末尾要素参照
		inline const value_type* _ref_front_new() const { return _ref_real_element(_front_new_real_index()); }//先頭の新規要素参照
		inline const value_type* _ref_back_new() const { return _ref_real_element(_back_new_real_index()); }//末尾の新規要素参照
		inline value_type* _ref_real_element(const index_type real_index){ return &m_array[real_index]; }//要素参照
		inline value_type* _ref_element(const index_type logical_index){ return &m_array[_to_real_index(logical_index)]; }//要素参照
		inline value_type* _ref_front(){ return _ref_element(0); }//先頭要素参照
		inline value_type* _ref_back(){ return _ref_element(m_size - 1); }//末尾要素参照
		inline value_type* _ref_front_new(){ return _ref_real_element(_front_new_real_index()); }//先頭の新規要素参照
		inline value_type* _ref_back_new(){ return _ref_real_element(_back_new_real_index()); }//末尾の新規要素参照
	public:
		//※範囲チェックあり
		//inline const value_type* ref_element(const index_type logical_index) const { return logical_index >= 0 && logical_index < m_size ? _ref_element(logical_index) : nullptr; }//要素参照
		inline const value_type* ref_element(const index_type logical_index) const { return logical_index < m_size ? _ref_element(logical_index) : nullptr; }//要素参照
		inline const value_type* ref_front() const { return m_size == 0 ? nullptr : _ref_front(); }//先頭要素参照
		inline const value_type* ref_back() const { return m_size == 0 ? nullptr : _ref_back(); }//末尾要素参照
		inline const value_type* ref_front_new() const { return m_size == m_maxSize ? nullptr : _ref_front_new(); }//先頭の新規要素参照
		inline const value_type* ref_back_new() const { return m_size == m_maxSize ? nullptr : _ref_back_new(); }//末尾の新規要素参照
		inline value_type* ref_element(const index_type logical_index){ return  const_cast<value_type*>(const_cast<const container*>(this)->ref_element(logical_index)); }//要素参照
		inline value_type* ref_front(){ return const_cast<value_type*>(const_cast<const container*>(this)->ref_front()); }//先頭要素参照
		inline value_type* ref_back(){ return const_cast<value_type*>(const_cast<const container*>(this)->ref_back()); }//末尾要素参照
		inline value_type* ref_front_new(){ return const_cast<value_type*>(const_cast<const container*>(this)->ref_front_new()); }//先頭の新規要素参照
		inline value_type* ref_back_new(){ return const_cast<value_type*>(const_cast<const container*>(this)->ref_back_new()); }//末尾の新規要素参照
	private:
		inline int _adj_logical_index(const int logical_index) const { return logical_index >= 0 && logical_index < m_maxSize ? logical_index : INVALID_INDEX; }//論理インデックスを範囲内に補正
		inline int _ref_real_index(const value_type* node) const{ return node - _ref_front(); }//要素を物理インデックスに変換 ※範囲チェックなし
		inline int _ref_logical_index(const value_type* node) const{ return _to_logical_index(_ref_real_index(node)); }//要素を論理インデックスに変換 ※範囲チェックなし
	public:
		inline int ref_logical_index(const value_type* node) const{ return _adj_logical_index(_ref_logical_index(node)); }//要素を論理インデックスに変換
	public:
		inline const value_type* front() const { return ref_front(); }//先頭要素参照
		inline value_type* front(){ return ref_front(); }//先頭要素参照
		inline const value_type* back() const { return ref_back(); }//末尾要素参照
		inline value_type* back(){ return ref_back(); }//末尾要素参照
		//イテレータを取得
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
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
		//リバースイテレータを取得
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
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
	public:
		//配列の再割り当て
		//※コンテナ生成時のコンストラクタで配列を指定できなかった時に使用する。
		//※初期状態で使用中の要素数を指定する（-1で全要素を使用中にする）
		//※要素の初期化は行わない（必要なら size に 0 を指定して、後で resize() を呼び出す）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		template<size_type N>
		inline void assign_array(value_type(&array)[N], const int size = 0)
		{
			assign_array(array, N, size);
		}
		//※ポインタと配列要素数指定版
		void assign_array(value_type* array, const size_type max_size, const int size = 0)
		{
			if (m_array && m_autoClearAttr == AUTO_CLEAR)
				clear();//クリア
			m_array = array;
			m_maxSize = max_size;
			m_size = size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size);
			m_offset = 0;
		}
		//※voidポインタとバッファサイズ数指定版
		void assign_array(void* buff_ptr, const size_type buff_size, const int size = 0)
		{
			assign_array(static_cast<value_type*>(buff_ptr), buff_size / sizeof(value_type), size);
		}
		//使用中のサイズを変更（新しいサイズを返す）
		//※新しい要素にはnew_valueをセットし、削除された要素はデストラクタを呼び出す
		//※最大要素数を超えるサイズにはできない
		//※指定数が -1 なら最大要素数に変更
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		size_type resize(const int size, const value_type& new_value)
		{
			const size_type _size = size < 0 ? m_maxSize : static_cast<size_type>(size) < m_maxSize ? static_cast<size_type>(size) : m_maxSize;
			if (_size > m_size)
			{
				for (index_type index = m_size; index < _size; ++index)
				{
					value_type* value = _ref_element(index);
					*value = new_value;//新しい値を初期化
				}
			}
			else if (_size < m_size)
			{
				for (index_type index = _size; index < m_size; ++index)
				{
					value_type* value = _ref_element(index);
					ope_type::callDestructor(value);//デストラクタ呼び出し
					operator delete(value, value);//（作法として）deleteオペレータ呼び出し
				}
			}
			m_size = _size;
			return m_size;
		}
		//※コンストラクタ呼び出し版
		template<typename... Tx>
		size_type resize(const int size, Tx... args)
		{
			const size_type _size = size < 0 ? m_maxSize : static_cast<size_type>(size) < m_maxSize ? static_cast<size_type>(size) : m_maxSize;
			if (_size > m_size)
			{
				for (index_type index = m_size; index < _size; ++index)
				{
					value_type* value = _ref_element(index);
					new(value)value_type(args...);//コンストラクタ呼び出し
				}
			}
			else if (_size < m_size)
			{
				for (index_type index = size; index < m_size; ++index)
				{
					value_type* value = _ref_element(index);
					ope_type::callDestructor(value);//デストラクタ呼び出し
					operator delete(value, value);//（作法として）deleteオペレータ呼び出し
				}
			}
			m_size = _size;
			return m_size;
		}
		//使用中のサイズを変更（新しいサイズを返す）
		//※新しい値の代入も削除された要素のデストラクタ呼び出しも行わず、
		//　使用中のサイズだけを変更する
		//※最大要素数を超えるサイズにはできない
		//※指定数が -1 なら最大要素数に変更
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		size_type resize_silent(const int size)
		{
			const size_type _size = size < 0 ? m_maxSize : static_cast<size_type>(size) < m_maxSize ? static_cast<size_type>(size) : m_maxSize;
			m_size = _size;
			return m_size;
		}
		//先頭から指定数の要素にデータを割り当てる
		//※new_valueで要素を上書きする
		//※既存の要素を上書きする際は、コピーオペレータを使用（デストラクタは呼び出さない）
		//※指定数が -1 なら最大素数を対象にする
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		size_type assign(const int size, const value_type& new_value)
		{
			const size_type _size = size < 0 ? m_maxSize : static_cast<size_type>(size) < m_maxSize ? static_cast<size_type>(size) : m_maxSize;
			//{
			//	const size_type used_size = _size < m_size ? _size : m_size;
			//	for (index_type index = 0; index < used_size; ++index)
			//	{
			//		value_type* value = _ref_element(index);
			//		ope_type::callDestructor(value);//デストラクタ呼び出し
			//		operator delete(value, value);//（作法として）deleteオペレータ呼び出し
			//	}
			//}
			{
				for (index_type index = 0; index < _size; ++index)
				{
					value_type* value = _ref_element(index);
					*value = new_value;//データを上書き
				}
			}
			if(m_size < _size)
				m_size = _size;
			return m_size;
		}
		//※コンストラクタ呼び出し版
		//※既存の要素を上書きする際は、先にデストラクタを呼び出す
		template<typename... Tx>
		size_type assign(const int size, Tx... args)
		{
			const size_type _size = size < 0 ? m_maxSize : static_cast<size_type>(size) < m_maxSize ? static_cast<size_type>(size) : m_maxSize;
			{
				const size_type used_size = _size < m_size ? _size : m_size;
				for (index_type index = 0; index < used_size; ++index)
				{
					value_type* value = _ref_element(index);
					ope_type::callDestructor(value);//デストラクタ呼び出し
					operator delete(value, value);//（作法として）deleteオペレータ呼び出し
				}
			}
			{
				for (index_type index = 0; index < _size; ++index)
				{
					value_type* value = _ref_element(index);
					new(value)value_type(args...);//コンストラクタ呼び出し
				}
			}
			if (m_size < _size)
				m_size = _size;
			return m_size;
		}
		//先頭に要素を追加
		//※オブジェクト渡し
		//※オブジェクトのコピーが発生する点に注意（少し遅くなる）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		inline value_type* push_front(const value_type&& src)//ムーブ版
		{
			value_type* obj = ref_front_new();//サイズチェック含む
			if (!obj)
				return nullptr;
			*obj = std::move(src);
			++m_size;
			m_offset = m_offset == 0 ? m_maxSize - 1 : m_offset - 1;
			return obj;
		}
		inline value_type* push_front(const value_type& src)//コピー版
		{
			value_type* obj = ref_front_new();//サイズチェック含む
			if (!obj)
				return nullptr;
			*obj = src;
			++m_size;
			m_offset = m_offset == 0 ? m_maxSize - 1 : m_offset - 1;
			return obj;
		}
		//先頭に要素を追加
		//※パラメータ渡し
		//※オブジェクトのコンストラクタが呼び出される
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		template<typename... Tx>
		value_type* push_front(Tx... args)
		{
			value_type* obj = ref_front_new();//サイズチェック含む
			if (!obj)
				return nullptr;
			new(obj)value_type(args...);//コンストラクタ呼び出し
			++m_size;
			m_offset = m_offset == 0 ? m_maxSize - 1 : m_offset - 1;
			return obj;
		}
		//末尾に要素を追加
		//※オブジェクト渡し
		//※オブジェクトのコピーが発生する点に注意（少し遅くなる）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		inline value_type* push_back(const value_type&& src)//ムーブ版
		{
			value_type* obj = ref_back_new();//サイズチェック含む
			if (!obj)
				return nullptr;
			*obj = std::move(src);
			++m_size;
			return obj;
		}
		inline value_type* push_back(const value_type& src)//コピー版
		{
			value_type* obj = ref_back_new();//サイズチェック含む
			if (!obj)
				return nullptr;
			*obj = src;
			++m_size;
			return obj;
		}
		//末尾に要素を追加
		//※パラメータ渡し
		//※オブジェクトのコンストラクタが呼び出される
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		template<typename... Tx>
		value_type* push_back(Tx... args)
		{
			value_type* obj = ref_back_new();//サイズチェック含む
			if (!obj)
				return nullptr;
			new(obj)value_type(args...);//コンストラクタ呼び出し
			++m_size;
			return obj;
		}
		//先頭の要素を削除
		//※オブジェクトのデストラクタが呼び出される
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		bool pop_front()
		{
			if (m_size == 0)
				return false;
			value_type* value = const_cast<value_type*>(ref_front());
			ope_type::callDestructor(value);//デストラクタ呼び出し
			operator delete(value, value);//（作法として）deleteオペレータ呼び出し
			--m_size;
			m_offset = m_offset == m_maxSize - 1 ? 0 : m_offset + 1;
			return true;
		}
		//※オブジェクトの値を受け取る
		bool pop_front(value_type& value)
		{
			if (m_size == 0)
				return false;
			value_type* obj = const_cast<value_type*>(ref_front());
			value = std::move(*obj);//ムーブ
			ope_type::callDestructor(obj);//デストラクタ呼び出し
			operator delete(obj, obj);//（作法として）deleteオペレータ呼び出し
			--m_size;
			m_offset = m_offset == m_maxSize - 1 ? 0 : m_offset + 1;
			return true;
		}
		//末尾の要素を削除
		//※オブジェクトのデストラクタが呼び出される
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		bool pop_back()
		{
			if (m_size == 0)
				return false;
			value_type* value = const_cast<value_type*>(ref_front());
			ope_type::callDestructor(value);//デストラクタ呼び出し
			operator delete(value, value);//（作法として）deleteオペレータ呼び出し
			--m_size;
			return true;
		}
		//※オブジェクトの値を受け取る
		bool pop_back(value_type& value)
		{
			if (m_size == 0)
				return false;
			value_type* obj = const_cast<value_type*>(ref_back());
			value = std::move(*obj);//ムーブ
			ope_type::callDestructor(obj);//デストラクタ呼び出し
			operator delete(obj, obj);//（作法として）deleteオペレータ呼び出し
			--m_size;
			return true;
		}
	public:
		//クリア
		//※デストラクタを呼び出す
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		inline void clear()
		{
			if (m_size == 0)
				return;
			for (size_type i = 0; i < m_size; ++i)
			{
				value_type* value = _ref_element(i);
				ope_type::callDestructor(value);//デストラクタ呼び出し
				operator delete(value, value);//（作法として）deleteオペレータ呼び出し
			}
			m_size = 0;
			m_offset = 0;
		}
	private:
		//要素の移動（昇順）
		void move_asc(const index_type dst_pos, const index_type src_pos, const size_type num)
		{
			index_type _dst_pos = dst_pos;
			index_type _src_pos = src_pos;
			for (size_type i = 0; i < num; ++i)
			{
				value_type* dst = _ref_element(_dst_pos);
				value_type* src = _ref_element(_src_pos);
				if (_dst_pos >= m_size)
					new(dst)value_type(std::move(*src));//ムーブコンストラクタ
				else
					*dst = std::move(*src);//ムーブオペレータ
				++_dst_pos;
				++_src_pos;
			}
		}
		//要素の移動（降順）
		void move_desc(const index_type dst_pos, const index_type src_pos, const size_type num)
		{
			index_type _dst_pos = dst_pos + num - 1;
			index_type _src_pos = src_pos + num - 1;
			for (size_type i = 0; i < num; ++i)
			{
				value_type* dst = _ref_element(_dst_pos);
				value_type* src = _ref_element(_src_pos);
				if (_dst_pos >= m_size)
					new(dst)value_type(std::move(*src));//ムーブコンストラクタ
				else
					*dst = std::move(*src);//ムーブオペレータ
				--_dst_pos;
				--_src_pos;
			}
		}
	public:
		//要素の挿入
		//※numの挿入によって最大要素数を超える場合は、可能なサイズに縮小して返す
		//※挿入した位置（先頭）のイテレータを返す
		//※指定数が -1 なら可能な限りの数を挿入
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		iterator insert(iterator pos, const int num, value_type& value)
		{
			if (pos.isNotEnabled() || num == 0 || m_size == m_maxSize)
			{
				iterator ite(*this, INVALID_INDEX);
				return std::move(ite);
			}
			index_type index = pos.getIndex();
			const size_type remain = m_maxSize - m_size;
			const size_type _num = num < 0 || static_cast<size_type>(num) > remain ? remain : static_cast<size_type>(num);
			//移動
			move_desc(index + _num, index, _num);
			//要素数変更
			m_size += _num;
			//挿入
			index_type _index = index;
			for (size_type i = 0; i < _num; ++i)
			{
				value_type* new_value = _ref_element(_index);
				*new_value = value;
				++_index;
			}
			//終了
			iterator now(*this, index);
			return std::move(now);
		}
		//※コンストラクタ呼び出し版
		template<typename... Tx>
		iterator insert(iterator pos, const int num, Tx... args)
		{
			if (pos.isNotEnabled() || num == 0 || m_size == m_maxSize)
			{
				iterator ite(*this, INVALID_INDEX);
				return std::move(ite);
			}
			index_type index = pos.getIndex();
			const size_type remain = m_maxSize - m_size;
			const size_type _num = num < 0 || static_cast<size_type>(num) > remain ? remain : static_cast<size_type>(num);
			const size_type move_num = m_size - index;
			//移動
			move_desc(index + _num, index, move_num);
			//要素数変更
			m_size += _num;
			//挿入
			index_type _index = index;
			for (size_type i = 0; i < _num; ++i)
			{
				value_type* new_value = _ref_element(_index);
				new(new_value)value_type(args...);
				++_index;
			}
			//終了
			iterator now(*this, index);
			return std::move(now);
		}
	private:
		//要素の削除
		void _erase(const index_type index, const size_type num)
		{
			const size_type remain = m_maxSize - m_size;
			const size_type _num = num < 0 || num > remain ? remain : num;
			const size_type move_num = m_size - index;
			//削除
			index_type _index = index;
			for (size_type i = 0; i < _num; ++i)
			{
				value_type* delete_value = _ref_element(_index);
				ope_type::callDestructor(delete_value);//デストラクタ呼び出し
				operator delete(delete_value, delete_value);//（作法として）deleteオペレータ呼び出し
				++_index;
			}
			//移動
			move_asc(index, index + _num, move_num);
			//要素数変更
			m_size -= _num;
		}
	public:
		//要素の削除
		//※デストラクタを呼び出す
		//※削除された直後の要素のイテレータを返す
		//※指定数が -1 なら以降の全要素を削除
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		iterator erase(iterator pos, const int num = 1)
		{
			if (pos.isNotExist() || num == 0 || m_size == 0)
			{
				iterator ite(*this, INVALID_INDEX);
				return std::move(ite);
			}
			const index_type index = pos.getIndex();
			//削除
			_erase(index, num);
			//終了
			iterator now(*this, index);
			return std::move(now);
		}
		//※範囲指定版
		iterator erase(iterator start, iterator end)
		{
			if (start.isNotExist() || end.isNotExist() || start >= end || m_size == 0)
			{
				iterator ite(*this, INVALID_INDEX);
				return std::move(ite);
			}
			index_type index = start.getIndex();
			index_type end_index = end.getIndex();
			const size_type num = end_index - index;
			//削除
			_erase(index, num);
			//終了
			iterator now(*this, index);
			return std::move(now);
		}
	public:
		//ソート
		//※シェルソートを使用
		//※ope_type::sort_predicate() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		void sort()
		{
			iteratorIntroSort(begin(), end(), typename ope_type::sort_predicate());
		}
		//※プレディケート関数指定版
		template<class PREDICATE>
		void sort(PREDICATE predicate)
		{
			iteratorIntroSort(begin(), end(), predicate);
		}
		//安定ソート
		//※挿入ソートを使用
		//※ope_type::sort_predicate() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		void stable_sort()
		{
			iteratorInsertionSort(begin(), end(), typename ope_type::sort_predicate());
		}
		//※プレディケート関数指定版
		template<class PREDICATE>
		void stable_sort(PREDICATE predicate)
		{
			iteratorInsertionSort(begin(), end(), predicate);
		}
		//ソート済み状態チェック
		//※ope_type::sort_predicate() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		bool is_ordered() const
		{
			return iteratorCalcUnordered(begin(), end(), typename ope_type::sort_predicate()) == 0;
		}
		//※プレディケート関数指定版
		template<class PREDICATE>
		bool is_ordered(PREDICATE predicate) const
		{
			return iteratorCalcUnordered(begin(), end(), predicate) == 0;
		}
	public:
		//線形探索
		//※探索値指定版
		//※ope_type::find_predicate() を使用して探索（標準では、データ型の operator==() に従って探索）
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		template<typename V>
		iterator find_value(const V& value)
		{
			iterator found = iteratorLinearSearchValue(begin(), end(), value, typename ope_type::find_predicate());
			return std::move(found);
		}
		//※比較関数＋値指定版
		template<typename V, class PREDICATE>
		iterator find_value(const V& value, PREDICATE predicate)
		{
			iterator found = iteratorLinearSearchValue(begin(), end(), value, predicate);
			return std::move(found);
		}
		//※比較関数指定版
		//※値の指定は関数に含んでおく（クロ―ジャを用いるなどする）
		template<class PREDICATE>
		iterator find(PREDICATE predicate)
		{
			iterator found = iteratorLinearSearch(begin(), end(), predicate);
			return std::move(found);
		}
		//二分探索
		//※探索値指定版
		//※ope_type::search_comparison() を使用して探索（標準では、データ型の operator==() と operator<() に従って探索）
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		template<typename V>
		iterator binary_search_value(const V& value)
		{
			iterator found = iteratorBinarySearchValue(begin(), end(), value, typename ope_type::search_comparison());
			return std::move(found);
		}
		//※比較関数＋値指定版
		template<typename V, class COMPARISON>
		iterator binary_search_value(const V& value, COMPARISON comparison)
		{
			iterator found = iteratorBinarySearchValue(begin(), end(), value, comparison);
			return std::move(found);
		}
		//※比較関数指定版
		//※値の指定は関数に含んでおく（クロ―ジャを用いるなどする）
		template<class COMPARISON>
		iterator binary_search(COMPARISON comparison)
		{
			iterator found = iteratorBinarySearch(begin(), end(), comparison);
			return std::move(found);
		}
	public:
		//コンストラクタ
		//※初期状態で使用中の要素数を指定する（-1で全要素を使用中にする）
		//※要素の初期化は行わない（必要なら size に 0 を指定して、後で resize() を呼び出す）
		template<size_type N>
		container(value_type(&array)[N], const int size = 0, const auto_clear_attr_t auto_clear_attr = NEVER_CLEAR) :
			m_array(array),
			m_maxSize(N),
			m_size(size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size)),
			m_offset(0),
			m_autoClearAttr(auto_clear_attr)
		{}
		//※ポインタと配列要素数指定版
		container(value_type* array, const size_type max_size, const int size = 0, const auto_clear_attr_t auto_clear_attr = NEVER_CLEAR) :
			m_array(array),
			m_maxSize(max_size),
			m_size(size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size)),
			m_offset(0),
			m_autoClearAttr(auto_clear_attr)
		{}
		//※voidポインタとバッファサイズ数指定版
		container(void* buff_ptr, const size_type buff_size, const int size = 0, const auto_clear_attr_t auto_clear_attr = NEVER_CLEAR) :
			m_array(static_cast<value_type*>(buff_ptr)),
			m_maxSize(buff_size / sizeof(value_type)),
			m_size(size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size)),
			m_offset(0),
			m_autoClearAttr(auto_clear_attr)
		{}
		//デフォルトコンストラクタ
		container() :
			m_array(nullptr),
			m_maxSize(0),
			m_size(0),
			m_offset(0),
			m_autoClearAttr(NEVER_CLEAR)
		{}
		//デストラクタ
		~container()
		{
			//自動クリア属性が有効なら、自動クリアする
			if (m_array && m_autoClearAttr == AUTO_CLEAR)
				clear();
		}
	private:
		//フィールド
		value_type* m_array;//配列の先頭
		size_type m_maxSize;//最大要素数（後から変更可能なサイズ）
		size_type m_size;//使用中の要素数
		index_type m_offset;//有効要素の先頭インデックス（オフセット）
		auto_clear_attr_t m_autoClearAttr;//コンテナ破棄時に残っている要素の自動クリア属性
		mutable lock_type m_lock;//ロックオブジェクト
	};
	//イテレータのムーブオペレータ
	template<class OPE_TYPE>
	//typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(typename container<OPE_TYPE>::const_reverse_iterator&& rhs)//GCCはOK, VC++はNG
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator&& rhs)//VC++もOK
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		update(m_logicalIndex);
		return *this;
	}
	//イテレータのコピーオペレータ
	template<class OPE_TYPE>
	//typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(typename container<OPE_TYPE>::const_reverse_iterator& rhs)//GCCはOK, VC++はNG
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)//VC++もOK
	{
		m_con = rhs.m_con;
		m_logicalIndex = rhs.m_logicalIndex;
		update(m_logicalIndex);
		return *this;
	}
	//イテレータのムーブコンストラクタ
	template<class OPE_TYPE>
	//container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::const_reverse_iterator&& obj) ://GCCはOK, VC++はNG
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator&& obj) ://VC++もOK
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(nullptr)
	{
		update(m_logicalIndex);
	}
	//イテレータのコピーコンストラクタ
	template<class OPE_TYPE>
	//container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::const_reverse_iterator& obj) ://GCCはOK, VC++はNG
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) ://VC++もOK
		m_con(obj.m_con),
		m_logicalIndex(obj.m_logicalIndex),
		m_value(nullptr)
	{
		update(m_logicalIndex);
	}
	//--------------------
	//基本型定義マクロ消去
	#undef DECLARE_OPE_TYPES
#endif
}//namespace ring_buffer

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__RING_BUFFER_H_

// End of file

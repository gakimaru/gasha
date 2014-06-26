#pragma once
#ifndef __BINARY_HEAP_H_
#define __BINARY_HEAP_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// binary_heap.h
// 二分ヒープコンテナ【宣言部】※データのメモリ管理を行わない擬似コンテナ
//
// ※コンテナをインスタンス化する際は、別途下記のファイルをインクルードする必要あり
//
//   ・binary_heap.inl   ... 【インライン関数／テンプレート関数定義部】
//                            コンテナクラスの操作が必要な場所でインクルード。
//   ・binary_heap.cpp.h ... 【関数定義部】
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
//二分ヒープ
//--------------------------------------------------------------------------------
//データ構造とアルゴリズム
//【特徴】
//・二分木である。
//	  - ノードの子（左右両方）には、自身よりキーが小さいノードを連結。
//	  - 兄弟ノード間のキーの大小は不定。
//・平衡木である。
//	  - 木のバランスは常に保たれる。
//・以上の特徴により、最もキーが大きいノードの探索時間が、常にO(1)に保たれる。
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//【本プログラムにおける実装要件】
//・固定配列で実装し、一切メモリ確保・削除をしない。
//・ノード連結のポインタを使用しない。
//　一般的な二分ヒープと同じく、配列の順序に基づいて連結する。
//・この二分ヒープをプライオリティキューに利用した場合、
//　デキュー（pop）時に、エンキュー（push）の順序性は保証されない。
//　※この挙動はSTLと同じ。
//・ただし、（二分ヒープを内包する）プライオリティキューでは、キーの比較に
//　プライオリティとシーケンス番号を併用することで、順序性を保証する。
//・STL（std::priority_queue）との違いは下記の通り
//    - 固定長配列である。（STLは内部にstd::vectorを使用しており、自動拡張する）
//    - 赤黒木コンテナ（rb_tree）の実装と合わせた構造にしており、
//　　  操作用テンプレート構造体を用いる。
//　　  ※STLで使用する比較用の関数オブジェクトクラス(less)は用いない。
//　　- C++11の「範囲に基づくforループ」と、STLの「std::for_each()」が使用可能。
//　　  単純に配列の内容を列挙する。
//    - 効率化のために、オブジェクトのコピーを伴う操作を、
//      明示的なメソッド（pushCoping/ popCoping）にしている。
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//【具体的な活用の想定】
//・任意の構造のプライオリティキューに適用。
//--------------------------------------------------------------------------------

//#include <cstddef>//std::size_t, std::ptrdiff_t用
////#include <cstdint>//std::intptr_t用
//#include <iterator>//std::iterator用
//#include <new>//配置new,配置delete用

namespace binary_heap
{
#if 0
	//--------------------
	//二分ヒープ操作用テンプレート構造体
	//※CRTPを活用し、下記のような派生構造体を作成して使用する
	//  //template<class OPE_TYPE, typename NODE_TYPE>
	//  //struct baseOpe_t;
	//  //struct 派生構造体名 : public binary_heap::baseOpe_t<派生構造体, ノード型>
	//	struct ope_t : public binary_heap::baseOpe_t<ope_t, data_t>
	//	{
	//		//キーを比較
	//		//※lhsの方が小さいければ true を返す
	//		inline static bool less(const node_type& lhs, const node_type& rhs)
	//		{
	//			return lhs.m_key < rhs.m_key;
	//		}
	//		
	//		//ロック型 ※必要に応じて定義
	//		//※ロックでコンテナ操作をスレッドセーフにしたい場合は、
	//		//　有効なロック型（spin_lockなど）を lock_type 型として定義する。
	//		typedef spin_lock lock_type;//ロックオブジェクト型
	//	};
	template<class OPE_TYPE, typename NODE_TYPE>
	struct baseOpe_t
	{
		//型
		typedef OPE_TYPE ope_type;//ノード操作型
		typedef NODE_TYPE node_type;//ノード型

		//ロック型
		typedef dummy_lock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※ロックでコンテナ操作をスレッドセーフにしたい場合は、
		//　baseOpe_tの派生クラスにて、有効なロック型（spin_lock など）を
		//　lock_type 型として再定義する。
		//【補足】コンテナには、あらかじめロック制御のための仕組みがソースコードレベルで
		//　　　　仕込んであるが、有効な型を与えない限りは、実行時のオーバーヘッドは一切ない。

		//キーを比較
		//※lhsの方が小さいければ true を返す
		//※派生クラスでの実装が必要
		//inline static bool less(const node_type& lhs, const node_type& rhs)

		//STLのstd::priority_queueと共用するための関数オブジェクト
		inline bool operator()(const node_type& lhs, const node_type& rhs) const{ return ope_type::less(lhs, rhs); }

		//デストラクタ呼び出し
		inline static void callDestructor(node_type* obj){ obj->~NODE_TYPE(); }
	};
	//--------------------
	//基本型定義マクロ
	#define DECLARE_OPE_TYPES(OPE_TYPE) \
		typedef OPE_TYPE ope_type; \
		typedef typename ope_type::node_type node_type; \
		typedef node_type value_type; \
		typedef value_type& reference; \
		typedef const value_type& const_reference; \
		typedef value_type* pointer; \
		typedef const value_type* const_pointer; \
		typedef std::size_t size_type; \
		typedef std::size_t index_type; \
		typedef typename ope_type::lock_type lock_type;
	//--------------------
	//二分ヒープ操作関数：親のインデックス計算
	inline static std::size_t calcParent(const std::size_t index){ return (index - 1) >> 1; }
	//--------------------
	//二分ヒープ操作関数：子のインデックス計算
	inline static std::size_t calcChildL(const std::size_t index){ return (index << 1) + 1; }
	inline static std::size_t calcChildR(const std::size_t index){ return calcChildL(index) + 1; }
	//--------------------
	//二分ヒープ操作関数：アップヒープ
	//※ノードを上方に移動
	template<class OPE_TYPE, class PREDICATE>
	static typename OPE_TYPE::node_type* upHeap(typename OPE_TYPE::node_type* top, const std::size_t size, typename OPE_TYPE::node_type* now, PREDICATE less)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		index_type index = now - top;
		//if (index < 0 || index >= size)
		if (index >= size)
			return nullptr;
		while (index != 0)
		{
			index = calcParent(index);
			node_type* parent = top + index;
			if (!less(*parent, *now))
				break;
			swapValues(*parent, *now);
			now = parent;
		}
		return now;
	}
	//--------------------
	//二分ヒープ操作関数：ダウンヒープ
	//※ノードを下方に移動
	template<class OPE_TYPE, class PREDICATE>
	static typename OPE_TYPE::node_type* downHeap(typename OPE_TYPE::node_type* top, const std::size_t size, typename OPE_TYPE::node_type* now, PREDICATE less)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		index_type index = now - top;
		//if (index < 0 || index > size)
		if (index >= size)
			return nullptr;
		const size_type size_1 = size - 1;
		while (true)
		{
			index = calcChildL(index);
			if (index > size_1)
				break;
			node_type* child = top + index;
			const bool l_is_less = less(*child, *now);
			bool is_swap = false;
			if (index == size_1)
			{
				if (!l_is_less)
					is_swap = true;
			}
			else//if (index < size_1)
			{
				node_type* child_r = child + 1;
				const bool r_is_less = less(*child_r, *now);
				if (!r_is_less && (l_is_less || !less(*child_r, *child)))
				{
					is_swap = true;
					child = child_r;
					++index;
				}
				else if (!l_is_less)
					is_swap = true;
			}
			if (!is_swap)
				break;
			swapValues(*child, *now);
			now = child;
		}
		return now;
	}
	
	//----------------------------------------
	//二分ヒープコンテナ
	//※固定配列と使用中の要素数を持つ
	//※std::vectorとstd::priority_queueを一つにしたようなコンテナ。
	//　なお、emplace()メソッドには非対応。
	//※データのコピーを避けて処理効率を向上させるために、
	//　pushBegin()～pushEnd()、popBegin()～popEnd()
	//　というメソッドを用意している。内部のバッファを直接参照するので高速。
	//　なお、begin～end の間はロックが行われる点に注意。
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	class container
	{
	public:
		//型
		DECLARE_OPE_TYPES(OPE_TYPE);
	public:
		//定数
		static const size_type TABLE_SIZE = _TABLE_SIZE;//配列要素数
		static const index_type INVALID_INDEX = 0xffffffffu;//無効なインデックス
		enum status_t//ステータス
		{
			IDLE = 0,//アイドル
			PUSH_BEGINNING,//プッシュ開始中
			PUSH_ENDED,//プッシュ終了した
			PUSH_CANCELLED,//プッシュ取り消した
			POP_BEGINNING,//ポップ開始中
			POP_ENDED,//ポップ終了した
			POP_CANCELLED,//ポップ取り消した
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
		class iterator : public std::iterator<std::input_iterator_tag, value_type>
		{
			friend class container;
			friend class reverse_iterator;
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const value_type&() const { return *getValue(); }
			//inline operator value_type&(){ return *getValue(); }//std::input_iterator_tag には不要
			inline operator const value_type*() const { return getValue(); }
			//inline operator value_type*(){ return getValue(); }//std::input_iterator_tag には不要
		public:
			//オペレータ
			inline const value_type& operator*() const { return *getValue(); }
			//inline value_type& operator*(){ return *getValue(); }//std::input_iterator_tag には不要
			inline const_pointer operator->() const { return getValue(); }
			//inline pointer operator->(){ return getValue(); }//std::input_iterator_tag には不要
		#if 1//std::input_iterator_tag には本来必要ではない
			inline const_iterator operator[](const int index) const
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
		#endif
		public:
			//比較オペレータ
			inline bool operator==(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_index == rhs.index;
			}
			inline bool operator!=(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_index != rhs.m_index;
			}
		#if 1//std::input_iterator_tag には本来必要ではない
			inline bool operator>(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_index > rhs.m_index;
			}
			inline bool operator>=(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_index >= rhs.m_index;
			}
			inline bool operator<(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_index < rhs.m_index;
			}
			inline bool operator<=(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_index <= rhs.m_index;
			}
		#endif
			//演算オペレータ
			inline const_iterator& operator++() const
			{
				addIndexAndUpdate(1);
				return *this;
			}
		#if 1//std::input_iterator_tag には本来必要ではない
			inline const_iterator& operator--() const
			{
				addIndexAndUpdate(-1);
				return *this;
			}
		#endif
			inline iterator& operator++()
			{
				addIndexAndUpdate(1);
				return *this;
			}
		#if 1//std::input_iterator_tag には本来必要ではない
			inline iterator& operator--()
			{
				addIndexAndUpdate(-1);
				return *this;
			}
		#endif
			inline const_iterator operator++(int) const
			{
				iterator ite(*this);
				++(*this);
				return ite;
			}
		#if 1//std::input_iterator_tag には本来必要ではない
			inline const_iterator operator--(int) const
			{
				iterator ite(*this);
				--(*this);
				return ite;
			}
		#endif
			inline iterator operator++(int)
			{
				iterator ite(*this);
				++(*this);
				return ite;
			}
		#if 1//std::input_iterator_tag には本来必要ではない
			inline iterator operator--(int)
			{
				iterator ite(*this);
				--(*this);
				return ite;
			}
		#endif
		#if 1//std::input_iterator_tag には本来必要ではない
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
				return ite;
			}
			inline const_iterator operator+(const std::size_t rhs) const
			{
				return std::move(operator+(static_cast<typename iterator::difference_type>(rhs)));
			}
			inline const_iterator operator-(const typename iterator::difference_type rhs) const
			{
				iterator ite(*this);
				ite -= rhs;
				return ite;
			}
			inline const_iterator operator-(const std::size_t rhs) const
			{
				return std::move(operator-(static_cast<typename iterator::difference_type>(rhs)));
			}
			inline iterator operator+(const typename iterator::difference_type rhs)
			{
				iterator ite(*this);
				ite += rhs;
				return ite;
			}
			inline iterator operator+(const std::size_t rhs)
			{
				return std::move(operator+(static_cast<typename iterator::difference_type>(rhs)));
			}
			inline iterator operator-(const typename iterator::difference_type rhs)
			{
				iterator ite(*this);
				ite -= rhs;
				return ite;
			}
			inline iterator operator-(const std::size_t rhs)
			{
				return std::move(operator-(static_cast<typename iterator::difference_type>(rhs)));
			}
			inline typename iterator::difference_type operator-(const iterator rhs) const
			{
				if (m_index == INVALID_INDEX || rhs.m_index == INVALID_INDEX || m_index < rhs.m_index)
					return 0;
				return m_index - rhs.m_index;
			}
		#endif
		public:
			//ムーブオペレータ
			inline iterator& operator=(const_iterator&& rhs)
			{
				m_con = rhs.m_con;
				m_index = rhs.m_index;
				m_value = rhs.m_value;
				return *this;
			}
		#if 1//std::input_iterator_tag には本来必要ではない
			iterator& operator=(const_reverse_iterator&& rhs);
		#endif
			//コピーオペレータ
			inline iterator& operator=(const_iterator& rhs)
			{
				m_con = rhs.m_con;
				m_index = rhs.m_index;
				m_value = rhs.m_value;
				return *this;
			}
		#if 1//std::input_iterator_tag には本来必要ではない
			iterator& operator=(const_reverse_iterator& rhs);
		#endif
		public:
			//アクセッサ
			inline bool isExist() const { return m_index != INVALID_INDEX && m_index < m_con->m_used; }
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const { return m_index != INVALID_INDEX; }
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const { return m_index == m_con->m_size; }//終端か？
			inline index_type getIndex() const { return m_index; }//インデックス
			inline const value_type* getValue() const { return m_value; }//現在の値
			inline value_type* getValue(){ return m_value; }//現在の値
		private:
			//メソッド
			void update(const index_type index) const
			{
				//if (index == INVALID_INDEX || index < 0 || index > static_cast<index_type>(m_con->m_used))
				if (index > static_cast<index_type>(m_con->m_used))
				{
					m_index = INVALID_INDEX;
					m_value = nullptr;
				}
				else
				{
					m_index = index;
					m_value = const_cast<value_type*>(m_con->_ref_node(m_index));
				}
			}
			inline void addIndexAndUpdate(const int add) const
			{
				update(m_index + add);
			}
		public:
			//ムーブコンストラクタ
			iterator(const_iterator&& obj) :
				m_con(obj.m_con),
				m_index(obj.m_index),
				m_value(obj.m_value)
			{}
		#if 1//std::input_iterator_tag には本来必要ではない
			iterator(const_reverse_iterator&& obj);
		#endif
			//コピーコンストラクタ
			inline iterator(const_iterator& obj) :
				m_con(obj.m_con),
				m_index(obj.m_index),
				m_value(obj.m_value)
			{}
		#if 1//std::input_iterator_tag には本来必要ではない
			iterator(const_reverse_iterator& obj);
		#endif
			//コンストラクタ
			inline iterator(const container& con, const bool is_end) :
				m_con(&con),
				m_index(INVALID_INDEX),
				m_value(nullptr)
			{
				if (!is_end)
					update(0);//先頭データ
				else
					update(m_con->m_used);//末尾データ
			}
			inline iterator() :
				m_con(nullptr),
				m_index(INVALID_INDEX),
				m_value(nullptr)
			{}
			//デストラクタ
			inline ~iterator()
			{}
		protected:
			//フィールド
			const container* m_con;//コンテナ
			mutable index_type m_index;//現在のインデックス
			mutable value_type* m_value;//現在の値
		};
	#if 1//std::input_iterator_tag には本来必要ではない
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
			//inline operator value_type&(){ return *getValue(); }//std::input_iterator_tag には不要
			inline operator const value_type*() const { return getValue(); }
			//inline operator value_type*(){ return getValue(); }//std::input_iterator_tag には不要
		public:
			//オペレータ
			inline const value_type& operator*() const { return *getValue(); }
			//inline value_type& operator*(){ return *getValue(); }//std::input_iterator_tag には不要
			inline const_pointer operator->() const { return getValue(); }
			//inline pointer operator->(){ return getValue(); }//std::input_iterator_tag には不要
			inline const_reverse_iterator operator[](const int index) const
			{
				reverse_iterator ite(*m_con, false);
				ite.update(m_con->m_used - index);
				return ite;
			}
			inline reverse_iterator operator[](const int index)
			{
				reverse_iterator ite(*m_con, false);
				ite.update(m_con->m_used - index);
				return ite;
			}
		public:
			//比較オペレータ
			inline bool operator==(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_index == m_index;
			}
			inline bool operator!=(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_index != m_index;
			}
			inline bool operator>(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_index > m_index;
			}
			inline bool operator>=(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_index >= m_index;
			}
			inline bool operator<(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_index < m_index;
			}
			inline bool operator<=(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_index <= m_index;
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
				return ite;
			}
			inline const_reverse_iterator operator--(int) const
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
				return ite;
			}
			inline const_reverse_iterator operator+(const std::size_t rhs) const
			{
				return std::move(operator+(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			inline const_reverse_iterator operator-(const typename reverse_iterator::difference_type rhs) const
			{
				reverse_iterator ite(*this);
				ite -= rhs;
				return ite;
			}
			inline const_reverse_iterator operator-(const std::size_t rhs) const
			{
				return std::move(operator-(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			inline reverse_iterator operator+(const typename reverse_iterator::difference_type rhs)
			{
				reverse_iterator ite(*this);
				ite += rhs;
				return ite;
			}
			inline reverse_iterator operator+(const std::size_t rhs)
			{
				return std::move(operator+(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			inline reverse_iterator operator-(const typename reverse_iterator::difference_type rhs)
			{
				reverse_iterator ite(*this);
				ite -= rhs;
				return ite;
			}
			inline reverse_iterator operator-(const std::size_t rhs)
			{
				return std::move(operator-(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			inline typename reverse_iterator::difference_type operator-(const reverse_iterator rhs) const
			{
				if (m_index == INVALID_INDEX || rhs.m_index == INVALID_INDEX || rhs.m_index < m_index)
					return 0;
				return rhs.m_index - m_index;
			}
		public:
			//ムーブオペレータ
			inline reverse_iterator& operator=(const_reverse_iterator&& rhs)
			{
				m_con = rhs.m_con;
				m_index = rhs.m_index;
				m_value = rhs.m_value;
				return *this;
			}
			inline reverse_iterator& operator=(const_iterator&& rhs)
			{
				m_con = rhs.m_con;
				m_index = rhs.m_index;
				update(m_index);
				return *this;
			}
			//コピーオペレータ
			inline reverse_iterator& operator=(const_reverse_iterator& rhs)
			{
				m_con = rhs.m_con;
				m_index = rhs.m_index;
				m_value = rhs.m_value;
				return *this;
			}
			inline reverse_iterator& operator=(const_iterator& rhs)
			{
				m_con = rhs.m_con;
				m_index = rhs.m_index;
				update(m_index);
				return *this;
			}
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
			void update(const index_type index) const
			{
				//if (index == INVALID_INDEX || index < 0 || index > static_cast<index_type>(m_con->m_used))
				if (index > static_cast<index_type>(m_con->m_used))
				{
					m_index = INVALID_INDEX;
					m_value = nullptr;
				}
				else
				{
					m_index = index;
					m_value = const_cast<value_type*>(m_con->_ref_node(m_index)) - 1;
				}
			}
			inline void addIndexAndUpdate(const int add) const
			{
				update(m_index - add);
			}
		public:
			//ベースを取得
			inline const_iterator base() const
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
			inline reverse_iterator(const_reverse_iterator&& obj) :
				m_con(obj.m_con),
				m_index(obj.m_index),
				m_value(obj.m_value)
			{}
			inline reverse_iterator(const_iterator&& obj) :
				m_con(obj.m_con),
				m_index(obj.m_index),
				m_value(nullptr)
			{
				update(m_index);
			}
			//コピーコンストラクタ
			inline reverse_iterator(const_reverse_iterator& obj) :
				m_con(obj.m_con),
				m_index(obj.m_index),
				m_value(obj.m_value)
			{}
			inline reverse_iterator(const_iterator& obj) :
				m_con(obj.m_con),
				m_index(obj.m_index),
				m_value(nullptr)
			{
				update(m_index);
			}
			//コンストラクタ
			inline reverse_iterator(const container& con, const bool is_end) :
				m_con(&con),
				m_index(INVALID_INDEX),
				m_value(nullptr)
			{
				if (!is_end)
					update(m_con->m_used);//末尾データ
				else
					update(0);//先頭データ
			}
			inline reverse_iterator() :
				m_con(nullptr),
				m_index(INVALID_INDEX),
				m_value(nullptr)
			{}
			//デストラクタ
			inline ~reverse_iterator()
			{}
		protected:
			//フィールド
			const container* m_con;//コンテナ
			mutable index_type m_index;//現在のインデックス
			mutable value_type* m_value;//現在の値
		};
	#endif
	public:
		//アクセッサ
		inline const node_type* at(const int index) const { return ref_node(index); }
		//inline node_type* at(const int index){ return ref_node(index); }//直接変更禁止
		inline const node_type* operator[](const int index) const { return ref_node(index); }
		//inline node_type* operator[](const int index){ return ref_node(index); }//直接変更禁止
		inline status_t status() const { return m_status; }
	public:
		//キャストオペレータ
		inline operator lock_type&(){ return m_lock; }//共有ロックオブジェクト
		inline operator lock_type&() const { return m_lock; }//共有ロックオブジェクト ※mutable
	public:
		//メソッド
		inline size_type max_size() const { return TABLE_SIZE; }//最大要素数を取得
		inline size_type capacity() const { return TABLE_SIZE; }//最大要素数を取得
		inline size_type size() const { return m_used; }//使用中の要素数を取得
		inline size_type remain() const { return TABLE_SIZE - m_used; }//残りの要素数を取得
		inline bool empty() const { return m_used == 0; }//空か？
		inline bool full() const { return m_used == TABLE_SIZE; }//満杯か？
	public:
		int depth_max() const//最大の深さを取得
		{
			if (m_used == 0)
				return -1;
			int depth = 0;
			int used = m_used >> 1;
			while (used != 0)
			{
				++depth;
				used >>= 1;
			}
			return depth;
		}
	private:
		inline const node_type* _ref_node(const index_type index) const { return reinterpret_cast<const node_type*>(&m_table[index]); }//ノード参照
		inline const node_type* _ref_top() const { return _ref_node(0); }//先頭ノード参照
		inline const node_type* _ref_bottom() const { return _ref_node(m_used - 1); }//終端ノード参照
		inline const node_type* _ref_new() const { return _ref_node(m_used); }//新規ノード参照
		inline node_type* _ref_node(const index_type index){ return reinterpret_cast<node_type*>(&m_table[index]); }//ノード参照
		inline node_type* _ref_top(){ return _ref_node(0); }//先頭ノード参照
		inline node_type* _ref_bottom(){ return _ref_node(m_used - 1); }//終端ノード参照
		inline node_type* _ref_new(){ return _ref_node(m_used); }//新規ノード参照
	public:
		//inline const node_type* ref_node(const index_type index) const { return index >= 0 && index < m_used ? _ref_node(index) : nullptr; }//ノード参照
		inline const node_type* ref_node(const index_type index) const { return index < m_used ? _ref_node(index) : nullptr; }//ノード参照
		inline const node_type* ref_top() const { return m_used == 0 ? nullptr : _ref_top(); }//先頭ノード参照
		inline const node_type* ref_bottom() const { return m_used == 0 ? nullptr : _ref_bottom(); }//終端ノード参照
		inline const node_type* ref_new() const { return m_used == TABLE_SIZE ? nullptr : _ref_new(); }//新規ノード参照
		inline node_type* ref_node(const index_type index){ return  const_cast<node_type*>(const_cast<const container*>(this)->ref_node(index)); }//ノード参照
		inline node_type* ref_top(){ return const_cast<node_type*>(const_cast<const container*>(this)->ref_top()); }//先頭ノード参照
		inline node_type* ref_bottom(){ return const_cast<node_type*>(const_cast<const container*>(this)->ref_bottom()); }//終端ノード参照
		inline node_type* ref_new(){ return const_cast<node_type*>(const_cast<const container*>(this)->ref_new()); }//新規ノード参照
	private:
		//inline index_type _adj_index(const index_type index) const { return index >= 0 && index < TABLE_SIZE ? index : INVALID_INDEX; }//インデックスを範囲内に補正
		inline index_type _adj_index(const index_type index) const { return index < TABLE_SIZE ? index : INVALID_INDEX; }//インデックスを範囲内に補正
		inline index_type _ref_index(const node_type* node) const{ return node - _ref_top(); }//ノードをインデックスに変換 ※範囲チェックなし
		inline index_type _calc_parent(const index_type index) const { return binary_heap::calcParent(index); }//親インデックス計算 ※範囲チェックなし
		inline index_type _calc_child_l(const index_type index) const { return binary_heap::calcChildL(index); }//左側の子インデックス計算 ※範囲チェックなし
		inline index_type _calc_child_r(const index_type index) const { return binary_heap::calcChildR(index); }//右側の子インデックス計算 ※範囲チェックなし
	public:
		inline index_type ref_index(const node_type* node) const{ return _adj_index(_ref_index(node)); }//ノードをインデックスに変換
		inline index_type calc_parent(const index_type index) const { return _adj_index(_calc_parent(index)); }//親インデックス計算
		inline index_type calc_child_l(const index_type index) const { return _adj_index(_calc_child_l(index)); }//左側の子インデックス計算
		inline index_type calc_child_r(const index_type index) const { return _adj_index(_calc_child_r(index)); }//右側の子インデックス計算
		inline index_type calc_child(const int index, const bool is_right) const { return is_right ? calc_child_r(index) : calc_child_l(index); }//子インデックス計算
		inline const node_type* ref_parent(const node_type* node) const { return ref_node(_calc_parent(_ref_index(node))); }//親ノード参照
		inline const node_type* ref_child_l(const node_type* node) const { return ref_node(_calc_child_l(_ref_index(node))); }//左側の子ノード参照
		inline const node_type* ref_child_r(const node_type* node) const { return ref_node(_calc_child_r(_ref_index(node))); }//右側の子ノード参照
		inline const node_type* ref_child(const node_type* node, const bool is_right) const { return is_right ? ref_child_r(node) : ref_child_l(node); }//子ノード参照
		inline node_type* ref_parent(const index_type index){ return const_cast<node_type*>(const_cast<const container*>(this)->ref_parent(index)); }//親ノード参照
		inline node_type* ref_child_l(const index_type index){ return const_cast<node_type*>(const_cast<const container*>(this)->ref_child_l(index)); }//左側の子ノード参照
		inline node_type* ref_child_r(const index_type index){ return const_cast<node_type*>(const_cast<const container*>(this)->ref_child_r(index)); }//左側の子ノード参照
		inline node_type* ref_child(const index_type index, const bool is_right){ return const_cast<node_type*>(const_cast<const container*>(this)->ref_child(index, is_right)); }//子ノード参照
	public:
		inline const node_type* top() const { return ref_top(); }//先頭ノード参照
		inline node_type* top(){ return ref_top(); }//先頭ノード参照
		inline bool less(const node_type& lhs, const node_type& rhs) const { return ope_type::less(lhs, rhs); }//キー比較
		//イテレータを取得
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロック全体の前後で共有ロック（リードロック）の取得と解放を行う必要がある
		inline const_iterator cbegin() const
		{
			iterator ite(*this, false);
			return ite;
		}
		inline const_iterator cend() const
		{
			iterator ite(*this, true);
			return ite;
		}
		inline const_iterator begin() const
		{
			iterator ite(*this, false);
			return ite;
		}
		inline const_iterator end() const
		{
			iterator ite(*this, true);
			return ite;
		}
		inline iterator begin()
		{
			iterator ite(*this, false);
			return ite;
		}
		inline iterator end()
		{
			iterator ite(*this, true);
			return ite;
		}
		//リバースイテレータを取得
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロック全体の前後で共有ロック（リードロック）の取得と解放を行う必要がある
		inline const_reverse_iterator crbegin() const
		{
			reverse_iterator ite(*this, false);
			return ite;
		}
		inline const_reverse_iterator crend() const
		{
			reverse_iterator ite(*this, true);
			return ite;
		}
		inline const_reverse_iterator rbegin() const
		{
			reverse_iterator ite(*this, false);
			return ite;
		}
		inline const_reverse_iterator rend() const
		{
			reverse_iterator ite(*this, true);
			return ite;
		}
		inline reverse_iterator rbegin()
		{
			reverse_iterator ite(*this, false);
			return ite;
		}
		inline reverse_iterator rend()
		{
			reverse_iterator ite(*this, true);
			return ite;
		}
	private:
		//プッシュ（本体）：ムーブ
		node_type* _pushCopying(const node_type&& src)
		{
			if (m_status == PUSH_BEGINNING || m_status == POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
				return nullptr;
			node_type* obj = ref_new();
			if (!obj)
				return nullptr;
			*obj = std::move(src);
			m_status = PUSH_BEGINNING;
			return pushEnd();
		}
		//プッシュ（本体）：コピー
		node_type* _pushCopying(const node_type& src)
		{
			if (m_status == PUSH_BEGINNING || m_status == POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
				return nullptr;
			node_type* obj = ref_new();
			if (!obj)
				return nullptr;
			*obj = src;
			m_status = PUSH_BEGINNING;
			return pushEnd();
		}
	public:
		//プッシュ
		//※オブジェクト渡し
		//※オブジェクトのコピーが発生する点に注意（少し遅くなる）
		//※ムーブオペレータを使用してコピーする点に注意
		//※処理中、ロックを取得する
		inline node_type* pushCopying(const node_type&& src)
		{
			lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
			return _pushCopying(std::move(src));
		}
		inline node_type* pushCopying(const node_type& src)
		{
			lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
			return _pushCopying(src);
		}
	private:
		//プッシュ（本体）
		template<typename... Tx>
		node_type* _push(Tx... args)
		{
			if (m_status == PUSH_BEGINNING || m_status == POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
				return nullptr;
			node_type* obj = pushBegin(args...);
			if (!obj)
				return nullptr;
			return pushEnd();
		}
	public:
		//プッシュ
		//※パラメータ渡し
		//※オブジェクトのコンストラクタが呼び出される
		//※処理中、ロックを取得する
		template<typename... Tx>
		inline node_type* push(Tx... args)
		{
			lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
			return _push(args...);
		}
	private:
		//プッシュ開始（本体）
		template<typename... Tx>
		node_type* _pushBegin(Tx... args)
		{
			if (m_status == PUSH_BEGINNING || m_status == POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
				return nullptr;
			node_type* obj = ref_new();
			if (!obj)
				return nullptr;
			obj = new(obj)node_type(args...);//コンストラクタ呼び出し
			if (obj)
				m_status = PUSH_BEGINNING;
			return obj;
		}
	public:
		//プッシュ開始
		//※空きノードを取得し、コンストラクタが呼び出される
		//※処理が成功すると、ロックを取得した状態になる（pushEndで解放する）
		template<typename... Tx>
		inline node_type* pushBegin(Tx... args)
		{
			m_lock.lock();//ロックを取得（そのまま関数を抜ける）
			node_type* obj = _pushBegin(args...);//プッシュ開始
			if (!obj)
				m_lock.unlock();//プッシュ失敗時はロック解放
			return obj;
		}
	private:
		//プッシュ終了（本体）
		node_type* _pushEnd()
		{
			if (m_status != PUSH_BEGINNING)//プッシュ開始中以外なら処理しない
				return nullptr;
			node_type* obj = ref_new();
			if (!obj)
				return nullptr;
			++m_used;
			m_status = PUSH_ENDED;
			//末端の葉ノードとして登録された新規ノードを上方に移動
			return upHeap(obj);
		}
	public:
		//プッシュ終了
		//※追加した新規ノードを上に移動
		//※pushBeginで取得したロックを解放する
		inline node_type* pushEnd()
		{
			const bool unlock = (m_status == PUSH_BEGINNING);//プッシュ開始中ならアンロックする
			node_type* new_obj = _pushEnd();//プッシュ終了
			if (unlock)
				m_lock.unlock();//ロック解放
			return new_obj;
		}
	private:
		//プッシュ取り消し（本体）
		bool _pushCancel()
		{
			if (m_status != PUSH_BEGINNING)//プッシュ開始中以外なら処理しない
				return false;
			m_status = PUSH_CANCELLED;
			return true;
		}
	public:
		//プッシュ取り消し
		//※pushBeginで取得したロックを解放する
		inline bool pushCancel()
		{
			const bool unlock = (m_status == PUSH_BEGINNING);//プッシュ開始中ならアンロックする
			const bool result = _pushCancel();//プッシュ取り消し
			if (unlock)
				m_lock.unlock();//ロック解放
			return result;
		}
	private:
		//ポップ（本体）
		bool _popCopying(node_type& dst)
		{
			if (m_status == PUSH_BEGINNING || m_status == POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
				return false;
			const node_type* obj = popBegin();
			if (!obj)
				return false;
			dst = *obj;
			return popEnd();
		}
	public:
		//ポップ
		//※オブジェクトのコピーを受け取る領域を渡す
		//※オブジェクトのデストラクタが呼び出される ※コピー後に実行
		//※処理中、ロックを取得する
		inline bool popCopying(node_type& dst)
		{
			lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
			return _popCopying(dst);
		}
	private:
		//ポップ開始（本体）
		node_type* _popBegin()
		{
			if (m_status == PUSH_BEGINNING || m_status == POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
				return nullptr;
			node_type* obj = ref_top();
			if (obj)
				m_status = POP_BEGINNING;
			return obj;
		}
	public:
		//ポップ開始
		//※根ノード取得
		//※処理が成功すると、ロックを取得した状態になる（popEndで解放する）
		node_type* popBegin()
		{
			m_lock.lock();//ロックを取得（そのまま関数を抜ける）
			node_type* obj = _popBegin();//ポップ開始
			if (!obj)
				m_lock.unlock();//プッシュ失敗時はロック解放
			return obj;
		}
	private:
		//ポップ終了（本体）
		bool _popEnd()
		{
			if (m_status != POP_BEGINNING)//ポップ開始中以外なら処理しない
				return false;
			node_type* obj = ref_bottom();
			if (!obj)
				return false;
			ope_type::callDestructor(obj);//デストラクタ呼び出し
			operator delete(obj, obj);//（作法として）deleteオペレータ呼び出し
			m_status = POP_ENDED;
			//根ノードがポップされたので、末端の葉ノードを根ノードに上書きした上で、それを下方に移動
			node_type* top_obj = _ref_top();
			*top_obj = std::move(*obj);
			--m_used;
			downHeap(top_obj);
			return true;
		}
	public:
		//ポップ終了
		//※オブジェクトのデストラクタが呼び出される
		//※削除した根ノードの隙間を埋めるために、以降のノードを上に移動
		//※popBeginで取得したロックを解放する
		inline bool popEnd()
		{
			const bool unlock = (m_status == POP_BEGINNING);//ポップ開始中ならアンロックする
			const bool result = _popEnd();//ポップ終了
			if (unlock)
				m_lock.unlock();//ロック解放
			return result;
		}
	private:
		//ポップ取り消し（本体）
		bool _popCancel()
		{
			if (m_status != POP_BEGINNING)//ポップ開始中以外なら処理しない
				return false;
			m_status = POP_CANCELLED;
			return true;
		}
	public:
		//ポップ取り消し
		//※popBeginで取得したロックを解放する
		inline bool popCancel()
		{
			const bool unlock = (m_status == POP_BEGINNING);//ポップ開始中ならアンロックする
			const bool result = _popCancel();//ポップ取り消し
			if (unlock)
				m_lock.unlock();//ロック解放
			return result;
		}
	public:
		//ノードを上方に移動
		//※ロックを取得しないで処理するので注意！
		//　（局所的なロックで済む処理ではないため）
		//　必ず呼び出し元でロックを取得すること！
		node_type* upHeap(node_type* obj)
		{
			return binary_heap::upHeap<ope_type>(_ref_top(), m_used, obj, ope_type::less);
		}
		//ノードを下方に移動
		//※ロックを取得しないで処理するので注意！
		//　（局所的なロックで済む処理ではないため）
		//　必ず呼び出し元でロックを取得すること！
		node_type* downHeap(node_type* obj)
		{
			return binary_heap::downHeap<ope_type>(_ref_top(), m_used, obj, ope_type::less);
		}
	private:
		//クリア（本体）
		void _clear()
		{
			if (m_used == 0)
				return;
			node_type* obj_end = _ref_top() + m_used;
			for (node_type* obj = _ref_top(); obj < obj_end; ++obj)
			{
				ope_type::callDestructor(obj);//デストラクタ呼び出し
				operator delete(obj, obj);//（作法として）deleteオペレータ呼び出し
			}
			m_used = 0;
		}
	public:
		//クリア
		//※処理中、ロックを取得する
		inline void clear()
		{
			lock_guard<lock_type> lock(m_lock);//ロック取得（関数を抜ける時に自動開放）
			_clear();
		}
	public:
		//コンストラクタ
		container() :
			m_used(0),
			m_status(IDLE)
		{}
		//デストラクタ
		~container()
		{
			pushCancel();//プッシュ取り消し
			popCancel();//ポップ取り消し
		}
	private:
		//フィールド
		unsigned char m_table[TABLE_SIZE][sizeof(value_type)];//データテーブル
		size_type m_used;//使用数
		status_t m_status;//ステータス
		mutable lock_type m_lock;//ロックオブジェクト
	};
#if 1//std::input_iterator_tag には本来必要ではない
	//イテレータのムーブオペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	//typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator=(typename container<OPE_TYPE, _TABLE_SIZE>::const_reverse_iterator&& rhs)//GCCはOK, VC++はNG
	typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator&& rhs)//VC++もOK
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
	//イテレータのコピーオペレータ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	//typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator=(typename container<OPE_TYPE, _TABLE_SIZE>::const_reverse_iterator& rhs)//GCCはOK, VC++はNG
	typename container<OPE_TYPE, _TABLE_SIZE>::iterator& container<OPE_TYPE, _TABLE_SIZE>::iterator::operator=(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& rhs)//VC++もOK
	{
		m_con = rhs.m_con;
		m_index = rhs.m_index;
		update(m_index);
		return *this;
	}
	//イテレータのムーブコンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	//container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(typename container<OPE_TYPE, _TABLE_SIZE>::const_reverse_iterator&& obj) ://GCCはOK, VC++はNG
	container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator&& obj) ://VC++もOK
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
	}
	//イテレータのコピーコンストラクタ
	template<class OPE_TYPE, std::size_t _TABLE_SIZE>
	//container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(typename container<OPE_TYPE, _TABLE_SIZE>::const_reverse_iterator& obj) ://GCCはOK, VC++はNG
	container<OPE_TYPE, _TABLE_SIZE>::iterator::iterator(const typename container<OPE_TYPE, _TABLE_SIZE>::reverse_iterator& obj) ://VC++もOK
		m_con(obj.m_con),
		m_index(obj.m_index),
		m_value(nullptr)
	{
		update(m_index);
	}
#endif
	//--------------------
	//安全なプッシュ／ポップ操作クラス
	//※操作状態を記憶し、デストラクタで必ず完了させる
	template<class CON>
	class operation_guard
	{
	public:
		//型
		typedef CON container_type;//コンテナ型
		typedef typename CON::node_type node_type;//ノード型
		typedef typename CON::status_t status_t;//ステータス型
	public:
		//アクセッサ
		status_t status() const { return m_status; }//ステータスを取得
	public:
		//プッシュ開始
		template<typename... Tx>
		node_type* pushBegin(Tx... args)
		{
			if (m_status == status_t::PUSH_BEGINNING || m_status == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
				return nullptr;
			node_type* node = m_container.pushBegin(args...);//プッシュ開始
			if (node)
				m_status = status_t::PUSH_BEGINNING;//ステータス変更
			return node;
		}
		//プッシュ終了
		node_type* pushEnd()
		{
			if (m_status != status_t::PUSH_BEGINNING)//プッシュ開始中以外なら処理しない
				return nullptr;
			node_type* node = m_container.pushEnd();//プッシュ終了
			m_status = status_t::PUSH_ENDED;//ステータス変更
			return node;
		}
		//プッシュ取り消し
		bool pushCancel()
		{
			if (m_status != status_t::PUSH_BEGINNING)//プッシュ開始中以外なら処理しない
				return false;
			m_container.pushCancel();//プッシュ取り消し
			m_status = status_t::PUSH_CANCELLED;//ステータス変更
			return true;
		}
		//ポップ開始
		node_type* popBegin()
		{
			if (m_status == status_t::PUSH_BEGINNING || m_status == status_t::POP_BEGINNING)//プッシュ／ポップ開始中なら処理しない
				return nullptr;
			node_type* node = m_container.popBegin();//ポップ開始
			if (node)
				m_status = status_t::POP_BEGINNING;//ステータス変更
			return node;
		}
		//ポップ終了
		bool popEnd()
		{
			if (m_status != status_t::POP_BEGINNING)//ポップ開始中以外なら処理しない
				return false;
			const bool result = m_container.popEnd();//ポップ終了
			m_status = status_t::POP_ENDED;//ステータス変更
			return result;
		}
		//ポップ取り消し
		bool popCancel()
		{
			if (m_status != status_t::POP_BEGINNING)//ポップ開始中以外なら処理しない
				return false;
			m_container.popCancel();//ポップ取り消し
			m_status = status_t::POP_CANCELLED;//ステータス変更
			return true;
		}
	public:
		//コンストラクタ
		operation_guard(container_type& container) :
			m_container(container),
			m_status(status_t::IDLE)
		{}
		//デストラクタ
		~operation_guard()
		{
			pushEnd();//プッシュ終了
			popEnd();//ポップ終了
		}
	private:
		//フィールド
		container_type& m_container;//コンテナ
		status_t m_status;//ステータス
	};
	//--------------------
	//基本型定義マクロ消去
	#undef DECLARE_OPE_TYPES
#endif
}//namespace binary_heap

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__BINARY_HEAP_H_

// End of file

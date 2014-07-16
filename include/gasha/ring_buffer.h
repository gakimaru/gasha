#pragma once
#ifndef GASHA_INCLUDED_RING_BUFFER_H
#define GASHA_INCLUDED_RING_BUFFER_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// ring_buffer.h
// リングバッファコンテナ【宣言部】
// ※データのメモリ管理を行わない擬似コンテナ
//
// ※クラスをインスタンス化する際は、別途 .cpp.h ファイルをインクルードする必要あり。
// ※明示的なインスタンス化を避けたい場合は、ヘッダーファイルと共にインクルード。
// 　（この場合、実際に使用するメンバー関数しかインスタンス化されないので、対象クラスに不要なインターフェースを実装しなくても良い）
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
#include <gasha/sort_basic.h>//ソート処理基本
#include <gasha/search_basic.h>//探索処理基本
#include <gasha/allocator_common.h>//アロケータ共通設定・処理：コンストラクタ／デストラクタ呼び出し

#include <cstddef>//std::size_t, std::ptrdiff_t

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <iterator> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <iterator>//std::iterator

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//リングバッファ（ring buffer）
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//データ構造とアルゴリズム
//--------------------------------------------------------------------------------
//【特徴】
//・単純な一次元配列をリングバッファとして扱う。
//・配列の有効要素数を動的に変化させて扱う。
//--------------------------------------------------------------------------------
//【利点】
//・配列要素の昇順アクセスができる。
//・配列要素の降順アクセスができる。
//・配列の先頭・末端へのの要素追加がO(1)で行える。
//・配列の先頭・末端要素の削除がO(1)で行える。
//・ランダムアクセスができる。※動的配列よりは遅い
//・データ登録順の昇順アクセス、降順アクセスができる。※動的配列よりは遅い
//・ソートが速い。※動的配列よりは遅い
//・ソート済みを前提に、二分探索で高速な探索が行える。※動的配列よりは遅い
//・各要素のメモリオーバーヘッドがない。※要素間の連結情報などを持つ必要がない
//--------------------------------------------------------------------------------
//【欠点】
//・途中への要素挿入が遅い。
//・途中要素の削除が遅い。
//・最大要素数を想定したメモリを用意する必要がある。
//--------------------------------------------------------------------------------
//【本プログラムにおける実装要件】
//・アルゴリズムとデータを分離した擬似コンテナとする。
//・コンテナ自体はデータの実体（配列）を持たず、メモリ確保／解放を行わない。
//・データの実体（配列）はコンテナの外部から受け取り、コンテナは有効要素数とデータの先頭位置を管理する。
//・配列の最大要素数を超えない範囲で、有効要素数を動的に変化させて扱う。
//・有効要素の増減に伴い、コンストラクタ／デストラクタの呼び出しを行う。
//・コンテナは、STLの std::deque をモデルとしたインターフェースを実装する。
//・STL（std::deque）との主な違いは下記のとおり。
//    - 固定長配列であり、配列拡張に伴うメモリ再配置を行わない。
//    - 例外を扱わない。そのため、イテレータへの isExist() メソッドの追加や、
//      at()メソッドが返す情報がポインターなど、インターフェースの相違点がある。
//    - 利便性を考慮し、ソート処理、探索処理をメソッドとして実装。
//    - （他のコンテナと同様に）コンテナ操作対象・方法を設定した
//      構造体をユーザー定義して用いる。
//--------------------------------------------------------------------------------
//【想定する具的的な用途】
//・ランダムアクセス可能なキュー／スタック。
//・ストリーミング処理。
//--------------------------------------------------------------------------------

namespace ring_buffer
{
	//--------------------
	//リングバッファ操作用テンプレート構造体
	//※CRTPを活用し、下記のような派生構造体を作成して使用する
	//  //template<class OPE_TYPE, typename VALUE_TYPE>
	//  //struct 派生構造体名 : public ring_buffer::baseOpe<派生構造体名, 要素の型>
	//	struct ope : public ring_buffer::baseOpe<ope, data_t>
	//	{
	//		//ソート用プレディケート関数オブジェクト
	//		//※必要に応じて実装する
	//		struct predicateForSort{
	//			inline bool operator()(const value_type& lhs, const value_type& rhs) const { return lhs.??? < rhs.???; }
	//		};
	//
	//		//探索用プレディケート関数オブジェクト
	//		//※必要に応じて実装する
	//		struct predicateForFind{
	//			inline bool operator()(const value_type& lhs, const ???& rhs) const { return lhs.??? == rhs; }
	//		};
	//		
	//		//探索用比較関数オブジェクト
	//		//※必要に応じて実装する
	//		struct comparisonForSearch{
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
	struct baseOpe
	{
		//型
		typedef OPE_TYPE ope_type;//要素操作型
		typedef VALUE_TYPE value_type;//要素型

		//ロック型
		typedef dummySharedLock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※共有ロック（リード・ライトロック）でコンテナ操作をスレッドセーフにしたい場合は、
		//　baseOpeの派生クラスにて、有効なロック型（sharedSpinLock など）を
		//　lock_type 型として再定義する。

		//デストラクタ呼び出し
		inline static void callDestructor(value_type* obj){ GASHA_ callDestructor(obj); }
		//※デストラクタの呼び出しを禁止したい場合、
		//　baseOpeの派生クラスにて、なにもしない
		//　callDestructor メソッドを再定義する。

		//ソート用プレディケート関数オブジェクト
		//※trueでlhsの方が小さい（並び順が正しい）
		struct predicateForSort{
			inline bool operator()(const value_type& lhs, const value_type& rhs) const { return GASHA_ less<value_type>()(lhs, rhs); }
		};

		//探索用プレディケート関数オブジェクト
		//※trueで一致（探索成功）
		struct predicateForFind{
			template<typename V>
			inline bool operator()(const value_type& lhs, const V& rhs) const { return GASHA_ equal_to<value_type>()(lhs, rhs); }
		};

		//探索用比較関数オブジェクト
		//※0で一致（探索成功）、1以上でlhsの方が大きい、-1以下でrhsの方が大きい
		struct comparisonForSearch{
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
		typedef int difference_type; \
		typedef std::size_t size_type; \
		typedef std::size_t index_type; \
		typedef typename ope_type::lock_type lock_type;
		//typedef std::ptrdiff_t difference_type;//※difference_typeは、std::ptrdiff_t を使用するとイテレータのオペレータのオーバーロードで問題を起こすので、int 型で扱う

	//----------------------------------------
	//コンテナ破棄時の要素の自動クリア属性
	//※STLのコンテナと同様に、コンテナの破棄と共にコンテナ内の要素を破棄（デストラクタ呼び出し）するなら、AUTO_CLEARを指定する。
	//※デフォルトは NEVER_CLEAR。本クラスは擬似コンテナであり、外部の要素を操作するため、コンテナの破棄に連動しないのが基本。
	enum autoClearAttr_t
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
		typedef std::random_access_iterator_tag iterator_category;
		class iterator;
		class reverse_iterator;
		typedef const iterator const_iterator;
		typedef const reverse_iterator const_reverse_iterator;
		//--------------------
		//イテレータ
		class iterator : public std::iterator<iterator_category, value_type>
		{
			friend class container;
			friend class reverse_iterator;
		public:
			//※コンパイラによって優先して参照する型があいまいになることを避けるための定義
			typedef typename container::value_type value_type;
			typedef typename container::difference_type difference_type;
			typedef typename container::size_type size_type;
			typedef typename container::index_type indextype;
			typedef typename container::reverse_iterator reverse_iterator;
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const_reference() const { return *getValue(); }
			inline operator reference(){ return *getValue(); }
			inline operator const_pointer() const { return getValue(); }
			inline operator pointer(){ return getValue(); }
		public:
			//基本オペレータ
			inline const_reference operator*() const { return *getValue(); }
			inline reference operator*(){ return *getValue(); }
			inline const_pointer operator->() const { return getValue(); }
			inline pointer operator->(){ return getValue(); }
			inline const iterator operator[](const int logical_index) const;
			inline iterator operator[](const int logical_index);
		public:
			//比較オペレータ
			inline bool operator==(const iterator& rhs) const;
			inline bool operator!=(const iterator& rhs) const;
			inline bool operator>(const iterator& rhs) const;
			inline bool operator>=(const iterator& rhs) const;
			inline bool operator<(const iterator& rhs) const;
			inline bool operator<=(const iterator& rhs) const;
			//演算オペレータ
			inline iterator& operator++();
			inline iterator& operator--();
			inline iterator operator++(int);
			inline iterator operator--(int);
			inline const iterator& operator++() const { return const_cast<iterator*>(this)->operator++(); }
			inline const iterator& operator--() const { return const_cast<iterator*>(this)->operator--(); }
			inline const iterator operator++(int) const { return const_cast<iterator*>(this)->operator++(0); }
			inline const iterator operator--(int) const { return const_cast<iterator*>(this)->operator--(0); }
			inline iterator& operator+=(const difference_type rhs);
			inline iterator& operator+=(const size_type rhs) { return operator+=(static_cast<difference_type>(rhs)); }
			inline iterator& operator-=(const difference_type rhs);
			inline iterator& operator-=(const size_type rhs) { return operator-=(static_cast<difference_type>(rhs)); }
			inline const iterator& operator+=(const difference_type rhs) const { return const_cast<iterator*>(this)->operator+=(rhs); }
			inline const iterator& operator+=(const size_type rhs) const { return const_cast<iterator*>(this)->operator+=(rhs); }
			inline const iterator& operator-=(const difference_type rhs) const { return const_cast<iterator*>(this)->operator-=(rhs); }
			inline const iterator& operator-=(const size_type rhs) const  { return const_cast<iterator*>(this)->operator-=(rhs); }
			inline iterator operator+(const difference_type rhs) const;
			inline iterator operator+(const size_type rhs) const { return operator+(static_cast<difference_type>(rhs)); }
			inline iterator operator-(const difference_type rhs) const;
			inline iterator operator-(const size_type rhs) const { return operator-(static_cast<difference_type>(rhs)); }
			inline difference_type operator-(const iterator& rhs) const;
		public:
			//アクセッサ
			inline bool isExist() const;
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const;
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const;//終端か？
			inline index_type getRealIndex() const;//物理インデックス
			inline index_type getIndex() const;//論理インデックス
			inline const value_type* getValue() const;//現在の値
			inline value_type* getValue();//現在の値
		private:
			//メソッド
			//参照を更新
			void update(const index_type logical_index) const;
			void addIndexAndUpdate(const difference_type add) const;
		public:
			//ムーブオペレータ
			iterator& operator=(iterator&& rhs);
			iterator& operator=(reverse_iterator&& rhs);
			//コピーオペレータ
			iterator& operator=(const iterator& rhs);
			iterator& operator=(const reverse_iterator& rhs);
		public:
			//ムーブコンストラクタ
			iterator(iterator&& obj);
			iterator(reverse_iterator&& obj);
			//コピーコンストラクタ
			iterator(const iterator& obj);
			iterator(const reverse_iterator& obj);
		public:
			//コンストラクタ
			iterator(const container& con, const bool is_end);
			iterator(const container& con, const index_type logical_index);
			//デフォルトコンストラクタ
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
		class reverse_iterator : public std::iterator<iterator_category, value_type>
		{
			friend class container;
			friend class iterator;
		public:
			//※コンパイラによって優先して参照する型があいまいになることを避けるための定義
			typedef typename container::value_type value_type;
			typedef typename container::difference_type difference_type;
			typedef typename container::size_type size_type;
			typedef typename container::index_type indextype;
			typedef typename container::iterator iterator;
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const_reference() const { return *getValue(); }
			inline operator reference(){ return *getValue(); }
			inline operator const_pointer() const { return getValue(); }
			inline operator pointer(){ return getValue(); }
		public:
			//基本オペレータ
			inline const_reference operator*() const { return *getValue(); }
			inline reference operator*(){ return *getValue(); }
			inline const_pointer operator->() const { return getValue(); }
			inline pointer operator->(){ return getValue(); }
			inline const reverse_iterator operator[](const int logical_index) const;
			inline reverse_iterator operator[](const int logical_index);
		public:
			//比較オペレータ
			inline bool operator==(const reverse_iterator& rhs) const;
			inline bool operator!=(const reverse_iterator& rhs) const;
			inline bool operator>(const reverse_iterator& rhs) const;
			inline bool operator>=(const reverse_iterator& rhs) const;
			inline bool operator<(const reverse_iterator& rhs) const;
			inline bool operator<=(const reverse_iterator& rhs) const;
			//演算オペレータ
			inline reverse_iterator& operator++();
			inline reverse_iterator& operator--();
			inline reverse_iterator operator++(int);
			inline reverse_iterator operator--(int);
			inline const reverse_iterator& operator++() const { return const_cast<reverse_iterator*>(this)->operator++(); }
			inline const reverse_iterator& operator--() const { return const_cast<reverse_iterator*>(this)->operator--(); }
			inline const reverse_iterator operator++(int) const { return const_cast<reverse_iterator*>(this)->operator++(0); }
			inline const reverse_iterator operator--(int) const { return const_cast<reverse_iterator*>(this)->operator--(0); }
			inline reverse_iterator& operator+=(const difference_type rhs);
			inline reverse_iterator& operator+=(const size_type rhs) { return operator+=(static_cast<difference_type>(rhs)); }
			inline reverse_iterator& operator-=(const difference_type rhs);
			inline reverse_iterator& operator-=(const size_type rhs) { return operator-=(static_cast<difference_type>(rhs)); }
			inline const reverse_iterator& operator+=(const difference_type rhs) const { return const_cast<reverse_iterator*>(this)->operator+=(rhs); }
			inline const reverse_iterator& operator+=(const size_type rhs) const { return const_cast<reverse_iterator*>(this)->operator+=(rhs); }
			inline const reverse_iterator& operator-=(const difference_type rhs) const { return const_cast<reverse_iterator*>(this)->operator-=(rhs); }
			inline const reverse_iterator& operator-=(const size_type rhs) const  { return const_cast<reverse_iterator*>(this)->operator-=(rhs); }
			inline reverse_iterator operator+(const difference_type rhs) const;
			inline reverse_iterator operator+(const size_type rhs) const { return operator+(static_cast<difference_type>(rhs)); }
			inline reverse_iterator operator-(const difference_type rhs) const;
			inline reverse_iterator operator-(const size_type rhs) const { return operator-(static_cast<difference_type>(rhs)); }
			inline difference_type operator-(const reverse_iterator& rhs) const;
		public:
			//アクセッサ
			inline bool isExist() const;
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const;
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const;//終端か？
			inline index_type getRealIndex() const;//物理インデックス
			inline index_type getIndex() const;//論理インデックス
			inline const value_type* getValue() const;//現在の値
			inline value_type* getValue();//現在の値
		public:
			//ベースを取得
			inline const iterator base() const;
			inline iterator base();
		private:
			//メソッド
			//参照を更新
			void update(const index_type logical_index) const;
			void addIndexAndUpdate(const difference_type add) const;
		public:
			//ムーブオペレータ
			reverse_iterator& operator=(reverse_iterator&& rhs);
			reverse_iterator& operator=(iterator&& rhs);
			//コピーオペレータ
			reverse_iterator& operator=(const reverse_iterator& rhs);
			reverse_iterator& operator=(const iterator& rhs);
		public:
			//ムーブコンストラクタ
			reverse_iterator(reverse_iterator&& obj);
			reverse_iterator(iterator&& obj);
			//コピーコンストラクタ
			reverse_iterator(const reverse_iterator& obj);
			reverse_iterator(const iterator& obj);
		public:
			//コンストラクタ
			reverse_iterator(const container& con, const bool is_end);
			reverse_iterator(const container& con, const index_type logical_index);
			//デフォルトコンストラクタ
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
		//※at(), []()は、std::dequeと異なり、値のポインタを返し、例外を発生させない点に注意
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）または
		//　排他ロック（ライトロック）の取得と解放を行う必要がある
		inline const value_type* at(const int logical_index) const { return refElement(logical_index); }
		inline value_type* at(const int logical_index){ return refElement(logical_index); }
		inline const value_type* operator[](const int logical_index) const { return refElement(logical_index); }
		inline value_type* operator[](const int logical_index){ return refElement(logical_index); }
		autoClearAttr_t getAutoClearAttr() const { return m_autoClearAttr; }//コンテナ破棄時に残っている要素の自動クリア属性を取得
		void setAutoClearAttr(const autoClearAttr_t attr){ m_autoClearAttr = attr; }//コンテナ破棄時に残っている要素の自動クリア属性を変更
	public:
		//キャストオペレータ
		inline operator lock_type&(){ return m_lock; }//共有ロックオブジェクト
		inline operator lock_type&() const { return m_lock; }//共有ロックオブジェクト ※mutable
	public:
		//メソッド：ロック取得系
		//単一ロック取得
		inline GASHA_ unique_shared_lock<lock_type> lockUnique() const { GASHA_ unique_shared_lock<lock_type> lock(*this); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ with_lock_t&) const { GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ with_lock); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ with_lock_shared_t&) const { GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ with_lock_shared); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ try_to_lock_t&) const { GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ try_to_lock); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ try_to_lock_shared_t&) const { GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ try_to_lock_shared); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ adopt_lock_t&) const { GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ adopt_lock); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ adopt_shared_lock_t&) const { GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ adopt_shared_lock); return lock; }
		inline GASHA_ unique_shared_lock<lock_type> lockUnique(const GASHA_ defer_lock_t&) const { GASHA_ unique_shared_lock<lock_type> lock(*this, GASHA_ defer_lock); return lock; }
		//スコープロック取得
		inline GASHA_ lock_guard<lock_type> lockScoped() const { GASHA_ lock_guard<lock_type> lock(*this); return lock; }
		inline GASHA_ shared_lock_guard<lock_type> lockSharedScoped() const { GASHA_ shared_lock_guard<lock_type> lock(*this); return lock; }
	public:
		//メソッド：イテレータ取得系
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		//イテレータを取得
		inline const iterator cbegin() const { iterator ite(*this, false); return ite; }
		inline const iterator cend() const { iterator ite(*this, true); return ite; }
		inline const iterator begin() const { iterator ite(*this, false); return ite; }
		inline const iterator end() const { iterator ite(*this, true); return ite; }
		inline iterator begin() { iterator ite(*this, false); return ite; }
		inline iterator end() { iterator ite(*this, true); return ite; }
		//リバースイテレータを取得
		inline const reverse_iterator crbegin() const { reverse_iterator ite(*this, false); return ite; }
		inline const reverse_iterator crend() const { reverse_iterator ite(*this, true); return ite; }
		inline const reverse_iterator rbegin() const { reverse_iterator ite(*this, false); return ite; }
		inline const reverse_iterator rend() const { reverse_iterator ite(*this, true); return ite; }
		inline reverse_iterator rbegin() { reverse_iterator ite(*this, false); return ite; }
		inline reverse_iterator rend() { reverse_iterator ite(*this, true); return ite; }
	private:
		//メソッド：要素アクセス系（独自拡張版）
		//※範囲チェックなし（非公開）
		inline index_type _toRealIndex(const index_type logical_index) const;//論理インデックスを物理インデックスに変換
		inline index_type _toLogicalIndex(const index_type real_index) const;//物理インデックスを論理インデックスに変換
		inline size_type _frontNewRealIndex() const;//先頭の新規インデックス
		inline size_type _backNewRealIndex() const;//末尾の新規インデックス
		inline const value_type* _refRealElement(const index_type real_index) const { return &m_array[real_index]; }//要素参照
		inline const value_type* _refElement(const index_type logical_index) const { return &m_array[_toRealIndex(logical_index)]; }//要素参照
		inline const value_type* _refFront() const { return _refElement(0); }//先頭要素参照
		inline const value_type* _refBack() const { return _refElement(m_size - 1); }//末尾要素参照
		inline const value_type* _refFrontNew() const { return _refRealElement(_frontNewRealIndex()); }//先頭の新規要素参照
		inline const value_type* _refBackNew() const { return _refRealElement(_backNewRealIndex()); }//末尾の新規要素参照
		inline value_type* _refRealElement(const index_type real_index){ return &m_array[real_index]; }//要素参照
		inline value_type* _refElement(const index_type logical_index){ return &m_array[_toRealIndex(logical_index)]; }//要素参照
		inline value_type* _refFront(){ return _refElement(0); }//先頭要素参照
		inline value_type* _refBack(){ return _refElement(m_size - 1); }//末尾要素参照
		inline value_type* _refFrontNew(){ return _refRealElement(_frontNewRealIndex()); }//先頭の新規要素参照
		inline value_type* _refBackNew(){ return _refRealElement(_backNewRealIndex()); }//末尾の新規要素参照
		inline index_type _adjLogicalIndex(const index_type logical_index) const;//論理インデックスを範囲内に補正
		inline index_type _refRealIndex(const value_type* node) const;//要素を物理インデックスに変換 ※範囲チェックなし
		inline index_type _refLogicalIndex(const value_type* node) const;//要素を論理インデックスに変換 ※範囲チェックなし
	public:
		//メソッド：要素アクセス系（独自拡張版）
		//※範囲チェックあり（公開）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）または
		//　排他ロック（ライトロック）の取得と解放を行う必要がある
		//inline const value_type* refElement(const index_type logical_index) const { return logical_index >= 0 && logical_index < m_size ? _refElement(logical_index) : nullptr; }//要素参照
		inline const value_type* refElement(const index_type logical_index) const { return logical_index < m_size ? _refElement(logical_index) : nullptr; }//要素参照
		inline const value_type* refFront() const { return m_size == 0 ? nullptr : _refFront(); }//先頭要素参照
		inline const value_type* refBack() const { return m_size == 0 ? nullptr : _refBack(); }//末尾要素参照
		inline const value_type* refFrontNew() const { return m_size == m_maxSize ? nullptr : _refFrontNew(); }//先頭の新規要素参照
		inline const value_type* refBackNew() const { return m_size == m_maxSize ? nullptr : _refBackNew(); }//末尾の新規要素参照
		inline value_type* refElement(const index_type logical_index){ return  const_cast<value_type*>(const_cast<const container*>(this)->refElement(logical_index)); }//要素参照
		inline value_type* refFront(){ return const_cast<value_type*>(const_cast<const container*>(this)->refFront()); }//先頭要素参照
		inline value_type* refBack(){ return const_cast<value_type*>(const_cast<const container*>(this)->refBack()); }//末尾要素参照
		inline value_type* refFrontNew(){ return const_cast<value_type*>(const_cast<const container*>(this)->refFrontNew()); }//先頭の新規要素参照
		inline value_type* refBackNew(){ return const_cast<value_type*>(const_cast<const container*>(this)->refBackNew()); }//末尾の新規要素参照
		inline index_type refLogicalIndex(const value_type* node) const{ return _adjLogicalIndex(_refLogicalIndex(node)); }//要素を論理インデックスに変換
	public:
		//メソッド：基本情報系
		inline size_type max_size() const { return m_maxSize; }//最大要素数を取得
		inline size_type capacity() const { return m_maxSize; }//最大要素数を取得
		inline size_type size() const { return m_size; }//使用中の要素数を取得
		inline size_type remain() const { return m_maxSize - m_size; }//残りの要素数を取得
		inline bool empty() const { return m_size == 0; }//空か？
		inline bool full() const { return m_size == m_maxSize; }//満杯か？
		inline index_type offset() const { return m_offset; }//有効要素の先頭インデックス（オフセット）
	public:
		//メソッド：要素アクセス系
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）または
		//　排他ロック（ライトロック）の取得と解放を行う必要がある
		inline const value_type* front() const { return refFront(); }//先頭要素参照
		inline const value_type* back() const { return refBack(); }//末尾要素参照
		inline value_type* front(){ return refFront(); }//先頭要素参照
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
		inline void assignArray(void* buff_ptr, const size_type buff_size, const int size = 0);
		
		//使用中のサイズを変更（新しいサイズを返す）
		//※新しい要素にはnew_valueをセットし、削除された要素はデストラクタを呼び出す
		//※最大要素数を超えるサイズにはできない
		//※指定数が -1 なら最大要素数に変更
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		size_type resize(const int size, const value_type& new_value);
		//※コンストラクタ呼び出し版
		template<typename... Tx>
		size_type resize(const int size, Tx&&... args);
		
		//使用中のサイズを変更（新しいサイズを返す）
		//※新しい値の代入も削除された要素のデストラクタ呼び出しも行わず、
		//　使用中のサイズだけを変更する
		//※最大要素数を超えるサイズにはできない
		//※指定数が -1 なら最大要素数に変更
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		size_type resizeSilently(const int size);
		
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
		size_type assign(const int size, Tx&&... args);
		
		//先頭に要素を追加
		//※オブジェクト渡し
		//※オブジェクトのコピーが発生する点に注意（少し遅くなる）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		value_type* push_front(value_type&& src);//ムーブ版
		value_type* push_front(const value_type& src);//コピー版
		
		//先頭に要素を追加
		//※パラメータ渡し
		//※オブジェクトのコンストラクタが呼び出される
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		template<typename... Tx>
		value_type* push_front(Tx&&... args);
		
		//末尾に要素を追加
		//※オブジェクト渡し
		//※オブジェクトのコピーが発生する点に注意（少し遅くなる）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		value_type* push_back(value_type&& src);//ムーブ版
		value_type* push_back(const value_type& src);//コピー版
		
		//末尾に要素を追加
		//※パラメータ渡し
		//※オブジェクトのコンストラクタが呼び出される
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		template<typename... Tx>
		value_type* push_back(Tx&&... args);
		
		//先頭の要素を削除
		//※オブジェクトのデストラクタが呼び出される
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		bool pop_front();
		//※オブジェクトの値を受け取る
		bool pop_front(value_type& value);
		
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
		void moveAsc(const index_type dst_pos, const index_type src_pos, const size_type num);
		//要素の移動（降順）
		void moveDesc(const index_type dst_pos, const index_type src_pos, const size_type num);
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
		iterator insert(iterator pos, const int num, Tx&&... args);
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
		//※ope_type::predicateForSort() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		inline void sort();
		//※プレディケート関数指定版
		template<class PREDICATE>
		inline void sort(PREDICATE predicate);
		
		//安定ソート
		//※挿入ソートを使用
		//※ope_type::predicateForSort() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		inline void stableSort();
		//※プレディケート関数指定版
		template<class PREDICATE>
		inline void stableSort(PREDICATE predicate);
		
		//ソート済み状態チェック
		//※ope_type::predicateForSort() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		inline bool isOrdered() const;
		//※プレディケート関数指定版
		template<class PREDICATE>
		inline bool isOrdered(PREDICATE predicate) const;
	public:
		//線形探索
		//※探索値指定版
		//※ope_type::predicateForFind() を使用して探索（標準では、データ型の operator==() に従って探索）
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		template<typename V>
		inline iterator findValue(const V& value);
		//※比較関数＋値指定版
		template<typename V, class PREDICATE>
		inline iterator findValue(const V& value, PREDICATE predicate);
		//※比較関数指定版
		//※値の指定は関数に含んでおく（クロ―ジャを用いるなどする）
		template<class PREDICATE>
		inline iterator find(PREDICATE predicate);
		
		//二分探索
		//※探索値指定版
		//※ope_type::comparisonForSearch() を使用して探索（標準では、データ型の operator==() と operator<() に従って探索）
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		template<typename V>
		inline iterator binarySearchValue(const V& value);
		//※比較関数＋値指定版
		template<typename V, class COMPARISON>
		inline iterator binarySearchValue(const V& value, COMPARISON comparison);
		//※比較関数指定版
		//※値の指定は関数に含んでおく（クロ―ジャを用いるなどする）
		template<class COMPARISON>
		inline iterator binary_search(COMPARISON comparison);
	public:
		//コンストラクタ
		//※初期状態で使用中の要素数を指定する（-1で全要素を使用中にする）
		//※要素の初期化は行わない（必要なら size に 0 を指定して、後で resize() を呼び出す）
		template<std::size_t N>
		container(value_type(&array)[N], const int size = 0, const autoClearAttr_t auto_clear_attr = NEVER_CLEAR);
		//※ポインタと配列要素数指定版
		container(value_type* array, const size_type max_size, const int size = 0, const autoClearAttr_t auto_clear_attr = NEVER_CLEAR);
		//※voidポインタとバッファサイズ数指定版
		container(void* buff_ptr, const size_type buff_size, const int size = 0, const autoClearAttr_t auto_clear_attr = NEVER_CLEAR);
		//デフォルトコンストラクタ
		inline container();
		//デストラクタ
		~container();
	private:
		//フィールド
		value_type* m_array;//配列の先頭
		size_type m_maxSize;//最大要素数（後から変更可能なサイズ）
		size_type m_size;//使用中の要素数
		index_type m_offset;//有効要素の先頭インデックス（オフセット）
		autoClearAttr_t m_autoClearAttr;//コンテナ破棄時に残っている要素の自動クリア属性
		mutable lock_type m_lock;//ロックオブジェクト
	};
	//----------------------------------------
	//シンプルリングバッファコンテナ
	//※操作用構造体の定義を省略してコンテナを使用するためのクラス。
	//※最も基本的な操作用構造体とそれに基づくコンテナ型を自動定義する。
	template<typename VALUE_TYPE>
	class simpleContainer
	{
	public:
		//リングバッファ操作用構造体
		struct ope : public baseOpe<ope, VALUE_TYPE>{};

		//基本型定義
		DECLARE_OPE_TYPES(ope);

		//リングバッファコンテナ
		class con : public container<ope_type>
		{
		public:
		#ifdef GASHA_HAS_INHERITING_CONSTRUCTORS
			using container<ope_type>::container;//継承コンストラクタ
		#else//GASHA_HAS_INHERITING_CONSTRUCTORS
			//コンストラクタ
			template<std::size_t N>
			inline con(value_type(&array)[N], const int size = 0, const autoClearAttr_t auto_clear_attr = NEVER_CLEAR) :
				container<ope_type>(array, size, auto_clear_attr)
			{}
			inline con(value_type* array, const size_type max_size, const int size = 0, const autoClearAttr_t auto_clear_attr = NEVER_CLEAR) :
				container<ope_type>(array, max_size, size, auto_clear_attr)
			{}
			inline con(void* buff_ptr, const size_type buff_size, const int size = 0, const autoClearAttr_t auto_clear_attr = NEVER_CLEAR) :
				container<ope_type>(buff_ptr, buff_size, size, auto_clear_attr)
			{}
			//デフォルトコンスタラクタ
			inline con() :
				container<ope_type>()
			{}
		#endif//GASHA_HAS_INHERITING_CONSTRUCTORS
			//デストラクタ
			inline ~con()
			{}
		};
	};

	//--------------------
	//基本型定義マクロ消去
	#undef DECLARE_OPE_TYPES
}//namespace ring_buffer

//--------------------
//クラスの別名
//※ネームスペースの指定を省略してクラスを使用するための別名

//リングバッファ操作用テンプレート構造体
template<class OPE_TYPE, typename VALUE_TYPE>
using rBuff_baseOpe = ring_buffer::baseOpe<OPE_TYPE, VALUE_TYPE>;

//リングバッファコンテナ
template<class OPE_TYPE>
using rBuff = ring_buffer::container<OPE_TYPE>;

//シンプルリングバッファコンテナ
template<typename VALUE_TYPE>
using simpleRBuff = ring_buffer::simpleContainer<VALUE_TYPE>;

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/ring_buffer.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_RING_BUFFER_ALLWAYS_TOGETHER_CPP_H
#include <gasha/ring_buffer.cpp.h>
#endif//GASHA_RING_BUFFER_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_RING_BUFFER_H

// End of file

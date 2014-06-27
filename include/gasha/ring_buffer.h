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
//   ・ring_buffer.inl   ... 【インライン関数／テンプレート関数定義部】
//                            コンテナクラスの操作が必要な場所でインクルード。
//   ・ring_buffer.cpp.h ... 【関数定義部】
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

#include <gasha/sort_basic.h>//ソート処理基本
#include <gasha/search_basic.h>//探索処理基本

#include <cstddef>//std::size_t, std::ptrdiff_t
//#include <cstdint>//std::intptr_t

//【VC++】例外を無効化した状態で <iterator> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <iterator>//std::iterator


//移動予定
#include <gasha/is_ordered.h>//整列状態確認
#include <gasha/intro_sort.h>//イントロソート
#include <gasha/insertion_sort.h>//挿入ソート

#include <gasha/linear_search.h>//線形探索
#include <gasha/binary_search.h>//二分探索

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
	//  //struct baseOpe_t;
	//  //struct 派生構造体名 : public ring_buffer::baseOpe_t<派生構造体, 要素型>
	//	struct ope_t : public ring_buffer::baseOpe_t<ope_t, data_t>
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
	struct baseOpe_t
	{
		//型
		typedef OPE_TYPE ope_type;//要素操作型
		typedef VALUE_TYPE value_type;//要素型

		//ロック型
		typedef dummySharedLock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※共有ロック（リード・ライトロック）でコンテナ操作をスレッドセーフにしたい場合は、
		//　baseOpe_tの派生クラスにて、有効なロック型（sharedSpinLock など）を
		//　lock_type 型として再定義する。

		//デストラクタ呼び出し
		inline static void callDestructor(value_type* obj){ obj->~VALUE_TYPE(); }
		//※デストラクタの呼び出しを禁止したい場合、
		//　baseOpe_tの派生クラスにて、なにもしない
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
		typedef std::ptrdiff_t difference_type; \
		typedef std::size_t size_type; \
		typedef std::size_t index_type; \
		typedef typename ope_type::lock_type lock_type;
	
	//----------------------------------------
	//コンテナ破棄時の要素の自動クリア属性
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
			//※コンパイラによって優先して参照する型があいまいになることを避けるための定義
			typedef typename container::value_type value_type;
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
			inline const iterator& operator++() const;
			inline const iterator& operator--() const;
			inline iterator& operator++();
			inline iterator& operator--();
			inline const iterator operator++(int) const;
			inline const iterator operator--(int) const;
			inline iterator operator++(int);
			inline iterator operator--(int);
			inline const iterator& operator+=(const int rhs) const;
			inline const iterator& operator+=(const std::size_t rhs) const { return operator+=(static_cast<int>(rhs)); }
			inline const iterator& operator-=(const int rhs) const;
			inline const iterator& operator-=(const std::size_t rhs) const { return operator-=(static_cast<int>(rhs)); }
			inline iterator& operator+=(const int rhs);
			inline iterator& operator+=(const std::size_t rhs) { return operator+=(static_cast<int>(rhs)); }
			inline iterator& operator-=(const int rhs);
			inline iterator& operator-=(const std::size_t rhs) { return operator-=(static_cast<int>(rhs)); }
			inline const iterator operator+(const int rhs) const;
			inline const iterator operator+(const std::size_t rhs) const { return operator+(static_cast<int>(rhs)); }
			inline const iterator operator-(const int rhs) const;
			inline const iterator operator-(const std::size_t rhs) const { return operator-(static_cast<int>(rhs)); }
			inline iterator operator+(const int rhs);
			inline iterator operator+(const std::size_t rhs) { return operator+(static_cast<int>(rhs)); }
			inline iterator operator-(const int rhs);
			inline iterator operator-(const std::size_t rhs) { return operator-(static_cast<int>(rhs)); }
			inline int operator-(const iterator& rhs) const;
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
			inline void addIndexAndUpdate(const int add) const;
		public:
			//ムーブオペレータ
			inline iterator& operator=(const iterator&& rhs);
			inline iterator& operator=(const reverse_iterator&& rhs);
			//コピーオペレータ
			inline iterator& operator=(const iterator& rhs);
			inline iterator& operator=(const reverse_iterator& rhs);
		public:
			//ムーブコンストラクタ
			inline iterator(const iterator&& obj);
			inline iterator(const reverse_iterator&& obj);
			//コピーコンストラクタ
			inline iterator(const iterator& obj);
			inline iterator(const reverse_iterator& obj);
		public:
			//コンストラクタ
			inline iterator(const container& con, const bool is_end);
			inline iterator(const container& con, const index_type logical_index);
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
		class reverse_iterator : public std::iterator<std::random_access_iterator_tag, value_type>
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
			inline const reverse_iterator& operator++() const;
			inline const reverse_iterator& operator--() const;
			inline reverse_iterator& operator++();
			inline reverse_iterator& operator--();
			inline const reverse_iterator operator++(int) const;
			inline const reverse_iterator operator--(int) const;
			inline reverse_iterator operator++(int);
			inline reverse_iterator operator--(int);
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
			inline int operator-(const reverse_iterator& rhs) const;
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
			inline void addIndexAndUpdate(const int add) const;
		public:
			//ベースを取得
			inline const iterator base() const;
			inline iterator base();
		public:
			//ムーブオペレータ
			inline reverse_iterator& operator=(const reverse_iterator&& rhs);
			inline reverse_iterator& operator=(const iterator&& rhs);
			//コピーオペレータ
			inline reverse_iterator& operator=(const reverse_iterator& rhs);
			inline reverse_iterator& operator=(const iterator& rhs);
		public:
			//ムーブコンストラクタ
			inline reverse_iterator(const reverse_iterator&& obj);
			inline reverse_iterator(const iterator&& obj);
			//コピーコンストラクタ
			inline reverse_iterator(const reverse_iterator& obj);
			inline reverse_iterator(const iterator& obj);
		public:
			//コンストラクタ
			inline reverse_iterator(const container& con, const bool is_end);
			inline reverse_iterator(const container& con, const index_type logical_index);
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
		inline int _adjLogicalIndex(const int logical_index) const { return logical_index >= 0 && logical_index < m_maxSize ? logical_index : INVALID_INDEX; }//論理インデックスを範囲内に補正
		inline int _refRealIndex(const value_type* node) const{ return node - _refFront(); }//要素を物理インデックスに変換 ※範囲チェックなし
		inline int _refLogicalIndex(const value_type* node) const{ return _toLogicalIndex(_refRealIndex(node)); }//要素を論理インデックスに変換 ※範囲チェックなし
	public:
		//メソッド：要素アクセス系（独自拡張版）
		//※範囲チェックあり（公開）
		//※取扱い注意（コンテナアダプタ以外からの利用は非推奨）
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
		inline int refLogicalIndex(const value_type* node) const{ return _adjLogicalIndex(_refLogicalIndex(node)); }//要素を論理インデックスに変換
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
		template<size_type N>
		inline void assignArray(value_type(&array)[N], const int size = 0)
		{
			assignArray(array, N, size);
		}
		//※ポインタと配列要素数指定版
		void assignArray(value_type* array, const size_type max_size, const int size = 0)
		{
			if (m_array && m_autoClearAttr == AUTO_CLEAR)
				clear();//クリア
			m_array = array;
			m_maxSize = max_size;
			m_size = size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size);
			m_offset = 0;
		}
		//※voidポインタとバッファサイズ数指定版
		void assignArray(void* buff_ptr, const size_type buff_size, const int size = 0)
		{
			assignArray(static_cast<value_type*>(buff_ptr), buff_size / sizeof(value_type), size);
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
					value_type* value = _refElement(index);
					*value = new_value;//新しい値を初期化
				}
			}
			else if (_size < m_size)
			{
				for (index_type index = _size; index < m_size; ++index)
				{
					value_type* value = _refElement(index);
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
					value_type* value = _refElement(index);
					new(value)value_type(args...);//コンストラクタ呼び出し
				}
			}
			else if (_size < m_size)
			{
				for (index_type index = size; index < m_size; ++index)
				{
					value_type* value = _refElement(index);
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
		size_type resizeSilently(const int size)
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
			//		value_type* value = _refElement(index);
			//		ope_type::callDestructor(value);//デストラクタ呼び出し
			//		operator delete(value, value);//（作法として）deleteオペレータ呼び出し
			//	}
			//}
			{
				for (index_type index = 0; index < _size; ++index)
				{
					value_type* value = _refElement(index);
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
					value_type* value = _refElement(index);
					ope_type::callDestructor(value);//デストラクタ呼び出し
					operator delete(value, value);//（作法として）deleteオペレータ呼び出し
				}
			}
			{
				for (index_type index = 0; index < _size; ++index)
				{
					value_type* value = _refElement(index);
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
			value_type* obj = refFrontNew();//サイズチェック含む
			if (!obj)
				return nullptr;
			*obj = std::move(src);
			++m_size;
			m_offset = m_offset == 0 ? m_maxSize - 1 : m_offset - 1;
			return obj;
		}
		inline value_type* push_front(const value_type& src)//コピー版
		{
			value_type* obj = refFrontNew();//サイズチェック含む
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
			value_type* obj = refFrontNew();//サイズチェック含む
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
			value_type* obj = refBackNew();//サイズチェック含む
			if (!obj)
				return nullptr;
			*obj = std::move(src);
			++m_size;
			return obj;
		}
		inline value_type* push_back(const value_type& src)//コピー版
		{
			value_type* obj = refBackNew();//サイズチェック含む
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
			value_type* obj = refBackNew();//サイズチェック含む
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
			value_type* value = const_cast<value_type*>(refFront());
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
			value_type* obj = const_cast<value_type*>(refFront());
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
			value_type* value = const_cast<value_type*>(refFront());
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
			value_type* obj = const_cast<value_type*>(refBack());
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
				value_type* value = _refElement(i);
				ope_type::callDestructor(value);//デストラクタ呼び出し
				operator delete(value, value);//（作法として）deleteオペレータ呼び出し
			}
			m_size = 0;
			m_offset = 0;
		}
	private:
		//要素の移動（昇順）
		void moveAsc(const index_type dst_pos, const index_type src_pos, const size_type num)
		{
			index_type _dst_pos = dst_pos;
			index_type _src_pos = src_pos;
			for (size_type i = 0; i < num; ++i)
			{
				value_type* dst = _refElement(_dst_pos);
				value_type* src = _refElement(_src_pos);
				if (_dst_pos >= m_size)
					new(dst)value_type(std::move(*src));//ムーブコンストラクタ
				else
					*dst = std::move(*src);//ムーブオペレータ
				++_dst_pos;
				++_src_pos;
			}
		}
		//要素の移動（降順）
		void moveDesc(const index_type dst_pos, const index_type src_pos, const size_type num)
		{
			index_type _dst_pos = dst_pos + num - 1;
			index_type _src_pos = src_pos + num - 1;
			for (size_type i = 0; i < num; ++i)
			{
				value_type* dst = _refElement(_dst_pos);
				value_type* src = _refElement(_src_pos);
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
				return ite;
			}
			index_type index = pos.getIndex();
			const size_type remain = m_maxSize - m_size;
			const size_type _num = num < 0 || static_cast<size_type>(num) > remain ? remain : static_cast<size_type>(num);
			//移動
			moveDesc(index + _num, index, _num);
			//要素数変更
			m_size += _num;
			//挿入
			index_type _index = index;
			for (size_type i = 0; i < _num; ++i)
			{
				value_type* new_value = _refElement(_index);
				*new_value = value;
				++_index;
			}
			//終了
			iterator now(*this, index);
			return now;
		}
		//※コンストラクタ呼び出し版
		template<typename... Tx>
		iterator insert(iterator pos, const int num, Tx... args)
		{
			if (pos.isNotEnabled() || num == 0 || m_size == m_maxSize)
			{
				iterator ite(*this, INVALID_INDEX);
				return ite;
			}
			index_type index = pos.getIndex();
			const size_type remain = m_maxSize - m_size;
			const size_type _num = num < 0 || static_cast<size_type>(num) > remain ? remain : static_cast<size_type>(num);
			const size_type move_num = m_size - index;
			//移動
			moveDesc(index + _num, index, move_num);
			//要素数変更
			m_size += _num;
			//挿入
			index_type _index = index;
			for (size_type i = 0; i < _num; ++i)
			{
				value_type* new_value = _refElement(_index);
				new(new_value)value_type(args...);
				++_index;
			}
			//終了
			iterator now(*this, index);
			return now;
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
				value_type* delete_value = _refElement(_index);
				ope_type::callDestructor(delete_value);//デストラクタ呼び出し
				operator delete(delete_value, delete_value);//（作法として）deleteオペレータ呼び出し
				++_index;
			}
			//移動
			moveAsc(index, index + _num, move_num);
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
				return ite;
			}
			const index_type index = pos.getIndex();
			//削除
			_erase(index, num);
			//終了
			iterator now(*this, index);
			return now;
		}
		//※範囲指定版
		iterator erase(iterator start, iterator end)
		{
			if (start.isNotExist() || end.isNotExist() || start >= end || m_size == 0)
			{
				iterator ite(*this, INVALID_INDEX);
				return ite;
			}
			index_type index = start.getIndex();
			index_type end_index = end.getIndex();
			const size_type num = end_index - index;
			//削除
			_erase(index, num);
			//終了
			iterator now(*this, index);
			return now;
		}
	public:
		//ソート
		//※シェルソートを使用
		//※ope_type::predicateForSort() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		void sort()
		{
			GASHA_ iteratorIntroSort(begin(), end(), typename ope_type::predicateForSort());
		}
		//※プレディケート関数指定版
		template<class PREDICATE>
		void sort(PREDICATE predicate)
		{
			GASHA_ iteratorIntroSort(begin(), end(), predicate);
		}
		//安定ソート
		//※挿入ソートを使用
		//※ope_type::predicateForSort() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		void stableSort()
		{
			GASHA_ iteratorInsertionSort(begin(), end(), typename ope_type::predicateForSort());
		}
		//※プレディケート関数指定版
		template<class PREDICATE>
		void stableSort(PREDICATE predicate)
		{
			GASHA_ iteratorInsertionSort(begin(), end(), predicate);
		}
		//ソート済み状態チェック
		//※ope_type::predicateForSort() を使用して探索（標準では、データ型の operator<() に従って探索）
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		bool isOrdered() const
		{
			return GASHA_ iteratorIsOrdered(begin(), end(), typename ope_type::predicateForSort());
		}
		//※プレディケート関数指定版
		template<class PREDICATE>
		bool isOrdered(PREDICATE predicate) const
		{
			return GASHA_ iteratorIsOrdered(begin(), end(), predicate);
		}
	public:
		//線形探索
		//※探索値指定版
		//※ope_type::predicateForFind() を使用して探索（標準では、データ型の operator==() に従って探索）
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		template<typename V>
		iterator findValue(const V& value)
		{
			iterator found = GASHA_ iteratorLinearSearchValue(begin(), end(), value, typename ope_type::predicateForFind());
			return found;
		}
		//※比較関数＋値指定版
		template<typename V, class PREDICATE>
		iterator findValue(const V& value, PREDICATE predicate)
		{
			iterator found = GASHA_ iteratorLinearSearchValue(begin(), end(), value, predicate);
			return found;
		}
		//※比較関数指定版
		//※値の指定は関数に含んでおく（クロ―ジャを用いるなどする）
		template<class PREDICATE>
		iterator find(PREDICATE predicate)
		{
			iterator found = GASHA_ iteratorLinearSearch(begin(), end(), predicate);
			return found;
		}
		//二分探索
		//※探索値指定版
		//※ope_type::comparisonForSearch() を使用して探索（標準では、データ型の operator==() と operator<() に従って探索）
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある
		template<typename V>
		iterator binarySearchValue(const V& value)
		{
			iterator found = GASHA_ iteratorBinarySearchValue(begin(), end(), value, typename ope_type::comparisonForSearch());
			return found;
		}
		//※比較関数＋値指定版
		template<typename V, class COMPARISON>
		iterator binarySearchValue(const V& value, COMPARISON comparison)
		{
			iterator found = GASHA_ iteratorBinarySearchValue(begin(), end(), value, comparison);
			return found;
		}
		//※比較関数指定版
		//※値の指定は関数に含んでおく（クロ―ジャを用いるなどする）
		template<class COMPARISON>
		iterator binary_search(COMPARISON comparison)
		{
			iterator found = GASHA_ iteratorBinarySearch(begin(), end(), comparison);
			return found;
		}
	public:
		//コンストラクタ
		//※初期状態で使用中の要素数を指定する（-1で全要素を使用中にする）
		//※要素の初期化は行わない（必要なら size に 0 を指定して、後で resize() を呼び出す）
		template<size_type N>
		container(value_type(&array)[N], const int size = 0, const autoClearAttr_t auto_clear_attr = NEVER_CLEAR) :
			m_array(array),
			m_maxSize(N),
			m_size(size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size)),
			m_offset(0),
			m_autoClearAttr(auto_clear_attr)
		{}
		//※ポインタと配列要素数指定版
		container(value_type* array, const size_type max_size, const int size = 0, const autoClearAttr_t auto_clear_attr = NEVER_CLEAR) :
			m_array(array),
			m_maxSize(max_size),
			m_size(size < 0 || static_cast<size_type>(size) >= m_maxSize ? m_maxSize : static_cast<size_type>(size)),
			m_offset(0),
			m_autoClearAttr(auto_clear_attr)
		{}
		//※voidポインタとバッファサイズ数指定版
		container(void* buff_ptr, const size_type buff_size, const int size = 0, const autoClearAttr_t auto_clear_attr = NEVER_CLEAR) :
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
		autoClearAttr_t m_autoClearAttr;//コンテナ破棄時に残っている要素の自動クリア属性
		mutable lock_type m_lock;//ロックオブジェクト
	};
	
	//--------------------
	//基本型定義マクロ消去
	#undef DECLARE_OPE_TYPES
}//namespace ring_buffer

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルードする場合
#ifdef GASHA_RING_BUFFER_ALLWAYS_TOGETHER_INL
#include <gasha/ring_buffer.inl>
#endif//GASHA_RING__BUFFER_ALLWAYS_TOGETHER_INL

//.hファイルのインクルードに伴い、常に.cp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_RING_BUFFER_ALLWAYS_TOGETHER_CPP_H
#include <gasha/ring_buffer.cpp.h>
#endif//GASHA_RING_BUFFER_ALLWAYS_TOGETHER_CPP_H

#endif//__RING_BUFFER_H_

// End of file

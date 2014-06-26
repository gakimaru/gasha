#pragma once
#ifndef __RB_TREE_H_
#define __RB_TREE_H_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// rb_tree.h
// 赤黒木コンテナ【宣言部】※データのメモリ管理を行わない擬似コンテナ
//
// ※コンテナをインスタンス化する際は、別途下記のファイルをインクルードする必要あり
//
//   ・rb_tree.inl   ... 【インライン関数／テンプレート関数定義部】
//                            コンテナクラスの操作が必要な場所でインクルード。
//   ・rb_tree.cpp.h ... 【関数定義部】
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
//赤黒木（red-black tree）
//--------------------------------------------------------------------------------
//データ構造とアルゴリズム
//【特徴】
//・二分探索木である。
//	  - ノードの左側の子には、キーの値が小さいノードを連結。
//	  - ノードの右側の子には、キーの値が大きいか等しいノードを連結。
//・平衡木である。
//	  - 常に左右の木のバランスを保つ。
//・以上の特徴により、探索・追加・削除の時間が、常にO(log n)に保たれる。
//【条件】
//・条件①：各ノードは「赤」か「黒」の「色」を持つ。
//・条件②：「根」（root）は必ず「黒」。
//・条件③：「赤」の子は必ず「黒」。
//	          - 対偶により、「赤」の親は必ず「黒」。
//	          - 「黒」の子は「赤」でも「黒」でも良い。
//・条件④：「根」から「葉」までの経路上にある「黒」の数は、
//　　　　　あらゆる経路で一定。
//            - 「葉」とは、末端のnullptrであるものとする。
//            - 「葉」（nullptr）の色は「黒」であるものとする。
//              つまり、「赤」の子がnullptrであっても良い。
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//【本プログラムにおける実装要件】
//・メモリ節約のために、親ノードへのリンクを持たないデータ構造とする。
//・代わりに、検索等の関数実行時に外部からスタックを渡す事で親を辿ることが可能。
//・テンプレートにより、アルゴリズムを汎用化。
//・キー重複（同キーのノードが複数ある状態）を許容するアルゴリズムとする。
//・リンクの方法に柔軟性を持たせ、かつ、virtualを必須としないように、
//　データ構造は自由とする。
//　（リンクの変数をポインタにしようがインデックスにしようが自由）
//・代わりに、データ操作専用のクラスをユーザー定義することで、
//　処理に汎用性を持たせる。
//・一切メモリ操作は行わず、ノードのオブジェクトを受け取って操作する
//　アルゴリズムのみを提供する。
//・std::mapを模したコンテナとイテレータを利用可能とする。
//・コンテナは、根ノードのみを保持し、一切メモリ操作しないものとする。
//・イテレータは、上述のスタック操作を隠ぺいする。
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//【具体的な活用の想定】※以下、メモリ管理処理に適用することを想定した要件。
//・メモリマネージャの管理情報の連結に使用することを想定。
//・空きメモリサイズ順の連結を行うため、キー重複を許容する必要がある。
//・また、連結リストが肥大化することを避けるため、親ノードへのリンク情報は
//　持たない。
//・必要量以上の空きメモリのノードを検索したのち、アラインメントを加味した
//　上で利用可能なノードかを確認し、一つずつ大きなノードを辿って最適な
//　ノードを探し出す。
//・ノード間のリンク情報は、64bitポインタ変数による肥大化が起こらないように、
//　メモリ空間の先頭からオフセットされた32bitの相対ポインタを扱う。
//・一つのメモリ管理情報を、2種類の木に振り分けて使用する。
//　削除済みメモリノードの二分探索木はサイズをキーにし、
//　使用中メモリノードの二分探索木はポインタをキーにする。
//・このような利用を可能とするために、データ構造とアルゴリズムを完全に
//　切り離した構成にする。
//--------------------------------------------------------------------------------

//#include <memory.h>//memcpy用
//#include <assert.h>//assert用
//#include <cstddef>//srd::size_t用
//#include <iterator>//std::iterator用
//#include <algorithm>//C++11 std::move用

namespace rb_tree
{
#if 0
	//--------------------
	//赤黒木ノード操作用基底テンプレートクラス
	//※下記のような派生クラス（CRTP）を定義して使用する
	//  //struct クラス名 : public rb_tree::baseOpe_t<クラス名, ノード型, キー型, スタックの最大の深さ = 32>
	//	struct ope_t : public rb_tree::baseOpe_t<ope_t, data_t, int>
	//	{
	//		//子ノードを取得
	//		inline static const node_type* getChildL(const node_type& node){ return ???; }//大（右）側
	//		inline static const node_type* getChildS(const node_type& node){ return ???; }//小（左）側
	//		//子ノードを変更
	//		inline static void setChildL(node_type& node, const node_type* child){ ??? = child; }//大（右）側
	//		inline static void setChildS(node_type& node, const node_type* child){ ??? = child; }//小（左）側
	//		
	//		//ノードの色を取得
	//		inline static color_t getColor(const node_type& node){ return ??? ? BLACK : RED; }
	//		//ノードの色を変更
	//		inline static void setColor(node_type& node, const color_t color){ ??? = color == BLACK; }
	//		
	//		//キーを取得
	//		inline static key_type getKey(const node_type& node){ return ???; }
	//		
	//		//キーを比較 ※必要に応じて定義
	//		inline static int compareKey(const key_type lhs, const key_type rhs){ return ???; }
	//		
	//		//ロック型 ※必要に応じて定義
	//		//※共有ロック（リード・ライトロック）でコンテナ操作をスレッドセーフにしたい場合は、
	//		//　有効な共有ロック型（shared_spin_lockなど）を lock_type 型として定義する。
	//		typedef shared_spin_lock lock_type;//ロックオブジェクト型
	//	};
	template<class OPE_TYPE, typename NODE_TYPE, typename KEY_TYPE, int _STACK_DEPTH_MAX = 32>
	struct baseOpe_t
	{
		//定数
		static const int STACK_DEPTH_MAX = _STACK_DEPTH_MAX;//スタックの最大の深さ
		enum color_t//色
		{
			RED = 0,//赤
			BLACK = 1,//黒
		};

		//型
		typedef OPE_TYPE ope_type;//ノード操作型
		typedef NODE_TYPE node_type;//ノード型
		typedef KEY_TYPE key_type;//キー型

		//ロック型
		typedef dummy_shared_lock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※共有ロック（リード・ライトロック）でコンテナ操作をスレッドセーフにしたい場合は、
		//　baseOpe_tの派生クラスにて、有効な共有ロック型（shared_spin_lock など）を
		//　lock_type 型として再定義する。

		//子ノードを取得 ※const外し(remove_const)
		inline static node_type* getChildL_rc(node_type& node){ return const_cast<node_type*>(ope_type::getChildL(const_cast<const node_type&>(node))); }//大（右）側
		inline static node_type* getChildS_rc(node_type& node){ return const_cast<node_type*>(ope_type::getChildS(const_cast<const node_type&>(node))); }//小（左）側
		//子ノードを取得
		inline static const node_type* getChild(const node_type& node, const bool child_is_large)
		{
			return child_is_large ?
				ope_type::getChildL(node) ://大（右）側
				ope_type::getChildS(node); //小（左）側
		}
		inline static node_type* getChild_rc(node_type& node, const bool child_is_large){ return const_cast<node_type*>(getChild(const_cast<const node_type&>(node), child_is_large)); }
		//子ノードを変更
		inline static void setChild(node_type& node, const bool child_is_large, const node_type* child)
		{
			if (child_is_large)
				ope_type::setChildL(node, child);//大（右）側
			else
				ope_type::setChildS(node, child);//小（左）側
		}

		//キーを比較
		//※デフォルト
		//Return value:
		//  0     ... lhs == rhs
		//  1以上 ... lhs > rhs
		// -1以下 ... lhs < rhs
		inline static int compareKey(const key_type lhs, const key_type rhs)
		{
			return static_cast<int>(lhs) - static_cast<int>(rhs);
		}

		//ノードとキーを比較
		inline static int compare(const node_type& lhs, const node_type& rhs)
		{
			return ope_type::compareKey(ope_type::getKey(lhs), ope_type::getKey(rhs));
		}
		inline static int compare(const node_type& lhs, const key_type rhs)
		{
			return ope_type::compareKey(ope_type::getKey(lhs), rhs);
		}
		inline static int compare(const key_type lhs, const node_type& rhs)
		{
			return ope_type::compareKey(lhs, ope_type::getKey(rhs));
		}
		inline static int compare(const key_type lhs, const key_type rhs)
		{
			return ope_type::compareKey(lhs, rhs);
		}
		inline static bool eq(const node_type& lhs, const node_type& rhs){ return compare(lhs, rhs) == 0; }
		inline static bool eq(const node_type& lhs, const key_type rhs){ return compare(lhs, rhs) == 0; }
		inline static bool eq(const key_type lhs, const key_type rhs){ return compare(lhs, rhs) == 0; }
		inline static bool ne(const node_type& lhs, const node_type& rhs){ return compare(lhs, rhs) != 0; }
		inline static bool ne(const node_type& lhs, const key_type rhs){ return compare(lhs, rhs) != 0; }
		inline static bool ne(const key_type lhs, const key_type rhs){ return compare(lhs, rhs) != 0; }
		inline static bool gt(const node_type& lhs, const node_type& rhs){ return compare(lhs, rhs) > 0; }
		inline static bool gt(const node_type& lhs, const key_type rhs){ return compare(lhs, rhs) > 0; }
		inline static bool gt(const key_type lhs, const key_type rhs){ return compare(lhs, rhs) > 0; }
		inline static bool ge(const node_type& lhs, const node_type& rhs){ return compare(lhs, rhs) >= 0; }
		inline static bool ge(const node_type& lhs, const key_type rhs){ return compare(lhs, rhs) >= 0; }
		inline static bool ge(const key_type lhs, const key_type rhs){ return compare(lhs, rhs) >= 0; }
		inline static bool lt(const node_type& lhs, const node_type& rhs){ return compare(lhs, rhs) < 0; }
		inline static bool lt(const node_type& lhs, const key_type rhs){ return compare(lhs, rhs) < 0; }
		inline static bool lt(const key_type lhs, const key_type rhs){ return compare(lhs, rhs) < 0; }
		inline static bool le(const node_type& lhs, const node_type& rhs){ return compare(lhs, rhs) <= 0; }
		inline static bool le(const node_type& lhs, const key_type rhs){ return compare(lhs, rhs) <= 0; }
		inline static bool le(const key_type lhs, const key_type rhs){ return compare(lhs, rhs) <= 0; }

		//色判定
		inline static bool isBlack(const node_type& node){ return ope_type::getColor(node) == BLACK; }
		inline static bool isRed(const node_type& node){ return ope_type::getColor(node) == RED; }
		//色更新
		inline static void setBlack(node_type& node){ ope_type::setColor(node, BLACK); }
		inline static void setRed(node_type& node){ ope_type::setColor(node, RED); }
	};
	//--------------------
	//基本型定義マクロ
	#define DECLARE_OPE_TYPES(OPE_TYPE) \
		typedef OPE_TYPE ope_type; \
		typedef typename ope_type::node_type node_type; \
		typedef typename ope_type::key_type key_type; \
		typedef node_type value_type; \
		typedef value_type& reference; \
		typedef const value_type& const_reference; \
		typedef value_type* pointer; \
		typedef const value_type* const_pointer; \
		typedef std::size_t size_type; \
		typedef stack_t<ope_type> stack_type; \
		typedef typename stack_type::info_t stack_info_type; \
		typedef typename ope_type::lock_type lock_type;
	//----------------------------------------
	//デバッグ用補助関数
#ifdef DEBUG_PRINT_FOR_ADD
	template<typename... Tx>
	inline int printf_dbg_add(const char* fmt, Tx... args)
	{
		return printf(fmt, args...);
	}
#else//DEBUG_PRINT_FOR_ADD
	inline int printf_dbg_add(const char* fmt, ...){ return 0; }
#endif//DEBUG_PRINT_FOR_ADD
#ifdef DEBUG_PRINT_FOR_REMOVE
	template<typename... Tx>
	inline int printf_dbg_remove(const char* fmt, Tx... args)
	{
		return printf(fmt, args...);
	}
#else//DEBUG_PRINT_FOR_REMOVE
	inline int printf_dbg_remove(const char* fmt, ...){ return 0; }
#endif//DEBUG_PRINT_FOR_REMOVE
	//--------------------
	//赤黒木処理用スタッククラス
	//※赤黒木のノード情報を扱う
	template<class OPE_TYPE>
	class stack_t
	{
	public:
		//基本型
		typedef OPE_TYPE ope_type;
		typedef typename OPE_TYPE::node_type node_type;
	public:
		//定数
		static const int DEPTH_MAX = ope_type::STACK_DEPTH_MAX;//最大の深さ（スタック処理用）
	public:
		//型
		//ノード情報型
		struct info_t
		{
			const node_type* m_nodeRef;//ノード参照
			bool m_isLarge;//大（右）側に連結か？
		};
	public:
		//スタックにノード情報を記録
		info_t* push(const node_type* node, const bool is_large)
		{
			assert(m_depth < DEPTH_MAX);
			info_t* stack_node = &m_array[m_depth++];
			stack_node->m_nodeRef = node;
			stack_node->m_isLarge = is_large;
			return stack_node;
		}
		//スタックからノード情報を取得
		info_t* pop()
		{
			if (m_depth == 0)
				return nullptr;
			return &m_array[--m_depth];
		}
		//スタックの先頭のノード情報を参照
		//※要素が減らない
		info_t* top()
		{
			if (m_depth == 0)
				return nullptr;
			return &m_array[m_depth - 1];
		}
		//スタックの現在の深さを取得
		int getDepth() const { return m_depth; }
		//スタックの現在の深さを更新
		void setDepth(const int depth)
		{
			if (depth < 0)
				m_depth = 0;
			else if (depth < m_depth)
				m_depth = depth;
		}
		//スタックの現在の深さをリセット
		void reset()
		{
			m_depth = 0;
		}
		//スタックの現在の幅を算出
		//※「幅」＝スタックの現在の深さまでの大小連結の合計値を算出
		//※小側を-1、大側を+1として計算
		long long calcBreadth()
		{
			long long breadth = 0;
			for (int depth = 0; depth < m_depth; ++depth)
			{
				breadth *= 2ll;
				breadth += (m_array[depth].m_isLarge ? 1ll : 0ll);
			}
			return breadth;
		}
		//ムーブオペレータ
		inline stack_t& operator=(const stack_t&& rhs)
		{
			m_depth = rhs.m_depth;
			if (m_depth > 0)
				memcpy(m_array, rhs.m_array, sizeof(info_t)* m_depth);
			return *this;
		}
		//コピーオペレータ
		inline stack_t& operator=(const stack_t& rhs)
		{
			//return operator=(std::move(rhs));
			m_depth = rhs.m_depth;
			if (m_depth > 0)
				memcpy(m_array, rhs.m_array, sizeof(info_t)* m_depth);
			return *this;
		}
		//ムーブコンストラクタ
		inline stack_t(const stack_t&& obj) :
			m_depth(obj.m_depth)
		{
			if (m_depth > 0)
				memcpy(m_array, obj.m_array, sizeof(info_t)* m_depth);
		}
		//コピーコンストラクタ
		inline stack_t(const stack_t& obj) :
			m_depth(obj.m_depth)
		{
			if (m_depth > 0)
				memcpy(m_array, obj.m_array, sizeof(info_t)* m_depth);
		}
		//コンストラクタ
		inline stack_t() :
			m_depth(0)
		{}
	private:
		//フィールド
		info_t m_array[DEPTH_MAX];//ノード情報の配列（スタック）
		int m_depth;//スタックのカレントの深さ
	};
	//--------------------
	//赤黒木操作関数：最小ノード探索
	//※後からget**Node()やsearchNode()を実行できるように、スタックを渡す必要あり
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getSmallestNode(const typename OPE_TYPE::node_type* root, stack_t<OPE_TYPE>& stack)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		if (!root)
			return nullptr;
		const node_type* curr_node = root;//現在の探索ノード
		while (true)
		{
			const node_type* child_node = ope_type::getChildS(*curr_node);//小（左）側の子ノードを取得
			if (!child_node)//子が無ければ終了
				return  curr_node;
			stack.push(curr_node, false);//親ノードをスタックに記録
			curr_node = child_node;
		}
		return nullptr;
	}
	//--------------------
	//赤黒木操作関数：最大ノード探索
	//※後からget**Node()やsearchNode()を実行できるように、スタックを渡す必要あり
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getLargestNode(const typename OPE_TYPE::node_type* root, stack_t<OPE_TYPE>& stack)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		if (!root)
			return nullptr;
		const node_type* curr_node = root;//現在の探索ノード
		while (true)
		{
			const node_type* child_node = ope_type::getChildL(*curr_node);//大（右）側の子ノードを取得
			if (!child_node)//子が無ければ終了
				return  curr_node;
			stack.push(curr_node, true);//親ノードをスタックに記録
			curr_node = child_node;
		}
		return nullptr;
	}
	//--------------------
	//赤黒木操作関数：次ノード探索（カレントノードの次に大きいノードを探索）
	//※get**Node()やsearchNode()でカレントノードを取得した際のスタックを渡す必要あり
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getNextNode(const typename OPE_TYPE::node_type* curr_node, stack_t<OPE_TYPE>& stack)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		if (!curr_node)
			return nullptr;
		const node_type* child_node_l = ope_type::getChildL(*curr_node);//大（右）側の子ノードを取得
		if (child_node_l)//子がある場合
		{
			stack.push(curr_node, true);//親ノードをスタックに記録
			return getSmallestNode<ope_type>(child_node_l, stack);//子から一番小さい子孫ノードを取得
		}
		else//if(!child_node_l)//子がない場合
		{
			const stack_info_type* stack_node = nullptr;
			while (stack_node = stack.pop())//親ノードを取得
			{
				if (stack_node->m_isLarge == false)//小（左）側の子の親なら終了
					return stack_node->m_nodeRef;
			}
		}
		return nullptr;
	}
	//--------------------
	//赤黒木操作関数：前ノード探索（カレントノードの次に小さいノードを探索）
	//※get**Node()やsearchNode()でカレントノードを取得した際のスタックを渡す必要あり
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getPrevNode(const typename OPE_TYPE::node_type* curr_node, stack_t<OPE_TYPE>& stack)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		if (!curr_node)
			return nullptr;
		const node_type* child_node_s = ope_type::getChildS(*curr_node);//小（左）側の子ノードを取得
		if (child_node_s)//子がある場合
		{
			stack.push(curr_node, false);//親ノードをスタックに記録
			return getLargestNode<ope_type>(child_node_s, stack);//子から一番大きい子孫ノードを取得
		}
		else//if(!child_node_s)//子がない場合
		{
			const stack_info_type* stack_node = nullptr;
			while (stack_node = stack.pop())//親ノードを取得
			{
				if (stack_node->m_isLarge == true)//大（右）側の子の親なら終了
					return stack_node->m_nodeRef;
			}
		}
		return nullptr;
	}
	//--------------------
	//赤黒木操作関数：ノード探索（指定のキーと一致するノード、もしくは、指定のキーに最も近いノードを検索）
	//※後からget**Node()やsearchNode()を実行できるように、スタックを渡す必要あり
	//※一致ノードが複数ある場合、最小位置にあるノードを返す
	//※指定のキーの内輪で最も近いノードと同キーのノードが複数ある場合、その最後のノードを返す
	//※指定のキーより大きく最も近いノードと同キーのノードが複数ある場合、その最初のノードを返す
	enum match_type_t
	{
		FOR_MATCH = 0,//一致ノードを検索
		FOR_NEAREST_SMALLER,//一致ノード、もしくは、内輪で最も近いノードを検索
		FOR_NEAREST_LARGER,//一致ノード、もしくは、それより大きく最も近いノードを検索
	};
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* searchNode(const typename OPE_TYPE::node_type* root, const typename OPE_TYPE::key_type key, stack_t<OPE_TYPE>& stack, const match_type_t search_type = FOR_MATCH)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		if (!root)
			return nullptr;
		const node_type* match_node = nullptr;//一致ノード
		int match_stack_depth = -1;//一致ノード検出時のスタック位置
		const node_type* nearest_node = nullptr;//最近ノード
		int nearest_stack_depth = -1;//最近ノード検出時のスタック位置
		const node_type* curr_node = root;//現在の探索ノード
		while (true)
		{
			const int cmp = ope_type::compare(*curr_node, key);//指定のキーと大小比較
			if (cmp == 0)//指定のキーと一致
			{
				//※キー重複を許容するアルゴリズムのため、
				//　一致ノードを記録した上で検索を続行する
				match_node = curr_node;//一致ノードを記録
				match_stack_depth = stack.getDepth();//一致ノード検出時のスタック位置を記録
				stack.push(curr_node, false);//仮の親ノードとしてスタックに記録
				curr_node = ope_type::getChildS(*curr_node);//小（左）側の子の方に検索を続行
				if (!curr_node)//子が無ければ探索終了
					break;
			}
			else if (cmp < 0)//指定のキーより小さい
			{
				if (search_type == FOR_NEAREST_SMALLER && (!nearest_node || nearest_node && ope_type::le(*nearest_node, *curr_node)))
				{
					nearest_node = curr_node;//最近ノードを記録
					nearest_stack_depth = stack.getDepth();//最近ノード検出時のスタック位置を記録
				}
				const node_type* child_node = ope_type::getChildL(*curr_node);//大（右）側の子ノードを取得
				if (!child_node)//子が無ければ終了
					break;
				stack.push(curr_node, true);//仮の親ノードとしてスタックに記録
				curr_node = child_node;
			}
			else//if (cmp > 0)//指定のキーより大きい
			{
				if (search_type == FOR_NEAREST_LARGER && (!nearest_node || nearest_node && ope_type::ge(*nearest_node, *curr_node)))
				{
					nearest_node = curr_node;//最近ノードを記録
					nearest_stack_depth = stack.getDepth();//最近ノード検出時のスタック位置を記録
				}
				const node_type* child_node = ope_type::getChildS(*curr_node);//小（左）側の子ノードを取得
				if (!child_node)//子が無ければ終了
					break;
				stack.push(curr_node, false);//仮の親ノードとしてスタックに記録
				curr_node = child_node;
			}
		}
		if (match_node)//一致ノード検出時
		{
			stack.setDepth(match_stack_depth);//一致ノード検出時のスタック位置に戻す
			return match_node;//一致ノードを返す
		}
		else//if (!match_node)
		{
			if (nearest_node)//最近ノード検出時
			{
				stack.setDepth(nearest_stack_depth);//最近ノード検出時のスタック位置に戻す
				return nearest_node;//最近ノードを返す
			}
		}
		return nullptr;//検索失敗
	}
	//既存ノード探索 ※キーを指定する代わりに、既存のノードを渡して探索する
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* searchNode(const typename OPE_TYPE::node_type* root, const typename OPE_TYPE::node_type* node, stack_t<OPE_TYPE>& stack)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		const key_type key = ope_type::getKey(*node);//キーを取得
		node_type* target_node = const_cast<node_type*>(searchNode<ope_type>(root, key, stack));//キーが一致するノードを検索
		if (!target_node)//キーが一致するノードがなければ終了
			return nullptr;
		while (target_node != node && ope_type::eq(*target_node, key))//指定ノード（と同じアドレス）を検索
			target_node = const_cast<node_type*>(getNextNode<ope_type>(target_node, stack));
		if (target_node != node)
			return nullptr;
		return target_node;
	}
	//--------------------
	//赤黒木操作関数：木の最大深度を計測
	//※内部でスタックを作成
	//※-1でリストなし
	template<class OPE_TYPE>
	int getDepthMax(const typename OPE_TYPE::node_type* root)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		if (!root)
			return -1;
		int depth_max = 0;//最大の深さ
		stack_type stack;//スタックを作成
		const node_type* node = getSmallestNode<ope_type>(root, stack);//最小ノード取得
		while (node)
		{
			const int depth = stack.getDepth();//深さを取得
			depth_max = depth_max >= depth ? depth_max : depth;//最大の深さを更新
			node = getNextNode<ope_type>(node, stack);//次のノード取得
		}
		return depth_max;
	}
	//--------------------
	//赤黒木操作関数：木のノード数を計測
	template<class OPE_TYPE>
	std::size_t countNodes(const typename OPE_TYPE::node_type* node)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
	#if 0
		//再帰処理版
		return !node ? 0 : 1 + countNodes<ope_type>(ope_type::getChildL(*node)) +
		                       countNodes<ope_type>(ope_type::getChildS(*node));
	#else
		//ループ処理版
		//※再帰処理版よりもループ処理版の方が高速
		//　▼実際に処理時間を測定して確認
		//　　以下、ノード数100の計測を10000000回行った結果
		//　　・再帰処理版：　3.443212500 sec
		//　　・ループ処理版：1.663094300 sec
		int count = 0;
		const node_type* stack[stack_type::DEPTH_MAX];
		int stack_pos = 0;
		while (node)
		{
			while (node)
			{
				++count;
				stack[stack_pos++] = node;
				node = ope_type::getChildS(*node);
			}
			while (!node && stack_pos != 0)
				node = ope_type::getChildL(*stack[--stack_pos]);
		}
		return count;
	#endif
	}
	//--------------------
	//赤黒木操作関数：指定のキーのノード数を計測
	//※関数内でスタックを使用
	template<class OPE_TYPE>
	std::size_t countNodes(const typename OPE_TYPE::node_type* root, const typename OPE_TYPE::key_type key)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		if (!root)
			return 0;
		stack_type stack;
		const node_type* node = searchNode<ope_type>(root, key, stack, FOR_MATCH);
		if (!node)
			return 0;
		int count = 0;//ノード数
		while (node && ope_type::eq(*node, key))
		{
			++count;
			node = getNextNode<ope_type>(node, stack);
		}
		return count;
	}
	//--------------------
	//赤黒木操作関数：ノード追加時の赤黒バランス調整
	template<class OPE_TYPE>
	void _balanceForAdd(typename OPE_TYPE::node_type*& root, stack_t<OPE_TYPE>& stack, typename OPE_TYPE::node_type* curr_node, bool child_is_large, typename OPE_TYPE::node_type* child_node);
	//--------------------
	//赤黒木操作関数：ノードを追加
	//※関数内でスタックを使用
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* addNode(typename OPE_TYPE::node_type* new_node, typename OPE_TYPE::node_type*& root)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		if (!new_node)
			return nullptr;
		ope_type::setChildL(*new_node, nullptr);//新規ノードはリンクをクリア：大（右）側
		ope_type::setChildS(*new_node, nullptr);//新規ノードはリンクをクリア：小（左）側
		if (!root)//根ノードが未登録の場合
		{
			root = new_node;//根ノードに登録
		#ifndef DISABLE_COLOR_FOR_ADD
			ope_type::setBlack(*root);//根ノードは黒
		#endif//DISABLE_COLOR_FOR_ADD
			return new_node;//この時点で処理終了
		}
	#ifndef DISABLE_COLOR_FOR_ADD
		ope_type::setRed(*new_node);//新規ノードは暫定で赤
	#endif//DISABLE_COLOR_FOR_ADD
		key_type new_key = ope_type::getKey(*new_node);//新規ノードのキーを取得
		stack_type stack;//スタックを用意
		node_type* curr_node = root;//現在の探索ノード
		bool new_key_is_large = false;
		while (true)
		{
			new_key_is_large = ope_type::ge(new_key, *curr_node);//指定のキーと一致もしくは指定のキーの方が大きければtrue
			node_type* child_node = ope_type::getChild_rc(*curr_node, new_key_is_large);//子ノードを取得
			if (!child_node)//子ノードが無ければそこに新規ノードを追加して終了
			{
				ope_type::setChild(*curr_node, new_key_is_large, new_node);//子ノードとして新規ノードを追加
				break;
			}
			stack.push(curr_node, new_key_is_large);//親ノードをスタックに記録
			curr_node = child_node;
		}
	#ifndef DISABLE_COLOR_FOR_ADD
		//赤黒バランス調整
		_balanceForAdd<ope_type>(root, stack, curr_node, new_key_is_large, new_node);
	#endif//DISABLE_COLOR_FOR_ADD
		return new_node;
	}
	//--------------------
	//赤黒木操作関数：ノード削除時の赤黒バランス調整
	template<class OPE_TYPE>
	void _balanceForRemove(typename OPE_TYPE::node_type*& root, stack_t<OPE_TYPE>& stack, typename OPE_TYPE::node_type* removing_node, typename OPE_TYPE::node_type* replacing_node);
	//--------------------
	//赤黒木操作関数：ノードを削除
	//※関数内でスタックを二つ使用
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* removeNode(const typename OPE_TYPE::node_type* target_node, typename OPE_TYPE::node_type*& root)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		if (!target_node || !root)
			return nullptr;
		stack_type stack;//スタックを用意
		node_type* removing_node = const_cast<node_type*>(searchNode<ope_type>(root, target_node, stack));//削除ノードを検索してスタックを作る
		if (!removing_node)//検索に失敗したら終了
			return nullptr;
		//削除開始
		node_type* parent_node = nullptr;//削除ノードの親ノード
		bool curr_is_large = false;//削除ノードの親ノードからの連結方向
		{
			stack_info_type* parent_info = stack.top();//親ノード参照情報を取得
			if (parent_info)
			{
				parent_node = const_cast<node_type*>(parent_info->m_nodeRef);//親ノードを取得
				curr_is_large = parent_info->m_isLarge;//親ノードからの連結方向
			}
		}
		node_type* child_node_l = ope_type::getChildL_rc(*removing_node);//大（右）側の子ノードを取得
		node_type* child_node_s = ope_type::getChildS_rc(*removing_node);//小（左）側の子ノードを取得
		node_type* descendant_node = nullptr;//削除ノードの最大子孫ノード（削除ノードの次に小さいノード）
		node_type* replacing_node = nullptr;//削除ノードと置き換えるノード
		//削除ノードと置き換えるノードの選出処理
		//-------------------------------------------------------------------------
		//【木の説明の凡例】
		//・「(S)」は、小（左）側のノードを示す。
		//・「(L)」は、大（右）側のノードを示す。
		//・「(SL)」は、小（左）側の子の大（右）側のノードを示す。
		//　（「(LS)」[(SS)」「(SSL)」などのパターンも同様）
		//・「(null)」は、ノードがないことを示す。
		//-------------------------------------------------------------------------
		if (child_node_s && !child_node_l)
		{
			//削除ノードの小（左）側にのみ子ノードがある場合、それを置き換えノードとする
			//-------------------------------------------------------------------------
			//            .-------[removing_node]------.                               
			//   .--[replacing_node]--.             (null)                             
			// [(SS)]              [(SL)]                                              
			//-------------------------------------------------------------------------
			//                          ↓【置き換え】                                 
			//                 .--[replacing_node]--.                                  
			//               [(SS)]              [(SL)]                                
			//-------------------------------------------------------------------------
			replacing_node = child_node_s;//削除ノードと置き換えるノードをセット
		}
		else if (!child_node_s && child_node_l)
		{
			//削除ノードの大（右）側にのみ子ノードがある場合、それを置き換えノードとする
			//-------------------------------------------------------------------------
			//            .-------[removing_node]------.                               
			//          (null)              .--[replacing_node]--.                     
			//                            [(LS)]              [(LL)]                   
			//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			//                          ↓【置き換え】                                 
			//                 .--[replacing_node]--.                                  
			//               [(LS)]              [(LL)]                                
			//-------------------------------------------------------------------------
			replacing_node = child_node_l;//削除ノードと置き換えるノードをセット
		}
		else if (child_node_s && child_node_l)
		{
			//削除ノードの小（左）側と大（右）側の両方の子ノードがある場合、前ノード（最大子孫ノード）を置き換えノードとする
			//-------------------------------------------------------------------------
			//【ケース①】                                                             
			//            .---------------[removing_node]---------------.              
			//          [(S)]----.                                    [(L)]            
			//                   .(最大子孫)                                           
			//           .--[replacing_node]--.                                        
			//         [(SLS)]             (null)                                      
			//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			//                                 ↓【置き換え】                          
			//            .---------------[replacing_node]--------------.              
			//          [(S)]----.                                    [(L)]            
			//                   .(最大子孫)                                           
			//                [(SLS)]                                                  
			//-------------------------------------------------------------------------
			//【ケース②】                                                             
			//            .---------------[removing_node]---------------.              
			//    .--[replacing_node]--.                              [(L)]            
			//  [(SLS)]             (null)                                             
			//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			//                                 ↓【置き換え】                          
			//            .---------------[replacing_node]--------------.              
			//         [(SLS)]                                        [(L)]            
			//-------------------------------------------------------------------------
			stack_info_type* remove_info = stack.push(removing_node, false);//スタックに削除ノードを追加
			descendant_node = const_cast<node_type*>(getLargestNode<ope_type>(child_node_s, stack));//最大子孫ノードを取得
			replacing_node = descendant_node;//削除ノードと置き換えるノードをセット
			if (replacing_node != child_node_s)
			{
				//【ケース①：前ノード（最大子孫ノード）が削除ノードの小（左）側の子の最大子孫】
				stack_info_type* descendant_parent_info = stack.top();//最大子孫ノードの親ノードを取得
				node_type* descendant_parent_node = const_cast<node_type*>(descendant_parent_info->m_nodeRef);//最大子孫ノードの親ノードを取得
				ope_type::setChildL(*descendant_parent_node, ope_type::getChildS(*replacing_node));//最大子孫ノードの親ノードの大（右）側の子ノードを変更
				ope_type::setChildS(*replacing_node, child_node_s);//置き換えノードの小（左）側の子ノードを変更
			}
			else//if (descendant_node == child_node_s)
			{
				//【ケース②：前ノード（最大子孫ノード）が削除ノードの小（左）側の子】
				//なにもしない
			}
			ope_type::setChildL(*replacing_node, child_node_l);//置き換えノードの大（右）側の子ノードを変更
			remove_info->m_nodeRef = replacing_node;//スタック上の削除ノード参照を置き換えノード参照に書き換え
		}
		//else//if (!child_node_s && !child_node_l)
		//{
		//	//削除ノードの小（左）側と大（右）側の両方の子ノードがない場合、置き換えノードはnullptr
		//	replacing_node = nullptr;//削除ノードと置き換えるノードをセット
		//}
		//削除ノードの置き換え処理
		ope_type::setChildL(*removing_node, nullptr);
		ope_type::setChildS(*removing_node, nullptr);
		if (!parent_node)//親ノードがない場合 → 根ノードが削除された場合である
		{
			root = replacing_node;//根ノードを置き換え
			if (!root)//根ノードがなくなったら、この時点で処理終了
				return removing_node;
		}
		else//if(parent_node)//親ノードがある場合
		{
			ope_type::setChild(*parent_node, curr_is_large, replacing_node);//親ノードの子ノードを置き換え
		}
	#ifndef DISABLE_COLOR_FOR_REMOVE
		//赤黒バランス調整
		_balanceForRemove<ope_type>(root, stack, removing_node, replacing_node);
	#endif//DISABLE_COLOR_FOR_REMOVE
		return removing_node;
	}
	//--------------------
	//赤黒木操作関数：【汎用処理】ノード左回転処理
	//-------------------------------------------------------------------------
	//              .--------------[curr_node]--------------.                  
	//       [child_node_s]                     .----[child_node_l]            
	//                                    [child_node_ls]                      
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//                               ↓【左回転】                              
	//              .-------------[child_node_l]                               
	//      .-----[curr_node]----.                                             
	// [child_node_s]  [child_node_ls]                                         
	//-------------------------------------------------------------------------
	template<class OPE_TYPE>
	inline typename OPE_TYPE::node_type* _rotateL(typename OPE_TYPE::node_type* curr_node)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		node_type* child_node_l = ope_type::getChildL_rc(*curr_node);
		node_type* child_node_ls = ope_type::getChildS_rc(*child_node_l);
		ope_type::setChildS(*child_node_l, curr_node);
		ope_type::setChildL(*curr_node, child_node_ls);
		return child_node_l;
	};
	//--------------------
	//赤黒木操作関数：【汎用処理】ノード右回転処理
	//-------------------------------------------------------------------------
	//        .--------------[curr_node]--------------.                        
	// [child_node_s]----.                       [child_node_l]                
	//          [child_node_sl]                                                
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//                            ↓【右回転】                                 
	//                      [child_node_s]------------.                        
	//                                      .----[curr_node]----.              
	//                                 [child_node_sl]  [child_node_l]         
	//-------------------------------------------------------------------------
	template<class OPE_TYPE>
	inline typename OPE_TYPE::node_type* _rotateR(typename OPE_TYPE::node_type* curr_node)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		node_type* child_node_s = ope_type::getChildS_rc(*curr_node);
		node_type* child_node_sl = ope_type::getChildL_rc(*child_node_s);
		ope_type::setChildL(*child_node_s, curr_node);
		ope_type::setChildS(*curr_node, child_node_sl);
		return child_node_s;
	};
	//--------------------
	//赤黒木操作関数：ノード追加時の赤黒バランス調整
	template<class OPE_TYPE>
	void _balanceForAdd(typename OPE_TYPE::node_type*& root, stack_t<OPE_TYPE>& stack, typename OPE_TYPE::node_type* curr_node, bool child_is_large, typename OPE_TYPE::node_type* child_node)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		bool is_rotated = false;//回転処理済みフラグ
		bool is_balanced = false;//平衡状態検出フラグ
		while (true)
		{
			stack_info_type* parent_info = stack.pop();//スタックから親ノード情報取得
			if (!parent_info)
				break;
			node_type* parent_node = const_cast<node_type*>(parent_info->m_nodeRef);//親ノード取得
			bool curr_is_large = parent_info->m_isLarge;//親ノードからの大小連結情報取得
			if (is_rotated)//前回回転処理済みなら親ノードの子ノードを連結し直す
			{
				ope_type::setChild(*parent_node, curr_is_large, curr_node);
				is_rotated = false;//回転処理済み状態リセット
			}
			//回転処理
			//-------------------------------------------------------------------------
			//【木の説明の凡例】
			//・「:B」は、黒を示す。
			//・「:R」は、赤を示す。
			//・「:+B」は、黒に変更することを示す。
			//・「:+R」は、赤に変更することを示す。
			//・「(S)」は、小（左）側のノードを示す。
			//・「(L)」は、大（右）側のノードを示す。
			//・「(SL)」は、小（左）側の子の大（右）側のノードを示す。
			//　（「(LS)」[(SS)」「(SSL)」などのパターンも同様）
			//・「(null)」は、ノードがないことを示す。
			//-------------------------------------------------------------------------
			if (!is_balanced && ope_type::isBlack(*curr_node))//現在ノードが黒なら平衡状態検出（以後の条件違反チェックは不要）
				is_balanced = true;
			if (!is_balanced && ope_type::isRed(*child_node))//赤ノードが二つ連結していたら条件違反のため、回転処理
			{
				//【共通処理】親ノードを左回転処理
				//-------------------------------------------------------------------------
				//             .-------[parent_node:B]--------.                            
				//           [(S)]                    .--[curr_node:R]--.                  
				//                                  [(LS)]       [child_node:R]            
				//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				//                          ↓【左回転】                                   
				//          .-----------[curr_node:R]---------.                            
				//   .--[parent_node:B]--.               [child_node:R]                    
				// [(S)]              [(LS)]                                               
				//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				//                          ↓【変数と色を調整】                           
				//                                                  黒に                   
				//            .--------[parent_node:R]--------.      ↓                    
				//   .------[(S):B]------.                [curr_node:+B]                   
				// [(SS)]             [(SL)]                                               
				//-------------------------------------------------------------------------
				auto rotateParentL = [&]() -> const node_type*
				{
					parent_node = _rotateL<ope_type>(parent_node);//左回転処理
					curr_is_large = true;
					curr_node = child_node;
					ope_type::setBlack(*curr_node);//ノードを黒にする
					//child_is_large = true;
					//child_node = nullptr;
					return parent_node;
				};
				//【共通処理】親ノードを右回転処理
				//-------------------------------------------------------------------------
				//               .-----[parent_node:B]-----.                               
				//       .--[curr_node:R]--.             [(L)]                             
				// [child_node:R]       [(SL)]                                             
				//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				//                            ↓【右回転】                                 
				//               .-------[curr_node:R]-------.                             
				//          [child_node:R]         .--[parent_node:B]--.                   
				//                               [(SL)]              [(L)]                 
				//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				//                            ↓【変数と色を調整】                         
				//               .-------[parent_node:R]-----.                             
				//           [curr_node:+B]        .------[(L):B]------.                   
				//                      ↑       [(LS)]             [(LL)]                 
				//                     黒に                                                
				//-------------------------------------------------------------------------
				auto rotateParentR = [&]() -> const node_type*
				{
					parent_node = _rotateR<ope_type>(parent_node);//右回転処理
					curr_is_large = false;
					curr_node = child_node;
					ope_type::setBlack(*curr_node);//ノードを黒にする
					//child_is_large = false;
					//child_node = nullptr;
					return parent_node;
				};
				//【共通処理】現在のノードを左回転処理
				//-------------------------------------------------------------------------
				//           .-------[curr_node:R]-------.                                 
				//         [(S)]                .--[child_node:R]                          
				//                            [(LS)]                                       
				//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				//                          ↓【左回転】                                   
				//           .-------[child_node:R]                                        
				//   .--[curr_node:R]--.                                                   
				// [(S)]            [(LS)]                                                 
				//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				//                          ↓【変数を調整】※色の調整はしない             
				//           .--------[curr_node:R]                                        
				//   .--[child_node:R]--.                                                  
				// [(SS)]            [(SL)]                                                
				//-------------------------------------------------------------------------
				auto rotateCurrL = [&]() -> const node_type*
				{
					node_type* curr_node_tmp = curr_node;
					curr_node = _rotateL<ope_type>(curr_node);//左回転処理
					child_is_large = false;
					child_node = curr_node_tmp;
					return curr_node;
				};
				//【共通処理】現在のノードを右回転処理
				//-------------------------------------------------------------------------
				//       .--------[curr_node:R]--------.                                   
				// [child_node:R]--.                 [(L)]                                 
				//              [(SS)]                                                     
				//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				//                     ↓【右回転】                                        
				//               [child_node:R]-------.                                    
				//                           .--[curr_node:R]--.                           
				//                         [(SS)]            [(L)]                         
				//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				//                     ↓【変数を調整】※色の調整はしない                  
				//                [curr_node:R]-------.                                    
				//                          .--[child_node:R]--.                           
				//                        [(LS)]            [(LL)]                         
				//-------------------------------------------------------------------------
				auto rotateCurrR = [&]() -> const node_type*
				{
					node_type* curr_node_tmp = curr_node;
					curr_node = _rotateR<ope_type>(curr_node);//右回転処理
					child_is_large = true;
					child_node = curr_node_tmp;
					return curr_node;
				};
				//各状態に合わせた回転処理
				if (!curr_is_large && !child_is_large)
				{
					//【回転ケース[S-S]：［小（左）側の子：赤］→［小（左）側の子：赤］】
					//-------------------------------------------------------------------------
					//               .-----[parent_node:B]-----.                               
					//       .--[curr_node:R]--.             [(L)]                             
					// [child_node:R]       [(SL)]                                             
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                            ↓【親ノードを右回転】                       
					//               .-------[curr_node:R]-------.                             
					//          [child_node:R]         .--[parent_node:B]--.                   
					//                               [(SL)]              [(L)]                 
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                            ↓【変数と色を調整】                         
					//               .-------[parent_node:R]-----.                             
					//           [curr_node:+B]         .------[(L):B]------.                  
					//                      ↑        [(LS)]             [(LL)]                
					//                     黒に                                                
					//-------------------------------------------------------------------------
					printf_dbg_add("<ADD-SS-[%d:B]→[%d:R]→[%d:R]>", ope_type::getKey(*parent_node), ope_type::getKey(*curr_node), ope_type::getKey(*child_node));
					rotateParentR();//親ノードを右回転処理
				}
				else if (!curr_is_large &&  child_is_large)
				{
					//【回転ケース[S-L]：［小（左）側の子：赤］→［大（右）側の子：赤］】
					//-------------------------------------------------------------------------
					//                       .-----------------[parent_node:B]-------.         
					//               .--[curr_node:R]--.                           [(L)]       
					//             [(SS)]        .--[child_node:R]--.                          
					//                        [(SLS)]          [(SLL)]                         
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                                               ↓【現在ノードを左回転】  
					//                        .----------------[parent_node:B]-------.         
					//           .------[child_node:R]------.                      [(L)]       
					//   .--[curr_node:R]--.            [(SLL)]                                
					// [(SS)]          [(SLS)]                                                 
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                                               ↓【親ノードを右回転】    
					//                        .-----------------[child_node:R]-------.         
					//                .--[curr_node:R]--.               .--[parent_node:B]--.  
					//              [(SS)]           [(SLS)]          [(SLL)]             [(L)]
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                                               ↓【変数と色を調整】      
					//                        .-----------------[parent_node:R]-----.          
					//                .--[curr_node:+B]--.                  .-----[(L):B]---.  
					//               [(SS)])        ↑ [(SL)]             [(LS)]         [(LL)]
					//                             黒に                                        
					//-------------------------------------------------------------------------
					printf_dbg_add("<ADD-SL-[%d:B]→[%d:R]→[%d:R]>", ope_type::getKey(*parent_node), ope_type::getKey(*curr_node), ope_type::getKey(*child_node));
					ope_type::setChildS(*parent_node, rotateCurrL());//現在のノードを左回転処理
					rotateParentR();//親ノードを右回転処理
				}
				else if (curr_is_large && !child_is_large)
				{
					//【回転ケ－ス[L-S]：［大（右）側の子：赤］→［小（左）側の子：赤］】
					//---------------------------------------------------------------------------------
					//             .-------[parent_node:B]----------------.                            
					//           [(S)]                       .-------[curr_node:R]-------.             
					//                               .--[child_node:R]--.              [(LL)]          
					//                             [(LSS)]          [(LSL)]                            
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                          ↓【現在ノードを右回転】                               
					//             .-------[parent_node:B]-----------------.                           
					//           [(S)]                       .-------[child_node:R]-------.            
					//                                     [(LSS)]                .--[curr_node:R]--.  
					//                                                          [(LSL)]          [(LL)]
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                          ↓【親ノードを左回転】                                 
					//           .----------[child_node:R]---------.                                   
					//   .--[parent_node:B]--.             .--[curr_node:R]--.                         
					// [(S)]             [(LSS)]         [(LSL)]          [(LL)]                       
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                          ↓【変数と色を調整】                                   
					//            .--------[parent_node:R]--------.       ↓黒に                       
					//   .------[(S):B]------.              .--[curr_node:+B]--.                       
					// [(SS)]             [(SL)]          [(LSL)]           [(LL)]                     
					//---------------------------------------------------------------------------------
					printf_dbg_add("<ADD-LS-[%d:B]→[%d:R]→[%d:R]>", ope_type::getKey(*parent_node), ope_type::getKey(*curr_node), ope_type::getKey(*child_node));
					ope_type::setChildL(*parent_node, rotateCurrR());//現在のノードを右回転処理
					rotateParentL();//親ノードを左回転処理
				}
				else//if (!curr_is_large && !child_is_large)
				{
					//【回転ケース[L-L]：［大（右）側の子：赤］→［大（右）側の子：赤］】
					//-------------------------------------------------------------------------
					//             .-------[parent_node:B]--------.                            
					//           [(S)]                    .--[curr_node:R]--.                  
					//                                  [(LS)]       [child_node:R]            
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                          ↓【親ノードを左回転】                         
					//          .-----------[curr_node:R]---------.                            
					//   .--[parent_node:B]--.               [child_node:R]                    
					// [(S)]              [(LS)]                                               
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                          ↓【変数と色を調整】                           
					//            .--------[parent_node:R]--------.      ↓ 黒に               
					//   .------[(S):B]------.                [curr_node:+B]                   
					// [(SS)]             [(SL)]                                               
					//-------------------------------------------------------------------------
					printf_dbg_add("<ADD-LL-[%d:B]→[%d:R]→[%d:R]>", ope_type::getKey(*parent_node), ope_type::getKey(*curr_node), ope_type::getKey(*child_node));
					rotateParentL();//親ノードを左回転処理
				}
				is_rotated = true;//回転済み
			}
			//親ノードに処理を移す
			child_is_large = curr_is_large;
			child_node = curr_node;
			curr_node = parent_node;
		}
		if (curr_node)
		{
			root = curr_node;//根ノードを更新
			ope_type::setBlack(*root);//根ノードは黒
		}
	}
	//--------------------
	//赤黒木操作関数：ノード削除時の赤黒バランス調整
	template<class OPE_TYPE>
	void _balanceForRemove(typename OPE_TYPE::node_type*& root, stack_t<OPE_TYPE>& stack, typename OPE_TYPE::node_type* removing_node, typename OPE_TYPE::node_type* replacing_node)
	{
		DECLARE_OPE_TYPES(OPE_TYPE);
		bool is_necessary_rotate = false;//回転処理必要？
		bool remove_node_is_black = ope_type::isBlack(*removing_node);
		bool replace_node_is_black = replacing_node ? ope_type::isBlack(*replacing_node) : false;
		//回転処理必要判定
		//-------------------------------------------------------------------------
		//【木の説明の凡例】
		//・「:B」は、黒を示す。
		//・「:R」は、赤を示す。
		//・「:R/n」は、赤、もしくは、ノードがないことを示す。
		//・「:+B」は、黒に変更することを示す。
		//・「:+R」は、赤に変更することを示す。
		//・「(S)」は、小（左）側のノードを示す。
		//・「(L)」は、大（右）側のノードを示す。
		//・「(SS)」は、小（左）側の子の小（左）側のノードを示す。
		//・「(SL)」は、小（左）側の子の大（右）側のノードを示す。
		//　（「(LS)」[(SS)」「(SSL)」などのパターンも同様）
		//・「(null)」は、ノードがないことを示す。
		//-------------------------------------------------------------------------
		if (!remove_node_is_black)//削除ノードが赤の場合
		{
			if (!replacing_node)//置き換えノードがない場合
			{
				//【判定ケース[R-on-null]：削除ノードが赤、置き換えノードがない】：回転処理不要
				//-------------------------------------------------------------------------
				//    .--[removing_node:R]--.                                              
				// (null)                 (null)                                           
				//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				//             ↓【削除】                                                  
				//           (null)                                                        
				//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				//※黒が減らないので、木に影響がない。                                     
				//-------------------------------------------------------------------------
				//【最終状態の特徴】                                                       
				//・赤黒の位置関係（条件③）も                                             
				//　黒のバランス（条件④）も崩れない。                                     
				//-------------------------------------------------------------------------
				printf_dbg_remove("<DEL-Rn-[%d:R]←(null)=END", ope_type::getKey(*removing_node));
				//is_necessary_rotate = false;//回転処理不要
			}
			else//if(replacing_node)//置き換えノードがある場合
			{
				if (!replace_node_is_black)//置き換えノードが黒の場合
				{
					//【判定ケース[R-on-R]：削除ノードが赤、置き換えノードが赤】：回転処理不要
					//-------------------------------------------------------------------------
					//     .---------------------[removing_node:R]-----.                       
					//  [(S:B)]---------.                           [(L:B)]                    
					//                  .(最大子孫)                                            
					//         .--[replacing_node:R]--.                                        
					//      [(SLS)]                 (null)                                     
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                                  ↓【置き換え】                         
					//     .---------------------[replacing_node:R]----.                       
					//  [(S:B)]---------.                           [(L:B)]                    
					//                  .(最大子孫)                                            
					//               [(SLS)]                                                   
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//※黒が減らないので、木に影響がない。                                     
					//-------------------------------------------------------------------------
					//【他のケース】                                                           
					//※置き換えノードが削除ノードの子であるケースはない。                     
					//　（赤の子が赤であってはいけないルールのため）                           
					//-------------------------------------------------------------------------
					//【最終状態の特徴】                                                       
					//・赤黒の位置関係（条件③）も                                             
					//　黒のバランス（条件④）も崩れない。                                     
					//-------------------------------------------------------------------------
					printf_dbg_remove("<DEL-RR-[%d:R]←[%d:R]=END>", ope_type::getKey(*removing_node), ope_type::getKey(*replacing_node));
					//is_necessary_rotate = false;//回転処理不要
				}
				else//if (replace_node_is_black)//置き換えノードが黒の場合
				{
					//【判定ケース[R-on-B]：削除ノードが赤、置き換えノードが黒】：回転処理必要
					//-------------------------------------------------------------------------
					//【ケース①】                                                             
					//     .---------------------[removing_node:R]-----.                       
					//  [(S:B)]---------.                           [(L:B)]                    
					//                  .(最大子孫)                                            
					//         .--[replacing_node:B]--.                                        
					//      [(SLS)]                 (null)                                     
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                                  ↓【置き換え】                         
					//     .---------------------[replacing_node:B]----.                       
					//  [(S:B)]---------.                           [(L:B)]                    
					//                  .(最大子孫)                                            
					//               [(SLS)]                                                   
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                                  ↓【色を調整】                         
					//     .---------------------[replacing_node:+R]----.                      
					//  [(S:B)]---------.                        ↑  [(L:B)]                   
					//                  .(最大子孫)             赤に                           
					//               [(SLS)]                                                   
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//※「(SLS)」とその兄弟で黒のバランスが崩れるので、                        
					//　「(SLS)」から調整（回転処理）が必要。                                  
					//-------------------------------------------------------------------------
					//【ケース②】                                                             
					//                    .------[removing_node:R]-----.                       
					//     .----[replacing_node:B]----.             [(L:B)]                    
					// [(SS:R/n)]                   (null)                                     
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                                  ↓【置き換え】                         
					//       .-------------------[replacing_node:B]----.                       
					//   [(SS:R/n)]----.                            [(L:B)]                    
					//               (null)                                                    
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                                  ↓【色を調整】                         
					//       .-------------------[replacing_node:+R]---.                       
					//   [(SS:R/n)]----.                            [(L:B)]                    
					//               (null)                                                    
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//※「(SS:R/n)」と「(L:B)」で黒のバランスが崩れるので、                    
					//　「(SS:R/n)」から調整（回転処理）が必要。                               
					//-------------------------------------------------------------------------
					//【参考ケース①】※この状態は既に条件④に違反しているため、ありえない     
					//              .-----[removing_node:R]-----.                              
					//   .--[replacing_node:B]--.             (null)                           
					// [(SS)]                [(SL)]                                            
					//-------------------------------------------------------------------------
					//【参考ケース②】※この状態は既に条件④に違反しているため、ありえない     
					//      .-----[removing_node:R]-----.                                      
					//    (null)           .--[replacing_node:B]--.                            
					//                   [(LS)]                [(LL)]                          
					//-------------------------------------------------------------------------
					//【最終状態の特徴】                                                       
					//・置き換え元の位置で、黒のバランス（条件④）が崩れる。                   
					//・置き換え元の位置で、赤→赤の連結（条件③違反）が起こる可能性がある。   
					//-------------------------------------------------------------------------
					printf_dbg_remove("<DEL-RB-[%d:R]←[%d:B]=ROT>", ope_type::getKey(*removing_node), ope_type::getKey(*replacing_node));
					is_necessary_rotate = true;//回転処理必要
					ope_type::setRed(*replacing_node);//置き換えノードを赤にする
					//remove_node_is_black = true;//削除ノードは黒
					//                            //※置き換え元のノードが削除されたものとして処理する
				}
			}
		}
		else//if (remove_node_is_black)//削除ノードが黒の場合
		{
			if (!replacing_node)//置き換えノードがない場合
			{
				//【判定ケース[B-on-null]：削除ノードが黒、置き換えノードがない】：回転処理必要
				//-------------------------------------------------------------------------
				//    .--[removing_node:B]--.                                              
				// (null)                 (null)                                           
				//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				//             ↓【削除】                                                  
				//           (null)                                                        
				//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				//※黒が一つ減るので、                                                     
				//　親に遡って木の調整（回転処理）が必要。                                 
				//-------------------------------------------------------------------------
				//【最終状態の特徴】                                                       
				//・置き換え元の位置で、黒のバランス（条件④）が崩れる。                   
				//・置き換え元の位置で、赤→赤の連結（条件③違反）が起こる可能性がある。   
				//-------------------------------------------------------------------------
				printf_dbg_remove("<DEL-Bn-[%d:B]←(null)=ROT>", ope_type::getKey(*removing_node));
				is_necessary_rotate = true;//回転処理必要
			}
			else//if(replacing_node)//置き換えノードがある場合
			{
				if (!replace_node_is_black)//置き換えノードが赤の場合
				{
					//【判定ケース[B-on-R]：削除ノードが黒、置き換えノードが赤】：回転処理不要
					//-------------------------------------------------------------------------
					//【ケース①】                                                             
					//     .---------------------[removing_node:B]-----.                       
					//   [(S)]---------.                             [(L)]                     
					//                 .(最大子孫)                                             
					//        .--[replacing_node:R]--.                                         
					//     [(SLS)]                (null)                                       
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                                  ↓【置き換え】                         
					//     .---------------------[replacing_node:R]----.                       
					//   [(S)]---------.                             [(L)]                     
					//                 .(最大子孫)                                             
					//              [(SLS)]                                                    
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                                  ↓【色を調整】                         
					//     .---------------------[replacing_node:+B]----.                      
					//   [(S)]---------.                       ↑     [(L)]                    
					//                 .(最大子孫)            黒に                             
					//              [(SLS)]                                                    
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//※結果的に黒が減らないので、木に影響がない。                             
					//-------------------------------------------------------------------------
					//【ケース②】                                                             
					//               .-----[removing_node:B]-----.                             
					//    .--[replacing_node:R]--.            [(L:R/n)]                        
					// [(null)]               [(null)]                                         
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                          ↓【置き換え】                                 
					//               .-----[replacing_node:R]-----.                            
					//            [(null)]                     [(L:R/n)]                       
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                          ↓【色を調整】                                 
					//               .-----[replacing_node:+B]----.                            
					//            [(null)]                 ↑  [(L:R/n)]                       
					//                                    黒に                                 
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//※結果的に黒が減らないので、木に影響がない。                             
					//-------------------------------------------------------------------------
					//【ケース③】                                                             
					//      .-----[removing_node:B]-----.                                      
					//    (null)           .--[replacing_node:R]--.                            
					//                  [(null)]               [(null)]                        
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                    ↓【置き換え】                                       
					//      .-----[replacing_node:R]-----.                                     
					//   [(null)]                     [(null)]                                 
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                    ↓【色を調整】                                       
					//      .-----[replacing_node:+B]----.                                     
					//   [(null)]                ↑   [(null)]                                 
					//                         黒に                                            
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//※結果的に黒が減らないので、木に影響がない。                             
					//-------------------------------------------------------------------------
					//【最終状態の特徴】                                                       
					//・赤黒の位置関係（条件③）も                                             
					//　黒のバランス（条件④）も崩れない。                                     
					//-------------------------------------------------------------------------
					printf_dbg_remove("<DEL-BR-[%d:B]←[%d:R]=END>", ope_type::getKey(*removing_node), ope_type::getKey(*replacing_node));
					//is_necessary_rotate = false;//回転処理不要
					ope_type::setBlack(*replacing_node);//置き換えノードを黒にする
					//remove_node_is_black = false;//削除ノードは赤
					//                             //※置き換え元のノードが削除されたものとして処理する
				}
				else//if (replace_node_is_black)//置き換えノードが黒の場合
				{
					//【範囲ケース[B-on-B]：削除ノードが黒、置き換えノードが黒】：回転処理必要
					//-------------------------------------------------------------------------
					//【ケース①】                                                             
					//     .---------------------[removing_node:B]-----.                       
					//   [(S)]---------.                             [(L)]                     
					//                 .(最大子孫)                                             
					//        .--[replacing_node:B]--.                                         
					//     [(SLS)]              (null)                                         
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                                  ↓【置き換え】                         
					//     .---------------------[replacing_node:B]----.                       
					//   [(S)]---------.                             [(L)]                     
					//                 .(最大子孫)                                             
					//              [(SLS)]                                                    
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//※「(SLS)」とその兄弟で黒のバランスが崩れるので、                        
					//　「(SLS)」から調整（回転処理）が必要。                                  
					//-------------------------------------------------------------------------
					//【ケース②】                                                             
					//                    .------[removing_node:B]-----.                       
					//     .----[replacing_node:B]----.              [(L)]                     
					// [(SS:R/n)]                   (null)                                     
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//                                  ↓【置き換え】                         
					//       .-------------------[replacing_node:B]----.                       
					//   [(SS:R/n)]----.                             [(L)]                     
					//               (null)                                                    
					//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					//※「(SS:R/n)」と「(L)」で黒のバランスが崩れるので、                      
					//　「(SS:R/n)」から調整（回転処理）が必要。                               
					//-------------------------------------------------------------------------
					//【参考ケース①】※この状態は既に条件④に違反しているため、ありえない     
					//              .-----[removing_node:B]-----.                              
					//   .--[replacing_node:B]--.             (null)                           
					// [(SS)]                [(SL)]                                            
					//-------------------------------------------------------------------------
					//【参考ケース②】※この状態は既に条件④に違反しているため、ありえない     
					//      .-----[removing_node:B]-----.                                      
					//    (null)           .--[replacing_node:B]--.                            
					//                   [(LS)]              [(LL)]                            
					//-------------------------------------------------------------------------
					//【最終状態の特徴】                                                       
					//・赤黒の位置関係（条件③）は保たれるが、                                 
					//　黒のバランス（条件④）が崩れる。                                       
					//-------------------------------------------------------------------------
					printf_dbg_remove("<DEL-BB-[%d:B]←[%d:B]=ROT>", ope_type::getKey(*removing_node), ope_type::getKey(*replacing_node));
					is_necessary_rotate = true;//回転処理必要
				}
			}
		}
		//回転処理
		//※黒ノードが削除された時のみ回転を行う
		//-------------------------------------------------------------------------
		//【木の説明の凡例】
		//・「:B」は、黒を示す。
		//・「:R」は、赤を示す。
		//・「:?」は、黒もしくは赤を示す。
		//・「:?/n」は、黒もしくは赤、もしくは、ノードがないことを示す。
		//・「:B/n」は、黒、もしくは、ノードがないことを示す。
		//・「:-B」は、そのノード、もしくは、その子孫で、黒ノードの削除があったことを示す。
		//・「:+B」は、黒に変更することを示す。
		//・「:+R」は、赤に変更することを示す。
		//・「:+?」は、元の位置と同じ色に変更することを示す。
		//・「(S)」は、小（左）側のノードを示す。
		//・「(L)」は、大（右）側のノードを示す。
		//・「(SL)」は、小（左）側の子の大（右）側のノードを示す。
		//　（「(LS)」[(SS)」「(SSL)」などのパターンも同様）
		//・「(null)」は、ノードがないことを示す。
		//-------------------------------------------------------------------------
		if (is_necessary_rotate)
		{
			node_type* parent_node = nullptr;//親ノード
			node_type* parent_node_prev = nullptr;//前回のループ処理での親ノード
			bool curr_is_large = false;//親ノードからの連結方向
			while (true)
			{
				stack_info_type* parent_info = stack.pop();//親ノード情報を取得
				if (!parent_info)
					break;
				parent_node = const_cast<node_type*>(parent_info->m_nodeRef);//親ノードを取得
				curr_is_large = parent_info->m_isLarge;//親ノードからの連結方向を取得
				if (parent_node_prev)//前回のループ処理での親ノードを親子連結
				{
					ope_type::setChild(*parent_node, curr_is_large, parent_node_prev);
					parent_node_prev = nullptr;//前回のループ処理での親ノードをクリア
				}
				if (is_necessary_rotate)//回転処理が必要
				{
					node_type* curr_node =//現在のノード取得 ※この側で黒ノードが一つ減少している
						parent_node ?
							ope_type::getChild_rc(*parent_node, curr_is_large) :
							nullptr;
					node_type* sibling_node =//兄弟ノード取得
						parent_node ?
							ope_type::getChild_rc(*parent_node, !curr_is_large) :
							nullptr;
					node_type* sibling_node_s =//兄弟ノードの小（左）側の子ノード取得
						sibling_node ?
							ope_type::getChildS_rc(*sibling_node) :
							nullptr;
					node_type* sibling_node_l =//兄弟ノードの大（右）側の子ノード取得
						sibling_node ?
							ope_type::getChildL_rc(*sibling_node) :
							nullptr;
					if (sibling_node)//兄弟ノードが存在する場合
					{
						if (!curr_is_large)//【ケース：［小（左）側：黒］が削除】
						{
							if (ope_type::isBlack(*sibling_node))//【ケース：削除側の兄弟が［大（右）側：黒］】
							{
								if (sibling_node_s && ope_type::isRed(*sibling_node_s))//【ケース：兄弟の子が［小（左）側の子：赤］】
								{
									//【回転ケース[S-1]：［削除：小（左）側：黒］→［兄弟：大（右）側：黒］→［小（左）側の子：赤］】
									//-------------------------------------------------------------------------------------------------
									//              .----------[parent_node:?]-------------------.                                     
									//        [curr_node:-B]                      .------[sibling_node:B]------.                       
									//                                   .--[sibling_node_s:R]--.      [sibling_node_l:?/n]            
									//                                [(LSS)]                [(LSL)]                                   
									//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
									//                               ↓【兄弟ノードを右回転】                                          
									//              .----------[parent_node:?]-------------------.                                     
									//        [curr_node:-B]                      .------[sibling_node_s:R]------.                     
									//                                         [(LSS)]                .--[sibling_node:B]--.           
									//                                                             [(LSL)]         [sibling_node_l:?/n]
									//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
									//                               ↓【親ノードを左回転】                                            
									//              .----------[sibling_node_s:R]----------------.                                     
									//     .--[parent_node:?]--.                   .------[sibling_node:B]------.                      
									// [curr_node:-B]       [(LSS)]             [(LSL)]                 [sibling_node_l:?/n]           
									//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
									//                               ↓【色を調整】                                                    
									//                                         ↓元のparent_nodeと同じ色に                             
									//              .----------[sibling_node_s:+?]----------------.                                    
									//     .--[parent_node:+B]--.                   .------[sibling_node:B]------.                     
									// [curr_node:?]       ↑[(LSS)]             [(LSL)]                 [sibling_node_l:?/n]          
									//           ↑       黒に                                                                         
									//          上の「+B」により、この「-B」が解消                                                     
									//-------------------------------------------------------------------------------------------------
									//【最終状態の特徴】                                                                               
									//・赤黒の位置関係（条件③）は保たれる。                                                           
									//・部分木の小（左）側に黒が一つ増えることで、                                                     
									//　木全体のバランス（条件④）が回復し、木の調整は完了する。                                       
									//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
									//【初期状態の考察】                                                                               
									//・「curr_node」以下の黒の数は、                                                                  
									//　「sibling_node」以下の黒の数より一つ少ない。                                                   
									//・「(LSS)」以下と「(LSL)」以下と「sibling_node_l」以下の黒の数は、それぞれ                       
									//　「sibling_node」以下の黒の数より一つ少ない。                                                   
									//・「curr_node」以下と「(LSS)」以下と「(LSL)」以下と「sibling_node_l」以下の                      
									//　黒の数は、すべて同じ。                                                                         
									//・「curr_node」と「parent_node」が共に赤（条件③違反）の可能性がある。                           
									//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
									//【最終状態の考察】                                                                               
									//・部分木の大（右）側の黒の数は変化しない。                                                       
									//・部分木の小（左）側の黒の数は一つ増える。                                                       
									//・木全体の黒の数は変化しない。                                                                   
									//・初期状態で「curr_node」と「parent_node」が共に赤（条件③違反）だった場合、問題が解消される。   
									//-------------------------------------------------------------------------------------------------
									printf_dbg_remove("<ROT-S1-[%d:?]→[%d:B]→[%d:R]=END>", ope_type::getKey(*parent_node), ope_type::getKey(*sibling_node), ope_type::getKey(*sibling_node_s));
									ope_type::setChildL(*parent_node, _rotateR<ope_type>(sibling_node));//兄弟ノードを右回転処理
									_rotateL<ope_type>(parent_node);//親ノードを左回転処理
									ope_type::setColor(*sibling_node_s, ope_type::getColor(*parent_node));//親ノードを元のparent_nodeと同じ色に
									ope_type::setBlack(*parent_node);//削除側ノードを黒に
									parent_node_prev = sibling_node_s;//親ノードを記録（次のループ処理の親の子に連結する）
									is_necessary_rotate = false;//調整完了
								}
								else if (sibling_node_l && ope_type::isRed(*sibling_node_l))//【ケース：兄弟の子が［大（右）側の子：赤］】
								{
									//【回転ケース[S-2]：［削除：小（左）側：黒］→［兄弟：大（右）側：黒］→［大（右）側の子：赤］】
									//-----------------------------------------------------------------------------------------
									//              .----------[parent_node:?]-------------------.                             
									//        [curr_node:-B]                      .------[sibling_node:B]------.               
									//                                      [sibling_node_s:B/n]    .--[sibling_node_l:R]--.   
									//                                                           [(LLS)]                [(LLL)]
									//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
									//                               ↓【親ノードを左回転】                                    
									//              .-----------[sibling_node:B]----------------.                              
									//     .--[parent_node:?]--.                  .------[sibling_node_l:R]------.             
									// [curr_node:-B] [sibling_node_s:B/n]     [(LLS)]                        [(LLL)]          
									//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
									//                               ↓【色を調整】                                            
									//                                        ↓元のparent_nodeと同じ色に                      
									//              .-----------[sibling_node:+?]----------------.                             
									//     .--[parent_node:+B]--.                 .------[sibling_node_l:+B]-----.             
									// [curr_node:?]  [sibling_node_s:B/n]     [(LLS)]                   ↑   [(LLL)]          
									//           ↑       ↑黒に                                        黒に                   
									//          上の「+B」により、この「-B」が解消                                             
									//-----------------------------------------------------------------------------------------
									//【最終状態の特徴】                                                                       
									//・赤黒の位置関係（条件③）は保たれる。                                                   
									//・部分木の小（左）側に黒が一つ増えることで、                                             
									//　木全体のバランス（条件④）が回復し、木の調整は完了する。                               
									//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
									//【初期状態の考察】                                                                       
									//・「curr_node」以下の黒の数は、                                                          
									//　「sibling_node」以下の黒の数より一つ少ない。                                           
									//・「(LLS)」以下と「(LLL)」以下と「sibling_node_s」以下の黒の数は、それぞれ               
									//　「sibling_node」以下の黒の数より一つ少ない。                                           
									//・「curr_node」以下と「(LLS)」以下と「(LLL)」以下と「sibling_node_s」以下の              
									//　黒の数は、すべて同じ。                                                                 
									//・「curr_node」と「parent_node」が共に赤（条件③違反）の可能性がある。                           
									//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
									//【最終状態の考察】                                                                       
									//・部分木の大（右）側の黒の数は変化しない。                                               
									//・部分木の小（左）側の黒の数は一つ増える。                                               
									//・木全体の黒の数は変化しない。                                                           
									//・初期状態で「curr_node」と「parent_node」が共に赤（条件③違反）だった場合、問題が解消される。
									//-----------------------------------------------------------------------------------------
									printf_dbg_remove("<ROT-S2-[%d:?]→[%d:B]→[%d:R]=END>", ope_type::getKey(*parent_node), ope_type::getKey(*sibling_node), ope_type::getKey(*sibling_node_l));
									_rotateL<ope_type>(parent_node);//親ノードを左回転処理
									ope_type::setColor(*sibling_node, ope_type::getColor(*parent_node));//親ノードを元のparent_nodeと同じ色に
									ope_type::setBlack(*parent_node);//削除側ノードを黒に
									ope_type::setBlack(*sibling_node_l);//兄弟ノードを黒に
									parent_node_prev = sibling_node;//親ノードを記録（次のループ処理の親の子に連結する）
									is_necessary_rotate = false;//調整完了
								}
								else//【ケース：兄弟の子が大小（左右）両方とも黒もしくはヌル】
								{
									if (ope_type::isRed(*parent_node))//【ケース：削除ノードの親が赤】
									{
										//【回転ケース[S-3a]：［削除：小（左）側：黒］→［親：赤］→［兄弟：大（右）側：黒］→［大小（左右）両側の子：赤以外］】
										//------------------------------------------------------------------------------
										//       .----------[parent_node:R]-------------------.                         
										// [curr_node:-B]                      .------[sibling_node:B]------.           
										//                               [sibling_node_s:B/n]    .--[sibling_node_l:B/n]
										//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ------
										//                        ↓【色を調整】                                        
										//       .----------[parent_node:+B]-------------------.   ↓赤に               
										// [curr_node:?]             黒に↑   .------[sibling_node:+R]------.           
										//           ↑                  [sibling_node_s:B/n]    .--[sibling_node_l:B/n]
										//           上の「+B」により、この「-B」が解消                                 
										//------------------------------------------------------------------------------
										//【最終状態の特徴】                                                            
										//・赤黒の位置関係（条件③）は保たれる。                                        
										//・部分木の大（右）側の黒が一つ減ることで、                                    
										//　部分木のバランスが回復し、                                                  
										//　更に、部分木の根に黒が一つ増えることで、                                    
										//　木全体のバランス（条件④）が回復し、木の調整は完了する。                    
										//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
										//【初期状態の考察】                                                            
										//・「curr_node」以下の黒の数は、                                               
										//　「sibling_node」以下の黒の数より一つ少ない。                                
										//・「sibling_node_s」以下と「sibling_node_l」以下の黒の数は、                  
										//　それぞれ「sibling_node」以下の黒の数より一つ少ない。                        
										//・「curr_node」以下と「sibling_node_s」以下と「sibling_node_l」以下の         
										//　黒の数は、すべて同じ。                                                      
										//・「curr_node」と「parent_node」が共に赤（条件③違反）の可能性がある。                           
										//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
										//【最終状態の考察】                                                            
										//・部分木の大（右）側の黒の数は±０。                                          
										//・部分木の小（左）側の黒の数は一つ増える。                                    
										//・木全体の黒の数は変化しない。                                                
										//・初期状態で「curr_node」と「parent_node」が共に赤（条件③違反）だった場合、問題が解消される。
										//------------------------------------------------------------------------------
										printf_dbg_remove("<ROT-S3a-[%d:R]→[%d:B]→[*:B/n]=END>", ope_type::getKey(*parent_node), ope_type::getKey(*sibling_node));
										ope_type::setBlack(*parent_node);//親ノードを黒に
										ope_type::setRed(*sibling_node);//兄弟ノードを赤に
										parent_node_prev = parent_node;//親ノードを記録（次のループ処理の親の子に連結する）
										is_necessary_rotate = false;//調整完了
									}
									else//if (ope_type::isBlack(*parent_node))//【ケース：削除ノードの親が黒】
									{
										//【回転ケース[S-3b]：［削除：小（左）側：黒］→［親：黒］→［兄弟：大（右）側：黒］→［大小（左右）両側の子：赤以外］】
										//-------------------------------------------------------------------------------------
										//              .----------[parent_node:B]-------------------.                         
										//        [curr_node:-B]                      .------[sibling_node:B]------.           
										//                                      [sibling_node_s:B/n]    .--[sibling_node_l:B/n]
										//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
										//                               ↓【色を調整】                                        
										//              .----------[parent_node:B]--------------------.   ↓赤に               
										//        [curr_node:?]                      .------[sibling_node:+R]------.           
										//                  ↑                  [sibling_node_s:B/n]    .--[sibling_node_l:B/n]
										//                  右の「+R」により、この「-B」が解消（右側も黒が減ったので）         
										//-------------------------------------------------------------------------------------
										//【最終状態の特徴】                                                                   
										//・赤黒の位置関係（条件③）は保たれる。                                               
										//・部分木の大（右）側の黒が一つ減ることで、                                           
										//　部分木のバランスが回復するが、                                                     
										//　部分木全体では黒が一つ減った状態のため、                                           
										//　親に遡って木の調整が必要。                                                         
										//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
										//【初期状態の考察】                                                                   
										//・「curr_node」以下の黒の数は、                                                      
										//　「sibling_node」以下の黒の数より一つ少ない。                                       
										//・「sibling_node_s」以下と「sibling_node_l」以下の黒の数は、                         
										//　それぞれ「sibling_node」以下の黒の数より一つ少ない。                               
										//・「curr_node」以下と「sibling_node_s」以下と「sibling_node_l」以下の                
										//　黒の数は、すべて同じ。                                                             
										//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
										//【最終状態の考察】                                                                   
										//・部分木の大（右）側の黒の数は一つ減る。                                             
										//・部分木の小（左）側の黒の数は変わらない。                                           
										//・部分木のバランスは取れるが、                                                       
										//　部分木全体で黒の数が一つ少ない。                                                   
										//-------------------------------------------------------------------------------------
										printf_dbg_remove("<ROT-S3b-[%d:B]→[%d:B]→[*:B/n]=NEXT>", ope_type::getKey(*parent_node), ope_type::getKey(*sibling_node));
										ope_type::setRed(*sibling_node);//兄弟ノードを赤に
										parent_node_prev = parent_node;//親ノードを記録（次のループ処理の親の子に連結する）
										//is_necessary_rotate = true;//調整続行
									}
								}
							}
							else//if (ope_type::isRed(*sibling_node))//【ケース：削除側の兄弟が［大（右）側：赤］】
							{
								//※兄弟が赤であるため、「条件③」により、必然的に親は黒になる
								//【回転ケース[S-4]：［削除：小（左）側：黒］→［親：黒］→［兄弟：大（右）側：赤］】
								//--------------------------------------------------------------------------------------
								//                 .--------------[parent_node:B]---------------.                       
								//        [curr_node:-B]                         .------[sibling_node:R]------.         
								//                                         [sibling_node_s:B]       [sibling_node_l:B]  
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
								//                                      ↓【親ノードを左回転】                          
								//                 .--------------[sibling_node:R]--------------.                       
								//      .----[parent_node:B]----.                      [sibling_node_l:B]               
								// [curr_node:-B]   [sibling_node_s:B]                                                  
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
								//                                      ↓【色を調整】                                  
								//                 .--------------[sibling_node:+B]-------------.                       
								//      .----[parent_node:+R]----.              ↑黒に [sibling_node_l:B]               
								// [curr_node:-B]   [sibling_node_s:B]                                                  
								//            ↑          ↑赤に（curr_nodeが赤だと、「条件③」が崩れる）               
								//            この「-B」は解消されない                                                  
								//--------------------------------------------------------------------------------------
								//【最終状態の特徴】                                                                    
								//・赤黒の位置関係（条件③）が崩れる可能性があるが、                                    
								//　この後の処理で必ずcurr_nodeの親が黒になるので問題ない。                             
								//・部分木のバランスは回復しない。                                                      
								//・この状態のparent_nodeとcurr_node部分木に対して、                                    
								//　再帰的に【ケース[①～③]】を実行すると、部分木の                                    
								//　バランスが保たれる。                                                                
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
								//【初期状態の考察】                                                                    
								//・「curr_node」以下の黒の数は、                                                       
								//　「sibling_node_s」／「sibling_node_l」以下の黒の数より一つ少ない。                  
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
								//【最終状態の考察】                                                                    
								//・部分木の大（右）側の黒の数は変化しない。                                            
								//・部分木の小（左）側はバランスが崩れるため、                                          
								//　さらにその部分木でバランスを調整する。                                              
								//--------------------------------------------------------------------------------------
								printf_dbg_remove("<ROT-S4-[%d:B]→[%d:R]→[*:B/n]=RETRY>", ope_type::getKey(*parent_node), ope_type::getKey(*sibling_node));
								_rotateL<ope_type>(parent_node);//親ノードを左回転処理
								ope_type::setRed(*parent_node);//削除側ノードを赤に
								ope_type::setBlack(*sibling_node);//親ノードを黒に
								stack_info_type* ancestor_info = stack.top();//スタックから現在の親情報を取得
								const bool parent_is_large = ancestor_info ? ancestor_info->m_isLarge : false;//親の親からの連結方向
								if (ancestor_info)
								{
									node_type* ancestor_node = const_cast<node_type*>(ancestor_info->m_nodeRef);
									ope_type::setChild(*ancestor_node, parent_is_large, sibling_node);//親の親に新しい親を連結
								}
								stack.push(sibling_node, false);//もう一度同じ部分木を回転操作するために、兄弟ノードをスタックにプッシュ
								stack.push(parent_node, false);//もう一度同じ部分木を回転操作するために、親ノードをスタックにプッシュ
								parent_node_prev = curr_node;//親ノードを記録（次のループ処理の親の子に連結する）
								//is_necessary_rotate = true;//再帰的に調整続行
							}
						}
						else//if (curr_is_large)//【ケース：［大（右）側：黒］が削除】
						{
							if (ope_type::isBlack(*sibling_node))//【ケース：削除側の兄弟が［小（左）側：黒］】
							{
								if (sibling_node_l && ope_type::isRed(*sibling_node_l))//【ケース：兄弟の子が［大（右）側の子：赤］】
								{
									//【回転ケース[L-1]：［削除：大（右）側：黒］→［兄弟：小（左）側：黒］→［大（右）側の子：赤］】
									//--------------------------------------------------------------------------------------------
									//                               .-------------------[parent_node:?]----------.               
									//                 .------[sibling_node:B]------.                      [curr_node:-B]         
									//       [sibling_node_s:?/n]      .--[sibling_node_l:R]--.                                   
									//                              [(SLS)]                [(SLL)]                                
									//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
									//                                                         ↓【兄弟ノードを左回転】           
									//                               .-------------------[parent_node:?]----------.               
									//                 .------[sibling_node_l:R]------.                    [curr_node:-B]         
									//           .--[sibling_node:B]--.            [(SLL)]                                        
									// [sibling_node_s:?/n]        [(SLS)]                                                        
									//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
									//                                                         ↓【親ノードを右回転】             
									//                               .-----------------[sibling_node_l:R]---------.               
									//                 .------[sibling_node:B]------.                .--[parent_node:?]--.        
									//           [sibling_node_s:?/n]            [(SLS)]          [(SLL)]          [curr_node:-B] 
									//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
									//                                                         ↓【色を調整】                     
									//                                                                 ↓元のparent_nodeと同じ色に
									//                               .-----------------[sibling_node_l:+?]--------.  ↓黒に       
									//                 .------[sibling_node:B]------.                .--[parent_node:+B]--.       
									//           [sibling_node_s:?/n]            [(SLS)]          [(SLL)]          [curr_node:?]  
									//                                                     上の「+B」により、この「-B」が解消↑   
									//--------------------------------------------------------------------------------------------
									//※調整完了。                                                                                
									//※説明は省略。【ケース[S-1]】と同じ                                                         
									//--------------------------------------------------------------------------------------------
									printf_dbg_remove("<ROT-L1-[%d:?]→[%d:B]→[%d:R]=END>", ope_type::getKey(*parent_node), ope_type::getKey(*sibling_node), ope_type::getKey(*sibling_node_l));
									ope_type::setChildS(*parent_node, _rotateL<ope_type>(sibling_node));//兄弟ノードを左回転処理
									_rotateR<ope_type>(parent_node);//親ノードを右回転処理
									ope_type::setColor(*sibling_node_l, ope_type::getColor(*parent_node));//親ノードを元のparent_nodeと同じ色に
									ope_type::setBlack(*parent_node);//削除側ノードを黒に
									parent_node_prev = sibling_node_l;//親ノードを記録（次のループ処理の親の子に連結する）
									is_necessary_rotate = false;//調整完了
								}
								else if (sibling_node_s && ope_type::isRed(*sibling_node_s))//【ケース：兄弟の子が［小（左）側の子：赤］】
								{
									//【回転ケース[L-2]：［削除：大（右）側：黒］→［兄弟：小（左）側：黒］→［小（左）側の子：赤］】
									//---------------------------------------------------------------------------------------------
									//                               .-------------------[parent_node:?]----------.                
									//                 .------[sibling_node:B]------.                      [curr_node:-B]          
									//    .--[sibling_node_s:R]--.      [sibling_node_l:B/n]                                       
									// [(SLS)]                [(SLL)]                                                              
									//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
									//                                                        ↓【親ノードを右回転】               
									//                               .------------------[sibling_node:R]----------.                
									//                 .------[sibling_node_s:R]------.                .--[parent_node:?]--.       
									//              [(SLS)]                        [(SLL)]    [sibling_node_l:B/n]   [curr_node:-B]
									//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -                    
									//                                                        ↓【色を調整】                       
									//                                                                ↓元のparent_nodeと同じ色に  
									//                               .------------------[sibling_node:+?]---------.   ↓黒に       
									//                 .------[sibling_node_s:+B]-----.               .--[parent_node:+B]--.       
									//              [(SLS)]                   ↑   [(SLL)]    [sibling_node_l:B/n]   [curr_node:?] 
									//                                       黒に            上の「+B」により、この「-B」が解消↑  
									//---------------------------------------------------------------------------------------------
									//※調整完了。                                                                                 
									//※説明は省略。【ケース[S-2]】と同じ                                                          
									//---------------------------------------------------------------------------------------------
									printf_dbg_remove("<ROT-L2-[%d:?]→[%d:B]→[%d:R]=OK>", ope_type::getKey(*parent_node), ope_type::getKey(*sibling_node), ope_type::getKey(*sibling_node_s));
									_rotateR<ope_type>(parent_node);//親ノードを右回転処理
									ope_type::setColor(*sibling_node, ope_type::getColor(*parent_node));//親ノードを元のparent_nodeと同じ色に
									ope_type::setBlack(*parent_node);//削除側ノードを黒に
									ope_type::setBlack(*sibling_node_s);//兄弟ノードを黒に
									parent_node_prev = sibling_node;//親ノードを記録（次のループ処理の親の子に連結する）
									is_necessary_rotate = false;//調整完了
								}
								else//【ケース：兄弟の子が大小（左右）両方とも黒もしくはヌル】
								{
									if (ope_type::isRed(*parent_node))//【ケース：削除ノードの親が赤】
									{
										//【回転ケース[L-3a]：［削除：大（右）側：黒］→［親：赤］→［兄弟：小（左）側：黒］→［大小（左右）両側の子：赤以外］】
										//---------------------------------------------------------------------------------
										//                       .-------------------[parent_node:R]----------.            
										//         .------[sibling_node:B]------.                      [curr_node:-B]      
										// [sibling_node_s:B/n]     [sibling_node_l:B/n]                                   
										//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
										//                                                 ↓【色を調整】                  
										//                       .-------------------[parent_node:+B]---------.            
										//         .------[sibling_node:+B]-----.                 ↑   [curr_node:?]       
										// [sibling_node_s:B/n]     [sibling_node_l:B/n]         黒に			 ↑        
										//                              ↑赤に                         上の「 + B」により、
										//                                                             この「 - B」が解消  
										//---------------------------------------------------------------------------------
										//※調整完了。                                                                     
										//※説明は省略。【ケース[S-3a]】と同じ                                             
										//---------------------------------------------------------------------------------
										printf_dbg_remove("<ROT-L3a-[%d:R]→[%d:B]→[*:B/n]=OK>", ope_type::getKey(*parent_node), ope_type::getKey(*sibling_node));
										ope_type::setBlack(*parent_node);//親ノードを黒に
										ope_type::setRed(*sibling_node);//兄弟ノードを赤に
										parent_node_prev = parent_node;//親ノードを記録（次のループ処理の親の子に連結する）
										is_necessary_rotate = false;//調整完了
									}
									else//if (ope_type::isBlack(*parent_node))//【ケース：削除ノードの親が黒】
									{
										//【回転ケース[L-3b]：［削除：大（右）側：黒］→［親：黒］→［兄弟：小（左）側：黒］→［大小（左右）両側の子：赤以外］】
										//---------------------------------------------------------------------------
										//                       .-------------------[parent_node:B]----------.      
										//         .------[sibling_node:B]------.                      [curr_node:-B]
										// [sibling_node_s:B/n]     [sibling_node_l:B/n]                             
										//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
										//                                                 ↓【色を調整】            
										//                       .-------------------[parent_node:B]---------.       
										//         .------[sibling_node:+R]-----.                      [curr_node:?] 
										// [sibling_node_s:B/n]     [sibling_node_l:B/n]       左の「+R」により、↑  
										//                              ↑赤に                 この「-B」が解消      
										//                                                   （左側も黒が減ったので）
										//---------------------------------------------------------------------------
										//※調整続行。                                                               
										//※説明は省略。【ケース[S-3b]】と同じ                                       
										//---------------------------------------------------------------------------
										printf_dbg_remove("<ROT-L3b-[%d:B]→[%d:B]→[*:B/n]=NEXT>", ope_type::getKey(*parent_node), ope_type::getKey(*sibling_node));
										ope_type::setRed(*sibling_node);//兄弟ノードを赤に
										parent_node_prev = parent_node;//親ノードを記録（次のループ処理の親の子に連結する）
										//is_necessary_rotate = true;//調整続行
									}
								}
							}
							else//if (ope_type::isRed(*sibling_node))//【ケース：削除側の兄弟が［小（左）側：赤］】
							{
								//※兄弟が赤であるため、「条件③」により、必然的に親は黒になる
								//【回転ケース[L-4]：［削除：大（右）側：黒］→［親：黒］→［兄弟：小（左）側：赤］】
								//--------------------------------------------------------------------------------------------------------
								//                         .-------------------[parent_node:B]----------.                                 
								//           .------[sibling_node:R]------.                      [curr_node:-B]                           
								// [sibling_node_s:B]          [sibling_node_l:B]                                                         
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
								//                                                   ↓【親ノードを右回転】                               
								//                         .------------------[sibling_node:R]----------.                                 
								//                 [sibling_node_s:B]                         .--[parent_node:B]--.                       
								//                                                    [sibling_node_l:B]     [curr_node:-B]               
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
								//                                                   ↓【色を調整】                                       
								//                         .-----------------[sibling_node:+B]----------.     ↓赤に（curr_nodeが赤だと、 
								//                 [sibling_node_s:B]                  黒に↑ .--[parent_node:+R]--.  「条件③」が崩れる）
								//                                                    [sibling_node_l:B]     [curr_node:-B]               
								//                                                                                      ↑                
								//                                                                 この「-B」は解消されない               
								//--------------------------------------------------------------------------------------------------------
								//※再帰的に調整続行。                                                                                    
								//※説明は省略。【ケース[S-4]】と同じ                                                                     
								//--------------------------------------------------------------------------------------------------------
								printf_dbg_remove("<ROT-L4-[%d:B]→[%d:R]→[*:B/n]=RETRY>", ope_type::getKey(*parent_node), ope_type::getKey(*sibling_node));
								_rotateR<ope_type>(parent_node);//親ノードを右回転処理
								ope_type::setRed(*parent_node);//削除側ノードを赤に
								ope_type::setBlack(*sibling_node);//親ノードを黒に
								stack_info_type* ancestor_info = stack.top();//スタックから現在の親情報を取得
								const bool parent_is_large = ancestor_info ? ancestor_info->m_isLarge : false;//親の親からの連結方向
								if (ancestor_info)
								{
									node_type* ancestor_node = const_cast<node_type*>(ancestor_info->m_nodeRef);
									ope_type::setChild(*ancestor_node, parent_is_large, sibling_node);//親の親に新しい親を連結
								}
								stack.push(sibling_node, true);//もう一度同じ部分木を回転操作するために、兄弟ノードをスタックにプッシュ
								stack.push(parent_node, true);//もう一度同じ部分木を回転操作するために、親ノードをスタックにプッシュ
								parent_node_prev = curr_node;//親ノードを記録（次のループ処理の親の子に連結する）
								//is_necessary_rotate = true;//再帰的に調整続行
							}
						}
					}
					else//if (!sibling_node)//兄弟ノードが存在しない場合
					{
						//黒ノード（削除対象ノード）の兄弟ノードが存在しないことは本来ありえない
						//※「条件④」により、根から葉までのあらゆる経路で黒の数は一定のため、
						//　黒ノードを削除した場合、必ず兄弟ノードもしくはその子孫に黒がいる。
						printf_dbg_remove("【赤黒木にバグあり！】黒ノード（削除）の兄弟ノードが存在しない\n");
						assert(sibling_node != nullptr);
						parent_node_prev = parent_node;//親ノードを記録（次のループ処理の親の子に連結する）
					}
				}
				else//if (!is_necessary_rotate)//回転処理が不要
				{
					parent_node_prev = parent_node;//親ノードを記録（次のループ処理の親の子に連結する）
				}
			}
			if (parent_node_prev)
			{
				root = parent_node_prev;
				ope_type::setBlack(*root);
			}
		}
	}

	//----------------------------------------
	//赤黒木コンテナ
	//※根ノードを持つのみ
	//※std::mapを模した構造
	template<class OPE_TYPE>
	class container
	{
	public:
		//型
		DECLARE_OPE_TYPES(OPE_TYPE);
	public:
		//--------------------
		//イテレータ宣言
		class iterator;
		class reverse_iterator;
		typedef const iterator const_iterator;
		typedef const reverse_iterator const_reverse_iterator;
		//--------------------
		//イテレータ
		class iterator : public std::iterator<std::bidirectional_iterator_tag, node_type>
		{
			friend class container;
			friend class reverse_iterator;
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const node_type() const { return *getNode(); }
			inline operator node_type&(){ return *getNode(); }
			inline operator key_type() const { return ope_type::getKey(*getNode()); }
		public:
			//オペレータ
			inline const_reference operator*() const { return *getNode(); }
			inline reference operator*(){ return *getNode(); }
			inline const_pointer operator->() const { return getNode(); }
			inline pointer operator->(){ return getNode(); }
		#if 1//std::bidirectional_iterator_tag には本来必要ではない
			inline const_iterator operator[](const int index) const
			{
				iterator ite(m_con->m_root, false);
				ite += index;
				return ite;
			}
			inline iterator operator[](const int index)
			{
				iterator ite(m_con->m_root, false);
				ite += index;
				return ite;
			}
		#endif
			//比較オペレータ
			inline bool operator==(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_isEnd && rhs.m_isEnd ? true :
				       m_isEnd || rhs.m_isEnd ? false :
				       ope_type::eq(*m_node, *rhs);
			}
			inline bool operator!=(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_isEnd && rhs.m_isEnd ? false :
				       m_isEnd || rhs.m_isEnd ? true :
				       ope_type::ne(*m_node, *rhs);
			}
		#if 1//std::bidirectional_iterator_tag には本来必要ではない
			inline bool operator>(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_isEnd && !rhs.m_isEnd ? true :
				       m_isEnd || rhs.m_isEnd ? false :
				       ope_type::gt(*m_node, *rhs);
			}
			inline bool operator>=(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_isEnd && rhs.m_isEnd ? true :
				       m_isEnd && !rhs.m_isEnd ? true :
				       m_isEnd || rhs.m_isEnd ? false :
				       ope_type::ge(*m_node, *rhs);
			}
			inline bool operator<(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       !m_isEnd && rhs.m_isEnd ? true :
				       m_isEnd || rhs.m_isEnd ? false :
				       ope_type::lt(*m_node, *rhs);
			}
			inline bool operator<=(const_iterator& rhs) const
			{
				return !isEnabled() || !rhs.isEnabled() ? false :
				       m_isEnd && rhs.m_isEnd ? true :
				       !m_isEnd && rhs.m_isEnd ? true :
				       m_isEnd || rhs.m_isEnd ? false :
				       ope_type::le(*m_node, *rhs);
			}
		#endif
			//演算オペレータ
			inline const_iterator& operator++() const
			{
				updateNext();
				return *this;
			}
			inline const_iterator& operator--() const
			{
				updatePrev();
				return *this;
			}
			inline iterator& operator++()
			{
				updateNext();
				return *this;
			}
			inline iterator& operator--()
			{
				updatePrev();
				return *this;
			}
			inline const_iterator operator++(int) const
			{
				iterator ite(*this);
				++(*this);
				return ite;
			}
			inline const_iterator operator--(int) const
			{
				iterator ite(*this);
				--(*this);
				return  ite;
			}
			inline iterator operator++(int)
			{
				iterator ite(*this);
				++(*this);
				return  ite;
			}
			inline iterator operator--(int)
			{
				iterator ite(*this);
				--(*this);
				return  ite;
			}
		#if 1//std::bidirectional_iterator_tag には本来必要ではない
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
				return  ite;
			}
			inline const_iterator operator+(const std::size_t rhs) const
			{
				return std::move(operator+(static_cast<typename iterator::difference_type>(rhs)));
			}
			inline const_iterator operator-(const typename iterator::difference_type rhs) const
			{
				iterator ite(*this);
				ite -= rhs;
				return  ite;
			}
			inline const_iterator operator-(const std::size_t rhs) const
			{
				return std::move(operator-(static_cast<typename iterator::difference_type>(rhs)));
			}
			inline iterator operator+(const typename iterator::difference_type rhs)
			{
				iterator ite(*this);
				ite += rhs;
				return  ite;
			}
			inline iterator operator+(const std::size_t rhs)
			{
				return std::move(operator+(static_cast<typename iterator::difference_type>(rhs)));
			}
			inline iterator operator-(const typename iterator::difference_type rhs)
			{
				iterator ite(*this);
				ite -= rhs;
				return  ite;
			}
			inline iterator operator-(const std::size_t rhs)
			{
				return std::move(operator-(static_cast<typename iterator::difference_type>(rhs)));
			}
			//inline typename iterator::difference_type operator-(const iterator rhs) const
			//{
			//	return ???;
			//}
		#endif
		public:
			//ムーブオペレータ
			inline iterator& operator=(const_iterator&& rhs)
			{
				m_stack = std::move(rhs.m_stack);
				m_con = rhs.m_con;
				m_node = rhs.m_node;
				m_isEnd = rhs.m_isEnd;
				return *this;
			}
			iterator& operator=(const_reverse_iterator&& rhs);
			//コピーオペレータ
			inline iterator& operator=(const_iterator& rhs)
			{
				m_stack = rhs.m_stack;
				m_con = rhs.m_con;
				m_node = rhs.m_node;
				m_isEnd = rhs.m_isEnd;
				return *this;
			}
			iterator& operator=(const_reverse_iterator& rhs);
		public:
			//アクセッサ
			inline bool isExist() const { return m_node != nullptr; }
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const { return m_node != nullptr || m_isEnd; }
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const { return m_isEnd; }//終端か？
			inline const node_type* getNode() const { return m_node; }//現在のノード
			inline node_type* getNode(){ return m_node; }//現在のノード
		private:
			//メソッド
			inline void updateNext() const
			{
				node_type* prev = m_node;
				m_node = const_cast<node_type*>(getNextNode<ope_type>(m_node, m_stack));
				m_isEnd = (prev && !m_node);
			}
			inline void updatePrev() const
			{
				if (m_isEnd)
				{
					m_stack.reset();
					m_node = const_cast<node_type*>(getLargestNode<ope_type>(m_con->m_root, m_stack));
					m_isEnd = false;
					return;
				}
				m_node = const_cast<node_type*>(getPrevNode<ope_type>(m_node, m_stack));
				m_isEnd = false;
			}
			void updateForward(const int step) const
			{
				int _step = step;
				node_type* prev = m_node;
				while (_step > 0 && m_node)
				{
					m_node = const_cast<node_type*>(getNextNode<ope_type>(m_node, m_stack));
					--_step;
				}
				m_isEnd = (prev && !m_node && _step == 0);
			}
			void updateBackward(const int step) const
			{
				int _step = step;
				if (_step > 0 && m_isEnd)
				{
					m_stack.reset();
					m_node = const_cast<node_type*>(getLargestNode<ope_type>(m_root->m_root, m_stack));
					--_step;
				}
				while (_step > 0 && m_node)
				{
					m_node = const_cast<node_type*>(getPrevNode<ope_type>(m_node, m_stack));
					--_step;
				}
				m_isEnd = false;
			}
		public:
			//ムーブコンストラクタ
			inline iterator(const_iterator&& obj) :
				m_stack(std::move(obj.m_stack)),
				m_con(obj.m_con),
				m_node(obj.m_node),
				m_isEnd(obj.m_isEnd)
			{}
			iterator(const_reverse_iterator&& obj);
			//コピーコンストラクタ
			inline iterator(const_iterator& obj) :
				m_stack(obj.m_stack),
				m_con(obj.m_con),
				m_node(obj.m_node),
				m_isEnd(obj.m_isEnd)
			{}
			iterator(const_reverse_iterator& obj);
			//コンストラクタ
			inline iterator(const container& con, const bool is_end) :
				m_stack(),
				m_con(&con),
				m_node(nullptr),
				m_isEnd(is_end)
			{
				if (!is_end)
				{
					m_node = const_cast<node_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
					if (!m_node)
						m_isEnd = true;
				}
			}
			inline iterator(stack_type&& stack, const container& con, node_type* node, const bool is_end) :
				m_stack(std::move(stack)),
				m_con(&con),
				m_node(node),
				m_isEnd(is_end)
			{}
			inline iterator(stack_type& stack, const container& con, node_type* node, const bool is_end) :
				m_stack(stack),
				m_con(con),
				m_node(node),
				m_isEnd(is_end)
			{}
			inline iterator() :
				m_stack(),
				m_con(nullptr),
				m_node(nullptr),
				m_isEnd(false)
			{}
			//デストラクタ
			inline ~iterator()
			{}
		protected:
			//フィールド
			mutable stack_type m_stack;//スタック
			const container* m_con;//コンテナ
			mutable node_type* m_node;//現在のノード
			mutable bool m_isEnd;//終端か？
		};
		//--------------------
		//リバースイテレータ
		//class reverse_iterator : public std::reverse_iterator<iterator>
		class reverse_iterator : public std::iterator<std::bidirectional_iterator_tag, node_type>
		{
			friend class container;
			friend class iterator;
		public:
			//キャストオペレータ
			inline operator bool() const { return isExist(); }
			inline operator const node_type() const { return *getNode(); }
			inline operator node_type&(){ return *getNode(); }
			inline operator key_type() const { return ope_type::getKey(*getNode()); }
		public:
			//オペレータ
			inline const_reference operator*() const { return *getNode(); }
			inline reference operator*(){ return *getNode(); }
			inline const_pointer operator->() const { return getNode(); }
			inline pointer operator->(){ return getNode(); }
		#if 1//std::bidirectional_iterator_tag には本来必要ではない
			inline const_reverse_iterator operator[](const int index) const
			{
				reverse_iterator ite(m_con->m_root, false);
				ite += index;
				return ite;
			}
			inline reverse_iterator operator[](const int index)
			{
				reverse_iterator ite(m_con->m_root, false);
				ite += index;
				return ite;
			}
		#endif
		public:
			//比較オペレータ
			inline bool operator==(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_isEnd && m_isEnd ? true :
				       rhs.m_isEnd || m_isEnd ? false :
				       ope_type::eq(*rhs, *m_node);
			}
			inline bool operator!=(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_isEnd && m_isEnd ? false :
				       rhs.m_isEnd || m_isEnd ? true :
					   ope_type::ne(*rhs, *m_node);
			}
		#if 1//std::bidirectional_iterator_tag には本来必要ではない
			inline bool operator>(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_isEnd && !m_isEnd ? true :
				       rhs.m_isEnd || m_isEnd ? false :
				       ope_type::gt(*rhs, *m_node);
			}
			inline bool operator>=(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_isEnd && m_isEnd ? true :
				       rhs.m_isEnd && !m_isEnd ? true :
				       rhs.m_isEnd || m_isEnd ? false :
				       ope_type::ge(*rhs, *m_node);
			}
			inline bool operator<(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       !rhs.m_isEnd && m_isEnd ? true :
				       rhs.m_isEnd || m_isEnd ? false :
				       ope_type::lt(*rhs, *m_node);
			}
			inline bool operator<=(const_reverse_iterator& rhs) const
			{
				return !rhs.isEnabled() || !isEnabled() ? false :
				       rhs.m_isEnd && m_isEnd ? true :
				       !rhs.m_isEnd && m_isEnd ? true :
				       rhs.m_isEnd || m_isEnd ? false :
				       ope_type::le(*rhs, *m_node);
			}
		#endif
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
				return  ite;
			}
			inline const_reverse_iterator operator--(int) const
			{
				reverse_iterator ite(*this);
				--(*this);
				return  ite;
			}
			inline reverse_iterator operator++(int)
			{
				reverse_iterator ite(*this);
				++(*this);
				return  ite;
			}
			inline reverse_iterator operator--(int)
			{
				reverse_iterator ite(*this);
				--(*this);
				return  ite;
			}
		#if 1//std::bidirectional_iterator_tag には本来必要ではない
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
				return  ite;
			}
			inline const_reverse_iterator operator+(const std::size_t rhs) const
			{
				return std::move(operator+(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			inline const_reverse_iterator operator-(const typename reverse_iterator::difference_type rhs) const
			{
				reverse_iterator ite(*this);
				ite -= rhs;
				return  ite;
			}
			inline const_reverse_iterator operator-(const std::size_t rhs) const
			{
				return std::move(operator-(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			inline reverse_iterator operator+(const typename reverse_iterator::difference_type rhs)
			{
				reverse_iterator ite(*this);
				ite += rhs;
				return  ite;
			}
			inline reverse_iterator operator+(const std::size_t rhs)
			{
				return std::move(operator+(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			inline reverse_iterator operator-(const typename reverse_iterator::difference_type rhs)
			{
				reverse_iterator ite(*this);
				ite -= rhs;
				return  ite;
			}
			inline reverse_iterator operator-(const std::size_t rhs)
			{
				return std::move(operator-(static_cast<typename reverse_iterator::difference_type>(rhs)));
			}
			//inline typename reverse_iterator::difference_type operator-(const reverse_iterator rhs)
			//{
			//	return ???;
			//}
		#endif
		public:
			//ムーブオペレータ
			inline reverse_iterator& operator=(const_reverse_iterator&& rhs)
			{
				m_stack = std::move(rhs.m_stack);
				m_con = rhs.m_con;
				m_node = rhs.m_node;
				m_isEnd = rhs.m_isEnd;
				return *this;
			}
			inline reverse_iterator& operator=(const_iterator&& rhs)
			{
				m_con = rhs.m_con;
				m_node = rhs.m_node;
				m_isEnd = false;
				if (m_node)
				{
					m_stack = std::move(rhs.m_stack);
					++(*this);
				}
				else
				{
					m_stack.reset();
					if (rhs.m_isEnd)
						m_node = const_cast<node_type*>(getLargestNode<ope_type>(m_con->m_root, m_stack));
				}
				return *this;
			}
			//コピーオペレータ
			inline reverse_iterator& operator=(const_reverse_iterator& rhs)
			{
				m_stack = rhs.m_stack;
				m_con = rhs.m_con;
				m_node = rhs.m_node;
				m_isEnd = rhs.m_isEnd;
				return *this;
			}
			inline reverse_iterator& operator=(const_iterator& rhs)
			{
				m_con = rhs.m_con;
				m_node = rhs.m_node;
				m_isEnd = false;
				if (m_node)
				{
					m_stack = rhs.m_stack;
					++(*this);
				}
				else
				{
					m_stack.reset();
					if (rhs.m_isEnd)
						m_node = const_cast<node_type*>(getLargestNode<ope_type>(m_con->m_root, m_stack));
				}
				return *this;
			}
		public:
			//アクセッサ
			inline bool isExist() const { return m_node != nullptr; }
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const { return m_node != nullptr || m_isEnd; }
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const { return m_isEnd; }//終端か？
			inline const node_type* getNode() const { return m_node; }//現在のノード
			inline node_type* getNode(){ return m_node; }//現在のノード
		public:
			//メソッド
			inline void updateNext() const
			{
				node_type* prev = m_node;
				m_node = const_cast<node_type*>(getPrevNode<ope_type>(m_node, m_stack));
				m_isEnd = (prev && !m_node);
			}
			inline void updatePrev() const
			{
				if (m_isEnd)
				{
					m_stack.reset();
					m_node = const_cast<node_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
					m_isEnd = false;
					return;
				}
				m_node = const_cast<node_type*>(getNextNode<ope_type>(m_node, m_stack));
				m_isEnd = false;
			}
			void updateForward(const int step) const
			{
				int _step = step;
				node_type* prev = m_node;
				while (_step > 0 && m_node)
				{
					m_node = const_cast<node_type*>(getPrevNode<ope_type>(m_node, m_stack));
					--_step;
				}
				m_isEnd = (prev && !m_node && _step == 0);
			}
			void updateBackward(const int step) const
			{
				int _step = step;
				if (_step > 0 && m_isEnd)
				{
					m_stack.reset();
					m_node = const_cast<node_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
					--_step;
				}
				while (_step > 0 && m_node)
				{
					m_node = const_cast<node_type*>(getNextNode<ope_type>(m_node, m_stack));
					--_step;
				}
				m_isEnd = false;
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
				m_stack(std::move(obj.m_stack)),
				m_con(obj.m_con),
				m_node(obj.m_node),
				m_isEnd(obj.m_isEnd)
			{}
			inline reverse_iterator(const_iterator&& obj) :
				m_stack(),
				m_con(obj.m_con),
				m_node(obj.m_node),
				m_isEnd(false)
			{
				if (m_node)
				{
					m_stack = std::move(obj.m_stack);
					++(*this);
				}
				else
				{
					m_stack.reset();
					if (obj.m_isEnd)
						m_node = const_cast<node_type*>(getLargestNode<ope_type>(m_con->m_root, m_stack));
				}
			}
			//コピーコンストラクタ
			inline reverse_iterator(const_reverse_iterator& obj) :
				m_stack(obj.m_stack),
				m_con(obj.m_con),
				m_node(obj.m_node),
				m_isEnd(obj.m_isEnd)
			{}
			inline reverse_iterator(const_iterator& obj) :
				m_stack(),
				m_con(obj.m_con),
				m_node(obj.m_node),
				m_isEnd(obj.m_isEnd)
			{
				if (m_node)
				{
					m_stack = obj.m_stack;
					++(*this);
				}
				else
				{
					m_stack.reset();
					if (obj.m_isEnd)
						m_node = const_cast<node_type*>(getLargestNode<ope_type>(m_con->m_root, m_stack));
				}
			}
			//コンストラクタ
			inline reverse_iterator(const container& con, const bool is_end) :
				m_stack(),
				m_con(&con),
				m_node(nullptr),
				m_isEnd(is_end)
			{
				if (!is_end)
				{
					m_node = const_cast<node_type*>(getLargestNode<ope_type>(m_con->m_root, m_stack));
					if (!m_node)
						m_isEnd = true;
				}
			}
			inline reverse_iterator(stack_type&& stack, const container& con, node_type* node, const bool is_end) :
				m_stack(std::move(stack)),
				m_con(&con),
				m_node(node),
				m_isEnd(is_end)
			{}
			inline reverse_iterator(stack_type& stack, const container& con, node_type* node, const bool is_end) :
				m_stack(stack),
				m_con(&con),
				m_node(node),
				m_isEnd(is_end)
			{}
			inline reverse_iterator() :
				m_stack(),
				m_con(nullptr),
				m_node(nullptr),
				m_isEnd(false)
			{}
			//デストラクタ
			inline ~reverse_iterator()
			{}
		protected:
			//フィールド
			mutable stack_type m_stack;//スタック
			const container* m_con;//コンテナ
			mutable node_type* m_node;//現在のノード
			mutable bool m_isEnd;//終端か？
		};
	public:
		//アクセッサ
		//※std::mapと異なり、ノードのポインタを返す
		const node_type* at(const key_type key) const
		{
			stack_type stack;
			return searchNode<ope_type>(m_root, key, stack, FOR_MATCH);
		}
		inline node_type* at(const key_type key){ return const_cast<node_type*>(const_cast<const container*>(this)->at(key)); }
		inline const node_type* operator[](const key_type key) const { return at(key); }
		inline node_type* operator[](const key_type key){ return at(key); }
	public:
		//キャストオペレータ
		inline operator lock_type&(){ return m_lock; }//ロックオブジェクト
		inline operator lock_type&() const { return m_lock; }//ロックオブジェクト ※mutable
	public:
		//メソッド
		//inline std::size_t max_size() const { return (不定); }
		//inline std::size_t capacity() const { return (不定); }
		inline std::size_t size() const { return countNodes<ope_type>(m_root); }//ノード数を取得
		inline bool empty() const { return m_root == nullptr; }//木が空か？
		inline int depth_max() const { return getDepthMax<ope_type>(m_root); }//木の深さを取得
		inline const node_type* root() const { return m_root; }//根ノードを参照
		inline node_type* root(){ return m_root; }//根ノードを参照
		inline node_type*& root_ref(){ return m_root; }//根ノードの参照を取得
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
		
		//追加／削除系メソッド
		//※std::mapと異なり、追加／削除対象のノードを直接指定し、結果をポインタで受け取る（成功したら、追加／削除したポインタを返す）
		//※要素のメモリ確保／解放を行わない点に注意
		//※emplace(), emplace_hint()には非対応
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		
		//ノードを挿入（連結に追加）
		inline node_type* insert(const node_type& node)
		{
			return addNode<ope_type>(const_cast<node_type*>(&node), m_root);
		}
		//ノードを削除（連結解除）
		inline node_type* erase(const node_type& node)
		{
			return removeNode<ope_type>(&node, m_root);
		}
		//ノードを削除（連結解除）
		//※キー指定
		inline node_type* erase(const key_type key)
		{
			return removeNode<ope_type>(at(key), m_root);
		}
		//全ノードをクリア
		//※根ノードを返す
		inline node_type* clear()
		{ 
			node_type* root = m_root;
			m_root = nullptr;
			return root;
		}

		//探索系メソッド
		//※lower_bound(), upper_bound()には非対応
		//※代わりに、find_nearestに対応
		//※自動的な共有ロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）の取得と解放を行う必要がある

	private:
		//キーを探索
		//※キーが一致する範囲の先頭のイテレータを返す
		inline const_iterator& _find(const_iterator& ite, const key_type key, const match_type_t type = FOR_MATCH) const
		{
			ite.m_node = const_cast<node_type*>(searchNode<ope_type>(m_root, key, ite.m_stack, type));
			return ite;
		}
	public:
		//キーを探索
		//※キーが一致する範囲の先頭のイテレータを返す
		inline const_iterator find(const key_type key, const match_type_t type = FOR_MATCH) const
		{
			const_iterator ite;
			return std::move(_find(ite, key, type));
		}
		inline iterator find(const key_type key, const match_type_t type = FOR_MATCH)
		{
			iterator ite;
			return std::move(_find(ite, key, type));
		}
		//キーが一致するノードの数を返す
		inline std::size_t count(const key_type key) const { return countNodes<ope_type>(m_root, key); }
	private:
		//キーが一致する範囲を返す
		//※キーが一致する範囲の末尾（の次）のイテレータを返す
		const_iterator& _equal_range(const_iterator& ite, const key_type key) const
		{
			ite.m_node = const_cast<node_type*>(searchNode<ope_type>(m_root, key, ite.m_stack, FOR_MATCH));
			while (ite.m_node && ope_type::getKey(*ite) == key)
				++ite;
			return ite;
		}
	public:
		//キーが一致する範囲を返す
		//※キーが一致する範囲の末尾（の次）のイテレータを返す
		inline const_iterator equal_range(const key_type key) const
		{
			const_iterator ite;
			return std::move(_equal_range(ite, key));
		}
		inline iterator equal_range(const key_type key)
		{
			iterator ite;
			return std::move(_equal_range(ite, key));
		}
	public:
		//ムーブコンストラクタ
		container(const container&& con) :
			m_root(con.m_root)
		{}
		//コピーコンストラクタ
		container(const container& con) :
			m_root(con.m_root)
		{}
		//コンストラクタ
		container() :
			m_root(nullptr)
		{}
		//デストラクタ
		~container()
		{}
	private:
		//フィールド
		node_type* m_root;//根ノード
		mutable lock_type m_lock;//ロックオブジェクト
	};
	//イテレータのムーブオペレータ
	template<class OPE_TYPE>
	//typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(typename container<OPE_TYPE>::const_reverse_iterator&& rhs)//GCCはOK, VC++はNG
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator&& rhs)//VC++もOK
	{
		m_con = rhs.m_con;
		m_node = rhs.m_node;
		m_isEnd = false;
		if (m_node)
		{
			m_stack = std::move(rhs.m_stack);
			++(*this);
		}
		else
		{
			m_stack.reset();
			if (rhs.m_isEnd)
				m_node = const_cast<node_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
		}
		return *this;
	}
	//イテレータのコピーオペレータ
	template<class OPE_TYPE>
	//typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(typename container<OPE_TYPE>::const_reverse_iterator& rhs)//GCCはOK, VC++はNG
	typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator=(const typename container<OPE_TYPE>::reverse_iterator& rhs)//VC++もOK
	{
		m_con = rhs.m_con;
		m_node = rhs.m_node;
		m_isEnd = false;
		if (m_node)
		{
			m_stack = rhs.m_stack;
			++(*this);
		}
		else
		{
			m_stack.reset();
			if (rhs.m_isEnd)
				m_node = const_cast<node_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
		}
		return *this;
	}
	//イテレータのムーブコンストラクタ
	template<class OPE_TYPE>
	//container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::const_reverse_iterator&& obj) ://GCCはOK, VC++はNG
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator&& obj) ://VC++もOK
		m_stack(),
		m_con(obj.m_con),
		m_node(obj.m_node),
		m_isEnd(false)
	{
		if (m_node)
		{
			m_stack = std::move(obj.m_stack);
			++(*this);
		}
		else
		{
			m_stack.reset();
			if (obj.m_isEnd)
				m_node = const_cast<node_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
		}
	}
	//イテレータのコピーコンストラクタ
	template<class OPE_TYPE>
	//container<OPE_TYPE>::iterator::iterator(typename container<OPE_TYPE>::const_reverse_iterator& obj) ://GCCはOK, VC++はNG
	container<OPE_TYPE>::iterator::iterator(const typename container<OPE_TYPE>::reverse_iterator& obj) ://VC++もOK
		m_stack(),
		m_con(obj.m_con),
		m_node(obj.m_node),
		m_isEnd(false)
	{
		if (m_node)
		{
			m_stack = obj.m_stack;
			++(*this);
		}
		else
		{
			m_stack.reset();
			if (obj.m_isEnd)
				m_node = const_cast<node_type*>(getSmallestNode<ope_type>(m_con->m_root, m_stack));
		}
	}
	//--------------------
	//基本型定義マクロ消去
	#undef DECLARE_OPE_TYPES
#endif
}//namespace rb_tree

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__RB_TREE_H_

// End of file

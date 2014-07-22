#pragma once
#ifndef GASHA_INCLUDED_RB_TREE_H
#define GASHA_INCLUDED_RB_TREE_H

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// rb_tree.h
// 赤黒木コンテナ【宣言部】
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

#include <gasha/crc32.h>//CRC32計算

#include <cstddef>//std::size_t, std::ptrdiff_t
#include <cstdint>//C++11 std::int64_t, std::uint32_t
#include <utility>//C++11 std::move

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <iterator> <string> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <iterator>//std::iterator用
#include <string>//std::string

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//赤黒木（red-black tree）
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//データ構造とアルゴリズム
//--------------------------------------------------------------------------------
//【特徴】
//・二分探索木により、明示的なソートを行わずとも、二分探索の性能 O(log n) を
//  得ることができる。
//　＜二分木のデータ構造＞
//	  - ノードの左側の子には、キーの値が小さいノードを連結。
//	  - ノードの右側の子には、キーの値が大きいか等しいノードを連結。
//・平衡木を保つアルゴリズムにより、常に十分な二分探索性能が得られることを保証する。
//    - 通常の二分探索木は、木の状態によっては O(n) まで探索性能が劣化するが、
//      平衡木は木の左右のバランスを保つことにより、O(log n) に近い探索性能を
//	    維持する。
//--------------------------------------------------------------------------------
//【利点】
//・木の要素の探索がほぼO(long n)で行える。
//・木への要素の挿入がほぼO(log n)で行える。
//・木の要素の削除がほぼO(long n)で行える。
//・木の要素数の上限を決めずに扱える。
//--------------------------------------------------------------------------------
//【欠点】
//・ランダムアクセスができない。
//・要素の昇順・降順アクセスが遅い。
//・各要素に二つの子の連結情報と色情報（赤or黒の1ビット）を追加する必要がある。
//  （メモリオーバーヘッドがある。ただし、他の平衡木アルゴリズムよりもオーバー
//  ヘッドが少ない）
//・赤黒木は厳密な平衡木ではないため、要素の挿入・削除・探索は、最悪 O(2 log n) に
//  なり得る。
//・メモリ節約の都合から、イテレータ操作中に木への要素の追加・削除ができない。
//　（イテレータが根からの経路をスタックで保持するため、途中で木構造が変わると
//  処理できなくなる）
//--------------------------------------------------------------------------------
//【本プログラムにおける実装要件】
//・アルゴリズムとデータを分離した擬似コンテナとする。
//・ノードの連結情報をユーザー定義可能とする。
//　コンテナ操作用のユーザー定義構造体に、連結情報へのアクセスメソッドを実装することを必要とする。
//・コンテナを用いずとも、ノードの連結操作アルゴリズム（関数）のみの利用も可能とする。
//・コンテナ自体はデータの実体（ノード）を持たず、メモリ確保／解放を行わない。
//・データの実体（ノード）はコンテナの外部から受け取り、コンテナは根の要素を管理する。
//・キーの重複を許容する。
//・メモリ節約のために、親への連結情報を持たない。その代わり、スタックを用いて処理する。
//  イテレータは、このスタック操作を隠蔽する。
//  （注）これにより、イテレータ操作中に要素の追加・削除（木構造の変更）ができないことに注意。
//・文字列キー（std::string/char*）をサポートしない。
//  文字列キーの代わりに、文字列のcrc32値を扱う。（文字列は保持しない）
//・コンテナは、STLの std::map/set をモデルとしたインターフェースを実装する。
//・STL（std::set/map）との主な違いは下記のとおり。
//    - ノードの生成／破棄を行わない。
//    - 例外を扱わない。そのため、イテレータへの isExist() メソッドの追加や、
//      at()メソッドが返す情報がポインターなど、インターフェースの相違点がある。
//    - キーの重複を許容する。（std::multimap/multisetと同様）
//    - 必ずキーと値を扱い、キーは値に含まれるものとする。
//      （std::mapよりはstd::setに近く、キー以外の情報を付加して扱うイメージ。
//      結果としてstd::mapと同等の情報を扱う。）
//    - 下記「想定する具体的な用途」にあるヒープメモリマネージャでの利用を想定し、
//      空きサイズ探索などで活用できるように、「最も近い値の探索」に対応。
//    - （他のコンテナと同様に）コンテナ操作対象・方法を設定した
//      構造体をユーザー定義して用いる。
//--------------------------------------------------------------------------------
//【想定する具的的な用途】
//・常にソート済み状態で情報を管理したいリスト。探索性能と、追加・削除の性能が
//  求められる場合に最適。
//・複数種の木を同一ノードが渡り歩く必要がある場合。
//　　例：ヒープメモリマネージャで利用。使用中のメモリブロックはメモリアドレス
//        探索木に連結し、解放されたメモリブロックは空きサイズ探索木に連結する。
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//【赤黒木の成立条件】※平衡化を実現するためのデータ状態の条件
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

namespace rb_tree
{
	//----------------------------------------
	//デバッグ用補助関数
#ifdef GASHA_RB_TREE_USE_DEBUG_PRINT_FOR_ADD
	template<typename... Tx>
	inline int printf_dbg_add(const char* fmt, Tx&&... args);
#else//GASHA_RB_TREE_USE_DEBUG_PRINT_FOR_ADD
	inline int printf_dbg_add(const char* fmt, ...){ return 0; }
#endif//GASHA_RB_TREE_USE_DEBUG_PRINT_FOR_ADD
#ifdef GASHA_RB_TREE_USE_DEBUG_PRINT_FOR_REMOVE
	template<typename... Tx>
	inline int printf_dbg_remove(const char* fmt, Tx&&... args);
#else//GASHA_RB_TREE_USE_DEBUG_PRINT_FOR_REMOVE
	inline int printf_dbg_remove(const char* fmt, ...){ return 0; }
#endif//GASHA_RB_TREE_USE_DEBUG_PRINT_FOR_REMOVE
	
	//--------------------
	//赤黒木操作用テンプレート構造体
	//※CRTPを活用し、下記のような派生構造体を作成して使用する
	//  //struct 派生構造体名 : public rb_tree::baseOpe<派生構造体名, ノードの型, キーの型, スタックの最大の深さ = 40>
	//	//※文字列キーを扱いたい場合は、キー型に crc32_t を指定すること
	//	//※スタックの最大の深さ = log2(最大件数) * 2 以上（デフォルトの40は100万件を処理可能）
	//	struct ope : public rb_tree::baseOpe<ope, data_t, int>
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
	enum color_t//カラー型
	{
		RED = 0,//赤
		BLACK = 1,//黒
	};
	template<class OPE_TYPE, typename NODE_TYPE, typename KEY_TYPE = std::uint32_t, std::size_t _STACK_DEPTH_MAX = 40>
	struct baseOpe
	{
		//定数
		static const std::size_t STACK_DEPTH_MAX = _STACK_DEPTH_MAX;//スタックの最大の深さ

		//型
		typedef OPE_TYPE ope_type;//ノード操作型
		typedef NODE_TYPE node_type;//ノード型
		typedef KEY_TYPE key_type;//キー型
		typedef rb_tree::color_t color_t;//カラー型

		//ロック型
		typedef dummySharedLock lock_type;//ロックオブジェクト型
		//※デフォルトはダミーのため、一切ロック制御しない。
		//※共有ロック（リード・ライトロック）でコンテナ操作をスレッドセーフにしたい場合は、
		//　baseOpeの派生クラスにて、有効な共有ロック型（sharedSpinLock など）を
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
		inline static int compareKey(const key_type& lhs, const key_type& rhs)
		{
			if (lhs == rhs)
				return 0;
			else if (lhs < rhs)
				return -1;
			else//if (lhs > rhs)
				return 1;
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
		inline static bool eq(const key_type lhs, const node_type& rhs){ return compare(lhs, rhs) == 0; }
		inline static bool eq(const key_type lhs, const key_type rhs){ return compare(lhs, rhs) == 0; }
		inline static bool ne(const node_type& lhs, const node_type& rhs){ return compare(lhs, rhs) != 0; }
		inline static bool ne(const node_type& lhs, const key_type rhs){ return compare(lhs, rhs) != 0; }
		inline static bool ne(const key_type lhs, const node_type& rhs){ return compare(lhs, rhs) != 0; }
		inline static bool ne(const key_type lhs, const key_type rhs){ return compare(lhs, rhs) != 0; }
		inline static bool gt(const node_type& lhs, const node_type& rhs){ return compare(lhs, rhs) > 0; }
		inline static bool gt(const node_type& lhs, const key_type rhs){ return compare(lhs, rhs) > 0; }
		inline static bool gt(const key_type lhs, const node_type& rhs){ return compare(lhs, rhs) > 0; }
		inline static bool gt(const key_type lhs, const key_type rhs){ return compare(lhs, rhs) > 0; }
		inline static bool ge(const node_type& lhs, const node_type& rhs){ return compare(lhs, rhs) >= 0; }
		inline static bool ge(const node_type& lhs, const key_type rhs){ return compare(lhs, rhs) >= 0; }
		inline static bool ge(const key_type lhs, const node_type& rhs){ return compare(lhs, rhs) >= 0; }
		inline static bool ge(const key_type lhs, const key_type rhs){ return compare(lhs, rhs) >= 0; }
		inline static bool lt(const node_type& lhs, const node_type& rhs){ return compare(lhs, rhs) < 0; }
		inline static bool lt(const node_type& lhs, const key_type rhs){ return compare(lhs, rhs) < 0; }
		inline static bool lt(const key_type lhs, const node_type& rhs){ return compare(lhs, rhs) < 0; }
		inline static bool lt(const key_type lhs, const key_type rhs){ return compare(lhs, rhs) < 0; }
		inline static bool le(const node_type& lhs, const node_type& rhs){ return compare(lhs, rhs) <= 0; }
		inline static bool le(const node_type& lhs, const key_type rhs){ return compare(lhs, rhs) <= 0; }
		inline static bool le(const key_type lhs, const node_type& rhs){ return compare(lhs, rhs) <= 0; }
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
		typedef int difference_type; \
		typedef std::size_t size_type; \
		typedef std::size_t index_type; \
		typedef stack_t<ope_type> stack_type; \
		typedef typename stack_type::info_t stack_info_type; \
		typedef typename ope_type::lock_type lock_type;
		//typedef std::ptrdiff_t difference_type;//※difference_typeは、std::ptrdiff_t を使用するとイテレータのオペレータのオーバーロードで問題を起こすので、int 型で扱う
	
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
		static const std::size_t DEPTH_MAX = ope_type::STACK_DEPTH_MAX;//最大の深さ（スタック処理用）
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
		info_t* push(const node_type& node, const bool is_large);
		//スタックからノード情報を取得
		info_t* pop();
		//スタックの先頭のノード情報を参照
		//※要素が減らない
		info_t* top();
		//スタックの現在の深さを取得
		inline int getDepth() const;
		//スタックの現在の深さを更新
		void setDepth(const int depth);
		//スタックの現在の深さをリセット
		void reset();
		//スタックの現在の幅を算出
		//※「幅」＝スタックの現在の深さまでの大小連結の合計値を算出
		//※小側を-1、大側を+1として計算
		std::int64_t calcBreadth();
		//ムーブオペレータ
		stack_t& operator=(stack_t&& rhs);
		//コピーオペレータ
		stack_t& operator=(const stack_t& rhs);
		//ムーブコンストラクタ
		stack_t(stack_t&& obj);
		//コピーコンストラクタ
		stack_t(const stack_t& obj);
		//デフォルトコンストラクタ
		inline stack_t();
		//デストラクタ
		inline ~stack_t();
	private:
		//フィールド
		info_t m_array[DEPTH_MAX];//ノード情報の配列（スタック）
		int m_depth;//スタックのカレントの深さ
	};
	
	//--------------------
	//赤黒木操作関数：最小ノード探索
	//※後からget**Node()やsearchNode()を実行できるように、スタックを渡す必要あり
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getSmallestNode(const typename OPE_TYPE::node_type* root, stack_t<OPE_TYPE>& stack);
	//--------------------
	//赤黒木操作関数：最大ノード探索
	//※後からget**Node()やsearchNode()を実行できるように、スタックを渡す必要あり
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getLargestNode(const typename OPE_TYPE::node_type* root, stack_t<OPE_TYPE>& stack);
	//--------------------
	//赤黒木操作関数：次ノード探索（カレントノードの次に大きいノードを探索）
	//※get**Node()やsearchNode()でカレントノードを取得した際のスタックを渡す必要あり
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getNextNode(const typename OPE_TYPE::node_type& curr_node, stack_t<OPE_TYPE>& stack);
	//--------------------
	//赤黒木操作関数：前ノード探索（カレントノードの次に小さいノードを探索）
	//※get**Node()やsearchNode()でカレントノードを取得した際のスタックを渡す必要あり
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getPrevNode(const typename OPE_TYPE::node_type& curr_node, stack_t<OPE_TYPE>& stack);
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
	const typename OPE_TYPE::node_type* searchNode(const typename OPE_TYPE::node_type* root, const typename OPE_TYPE::key_type key, stack_t<OPE_TYPE>& stack, const match_type_t search_type = FOR_MATCH);
	//既存ノード探索 ※キーを指定する代わりに、既存のノードを渡して探索する
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* searchNode(const typename OPE_TYPE::node_type* root, const typename OPE_TYPE::node_type& node, stack_t<OPE_TYPE>& stack);
	//--------------------
	//赤黒木操作関数：木の最大深度を計測
	//※内部でスタックを作成
	//※根ノードのみの場合は 0
	//※根ノードがnullの場合は -1
	template<class OPE_TYPE>
	int getDepthMax(const typename OPE_TYPE::node_type* root);
	//--------------------
	//赤黒木操作関数：木のノード数を計測
	template<class OPE_TYPE>
	std::size_t countNodes(const typename OPE_TYPE::node_type* node);
	//--------------------
	//赤黒木操作関数：指定のキーのノード数を計測
	//※関数内でスタックを使用
	template<class OPE_TYPE>
	std::size_t countNodes(const typename OPE_TYPE::node_type* root, const typename OPE_TYPE::key_type key);
	//--------------------
	//赤黒木操作関数：ノードを追加
	//※関数内でスタックを使用
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* addNode(typename OPE_TYPE::node_type& new_node, typename OPE_TYPE::node_type*& root);
	//--------------------
	//赤黒木操作関数：ノードを削除
	//※関数内でスタックを二つ使用
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* removeNode(const typename OPE_TYPE::node_type& target_node, typename OPE_TYPE::node_type*& root);

	//----------------------------------------
	//赤黒木コンテナ
	//※根ノードを持つのみ
	template<class OPE_TYPE>
	class container
	{
	public:
		//型
		DECLARE_OPE_TYPES(OPE_TYPE);
	public:
		//--------------------
		//イテレータ宣言
		typedef std::bidirectional_iterator_tag iterator_category;
		class iterator;
		class reverse_iterator;
		typedef const iterator const_iterator;
		typedef const reverse_iterator const_reverse_iterator;
		//--------------------
		//イテレータ
		class iterator : public std::iterator<iterator_category, node_type>
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
		#ifdef GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
			inline const iterator operator[](const int index) const;
			inline iterator operator[](const int index);
		#endif//GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE
			//比較オペレータ
			inline bool operator==(const iterator& rhs) const;
			inline bool operator!=(const iterator& rhs) const;
		#ifdef GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
			inline bool operator>(const iterator& rhs) const;
			inline bool operator>=(const iterator& rhs) const;
			inline bool operator<(const iterator& rhs) const;
			inline bool operator<=(const iterator& rhs) const;
		#endif//GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//演算オペレータ
			inline iterator& operator++();
			inline iterator& operator--();
			inline iterator operator++(int);
			inline iterator operator--(int);
			inline const iterator& operator++() const { return const_cast<iterator*>(this)->operator++(); }
			inline const iterator& operator--() const { return const_cast<iterator*>(this)->operator--(); }
			inline const iterator operator++(int) const { return const_cast<iterator*>(this)->operator++(0); }
			inline const iterator operator--(int) const { return const_cast<iterator*>(this)->operator--(0); }
		#ifdef GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
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
			difference_type operator-(const iterator& rhs) const;//【注意】低速処理
		#endif//GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//アクセッサ
			inline bool isExist() const;
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const;
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const;//終端か？
			inline key_type getKey() const;//現在のキー
			inline const value_type* getValue() const;//現在の値（ノード）
			inline value_type* getValue();//現在の値（ノード）
		private:
			//メソッド
			void updateNext() const;
			void updatePrev() const;
			void updateForward(const difference_type step) const;
			void updateBackward(const difference_type step) const;
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
			//コンストラクタ
			iterator(const container& con, const bool is_end);
			iterator(stack_type&& stack, const container& con, value_type* value, const bool is_end);
			iterator(const stack_type& stack, const container& con, value_type* value, const bool is_end);
			//デフォルトコンストラクタ
			inline iterator() :
				m_stack(),
				m_con(nullptr),
				m_value(nullptr),
				m_isEnd(false)
			{}
			//デストラクタ
			inline ~iterator()
			{}
		protected:
			//フィールド
			mutable stack_type m_stack;//スタック
			const container* m_con;//コンテナ
			mutable value_type* m_value;//現在の値（ノード）
			mutable bool m_isEnd;//終端か？
		};
		//--------------------
		//リバースイテレータ
		//class reverse_iterator : public std::reverse_iterator<iterator>
		class reverse_iterator : public std::iterator<iterator_category, node_type>
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
		#ifdef GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
			inline const reverse_iterator operator[](const int index) const;
			inline reverse_iterator operator[](const int index);
		#endif//GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//比較オペレータ
			inline bool operator==(const reverse_iterator& rhs) const;
			inline bool operator!=(const reverse_iterator& rhs) const;
		#ifdef GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
			inline bool operator>(const reverse_iterator& rhs) const;
			inline bool operator>=(const reverse_iterator& rhs) const;
			inline bool operator<(const reverse_iterator& rhs) const;
			inline bool operator<=(const reverse_iterator& rhs) const;
		#endif//GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//演算オペレータ
			inline reverse_iterator& operator++();
			inline reverse_iterator& operator--();
			inline reverse_iterator operator++(int);
			inline reverse_iterator operator--(int);
			inline const reverse_iterator& operator++() const { return const_cast<reverse_iterator*>(this)->operator++(); }
			inline const reverse_iterator& operator--() const { return const_cast<reverse_iterator*>(this)->operator--(); }
			inline const reverse_iterator operator++(int) const { return const_cast<reverse_iterator*>(this)->operator++(0); }
			inline const reverse_iterator operator--(int) const { return const_cast<reverse_iterator*>(this)->operator--(0); }
		#ifdef GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
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
			difference_type operator-(const reverse_iterator& rhs);//【注意】低速処理
		#endif//GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE
		public:
			//アクセッサ
			inline bool isExist() const;
			inline bool isNotExist() const { return !isExist(); }
			inline bool isEnabled() const;
			inline bool isNotEnabled() const { return !isEnabled(); }
			inline bool isEnd() const;//終端か？
			inline key_type getKey() const;//現在のキー
			inline const value_type* getValue() const;//現在の値（ノード）
			inline value_type* getValue();//現在の値（ノード）
		public:
			//ベースを取得
			inline const iterator base() const;
			inline iterator base();
		public:
			//メソッド
			void updateNext() const;
			void updatePrev() const;
			void updateForward(const difference_type step) const;
			void updateBackward(const difference_type step) const;
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
			//コンストラクタ
			reverse_iterator(const container& con, const bool is_end);
			reverse_iterator(stack_type&& stack, const container& con, value_type* value, const bool is_end);
			reverse_iterator(const stack_type& stack, const container& con, value_type* value, const bool is_end);
			//デフォルトコンストラクタ
			inline reverse_iterator() :
				m_stack(),
				m_con(nullptr),
				m_value(nullptr),
				m_isEnd(false)
			{}
			//デストラクタ
			inline ~reverse_iterator()
			{}
		protected:
			//フィールド
			mutable stack_type m_stack;//スタック
			const container* m_con;//コンテナ
			mutable value_type* m_value;//現在の値（ノード）
			mutable bool m_isEnd;//終端か？
		};
	public:
		//アクセッサ
		//※at(), []()は、std::set/mapと異なり、値のポインタを返し、例外を発生させない点に注意
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）または
		//　排他ロック（ライトロック）の取得と解放を行う必要がある
		inline const node_type* at(const key_type key) const { return findValue(key, FOR_MATCH); }
		inline node_type* at(const key_type key){ return findValue(key, FOR_MATCH); }
		inline const node_type* operator[](const key_type key) const { return findValue(key, FOR_MATCH); }
		inline node_type* operator[](const key_type key){ return findValue(key, FOR_MATCH); }
		//※文字列キーはCRC32に変換して扱う
		inline const node_type* at(const char* key) const { return findValue(key, FOR_MATCH); }
		inline node_type* at(const char* key){ return findValue(key, FOR_MATCH); }
		inline const node_type* operator[](const char* key) const { return findValue(key, FOR_MATCH); }
		inline node_type* operator[](const char* key){ return findValue(key, FOR_MATCH); }
		//※std::stringの場合も同様
		inline const node_type* at(const std::string& key) const { return findValue(key, FOR_MATCH); }
		inline node_type* at(const std::string& key){ return findValue(key, FOR_MATCH); }
		inline const node_type* operator[](const std::string& key) const { return findValue(key, FOR_MATCH); }
		inline node_type* operator[](const std::string& key){ return findValue(key, FOR_MATCH); }
		//※ノード型からキーを取得して探索することにも対応
		inline const node_type* at(const node_type& node) const { return findValue(node, FOR_MATCH); }
		inline node_type* at(const node_type& node){ return findValue(node, FOR_MATCH); }
		inline const node_type* operator[](const node_type& node) const { return findValue(node, FOR_MATCH); }
		inline node_type* operator[](const node_type& node){ return findValue(node, FOR_MATCH); }
	public:
		//キャストオペレータ
		inline operator lock_type&(){ return m_lock; }//ロックオブジェクト
		inline operator lock_type&() const { return m_lock; }//ロックオブジェクト ※mutable
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
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）または
		//　排他ロック（ライトロック）の取得と解放を行う必要がある
		//イテレータ取得
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
	public:
		//メソッド：基本情報系
		//inline std::size_t max_size() const { return (不定); }
		//inline std::size_t capacity() const { return (不定); }
		inline std::size_t size() const;//ノード数を取得
		inline bool empty() const;//木が空か？
		inline int maxDepth() const;//木の深さを取得 ※-1で木がない
	public:
		//メソッド：要素アクセス系
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）または
		//　排他ロック（ライトロック）の取得と解放を行う必要がある
		inline const node_type* root() const { return m_root; }//根ノードを参照
		inline node_type* root(){ return m_root; }//根ノードを参照
		inline node_type*& refRoot(){ return m_root; }//根ノードの参照を取得
	public:
		//追加／削除系メソッド
		//※std::mapと異なり、追加／削除対象のノードを直接指定し、結果をポインタで受け取る（成功したら、追加／削除したポインタを返す）
		//※要素のメモリ確保／解放を行わない点に注意
		//※emplace(), emplace_hint()には非対応
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で排他ロック（ライトロック）の取得と解放を行う必要がある
		
		//ノードを挿入（連結に追加）
		inline node_type* insert(node_type& node);

		//ノードを削除（連結解除）
		inline node_type* erase(node_type& node);

		//ノードを削除（連結解除）
		//※キー指定
		inline node_type* erase(const key_type key);
		inline node_type* erase(const char* key);
		inline node_type* erase(const std::string& key);
		//inline node_type* erase(const node_type& node);
		
		//ノードを削除（連結解除）
		//※イテレータ指定
		//※以後、イテレータを使用できないことに注意
		inline node_type* erase(iterator& ite);

		//全ノードをクリア
		//※根ノードを返す
		node_type* clear();

		//探索系メソッド
		//※lower_bound(), upper_bound()には非対応
		//※代わりに、find_nearestに対応
		//※自動的なロック取得は行わないので、マルチスレッドで利用する際は、
		//　一連の処理ブロックの前後で共有ロック（リードロック）または
		//　排他ロック（ライトロック）の取得と解放を行う必要がある
	private:
		//キーを探索（共通）
		//※キーが一致する範囲の先頭の値を返す
		const node_type* _findValue(const key_type key, const match_type_t type) const;
	public:
		//キーを探索
		//※キーが一致する範囲の先頭の値を返す
		inline const node_type* findValue(const key_type key, const match_type_t type = FOR_MATCH) const;
		inline const node_type* findValue(const char* key, const match_type_t type = FOR_MATCH) const;
		inline const node_type* findValue(const std::string& key, const match_type_t type = FOR_MATCH) const;
		inline const node_type* findValue(const node_type& value, const match_type_t type = FOR_MATCH) const;
		inline node_type* findValue(const key_type key, const match_type_t type = FOR_MATCH);
		inline node_type* findValue(const char* key, const match_type_t type = FOR_MATCH);
		inline node_type* findValue(const std::string& key, const match_type_t type = FOR_MATCH);
		inline node_type* findValue(const node_type& value, const match_type_t type = FOR_MATCH);
	private:
		//キーを探索（共通）
		//※キーが一致する範囲の先頭のイテレータを返す
		void _find(iterator& ite, const key_type key, const match_type_t type) const;
	public:
		//キーを探索
		//※キーが一致する範囲の先頭のイテレータを返す
		inline const iterator find(const key_type key, const match_type_t type = FOR_MATCH) const;
		inline const iterator find(const char* key, const match_type_t type = FOR_MATCH) const;
		inline const iterator find(const std::string& key, const match_type_t type = FOR_MATCH) const;
		inline const iterator find(const node_type& node, const match_type_t type = FOR_MATCH) const;
		inline iterator find(const key_type key, const match_type_t type = FOR_MATCH);
		inline iterator find(const char* key, const match_type_t type = FOR_MATCH);
		inline iterator find(const std::string& key, const match_type_t type = FOR_MATCH);
		inline iterator find(const node_type& node, const match_type_t type = FOR_MATCH);
		
		//キーが一致するノードの数を返す
		inline std::size_t count(const key_type key) const;
		inline std::size_t count(const char* key) const;
		inline std::size_t count(const std::string& key) const;
		inline std::size_t count(const node_type& node) const;
	private:
		//キーが一致する範囲を返す
		//※キーが一致する範囲の末尾（の次）のイテレータを返す
		void _equal_range(iterator& ite, const key_type key) const;
	public:
		//キーが一致する範囲を返す
		//※キーが一致する範囲の末尾（の次）のイテレータを返す
		inline const iterator equal_range(const key_type key) const;
		inline const iterator equal_range(const char* key) const;
		inline const iterator equal_range(const std::string& key) const;
		inline const iterator equal_range(const node_type& node) const;
		inline iterator equal_range(const key_type key);
		inline iterator equal_range(const char* key);
		inline iterator equal_range(const std::string& key);
		inline iterator equal_range(const node_type& node);
	public:
		//ムーブオペレータ
		container& operator=(container&& con);
		//コピーオペレータ
		container& operator=(const container& con);
	public:
		//ムーブコンストラクタ
		container(container&& con);
		//コピーコンストラクタ
		container(const container& con);
		//デフォルトコンストラクタ
		inline container();
		//デストラクタ
		~container();
	private:
		//フィールド
		node_type* m_root;//根ノード
		mutable lock_type m_lock;//ロックオブジェクト
	};
	//----------------------------------------
	//シンプル赤黒木コンテナ
	//※操作用構造体の定義を省略してコンテナを使用するためのクラス。
	//※最も基本的な操作用構造体とそれに基づくコンテナ型を自動定義する。
	template<typename VALUE_TYPE, typename KEY_TYPE = std::uint32_t>
	class simpleContainer
	{
	public:
		typedef VALUE_TYPE core_value_type;//値型
		typedef KEY_TYPE core_key_type;//キー型

		//データノード型
		//※元の値型に対するプロキシーとして振る舞う
		struct node
		{
			core_value_type m_value;//値
			core_key_type m_key;//キー
			mutable const node* m_childS;//小（左）側の子ノード
			mutable const node* m_childL;//大（右）側の子ノード
			mutable bool m_isBlack;//ノードは黒か？

			//キャストオペレータ
			inline operator const core_value_type&() const { return m_value; }
			inline operator core_value_type&(){ return m_value; }
			inline operator const core_value_type*() const { return &m_value; }
			inline operator core_value_type*(){ return &m_value; }
			//inline operator core_key_type() const { return m_key; }
			//アクセッサ
			inline const core_value_type& value() const { return m_value; }
			inline core_value_type& value(){ return m_value; }
			inline const core_value_type* pointer() const { return &m_value; }
			inline core_value_type* pointer(){ return &m_value; }
			inline const core_key_type& key() const { return m_key; }
			inline core_key_type& key(){ return m_key; }
			//基本オペレータ
			inline const core_value_type& operator*() const { return m_value; }
			inline core_value_type& operator*(){ return m_value; }
			inline const core_value_type* operator->() const { return &m_value; }
			inline core_value_type* operator->(){ return &m_value; }
			//比較オペレータ（テンプレート）
			inline bool operator==(const core_key_type rhs) const { return m_key == rhs; }//container::find(), std::find()に必要
			inline bool operator!=(const core_key_type rhs) const { return m_key != rhs; }
			inline bool operator<(const core_key_type rhs) const { return m_key < rhs; }//container::binarySearch(), std::binary_search(), std::lower_bound(), std::upper_bound() に必要
			inline bool operator>(const core_key_type rhs) const { return m_key > rhs; }
			inline bool operator<=(const core_key_type rhs) const { return m_key <= rhs; }
			inline bool operator>=(const core_key_type rhs) const { return m_key >= rhs; }
			//比較オペレータ（自身の型との比較）
			inline bool operator==(const node& rhs) const { return m_key == rhs.m_key; }
			inline bool operator!=(const node& rhs) const { return m_key != rhs.m_key; }
			inline bool operator<(const node& rhs) const { return m_key < rhs.m_key; }//container::sort(), container::stable_sort(), std::sort(), std::stable_sort() に必要
			inline bool operator>(const node& rhs) const { return m_key > rhs.m_key; }
			inline bool operator<=(const node& rhs) const { return m_key <= rhs.m_key; }
			inline bool operator>=(const node& rhs) const { return m_key >= rhs.m_key; }
			//フレンド比較演算子（静的関数になる） ※左辺値が自身の型以外の二項演算子
			friend inline bool operator==(const core_key_type lhs, const node& rhs){ return lhs == rhs.m_key; }
			friend inline bool operator!=(const core_key_type lhs, const node& rhs){ return lhs != rhs.m_key; }
			friend inline bool operator<(const core_key_type lhs, const node& rhs){ return lhs < rhs.m_key; }//std::binary_search(), std::upper_bound() に必要
			friend inline bool operator>(const core_key_type lhs, const node& rhs){ return lhs > rhs.m_key; }
			friend inline bool operator<=(const core_key_type lhs, const node& rhs){ return lhs <= rhs.m_key; }
			friend inline bool operator>=(const core_key_type lhs, const node& rhs){ return lhs >= rhs.m_key; }

			//明示的なコンストラクタ呼び出し
			template<typename... Tx>
			inline void constructor(const core_key_type key, Tx&&... args);
			template<typename... Tx>
			inline void constructor(const char* key, Tx&&... args);
			template<typename... Tx>
			inline void constructor(const std::string& key, Tx&&... args);
			//明示的なデストラクタ呼び出し
			inline void destructor();
			
			//キーと値を更新
			inline void emplace(const core_key_type key, core_value_type&& value);
			inline void emplace(const char* key, core_value_type&& value);
			inline void emplace(const std::string&key, core_value_type&& value);
			inline void emplace(const core_key_type key, const core_value_type& value);
			inline void emplace(const char* key, const core_value_type& value);
			inline void emplace(const std::string& key, const core_value_type& value);
			template<typename... Tx>
			inline void emplace(const core_key_type key, Tx&&... args);
			template<typename... Tx>
			inline void emplace(const char* key, Tx&&... args);
			template<typename... Tx>
			inline void emplace(const std::string& key, Tx&&... args);

			//ムーブオペレータ
			inline node& operator=(core_value_type&& value);
			//コピーオペレータ
			inline node& operator=(const core_value_type& value);
			//ムーブコンストラクタ
			inline node(const core_key_type key, core_value_type&& value);
			inline node(const char* key, core_value_type&& value);
			inline node(const std::string& key, core_value_type&& value);
			//コピーコンストラクタ
			inline node(const core_key_type key, const core_value_type& value);
			inline node(const char* key, const core_value_type& value);
			inline node(const std::string& key, const core_value_type& value);
			//コンストラクタ
			template<typename... Tx>
			inline node(const core_key_type key, Tx&&... args);
			template<typename... Tx>
			inline node(const char* key, Tx&&... args);
			template<typename... Tx>
			inline node(const std::string& key, Tx&&... args);
			//デフォルトコンストラクタ
			inline node();
			//デストラクタ
			inline ~node();
		};

		//赤黒木操作用構造体
		struct ope : public baseOpe<ope, node, core_key_type>
		{
			typedef typename baseOpe<ope, node>::node_type node_type;
			typedef typename baseOpe<ope, node>::key_type key_type;

			//子ノードを取得
			inline static const node_type* getChildS(const node_type& node){ return node.m_childS; }//小（左）側
			inline static const node_type* getChildL(const node_type& node){ return node.m_childL; }//大（右）側
			//子ノードを変更
			inline static void setChildS(node_type& node, const node_type* child){ node.m_childS = child; }//小（左）側
			inline static void setChildL(node_type& node, const node_type* child){ node.m_childL = child; }//大（右）側
			
			//ノードの色を取得
			inline static color_t getColor(const node_type& node){ return node.m_isBlack ? color_t::BLACK : color_t::RED; }
			//ノードの色を変更
			inline static void setColor(node_type& node, const color_t color){ node.m_isBlack = (color == color_t::BLACK); }
			
			//キーを取得
			inline static key_type getKey(const node_type& node){ return node.m_key; }
		};

		//基本型定義
		DECLARE_OPE_TYPES(ope);

		//赤黒木コンテナ
		class con : public container<ope_type>
		{
		public:
		#ifdef GASHA_HAS_INHERITING_CONSTRUCTORS
			using container<ope_type>::container;//継承コンストラクタ
		#else//GASHA_HAS_INHERITING_CONSTRUCTORS
			//ムーブコンストラクタ
			inline con(con&& con) :
				container<ope_type>(std::move(con))
			{}
			//コピーコンストラクタ
			inline con(const con& con) :
				container<ope_type>(con)
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
}//namespace rb_tree

//--------------------
//クラスの別名
//※ネームスペースの指定を省略してクラスを使用するための別名

//赤黒木操作用テンプレート構造体
template<class OPE_TYPE, typename VALUE_TYPE, typename KEY_TYPE = std::uint32_t, std::size_t _STACK_DEPTH_MAX = 40>
using rbTree_baseOpe = rb_tree::baseOpe<OPE_TYPE, VALUE_TYPE, KEY_TYPE, _STACK_DEPTH_MAX>;

//赤黒木コンテナ
template<class OPE_TYPE>
using rbTree = rb_tree::container<OPE_TYPE>;

//シンプル赤黒木コンテナ
template<typename VALUE_TYPE, typename KEY_TYPE = std::uint32_t>
using simpleRBTree = rb_tree::simpleContainer<VALUE_TYPE, KEY_TYPE>;

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/rb_tree.inl>

//.hファイルのインクルードに伴い、常に.cpp.hファイル（および.inlファイル）を自動インクルードする場合
#ifdef GASHA_RB_TREE_ALLWAYS_TOGETHER_CPP_H
#include <gasha/rb_tree.cpp.h>
#endif//GASHA_RB_TREE_ALLWAYS_TOGETHER_CPP_H

#endif//GASHA_INCLUDED_RB_TREE_H

// End of file

#pragma once
#ifndef __RB_TREE_INL_
#define __RB_TREE_INL_

//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// rb_tree.inl
// 赤黒木コンテナ【インライン関数／テンプレート関数定義部】
//
// ※コンテナクラスの操作が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。（コンパイルへの影響を気にしないならOK）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/rb_tree.h>//赤黒木コンテナ【宣言部】

#include <memory.h>//memcpy()
#include <assert.h>//assert()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

namespace rb_tree
{
	//----------------------------------------
	//デバッグ用補助関数
#ifdef GASHA_RB_TREE_USE_DEBUG_PRINT_FOR_ADD
	template<typename... Tx>
	inline int printf_dbg_add(const char* fmt, Tx... args)
	{
		return printf(fmt, args...);
	}
#else//GASHA_RB_TREE_USE_DEBUG_PRINT_FOR_ADD
	inline int printf_dbg_add(const char* fmt, ...){ return 0; }
#endif//GASHA_RB_TREE_USE_DEBUG_PRINT_FOR_ADD
#ifdef GASHA_RB_TREE_USE_DEBUG_PRINT_FOR_REMOVE
	template<typename... Tx>
	inline int printf_dbg_remove(const char* fmt, Tx... args)
	{
		return printf(fmt, args...);
	}
#else//GASHA_RB_TREE_USE_DEBUG_PRINT_FOR_REMOVE
	inline int printf_dbg_remove(const char* fmt, ...){ return 0; }
#endif//GASHA_RB_TREE_USE_DEBUG_PRINT_FOR_REMOVE

	//--------------------
	//赤黒木操作関数：最小ノード探索
	//※後からget**Node()やsearchNode()を実行できるように、スタックを渡す必要あり
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* getSmallestNode(const typename OPE_TYPE::node_type* root, stack_t<OPE_TYPE>& stack)
	{
		if (!root)
			return nullptr;
		const typename OPE_TYPE::node_type* curr_node = root;//現在の探索ノード
		while (true)
		{
			const typename OPE_TYPE::node_type* child_node = OPE_TYPE::getChildS(*curr_node);//小（左）側の子ノードを取得
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
		if (!root)
			return nullptr;
		const typename OPE_TYPE::node_type* curr_node = root;//現在の探索ノード
		while (true)
		{
			const typename OPE_TYPE::node_type* child_node = OPE_TYPE::getChildL(*curr_node);//大（右）側の子ノードを取得
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
		if (!curr_node)
			return nullptr;
		const typename OPE_TYPE::node_type* child_node_l = OPE_TYPE::getChildL(*curr_node);//大（右）側の子ノードを取得
		if (child_node_l)//子がある場合
		{
			stack.push(curr_node, true);//親ノードをスタックに記録
			return getSmallestNode<OPE_TYPE>(child_node_l, stack);//子から一番小さい子孫ノードを取得
		}
		else//if(!child_node_l)//子がない場合
		{
			const typename stack_t<OPE_TYPE>::stack_info_type* stack_node = nullptr;
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
		if (!curr_node)
			return nullptr;
		const typename OPE_TYPE::node_type* child_node_s = OPE_TYPE::getChildS(*curr_node);//小（左）側の子ノードを取得
		if (child_node_s)//子がある場合
		{
			stack.push(curr_node, false);//親ノードをスタックに記録
			return getLargestNode<OPE_TYPE>(child_node_s, stack);//子から一番大きい子孫ノードを取得
		}
		else//if(!child_node_s)//子がない場合
		{
			const typename stack_t<OPE_TYPE>::stack_info_type* stack_node = nullptr;
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
	template<class OPE_TYPE>
	const typename OPE_TYPE::node_type* searchNode(const typename OPE_TYPE::node_type* root, const typename OPE_TYPE::key_type key, stack_t<OPE_TYPE>& stack, const match_type_t search_type)
	{
		if (!root)
			return nullptr;
		const typename OPE_TYPE::node_type* match_node = nullptr;//一致ノード
		int match_stack_depth = -1;//一致ノード検出時のスタック位置
		const typename OPE_TYPE::node_type* nearest_node = nullptr;//最近ノード
		int nearest_stack_depth = -1;//最近ノード検出時のスタック位置
		const typename OPE_TYPE::node_type* curr_node = root;//現在の探索ノード
		while (true)
		{
			const int cmp = OPE_TYPE::compare(*curr_node, key);//指定のキーと大小比較
			if (cmp == 0)//指定のキーと一致
			{
				//※キー重複を許容するアルゴリズムのため、
				//　一致ノードを記録した上で検索を続行する
				match_node = curr_node;//一致ノードを記録
				match_stack_depth = stack.getDepth();//一致ノード検出時のスタック位置を記録
				stack.push(curr_node, false);//仮の親ノードとしてスタックに記録
				curr_node = OPE_TYPE::getChildS(*curr_node);//小（左）側の子の方に検索を続行
				if (!curr_node)//子が無ければ探索終了
					break;
			}
			else if (cmp < 0)//指定のキーより小さい
			{
				if (search_type == FOR_NEAREST_SMALLER && (!nearest_node || nearest_node && OPE_TYPE::le(*nearest_node, *curr_node)))
				{
					nearest_node = curr_node;//最近ノードを記録
					nearest_stack_depth = stack.getDepth();//最近ノード検出時のスタック位置を記録
				}
				const typename OPE_TYPE::node_type* child_node = OPE_TYPE::getChildL(*curr_node);//大（右）側の子ノードを取得
				if (!child_node)//子が無ければ終了
					break;
				stack.push(curr_node, true);//仮の親ノードとしてスタックに記録
				curr_node = child_node;
			}
			else//if (cmp > 0)//指定のキーより大きい
			{
				if (search_type == FOR_NEAREST_LARGER && (!nearest_node || nearest_node && OPE_TYPE::ge(*nearest_node, *curr_node)))
				{
					nearest_node = curr_node;//最近ノードを記録
					nearest_stack_depth = stack.getDepth();//最近ノード検出時のスタック位置を記録
				}
				const typename OPE_TYPE::node_type* child_node = OPE_TYPE::getChildS(*curr_node);//小（左）側の子ノードを取得
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
		const typename OPE_TYPE::key_type key = OPE_TYPE::getKey(*node);//キーを取得
		typename OPE_TYPE::node_type* target_node = const_cast<typename OPE_TYPE::node_type*>(searchNode<OPE_TYPE>(root, key, stack));//キーが一致するノードを検索
		if (!target_node)//キーが一致するノードがなければ終了
			return nullptr;
		while (target_node != node && OPE_TYPE::eq(*target_node, key))//指定ノード（と同じアドレス）を検索
			target_node = const_cast<typename OPE_TYPE::node_type*>(getNextNode<OPE_TYPE>(target_node, stack));
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
		if (!root)
			return -1;
		int depth_max = 0;//最大の深さ
		stack_t<OPE_TYPE> stack;//スタックを作成
		const typename OPE_TYPE::node_type* node = getSmallestNode<OPE_TYPE>(root, stack);//最小ノード取得
		while (node)
		{
			const int depth = stack.getDepth();//深さを取得
			depth_max = depth_max >= depth ? depth_max : depth;//最大の深さを更新
			node = getNextNode<OPE_TYPE>(node, stack);//次のノード取得
		}
		return depth_max;
	}
	//--------------------
	//赤黒木操作関数：木のノード数を計測
	template<class OPE_TYPE>
	std::size_t countNodes(const typename OPE_TYPE::node_type* node)
	{
	#if 0
		//再帰処理版
		return !node ? 0 : 1 + countNodes<OPE_TYPE>(OPE_TYPE::getChildL(*node)) +
			countNodes<OPE_TYPE>(OPE_TYPE::getChildS(*node));
	#else
		//ループ処理版
		//※再帰処理版よりもループ処理版の方が高速
		//　▼実際に処理時間を測定して確認
		//　　以下、ノード数100の計測を10000000回行った結果
		//　　・再帰処理版：　3.443212500 sec
		//　　・ループ処理版：1.663094300 sec
		int count = 0;
		const typename OPE_TYPE::node_type* stack[stack_t<OPE_TYPE>::DEPTH_MAX];
		int stack_pos = 0;
		while (node)
		{
			while (node)
			{
				++count;
				stack[stack_pos++] = node;
				node = OPE_TYPE::getChildS(*node);
			}
			while (!node && stack_pos != 0)
				node = OPE_TYPE::getChildL(*stack[--stack_pos]);
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
		if (!root)
			return 0;
		stack_t<OPE_TYPE> stack;
		const typename OPE_TYPE::node_type* node = searchNode<OPE_TYPE>(root, key, stack, FOR_MATCH);
		if (!node)
			return 0;
		int count = 0;//ノード数
		while (node && OPE_TYPE::eq(*node, key))
		{
			++count;
			node = getNextNode<OPE_TYPE>(node, stack);
		}
		return count;
	}
	//--------------------
	//赤黒木操作関数：ノードを追加
	//※関数内でスタックを使用
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* addNode(typename OPE_TYPE::node_type* new_node, typename OPE_TYPE::node_type*& root)
	{
		if (!new_node)
			return nullptr;
		OPE_TYPE::setChildL(*new_node, nullptr);//新規ノードはリンクをクリア：大（右）側
		OPE_TYPE::setChildS(*new_node, nullptr);//新規ノードはリンクをクリア：小（左）側
		if (!root)//根ノードが未登録の場合
		{
			root = new_node;//根ノードに登録
		#ifndef GASHA_RB_TREE_DISABLE_COLOR_FOR_ADD
			OPE_TYPE::setBlack(*root);//根ノードは黒
		#endif//GASHA_RB_TREE_DISABLE_COLOR_FOR_ADD
			return new_node;//この時点で処理終了
		}
	#ifndef GASHA_RB_TREE_DISABLE_COLOR_FOR_ADD
		OPE_TYPE::setRed(*new_node);//新規ノードは暫定で赤
	#endif//GASHA_RB_TREE_DISABLE_COLOR_FOR_ADD
		typename OPE_TYPE::key_type new_key = OPE_TYPE::getKey(*new_node);//新規ノードのキーを取得
		stack_t<OPE_TYPE> stack;//スタックを用意
		typename OPE_TYPE::node_type* curr_node = root;//現在の探索ノード
		bool new_key_is_large = false;
		while (true)
		{
			new_key_is_large = OPE_TYPE::ge(new_key, *curr_node);//指定のキーと一致もしくは指定のキーの方が大きければtrue
			typename OPE_TYPE::node_type* child_node = OPE_TYPE::getChild_rc(*curr_node, new_key_is_large);//子ノードを取得
			if (!child_node)//子ノードが無ければそこに新規ノードを追加して終了
			{
				OPE_TYPE::setChild(*curr_node, new_key_is_large, new_node);//子ノードとして新規ノードを追加
				break;
			}
			stack.push(curr_node, new_key_is_large);//親ノードをスタックに記録
			curr_node = child_node;
		}
	#ifndef GASHA_RB_TREE_DISABLE_COLOR_FOR_ADD
		//平衡化
		_private::balanceForAdd<OPE_TYPE>(root, stack, curr_node, new_key_is_large, new_node);
	#endif//GASHA_RB_TREE_DISABLE_COLOR_FOR_ADD
		return new_node;
	}
	//--------------------
	//赤黒木操作関数：ノードを削除
	//※関数内でスタックを二つ使用
	template<class OPE_TYPE>
	typename OPE_TYPE::node_type* removeNode(const typename OPE_TYPE::node_type* target_node, typename OPE_TYPE::node_type*& root)
	{
		if (!target_node || !root)
			return nullptr;
		stack_t<OPE_TYPE> stack;//スタックを用意
		typename OPE_TYPE::node_type* removing_node = const_cast<typename OPE_TYPE::node_type*>(searchNode<OPE_TYPE>(root, target_node, stack));//削除ノードを検索してスタックを作る
		if (!removing_node)//検索に失敗したら終了
			return nullptr;
		//削除開始
		typename OPE_TYPE::node_type* parent_node = nullptr;//削除ノードの親ノード
		bool curr_is_large = false;//削除ノードの親ノードからの連結方向
		{
			typename stack_t<OPE_TYPE>::stack_info_type* parent_info = stack.top();//親ノード参照情報を取得
			if (parent_info)
			{
				parent_node = const_cast<typename OPE_TYPE::node_type*>(parent_info->m_nodeRef);//親ノードを取得
				curr_is_large = parent_info->m_isLarge;//親ノードからの連結方向
			}
		}
		typename OPE_TYPE::node_type* child_node_l = OPE_TYPE::getChildL_rc(*removing_node);//大（右）側の子ノードを取得
		typename OPE_TYPE::node_type* child_node_s = OPE_TYPE::getChildS_rc(*removing_node);//小（左）側の子ノードを取得
		typename OPE_TYPE::node_type* descendant_node = nullptr;//削除ノードの最大子孫ノード（削除ノードの次に小さいノード）
		typename OPE_TYPE::node_type* replacing_node = nullptr;//削除ノードと置き換えるノード
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
			typename stack_t<OPE_TYPE>::stack_info_type* remove_info = stack.push(removing_node, false);//スタックに削除ノードを追加
			descendant_node = const_cast<typename OPE_TYPE::node_type*>(getLargestNode<OPE_TYPE>(child_node_s, stack));//最大子孫ノードを取得
			replacing_node = descendant_node;//削除ノードと置き換えるノードをセット
			if (replacing_node != child_node_s)
			{
				//【ケース①：前ノード（最大子孫ノード）が削除ノードの小（左）側の子の最大子孫】
				typename stack_t<OPE_TYPE>::stack_info_type* descendant_parent_info = stack.top();//最大子孫ノードの親ノードを取得
				typename OPE_TYPE::node_type* descendant_parent_node = const_cast<typename OPE_TYPE::node_type*>(descendant_parent_info->m_nodeRef);//最大子孫ノードの親ノードを取得
				OPE_TYPE::setChildL(*descendant_parent_node, OPE_TYPE::getChildS(*replacing_node));//最大子孫ノードの親ノードの大（右）側の子ノードを変更
				OPE_TYPE::setChildS(*replacing_node, child_node_s);//置き換えノードの小（左）側の子ノードを変更
			}
			else//if (descendant_node == child_node_s)
			{
				//【ケース②：前ノード（最大子孫ノード）が削除ノードの小（左）側の子】
				//なにもしない
			}
			OPE_TYPE::setChildL(*replacing_node, child_node_l);//置き換えノードの大（右）側の子ノードを変更
			remove_info->m_nodeRef = replacing_node;//スタック上の削除ノード参照を置き換えノード参照に書き換え
		}
		//else//if (!child_node_s && !child_node_l)
		//{
		//	//削除ノードの小（左）側と大（右）側の両方の子ノードがない場合、置き換えノードはnullptr
		//	replacing_node = nullptr;//削除ノードと置き換えるノードをセット
		//}
		//削除ノードの置き換え処理
		OPE_TYPE::setChildL(*removing_node, nullptr);
		OPE_TYPE::setChildS(*removing_node, nullptr);
		if (!parent_node)//親ノードがない場合 → 根ノードが削除された場合である
		{
			root = replacing_node;//根ノードを置き換え
			if (!root)//根ノードがなくなったら、この時点で処理終了
				return removing_node;
		}
		else//if(parent_node)//親ノードがある場合
		{
			OPE_TYPE::setChild(*parent_node, curr_is_large, replacing_node);//親ノードの子ノードを置き換え
		}
	#ifndef GASHA_RB_TREE_DISABLE_COLOR_FOR_REMOVE
		//平衡化
		_private::balanceForRemove<OPE_TYPE>(root, stack, removing_node, replacing_node);
	#endif//GASHA_RB_TREE_DISABLE_COLOR_FOR_REMOVE
		return removing_node;
	}
	//--------------------
	//※直接使用しない関数
	namespace _private
	{
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
		inline typename OPE_TYPE::node_type* rotateL(typename OPE_TYPE::node_type* curr_node)
		{
			typename OPE_TYPE::node_type* child_node_l = OPE_TYPE::getChildL_rc(*curr_node);
			typename OPE_TYPE::node_type* child_node_ls = OPE_TYPE::getChildS_rc(*child_node_l);
			OPE_TYPE::setChildS(*child_node_l, curr_node);
			OPE_TYPE::setChildL(*curr_node, child_node_ls);
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
		inline typename OPE_TYPE::node_type* rotateR(typename OPE_TYPE::node_type* curr_node)
		{
			typename OPE_TYPE::node_type* child_node_s = OPE_TYPE::getChildS_rc(*curr_node);
			typename OPE_TYPE::node_type* child_node_sl = OPE_TYPE::getChildL_rc(*child_node_s);
			OPE_TYPE::setChildL(*child_node_s, curr_node);
			OPE_TYPE::setChildS(*curr_node, child_node_sl);
			return child_node_s;
		};
		//--------------------
		//赤黒木操作関数：ノード追加時の平衡化
		template<class OPE_TYPE>
		void balanceForAdd(typename OPE_TYPE::node_type*& root, stack_t<OPE_TYPE>& stack, typename OPE_TYPE::node_type* curr_node, bool child_is_large, typename OPE_TYPE::node_type* child_node)
		{
			bool is_rotated = false;//回転処理済みフラグ
			bool is_balanced = false;//平衡状態検出フラグ
			while (true)
			{
				typename stack_t<OPE_TYPE>::stack_info_type* parent_info = stack.pop();//スタックから親ノード情報取得
				if (!parent_info)
					break;
				typename OPE_TYPE::node_type* parent_node = const_cast<typename OPE_TYPE::node_type*>(parent_info->m_nodeRef);//親ノード取得
				bool curr_is_large = parent_info->m_isLarge;//親ノードからの大小連結情報取得
				if (is_rotated)//前回回転処理済みなら親ノードの子ノードを連結し直す
				{
					OPE_TYPE::setChild(*parent_node, curr_is_large, curr_node);
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
				if (!is_balanced && OPE_TYPE::isBlack(*curr_node))//現在ノードが黒なら平衡状態検出（以後の条件違反チェックは不要）
					is_balanced = true;
				if (!is_balanced && OPE_TYPE::isRed(*child_node))//赤ノードが二つ連結していたら条件違反のため、回転処理
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
					auto rotateParentL = [&]() -> const typename OPE_TYPE::node_type*
					{
						parent_node = rotateL<OPE_TYPE>(parent_node);//左回転処理
						curr_is_large = true;
						curr_node = child_node;
						OPE_TYPE::setBlack(*curr_node);//ノードを黒にする
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
					auto rotateParentR = [&]() -> const typename OPE_TYPE::node_type*
					{
						parent_node = rotateR<OPE_TYPE>(parent_node);//右回転処理
						curr_is_large = false;
						curr_node = child_node;
						OPE_TYPE::setBlack(*curr_node);//ノードを黒にする
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
					auto rotateCurrL = [&]() -> const typename OPE_TYPE::node_type*
					{
						typename OPE_TYPE::node_type* curr_node_tmp = curr_node;
						curr_node = rotateL<OPE_TYPE>(curr_node);//左回転処理
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
					auto rotateCurrR = [&]() -> const typename OPE_TYPE::node_type*
					{
						typename OPE_TYPE::node_type* curr_node_tmp = curr_node;
						curr_node = rotateR<OPE_TYPE>(curr_node);//右回転処理
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
						printf_dbg_add("<ADD-SS-[%d:B]→[%d:R]→[%d:R]>", OPE_TYPE::getKey(*parent_node), OPE_TYPE::getKey(*curr_node), OPE_TYPE::getKey(*child_node));
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
						printf_dbg_add("<ADD-SL-[%d:B]→[%d:R]→[%d:R]>", OPE_TYPE::getKey(*parent_node), OPE_TYPE::getKey(*curr_node), OPE_TYPE::getKey(*child_node));
						OPE_TYPE::setChildS(*parent_node, rotateCurrL());//現在のノードを左回転処理
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
						printf_dbg_add("<ADD-LS-[%d:B]→[%d:R]→[%d:R]>", OPE_TYPE::getKey(*parent_node), OPE_TYPE::getKey(*curr_node), OPE_TYPE::getKey(*child_node));
						OPE_TYPE::setChildL(*parent_node, rotateCurrR());//現在のノードを右回転処理
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
						printf_dbg_add("<ADD-LL-[%d:B]→[%d:R]→[%d:R]>", OPE_TYPE::getKey(*parent_node), OPE_TYPE::getKey(*curr_node), OPE_TYPE::getKey(*child_node));
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
				OPE_TYPE::setBlack(*root);//根ノードは黒
			}
		}
		//--------------------
		//赤黒木操作関数：ノード削除時の平衡化
		template<class OPE_TYPE>
		void balanceForRemove(typename OPE_TYPE::node_type*& root, stack_t<OPE_TYPE>& stack, typename OPE_TYPE::node_type* removing_node, typename OPE_TYPE::node_type* replacing_node)
		{
			bool is_necessary_rotate = false;//回転処理必要？
			bool remove_node_is_black = OPE_TYPE::isBlack(*removing_node);
			bool replace_node_is_black = replacing_node ? OPE_TYPE::isBlack(*replacing_node) : false;
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
					printf_dbg_remove("<DEL-Rn-[%d:R]←(null)=END", OPE_TYPE::getKey(*removing_node));
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
						printf_dbg_remove("<DEL-RR-[%d:R]←[%d:R]=END>", OPE_TYPE::getKey(*removing_node), OPE_TYPE::getKey(*replacing_node));
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
						printf_dbg_remove("<DEL-RB-[%d:R]←[%d:B]=ROT>", OPE_TYPE::getKey(*removing_node), OPE_TYPE::getKey(*replacing_node));
						is_necessary_rotate = true;//回転処理必要
						OPE_TYPE::setRed(*replacing_node);//置き換えノードを赤にする
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
					printf_dbg_remove("<DEL-Bn-[%d:B]←(null)=ROT>", OPE_TYPE::getKey(*removing_node));
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
						printf_dbg_remove("<DEL-BR-[%d:B]←[%d:R]=END>", OPE_TYPE::getKey(*removing_node), OPE_TYPE::getKey(*replacing_node));
						//is_necessary_rotate = false;//回転処理不要
						OPE_TYPE::setBlack(*replacing_node);//置き換えノードを黒にする
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
						printf_dbg_remove("<DEL-BB-[%d:B]←[%d:B]=ROT>", OPE_TYPE::getKey(*removing_node), OPE_TYPE::getKey(*replacing_node));
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
				typename OPE_TYPE::node_type* parent_node = nullptr;//親ノード
				typename OPE_TYPE::node_type* parent_node_prev = nullptr;//前回のループ処理での親ノード
				bool curr_is_large = false;//親ノードからの連結方向
				while (true)
				{
					typename stack_t<OPE_TYPE>::stack_info_type* parent_info = stack.pop();//親ノード情報を取得
					if (!parent_info)
						break;
					parent_node = const_cast<typename OPE_TYPE::node_type*>(parent_info->m_nodeRef);//親ノードを取得
					curr_is_large = parent_info->m_isLarge;//親ノードからの連結方向を取得
					if (parent_node_prev)//前回のループ処理での親ノードを親子連結
					{
						OPE_TYPE::setChild(*parent_node, curr_is_large, parent_node_prev);
						parent_node_prev = nullptr;//前回のループ処理での親ノードをクリア
					}
					if (is_necessary_rotate)//回転処理が必要
					{
						typename OPE_TYPE::node_type* curr_node =//現在のノード取得 ※この側で黒ノードが一つ減少している
							parent_node ?
							OPE_TYPE::getChild_rc(*parent_node, curr_is_large) :
							nullptr;
						typename OPE_TYPE::node_type* sibling_node =//兄弟ノード取得
							parent_node ?
							OPE_TYPE::getChild_rc(*parent_node, !curr_is_large) :
							nullptr;
						typename OPE_TYPE::node_type* sibling_node_s =//兄弟ノードの小（左）側の子ノード取得
							sibling_node ?
							OPE_TYPE::getChildS_rc(*sibling_node) :
							nullptr;
						typename OPE_TYPE::node_type* sibling_node_l =//兄弟ノードの大（右）側の子ノード取得
							sibling_node ?
							OPE_TYPE::getChildL_rc(*sibling_node) :
							nullptr;
						if (sibling_node)//兄弟ノードが存在する場合
						{
							if (!curr_is_large)//【ケース：［小（左）側：黒］が削除】
							{
								if (OPE_TYPE::isBlack(*sibling_node))//【ケース：削除側の兄弟が［大（右）側：黒］】
								{
									if (sibling_node_s && OPE_TYPE::isRed(*sibling_node_s))//【ケース：兄弟の子が［小（左）側の子：赤］】
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
										printf_dbg_remove("<ROT-S1-[%d:?]→[%d:B]→[%d:R]=END>", OPE_TYPE::getKey(*parent_node), OPE_TYPE::getKey(*sibling_node), OPE_TYPE::getKey(*sibling_node_s));
										OPE_TYPE::setChildL(*parent_node, rotateR<OPE_TYPE>(sibling_node));//兄弟ノードを右回転処理
										rotateL<OPE_TYPE>(parent_node);//親ノードを左回転処理
										OPE_TYPE::setColor(*sibling_node_s, OPE_TYPE::getColor(*parent_node));//親ノードを元のparent_nodeと同じ色に
										OPE_TYPE::setBlack(*parent_node);//削除側ノードを黒に
										parent_node_prev = sibling_node_s;//親ノードを記録（次のループ処理の親の子に連結する）
										is_necessary_rotate = false;//調整完了
									}
									else if (sibling_node_l && OPE_TYPE::isRed(*sibling_node_l))//【ケース：兄弟の子が［大（右）側の子：赤］】
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
										printf_dbg_remove("<ROT-S2-[%d:?]→[%d:B]→[%d:R]=END>", OPE_TYPE::getKey(*parent_node), OPE_TYPE::getKey(*sibling_node), OPE_TYPE::getKey(*sibling_node_l));
										rotateL<OPE_TYPE>(parent_node);//親ノードを左回転処理
										OPE_TYPE::setColor(*sibling_node, OPE_TYPE::getColor(*parent_node));//親ノードを元のparent_nodeと同じ色に
										OPE_TYPE::setBlack(*parent_node);//削除側ノードを黒に
										OPE_TYPE::setBlack(*sibling_node_l);//兄弟ノードを黒に
										parent_node_prev = sibling_node;//親ノードを記録（次のループ処理の親の子に連結する）
										is_necessary_rotate = false;//調整完了
									}
									else//【ケース：兄弟の子が大小（左右）両方とも黒もしくはヌル】
									{
										if (OPE_TYPE::isRed(*parent_node))//【ケース：削除ノードの親が赤】
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
											printf_dbg_remove("<ROT-S3a-[%d:R]→[%d:B]→[*:B/n]=END>", OPE_TYPE::getKey(*parent_node), OPE_TYPE::getKey(*sibling_node));
											OPE_TYPE::setBlack(*parent_node);//親ノードを黒に
											OPE_TYPE::setRed(*sibling_node);//兄弟ノードを赤に
											parent_node_prev = parent_node;//親ノードを記録（次のループ処理の親の子に連結する）
											is_necessary_rotate = false;//調整完了
										}
										else//if (OPE_TYPE::isBlack(*parent_node))//【ケース：削除ノードの親が黒】
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
											printf_dbg_remove("<ROT-S3b-[%d:B]→[%d:B]→[*:B/n]=NEXT>", OPE_TYPE::getKey(*parent_node), OPE_TYPE::getKey(*sibling_node));
											OPE_TYPE::setRed(*sibling_node);//兄弟ノードを赤に
											parent_node_prev = parent_node;//親ノードを記録（次のループ処理の親の子に連結する）
											//is_necessary_rotate = true;//調整続行
										}
									}
								}
								else//if (OPE_TYPE::isRed(*sibling_node))//【ケース：削除側の兄弟が［大（右）側：赤］】
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
									printf_dbg_remove("<ROT-S4-[%d:B]→[%d:R]→[*:B/n]=RETRY>", OPE_TYPE::getKey(*parent_node), OPE_TYPE::getKey(*sibling_node));
									rotateL<OPE_TYPE>(parent_node);//親ノードを左回転処理
									OPE_TYPE::setRed(*parent_node);//削除側ノードを赤に
									OPE_TYPE::setBlack(*sibling_node);//親ノードを黒に
									typename stack_t<OPE_TYPE>::stack_info_type* ancestor_info = stack.top();//スタックから現在の親情報を取得
									const bool parent_is_large = ancestor_info ? ancestor_info->m_isLarge : false;//親の親からの連結方向
									if (ancestor_info)
									{
										typename OPE_TYPE::node_type* ancestor_node = const_cast<typename OPE_TYPE::node_type*>(ancestor_info->m_nodeRef);
										OPE_TYPE::setChild(*ancestor_node, parent_is_large, sibling_node);//親の親に新しい親を連結
									}
									stack.push(sibling_node, false);//もう一度同じ部分木を回転操作するために、兄弟ノードをスタックにプッシュ
									stack.push(parent_node, false);//もう一度同じ部分木を回転操作するために、親ノードをスタックにプッシュ
									parent_node_prev = curr_node;//親ノードを記録（次のループ処理の親の子に連結する）
									//is_necessary_rotate = true;//再帰的に調整続行
								}
							}
							else//if (curr_is_large)//【ケース：［大（右）側：黒］が削除】
							{
								if (OPE_TYPE::isBlack(*sibling_node))//【ケース：削除側の兄弟が［小（左）側：黒］】
								{
									if (sibling_node_l && OPE_TYPE::isRed(*sibling_node_l))//【ケース：兄弟の子が［大（右）側の子：赤］】
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
										printf_dbg_remove("<ROT-L1-[%d:?]→[%d:B]→[%d:R]=END>", OPE_TYPE::getKey(*parent_node), OPE_TYPE::getKey(*sibling_node), OPE_TYPE::getKey(*sibling_node_l));
										OPE_TYPE::setChildS(*parent_node, rotateL<OPE_TYPE>(sibling_node));//兄弟ノードを左回転処理
										rotateR<OPE_TYPE>(parent_node);//親ノードを右回転処理
										OPE_TYPE::setColor(*sibling_node_l, OPE_TYPE::getColor(*parent_node));//親ノードを元のparent_nodeと同じ色に
										OPE_TYPE::setBlack(*parent_node);//削除側ノードを黒に
										parent_node_prev = sibling_node_l;//親ノードを記録（次のループ処理の親の子に連結する）
										is_necessary_rotate = false;//調整完了
									}
									else if (sibling_node_s && OPE_TYPE::isRed(*sibling_node_s))//【ケース：兄弟の子が［小（左）側の子：赤］】
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
										printf_dbg_remove("<ROT-L2-[%d:?]→[%d:B]→[%d:R]=OK>", OPE_TYPE::getKey(*parent_node), OPE_TYPE::getKey(*sibling_node), OPE_TYPE::getKey(*sibling_node_s));
										rotateR<OPE_TYPE>(parent_node);//親ノードを右回転処理
										OPE_TYPE::setColor(*sibling_node, OPE_TYPE::getColor(*parent_node));//親ノードを元のparent_nodeと同じ色に
										OPE_TYPE::setBlack(*parent_node);//削除側ノードを黒に
										OPE_TYPE::setBlack(*sibling_node_s);//兄弟ノードを黒に
										parent_node_prev = sibling_node;//親ノードを記録（次のループ処理の親の子に連結する）
										is_necessary_rotate = false;//調整完了
									}
									else//【ケース：兄弟の子が大小（左右）両方とも黒もしくはヌル】
									{
										if (OPE_TYPE::isRed(*parent_node))//【ケース：削除ノードの親が赤】
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
											printf_dbg_remove("<ROT-L3a-[%d:R]→[%d:B]→[*:B/n]=OK>", OPE_TYPE::getKey(*parent_node), OPE_TYPE::getKey(*sibling_node));
											OPE_TYPE::setBlack(*parent_node);//親ノードを黒に
											OPE_TYPE::setRed(*sibling_node);//兄弟ノードを赤に
											parent_node_prev = parent_node;//親ノードを記録（次のループ処理の親の子に連結する）
											is_necessary_rotate = false;//調整完了
										}
										else//if (OPE_TYPE::isBlack(*parent_node))//【ケース：削除ノードの親が黒】
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
											printf_dbg_remove("<ROT-L3b-[%d:B]→[%d:B]→[*:B/n]=NEXT>", OPE_TYPE::getKey(*parent_node), OPE_TYPE::getKey(*sibling_node));
											OPE_TYPE::setRed(*sibling_node);//兄弟ノードを赤に
											parent_node_prev = parent_node;//親ノードを記録（次のループ処理の親の子に連結する）
											//is_necessary_rotate = true;//調整続行
										}
									}
								}
								else//if (OPE_TYPE::isRed(*sibling_node))//【ケース：削除側の兄弟が［小（左）側：赤］】
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
									printf_dbg_remove("<ROT-L4-[%d:B]→[%d:R]→[*:B/n]=RETRY>", OPE_TYPE::getKey(*parent_node), OPE_TYPE::getKey(*sibling_node));
									rotateR<OPE_TYPE>(parent_node);//親ノードを右回転処理
									OPE_TYPE::setRed(*parent_node);//削除側ノードを赤に
									OPE_TYPE::setBlack(*sibling_node);//親ノードを黒に
									typename stack_t<OPE_TYPE>::stack_info_type* ancestor_info = stack.top();//スタックから現在の親情報を取得
									const bool parent_is_large = ancestor_info ? ancestor_info->m_isLarge : false;//親の親からの連結方向
									if (ancestor_info)
									{
										typename OPE_TYPE::node_type* ancestor_node = const_cast<typename OPE_TYPE::node_type*>(ancestor_info->m_nodeRef);
										OPE_TYPE::setChild(*ancestor_node, parent_is_large, sibling_node);//親の親に新しい親を連結
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
					OPE_TYPE::setBlack(*root);
				}
			}
		}
	}

	//--------------------
	//イテレータのインライン関数
	
	//基本オペレータ
#ifdef GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator[](const int index) const
	{
		iterator ite(m_con->m_root, false);
		ite += index;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator[](const int index)
	{
		iterator ite(m_con->m_root, false);
		ite += index;
		return ite;
	}
#endif//GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE
	//比較オペレータ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator==(const typename container<OPE_TYPE>::iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			ope_type::eq(*m_value, *rhs);
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator!=(const typename container<OPE_TYPE>::iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? false :
			m_isEnd || rhs.m_isEnd ? true :
			ope_type::ne(*m_value, *rhs);
	}
#ifdef GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator>(const typename container<OPE_TYPE>::iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && !rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			ope_type::gt(*m_value, *rhs);
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator>=(const typename container<OPE_TYPE>::iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? true :
			m_isEnd && !rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			ope_type::ge(*m_value, *rhs);
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator<(const typename container<OPE_TYPE>::iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			!m_isEnd && rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			ope_type::lt(*m_value, *rhs);
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::operator<=(const typename container<OPE_TYPE>::iterator& rhs) const
	{
		return !isEnabled() || !rhs.isEnabled() ? false :
			m_isEnd && rhs.m_isEnd ? true :
			!m_isEnd && rhs.m_isEnd ? true :
			m_isEnd || rhs.m_isEnd ? false :
			ope_type::le(*m_value, *rhs);
	}
#endif//GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE
	//演算オペレータ
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator++() const
	{
		updateNext();
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator--() const
	{
		updatePrev();
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator++()
	{
		updateNext();
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator--()
	{
		updatePrev();
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator++(int) const
	{
		iterator ite(*this);
		++(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator--(int) const
	{
		iterator ite(*this);
		--(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator++(int)
	{
		iterator ite(*this);
		++(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator--(int)
	{
		iterator ite(*this);
		--(*this);
		return ite;
	}
#ifdef GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator+=(const int rhs) const
	{
		updateForward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator-=(const int rhs) const
	{
		updateBackward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator+=(const int rhs)
	{
		updateForward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator& container<OPE_TYPE>::iterator::operator-=(const int rhs)
	{
		updateBackward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator+(const int rhs) const
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator-(const int rhs) const
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator+(const int rhs)
	{
		iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::iterator::operator-(const int rhs)
	{
		iterator ite(*this);
		ite -= rhs;
		return ite;
	}
	//template<class OPE_TYPE>
	//inline int container<OPE_TYPE>::iterator::operator-(const iterator& rhs) const
	//{
	//	return ???;
	//}
#endif//GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE
	//アクセッサ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isExist() const
	{
		return m_value != nullptr;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isEnabled() const
	{
		return m_value != nullptr || m_isEnd;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::iterator::isEnd() const//終端か？
	{
		return m_isEnd;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::iterator::getValue() const//現在の値（ノード）
	{
		return m_value;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::iterator::getValue()//現在の値（ノード）
	{
		return m_value;
	}

	//--------------------
	//リバースイテレータのインライン関数
	
	//基本オペレータ
#ifdef GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator[](const int index) const
	{
		reverse_iterator ite(m_con->m_root, false);
		ite += index;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator[](const int index)
	{
		reverse_iterator ite(m_con->m_root, false);
		ite += index;
		return ite;
	}
#endif//GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE
	//比較オペレータ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator==(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			ope_type::eq(*rhs, *m_value);
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator!=(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? false :
			rhs.m_isEnd || m_isEnd ? true :
			ope_type::ne(*rhs, *m_value);
	}
#ifdef GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator>(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && !m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			ope_type::gt(*rhs, *m_value);
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator>=(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? true :
			rhs.m_isEnd && !m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			ope_type::ge(*rhs, *m_value);
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator<(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			!rhs.m_isEnd && m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			ope_type::lt(*rhs, *m_value);
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::operator<=(const typename container<OPE_TYPE>::reverse_iterator& rhs) const
	{
		return !rhs.isEnabled() || !isEnabled() ? false :
			rhs.m_isEnd && m_isEnd ? true :
			!rhs.m_isEnd && m_isEnd ? true :
			rhs.m_isEnd || m_isEnd ? false :
			ope_type::le(*rhs, *m_value);
	}
#endif//GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE
	//演算オペレータ
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator++() const
	{
		updateNext();
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator--() const
	{
		updatePrev();
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator++()
	{
		updateNext();
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator--()
	{
		updatePrev();
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator++(int) const
	{
		reverse_iterator ite(*this);
		++(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator--(int) const
	{
		reverse_iterator ite(*this);
		--(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator++(int)
	{
		reverse_iterator ite(*this);
		++(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator--(int)
	{
		reverse_iterator ite(*this);
		--(*this);
		return ite;
	}
#ifdef GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE//std::bidirectional_iterator_tag には本来必要ではない
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator+=(const int rhs) const
	{
		updateForward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator-=(const int rhs) const
	{
		updateBackward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator+=(const int rhs)
	{
		updateForward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator& container<OPE_TYPE>::reverse_iterator::operator-=(const int rhs)
	{
		updateBackward(rhs);
		return *this;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator+(const int rhs) const
	{
		reverse_iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator-(const int rhs) const
	{
		reverse_iterator ite(*this);
		ite -= rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator+(const int rhs)
	{
		reverse_iterator ite(*this);
		ite += rhs;
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::reverse_iterator container<OPE_TYPE>::reverse_iterator::operator-(const int rhs)
	{
		reverse_iterator ite(*this);
		ite -= rhs;
		return ite;
	}
	//template<class OPE_TYPE>
	//inline int container<OPE_TYPE>::reverse_iterator::operator-(const reverse_iterator& rhs)
	//{
	//	return ???;
	//}
#endif//GASHA_RB_TREE_ENABLE_RANDOM_ACCESS_INTERFACE
	//アクセッサ
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isExist() const
	{
		return m_value != nullptr;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isEnabled() const
	{
		return m_value != nullptr || m_isEnd;
	}
	template<class OPE_TYPE>
	inline bool container<OPE_TYPE>::reverse_iterator::isEnd() const//終端か？
	{
		return m_isEnd;
	}
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::reverse_iterator::getValue() const//現在の値（ノード）
	{
		return m_value;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::value_type* container<OPE_TYPE>::reverse_iterator::getValue()//現在の値（ノード）
	{
		return m_value;
	}
	//ベースを取得
	template<class OPE_TYPE>
	inline const typename container<OPE_TYPE>::iterator container<OPE_TYPE>::reverse_iterator::base() const
	{
		iterator ite(*this);
		return ite;
	}
	template<class OPE_TYPE>
	inline typename container<OPE_TYPE>::iterator container<OPE_TYPE>::reverse_iterator::base()
	{
		iterator ite(*this);
		return ite;
	}

	//----------------------------------------
	//コンテナ本体のメソッド

	//配列の再割り当て
//	template<class OPE_TYPE>
//	template<std::size_t N>
//	inline void container<OPE_TYPE>::assignArray(typename container<OPE_TYPE>::value_type(&array)[N], const int size)
//	{
//	}

}//namespace rb_tree

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//__RB_TREE_INL_

// End of file

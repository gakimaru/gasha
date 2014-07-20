#pragma once
#ifndef GASHA_INCLUDED_PROFILER_H
#define GASHA_INCLUDED_PROFILER_H

//--------------------------------------------------------------------------------
// profiler.h
// プロファイラ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_stack_allocator.h>//ロックフリースタックアロケータ
#include <gasha/lf_pool_allocator.h>//ロックフリープールアロケータ
#include <gasha/hash_table.h>//開番地法ハッシュテーブル
#include <gasha/rb_tree.h>//赤黒木
#include <gasha/singly_linked_list.h>//片方向連結リスト
#include <gasha/shared_spin_lock.h>//共有スピンロック
#include <gasha/crc32.h>//CRC32
#include <gasha/chrono.h>//時間処理ユーティリティ
#include <gasha/thread_id.h>//スレッドID

#include <cstdint>//C++11 std::uint32_t

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <mutex> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <mutex>//C++11 std::call_once
GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//プロファイラ
//--------------------------------------------------------------------------------

//【注意】
//※threadId で名前を付けていないスレッドはプロファイルを記録できないので注意。

//----------------------------------------
//プロファイラクラス
class profiler
{
public:
	//定数
	static const std::size_t STR_POOL_BUFF_SIZE = GASHA_PROFILER_STR_POOL_BUFF_SIZE;//文字列プールバッファサイズ（バイト数） ※全スレッド名＋全処理名を登録
	static const std::size_t STR_POOL_TABLE_SIZE = GASHA_PROFILER_STR_POOL_TABLE_SIZE;//文字列プールテーブルサイズ（個数） ※全スレッド名＋全処理名を登録
	static const std::size_t PROFILE_INFO_POOL_SIZE = GASHA_PROFILER_PROFILE_INFO_POOL_SIZE;//プロファイル情報プールサイズ（個数） ※全スレッドの全処理の処理時間計測結果を記録
	static const std::size_t THREAD_INFO_TABLE_SIZE = GASHA_PROFILER_THREAD_INFO_TABLE_SIZE;//スレッド情報テーブルサイズ（個数） ※全スレッドのスレッド情報を記録
	//集計種別
	enum profileSumup_type
	{
		isNormal,//集計のみ
		withUpdatePeriod,//期間集計の更新を同時に行う
	};
	//プロファイル情報順序
	enum profileOrder_type
	{
		//現在（のフレーム）の処理時間でソート
		ascOfTime,//処理時間昇順
		descOfTime,//処理時間降順
		ascOfAvgTime,//平均処理時間昇順
		descOfAvgTime,//平均処理時間降順
		ascOfMaxTime,//最長処理時間昇順
		descOfMaxTime,//最長処理時間降順 ※デフォルト
		ascOfMinTime,//最短処理時間昇順
		descOfMinTime,//最短処理時間降順
		ascOfCount,//計測回数昇順
		descOfCount,//計測回数降順

		//全体集計処理時間でソート
		ascOfTotalTime,//処理時間昇順
		descOfTotalTime,//処理時間降順
		ascOfAvgTotalTime,//平均処理時間昇順
		descOfAvgTotalTime,//平均処理時間降順
		ascOfMaxTotalTime,//最長処理時間昇順
		descOfMaxTotalTime,//最長処理時間降順
		ascOfMinTotalTime,//最短処理時間昇順
		descOfMinTotalTime,//最短処理時間降順
		ascOfTotalCount,//計測回数昇順
		descOfTotalCount,//計測回数降順
		ascOfMaxTotalCount,//最大計測回数昇順
		descOfMaxTotalCount,//最大計測回数降順
		ascOfMinTotalCount,//最小計測回数昇順
		descOfMinTotalCount,//最小計測回数降順
		ascOfAvgTotalCount,//平均計測回数昇順
		descOfAvgTotalCount,//平均計測回数降順

		//期間集計処理時間でソート
		ascOfPeriodicTime,//処理時間昇順
		descOfPeriodicTime,//処理時間降順
		ascOfAvgPeriodicTime,//平均処理時間昇順
		descOfAvgPeriodicTime,//平均処理時間降順
		ascOfMaxPeriodicTime,//最長処理時間昇順
		descOfMaxPeriodicTime,//最長処理時間降順
		ascOfMinPeriodicTime,//最短処理時間昇順
		descOfMinPeriodicTime,//最短処理時間降順
		ascOfPeriodicCount,//計測回数昇順
		descOfPeriodicCount,//計測回数降順
		ascOfMaxPeriodicCount,//最大計測回数昇順
		descOfMaxPeriodicCount,//最大計測回数降順
		ascOfMinPeriodicCount,//最小計測回数昇順
		descOfMinPeriodicCount,//最小計測回数降順
		ascOfAvgPeriodicCount,//平均計測回数昇順
		descOfAvgPeriodicCount,//平均計測回数降順
	};
public:
	//型
	struct explicitInit_type{};//明示的な初期化用構造体
public:
	//クラス／構造体宣言
	struct strPoolInfo;
	class timeInfo;
	class profileInfo;
	class threadInfo;
	struct profileInfoOpe;
	struct threadInfoOpe;
	struct threadInfoListOpe;
public:
	//----------------------------------------
	//文字列プール情報
	struct strPoolInfo
	{
		//フィールド
		GASHA_ crc32_t m_strCrc;//文字列のCRC（キー）
		const char* m_str;//文字列
	public:
		//比較オペレータ
		inline bool operator==(const strPoolInfo& rhs) const { return m_strCrc == rhs.m_strCrc; }
		inline bool operator!=(const strPoolInfo& rhs) const { return m_strCrc != rhs.m_strCrc; }
		inline bool operator<(const strPoolInfo& rhs) const { return m_strCrc < rhs.m_strCrc; }
	public:
		//ムーブオペレータ
		inline strPoolInfo& operator=(strPoolInfo&& rhs);
		//コピーオペレータ
		inline strPoolInfo& operator=(const strPoolInfo& rhs);
	public:
		//ムーブコンストラクタ
		inline strPoolInfo(strPoolInfo&& obj);
		//コピーコンストラクタ
		inline strPoolInfo(const strPoolInfo& obj);
		//コンストラクタ
		inline strPoolInfo(const GASHA_ crc32_t str_crc, const char* str);
		//デフォルトコンストラクタ
		inline strPoolInfo();
		//デストラクタ
		inline ~strPoolInfo();
	};
public:
	//----------------------------------------
	//文字列プール操作型
	struct strPoolOpe : public GASHA_ hash_table::baseOpe<strPoolOpe, strPoolInfo, GASHA_ crc32_t>
	{
		typedef GASHA_ sharedSpinLock lock_type;//ロック型

		//キーを取得
		inline static key_type getKey(const value_type& value){ return value.m_strCrc; }
	};
public:
	typedef GASHA_ lfStackAllocator_withBuff<STR_POOL_BUFF_SIZE> strPoolBuff_type;//文字列プールバッファ型
	typedef GASHA_ hash_table::container<strPoolOpe, STR_POOL_TABLE_SIZE> strPoolTable_type;//文字列プールテーブル型
public:
	//----------------------------------------
	//処理時間情報
	class timeInfo
	{
	#ifdef GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

		friend class profiler;
		friend class profileInfo;
		friend class summarizedTimeInfo;
		friend class threadInfo;
	public:
		//アクセッサ
		inline GASHA_ sec_t time() const { return m_time; }//処理時間
		inline GASHA_ sec_t maxTime() const { return m_maxTime; }//最長処理時間
		inline GASHA_ sec_t minTime() const { return m_minTime; }//最短処理時間
		inline GASHA_ sec_t avgTime() const { return m_avgTime; }//平均処理時間
		inline int count() const { return m_count; }//計測回数
	private:
		//メソッド
		bool add(const GASHA_ sec_t time);//処理時間加算
		bool sumup(const timeInfo& time_info);//処理時間集計
		inline bool clear();//クリア
	public:
		//ムーブオペレータ
		inline timeInfo& operator=(timeInfo&& rhs);
		//コピーオペレータ
		inline timeInfo& operator=(const timeInfo& rhs);
	public:
		//ムーブコンストラクタ
		inline timeInfo(timeInfo&& obj);
		//コピーコンストラクタ
		inline timeInfo(const timeInfo& obj);
		//デフォルトコンストラクタ
		inline timeInfo();
		//デストラクタ
		inline ~timeInfo();
	private:
		//フィールド
		GASHA_ sec_t m_time;//処理時間
		GASHA_ sec_t m_maxTime;//最長処理時間
		GASHA_ sec_t m_minTime;//最短処理時間
		GASHA_ sec_t m_avgTime;//平均処理時間
		int m_count;//計測回数

	#else//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

	public:
		//アクセッサ
		inline GASHA_ sec_t time() const { return static_cast<GASHA_ sec_t>(0.); }//処理時間
		inline GASHA_ sec_t maxTime() const { return static_cast<GASHA_ sec_t>(0.); }//最長処理時間
		inline GASHA_ sec_t minTime() const { return static_cast<GASHA_ sec_t>(0.); }//最短処理時間
		inline GASHA_ sec_t avgTime() const { return static_cast<GASHA_ sec_t>(0.); }//平均処理時間
		inline int count() const { return 0; }//計測回数
	public:
		inline timeInfo& operator=(timeInfo&& rhs){ return *this; }//ムーブオペレータ
		inline timeInfo& operator=(const timeInfo& rhs){ return *this; }//コピーオペレータ
	public:
		inline timeInfo(timeInfo&& obj){}//ムーブコンストラクタ
		inline timeInfo(const timeInfo& obj){}//コピーコンストラクタ
		inline timeInfo(){}//デフォルトコンストラクタ
		inline ~timeInfo(){}//デストラクタ

	#endif//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化
	};
	//----------------------------------------
	//処理時間集計情報
	class summarizedTimeInfo
	{
	#ifdef GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

		friend class profiler;
		friend class profileInfo;
		friend class threadInfo;
	public:
		//アクセッサ
		inline GASHA_ sec_t time() const { return m_time.m_time; }//処理時間
		inline GASHA_ sec_t maxTime() const { return m_time.m_maxTime; }//最長処理時間
		inline GASHA_ sec_t minTime() const { return m_time.m_minTime; }//最短処理時間
		inline GASHA_ sec_t avgTime() const { return m_time.m_minTime; }//平均処理時間
		inline int count() const { return m_time.m_count; }//計測回数
		inline int maxCount() const { return m_maxCount; }//最大計測回数
		inline int minCount() const { return m_minCount; }//最小計測回数
		inline float avgCount() const { return m_avgCount; }//平均計測回数
		inline int summarizedCount() const { return m_summarizedCount; }//集計回数
	private:
		//メソッド
		bool sumup(const timeInfo& time_info);//処理時間集計
		inline bool clear();//クリア
	public:
		//ムーブオペレータ
		inline summarizedTimeInfo& operator=(summarizedTimeInfo&& rhs);
		//コピーオペレータ
		inline summarizedTimeInfo& operator=(const summarizedTimeInfo& rhs);
	public:
		//ムーブコンストラクタ
		inline summarizedTimeInfo(summarizedTimeInfo&& obj);
		//コピーコンストラクタ
		inline summarizedTimeInfo(const summarizedTimeInfo& obj);
		//デフォルトコンストラクタ
		inline summarizedTimeInfo();
		//デストラクタ
		inline ~summarizedTimeInfo();
	private:
		//フィールド
		timeInfo m_time;//処理時間情報
		int m_maxCount;//最大計測回数
		int m_minCount;//最小計測回数
		float m_avgCount;//平均計測回数
		int m_summarizedCount;//集計回数

	#else//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

	public:
		//アクセッサ
		inline GASHA_ sec_t time() const { return static_cast<GASHA_ sec_t>(0.); }//処理時間
		inline GASHA_ sec_t maxTime() const { return static_cast<GASHA_ sec_t>(0.); }//最長処理時間
		inline GASHA_ sec_t minTime() const { return static_cast<GASHA_ sec_t>(0.); }//最短処理時間
		inline GASHA_ sec_t avgTime() const { return static_cast<GASHA_ sec_t>(0.); }//平均処理時間
		inline int count() const { return 0; }//計測回数
		inline int maxCount() const { return 0; }//最大計測回数
		inline int minCount() const { return 0; }//最小計測回数
		inline float avgCount() const { return 0.f; }//平均計測回数
		inline int summarizedCount() const { return 0; }//集計回数
	public:
		inline summarizedTimeInfo& operator=(summarizedTimeInfo&& rhs){ return *this; }//ムーブオペレータ
		inline summarizedTimeInfo& operator=(const summarizedTimeInfo& rhs){ return *this; }//コピーオペレータ
	public:
		inline summarizedTimeInfo(summarizedTimeInfo&& obj){}//ムーブコンストラクタ
		inline summarizedTimeInfo(const summarizedTimeInfo& obj){}//コピーコンストラクタ
		inline summarizedTimeInfo(){}//デフォルトコンストラクタ
		inline ~summarizedTimeInfo(){}//デストラクタ

	#endif//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化
	};
public:
	//----------------------------------------
	//プロファイル情報
	class profileInfo
	{
	#ifdef GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

		friend class profiler;
		friend struct profileInfoOpe;
		friend class threadInfo;
	public:
		//アクセッサ
		inline GASHA_ crc32_t nameCrc() const { return m_nameCrc; }//名前のCRC
		inline const char* name() const { return m_name; }//名前
		inline const timeInfo& time() const { return m_time; }//処理時間
		inline const summarizedTimeInfo& totalTime() const { return m_totalTime; }//処理時間：全体集計
		inline const summarizedTimeInfo& periodicTime() const { return m_periodicTime; }//処理時間：期間集計
		//inline const summarizedTimeInfo& periodicTimeWork() const { return m_periodicTimeWork; }//処理時間：期間集計（集計中）
	private:
		//メソッド
		bool add(const GASHA_ sec_t time);//処理時間加算
		bool sumup(const profileSumup_type type);//処理時間集計
	public:
		//比較オペレータ
		inline bool operator==(const profileInfo& rhs) const { return m_nameCrc == rhs.m_nameCrc; }
		inline bool operator!=(const profileInfo& rhs) const { return m_nameCrc != rhs.m_nameCrc; }
		inline bool operator<(const profileInfo& rhs) const { return m_nameCrc < rhs.m_nameCrc; }
	public:
		//ムーブオペレータ
		inline profileInfo& operator=(profileInfo&& rhs);
		//コピーオペレータ
		inline profileInfo& operator=(const profileInfo& rhs);
	public:
		//ムーブコンストラクタ
		inline profileInfo(profileInfo&& obj);
		//コピーコンストラクタ
		inline profileInfo(const profileInfo& obj);
		//コンストラクタ
		inline profileInfo(GASHA_ crc32_t name_crc, const char* name);
		//デフォルトコンストラクタ
		inline profileInfo();
		//デストラクタ
		inline ~profileInfo();
	private:
		//フィールド
		GASHA_ crc32_t m_nameCrc;//名前のCRC
		const char* m_name;//名前
		timeInfo m_time;//処理時間
		summarizedTimeInfo m_totalTime;//処理時間：全体集計
		summarizedTimeInfo m_periodicTime;//処理時間：期間集計
		summarizedTimeInfo m_periodicTimeWork;//処理時間：期間集計（集計中）
		GASHA_ sharedSpinLock m_lock;//ロックオブジェクト
		mutable const profileInfo* m_childS;//子ノード（小）
		mutable const profileInfo* m_childL;//子ノード（大）
		mutable bool m_isBlack;//色
	
	#else//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化
	
	public:
		//アクセッサ
		inline GASHA_ crc32_t nameCrc() const { return 0; }//名前のCRC
		inline const char* name() const { return nullptr; }//名前
		inline const timeInfo& time() const { return m_time; }//処理時間
		inline const summarizedTimeInfo& totalTime() const { return m_summarizedTime; }//処理時間：全体集計
		inline const summarizedTimeInfo& periodicTime() const { return m_summarizedTime; }//処理時間：期間集計
		//inline const summarizedTimeInfo& periodicTimeWork() const { return m_summarizedTime; }//処理時間：期間集計（集計中）
	public:
		//比較オペレータ
		inline bool operator==(const profileInfo& rhs) const { return true; }
		inline bool operator!=(const profileInfo& rhs) const { return false; }
		inline bool operator<(const profileInfo& rhs) const { return true; }
	public:
		inline profileInfo& operator=(profileInfo&& rhs){ return *this; }//ムーブオペレータ
		inline profileInfo& operator=(const profileInfo& rhs){ return *this; }//コピーオペレータ
	public:
		inline profileInfo(profileInfo&& obj){}//ムーブコンストラクタ
		inline profileInfo(const profileInfo& obj){}//コピーコンストラクタ
		inline profileInfo(GASHA_ crc32_t name_crc, const char* name){}//コンストラクタ
		inline profileInfo(){}//デフォルトコンストラクタ
		inline ~profileInfo(){}//デストラクタ
	private:
		//フィールド
		timeInfo m_time;//処理時間（ダミー）
		summarizedTimeInfo m_summarizedTime;//集計処理時間（ダミー）

	#endif//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化
	};
public:
	//----------------------------------------
	//プロファイル情報操作型
	struct profileInfoOpe : public GASHA_ rb_tree::baseOpe<profileInfoOpe, profileInfo>
	{
		typedef GASHA_ sharedSpinLock lock_type;//ロック型

	#ifdef GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化
		
		//子ノードを取得
		inline static const node_type* getChildL(const node_type& node){ return node.m_childS; }//大（右）側
		inline static const node_type* getChildS(const node_type& node){ return node.m_childL; }//小（左）側
		//子ノードを変更
		inline static void setChildL(node_type& node, const node_type* child){ node.m_childS = child; }//大（右）側
		inline static void setChildS(node_type& node, const node_type* child){ node.m_childL = child; }//小（左）側

		//ノードの色を取得
		inline static color_t getColor(const node_type& node){ return node.m_isBlack ? rb_tree::BLACK : rb_tree::RED; }
		//ノードの色を変更
		inline static void setColor(node_type& node, const color_t color){ node.m_isBlack = color == rb_tree::BLACK; }

		//キーを取得
		inline static key_type getKey(const node_type& node){ return node.m_nameCrc; }
	
	#else//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化
	
		//子ノードを取得
		inline static const node_type* getChildL(const node_type& node){ return nullptr; }//大（右）側
		inline static const node_type* getChildS(const node_type& node){ return nullptr; }//小（左）側
		//子ノードを変更
		inline static void setChildL(node_type& node, const node_type* child){}//大（右）側
		inline static void setChildS(node_type& node, const node_type* child){}//小（左）側

		//ノードの色を取得
		inline static color_t getColor(const node_type& node){ return rb_tree::RED; }
		//ノードの色を変更
		inline static void setColor(node_type& node, const color_t color){}

		//キーを取得
		inline static key_type getKey(const node_type& node){ return 0; }

	#endif//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化
	};
public:
	typedef GASHA_ rb_tree::container<profileInfoOpe> profileInfoTree_type;//プロファイル情報木型
	typedef GASHA_ lfPoolAllocator_withType<profileInfo, PROFILE_INFO_POOL_SIZE> profileInfoPool_type;//プロファイル情報プール型
public:
	//----------------------------------------
	//スレッド情報
	class threadInfo
	{
	#ifdef GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

		friend class profiler;
		friend struct threadInfoOpe;
		friend struct threadInfoListOpe;
	public:
		//アクセッサ
		inline GASHA_ crc32_t threadNameCrc() const { return m_threadNameCrc; }//スレッド名のCRC
		inline const char* threadName() const { return m_threadName; }//スレッド名
		//inline const profileInfoTree_type& profileInfoRoot() const { return m_profileInfoTree; }//プロファイル情報木の根
	public:
		//メソッド
		inline const profileInfo* refProfile(const GASHA_ crc32_t name_crc) const;//プロファイル情報参照
	private:
		profileInfo* regProfile(const GASHA_ crc32_t name_crc, const char* name, profileInfoPool_type& pool);//プロファイル情報登録
		inline profileInfo* refProfile(const GASHA_ crc32_t name_crc);//プロファイル情報参照
		bool add(const strPoolInfo& pooled_name, const GASHA_ sec_t elapsed_time, profileInfoPool_type& pool);//処理時間加算
		bool sumup(const profileSumup_type type);//処理時間集計
	public:
		//プロファイル情報を取得
		//※情報を受け取るための配列を渡す
		//※取得した情報数を返す
		//※プロファイル情報の並び順を指定する
		template<std::size_t N>
		inline std::size_t getProfileInfo(profileInfo(&array)[N], const profileOrder_type order = descOfMaxTime);
		std::size_t getProfileInfo(profileInfo* array, const std::size_t max_size, const profileOrder_type order = descOfMaxTime);
	public:
		//比較オペレータ
		inline bool operator==(const threadInfo& rhs) const { return m_threadNameCrc == rhs.m_threadNameCrc; }
		inline bool operator!=(const threadInfo& rhs) const { return m_threadNameCrc != rhs.m_threadNameCrc; }
		inline bool operator<(const threadInfo& rhs) const { return m_threadNameCrc < rhs.m_threadNameCrc; }
	public:
		//ムーブオペレータ
		inline threadInfo& operator=(threadInfo&& rhs);
		//コピーオペレータ
		inline threadInfo& operator=(const threadInfo& rhs);
	public:
		//ムーブコンストラクタ
		inline threadInfo(threadInfo&& obj);
		//コピーコンストラクタ
		inline threadInfo(const threadInfo& obj);
		//コンストラクタ
		inline threadInfo(const GASHA_ crc32_t name_crc, const char* name);
		//デフォルトコンストラクタ
		inline threadInfo();
		//デストラクタ
		inline ~threadInfo();
	private:
		//フィールド
		GASHA_ crc32_t m_threadNameCrc;//スレッド名のCRC
		const char* m_threadName;//スレッド名
		profileInfoTree_type m_profileInfoTree;//プロファイル情報木
		mutable const threadInfo* m_next;//次のスレッド情報への連結

	#else//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

	public:
		//アクセッサ
		inline GASHA_ crc32_t threadNameCrc() const { return 0; }//スレッド名のCRC
		inline const char* threadName() const { return nullptr; }//スレッド名
		//inline const profileInfoTree_type& profileInfoRoot() const { return ???; }//プロファイル情報木の根
	public:
		//メソッド
		const profileInfo* refProfile(const GASHA_ crc32_t name_crc) const{ return nullptr; }//プロファイル情報参照
	public:
		//プロファイル情報を取得
		template<std::size_t N>
		inline std::size_t getProfileInfo(profileInfo(&array)[N], const profileOrder_type order = descOfMaxTime){ return 0; }
		inline std::size_t getProfileInfo(profileInfo* array, const std::size_t max_size, const profileOrder_type order = descOfMaxTime){ return 0; }
	public:
		//比較オペレータ
		inline bool operator==(const threadInfo& rhs) const { return true; }
		inline bool operator!=(const threadInfo& rhs) const { return false; }
		inline bool operator<(const threadInfo& rhs) const { return true; }
	public:
		inline threadInfo& operator=(threadInfo&& rhs){ return *this; }//ムーブオペレータ
		inline threadInfo& operator=(const threadInfo& rhs){ return *this; }//コピーオペレータ
	public:
		inline threadInfo(threadInfo&& obj){}//ムーブコンストラクタ
		inline threadInfo(const threadInfo& obj){}//コピーコンストラクタ
		inline threadInfo(const GASHA_ crc32_t name_crc, const char* name){}//コンストラクタ
		inline threadInfo(){}//デフォルトコンストラクタ
		inline ~threadInfo(){}//デストラクタ

	#endif//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化
	};
public:
	//----------------------------------------
	//スレッド情報操作型
	struct threadInfoOpe : public GASHA_ hash_table::baseOpe<threadInfoOpe, threadInfo, GASHA_ crc32_t>
	{
		typedef GASHA_ sharedSpinLock lock_type;//ロック型

	#ifdef GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化
		
		//キーを取得
		inline static key_type getKey(const value_type& value){ return value.m_threadNameCrc; }
	
	#else//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化
	
		//キーを取得
		inline static key_type getKey(const value_type& value){ return 0; }

	#endif//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化
	};
	//----------------------------------------
	//スレッド情報連結リスト操作型
	struct threadInfoListOpe : public GASHA_ singly_linked_list::baseOpe<threadInfoListOpe, threadInfo>
	{
		typedef GASHA_ sharedSpinLock lock_type;//ロック型
		
	#ifdef GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化
		
		//次ノードを取得
		inline static const node_type* getNext(const node_type& node){ return node.m_next; }
		//次ノードを変更
		inline static void setNext(node_type& node, const node_type* next){ node.m_next = next; }
	
	#else//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

		//次ノードを取得
		inline static const node_type* getNext(const node_type& node){ return nullptr; }
		//次ノードを変更
		inline static void setNext(node_type& node, const node_type* next){}

	#endif//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化
	};
public:
	typedef GASHA_ hash_table::container<threadInfoOpe, THREAD_INFO_TABLE_SIZE> threadInfoTable_type;//スレッド情報テーブル型
	typedef GASHA_ singly_linked_list::container<threadInfoListOpe> threadInfoLink_type;//スレッド情報連結リスト型

#ifdef GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

public:
	//メソッド

	//処理時間加算
	//※スレッドと名前に対して、処理の回数と時間を記録する。
	bool add(const char* name, const GASHA_ sec_t elapsed_time);
	
	//処理時間集計
	//※フレームごとの集計に用いる。
	//※自スレッドのみを集計する。
	//※ with_update_period に true を指定すると、期間集計を更新する。
	//※期間集計は、リアルタイムに状況をモニターする際に利用する。
	//　毎フレーム目まぐるしく情報が変動していては情報を目視しきれないため、
	//　例えば1秒(30/60frame)置きに with_upate_period を指定して情報を更新する。
	//※期間集計は、表示用の情報と毎フレームの集計用の情報（ワーク）の二つの情報を管理し、
	//　 with_update_period の指定タイミングで表示用にコピーした上で、集計をリセットする。
	bool sumup(const profileSumup_type type);

	//スレッド情報を取得
	//※情報を受け取るための配列を渡す
	//※取得した情報数を返す
	//※スレッド情報の並びは登録順（プロファイリングが行われたスレッド順）
	template<std::size_t N>
	inline std::size_t getThreadInfo(threadInfo(&array)[N]);
	std::size_t getThreadInfo(threadInfo* array, std::size_t max_size);

	//プロファイル情報を取得
	//※対象となるスレッド名のCRCを指定する
	//※情報を受け取るための配列を渡す
	//※取得した情報数を返す
	//※プロファイル情報の並び順を指定する
	template<std::size_t N>
	inline std::size_t getProfileInfo(const GASHA_ crc32_t thread_name_crc, profileInfo(&array)[N], const profileOrder_type order = descOfMaxTime);
	template<std::size_t N>
	inline std::size_t getProfileInfo(const char* thread_name, profileInfo(&array)[N], const profileOrder_type order = descOfMaxTime);
	template<std::size_t N>
	inline std::size_t getProfileInfo(const threadInfo& thread_info, profileInfo(&array)[N], const profileOrder_type order = descOfMaxTime);
	template<std::size_t N>
	inline std::size_t getProfileInfo(GASHA_ threadId&& thread_id, profileInfo(&array)[N], const profileOrder_type order = descOfMaxTime);
	template<std::size_t N>
	inline std::size_t getProfileInfo(const GASHA_ threadId& thread_id, profileInfo(&array)[N], const profileOrder_type order = descOfMaxTime);
	std::size_t getProfileInfo(const GASHA_ crc32_t thread_name_crc, profileInfo* array, const std::size_t max_size, const profileOrder_type order = descOfMaxTime);
	inline std::size_t getProfileInfo(const char* thread_name, profileInfo* array, const std::size_t max_size, const profileOrder_type order = descOfMaxTime);
	inline std::size_t getProfileInfo(const threadInfo& thread_info, profileInfo* array, const std::size_t max_size, const profileOrder_type order = descOfMaxTime);
	inline std::size_t getProfileInfo(GASHA_ threadId&& thread_id, profileInfo* array, const std::size_t max_size, const profileOrder_type order = descOfMaxTime);
	inline std::size_t getProfileInfo(const GASHA_ threadId& thread_id, profileInfo* array, const std::size_t max_size, const profileOrder_type order = descOfMaxTime);

	//【使用注意】クリア
	//※強制的に記録を全てをクリアするので注意。
	//※スレッドセーフではないので注意。
	inline void clear();

private:
	//文字列プール登録
	const strPoolInfo* regStrPool(const char* name, GASHA_ crc32_t& name_crc);
	//文字列プール参照
	inline const strPoolInfo* refStrPool(const GASHA_ crc32_t name_crc);
	//スレッド情報登録
	threadInfo* regThread(const GASHA_ threadId& thread_id);
	//スレッド情報参照
	inline threadInfo* refThread(const GASHA_ crc32_t thread_name_crc);

private:
	//初期化メソッド（一回限り）
	static void initializeOnce();

public:
	//明示的な初期化用コンストラクタ
	inline profiler(const explicitInit_type&);
	//デフォルトコンストラクタ
	profiler();
	//デストラクタ
	inline ~profiler();

private:
	//静的フィールド
	static std::once_flag m_initialized;//初期化済み
	static strPoolBuff_type m_strPoolBuff;//文字列プールバッファ
	static strPoolTable_type m_strPoolTable;//文字列プールテーブル
	static profileInfoPool_type m_profileInfoPool;//プロファイル情報プール
	static threadInfoTable_type m_threadInfoTable;//スレッド情報テーブル
	static threadInfoLink_type m_threadInfoList;//スレッド情報連結リスト

#else//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

public:
	inline bool add(const char* name, const GASHA_ sec_t elapsed_time){ return true; }//処理時間加算
	inline bool sumup(const profileSumup_type type){ return true; }//処理時間集計
	template<std::size_t N>
	inline std::size_t getThreadInfo(threadInfo(&array)[N]){ return 0; }//スレッド情報を取得
	inline std::size_t getThreadInfo(threadInfo* array, std::size_t max_size){ return 0; }//スレッド情報を取得
	template<std::size_t N>
	inline std::size_t getProfileInfo(const GASHA_ crc32_t thread_name_crc, profileInfo(&array)[N], const profileOrder_type order = descOfMaxTime){ return 0; }//プロファイル情報を取得
	template<std::size_t N>
	inline std::size_t getProfileInfo(const char* thread_name, profileInfo(&array)[N], const profileOrder_type order = descOfMaxTime){ return 0; }//プロファイル情報を取得
	template<std::size_t N>
	inline std::size_t getProfileInfo(const threadInfo& thread_info, profileInfo(&array)[N], const profileOrder_type order = descOfMaxTime){ return 0; }//プロファイル情報を取得
	template<std::size_t N>
	inline std::size_t getProfileInfo(GASHA_ threadId&& thread_id, profileInfo(&array)[N], const profileOrder_type order = descOfMaxTime){ return 0; }//プロファイル情報を取得
	template<std::size_t N>
	inline std::size_t getProfileInfo(const GASHA_ threadId& thread_id, profileInfo(&array)[N], const profileOrder_type order = descOfMaxTime){ return 0; }//プロファイル情報を取得
	inline std::size_t getProfileInfo(const GASHA_ crc32_t thread_name_crc, profileInfo* array, const std::size_t max_size, const profileOrder_type order = descOfMaxTime){ return 0; }//プロファイル情報を取得
	inline std::size_t getProfileInfo(const char* thread_name, profileInfo* array, const std::size_t max_size, const profileOrder_type order = descOfMaxTime){ return 0; }//プロファイル情報を取得
	inline std::size_t getProfileInfo(const threadInfo& thread_info, profileInfo* array, const std::size_t max_size, const profileOrder_type order = descOfMaxTime){ return 0; }//プロファイル情報を取得
	inline std::size_t getProfileInfo(GASHA_ threadId&& thread_id, profileInfo* array, const std::size_t max_size, const profileOrder_type order = descOfMaxTime){ return 0; }//プロファイル情報を取得
	inline std::size_t getProfileInfo(const GASHA_ threadId& thread_id, profileInfo* array, const std::size_t max_size, const profileOrder_type order = descOfMaxTime){ return 0; }//プロファイル情報を取得
	inline void clear(){}//クリア
public:
	inline profiler(const explicitInit_type&){}//明示的な初期化用コンストラクタ
	inline profiler(){}//デフォルトコンストラクタ
	inline ~profiler(){}//デストラクタ

#endif//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

public:
	//静的フィールド
	static const explicitInit_type explicitInit;//明示的な初期化指定用
};

GASHA_NAMESPACE_END;//ネームスペース：終了

//【VC++】ワーニング設定を復元
#pragma warning(pop)

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/profiler.inl>

#endif//GASHA_INCLUDED_PROFILER_H

// End of file

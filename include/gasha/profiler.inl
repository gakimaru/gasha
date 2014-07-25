#pragma once
#ifndef GASHA_INCLUDED_PROFILER_INL
#define GASHA_INCLUDED_PROFILER_INL

//--------------------------------------------------------------------------------
// profiler.inl
// プロファイラ【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/profiler.h>//プロファイラ【宣言部】

#include <utility>//C++11 std::move()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//プロファイラ
//--------------------------------------------------------------------------------

#ifdef GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

//----------------------------------------
//文字列プール情報

//ムーブオペレータ
inline profiler::strPoolInfo& profiler::strPoolInfo::operator=(profiler::strPoolInfo&& rhs)
{
	m_strCrc = rhs.m_strCrc;
	m_str = rhs.m_str;
	return *this;
}

//コピーオペレータ
inline profiler::strPoolInfo& profiler::strPoolInfo::operator=(const profiler::strPoolInfo& rhs)
{
	m_strCrc = rhs.m_strCrc;
	m_str = rhs.m_str;
	return *this;
}

//ムーブコンストラクタ
inline profiler::strPoolInfo::strPoolInfo(profiler::strPoolInfo&& obj) :
	m_strCrc(obj.m_strCrc),
	m_str(obj.m_str)
{}

//コピーコンストラクタ
inline profiler::strPoolInfo::strPoolInfo(const profiler::strPoolInfo& obj) :
	m_strCrc(obj.m_strCrc),
	m_str(obj.m_str)
{}

//コンストラクタ
inline profiler::strPoolInfo::strPoolInfo(const GASHA_ crc32_t str_crc, const char* str) :
	m_strCrc(str_crc),
	m_str(str)
{}

//デフォルトコンストラクタ
inline profiler::strPoolInfo::strPoolInfo()
{}

//デストラクタ
inline profiler::strPoolInfo::~strPoolInfo()
{}

//----------------------------------------
//処理時間情報

//平均算出
inline void profiler::timeInfo::calcAvg()
{
	if (m_count > 0)
		m_avgTime = m_time / static_cast<GASHA_ sec_t>(m_count);
}

//クリア
inline bool profiler::timeInfo::clear()
{
	m_time = static_cast<GASHA_ sec_t>(0.);
	m_maxTime = static_cast<GASHA_ sec_t>(0.);
	m_minTime = static_cast<GASHA_ sec_t>(0.);
	m_avgTime = static_cast<GASHA_ sec_t>(0.);
	m_count = 0;
	return true;
}

//ムーブオペレータ
inline profiler::timeInfo& profiler::timeInfo::operator=(profiler::timeInfo&& rhs)
{
	m_time = rhs.m_time;
	m_maxTime = rhs.m_maxTime;
	m_minTime = rhs.m_minTime;
	m_avgTime = rhs.m_avgTime;
	m_count = rhs.m_count;
	return *this;
}

//コピーオペレータ
inline profiler::timeInfo& profiler::timeInfo::operator=(const profiler::timeInfo& rhs)
{
	m_time = rhs.m_time;
	m_maxTime = rhs.m_maxTime;
	m_minTime = rhs.m_minTime;
	m_avgTime = rhs.m_avgTime;
	m_count = rhs.m_count;
	return *this;
}

//ムーブコンストラクタ
inline profiler::timeInfo::timeInfo(profiler::timeInfo&& obj) :
	m_time(obj.m_time),
	m_maxTime(obj.m_maxTime),
	m_minTime(obj.m_minTime),
	m_avgTime(obj.m_avgTime),
	m_count(obj.m_count)
{}

//コピーコンストラクタ
inline profiler::timeInfo::timeInfo(const profiler::timeInfo& obj) :
	m_time(obj.m_time),
	m_maxTime(obj.m_maxTime),
	m_minTime(obj.m_minTime),
	m_avgTime(obj.m_avgTime),
	m_count(obj.m_count)
{}

//デフォルトコンストラクタ
inline profiler::timeInfo::timeInfo() :
	m_time(static_cast<sec_t>(0.)),
	m_maxTime(static_cast<sec_t>(0.)),
	m_minTime(static_cast<sec_t>(0.)),
	m_avgTime(static_cast<sec_t>(0.)),
	m_count(0)
{}

//デストラクタ
inline profiler::timeInfo::~timeInfo()
{}

//----------------------------------------
//処理時間集計情報

//平均算出
inline void profiler::summarizedTimeInfo::calcAvg()
{
	m_time.calcAvg();
	if (m_summarizedCount > 0)
		m_avgCount = static_cast<float>(static_cast<double>(m_time.m_count) / static_cast<double>(m_summarizedCount));
}

//クリア
inline bool profiler::summarizedTimeInfo::clear()
{
	m_time.clear();
	m_maxCount = 0;
	m_minCount = 0;
	m_avgCount = 0.f;
	m_summarizedCount = 0;
	return true;
}

//ムーブオペレータ
inline profiler::summarizedTimeInfo& profiler::summarizedTimeInfo::operator=(profiler::summarizedTimeInfo&& rhs)
{
	m_time = std::move(rhs.m_time);
	m_maxCount = rhs.m_maxCount;
	m_minCount = rhs.m_minCount;
	m_avgCount = rhs.m_avgCount;
	m_summarizedCount = rhs.m_summarizedCount;
	return *this;
}

//コピーオペレータ
inline profiler::summarizedTimeInfo& profiler::summarizedTimeInfo::operator=(const profiler::summarizedTimeInfo& rhs)
{
	m_time = rhs.m_time;
	m_maxCount = rhs.m_maxCount;
	m_minCount = rhs.m_minCount;
	m_avgCount = rhs.m_avgCount;
	m_summarizedCount = rhs.m_summarizedCount;
	return *this;
}

//ムーブコンストラクタ
inline profiler::summarizedTimeInfo::summarizedTimeInfo(profiler::summarizedTimeInfo&& obj) :
	m_time(std::move(obj.m_time)),
	m_maxCount(obj.m_maxCount),
	m_minCount(obj.m_minCount),
	m_avgCount(obj.m_avgCount),
	m_summarizedCount(obj.m_summarizedCount)
{}

//コピーコンストラクタ
inline profiler::summarizedTimeInfo::summarizedTimeInfo(const profiler::summarizedTimeInfo& obj) :
	m_time(obj.m_time),
	m_maxCount(obj.m_maxCount),
	m_minCount(obj.m_minCount),
	m_avgCount(obj.m_avgCount),
	m_summarizedCount(obj.m_summarizedCount)
{}

//デフォルトコンストラクタ
inline profiler::summarizedTimeInfo::summarizedTimeInfo() :
	m_time(),
	m_maxCount(0),
	m_minCount(0),
	m_avgCount(0.f),
	m_summarizedCount(0)
{}

//デストラクタ
inline profiler::summarizedTimeInfo::~summarizedTimeInfo()
{}

//----------------------------------------
//プロファイル情報

//ムーブオペレータ
inline profiler::profileInfo& profiler::profileInfo::operator=(profiler::profileInfo&& rhs)
{
	m_nameCrc = rhs.m_nameCrc;
	m_name = rhs.m_name;
	m_time = std::move(rhs.m_time);
	m_totalTime = std::move(rhs.m_totalTime);
	m_periodicTime = std::move(rhs.m_periodicTime);
	m_periodicTimeWork = std::move(rhs.m_periodicTimeWork);
	m_lock = std::move(rhs.m_lock);
	m_childS = rhs.m_childS;
	m_childL = rhs.m_childL;
	m_isBlack = rhs.m_isBlack;
	return *this;
}

//コピーオペレータ
inline profiler::profileInfo& profiler::profileInfo::operator=(const profiler::profileInfo& rhs)
{
	m_nameCrc = rhs.m_nameCrc;
	m_name = rhs.m_name;
	m_time = rhs.m_time;
	m_totalTime = rhs.m_totalTime;
	m_periodicTime = rhs.m_periodicTime;
	m_periodicTimeWork = rhs.m_periodicTimeWork;
	m_lock = rhs.m_lock;
	m_childS = rhs.m_childS;
	m_childL = rhs.m_childL;
	m_isBlack = rhs.m_isBlack;
	return *this;
}

//ムーブコンストラクタ
inline profiler::profileInfo::profileInfo(profiler::profileInfo&& obj) :
	m_nameCrc(obj.m_nameCrc),
	m_name(obj.m_name),
	m_time(std::move(obj.m_time)),
	m_totalTime(std::move(obj.m_totalTime)),
	m_periodicTime(std::move(obj.m_periodicTime)),
	m_periodicTimeWork(std::move(obj.m_periodicTimeWork)),
	m_lock(std::move(obj.m_lock)),
	m_childS(obj.m_childS),
	m_childL(obj.m_childL),
	m_isBlack(obj.m_isBlack)
{}

//コピーコンストラクタ
inline profiler::profileInfo::profileInfo(const profiler::profileInfo& obj) :
	m_nameCrc(obj.m_nameCrc),
	m_name(obj.m_name),
	m_time(obj.m_time),
	m_totalTime(obj.m_totalTime),
	m_periodicTime(obj.m_periodicTime),
	m_periodicTimeWork(obj.m_periodicTimeWork),
	m_lock(obj.m_lock),
	m_childS(obj.m_childS),
	m_childL(obj.m_childL),
	m_isBlack(obj.m_isBlack)
{}

//コンストラクタ
inline profiler::profileInfo::profileInfo(GASHA_ crc32_t name_crc, const char* name) :
	m_nameCrc(name_crc),
	m_name(name),
	m_time(),
	m_totalTime(),
	m_periodicTime(),
	m_periodicTimeWork(),
	m_childS(nullptr),
	m_childL(nullptr),
	m_isBlack(false)
{}

//デフォルトコンストラクタ
inline profiler::profileInfo::profileInfo()
{}

//デストラクタ
inline profiler::profileInfo::~profileInfo()
{}

//----------------------------------------
//スレッド情報

//プロファイル情報参照
inline const profiler::profileInfo* profiler::threadInfo::refProfile(const GASHA_ crc32_t name_crc) const
{
	return m_profileInfoTree.at(name_crc);
}

//プロファイル情報参照
inline profiler::profileInfo* profiler::threadInfo::refProfile(const GASHA_ crc32_t name_crc)
{
	return m_profileInfoTree.at(name_crc);
}

//プロファイル情報を取得
template<std::size_t N>
inline std::size_t profiler::threadInfo::getProfileInfo(profiler::profileInfo(&array)[N], const profiler::profileOrder_type order)
{
	return getProfileInfo(array, N, order);
}

//ムーブオペレータ
inline profiler::threadInfo& profiler::threadInfo::operator=(profiler::threadInfo&& rhs)
{
	m_threadNameCrc = rhs.m_threadNameCrc;
	m_threadName = rhs.m_threadName;
	m_profileInfoTree = std::move(rhs.m_profileInfoTree);
	m_next = rhs.m_next;
	return *this;
}

//コピーオペレータ
inline profiler::threadInfo& profiler::threadInfo::operator=(const profiler::threadInfo& rhs)
{
	m_threadNameCrc = rhs.m_threadNameCrc;
	m_threadName = rhs.m_threadName;
	m_profileInfoTree = rhs.m_profileInfoTree;
	m_next = rhs.m_next;
	return *this;
}

//ムーブコンストラクタ
inline profiler::threadInfo::threadInfo(profiler::threadInfo&& obj) :
	m_threadNameCrc(obj.m_threadNameCrc),
	m_threadName(obj.m_threadName),
	m_profileInfoTree(std::move(obj.m_profileInfoTree)),
	m_next(obj.m_next)
{}

//コピーコンストラクタ
inline profiler::threadInfo::threadInfo(const profiler::threadInfo& obj) :
	m_threadNameCrc(obj.m_threadNameCrc),
	m_threadName (obj.m_threadName),
	m_profileInfoTree(obj.m_profileInfoTree),
	m_next (obj.m_next)
{}

//コンストラクタ
inline profiler::threadInfo::threadInfo(const GASHA_ crc32_t name_crc, const char* name) :
	m_threadNameCrc(name_crc),
	m_threadName(name),
	m_profileInfoTree(),
	m_next(nullptr)
{}

//デフォルトコンストラクタ
inline profiler::threadInfo::threadInfo()
{}

//デストラクタ
inline profiler::threadInfo::~threadInfo()
{}

//----------------------------------------
//プロファイラクラス

//文字列プール参照
inline const profiler::strPoolInfo* profiler::refStrPool(const GASHA_ crc32_t name_crc)
{
	auto lock = m_strPoolTable.lockScoped();//共有ロック ※登録中の情報にアクセスする可能性がある
	const profiler::strPoolInfo* str_info = m_strPoolTable.at(name_crc);
	return str_info;
}

//スレッド情報参照
inline profiler::threadInfo* profiler::refThread(const GASHA_ crc32_t thread_name_crc)
{
	auto lock = m_threadInfoTable.lockScoped();//共有ロック ※登録中の情報にアクセスする可能性がある
	threadInfo* thread_info = m_threadInfoTable.at(thread_name_crc);
	return thread_info;
}

//スレッド情報を取得
template<std::size_t N>
inline std::size_t profiler::getThreadInfo(threadInfo(&array)[N])
{
	return getThreadInfo(array, N);
}

//プロファイル情報を取得
template<std::size_t N>
inline std::size_t profiler::getProfileInfo(const GASHA_ crc32_t thread_name_crc, profiler::profileInfo(&array)[N], const profiler::profileOrder_type order)
{
	return getProfileInfo(thread_name_crc, array, N, order);
}
template<std::size_t N>
inline std::size_t profiler::getProfileInfo(const char* thread_name, profiler::profileInfo(&array)[N], const profiler::profileOrder_type order)
{
	return getProfileInfo(GASHA_ calcCRC32(thread_name), array, N, order);
}
template<std::size_t N>
inline std::size_t profiler::getProfileInfo(GASHA_ threadId&& thread_id, profiler::profileInfo(&array)[N], const profiler::profileOrder_type order)
{
	return getProfileInfo(thread_id.nameCrc(), array, N, order);
}
template<std::size_t N>
inline std::size_t profiler::getProfileInfo(const GASHA_ threadId& thread_id, profiler::profileInfo(&array)[N], const profiler::profileOrder_type order)
{
	return getProfileInfo(thread_id.nameCrc(), array, N, order);
}
template<std::size_t N>
inline std::size_t profiler::getProfileInfo(const profiler::threadInfo& thread_info, profiler::profileInfo(&array)[N], const profiler::profileOrder_type order)
{
	return getProfileInfo(thread_info.threadNameCrc(), array, N, order);
}
inline std::size_t profiler::getProfileInfo(const char* thread_name, profiler::profileInfo* array, const std::size_t max_size, profiler::profileOrder_type order)
{
	return getProfileInfo(GASHA_ calcCRC32(thread_name), array, max_size, order);
}
inline std::size_t profiler::getProfileInfo(GASHA_ threadId&& thread_id, profiler::profileInfo* array, const std::size_t max_size, const profiler::profileOrder_type order)
{
	return getProfileInfo(thread_id.nameCrc(), array, max_size, order);
}
inline std::size_t profiler::getProfileInfo(const GASHA_ threadId& thread_id, profiler::profileInfo* array, const std::size_t max_size, const profiler::profileOrder_type order)
{
	return getProfileInfo(thread_id.nameCrc(), array, max_size, order);
}
inline std::size_t profiler::getProfileInfo(const profiler::threadInfo& thread_info, profiler::profileInfo* array, const std::size_t max_size, const profiler::profileOrder_type order)
{
	return getProfileInfo(thread_info.threadNameCrc(), array, max_size, order);
}

//【使用注意】クリア
inline void profiler::clear()
{
	initializeOnce();
}

//コンストラクタ
inline profiler::profiler()
{
#ifdef GASHA_PROFILER_SECURE_INITIALIZE
	std::call_once(m_initialized, initializeOnce);//グローバルログレベルマスク初期化（一回限り）
#endif//GASHA_PROFILER_SECURE_INITIALIZE
}

//デストラクタ
inline profiler::~profiler()
{}

#endif//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_PROFILER_INL

// End of file

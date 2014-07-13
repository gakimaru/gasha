#pragma once
#ifndef GASHA_INCLUDED_LOG_MESSAGE_H
#define GASHA_INCLUDED_LOG_MESSAGE_H

//--------------------------------------------------------------------------------
// log_message.h
// ログメッセージ【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログメッセージ
//--------------------------------------------------------------------------------

	//----------------------------------------
	//メッセージ固定バッファ
	class messageBuffer
	{
	public:
		//定数
		static const std::size_t BUFFER_SIZE = 2048;//1個あたりのバッファサイズ
		static const std::size_t BUFFER_NUM = 16;//バッファの個数
	public:
		//型
		typedef CPoolAllocatorWithBuff<BUFFER_SIZE, BUFFER_NUM> pool_t;//プールバッファ型
	public:
		//メッセージ用固定バッファ取得
		//※成功するまで（他のスレッドが解放するまで）リトライする
		void* assignBuffer()
		{
			void* p = nullptr;
			while (1)
			{
				p = m_pool.alloc(BUFFER_SIZE);
				if (p)
					break;
				std::this_thread::sleep_for(std::chrono::milliseconds(0));
			}
			return p;
		}
		//メッセージバッファ返却
		void releaseBuffer(void* p)
		{
			m_pool.free(p);
		}
	public:
		//コンストラクタ
		messageBuffer()
		{}
		//デストラクタ
		~messageBuffer()
		{}
	private:
		//フィールド
		static pool_t m_pool;//バッファのプール
	};
	//メッセージ固定バッファ用静的変数インスタンス化
	messageBuffer::pool_t messageBuffer::m_pool;//バッファのプール

	//----------------------------------------
	//表示属性
	//※コールポイントの集計に関する設定も扱う
	enum msgAttrEnum : unsigned char//属性
	{
		//表示属性
		withSeqNo = 0x01,//シーケンス番号表示（未実装）
		withLevel = 0x02,//レベル名表示
		withCategory = 0x04,//カテゴリ名表示
		withoutColor = 0x08,//カラーなし
		//コールポイント集計属性
		withoutProfile = 0x10,//プロファイラへの記録無効化（未実装）
		withoutThreadProfile = 0x20,//プロファイラでのスレッド別集計無効化（未実装）
	};
	//----------------------------------------
	//メッセージ操作
	enum msgCtrlEnum :unsigned char
	{
		//コールポイントスタック表示操作
		simplePrint = 0x00,//シンプル
		withFuncName = 0x01,//関数名を共に表示
		withFileName = 0x02,//ファイル名を共に表示
	};
	//----------------------------------------
	//メッセージクラス
	class message
	{
	public:
		//型
		typedef char buff_t;//メッセージバッファ用
		typedef unsigned char attr_t;//属性型
		typedef unsigned char control_t;//メッセージ操作型
	public:
		//定数
		static const attr_t DEFAULT_ATTR = withLevel | withCategory;//デフォルト表示属性
		static const level::value_t DEFAULT_LOG_LEVEL = asNormal;//デフォルトログレベル
		static const level::value_t DEFAULT_NOTOICE_LEVEL = asCritical;//デフォルト画面通知レベル
	public:
		//アクセッサ
		level::value_t getLevel() const { return m_level; }//レベル取得
		category::value_t getCategory() const { return m_category; }//カテゴリ取得
		const char* getName() const { return m_name; }//処理名取得
		const char* getSrcFileName() const { return getFileName(m_srcFileName); }//ソースファイル名取得
		const char* getFuncName() const { return m_funcName; }//関数名取得
		bool hasPushed() const { return m_hasPushed; }//コールポイントスタックにプッシュ済み
		bool attrHasChanged() const { return m_attrHasChanged; }//一時表示属性変更済み
		bool logLevelHasChanged() const { return m_logLevelHasChanged; }//一時ログレベル変更済み
		bool noticeLevelHasChanged() const { return m_noticeLevelHasChanged; }//一時画面通知レベル変更済み
	public:
		//メソッド
		//初期化（一回限り）
		static void initializeOnce();
		//適切なカテゴリに調整
		//※forCallPoint, forCriticalCallPointに基づく変換
		static category::value_t adjustProperCategory(const category::value_t category_)
		{
			if (category_ == forCallPoint)//直前のコールポイントのカテゴリ
			{
				const message* call_point = getLastCallPoint();
				if (call_point)
					return call_point->getCategory();
				return forAny;//コールポイントがなければ forAny 扱い
			}
			else if (category_ == forCriticalCallPoint)//直近のクリティカルコールポイントのカテゴリ
			{
				const message* call_point = getLastCriticalCallPoint();
				if (call_point)
					return call_point->getCategory();
				return forAny;//コールポイントがなければ forAny 扱い
			}
			return category_;
		}
		//-----「表示属性」系処理
		//現在の表示属性を取得
		static attr_t getAttrG(){ return m_attrG; }
		//【一時表示属性版】
		attr_t getAttr() const { return m_attr; }
		//【コールポイント版】
		static attr_t getAttrG_CP()
		{
			message* call_point = getLastCallPointWithAttrHasChanged();
			if (!call_point)
				return getAttrG();//コールポイント無かったら通常の表示属性
			return call_point->getAttr();
		}
		//【一時表示属性のコールポイント版】
		level::value_t getAttr_CP()
		{
			return m_attrHasChanged ? getAttr() : getAttrG_CP();
		}
		//現在の表示属性を変更
		//static void setAttrG(const attr_t attr){ m_attrG = attr; }//禁止
		static attr_t addAttrG(const attr_t attr){ m_attrG |= attr; return m_attrG; }
		static attr_t delAttrG(const attr_t attr){ m_attrG &= ~attr; return m_attrG; }
		//【一時表示属性版】
		//void setAttr(const attr_t attr){ copyAttr(); m_attr = attr; }//禁止
		attr_t addAttr(const attr_t attr){ copyAttr(); m_attr |= attr; return m_attr; }
		attr_t delAttr(const attr_t attr){ copyAttr(); m_attr &= ~attr; return m_attr; }
		//現在の表示属性をリセット
		//※デフォルトに戻す
		static void resetAttrG(){ m_attrG = DEFAULT_ATTR; }
		//【一時表示属性版】
		void resetAttr(){ copyAttr(); m_attr = DEFAULT_ATTR; }
		//現在の表示属性を一時表示属性にコピー
		void copyAttr()
		{
			if (m_attrHasChanged)
				return;
			message* call_point = getLastCallPointWithAttrHasChanged();
			m_attrHasChanged = true;
			if (m_hasPushed)
				++m_attrHasChangedG;
			if (call_point)
				m_attr = call_point->m_attr;//一時表示属性
			else
				m_attr = m_attrG;//一時表示属性
		}
		//一時表示属性のコピー状態を解除
		//※本来の表示属性に戻す
		void releaseAttr()
		{
			if (!m_attrHasChanged)
				return;
			m_attrHasChanged = false;
			if (m_hasPushed)
				--m_attrHasChangedG;
		}
		//-----「ログレベル」系処理
		//現在のログレベルを取得
		static level::value_t getLogLevelG(const category::value_t category_)
		{
			const category& o_category = category::get(category_);
			assert(o_category.forMask() == true);
			assert(o_category.forLog() == true || o_category.forNotice() == true);
			return m_logLevelG[category_];
		}
		//【コールポイント版】
		static level::value_t getLogLevelG_CP(const category::value_t category_)
		{
			message* call_point = getLastCallPointWithLogLevelHasChanged();
			if (!call_point)
				return getLogLevelG(category_);//コールポイント無かったら通常のログレベル
			return call_point->getLogLevel(category_);
		}
		//【一時ログレベルの有効判定】
		//※本来のレベルより低い一時ログレベルは扱えない
		bool isValidTempLogLevel(const category::value_t category_)
		{
			const category& o_category = category::get(category_);
			assert(o_category.forMask() == true);
			assert(o_category.forLog() == true || o_category.forNotice() == true);
			return m_logLevelHasChanged && level::calcValueAsOutput(m_logLevelG[category_]) <= level::calcValueAsOutput(m_logLevel[category_]);
		}
		//【一時ログレベル版】
		level::value_t getLogLevel(const category::value_t category_)
		{
			return isValidTempLogLevel(category_) ? m_logLevel[category_] : m_logLevelG[category_];
		}
		//【一時ログレベルのコールポイント版】
		level::value_t getLogLevel_CP(const category::value_t category_)
		{
			return isValidTempLogLevel(category_) ? m_logLevel[category_] : getLogLevelG_CP(category_);
		}
		//現在のログレベルを変更
		//※指定の値以上のレベルのメッセージのみをログ出力する
		//※変更前のログレベルを返す（forEvery指定時はforAnyのログレベルを返す）
		static level::value_t setLogLevelG(const level::value_t level_, const category::value_t category_)
		{
			const level& o_level = level::get(level_);
			assert(o_level.forMask() == true);
			const category& o_category = category::get(category_);
			assert(o_category.forMask() == true);
			if (category_ == forEvery)
			{
				const level::value_t prev = m_logLevelG[forAny];
				for (level::value_t& value : m_logLevelG)
					value = level_;
				return prev;
			}
			assert(o_category.forLog() == true);
			const level::value_t prev = m_logLevelG[category_];
			m_logLevelG[category_] = level_;
			return prev;
		}
		//【一時ログレベル版】
		level::value_t setLogLevel(const level::value_t level_, const category::value_t category_)
		{
			const level& o_level = level::get(level_);
			assert(o_level.forMask() == true);
			const category& o_category = category::get(category_);
			assert(o_category.forMask() == true);
			copyLogLevel();//更新前に現在のログレベルをコピー
			if (category_ == forEvery)
			{
				const level::value_t prev = m_logLevel[forAny];
				for (int category_tmp = category::NORMAL_MIN; category_tmp <= category::NORMAL_MAX; ++category_tmp)
				{
					m_logLevel[category_tmp] =
						level::calcValueAsOutput(m_logLevelG[category_tmp]) > level::calcValueAsOutput(level_) ?
							m_logLevelG[category_tmp] ://本来のレベルより低くすることはできない
							level_;
				}
				return prev;
			}
			assert(o_category.forLog() == true);
			const level::value_t prev = m_logLevel[category_];
			m_logLevel[category_] =
				level::calcValueAsOutput(m_logLevelG[category_]) > level::calcValueAsOutput(level_) ?
					m_logLevelG[category_] ://本来のレベルより低くすることはできない
					level_;
			return prev;
		}
		//現在のログレベルをリセット
		//※デフォルトに戻す
		static void resetLogLevelG(const category::value_t category_)
		{
			setLogLevelG(DEFAULT_LOG_LEVEL, category_);
		}
		//【一時ログレベル版】
		void resetLogLevel(const category::value_t category_)
		{
			setLogLevel(DEFAULT_LOG_LEVEL, category_);
		}
		//現在のログレベルを全てリセット
		//※デフォルトに戻す
		static void resetLogLevelAllG()
		{
			//resetLogLevelG(forEvery);//初期化処理でも呼び出されるので、共通処理を使わず単純に更新
			for (level::value_t& value : m_logLevelG)
				value = DEFAULT_LOG_LEVEL;
		}
		//【一時ログレベル版】
		void resetLogLevelAll()
		{
			resetLogLevel(forEvery);
		}
		//ログレベルを一時ログレベルにコピー
		void copyLogLevel()
		{
			if (m_logLevelHasChanged)
				return;
			message* call_point = getLastCallPointWithLogLevelHasChanged();
			m_logLevelHasChanged = true;
			if (m_hasPushed)
				++m_logLevelHasChangedG;
			if (call_point)
				std::memcpy(m_logLevel, call_point->m_logLevel, sizeof(m_logLevel));//一時ログレベル
			else
				std::memcpy(m_logLevel, m_logLevelG, sizeof(m_logLevel));//一時ログレベル
		}
		//一時ログレベルのコピー状態を解除
		//※本来のログレベルに戻す
		void releaseLogLevel()
		{
			if (!m_logLevelHasChanged)
				return;
			m_logLevelHasChanged = false;
			if (m_hasPushed)
				--m_logLevelHasChangedG;
		}
		//-----「画面通知レベル」系処理
		//現在の画面通知レベルを取得
		static level::value_t getNoticeLevelG(const category::value_t category_)
		{
			const category& o_category = category::get(category_);
			assert(o_category.forMask() == true);
			assert(o_category.forLog() == true || o_category.forNotice() == true);
			return m_noticeLevelG[category_];
		}
		//【コールポイント版】
		static level::value_t getNoticeLevelG_CP(const category::value_t category_)
		{
			message* call_point = getLastCallPointWithNoticeLevelHasChanged();
			if (!call_point)
				return getNoticeLevelG(category_);//コールポイント無かったら通常の画面通知レベル
			return call_point->getNoticeLevel(category_);
		}
		//【一時画面通知レベルの有効判定】
		//※本来のレベルより低い一時画面通知レベルは扱えない
		bool isValidTempNoticeLevel(const category::value_t category_)
		{
			const category& o_category = category::get(category_);
			assert(o_category.forMask() == true);
			assert(o_category.forLog() == true || o_category.forNotice() == true);
			return m_noticeLevelHasChanged && level::calcValueAsOutput(m_noticeLevelG[category_]) <= level::calcValueAsOutput(m_noticeLevel[category_]);
		}
		//【一時画面通知レベル版】
		level::value_t getNoticeLevel(const category::value_t category_)
		{
			return isValidTempNoticeLevel(category_) ? m_noticeLevel[category_] : m_noticeLevelG[category_];
		}
		//【一時画面通知レベルのコールポイント版】
		level::value_t getNoticeLevel_CP(const category::value_t category_)
		{
			return isValidTempNoticeLevel(category_) ? m_noticeLevel[category_] : getNoticeLevelG_CP(category_);
		}
		//現在の画面通知レベルを変更
		//※指定の値以上のレベルのメッセージのみをログ出力する
		//※変更前の画面通知レベルを返す（forEvery指定時はforAnyの画面通知レベルを返す）
		static level::value_t setNoticeLevelG(const level::value_t level_, const category::value_t category_)
		{
			const level& o_level = level::get(level_);
			assert(o_level.forMask() == true);
			const category& o_category = category::get(category_);
			assert(o_category.forMask() == true);
			if (category_ == forEvery)
			{
				const level::value_t prev = m_noticeLevelG[forAny];
				for (level::value_t& value : m_noticeLevelG)
					value = level_;
				return prev;
			}
			assert(o_category.forNotice() == true);
			const level::value_t prev = m_noticeLevelG[category_];
			m_noticeLevelG[category_] = level_;
			return prev;
		}
		//【一時画面通知レベル版】
		level::value_t setNoticeLevel(const level::value_t level_, const category::value_t category_)
		{
			const level& o_level = level::get(level_);
			assert(o_level.forMask() == true);
			const category& o_category = category::get(category_);
			assert(o_category.forMask() == true);
			copyNoticeLevel();//更新前に現在の画面通知レベルをコピー
			if (category_ == forEvery)
			{
				const level::value_t prev = m_noticeLevel[forAny];
				for (int category_tmp = category::NORMAL_MIN; category_tmp <= category::NORMAL_MAX; ++category_tmp)
				{
					m_noticeLevel[category_tmp] =
						level::calcValueAsOutput(m_noticeLevelG[category_tmp]) > level::calcValueAsOutput(level_) ?
						m_noticeLevelG[category_tmp] ://本来のレベルより低くすることはできない
						level_;
				}
				return prev;
			}
			assert(o_category.forNotice() == true);
			const level::value_t prev = m_noticeLevel[category_];
			m_noticeLevel[category_] =
				level::calcValueAsOutput(m_noticeLevelG[category_]) > level::calcValueAsOutput(level_) ?
				m_noticeLevelG[category_] ://本来のレベルより低くすることはできない
				level_;
			return prev;
		}
		//現在の画面通知レベルをリセット
		//※デフォルトに戻す
		static void resetNoticeLevelG(const category::value_t category_)
		{
			setNoticeLevelG(DEFAULT_NOTOICE_LEVEL, category_);
		}
		//【一時画面通知レベル版】
		void resetNoticeLevel(const category::value_t category_)
		{
			setNoticeLevel(DEFAULT_NOTOICE_LEVEL, category_);
		}
		//現在の画面通知レベルを全てリセット
		//※デフォルトに戻す
		static void resetNoticeLevelAllG()
		{
			//resetNoticeLevelG(forEvery);//初期化処理でも呼び出されるので、共通処理を使わず単純に更新
			for (level::value_t& value : m_noticeLevelG)
				value = DEFAULT_NOTOICE_LEVEL;
		}
		//【一時画面通知レベル版】
		void resetNoticeLevelAll()
		{
			resetNoticeLevel(forEvery);
		}
		//画面通知レベルを一時画面通知レベルにコピー
		void copyNoticeLevel()
		{
			if (m_noticeLevelHasChanged)
				return;
			message* call_point = getLastCallPointWithLogLevelHasChanged();
			m_noticeLevelHasChanged = true;
			if (m_hasPushed)
				++m_noticeLevelHasChangedG;
			if (call_point)
				std::memcpy(m_noticeLevel, call_point->m_noticeLevel, sizeof(m_noticeLevel));//一時ログレベル
			else
				std::memcpy(m_noticeLevel, m_noticeLevelG, sizeof(m_noticeLevel));//一時ログレベル
		}
		//一時画面通知レベルのコピー状態を解除
		//※本来の画面通知レベルに戻す
		void releaseNoticeLevel()
		{
			if (!m_noticeLevelHasChanged)
				return;
			m_noticeLevelHasChanged = false;
			if (m_hasPushed)
				--m_noticeLevelHasChangedG;
		}
	protected:
		//-----「コールポイント操作」系処理（共通内部処理）
		//コールポイントスタックに自身をプッシュ
		void pushCallPoint()
		{
			if (m_hasPushed)
				return;
			m_hasPushed = true;
			assert(m_callPointStackHead != this);
			m_callPointStackNext = m_callPointStackHead;
			m_callPointStackHead = this;
		}
		//コールポイントスタックから（自身を）ポップ
		void popCallPoint()
		{
			if (!m_hasPushed)
				return;
			m_hasPushed = false;
			assert(m_callPointStackHead == this);
			m_callPointStackHead = m_callPointStackNext;
			m_callPointStackNext = nullptr;
		}
	public:
		//-----「コールポイント操作」系処理
		//直前のコールポイントを取得
		static message* getLastCallPoint()
		{
			return m_callPointStackHead;
		}
		//直近のクリティカルコールポイントを取得
		static message* getLastCriticalCallPoint()
		{
			message* node = m_callPointStackHead;
			while (node)
			{
				if (node->getLevel() == asCritical)
					break;
				node = node->m_callPointStackNext;
			}
			return node;
		}
		//直近で表示属性を更新したコールポイントを取得
		static message* getLastCallPointWithAttrHasChanged()
		{
			if (m_attrHasChangedG == 0)
				return nullptr;
			message* node = m_callPointStackHead;
			while (node)
			{
				if (node->attrHasChanged())
					break;
				node = node->m_callPointStackNext;
			}
			return node;
		}
		//直近でログレベルを更新したコールポイントを取得
		static message* getLastCallPointWithLogLevelHasChanged()
		{
			if (m_logLevelHasChangedG == 0)
				return nullptr;
			message* node = m_callPointStackHead;
			while (node)
			{
				if (node->logLevelHasChanged())
					break;
				node = node->m_callPointStackNext;
			}
			return node;
		}
		//直近で画面通知レベルを更新したコールポイントを取得
		static message* getLastCallPointWithNoticeLevelHasChanged()
		{
			if (m_noticeLevelHasChangedG == 0)
				return nullptr;
			message* node = m_callPointStackHead;
			while (node)
			{
				if (node->noticeLevelHasChanged())
					break;
				node = node->m_callPointStackNext;
			}
			return node;
		}
	protected:
		//-----「メッセージバッファ操作」系処理（共通内部処理）
		//メッセージバッファ確保
		static void beginBuffer()
		{
			if (m_messageBuff)
				freeBuffer();
			messageBuffer allocator;//メッセージ固定バッファ
			m_messageBuff = reinterpret_cast<buff_t*>(allocator.assignBuffer());
			m_messageBuffSize = messageBuffer::BUFFER_SIZE;
			m_messageBuffUsed = 0;
			if (!m_messageBuff)
				m_messageBuffSize = 0;
		}
		//メッセージバッファ解放
		static void freeBuffer()
		{
			if (!m_messageBuff)
				return;
			messageBuffer allocator;//メッセージ固定バッファ
			allocator.releaseBuffer(m_messageBuff);
			m_messageBuff = nullptr;//メッセージバッファ
			m_messageBuffSize = 0;//メッセージバッファサイズ
			m_messageBuffUsed = 0;//メッセージバッファ使用量
		}
		//メッセージバッファフラッシュ
		static void flushBuffer(const level& o_level, const bool flush_log, const bool flush_notice, const bool log_without_color = false)
		{
			if (!m_messageBuff)
				return;
			//【仮処理】本来はメッセージをキューイングしてロギング処理／画面通知処理に渡す
			if (flush_log)
			{
				if (log_without_color)
				{
					fprintf(stdout, m_messageBuff);
				}
				else
				{
					color col(o_level.changeColor());
					fprintf(stdout, m_messageBuff);
				}
			}
			if (flush_notice)
			{
				color col(o_level.changeColorForNotice());
				fprintf(stderr, m_messageBuff);
			}
			freeBuffer();//バッファ解放
		}
		//バッファリング
		static int vbuffer(const char* fmt, va_list args)
		{
			std::size_t remain = m_messageBuffSize - m_messageBuffUsed;
			if (!m_messageBuff || remain == 0)
				return 0;
		#ifdef USE_STRCPY_S
			const int ret = vsprintf_s(m_messageBuff + m_messageBuffUsed, remain, fmt, args);
		#else//USE_STRCPY_S
			const int ret = vsprintf(m_messageBuff + m_messageBuffUsed, fmt, args);
		#endif//USE_STRCPY_S
			m_messageBuffUsed += ret;
			return ret;
		}
		static int buffer(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			const int ret = vbuffer(fmt, args);
			va_end(args);
			return ret;
		}
		//出力判定
		static void isAllowPrint(bool& is_allow_log, bool& is_allow_notice, const level::value_t log_level, const level::value_t notice_level, const level::value_t level_, const category::value_t category_)
		{
			const level& o_level = level::get(level_);
			assert(o_level.forLog() == true || o_level.forNotice() == true);
			const category& o_category = category::get(category_);
			assert(o_category.forLog() == true || o_category.forNotice() == true);
			const level& o_log_level = level::get(log_level);
			const level& o_notice_level = level::get(notice_level);
			is_allow_log = (o_level >= o_log_level && o_level.forLog() && o_category.forLog());
			is_allow_notice = (o_level >= o_notice_level && o_level.forNotice() && o_category.forNotice());
		}
		//-----「メッセージ出力」系処理（共通内部処理）
		//メッセージ出力：レベルを引数指定
		//※va_listを引数にとるバージョン
		static int vprintCommon(const attr_t attr, const level::value_t log_level, const level::value_t notice_level, const level::value_t level_, const category::value_t category_, const char* fmt, va_list args)
		{
			bool is_allow_log;
			bool is_allow_notice;
			isAllowPrint(is_allow_log, is_allow_notice, log_level, notice_level, level_, category_);
			if (!is_allow_log && !is_allow_notice)
				return 0;
			const level& o_level = level::get(level_);
			const category& o_category = category::get(category_);
			beginBuffer();
			const bool without_color = (attr & withoutColor) ? true : false;
			const attr_t attr_tmp = attr & (withLevel | withCategory);
			if (is_allow_log && attr_tmp != 0)
			{
				if (attr_tmp == withLevel)
					buffer("[%s]", o_level.name());
				else if (attr_tmp == withCategory)
					buffer("[%s]", o_category.name());
				else//if (attr_tmp == (withLevel | withCategory))
					buffer("[%s,%s]", o_level.name(), o_category.name());
				flushBuffer(o_level, true, false, without_color);
				beginBuffer();
				buffer(" ");
			}
			int ret = vbuffer(fmt, args);
			flushBuffer(o_level, is_allow_log, is_allow_notice, true);
			return ret;
		}
		//-----「コールポイントスタック出力」系処理（共通内部処理）
		//コールポイントスタックをリスト表示
		static void printCPStackCommon(const attr_t attr, const level::value_t log_level, const level::value_t level_, const category::value_t category_, const char* name, const control_t control)
		{
			bool is_allow_log;
			bool is_allow_notice;
			isAllowPrint(is_allow_log, is_allow_notice, log_level, asSilentAbsolutely, level_, category_);
			if (!is_allow_log)
				return;
			const level& o_level = level::get(level_);
			const category& o_category = category::get(category_);
			const bool without_color = (attr & withoutColor) ? true : false;
			const attr_t attr_tmp = attr & (withLevel | withCategory);
			beginBuffer();
			buffer("============================================================\n");
			if (attr_tmp != 0)
			{
				if (attr_tmp == withLevel)
					buffer("[%s]", o_level.name());
				else if (attr_tmp == withCategory)
					buffer("[%s]", o_category.name());
				else//if (attr_tmp == (withLevel | withCategory))
					buffer("[%s,%s]", o_level.name(), o_category.name());
			}
			if (!without_color)
			{
				flushBuffer(o_level, true, false);
				beginBuffer();
			}
			buffer(" Call point stack at \"%s\"\n", name);
			buffer("----------------------------------------\n");
			const message* call_point = m_callPointStackHead;
			while (call_point)
			{
				buffer("  \"%s\" ", call_point->getName());
				if (attr_tmp != 0)
				{
					const level& o_level_tmp = level::get(call_point->getLevel());
					const category& o_category_tmp = category::get(call_point->getCategory());
					if (!without_color)
					{
						flushBuffer(o_level, true, false, true);
						beginBuffer();
					}
					if (attr_tmp == withLevel)
						buffer("[%s]", o_level_tmp.name());
					else if (attr_tmp == withCategory)
						buffer("[%s]", o_category_tmp.name());
					else//if (attr_tmp == (withLevel | withCategory))
						buffer("[%s,%s]", o_level_tmp.name(), o_category_tmp.name());
					if (!without_color)
					{
						flushBuffer(o_level_tmp, true, false, false);
						beginBuffer();
					}
				}
				const control_t control_tmp = control & (withFuncName | withFileName);
				if (control_tmp != 0)
				{
					buffer(" ... ");
					if (control_tmp & withFuncName)
						buffer("%s", call_point->getFuncName());
					if (control_tmp == (withFuncName | withFileName))
						buffer(" : ");
					if (control_tmp & withFileName)
						buffer(" %s", call_point->getSrcFileName());
				}
				buffer("\n");
				call_point = call_point->m_callPointStackNext;
			}
			if (!without_color)
			{
				flushBuffer(o_level, true, false, true);
				beginBuffer();
			}
			buffer("============================================================\n");
			flushBuffer(o_level, true, false, without_color);
		}
	public:
		//-----「メッセージ出力」系処理
		//メッセージ出力／ログ出力／画面通知メソッド：レベルを引数指定
		//※vprint***/vlog***/vnotice***
		//※va_listを引数にとるバージョン
		static int vprintG(const level::value_t level_, const category::value_t category_, const char* fmt, va_list args)
		{
			const category::value_t category_adj = adjustProperCategory(category_);
			const attr_t attr_adj = getAttrG_CP();
			const level::value_t log_level = getLogLevelG_CP(category_adj);
			const level::value_t notice_level = getNoticeLevelG_CP(category_adj);
			return vprintCommon(attr_adj, log_level, notice_level, level_, category_adj, fmt, args);
		}
		int vprint(const level::value_t level_, const category::value_t category_, const char* fmt, va_list args)//【一時ログ／画面通知レベル版】
		{
			const category::value_t category_adj = adjustProperCategory(category_);
			const attr_t attr_adj = getAttr_CP();
			const level::value_t log_level = getLogLevel_CP(category_adj);
			const level::value_t notice_level = getNoticeLevel_CP(category_adj);
			return vprintCommon(attr_adj, log_level, notice_level, level_, category_adj, fmt, args);
		}
		int vprint(const level::value_t level_, const char* fmt, va_list args)//【一時ログ／画面通知レベル版】
		{
			return vprint(level_, m_category, fmt, args);
		}
		int vprint(const char* fmt, va_list args)//【一時ログ／画面通知レベル版】
		{
			return vprint(m_level, m_category, fmt, args);
		}
		static int vlogG(const level::value_t level_, const category::value_t category_, const char* fmt, va_list args)
		{
			const category::value_t category_adj = adjustProperCategory(category_);
			const attr_t attr_adj = getAttrG_CP();
			const level::value_t log_level = getLogLevelG_CP(category_adj);
			const level::value_t notice_level = asSilentAbsolutely;
			return vprintCommon(attr_adj, log_level, notice_level, level_, category_adj, fmt, args);
		}
		int vlog(const level::value_t level_, const category::value_t category_, const char* fmt, va_list args)//【一時ログレベル版】
		{
			const category::value_t category_adj = adjustProperCategory(category_);
			const attr_t attr_adj = getAttr_CP();
			const level::value_t log_level = getLogLevel_CP(category_adj);
			const level::value_t notice_level = asSilentAbsolutely;
			return vprintCommon(attr_adj, log_level, notice_level, level_, category_adj, fmt, args);
		}
		int vlog(const level::value_t level_, const char* fmt, va_list args)//【一時ログレベル版】
		{
			return vlog (level_, m_category, fmt, args);
		}
		int vlog(const char* fmt, va_list args)//【一時ログレベル版】
		{
			return vlog(m_level, m_category, fmt, args);
		}
		static int vnoticeG(const level::value_t level_, const category::value_t category_, const char* fmt, va_list args)
		{
			const category::value_t category_adj = adjustProperCategory(category_);
			const attr_t attr_adj = getAttrG_CP();
			const level::value_t log_level = asSilentAbsolutely;
			const level::value_t notice_level = getNoticeLevelG_CP(category_adj);
			return vprintCommon(attr_adj, log_level, notice_level, level_, category_adj, fmt, args);
		}
		int vnotice(const level::value_t level_, const category::value_t category_, const char* fmt, va_list args)//【一時画面通知レベル版】
		{
			const category::value_t category_adj = adjustProperCategory(category_);
			const attr_t attr_adj = getAttr_CP();
			const level::value_t log_level = asSilentAbsolutely;
			const level::value_t notice_level = getNoticeLevel_CP(category_adj);
			return vprintCommon(attr_adj, log_level, notice_level, level_, category_adj, fmt, args);
		}
		int vnotice(const level::value_t level_, const char* fmt, va_list args)//【一時画面通知レベル版】
		{
			return vnotice(level_, m_category, fmt, args);
		}
		int vnotice(const char* fmt, va_list args)//【一時画面通知レベル版】
		{
			return vnotice(m_level, m_category, fmt, args);
		}
		//メッセージ出力／ログ出力／画面通知メソッド：レベルを引数指定
		//※print***/log***/notice***
		//※可変長引数バージョン
		static int printG(const level::value_t level_, const category::value_t category_, const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			const int ret = vprintG(level_, category_, fmt, args);
			va_end(args);
			return ret;
		}
		int print(const level::value_t level_, const category::value_t category_, const char* fmt, ...)//【一時ログ／画面通知レベル版】
		{
			va_list args;
			va_start(args, fmt);
			const int ret = vprint(level_, category_, fmt, args);
			va_end(args);
			return ret;
		}
		int print(const level::value_t level_, const char* fmt, ...)//【一時ログ／画面通知レベル版】
		{
			va_list args;
			va_start(args, fmt);
			const int ret = vprint(level_, fmt, args);
			va_end(args);
			return ret;
		}
		int print(const char* fmt, ...)//【一時ログ／画面通知レベル版】
		{
			va_list args;
			va_start(args, fmt);
			const int ret = vprint(fmt, args);
			va_end(args);
			return ret;
		}
		static int logG(const level::value_t level_, const category::value_t category_, const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			const int ret = vlogG(level_, category_, fmt, args);
			va_end(args);
			return ret;
		}
		int log(const level::value_t level_, const category::value_t category_, const char* fmt, ...)//【一時ログレベル版】
		{
			va_list args;
			va_start(args, fmt);
			const int ret = vlog(level_, category_, fmt, args);
			va_end(args);
			return ret;
		}
		int log(const level::value_t level_, const char* fmt, ...)//【一時ログレベル版】
		{
			va_list args;
			va_start(args, fmt);
			const int ret = vlog(level_, fmt, args);
			va_end(args);
			return ret;
		}
		int log(const char* fmt, ...)//【一時ログレベル版】
		{
			va_list args;
			va_start(args, fmt);
			const int ret = vlog(fmt, args);
			va_end(args);
			return ret;
		}
		static int noticeG(const level::value_t level_, const category::value_t category_, const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			const int ret = vnoticeG(level_, category_, fmt, args);
			va_end(args);
			return ret;
		}
		int notice(const level::value_t level_, const category::value_t category_, const char* fmt, ...)//【一時画面通知レベル版】
		{
			va_list args;
			va_start(args, fmt);
			const int ret = vnotice(level_, category_, fmt, args);
			va_end(args);
			return ret;
		}
		int notice(const level::value_t level_, const char* fmt, ...)//【一時画面通知レベル版】
		{
			va_list args;
			va_start(args, fmt);
			const int ret = vnotice(level_, fmt, args);
			va_end(args);
			return ret;
		}
		int notice(const char* fmt, ...)//【一時画面通知レベル版】
		{
			va_list args;
			va_start(args, fmt);
			const int ret = vnotice(fmt, args);
			va_end(args);
			return ret;
		}
		//メッセージ出力／ログ出力／画面通知メソッド定義マクロ
		//※vprint***/vlog***/vnotice***
		//※va_listを引数にとるバージョン
	#define declare_vprintMethods(level_) \
		static int vprintAs##level_##G(const category::value_t category_, const char* fmt, va_list args) \
		{ \
			return vprintG(as##level_, category_, fmt, args); \
		} \
		int vprintAs##level_(const category::value_t category_, const char* fmt, va_list args)/*【一時ログ／画面通知レベル版】*/ \
		{ \
			return vprint(as##level_, category_, fmt, args); \
		} \
		int vprintAs##level_(const char* fmt, va_list args)/*【一時ログ／画面通知レベル版】*/ \
		{ \
			return vprint(as##level_, fmt, args); \
		} \
		static int vlogAs##level_##G(const category::value_t category_, const char* fmt, va_list args) \
		{ \
			return vlogG(as##level_, category_, fmt, args); \
		} \
		int vlogAs##level_(const category::value_t category_, const char* fmt, va_list args)/*【一時ログレベル版】*/ \
		{ \
			return vlog(as##level_, category_, fmt, args); \
		} \
		int vlogAs##level_(const char* fmt, va_list args)/*【一時ログレベル版】*/ \
		{ \
			return vlog(as##level_, fmt, args); \
		} \
		static int vnoticeAs##level_##G(const category::value_t category_, const char* fmt, va_list args) \
		{ \
			return vnoticeG(as##level_, category_, fmt, args); \
		} \
		int vnoticeAs##level_(const category::value_t category_, const char* fmt, va_list args)/*【一時画面通知レベル版】*/ \
		{ \
			return vnotice(as##level_, category_, fmt, args); \
		} \
		int vnoticeAs##level_(const char* fmt, va_list args)/*【一時画面通知レベル版】*/ \
		{ \
			return vnotice(as##level_, fmt, args); \
		}
		//メッセージ出力／ログ出力／画面通知メソッド定義マクロ
		//※print***/log***/notice***
		//※可変長引数バージョン
	#define declare_printMethods(level_) \
		static int printAs##level_##G(const category::value_t category_, const char* fmt, ...) \
		{ \
			va_list args; \
			va_start(args, fmt); \
			const int ret = vprintAs##level_##G(category_, fmt, args); \
			va_end(args); \
			return ret; \
		} \
		int printAs##level_(const category::value_t category_, const char* fmt, ...)/*【一時ログ／画面通知レベル版】*/ \
		{ \
			va_list args; \
			va_start(args, fmt); \
			const int ret = vprintAs##level_(category_, fmt, args); \
			va_end(args); \
			return ret; \
		} \
		int printAs##level_(const char* fmt, ...)/*【一時ログ／画面通知レベル版】*/ \
		{ \
			va_list args; \
			va_start(args, fmt); \
			const int ret = vprintAs##level_(fmt, args); \
			va_end(args); \
			return ret; \
		} \
		static int logAs##level_##G(const category::value_t category_, const char* fmt, ...) \
		{ \
			va_list args; \
			va_start(args, fmt); \
			const int ret = vlogAs##level_##G(category_, fmt, args); \
			va_end(args); \
			return ret; \
		} \
		int logAs##level_(const category::value_t category_, const char* fmt, ...)/*【一時ログレベル版】*/ \
		{ \
			va_list args; \
			va_start(args, fmt); \
			const int ret = vlogAs##level_(category_, fmt, args); \
			va_end(args); \
			return ret; \
		} \
		int logAs##level_(const char* fmt, ...)/*【一時ログレベル版】*/ \
		{ \
			va_list args; \
			va_start(args, fmt); \
			const int ret = vlogAs##level_(fmt, args); \
			va_end(args); \
			return ret; \
		} \
		static int noticeAs##level_##G(const category::value_t category_, const char* fmt, ...) \
		{ \
			va_list args; \
			va_start(args, fmt); \
			const int ret = vnoticeAs##level_##G(category_, fmt, args); \
			va_end(args); \
			return ret; \
		} \
		int noticeAs##level_(const category::value_t category_, const char* fmt, ...)/*【一時画面通知レベル版】*/ \
		{ \
			va_list args; \
			va_start(args, fmt); \
			const int ret = vnoticeAs##level_(category_, fmt, args); \
			va_end(args); \
			return ret; \
		} \
		int noticeAs##level_(const char* fmt, ...)/*【一時画面通知レベル版】*/ \
		{ \
			va_list args; \
			va_start(args, fmt); \
			const int ret = vnoticeAs##level_(fmt, args); \
			va_end(args); \
			return ret; \
		}
		//メッセージ出力／ログ出力／画面通知メソッド定義
		//※vprint***/vlog***/vnotice***
		//※va_listを引数にとるバージョン
		declare_vprintMethods(Normal);//通常メッセージ
		declare_vprintMethods(Verbose);//冗長メッセージ
		declare_vprintMethods(Detail);//詳細メッセージ
		declare_vprintMethods(Important);//重要メッセージ
		declare_vprintMethods(Warning);//警告メッセージ
		declare_vprintMethods(Critical);//重大（問題）メッセージ
		declare_vprintMethods(Absolute);//絶対（必須）メッセージ
		//メッセージ出力／ログ出力／画面通知メソッド定義
		//※print***/log***/notice***
		//※可変長引数バージョン
		declare_printMethods(Normal);//通常メッセージ
		declare_printMethods(Verbose);//冗長メッセージ
		declare_printMethods(Detail);//詳細メッセージ
		declare_printMethods(Important);//重要メッセージ
		declare_printMethods(Warning);//警告メッセージ
		declare_printMethods(Critical);//重大（問題）メッセージ
		declare_printMethods(Absolute);//絶対（必須）メッセージ
		//-----「コールポイントスタック出力」系処理
		//コールポイントスタックをリスト表示
		static void printCPStackG(const level::value_t level_, category::value_t category_, const char* name, const control_t control = simplePrint)
		{
			const category::value_t category_adj = adjustProperCategory(category_);
			const attr_t attr_adj = getAttrG_CP();
			const level::value_t log_level = getLogLevelG_CP(category_adj);
			printCPStackCommon(attr_adj, log_level, level_, category_adj, name, control);
		}
		void printCPStack(const level::value_t level_, category::value_t category_, const char* name, const control_t control = simplePrint)
		{
			const category::value_t category_adj = adjustProperCategory(category_);
			const attr_t attr_adj = getAttr_CP();
			const level::value_t log_level = getLogLevel_CP(category_adj);
			printCPStackCommon(attr_adj, log_level, level_, category_adj, name, control);
		}
	#define declare_printCPMethods(level_) \
		static void printCPStackAs##level_##G(const category::value_t category_, const char* name, const control_t control = simplePrint) \
		{ \
			printCPStackG(as##level_, category_, name, control); \
		} \
		void printCPStackAs##level_(const category::value_t category_, const char* name, const control_t control = simplePrint)/*【一時ログ／画面通知レベル版】*/ \
		{ \
			printCPStack(as##level_, category_, name, control); \
		} \
		void printCPStackAs##level_(const char* name, const control_t control = simplePrint)/*【一時ログ／画面通知レベル版】*/ \
		{ \
			printCPStack(as##level_, m_category, name, control); \
		}
		declare_printCPMethods(Normal);//通常メッセージ
		declare_printCPMethods(Verbose);//冗長メッセージ
		declare_printCPMethods(Detail);//詳細メッセージ
		declare_printCPMethods(Important);//重要メッセージ
		declare_printCPMethods(Warning);//警告メッセージ
		declare_printCPMethods(Critical);//重大（問題）メッセージ
		declare_printCPMethods(Absolute);//絶対（必須）メッセージ
	public:
		//デフォルトコンストラクタ
		message():
			m_level(asNormal),//レベル
			m_category(forAny),//カテゴリ
			m_name(nullptr),//名前
			m_srcFileName(nullptr),//ソースファイル名
			m_funcName(nullptr),//関数名
			m_attr(0),//一時表示属性
			m_hasPushed(false),//コールポイントスタックにプッシュ済み
			m_attrHasChanged(false),//一時表示属性変更済み
			m_logLevelHasChanged(false),//一時ログレベル変更済み
			m_noticeLevelHasChanged(false),//一時画面通知レベル変更済み
			m_callPointStackNext(nullptr)//次のコールポイントスタック
		{
			initializeOnce();
		}
		//コンストラクタ
		message(const level::value_t level_, const category::value_t category_, const char* name = nullptr, const char* src_file_name = nullptr, const char* func_name = nullptr) :
			m_level(level_),//レベル
			m_category(category_),//カテゴリ
			m_name(name),//名前
			m_srcFileName(src_file_name),//ソースファイル名
			m_funcName(func_name),//関数名
			m_attr(0),//一時表示属性
			m_hasPushed(false),//コールポイントスタックにプッシュ済み
			m_attrHasChanged(false),//一時表示属性変更済み
			m_logLevelHasChanged(false),//一時ログレベル変更済み
			m_noticeLevelHasChanged(false),//一時画面通知レベル変更済み
			m_callPointStackNext(nullptr)//次のコールポイントスタック
		{
			assert(category_ >= category::NORMAL_MIN && category_ <= category::NORMAL_MAX);
		}
		//デストラクタ
		~message()
		{
			releaseAttr();//一時表示属性を解放
			releaseLogLevel();//一時ログレベルを解放
			releaseNoticeLevel();//一時画面通知レベルを解放
			popCallPoint();//コールスタックからポップ
		}
	protected:
		//フィールド
		const level::value_t m_level;//レベル
		const category::value_t m_category;//カテゴリ
		const char* m_name;//処理名
		const char* m_srcFileName;//ソースファイル名
		const char* m_funcName;//関数名
		attr_t m_attr;//表示属性
		bool m_hasPushed;//コールポイントスタックにプッシュ済み
		bool m_attrHasChanged;//一時表示属性変更済み
		bool m_logLevelHasChanged;//一時ログレベル変更済み
		bool m_noticeLevelHasChanged;//一時画面通知レベル変更済み
		message* m_callPointStackNext;//次のコールポイントスタック
		level::value_t m_logLevel[category::NORMAL_NUM];//一時ログレベル
		level::value_t m_noticeLevel[category::NORMAL_NUM];//一時画面通知レベル
	private:
		static bool m_isInitialized;//初期化済みフラグ
		static attr_t m_attrG;//表示属性
		static level::value_t m_logLevelG[category::NORMAL_NUM];//現在のログレベル
		static level::value_t m_noticeLevelG[category::NORMAL_NUM];//現在の画面通知レベル
		static thread_local int m_attrHasChangedG;//一時表示属性変更スタック数
		static thread_local int m_logLevelHasChangedG;//一時ログレベル変更スタック数
		static thread_local int m_noticeLevelHasChangedG;//一時画面通知レベル変更スタック数
		static thread_local message* m_callPointStackHead;//コールポイントスタックの先頭
		static thread_local buff_t* m_messageBuff;//メッセージバッファ
		static thread_local std::size_t m_messageBuffSize;//メッセージバッファサイズ
		static thread_local std::size_t m_messageBuffUsed;//メッセージバッファ使用量
	};
	//----------------------------------------
	//メッセージの静的変数をインスタンス化
	bool message::m_isInitialized = false;//初期化済みフラグ
	message::attr_t message::m_attrG = 0;//表示属性
	level::value_t message::m_logLevelG[category::NORMAL_NUM];//現在のログレベル
	level::value_t message::m_noticeLevelG[category::NORMAL_NUM];//現在の画面通知レベル
	thread_local int message::m_attrHasChangedG = 0;//一時表示属性変更スタック数
	thread_local int message::m_logLevelHasChangedG = 0;//一時ログレベル変更スタック数
	thread_local int message::m_noticeLevelHasChangedG = 0;//一時画面通知レベル変更スタック数
	thread_local message* message::m_callPointStackHead = nullptr;//コールポイントスタックの先頭
	thread_local message::buff_t* message::m_messageBuff = nullptr;//メッセージバッファ
	thread_local std::size_t message::m_messageBuffSize = 0;//メッセージバッファサイズ
	thread_local std::size_t message::m_messageBuffUsed = 0;//メッセージバッファ使用量
	//----------------------------------------
	//メッセージ初期化（一回限り）
	void message::initializeOnce()
	{
		//初期化済みチェック
		if (m_isInitialized)
			return;
		//静的変数を初期化
		m_isInitialized = true;
		resetAttrG();
		resetLogLevelAllG();
		resetNoticeLevelAllG();
	}
	//----------------------------------------
	//メッセージ用変数
	static message s_messageForInitialize;//初期化処理実行のためのインスタンス

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/log_message.inl>

#endif//GASHA_INCLUDED_LOG_MESSAGE_H

// End of file

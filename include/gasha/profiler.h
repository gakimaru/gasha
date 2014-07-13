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

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//プロファイラ
//--------------------------------------------------------------------------------

	//----------------------------------------
	//プロファイラクラス
	class CProfiler
	{
	public:
		//定数
		static const std::size_t BUFF_SIZE = 64 * 1024;//記録用のバッファ
	public:
		//型
		typedef CStackAllocatorWithBuff<BUFF_SIZE> buffer_t;//バッファ型
		typedef uintptr_t key_t;//キー型
		//簡易文字列型
		struct PROFILE;
		struct THREAD_INFO;
		class string
		{
			friend struct PROFILE;
			friend struct THREAD_INFO;
		public:
			//オペレータ
			const char* operator()() const { return m_str; }
		public:
			//キャストオペレータ
			operator const char*() const { return m_str; }
		public:
			//メソッド
			const char* c_str() const { return m_str; }
		private:
			//ムーブコンストラクタ
			string(string&& src) :
				m_str(src.m_str)
			{
				src.m_str = nullptr;
			}
			//コンストラクタ
			string(const char* str, crc32_t crc = 0):
				m_str(nullptr)
			{
				if (!str)
					return;
				//CRCが渡されたら優先的に文字列プールテーブルをチェック
				strTable_t::iterator end_ite = m_strTable->end();
				strTable_t::iterator ite;
				if (crc != 0)
				{
					ite = m_strTable->find(crc);
					if (ite != end_ite)
					{
						//文字列プールテーブルから見つかった
						m_str = ite->second;
						return;
					}
				}
				//CRC算出/文字列コピー用に文字列長を算出
				const std::size_t len = strlen(str);
				//CRC未算出なら算出して文字列プールテーブルをチェック
				if (crc == 0)
				{
					crc = calcCRC32(str, len);
					ite = m_strTable->find(crc);
				}
				if (ite != end_ite)
				{
					//文字列プールテーブルから見つかった
					m_str = ite->second;
					return;
				}
				//文字列プールテーブルに文字列を登録
				{
					const std::size_t size = len + 1;
					char* buff = new char[len + 1];//delete は destroyTable()でまとめて行う
					if (buff)
					{
						std::memcpy(buff, str, size);
						m_strTable->emplace(crc, buff);
						m_str = buff;
					}
				}
			}
			//デストラクタ
			~string()
			{}
		private:
			//フィールド
			const char* m_str;
		};
		//計測時間
		struct TIME
		{
			double m_sum;//処理時間の合計
			float m_max;//最大の処理時間
			float m_min;//最小の処理時間
			int m_count;//計測回数
			float average() const { return m_count == 0 ? 0 : static_cast<float>(m_sum / static_cast<double>(m_count)); }
			//計測時間集計
			void add(const double time)
			{
				++m_count;
				m_sum += time;
				const float time_f = static_cast<float>(time);
				if (m_max == 0.f || m_max < time_f)
					m_max = time_f;
				if (m_min == 0.f || m_min > time_f)
					m_min = time_f;
			}
			//計測時間集計
			void add(const TIME& src)
			{
				m_count += src.m_count;
				m_sum += src.m_sum;
				if (m_max == 0.f || m_max < src.m_max)
					m_max = src.m_max;
				if (m_min == 0.f || m_min > src.m_min)
					m_min = src.m_min;
			}
			//計測時間リセット
			void reset()
			{
				m_sum = 0.;
				m_max = 0.f;
				m_min = 0.f;
				m_count = 0;
			}
			//コピー演算子
			TIME& operator=(TIME& rhs)
			{
				std::memcpy(this, &rhs, sizeof(*this));
				return *this;
			}
			//ムーブコンストラクタ
			TIME(const TIME&& src) :
				m_sum(src.m_sum),
				m_max(src.m_max),
				m_min(src.m_min),
				m_count(src.m_count)
			{}
			//デフォルトコンストラクタ
			TIME() :
				m_sum(0.),
				m_max(0.f),
				m_min(0.f),
				m_count(0)
			{}
		};
		//計測時間情報
		struct MEASURE
		{
			TIME m_total;//処理全体の集計
			TIME m_frame;//フレーム内の集計
			bool m_inCountOnFrame;//フレームの集計があったか？
			int m_frameCount;//計測フレーム数
			float m_time;//（前回計測時の）処理時間
			//計測時間を加算
			void add(const double time)
			{
				m_time = static_cast<float>(time);
				m_frame.add(time);
				m_inCountOnFrame = true;
			}
			//フレーム集計
			void sumOnFrame()
			{
				if (m_inCountOnFrame)
				{
					m_inCountOnFrame = false;
					m_total.add(m_frame);
					m_frame.reset();
					++m_frameCount;
				}
			}
			//ムーブコンストラクタ
			MEASURE(MEASURE&& src) :
				m_total(std::move(src.m_total)),
				m_frame(std::move(src.m_frame)),
				m_inCountOnFrame(src.m_inCountOnFrame),
				m_frameCount(src.m_frameCount),
				m_time(src.m_time)
			{}
			//コンストラクタ
			MEASURE() :
				m_total(),
				m_frame(),
				m_inCountOnFrame(false),
				m_frameCount(0),
				m_time(0.f)
			{}
		};
		//スレッド情報
		struct THREAD_INFO
		{
			crc32_t m_nameCrc;//スレッド名のCRC
			string m_name;//スレッド名
			MEASURE m_measure;//計測情報
			//計測時間を加算
			void add(const double time)
			{
				m_measure.add(time);
			}
			//フレーム集計
			void sumOnFrame()
			{
				m_measure.sumOnFrame();
			}
			//ムーブコンストラクタ
			THREAD_INFO(THREAD_INFO&& src) :
				m_nameCrc(src.m_nameCrc),
				m_name(std::move(src.m_name)),
				m_measure(std::move(src.m_measure))
			{}
			//コンストラクタ
			THREAD_INFO(const crc32_t name_crc, const char* name) :
				m_nameCrc(name_crc),
				m_name(name, name_crc),
				m_measure()
			{}
		};
		typedef std::map<crc32_t, THREAD_INFO> threadList_t;//スレッド情報リスト型
		//プロファイル
		struct PROFILE
		{
			key_t m_key;//キー
			string m_name;//処理名
			string m_srcFileName;//ソースファイル名
			string m_funcName;//関数名
			MEASURE m_measure;//計測情報
			threadList_t m_threadList;
			//計測時間を加算
			void add(const double time, const CThreadID& thread_id)
			{
				m_measure.add(time);
				
				//スレッドごとの集計
				const crc32_t thread_name_crc = thread_id.getNameCrc();
				const char* thread_name = thread_id.getName();
				THREAD_INFO* thread_info = nullptr;
				{
					threadList_t::iterator ite = m_threadList.find(thread_name_crc);
					if (ite != m_threadList.end())
						thread_info = &ite->second;
					else
					{
						m_threadList.emplace(thread_name_crc, std::move(THREAD_INFO(thread_name_crc, thread_name)));
						ite = m_threadList.find(thread_name_crc);
						if (ite != m_threadList.end())
							thread_info = &ite->second;
					}
				}
				if (thread_info)
				{
					thread_info->add(time);
				}
			}
			//フレーム集計
			void sumOnFrame()
			{
				if (m_measure.m_inCountOnFrame)
				{
					m_measure.sumOnFrame();

					for (auto& ite : m_threadList)
					{
						ite.second.sumOnFrame();
					}
				}
			}
			//ムーブコンストラクタ
			//※要素追加の際の無駄なオブジェクト生成と破棄を防ぐ
			PROFILE(PROFILE&& src) :
				m_key(src.m_key),
				m_name(std::move(src.m_name)),
				m_srcFileName(std::move(src.m_srcFileName)),
				m_funcName(std::move(src.m_funcName)),
				m_measure(std::move(src.m_measure)),
				m_threadList(std::move(src.m_threadList))
			{}
			//コンストラクタ
			PROFILE(const key_t key, const char* name, const char* src_file_name, const char* func_name) :
				m_key(key),
				m_name(name),
				m_srcFileName(src_file_name),
				m_funcName(func_name),
				m_measure()
			{}
		};
		typedef std::unordered_map<key_t, PROFILE> table_t;//プロファイルテーブル型（ハッシュテーブル）
		typedef table_t::iterator iterator;//イテレータ型
		typedef table_t::const_iterator const_iterator;//イテレータ型
		typedef std::unordered_map<crc32_t, const char*> strTable_t;//スレッド名テーブル型（ハッシュテーブル）
	public:
		//アクセッサ
		std::size_t getBuffTotal()const{ return m_buff.getTotal(); }
		std::size_t getBuffUsed()const{ return m_buff.getUsed(); }
		std::size_t getBuffRemain()const{ return m_buff.getRemain(); }
	public:
		//メソッド
		//プロファイル追加／集計
		void addProfile(const key_t key, const char* name, const CThreadID& thread_id, const char* src_file_name, const char* func_name, const double time)
		{
			m_lock.lock();
			CTempPolyStackAllocator allocator(m_buff);
			PROFILE* profile = nullptr;
			{
				table_t::iterator ite = m_table->find(key);
				if (ite != m_table->end())
					profile = &ite->second;
				else
				{
					m_table->emplace(key, std::move(PROFILE(key, name, src_file_name, func_name)));
					ite = m_table->find(key);
					if (ite != m_table->end())
						profile = &ite->second;
				}
			}
			if (profile)
			{
				profile->add(time, thread_id);
			}
			m_lock.unlock();
		}
		//フレーム集計
		void sumOnFrame()
		{
			m_lock.lock();
			for (auto& ite : *m_table)
			{
				PROFILE* profile = &ite.second;
				profile->sumOnFrame();
			}
			m_lock.unlock();
		}
		//イテレータ取得
		std::size_t size() const { return m_table->size(); }
		//table_t::iterator begin() { return m_table->begin(); }
		//table_t::iterator end() { return m_table->end(); }
		table_t::const_iterator begin() const { return m_table->begin(); }
		table_t::const_iterator end() const { return m_table->end(); }
		table_t::const_iterator cbegin() const { return m_table->cbegin(); }
		table_t::const_iterator cend() const { return m_table->cend(); }
		//テーブル生成
		static void createTable()
		{
			m_lock.lock();
			if (!m_table)
			{
				CTempPolyStackAllocator allocator(m_buff);
				//プロファイルテーブルを生成
				m_table = new table_t();
				//m_table->reserve(1024);//予約
				//文字列プールテーブルを生成
				m_strTable = new strTable_t();
				//m_strTable->reserve(1024);//予約
			}
			m_lock.unlock();
		}
		//テーブル破棄
		static void destroyTable()
		{
			m_lock.lock();
			if (m_table)
			{
				CTempPolyStackAllocator allocator(m_buff);
				//文字列プールテーブルを破棄
				for (auto& ite : *m_strTable)
				{
					if (ite.second)
						delete[] ite.second;//文字列を破棄
				}
				delete m_strTable;
				m_strTable = nullptr;
				//プロファイルテーブルを破棄
				delete m_table;
				m_table = nullptr;
				//プロファイル／文字列用バッファをクリア
				m_buff.clearN();
			}
			m_lock.unlock();
		}
		//テーブルをリセット
		void resetTable()
		{
			destroyTable();
			createTable();
		}
	public:
		//コンストラクタ
		CProfiler()
		{
			createTable();
		}
		//デストラクタ
		~CProfiler()
		{
			//シングルトンなのでデストラクタではなにもしない
		}
	private:
		//フィールド
		static table_t* m_table;//プロファイルハッシュテーブル（ハッシュテーブル）
		static strTable_t* m_strTable;//文字列プールテーブル（ハッシュテーブル）
		static buffer_t m_buff;//プロファイル／文字列用バッファ
		static CSpinLock m_lock;//ロック
	};
	//----------------------------------------
	//プロファイラの静的変数をインスタンス化
	CProfiler::table_t* CProfiler::m_table = nullptr;//プロファイルハッシュテーブル（ハッシュテーブル）
	CProfiler::strTable_t* CProfiler::m_strTable = nullptr;//文字列プールテーブル（ハッシュテーブル）
	CProfiler::buffer_t CProfiler::m_buff;//プロファイル／文字列用バッファ
	CSpinLock CProfiler::m_lock;//ロック

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/profiler.inl>

#endif//GASHA_INCLUDED_PROFILER_H

// End of file

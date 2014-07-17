#pragma once
#ifndef GASHA_INCLUDED_STD_LOG_PRINT_INL
#define GASHA_INCLUDED_STD_LOG_PRINT_INL

//--------------------------------------------------------------------------------
// std_log_print.inl
// 標準ログ出力【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/std_log_print.h>//標準ログ出力【宣言部】

#include <gasha/log_level.h>//ログレベル
#include <gasha/log_category.h>//ログカテゴリ
#include <gasha/log_attr.h>//ログ属性
#include <gasha/string.h>//文字列処理：spprintf()

#include <cstdint>//C++11 std::uint32_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準ログ出力
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//標準ログ出力
inline void stdLogPrint:: operator()(GASHA_ logPrintInfo& info)
{
	for (logPrintInfo::purpose_type purpose = 0; purpose < logPrintInfo::PURPOSE_NUM; ++purpose)
	{
		//コンソールが指定されていれば出力
		IConsole* console = info.m_consoles[purpose];
		if (console)
		{
			//属性取得
			const GASHA_ logAttr::attr_type attr = info.m_attr;

			//出力開始
			console->beginOutput();

			//カラー変更
			bool color_is_changed = false;
			if (!GASHA_ logAttr::has(attr, purpose, GASHA_ withoutColor))
			{
				const consoleColor* color = info.m_colors[purpose];
				if (color && !color->isStandard())
				{
					console->changeColor(*color);
					color_is_changed = true;
				}
			}

			//ヘッダー出力
			if (GASHA_ logAttr::has(attr, purpose, GASHA_ withAnyHeader))
			{
				char header[128];
				static const std::size_t max_header_size = sizeof(header) - 2;//※ヘッダーの終端分の文字数を引いておく
				std::size_t header_size = 0;
				GASHA_ spprintf(header, max_header_size, header_size, "[");
				bool is_first = true;
				auto with_sep = [&is_first, &header, &header_size]()
				{
					if (!is_first)
						GASHA_ spprintf(header, max_header_size, header_size, "/");
					else
						is_first = false;
				};
				if (GASHA_ logAttr::has(attr, purpose, GASHA_ withID))
				{
					with_sep();
					GASHA_ spprintf(header, max_header_size,  header_size, "%d", info.m_id);
				}
				if (GASHA_ logAttr::has(attr, purpose, GASHA_ withTime))
				{
					const double time = static_cast<double> (info.m_time);
					const std::uint32_t total_sec = static_cast<std::uint32_t>(time);
					const double dec = time - static_cast<double>(total_sec);
					const std::uint32_t total_minute = total_sec / 60;
					const std::uint32_t total_hour = total_minute / 60;
					const std::uint32_t sec = total_sec % 60;
					const std::uint32_t minute = total_minute % 60;
					//const std::uint32_t hour = total_hour % 24;
					//const std::uint32_t day = total_hour / 24;
					const std::uint32_t dec_i = static_cast<std::uint32_t>(dec * 1000000.);
					with_sep();
					GASHA_ spprintf(header, max_header_size, header_size, "%02d:%02d:%02d.%06d", total_hour, minute, sec, dec_i);
				}
				if (GASHA_ logAttr::has(attr, purpose, GASHA_ withLevel))
				{
					GASHA_ logLevel level(info.m_level);
					with_sep();
					GASHA_ spprintf(header, max_header_size, header_size, level.name());
				}
				if (GASHA_ logAttr::has(attr, purpose, GASHA_ withCategory))
				{
					GASHA_ logCategory category(info.m_category);
					with_sep();
					GASHA_ spprintf(header, max_header_size, header_size, category.name());
				}
				if (GASHA_ logAttr::has(attr, purpose, GASHA_ withCPName))
				{
					with_sep();
					//GASHA_ spprintf(header, max_header_size, header_size, ???);
				}
				if (GASHA_ logAttr::has(attr, purpose, GASHA_ withCriticalCPName))
				{
					with_sep();
					//GASHA_ spprintf(header, max_header_size, header_size, ???);
				}
				GASHA_ spprintf(header, header_size, "]");
				console->output(header);
			}
			
			//カラーを戻す
			if (color_is_changed && GASHA_ logAttr::has(attr, purpose, GASHA_ headerOnlyColored))
			{
				console->resetColor();
				color_is_changed = false;
			}

			//メッセージ出力
			console->output(info.m_message);

			//改行
			bool with_cr =
				(purpose == ofLog && !logAttr::has(info.m_attr, logWithoutCr)) ||
				(purpose == ofNotice && logAttr::has(info.m_attr, noticeWithCr));
			if (with_cr)
				console->outputCr(color_is_changed);
			else
			{
				//カラーを戻す
				if (color_is_changed)
					console->resetColor();
			}
			//出力終了
			console->endOutput();
		}
	}
}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_STD_LOG_PRINT_INL

// End of file

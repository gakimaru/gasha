#pragma once
#ifndef GASHA_INCLUDED_STD_LOG_PRINT_INL
#define GASHA_INCLUDED_STD_LOG_PRINT_INL

//--------------------------------------------------------------------------------
// std_log_print.inl
// 標準ログ出力【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/std_log_print.h>//標準ログ出力【宣言部】

#include <gasha/log_level.h>//ログレベル
#include <gasha/log_category.h>//ログカテゴリ
#include <gasha/log_attr.h>//ログ属性
#include <gasha/call_point.h>//コールポイント
#include <gasha/string.h>//文字列処理：spprintf()

#include <cstdint>//C++11 std::uint32_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準ログ出力
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//標準ログ出力
inline void stdLogPrint::operator()(GASHA_ logPrintInfo& info)
{
	for (logPrintInfo::purpose_type purpose = 0; purpose < logPrintInfo::PURPOSE_NUM; ++purpose)
	{
		//コンソールが指定されていれば出力
		iConsole* console = info.console(purpose);
		if (console)
		{
			//属性取得
			const GASHA_ logAttr::attr_type attr = info.attr();

			//出力開始
			console->begin();

			//カラー変更
			bool color_changed = false;
			if (!GASHA_ logAttr::has(attr, purpose, GASHA_ logPurposeWithoutColor))
			{
				const consoleColor* color = info.color(purpose);
				if (color && !color->isStandard())
				{
					console->changeColor(*color);
					color_changed = true;
				}

			}

			//ヘッダー出力
			if (GASHA_ logAttr::has(attr, purpose, GASHA_ logPurposeWithAnyHeader))
			{
				char header[128];
				static const std::size_t max_header_size = sizeof(header) - 2;//※ヘッダーの終端分の文字数を引いておく
				std::size_t header_len = 0;
				bool is_first = true;
				//ヘッダー始端
				GASHA_ spprintf(header, max_header_size, header_len, "[");
				//項目のセパレータ表示用共通関数
				auto with_sep = [&is_first, &header, &header_len]()
				{
					if (!is_first)
						GASHA_ spprintf(header, max_header_size, header_len, "|");
					else
						is_first = false;
				};
				//ID
				if (GASHA_ logAttr::has(attr, purpose, GASHA_ logPurposeWithID))
				{
					with_sep();
					GASHA_ spprintf(header, max_header_size,  header_len, "%d", info.id());
				}
				//時間
				if (GASHA_ logAttr::has(attr, purpose, GASHA_ logPurposeWithTime))
				{
					with_sep();
					char time_str[32];
					GASHA_ timeToStr(time_str, info.time(), timeStr_HHMMSS_MICRO);
					GASHA_ spprintf(header, max_header_size, header_len, time_str);
				}
				//ログレベル名
				if (GASHA_ logAttr::has(attr, purpose, GASHA_ logPurposeWithLevel))
				{
					with_sep();
					GASHA_ logLevel level(info.level());
					if (level.isExist())
						GASHA_ spprintf(header, max_header_size, header_len, level.name());
				}
				//ログカテゴリ名
				if (GASHA_ logAttr::has(attr, purpose, GASHA_ logPurposeWithCategory))
				{
					with_sep();
					GASHA_ logCategory category(info.category());
					if (category.isExist())
						GASHA_ spprintf(header, max_header_size, header_len, category.name());
				}
				//※ログキューを通すと正しいコールポイントが得られない
				////コールポイント名
				//if (GASHA_ logAttr::has(attr, purpose, GASHA_ logPurposeWithCPName))
				//{
				//	GASHA_ callPoint cp;
				//	const GASHA_ callPoint* recent_cp = cp.find();
				//	if (recent_cp)
				//	{
				//		with_sep();
				//		GASHA_ spprintf(header, max_header_size, header_len, recent_cp->name());
				//	}
				//}
				////クリティカルコールポイント名
				//if (GASHA_ logAttr::has(attr, purpose, GASHA_ logPurposeWithCriticalCPName))
				//{
				//	GASHA_ callPoint cp;
				//	const GASHA_ callPoint* recent_cp = cp.findCritical();
				//	if (recent_cp)
				//	{
				//		with_sep();
				//		GASHA_ spprintf(header, max_header_size, header_len, recent_cp->name());
				//	}
				//}
				//ヘッダー終端
				GASHA_ spprintf(header, header_len, "]");
				console->put(header);
			}
			
			//カラーを戻す
			if (color_changed && GASHA_ logAttr::has(attr, purpose, GASHA_ logPurposeHeaderOnlyColored))
			{
				console->resetColor();
				color_changed = false;
			}

			//メッセージ出力
			console->put(info.message());

			//カラーを戻す
			if (color_changed)
			{
				console->resetColor();
				color_changed = false;
			}

			//改行
			bool with_cr =
				(purpose == ofLog && !logAttr::has(info.attr(), logWithoutCr)) ||
				(purpose == ofNotice && logAttr::has(info.attr(), noticeWithCr));
			if (with_cr)
				console->putCr();
			
			//出力終了
			console->end();
		}
	}
}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_STD_LOG_PRINT_INL

// End of file

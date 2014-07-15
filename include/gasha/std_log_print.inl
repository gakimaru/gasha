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
			//出力開始
			console->beginOutput();

			//カラー変更
			const consoleColor* color = info.m_colors[purpose];
			bool color_is_changed = false;
			if (color && !color->isStandard())
			{
				console->changeColor(*color);
				color_is_changed = true;
			}

			char buff[20];
			console->printf(buff, "[%d] ", info.m_id);
			console->output(buff);

			//メッセージ出力
			console->output(info.m_message);

			if (!info.m_attr.hasAttr(logWithoutCr))
			{
				//改行出力
				console->outputCr();
			}
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

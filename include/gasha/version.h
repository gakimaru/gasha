#pragma once
#ifndef GASHA_INCLUDED_VERSION_H
#define GASHA_INCLUDED_VERSION_H

//--------------------------------------------------------------------------------
// version.h
// GASAHバージョン情報【宣言部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

//----------------------------------------
//GASHAバージョン情報

#define GASHA_VERSION_FIGURE (1000000)//合成バージョン計算用桁上げ数
#define GASHA_VERSION_MAKE(MAJOR, MINOR) (MAJOR * GASHA_VERSION_FIGURE + MINOR)//合成バージョン作成マクロ

#define GASHA_MAJOR (2)//メジャーバージョン
#define GASHA_MINOR (1)//マイナーバージョン

#define GASHA_VERSION GASHA_VERSION_MAKE(GASHA_MAJOR, GASHA_MINOR)//合成バージョン

#define GASHA_PREREQ(MAJOR, MINOR) (GASHA_VERSION_MAKE(MAJOR, MINOR) >= GASHA_VERSION)//バージョン比較

//----------------------------------------
//バージョン更新履歴
//・Ver.2.01 [master] Aug.11.2014 M.Itagaki
//・Ver.2.00 [master] Aug.10.2014 M.Itagaki
//・Ver.1.01 [master] Jul.28.2014 M.Itagaki
//・Ver.1.00 [master] Jul.26.2014 M.Itagaki

#endif//GASHA_INCLUDED_VERSION_H

// End of file

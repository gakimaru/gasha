#pragma once
#ifndef GASHA_INCLUDED_LW_SPIN_LOCK_INL
#define GASHA_INCLUDED_LW_SPIN_LOCK_INL

//--------------------------------------------------------------------------------
// lw_spin_lock.inl
// サイズ軽量スピンロック【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lw_spin_lock.h>//サイズ軽量スピンロック【宣言部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//サイズ軽量スピンロッククラス

//単一ロック取得
inline GASHA_ unique_lock<lwSpinLock> lwSpinLock::lockUnique(){ GASHA_ unique_lock<lwSpinLock> lock(*this); return lock; }
inline GASHA_ unique_lock<lwSpinLock> lwSpinLock::lockUnique(const GASHA_ with_lock_t&){ GASHA_ unique_lock<lwSpinLock> lock(*this, GASHA_ with_lock); return lock; }
inline GASHA_ unique_lock<lwSpinLock> lwSpinLock::lockUnique(const GASHA_ try_to_lock_t&){ GASHA_ unique_lock<lwSpinLock> lock(*this, GASHA_ try_to_lock); return lock; }
inline GASHA_ unique_lock<lwSpinLock> lwSpinLock::lockUnique(const GASHA_ adopt_lock_t&){ GASHA_ unique_lock<lwSpinLock> lock(*this, GASHA_ adopt_lock); return lock; }
inline GASHA_ unique_lock<lwSpinLock> lwSpinLock::lockUnique(const GASHA_ defer_lock_t&){ GASHA_ unique_lock<lwSpinLock> lock(*this, GASHA_ defer_lock); return lock; }

//ロックガード取得
inline GASHA_ lock_guard<lwSpinLock> lwSpinLock::lockScoped()
{
	GASHA_ lock_guard<lwSpinLock> lock(*this);
	return lock;//※ムーブコンストラクタが作用するか、最適化によって呼び出し元の領域を直接初期化するので、ロックの受け渡しが成立する。
}

//ロック取得を試行
inline bool lwSpinLock::try_lock()
{
	bool prev = false;
	return m_lock.compare_exchange_weak(prev, true) == false;
}

//ロック解放
inline void lwSpinLock::unlock()
{
	m_lock.store(false);
}

//コンストラクタ
inline lwSpinLock::lwSpinLock()
{
	m_lock.store(false);//ロック用フラグ
}

//デストラクタ
inline lwSpinLock::~lwSpinLock()
{}

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_LW_SPIN_LOCK_INL

// End of file

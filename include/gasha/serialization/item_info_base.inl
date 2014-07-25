#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_ITEM_INFO_BASE_INL
#define GASHA_INCLUDED_SERIALIZATION_ITEM_INFO_BASE_INL

//--------------------------------------------------------------------------------
// serialization/item_base.inl
// シリアライズ/データ項目情報基底クラス【インライン関数／テンプレート関数定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/item_info_base.h>//シリアライズ/データ項目情報基底クラス【宣言部】

#include <cstring>//std::memcpy()
#include <type_traits>//C++11std::is_const

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/データ項目情報基底クラス
	//--------------------------------------------------------------------------------

	//--------------------
	//データ項目情報基底クラス
	
	//名前のCRC
	inline GASHA_ crc32_t itemInfoBase::nameCrc() const
	{
		return m_nameCrc;
	}
	
	//データ項目名
	inline const char* itemInfoBase::name() const
	{
		return m_name;
	}
	
	//データ型情報
	inline const std::type_info& itemInfoBase::typeInfo() const
	{
		return *m_itemType;
	}
	
	//オブジェクト型か？
	inline bool itemInfoBase::isObj() const
	{
		return m_attr.isObj();
	}
	
	//配列型か？
	inline bool itemInfoBase::isArr() const
	{
		return m_attr.isArr();
	}
	
	//ポインタ型か？
	inline bool itemInfoBase::isPtr() const
	{
		return m_attr.isPtr();
	}
	
	//ヌルか？
	inline bool itemInfoBase::isNul() const
	{
		return m_attr.isNul();
	}
	
	//可変長か？
	inline bool itemInfoBase::isVLen() const
	{
		return m_attr.isVLen();
	}
	
	//【参考用】（セーブデータ上の）配列要素数を取得
	//※シリアライズ形式によっては正しくない可能性がある
	inline std::size_t itemInfoBase::extent() const
	{
		return	m_arrNum == 0 ?
					1 :
					m_arrNum;
	}
	
	//現在の配列要素数を取得
	inline std::size_t itemInfoBase::nowExtent() const
	{
		return	m_hasNowInfo ?
					m_nowArrNum == 0 ?
						1 :
						m_nowArrNum :
					0;
	}
	
	//【参考用】最小の配列要素数を取得
	//※シリアライズ形式によっては正しくない可能性がある
	inline std::size_t itemInfoBase::minExtent() const
	{
		return	m_hasNowInfo ?
					m_arrNum < m_nowArrNum ?
						m_arrNum == 0 ?
							1 :
							m_arrNum :
						m_nowArrNum == 0 ?
							1 :
							m_nowArrNum :
					m_arrNum == 0 ?
						 1 :
						 m_arrNum;
	}
	
	//現在のデータはオブジェクト型か？
	inline bool itemInfoBase::nowIsObj() const
	{
		return m_hasNowInfo && m_nowAttr.isObj();
	}
	
	//現在のデータは配列型か？
	inline bool itemInfoBase::nowIsArr() const
	{
		return m_hasNowInfo && m_nowAttr.isArr();
	}
	
	//現在のデータはポインタ型か？
	inline bool itemInfoBase::nowIsPtr() const
	{
		return m_hasNowInfo && m_nowAttr.isPtr();
	}
	
	//現在のデータはヌルか？
	inline bool itemInfoBase::nowIsNul() const
	{
		return m_hasNowInfo && m_nowAttr.isNul();
	}
	
	//現在のデータは可変長か？
	inline bool itemInfoBase::nowIsVLen() const
	{
		return m_hasNowInfo && m_nowAttr.isVLen();
	}
	
	//現在のデータとセーブデータの属性が一致するか？
	inline bool itemInfoBase::nowAndSaveDataIsSameAttr() const
	{
		return m_hasNowInfo && m_nowAttr == m_attr;
	}
	
	//現在のデータとセーブデータの属性が一致しないか？
	inline bool itemInfoBase::nowAndSaveDataIsDifferentAttr() const
	{
		return m_hasNowInfo && m_nowAttr != m_attr;
	}
	
	//現在のデータもセーブデータもオブジェクト型か？
	inline bool itemInfoBase::nowAndSaveDataIsObj() const
	{
		return m_hasNowInfo && m_nowAttr.isObj() && m_attr.isObj();
	}
	
	//現在のデータもセーブデータも配列型か？
	inline bool itemInfoBase::nowAndSaveDataIsArr() const
	{
		return m_hasNowInfo && m_nowAttr.isArr() && m_attr.isArr();
	}
	
	//現在のデータもセーブデータもポインタ型か？
	inline bool itemInfoBase::nowAndSaveDataIsPtr() const
	{
		return m_hasNowInfo && m_nowAttr.isPtr() && m_attr.isPtr();
	}
	
	//現在のデータもセーブデータもはヌルか？
	inline bool itemInfoBase::nowAndSaveDataIsNul() const
	{
		return m_hasNowInfo && m_nowAttr.isNul() && m_attr.isNul();
	}
	
	//現在のデータはオブジェクト型だがセーブデータはそうではないか？
	inline bool itemInfoBase::nowIsObjButSaveDataIsNot() const
	{
		return m_hasNowInfo && m_nowAttr.isObj() && !m_attr.isObj();
	}
	
	//現在のデータは配列型だがセーブデータはそうではないか？
	inline bool itemInfoBase::nowIsArrButSaveDataIsNot() const
	{
		return m_hasNowInfo && m_nowAttr.isArr() && !m_attr.isArr();
	}
	
	//現在のデータはポインタ型だがセーブデータはそうではないか？
	inline bool itemInfoBase::nowIsPtrButSaveDataIsNot() const
	{
		return m_hasNowInfo && m_nowAttr.isPtr() && !m_attr.isPtr();
	}
	
	//現在のデータはヌルだがセーブデータはそうではないか？
	inline bool itemInfoBase::nowIsNulButSaveDataIsNot() const
	{
		return m_hasNowInfo && m_nowAttr.isNul() && !m_attr.isNul();
	}
	
	//現在のデータはオブジェクト型ではないがセーブデータはそうか？
	inline bool itemInfoBase::nowIsNotObjButSaveDataIs() const
	{
		return m_hasNowInfo && !m_nowAttr.isObj() && m_attr.isObj();
	}
	
	//現在のデータは配列型ではないがセーブデータはそうか？
	inline bool itemInfoBase::nowIsNotArrButSaveDataIs() const
	{
		return m_hasNowInfo && !m_nowAttr.isArr() && m_attr.isArr();
	}
	
	//現在のデータはポインタ型ではないがセーブデータはそうか？
	inline bool itemInfoBase::nowIsNotPtrButSaveDataIs() const
	{
		return m_hasNowInfo && !m_nowAttr.isPtr() && m_attr.isPtr();
	}
	
	//現在のデータはヌルではないがセーブデータはそうか？
	inline bool itemInfoBase::nowIsNotNulButSaveDataIs() const
	{
		return m_hasNowInfo && !m_nowAttr.isNul() && m_attr.isNul();
	}
	
	//現在のサイズの方とセーブデータのサイズが同じか？
	inline bool itemInfoBase::nowSizeIsSame() const
	{
		return !isObj() && !isVLen() && m_hasNowInfo && m_nowItemSize == m_itemSize;
	}
	
	//現在のサイズの方がセーブデータのサイズより小さいか？
	inline bool itemInfoBase::nowSizeIsSamall() const
	{
		return !isObj() && !isVLen() && m_hasNowInfo && m_nowItemSize < m_itemSize;
	}
	
	//現在のサイズの方がセーブデータのサイズより大きいか？
	inline bool itemInfoBase::nowSizeIsLarge() const
	{
		return !isObj() && !isVLen() && m_hasNowInfo && m_nowItemSize > m_itemSize;
	}
	
	//現在の配列要素数とセーブデータの配列要素数が同じか？
	inline bool itemInfoBase::nowArrIsSame() const
	{
		return m_hasNowInfo && m_nowArrNum == m_arrNum;
	}
	
	//現在の配列要素数の方がセーブデータの配列要素数より小さいか？
	inline bool itemInfoBase::nowArrIsSmall() const
	{
		return m_hasNowInfo && m_nowArrNum < m_arrNum;
	}
	
	//現在の配列要素数の方がセーブデータの配列要素数より大きいか？
	inline bool itemInfoBase::nowArrIsLarge() const
	{
		return m_hasNowInfo && m_nowArrNum > m_arrNum;
	}
	
	//現在の情報コピー済み取得
	inline bool itemInfoBase::hasNowInfo() const
	{
		return m_hasNowInfo;
	}
	
	//セーブデータ上にのみ存在するデータか？
	inline bool itemInfoBase::isOnlyOnSaveData() const
	{
		return m_isOnlyOnSaveData;
	}
	
	//セーブデータ上にのみ存在するデータか？を更新
	inline void itemInfoBase::setOnlyOnSaveData() const
	{
		m_isOnlyOnSaveData = true; m_isOnlyOnMem = false;
	}
	
	//セーブデータ上にのみ存在するデータか？を更新
	inline void itemInfoBase::setOnlyOnSaveData(const bool enabled) const
	{
		if (enabled) setOnlyOnSaveData();
	}
	
	//セーブデータ上にのみ存在するデータか？をリセット
	inline void itemInfoBase::resetOnlyOnSaveData() const
	{
		m_isOnlyOnSaveData = false;
	}
	
	//セーブデータ上にないデータか？
	inline bool itemInfoBase::isOnlyOnMem() const
	{
		return m_isOnlyOnMem;
	}
	
	//セーブデータ上にないデータか？を更新
	inline void itemInfoBase::setOnlyOnMem() const
	{
		m_isOnlyOnMem = true; m_isOnlyOnSaveData = false;
	}
	
	//セーブデータ上にないデータか？を更新
	inline void itemInfoBase::setOnlyOnMem(const bool enabled) const
	{
		if (enabled) setOnlyOnMem();
	}
	
	//セーブデータ上にないデータか？をリセット
	inline void itemInfoBase::resetOnlyOnMem() const
	{
		m_isOnlyOnMem = false;
	}
	
	//処理済みか？
	inline bool itemInfoBase::isAlready() const
	{
		return m_isAlready;
	}
	
	//処理済みにする
	inline void itemInfoBase::setIsAlready() const
	{
		m_isAlready = true;
	}
	
	//処理済みを解除する
	inline void itemInfoBase::resetIsAlready() const
	{
		m_isAlready = false;
	}
	
	//値取得
	template<typename T>
	inline T& itemInfoBase::get()
	{
		return *static_cast<T*>(const_cast<void*>(m_itemP));
	}
	
	//値取得（配列要素）
	template<typename T>
	inline T& itemInfoBase::get(const int index)
	{
		return static_cast<T*>(const_cast<void*>(m_itemP))[index];
	}
	
	//constで値取得
	template<typename T>
	inline const T& itemInfoBase::get() const
	{
		return *static_cast<const T*>(m_itemP);
	}
	
	//constで値取得（配列要素）
	template<typename T>
	inline const T& itemInfoBase::get(const int index) const
	{
		return static_cast<const T*>(m_itemP)[index];
	}
	
	//constで値取得
	template<typename T>
	inline const T& itemInfoBase::getConst() const
	{
		return *static_cast<const T*>(m_itemP);
	}
	
	//constで値取得（配列要素）
	template<typename T>
	inline const T& itemInfoBase::getConst(const int index) const
	{
		return static_cast<const T*>(m_itemP)[index];
	}
	
	//個別デシリアライザーをセット
	template<typename OBJ>
	inline void itemInfoBase::setDeserializer(std::function<void(OBJ&, const itemInfoBase&)> functor)
	{
		m_deserialier = *reinterpret_cast<std::function<void()>*>(&functor);
	}
	//個別デシリアライザーを呼び出し
	template<typename OBJ>
	inline void itemInfoBase::callDeserializer(OBJ& obj)
	{
		if (m_deserialier)
			(*(reinterpret_cast<std::function<void(OBJ&, const itemInfoBase&)>*>(&m_deserialier)))(obj, *this);
	}

	//強制的に全情報をコピー
	inline void itemInfoBase::copyForce(const itemInfoBase& src)
	{
		std::memcpy(this, &src, sizeof(*this));
	}
	
	//コピーコンストラクタ
	inline itemInfoBase::itemInfoBase(const itemInfoBase& src) :
		m_name(src.m_name),
		m_nameCrc(src.m_nameCrc),
		m_itemP(src.m_itemP),
		m_itemType(src.m_itemType),
		m_itemSize(src.m_itemSize),
		m_arrNum(src.m_arrNum),
		m_attr(src.m_attr),
		m_typeCtrl(src.m_typeCtrl),
		m_nowItemSize(src.m_nowItemSize),
		m_nowArrNum(src.m_nowArrNum),
		m_nowAttr(src.m_nowAttr),
		m_nowTypeCtrl(src.m_nowTypeCtrl),
		m_deserialier(src.m_deserialier),
		m_hasNowInfo(src.m_hasNowInfo),
		m_isOnlyOnSaveData(src.m_isOnlyOnSaveData),
		m_isOnlyOnMem(src.m_isOnlyOnMem),
		m_isAlready(src.m_isAlready)
	{}
	
	//デフォルトコンストラクタ
	inline itemInfoBase::itemInfoBase() :
		m_name(nullptr),
		m_nameCrc(0),
		m_itemP(nullptr),
		m_itemType(&typeid(void)),
		m_itemSize(0),
		m_arrNum(0),
		m_attr(false, false, false, false, false),
		m_typeCtrl(GASHA_ serialization::typeCtrlBase()),
		m_nowItemSize(0),
		m_nowArrNum(0),
		m_nowAttr(false, false, false, false, false),
		m_nowTypeCtrl(GASHA_ serialization::typeCtrlBase()),
		m_deserialier(nullptr),
		m_hasNowInfo(false),
		m_isOnlyOnSaveData(false),
		m_isOnlyOnMem(false),
		m_isAlready(false)
	{}
	
	//コンストラクタ
	inline itemInfoBase::itemInfoBase(const char* name, const void* item_p, const std::type_info& item_type, const std::size_t item_size, const std::size_t arr_num, const bool is_object, const bool is_ptr, const bool is_vlen, GASHA_ serialization::typeCtrlBase type_ctrl) :
		m_name(name),
		m_nameCrc(GASHA_ calcCRC32(name)),
		m_itemP(item_p),
		m_itemType(&item_type),
		m_itemSize(static_cast<std::uint32_t>(item_size)),
		m_arrNum(static_cast<std::uint32_t>(arr_num)),
		m_attr(is_object, arr_num > 0, is_ptr, item_p == nullptr, is_vlen),
		m_typeCtrl(type_ctrl),
		m_nowItemSize(0),
		m_nowArrNum(0),
		m_nowAttr(false, false, false, false, false),
		m_nowTypeCtrl(GASHA_ serialization::typeCtrlBase()),
		m_deserialier(nullptr),
		m_hasNowInfo(false),
		m_isOnlyOnSaveData(false),
		m_isOnlyOnMem(false),
		m_isAlready(false)
	{}
	
	//コンストラクタ
	inline itemInfoBase::itemInfoBase(const itemInfoBase& src, const void* item_p, const std::type_info& item_type, const std::size_t item_size, const std::size_t arr_num, const bool is_object, const bool is_ptr, const bool is_vlen, GASHA_ serialization::typeCtrlBase type_ctrl) :
		m_name(src.m_name),
		m_nameCrc(src.m_nameCrc),
		m_itemP(item_p),
		m_itemType(&item_type),
		m_itemSize(static_cast<std::uint32_t>(item_size)),
		m_arrNum(static_cast<std::uint32_t>(arr_num)),
		m_attr(is_object, arr_num > 0, is_ptr, item_p == nullptr, is_vlen),
		m_typeCtrl(type_ctrl),
		m_nowItemSize(0),
		m_nowArrNum(0),
		m_nowAttr(false, false, false, false, false),
		m_nowTypeCtrl(GASHA_ serialization::typeCtrlBase()),
		m_deserialier(nullptr),
		m_hasNowInfo(false),
		m_isOnlyOnSaveData(false),
		m_isOnlyOnMem(false),
		m_isAlready(false)
	{}
	
	//デストラクタ
	inline itemInfoBase::~itemInfoBase()
	{}

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_INCLUDED_SERIALIZATION_ITEM_INFO_BASE_INL

// End of file

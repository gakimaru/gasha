#pragma once
#ifndef GASHA_INCLUDED_SERIALIZATION_ITEM_INFO_BASE_H
#define GASHA_INCLUDED_SERIALIZATION_ITEM_INFO_BASE_H

//--------------------------------------------------------------------------------
// serialization/item_base.h
// シリアライズ/データ項目情報基底クラス【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/item_attr.h>//シリアライズ/データ項目属性
#include <gasha/serialization/type_ctrl_base.h>//シリアライズ/型操作基底クラス

#include <gasha/crc32.h>//CRC32計算

#include <cstddef>//std::size_t
#include <typeinfo>//std::type_info
#include <cstdint>//C++11 std::uint32_t

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <functional>//C++11 std::functional
#pragma warning(pop)//【VC++】ワーニング設定を復元

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
	class itemInfoBase
	{
	public:
		//アクセッサ
		inline GASHA_ crc32_t nameCrc() const;//名前のCRC
		inline const char* name() const;//名前 ※セーブデータにしか存在しないデータ項目の場合、名前がないので注意（CRCのみ存在）
		inline const std::type_info& typeInfo() const;//データ型情報
		inline bool isObj() const;//オブジェクト型か？
		inline bool isArr() const;//配列型か？
		inline bool isPtr() const;//ポインタ型か？
		inline bool isNul() const;//ヌルか？
		inline bool isVLen() const;//可変長か？
		//【参考用】（セーブデータ上の）配列要素数を取得
		//※シリアライズ形式によっては正しくない可能性がある
		inline std::size_t extent() const;
		//現在の配列要素数を取得
		inline std::size_t nowExtent() const;
		//【参考用】最小の配列要素数を取得
		//※シリアライズ形式によっては正しくない可能性がある
		inline std::size_t minExtent() const;
		inline bool nowIsObj() const;//現在のデータはオブジェクト型か？
		inline bool nowIsArr() const;//現在のデータは配列型か？
		inline bool nowIsPtr() const;//現在のデータはポインタ型か？
		inline bool nowIsNul() const;//現在のデータはヌルか？
		inline bool nowIsVLen() const;//現在のデータは可変長か？
		inline bool nowAndSaveDataIsSameAttr() const;//現在のデータとセーブデータの属性が一致するか？
		inline bool nowAndSaveDataIsDifferentAttr() const;//現在のデータとセーブデータの属性が一致しないか？
		inline bool nowAndSaveDataIsObj() const;//現在のデータもセーブデータもオブジェクト型か？
		inline bool nowAndSaveDataIsArr() const;//現在のデータもセーブデータも配列型か？
		inline bool nowAndSaveDataIsPtr() const;//現在のデータもセーブデータもポインタ型か？
		inline bool nowAndSaveDataIsNul() const;//現在のデータもセーブデータもはヌルか？
		inline bool nowIsObjButSaveDataIsNot() const;//現在のデータはオブジェクト型だがセーブデータはそうではないか？
		inline bool nowIsArrButSaveDataIsNot() const;//現在のデータは配列型だがセーブデータはそうではないか？
		inline bool nowIsPtrButSaveDataIsNot() const;//現在のデータはポインタ型だがセーブデータはそうではないか？
		inline bool nowIsNulButSaveDataIsNot() const;//現在のデータはヌルだがセーブデータはそうではないか？
		inline bool nowIsNotObjButSaveDataIs() const;//現在のデータはオブジェクト型ではないがセーブデータはそうか？
		inline bool nowIsNotArrButSaveDataIs() const;//現在のデータは配列型ではないがセーブデータはそうか？
		inline bool nowIsNotPtrButSaveDataIs() const;//現在のデータはポインタ型ではないがセーブデータはそうか？
		inline bool nowIsNotNulButSaveDataIs() const;//現在のデータはヌルではないがセーブデータはそうか？
		inline bool nowSizeIsSame() const;//現在のサイズの方とセーブデータのサイズが同じか？
		inline bool nowSizeIsSamall() const;//現在のサイズの方がセーブデータのサイズより小さいか？
		inline bool nowSizeIsLarge() const;//現在のサイズの方がセーブデータのサイズより大きいか？
		inline bool nowArrIsSame() const;//現在の配列要素数とセーブデータの配列要素数が同じか？
		inline bool nowArrIsSmall() const;//現在の配列要素数の方がセーブデータの配列要素数より小さいか？
		inline bool nowArrIsLarge() const;//現在の配列要素数の方がセーブデータの配列要素数より大きいか？
		inline bool hasNowInfo() const;//現在の情報コピー済み取得
		inline bool isOnlyOnSaveData() const;//セーブデータ上にのみ存在するデータか？
		inline void setOnlyOnSaveData() const;//セーブデータ上にのみ存在するデータか？を更新
		inline void setOnlyOnSaveData(const bool enabled) const;//セーブデータ上にのみ存在するデータか？を更新
		inline void resetOnlyOnSaveData() const;//セーブデータ上にのみ存在するデータか？をリセット
		inline bool isOnlyOnMem() const;//セーブデータ上にないデータか？
		inline void setOnlyOnMem() const;//セーブデータ上にないデータか？を更新
		inline void setOnlyOnMem(const bool enabled) const;//セーブデータ上にないデータか？を更新
		inline void resetOnlyOnMem() const;//セーブデータ上にないデータか？をリセット
		inline bool isAlready() const;//処理済みか？
		inline void setIsAlready() const;//処理済みにする
		inline void resetIsAlready() const;//処理済みを解除する
	public:
		//オペレータ
		inline bool operator==(const itemInfoBase& rhs) const { return m_nameCrc == rhs.m_nameCrc; }//データ項目名CRCで一致判定
		inline bool operator!=(const itemInfoBase& rhs) const { return m_nameCrc != rhs.m_nameCrc; }//データ項目名CRCで不一致判定
		inline bool operator==(const GASHA_ crc32_t name_crc) const { return m_nameCrc == name_crc; }//データ項目名CRCで一致判定
		inline bool operator!=(const GASHA_ crc32_t name_crc) const { return m_nameCrc != name_crc; }//データ項目名CRCで不一致判定
		inline bool operator==(const char* name) const { return m_nameCrc == GASHA_ calcCRC32(name); }//データ項目名CRCで一致判定
		inline bool operator!=(const char* name) const { return m_nameCrc != GASHA_ calcCRC32(name); }//データ項目名CRCで不一致判定
	public:
		//キャストオペレータ
		inline operator GASHA_ crc32_t() const { return m_nameCrc; }
		inline operator const char*() const { return m_name; }
		inline operator const std::type_info& () const { return *m_itemType; }
	public:
		//メソッド
		template<typename T>//値取得
		inline T& get();
		template<typename T>//値取得（配列要素）
		inline T& get(const int index);
		template<typename T>//constで値取得
		inline const T& get() const;
		template<typename T>//constで値取得（配列要素）
		inline const T& get(const int index) const;
		template<typename T>//constで値取得
		inline const T& getConst() const;
		template<typename T>//constで値取得（配列要素）
		inline const T& getConst(const int index) const;
		//個別デシリアライザーをセット
		template<typename OBJ>
		inline void setDeserializer(std::function<void(OBJ&, const itemInfoBase&)> functor);
		//個別デシリアライザーを呼び出し
		template<typename OBJ>
		inline void callDeserializer(OBJ& obj);
		//読み込み情報をクリア
		void clearForLoad();
		//現在の情報をコピー
		void copyFromOnMem(const itemInfoBase& src);
		//強制的に全情報をコピー
		inline void copyForce(const itemInfoBase& src);
	public:
		//コピーコンストラクタ
		inline itemInfoBase(const itemInfoBase& src);
		//デフォルトコンストラクタ
		inline itemInfoBase();
		//コンストラクタ
		inline itemInfoBase(const char* name, const void* item_p, const std::type_info& item_type, const std::size_t item_size, const std::size_t arr_num, const bool is_object, const bool is_ptr, const bool is_vlen, GASHA_ serialization::typeCtrlBase type_ctrl);
		//コンストラクタ
		inline itemInfoBase(const itemInfoBase& src, const void* item_p, const std::type_info& item_type, const std::size_t item_size, const std::size_t arr_num, const bool is_object, const bool is_ptr, const bool is_vlen, GASHA_ serialization::typeCtrlBase type_ctrl);
		//デストラクタ
		inline ~itemInfoBase();
	public://フィールドを公開して直接操作
		//フィールド
		const char* m_name;//データ項目名
		const GASHA_ crc32_t m_nameCrc;//データ項目名CRC
		const void* m_itemP;//データの参照ポインタ
		const std::type_info* m_itemType;//データの型情報
		const std::uint32_t m_itemSize;//データサイズ
		const std::uint32_t m_arrNum;//データの配列要素数
		const GASHA_ serialization::itemAttr m_attr;//属性
		GASHA_ serialization::typeCtrlBase m_typeCtrl;//型操作
		std::uint32_t m_nowItemSize;//データサイズ　※現在のサイズ（デシリアライズ処理用）
		std::uint32_t m_nowArrNum;//データの配列要素数　※現在のサイズ（デシリアライズ処理用）
		GASHA_ serialization::itemAttr m_nowAttr;//属性　※現在の状態（デシリアライズ処理用）
		GASHA_ serialization::typeCtrlBase m_nowTypeCtrl;//型操作　※現在の状態（デシリアライズ処理用）
		std::function<void()> m_deserialier;//個別デシリアライザー
		bool m_hasNowInfo;//現在の情報コピー済み
		mutable bool m_isOnlyOnSaveData;//セーブデータ上にのみ存在するデータ
		mutable bool m_isOnlyOnMem;//セーブデータ上にないデータ
		mutable bool m_isAlready;//処理済み
	};

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

//.hファイルのインクルードに伴い、常に.inlファイルを自動インクルード
#include <gasha/serialization/item_info_base.inl>

#endif//GASHA_INCLUDED_SERIALIZATION_ITEM_INFO_BASE_H

// End of file

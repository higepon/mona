/**
	@file	CollectionArray.h
	@brief	コレクション 動的配列

	コレクション 動的配列

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_COLLECTIONARRAY_H
#define _MONAPI2_BASIC_COLLECTIONARRAY_H

#include "switch.h"
#include "Type.h"
#include "String.h"
#include "CollectionList.h"

namespace monapi2	{

/**
動的配列クラス。
intとかfloatとかの単純な型、又はすでにあるclass*をつなげたい時に扱う。
新しいclassなどを作ってリストに蓄えたい場合はArrayADを参照。

	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
class Array
{
public:
	Array(uint nItitialSize=0);					///<コンストラクタ。
	virtual ~Array();								///<デストラクタ

//取得
	uint getCount() const	{return m_nCount;};		///<要素数を得る
	uint getSize() const	{return m_nSize;};		///<領域の大きさを得る
	TYPE getAt(uint n) const;						///<n番目の要素を得る

//操作
	void add(TYPE t);								///<末尾に足す
	bool setAt(uint nIndex,TYPE tValue);			///<指定したインデックスの値を設定
	void setSize(uint nSize);						///<領域を確保
	bool insertAt(uint nIndex,uint nCount);			///<指定したインデックスにブロックを挿入。
	bool removeAt(uint nIndex,uint nCount);			///<指定したインデックスからブロックを削除。
	virtual void removeAll();						///<全てのメンバを消して初期化する。

///移動・並び替え
///ソートは常に昇順だが降順が欲しい場合はpFnCompareを逆にするか、後ろからアクセスすればいい・・・
	void sort(FnCompare pFnCompare);


protected:
///初期化。
	void init(uint nItitialSize);

///内部にnSizeの領域を確保する。
	TYPE* getNewSize(uint nSize);

///最低nCountが収まるまで拡張。バッファの再確保が行われバッファが移動したなら戻り値に古いバッファ、再確保はなかったのならNULL。
	TYPE* extendBuffer(uint nSize);

///安全のためのコピーコンストラクタ定義。
///ヒープポインタが破綻するのでprotectedでアクセスできないようにしてコピーは禁止。
	Array(Array&)		{};

///古いバッファ領域を削除。
	virtual void deleteOldBuffer(TYPE* pBuffer,uint nSize);

protected:
	TYPE*			m_apData;			///<内部ポインタ
	uint			m_nCount;			///<現在の要素数
	uint			m_nSize;			///<取ってあるバッファの大きさ。普通は要素数より大きい。
	FnCompare		m_pFnCompare;		///<比較関数
};


/**
メンバをnewで作って足してゆくArray。
こっちはListADほど綺麗にはかけない。
class Arrayをベースにdeleteに関わる部分を手動でオーバーライドしてゆく。

	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
class ArrayAD : public Array<TYPE>
{
public:
	~ArrayAD();								///<デストラクタ
	bool setAt(uint nIndex,TYPE tValue);
	bool removeAt(uint nIndex,uint nCount);

protected:
	virtual void deleteOldBuffer(TYPE* pBuffer,uint nSize);		///<古いバッファ領域を削除。
};

/**
	@brief	文字列を扱うArray。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
//@memo 高速化。Stringをタイプに使ってる事でバックグラウンドで結構Stringが生成されては消えている。
//String*とかにして自動生成を少しは抑えるべきだろうか・・・
class StringArray : public Array<String>
{
public:
///デフォルトのソート方法
	void sort(FnCompare pFnCompare=compareByPString)	{Array<String>::sort(pFnCompare);}
};



}	//namespace monapi2

#endif


//テンプレートは.cppに書いては認識されない事があるので.hに入れてる
#include "CollectionArrayInline.h"

/**
	@file	CollectionList.h
	@brief	コレクション リスト

	コレクション リスト

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_COLLECTIONLIST_H
#define _MONAPI2_BASIC_COLLECTIONLIST_H

#include "switch.h"
#include "Type.h"
#include "String.h"

namespace monapi2	{

//ソートで使う比較関数をtypedefしておく。
typedef int (__cdecl *FnCompare)(const void *p,const void *q);

///<StringList::sort()などで使う。
int compareByPString(const void *p, const void *q);

///リストの巡回に使う。
typedef void* position;


/**
	@brief	Listの内部で使用する。値と、次と前の要素へのポインタを持つ。これ自体はユーザーは使わない。。
			Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE>
class CListElement
{
public:
	CListElement();					///<コンストラクタ

public:
	TYPE m_tValue;					///<値
	CListElement<TYPE>* m_pPrev;	///<次の要素へのポインタ
	CListElement<TYPE>* m_pNext;	///<前の要素へのポインタ
};

//
/**
	@brief	デストラクタでm_tValueをdeleteするタイプのCListElement。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE>
class CListElementAD:public CListElement<TYPE>
{
public:
	~CListElementAD()	{delete CListElement<TYPE>::m_tValue;}	///<デストラクタ
};

/*
class Listとclass ListADのベースクラス。

リストのパラメーターには二種類があります。
一つめはint,float,class*などどの型を作成するかのタイプと
もう一つは自動deleteをオンにするかオフにするかのフラグ。

template <class TYPE,class ELEMENT>
のTYPEが前者でELEMENTが後者を指定しています。
TYPEにはint,float,class*などの任意の型を設定可能。
ELEMENTには自動deleteなしならCListElement<TYPE>、自動deleteありならCListElementAD<TYPE>の
二択が選択可能。

これをベースにパラメーター簡易版のList<TYPE>とListAD<TYPE>が組み立てられ、
そしてそれらをベースにList<int>などの実際のインスタンス化が行われます。

	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
class ListBase
{
public:
	ListBase();								///<コンストラクタ
	virtual ~ListBase();					///<デストラクタ

//取得
	position getHeadPosition() const;		///<先頭位置を得る。
	position getTailPosition() const;		///<終焉位置を得る。
	TYPE getNext(position* ppos) const;		///<巡回し、次の位置のTYPEを得る。
	TYPE getPrev(position* ppos) const;		///<巡回し、前の位置のTYPEを得る。
	uint getCount() const;					///<要素数を返す。
	bool isEmpty()	const {return (getCount()==0);}
	TYPE getAt(position pos) const;			///<posの位置のTYPEを得る。
	position find(TYPE tFind) const;		///<tFindの値を持つメンバの位置をpositionで得る。

//操作
	void addTail(TYPE t);					///<末尾にtを足す。
	void addHead(TYPE t);					///<先頭にtを足す。
	void insertAfter(position pos,TYPE t);	///<posの後にtを足す。
	void insertBefore(position pos,TYPE t);	///<posの前にtを足す。
	virtual void setAt(position pos,TYPE t);///<posの位置にtをセット。
	virtual void removeAt(position pos);	///<posの位置の要素を消す。
	virtual void removeAll();				///<全て消してリストを初期化する。

///nIndex番目の要素をリストの前、または後ろに動かす。
///@bToHead trueなら頭方向に、falseなら末尾方向に動かす。
///@return 動かした要素の新しい位置
	int moveNextPrev(uint nIndex,bool bToHead);


//検索
	position getPositionFromIndex(uint nIndex) const;	///<nIndexの位置をpositionで得る。（0=先頭）


///移動・並び替え
//ソートは常に昇順だが降順が欲しい場合はpFnCompareを逆にするか、末尾から逆にアクセスすればいい・・・
	void sort(FnCompare pFnCompare);

protected:
	void init();					///<初期化

//removeAtのリスト構造処理。
//正常に実行できた場合はposの指す要素が返ってくる。失敗した場合はNULL。
	ELEMENT* removeAt_reconstructList(position pos);			///<posの位置の要素を消す。

///安全のためのコピーコンストラクタ定義。
///ヒープポインタが破綻するのでprotectedでアクセスできないようにしてコピーは禁止。
	ListBase(ListBase&)		{};

/**
ソート関連。整列には挿入ソートを使う。

まずは順序の整ってない要素を見つける。
@param posStart 検索をスタートする場所。
@return 戻り値は入れ替えが必要なposition
*/
	position sort_FindUnordered(position posStart) const;

///@param posの要素を正しい位置までもって行く。
///@return 次の検索がスタートする場所(整列されてる事が保証されてる一番前の場所)
	position sort_BringToOrderedPos(position pos);

///m_pFnCompareと照らし合わせて順番が正しいかどうか
	bool isRightOrder(TYPE* p,TYPE* q)	const{return (m_pFnCompare(p,q) <= 0);}

#ifdef _DEBUG
	void assertSort() const;	///<デバッグ用にsort()で正しく並べられたか検証。
#endif


protected:
	ELEMENT*	m_pHead;			///<先頭要素
	ELEMENT*	m_pTail;			///<末尾要素
	uint		m_nCount;			///<要素数。本来リスト構造にはない概念だがサービスとしてgetCount()も用意しておくと便利な事もある。
	FnCompare	m_pFnCompare;		///<比較関数を覚えておく。
};


/**
	@brief	リストクラス。intとかfloatとかの単純な型、又はすでにあるclass*などをつなげたい時に扱う。
			Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
//ちなみに宣言で"TYPE> >"と半角スペースが空いているのはマクロの展開時に不都合があるのか
//なぜかそう書かないとVisual C++コンパイラで通らない・・・
template <class TYPE>
class List	: public ListBase<TYPE, CListElement<TYPE> >	{};

/**
	@brief	メンバをnewで作って足してゆくリスト。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE>
class ListAD: public ListBase<TYPE, CListElementAD<TYPE> >	{};


/**
	@brief	文字列を扱うリスト。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
//中身はListBase<String>。class Stringで定義してあるI/Oが文字列の入出力を全部面倒見てくれるので
//リスト側では実は何もしないでいい。
class StringList : public List<String>
{
public:
///デフォルトのソート方法
	void sort(FnCompare pFnCompare=compareByPString)	{List<String>::sort(pFnCompare);}
};



/**
	@brief	FILOなスタック。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
class Stack	: 	public List<TYPE>	//内部的にはリストとして実装。
{
public:
	void push(TYPE t)	{addTail(t);}
	TYPE pop();
};

/**
	@brief	FIFOなキュー。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
class Queue	: 	public List<TYPE>	//内部的にはリストとして実装。
{
public:
	void push(TYPE t)	{addTail(t);}
	TYPE pop();
};


}	//namespace monapi2

//////////////////////////////////////////////////////////////////////
//テンプレートは.cppに書いては認識されない事があるので.hに入れてる
#include "CollectionListInline.h"

#endif

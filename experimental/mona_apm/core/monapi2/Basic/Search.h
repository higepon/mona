/**
	@file	Search.h
	@brief	ソート・探索

	ソート・探索

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_SEARCH_H
#define _MONAPI2_BASIC_SEARCH_H

#include "switch.h"
#include "Type.h"
#include "Memory.h"

namespace monapi2	{

//public:///////////////
/**
	@brief	クイックソート。
	@date	2005/08/20	junjunn 作成
*/
void quicksort(void* pavArray,uint iArrayMemberCount,uint nArrayMemberWidth,int (__cdecl *pFnCompare)(const void *p,const void *q));





//protected:////////////////////////

//defineしてるとQuickSortのスピードを測るモード
//#define _PERFORMANCETEST

typedef int (__cdecl *FnQSortCompare)(const void *p,const void *q);

/**
	@brief	quicksortのルーチン実装部。
	@date	2005/08/20	junjunn 作成
*/
class CQuickSort
{
public:
///iStart番目からiStart+iCount番目までの区間をソート
	void Sort(void* pavArray,int iStart,int iCount,uint nArrayMemberWidth,FnQSortCompare pfnCompare);


protected:
///戻り値をiReturnとすると、
///これを実行しおえた時m_pavArrayは以下の二つの性質を満たすように動かされている。
///・m_pavArray[iStart]～m_pavArray[iReturn-1]の要素は全てm_pav[iReturn]より<=。
///・m_pavArray[iReturn+1]～m_pavArray[iStart+iCount-1]の要素は全てm_pav[iReturn]より>=。
	inline int Partition(int iStart,int iCount);

///iLeft、iMiddle、iRightの3つをソートする。ソートが速くなるピボットを見つけるのに使う。
	inline void sortTriplet(int iLeft,int iMiddle,int iRight);

///iIndexをアドレスに変換
	void* getElementAddress(int iIndex)		{return (char*)m_pavArray + iIndex*m_nArrayMemberWidth;}
///pvToのアドレスにpvFromの値を入れる。
	void set(void* pvTo,void* pvFrom)		{MemoryFn::copy(pvTo,pvFrom,m_nArrayMemberWidth);}

//比較
	int compare(void* p,void* q)			{return m_pfnCompare(p,q);}
///交換する必要があるか。
	bool isSwapRequired(void* p,void* q)	{return (compare(p,q)>0);}

///i1とi2の位置の値を取っ替える。
	inline void swap(int i1,int i2);

///ソートされているか検証。
	bool isSorted();


/* 未完成
///要素数が少ないときはQuickSortよりも別のソート法を使った方がオーバーヘッドの関係で速くなる。
///そのための小ソート。
	inline void SmallSort(int iStart,int iCount);
///SmallSortのサブルーチン
	inline void InsertionSort(int iStart,int iCount);
*/

protected:
	int m_iStart;					///<ソートのスタート位置
	int m_iCount;					///<ソートのカウント
	void* m_pavArray;				///<ソートすべき配列
	uint m_nArrayMemberWidth;		///<ソートすべき配列のメンバのビット幅
	FnQSortCompare m_pfnCompare;	///<比較関数

	char* m_pSwapBuffer;			///<Swap()中の一時待避領域に使う。Swap()の中で毎回newして確保するのも効率悪いので。


public:
#ifdef _PERFORMANCETEST
	uint m_nTimeTaken;				///<ソートにかかった時間
#endif
};

//qsortの速度をテストする
#ifdef _PERFORMANCETEST
	void TestQSort();
#endif


///Intの比較
int compareByInt(const void *p,const void *q);
///charでの文字列比較
int compareByString(const void *p,const void *q);

}	//namespace monapi2

#endif

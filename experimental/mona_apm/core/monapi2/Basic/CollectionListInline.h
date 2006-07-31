/**
	@file	CollectionListInline.h
	@brief	コレクション リスト

	コレクション リスト

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_COLLECTIONLISTINLINE_H
#define _MONAPI2_BASIC_COLLECTIONLISTINLINE_H

#include "Debug.h"

namespace monapi2	{

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE>
CListElement<TYPE>::CListElement()
{
//	m_tValue=0;
	m_pNext=NULL;
	m_pPrev=NULL;
}


//ListBase////////////////////////
/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
ListBase<TYPE,ELEMENT>::ListBase()
{
	init();
}

/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
ListBase<TYPE,ELEMENT>::~ListBase()
{
	removeAll();
}

/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
void ListBase<TYPE,ELEMENT>::init()
{
	m_pHead	= NULL;
	m_pTail	= NULL;
	m_nCount			= 0;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
position ListBase<TYPE,ELEMENT>::getHeadPosition() const
{
	return (position)m_pHead;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
position ListBase<TYPE,ELEMENT>::getTailPosition() const
{
	return (position)m_pTail;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
uint ListBase<TYPE,ELEMENT>::getCount() const
{
	return m_nCount;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
TYPE ListBase<TYPE,ELEMENT>::getNext(position* ppos) const
{
	if (! ppos)		return 0;
	ELEMENT* pCurrent = *(ELEMENT**)ppos;

	*ppos = pCurrent->m_pNext;
	return (TYPE)pCurrent->m_tValue;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
TYPE ListBase<TYPE,ELEMENT>::getPrev(position* ppos) const
{
	if (! ppos)		return 0;
	ELEMENT* pCurrent = *(ELEMENT**)ppos;

	*ppos = pCurrent->m_pPrev;
	return (TYPE)pCurrent->m_tValue;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE,class ELEMENT>
TYPE ListBase<TYPE,ELEMENT>::getAt(position pos) const
{
	if (! pos)	return 0;

	ELEMENT* pListElement = (ELEMENT*)pos;
	return (TYPE)(pListElement->m_tValue);
}

/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
position ListBase<TYPE,ELEMENT>::find(TYPE tFind) const
{
	position pos = getHeadPosition();
	while (pos)
	{
		position posCur = pos;
		TYPE t = getNext(&pos);
		if (t==tFind) return posCur;
	}

	return NULL;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
void ListBase<TYPE,ELEMENT>::addTail(TYPE t)
{
	ELEMENT* pCur = new ELEMENT;
	pCur->m_tValue = t;

	pCur->m_pPrev = m_pTail;

//初回時
	if (! m_pHead)
	{
		m_pHead = pCur;
		m_pTail = pCur;
	}
	else
	{
		m_pTail->m_pNext = pCur;
		m_pTail = pCur;
	}

	m_nCount++;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
void ListBase<TYPE,ELEMENT>::addHead(TYPE t)
{
	ELEMENT* pCur = new ELEMENT;
	pCur->m_tValue = t;

	pCur->m_pNext = m_pHead;

//初回時
	if (! m_pHead)
	{
		m_pHead = pCur;
		m_pTail = pCur;
	}
	else
	{
		m_pHead->m_pPrev = pCur;
		m_pHead = pCur;
	}

	m_nCount++;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
void ListBase<TYPE,ELEMENT>::insertAfter(position pos,TYPE t)
{
	if (! pos)	return;

	ELEMENT* pCur = new ELEMENT;
	pCur->m_tValue = t;

	ELEMENT* pPrev	= (ELEMENT*)pos;
	ELEMENT* pNext	= (ELEMENT*)pPrev->m_pNext;

	pPrev->m_pNext	= pCur;
	if (pNext)	pNext->m_pPrev = pCur;
	pCur->m_pPrev	= pPrev;
	pCur->m_pNext	= pNext;

	if (! pNext)	m_pTail = pCur;

	m_nCount++;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
void ListBase<TYPE,ELEMENT>::insertBefore(position pos,TYPE t)
{
	if (! pos)	return;

	ELEMENT* pCur = new ELEMENT;
	pCur->m_tValue = t;

	ELEMENT* pNext	= (ELEMENT*)pos;
	ELEMENT* pPrev	= (ELEMENT*)pNext->m_pPrev;

	if (pPrev)	pPrev->m_pNext	= pCur;
	pNext->m_pPrev	= pCur;
	pCur->m_pPrev	= pPrev;
	pCur->m_pNext	= pNext;

	if (! pPrev)	m_pHead = pCur;

	m_nCount++;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE,class ELEMENT>
position ListBase<TYPE,ELEMENT>::getPositionFromIndex(uint nIndex) const
{
	position pos = getHeadPosition();

	uint n=0;
	while (pos)
	{
		if (n >= nIndex)	break;
		getNext(&pos);
		n++;
	}

	return pos;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE,class ELEMENT>
void ListBase<TYPE,ELEMENT>::setAt(position pos,TYPE t)
{
	if (!pos)	return;

//なんで((ELEMENT*)pos)->m_tValue = t;とただ普通に置き換えないかと言えばListADの際には前の
//データをdeleteする必要もあるので。この書き方ならListもListADも共通化できる。
	insertAfter(pos,t);
	removeAt(pos);
}

/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
void ListBase<TYPE,ELEMENT>::removeAt(position pos)
{
	ELEMENT* pCur = removeAt_reconstructList(pos);
	if (pCur)
	{
		delete pCur;
	}
}
	
/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
ELEMENT* ListBase<TYPE,ELEMENT>::removeAt_reconstructList(position pos)
{
	if (! pos)			return NULL;

	ELEMENT* pCur = (ELEMENT*)pos;
	ELEMENT* pNext= (ELEMENT*)pCur->m_pNext;
	ELEMENT* pPrev= (ELEMENT*)pCur->m_pPrev;

	if (pNext)	{pNext->m_pPrev = pCur->m_pPrev;}
	if (pPrev)	{pPrev->m_pNext = pCur->m_pNext;}
	if (pCur ==	m_pTail)	{m_pTail = pPrev;}
	if (pCur ==	m_pHead)	{m_pHead = pNext;}
	m_nCount--;

	return pCur;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template <class TYPE,class ELEMENT>
void ListBase<TYPE,ELEMENT>::removeAll()
{
	position posHead;

	while (posHead = getHeadPosition())
	{
		removeAt(posHead);
	}

	init();
}


/**
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE,class ELEMENT>
int ListBase<TYPE,ELEMENT>::moveNextPrev(uint nIndex,bool bNext)
{
	position pos = getPositionFromIndex(nIndex);
	if (! pos)		{return nIndex;}

	TYPE tSource;
	position posOld = pos;

	int iInc;
	if (bNext)
	{
		if (nIndex>=m_nCount)	return nIndex;
		tSource = getNext(&pos);
		iInc=1;
	}
	else
	{
		if (nIndex==0)			return nIndex;
		tSource = getPrev(&pos);
		iInc=-1;
	}
	if (pos==NULL)	{return nIndex;}

	TYPE tDestin	= getAt(pos);
	setAt(pos		,tSource);
	setAt(posOld	,tDestin);

	return nIndex+iInc;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE,class ELEMENT>
void ListBase<TYPE,ELEMENT>::sort(FnCompare pFnCompare)
{
	m_pFnCompare=pFnCompare;

	position posHinge = getHeadPosition();
	while ((posHinge=sort_FindUnordered(posHinge))!=NULL)	//trueならposHingeは正しい場所にいないのでこのあと並べ替える
	{
//		char szState[100];char* pWrite = szState;position pos = getHeadPosition();while (pos)	{pWrite+=StringFn::format(pWrite,"%d,",getNext(&pos));}

		posHinge=sort_BringToOrderedPos(posHinge);			//posHingeを順番が正しい位置に並び替え。
		posHinge = getHeadPosition();						//先頭からまた検査

//		pWrite = szState;pos = getHeadPosition();while (pos){pWrite+=StringFn::format(pWrite,"%d,",getNext(&pos));}
	}

#ifdef _DEBUG
	assertSort();
#endif
}

/**
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE,class ELEMENT>
position ListBase<TYPE,ELEMENT>::sort_FindUnordered(position posStart) const
{
//支点
	while(posStart)
	{
		TYPE t1 = getNext(&posStart);
		if (posStart==NULL) return NULL;	//比べる対象がないので帰る

//比較点
		position pos2=posStart;
		while (pos2)
		{
			position posBookmark=pos2;
			TYPE t2 = getNext(&pos2);

			if (! isRightOrder(&t1,&t2))		return posBookmark;
		}
	}

	return NULL;
}

/**
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE,class ELEMENT>
position ListBase<TYPE,ELEMENT>::sort_BringToOrderedPos(position pos)
{
	position posOrig=pos;

//一個後に移動＆オリジナルの情報
	TYPE t1=getPrev(&pos);
	position posReturn=pos;

	while (pos)
	{
		position posBookmark = pos;
		TYPE t2 = getPrev(&pos);	//前に昇ってゆく

		if (! isRightOrder(&t1,&t2))	//正しい順番でいる限り前に動き続ける
		{
			if (posBookmark==NULL)		{addHead(t1);}
			else						{insertAfter(posBookmark,t1);}

			removeAt(posOrig);	//古いのを取り除く
			return posReturn;
		}
	}

//先頭に来た
	addHead(t1);
	removeAt(posOrig);	//古いのを取り除く
	return posReturn;
}

#ifdef _DEBUG
/**
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE,class ELEMENT>
void ListBase<TYPE,ELEMENT>::assertSort() const
{
	if (getCount()<2)	return;

	position pos = getHeadPosition();
	TYPE t1 = getNext(&pos);

	while(pos)
	{
		TYPE t2 = getNext(&pos);

//		ASSERT(isRightOrder(&t1,&t2));

		t1=t2;
	}
}
#endif


//Stack////////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
TYPE Stack<TYPE>::pop()
{
	position posTail = List<TYPE>::getTailPosition();
	if (posTail==NULL)	return 0;
	TYPE tReturn = List<TYPE>::getAt(posTail);
	List<TYPE>::removeAt(posTail);

	return tReturn;
}

//Queue////////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
TYPE Queue<TYPE>::pop()
{
	position posHead = List<TYPE>::getHeadPosition();
	if (posHead==NULL)	return 0;
	TYPE tReturn = List<TYPE>::getAt(posHead);
	List<TYPE>::removeAt(posHead);

	return tReturn;
}

}	//namespace monapi2
#endif

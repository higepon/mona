/**
	@file	CollectionMapInline.h
	@brief	コレクション マップ

	コレクション マップ

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_COLLECTIONMAPINLINE_H
#define _MONAPI2_BASIC_COLLECTIONMAPINLINE_H

#include "Memory.h"

namespace monapi2	{

//MapBase//////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template <class KEYTYPE,class VALUETYPE>
void MapBase<KEYTYPE,VALUETYPE>::initHashTable(uint nSize)
{
//	ASSERT(nSize % 10!=0);	//念のためちゃんと素数を選んだか簡単な検証・・・
	m_nHashTableSize = nSize;
}

//MapUncollidable///////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
void MapUncollidable<KEYTYPE,VALUETYPE>::init()
{
	m_apListPKVP=NULL;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
MapUncollidable<KEYTYPE,VALUETYPE>::~MapUncollidable()
{
	removeAll();
}


/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
void MapUncollidable<KEYTYPE,VALUETYPE>::initHashTable(uint nSize)
{
	MapBase<KEYTYPE,VALUETYPE>::initHashTable(nSize);

	m_apListPKVP = new ListADPSKVPair*[nSize];
	for (uint n=0;n<nSize;n++)
	{
		m_apListPKVP[n] = new ListADPSKVPair;
	}
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
SKVPairBase<KEYTYPE,VALUETYPE>*
 MapUncollidable<KEYTYPE,VALUETYPE>::lookup_Internal(uint nHash,KEYTYPE tKey) const
{
	ListADPSKVPair* pListAD;
	monapi2::position pos;
	if (lookup_Internal(nHash,tKey,&pListAD,&pos))
	{
		return pListAD->getAt(pos);
	}

	return NULL;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
bool MapUncollidable<KEYTYPE,VALUETYPE>::lookup_Internal(uint nHash,KEYTYPE tKey,ListADPSKVPair** ppListOut,monapi2::position* pPosOut) const
{
//同一ハッシュ値になった要素を集めたリスト
	ListADPSKVPair* pList = m_apListPKVP[nHash];

//リストの大きさが0なら見つかるはずもないので却下。
	if (pList->getCount() == 0)	return false;

//リストを巡回して各要素をtKeyとの直接比較。
	monapi2::position pos = pList->getHeadPosition();
	while (pos)
	{
		monapi2::position pCurPos = pos;
		SKVPair* pKVP = pList->getNext(&pos);
		if (pKVP->m_tKey == tKey)
		{
			*ppListOut = pList;
			*pPosOut = pCurPos;
			return true;
		}
	}
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
void MapUncollidable<KEYTYPE,VALUETYPE>::removeAll()
{
	if (MapBase<KEYTYPE,VALUETYPE>::m_nHashTableSize==0)	return;

	for (uint n=0;n<MapBase<KEYTYPE,VALUETYPE>::m_nHashTableSize;n++)
	{
		ListADPSKVPair* pList = m_apListPKVP[n];
		delete pList;
	}

	delete m_apListPKVP;

	MapBase<KEYTYPE,VALUETYPE>::m_nHashTableSize=0;
	m_apListPKVP=NULL;
	MapBase<KEYTYPE,VALUETYPE>::m_nCount=0;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
void MapUncollidable<KEYTYPE,VALUETYPE>::setAt(KEYTYPE tKey,const VALUETYPE tValue)
{
	MapBase<KEYTYPE,VALUETYPE>::m_nCount++;

	uint nHash = getHash(tKey);

	SKVPair* pKVP = lookup_Internal(nHash,tKey);
	if (pKVP)
	{
		pKVP->m_tValue = tValue;
	}
	else
	{
		SKVPair* pKVP	= new SKVPair;
		pKVP->m_tKey	= tKey;
		pKVP->m_tValue	= tValue;

		m_apListPKVP[nHash]->addTail(pKVP);
	}
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
bool MapUncollidable<KEYTYPE,VALUETYPE>::lookup(KEYTYPE tKey,VALUETYPE* ptOut) const
{
	uint nHash = getHash(tKey);

//キーに対応する値を探す。
	SKVPair* pKVP = lookup_Internal(nHash,tKey);

//見つかった
	if (pKVP)
	{
		*ptOut = pKVP->m_tValue;
		return true;
	}
//見つからなかった
	else
	{
		return false;
	}
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
bool MapUncollidable<KEYTYPE,VALUETYPE>::removeAt(KEYTYPE tKey)
{
	uint nHash = getHash(tKey);

	ListADPSKVPair* pListAD;
	monapi2::position pos;
	if (lookup_Internal(nHash,tKey,&pListAD,&pos))
	{
		pListAD->removeAt(pos);
		MapBase<KEYTYPE,VALUETYPE>::m_nCount--;
		return true;
	}

	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
mapposition MapUncollidable<KEYTYPE,VALUETYPE>::getStartPosition() const
{
	mapposition pos;

	findNextPosition(&pos);
	return pos;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
void MapUncollidable<KEYTYPE,VALUETYPE>::getNext(mapposition* ppos,KEYTYPE* ptKey,VALUETYPE* ptValue) const
{
	ListADPSKVPair* pList = m_apListPKVP[ppos->m_iListArrayIndex];
	position pos = pList->getPositionFromIndex(ppos->m_iListIndex);
	SKVPair* pKVPair = pList->getAt(pos);
	*ptKey	= pKVPair->m_tKey;
	*ptValue= pKVPair->m_tValue;

	findNextPosition(ppos);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
void MapUncollidable<KEYTYPE,VALUETYPE>::findNextPosition(mapposition* ppos) const
{
	(ppos->m_iListIndex)++;

	for (;ppos->m_iListArrayIndex < (int)MapBase<KEYTYPE,VALUETYPE>::m_nHashTableSize;(ppos->m_iListArrayIndex)++)
	{
		ListADPSKVPair* pList = m_apListPKVP[ppos->m_iListArrayIndex];

		if (ppos->m_iListIndex >= (int)pList->getCount())
		{
			ppos->m_iListIndex=0;
			continue;
		}
		else
		{
			return;
		}
	}

	ppos->setInvalid();
}

//MapCollidable///////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
void MapCollidable<KEYTYPE,VALUETYPE>::init()
{
	m_apKVP=NULL;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
MapCollidable<KEYTYPE,VALUETYPE>::~MapCollidable()
{
	removeAll();
}


/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
void MapCollidable<KEYTYPE,VALUETYPE>::initHashTable(uint nSize)
{
	MapBase<KEYTYPE,VALUETYPE>::initHashTable(nSize);

	ASSERT(m_apKVP==NULL);

	m_apKVP = new SKVPair*[nSize];
	for (uint n=0;n<nSize;n++)
	{
		m_apKVP[n] = NULL;
	}
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
void MapCollidable<KEYTYPE,VALUETYPE>::removeAll()
{
	if (MapBase<KEYTYPE,VALUETYPE>::m_nHashTableSize==0)	return;

	for (uint n=0;n<MapBase<KEYTYPE,VALUETYPE>::m_nHashTableSize;n++)
	{
		SKVPair* pKVP = m_apKVP[n];
		if (pKVP)		delete pKVP;
	}

	delete m_apKVP;

	MapBase<KEYTYPE,VALUETYPE>::m_nHashTableSize=0;
	m_apKVP=NULL;
	MapBase<KEYTYPE,VALUETYPE>::m_nCount=0;
}


//CStringMapSingle///////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
void MapCollidable<KEYTYPE,VALUETYPE>::setAt(KEYTYPE tKey,VALUETYPE tValue)
{
	MapBase<KEYTYPE,VALUETYPE>::m_nCount++;

	uint nHash = getHash(tKey);

	SKVPair* pKVP = lookup_Internal(nHash);
	if (pKVP)
	{
		pKVP->m_tKey	= tKey;
		pKVP->m_tValue	= tValue;
	}
	else
	{
		SKVPair* pKVP = new SKVPair;
		pKVP->m_tKey	= tKey;
		pKVP->m_tValue	= tValue;

		m_apKVP[nHash] = pKVP;
	}
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
bool MapCollidable<KEYTYPE,VALUETYPE>::lookup(KEYTYPE tKey,VALUETYPE* ptOut) const
{
	uint nHash = getHash(tKey);

//キーに対応する値を探す。
	SKVPair* pKVP = lookup_Internal(nHash);

//見つかったが、ハッシュされる先が一致するだけでまだキーが一致とは限らない。
	if (pKVP)
	{
//キーが同じか
		if (pKVP->m_tKey==tKey)
		{
			*ptOut = pKVP->m_tValue;
			return true;
		}
//キーが消えてる。
		else
		{
			return false;
		}
	}
//見つからなかった
	else
	{
		return false;
	}
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
bool MapCollidable<KEYTYPE,VALUETYPE>::removeAt(KEYTYPE tKey)
{
	uint nHash = getHash(tKey);
	SAFE_DELETE(m_apKVP[nHash]);
	MapBase<KEYTYPE,VALUETYPE>::m_nCount--;
	return true;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
mapposition MapCollidable<KEYTYPE,VALUETYPE>::getStartPosition() const
{
	mapposition pos;
	findNextPosition(&pos);
	return pos;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
void MapCollidable<KEYTYPE,VALUETYPE>::getNext(mapposition* ppos,KEYTYPE* ptKey,VALUETYPE* ptValue) const
{
	SKVPair* pKVPair = m_apKVP[ppos->m_iListArrayIndex];
	*ptKey	= pKVPair->m_tKey;
	*ptValue= pKVPair->m_tValue;

	findNextPosition(ppos);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
void MapCollidable<KEYTYPE,VALUETYPE>::findNextPosition(mapposition* ppos) const
{
	(ppos->m_iListArrayIndex)++;

	for (;ppos->m_iListArrayIndex < (int)MapBase<KEYTYPE,VALUETYPE>::m_nHashTableSize;(ppos->m_iListArrayIndex)++)
	{
		if (m_apKVP[ppos->m_iListArrayIndex])
		{
			return;
		}
	}

	ppos->setInvalid();
}

}	//namespace monapi2

#endif

/**
	@file	CollectionArrayInline.h
	@brief	コレクション 動的配列

	コレクション 動的配列

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_COLLECTIONARRAYINLINE_H
#define _MONAPI2_BASIC_COLLECTIONARRAYINLINE_H

#include "Search.h"
#include "Math.h"

namespace monapi2	{

//Array//////////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
Array<TYPE>::Array(uint nItitialSize)
{
	init(nItitialSize);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
void Array<TYPE>::init(uint nItitialSize)
{
	m_nCount		= 0;
	m_nSize			= nItitialSize;
	m_apData		= NULL;

	getNewSize(nItitialSize);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
Array<TYPE>::~Array()
{
	removeAll();
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
void Array<TYPE>::deleteOldBuffer(TYPE* pBuffer,uint )	//引数は次の継承クラスで使う。ここではダミー。
{
	delete[] pBuffer;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
TYPE Array<TYPE>::getAt(uint n) const
{
	if (n>=m_nCount)	return 0;

	return m_apData[n];
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
void Array<TYPE>::add(TYPE t)
{
	TYPE* pOldBuffer = extendBuffer(m_nCount+1);

//バッファの変動があった。
	if (pOldBuffer)
	{
//前の要素を移動。
		for (uint n=0;n<m_nCount;n++)
		{
			m_apData[n] = pOldBuffer[n];
		}
//データ削除ではなく移動なのでサイズ指定は0にしてある。
		deleteOldBuffer(pOldBuffer,0);
	}

	m_apData[m_nCount] = t;
	m_nCount++;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
TYPE* Array<TYPE>::extendBuffer(uint nSize)
{
//要求以上の領域をすでに持っているので何もしないでいい。
	if (nSize <= m_nSize)	return NULL;

	uint nNewSize = m_nSize;
	if (nNewSize == 0)	nNewSize =1;

//大きさは倍々で
	while (nNewSize < nSize)
		nNewSize*=2;

//バッファを確保。
	return getNewSize(nNewSize);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
bool Array<TYPE>::setAt(uint nIndex,TYPE tValue)
{
	if (nIndex>=getCount())	return false;

	m_nCount=getMax(m_nCount,nIndex);
	m_apData[nIndex] = tValue;
	return true;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
bool Array<TYPE>::insertAt(uint nIndex,uint nCount)
{
	if (nIndex>=getCount())	return false;

	uint nOldCount = m_nCount;
	m_nCount+=nCount;

//	uint nOldSize = getSize();
//バッファの拡張を試みる。(十分にあった場合は何も起こらない。)
	TYPE* pOldBuffer = extendBuffer(m_nCount);

	uint n;
//バッファの変動があった。
	if (pOldBuffer)
	{
//挿入インデックス前の要素。
		for (n=0;n<nIndex;n++)
		{
			m_apData[n] = pOldBuffer[n];
		}
	}

//挿入インデックス後の要素を移動。
	for (n=nIndex+nCount;n<nOldCount+nCount;n++)
	{
		m_apData[n] = pOldBuffer[n-nCount];
	}

//挿入された場所は空にしておく。（前のデータが残っててdeleteされるとマズい。）
	for (n=nIndex;n<nIndex+nCount;n++)
	{
		m_apData[n] = 0;
	}

	if (pOldBuffer)
	{
//deleteOldBufferを使うとArrayADの場合に蓄えていたポインタも破壊されるのでこっちが正解か。
		delete[] pOldBuffer;
//		deleteOldBuffer(pOldBuffer,nOldSize);
	}

	return true;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
void Array<TYPE>::setSize(uint nSize)
{
	uint nOldCount = getCount();
	TYPE* pOldBuffer = getNewSize(nSize);
	m_nCount=nSize;

//バッファの変動があった。
	if (pOldBuffer)
	{
//前の要素を移動。
		for (uint n=0;n<nOldCount;n++)
		{
			m_apData[n] = pOldBuffer[n];
		}
//削除ではなく移動なのでサイズは0。
		deleteOldBuffer(pOldBuffer,0);
	}
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
TYPE* Array<TYPE>::getNewSize(uint nSize)
{
	if (nSize==0)	return NULL;

//バッファを確保。
	TYPE* apNewData = new TYPE[nSize];
	for (uint n=0;n<nSize;n++)
	{
		apNewData[n]=0;
	}
	m_nSize = nSize;

	TYPE* pOldBuffer = m_apData;
	m_apData = apNewData;
	return pOldBuffer;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
void Array<TYPE>::sort(FnCompare pFnCompare)
{
	quicksort(m_apData,m_nCount,sizeof(TYPE),pFnCompare);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
bool Array<TYPE>::removeAt(uint nIndex,uint nCount)
{
	if (nIndex+nCount>=getCount())	return false;

//インデックス後の要素を移動。
	for (uint n=nIndex+nCount;n<getCount();n++)
	{
		m_apData[n-nCount] = m_apData[n];
	}

	m_nCount-=nCount;

	return true;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
void Array<TYPE>::removeAll()
{
//Sizeは確保してあってもバッファがあるとは限らないのでここはgetSizeではなくgetCountで正しい。
	deleteOldBuffer(m_apData,getCount());

	init(0);
}


//ArrayAD///////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
ArrayAD<TYPE>::~ArrayAD()
{
	Array<TYPE>::removeAll();
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
void ArrayAD<TYPE>::deleteOldBuffer(TYPE* pBuffer,uint nSize)
{
	for (uint n=0;n<nSize;n++)
		delete pBuffer[n];
	delete pBuffer;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
bool ArrayAD<TYPE>::setAt(uint nIndex,TYPE tValue)
{
	if (nIndex>=Array<TYPE>::getCount())	return false;

	Array<TYPE>::m_nCount = getMax(Array<TYPE>::m_nCount,nIndex);
	delete Array<TYPE>::m_apData[nIndex];
	Array<TYPE>::m_apData[nIndex] = tValue;
	return true;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
template<class TYPE>
bool ArrayAD<TYPE>::removeAt(uint nIndex,uint nCount)
{
	if (nIndex+nCount>=Array<TYPE>::getCount())	return false;

//範囲を削除
	for (uint n=nIndex;n<nIndex + nCount;n++)
	{
		delete Array<TYPE>::m_apData[n];
	}

	return Array<TYPE>::removeAt(nIndex,nCount);
}


} //namespace monapi2

#endif

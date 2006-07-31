/**
	@file	Buffer.cpp
	@brief	汎用バッファ

	汎用バッファ

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "Buffer.h"
#include "Memory.h"

namespace monapi2	{

//Buffer/////////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
Buffer::Buffer()						
{
	m_pBuffer	= NULL;
	clear();
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
Buffer::Buffer(int iInitialSize)
{
	m_pBuffer	=	NULL;
	extendBuffer(iInitialSize);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
Buffer::~Buffer()
{
	clear();
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Buffer::clear()
{
	SAFE_DELETE(m_pBuffer);
//	m_iLength	= 0;
	m_iSize		= 0;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
byte* Buffer::extendBuffer(int iNewSize,bool bKeepOriginalData)
{
//もし格納できる領域があるなら再確保はしないで現在の内容をクリア。
	if (m_iSize>=iNewSize)
	{
//		m_iLength=0;
		return m_pBuffer;
	}

	byte* pNewBuffer = new byte[iNewSize];
	m_iSize = iNewSize;
//	m_iLength = iNewSize;

	if (bKeepOriginalData)
	{
		MemoryFn::copy(pNewBuffer,m_pBuffer,getSize());
	}
	else
	{
//		m_iLength=0;
	}

	delete m_pBuffer;
	m_pBuffer	= pNewBuffer;

	return m_pBuffer;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Buffer::copy(const byte* cpSource,int iLength)
{
	extendBuffer(iLength);

//	m_iLength	= iLength;
	MemoryFn::copy(m_pBuffer,cpSource,iLength);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Buffer::copy(const Buffer* pbuf)	
{
	copy(pbuf->m_pBuffer,pbuf->getSize());
}


}		//namespace monapi2

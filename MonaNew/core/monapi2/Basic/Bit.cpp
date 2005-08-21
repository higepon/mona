/**
	@file	Memory.h
	@brief	メモリ読み込み・メモリ書き込み・メモリ操作

	メモリ読み込み・メモリ書き込み・メモリ操作

	License=Mona License
    \version $Revision$
    \date	$Date$
*/

#include "Type.h"
#include "Bit.h"
#include "ASSERT.h"

using namespace monapi2;

const uint BitFn::m_acnBitMask[]={
	0x00000001,	0x00000002,	0x00000004,	0x00000008,
	0x00000010,	0x00000020,	0x00000040,	0x00000080,
	0x00000100,	0x00000200,	0x00000400,	0x00000800,
	0x00001000,	0x00002000,	0x00004000,	0x00008000,
	0x00010000,	0x00020000,	0x00040000,	0x00080000,
	0x00100000,	0x00200000,	0x00400000,	0x00800000,
	0x01000000,	0x02000000,	0x04000000,	0x08000000,
	0x10000000,	0x20000000,	0x40000000,	0x80000000};


//BitFn///////////

///@author junjunn @date update:2005/08/20
bool BitFn::get(int iTarget,int iBitIndex)
{
//	ASSERT(iBitIndex >= 0);
	return ((iTarget & m_acnBitMask[iBitIndex]) != 0);
}

///@author junjunn @date update:2005/08/20
void BitFn::set(int *piTarget,int iBitIndex,bool bOn)
{
//	ASSERT(iBitIndex >= 0);

	int iBitMeasurer = m_acnBitMask[iBitIndex];

	if (bOn)
	{
		*piTarget |= iBitMeasurer;
	}
	else
	{
		*piTarget &= ~iBitMeasurer;
	}
}

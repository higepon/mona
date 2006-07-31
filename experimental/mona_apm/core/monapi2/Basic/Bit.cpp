/**
	@file	Bit.cpp
	@brief	ビット操作

	ビット操作

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "Bit.h"

namespace monapi2	{

///BitFn::getとBitFn::setで使う。
const uint BitFn::m_acnBitMask[]={
	0x00000001,	0x00000002,	0x00000004,	0x00000008,
	0x00000010,	0x00000020,	0x00000040,	0x00000080,
	0x00000100,	0x00000200,	0x00000400,	0x00000800,
	0x00001000,	0x00002000,	0x00004000,	0x00008000,
	0x00010000,	0x00020000,	0x00040000,	0x00080000,
	0x00100000,	0x00200000,	0x00400000,	0x00800000,
	0x01000000,	0x02000000,	0x04000000,	0x08000000,
	0x10000000,	0x20000000,	0x40000000,	0x80000000};


//BitFn///////////////////////////////////////
//public
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool BitFn::get(int iTarget,int iBitIndex)
{
//	ASSERT(iBitIndex >= 0);
	return ((iTarget & m_acnBitMask[iBitIndex]) != 0);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void BitFn::set(int *piTarget,int iBitIndex,bool bOn)
{
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

}		//namespace monapi2

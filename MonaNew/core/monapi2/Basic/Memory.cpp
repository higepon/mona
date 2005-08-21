/**
	@file	Memory.cpp
	@brief	メモリ読み込み・メモリ書き込み・メモリ操作

	メモリ読み込み・メモリ書き込み・メモリ操作

	License=Mona License
    \version $Revision$
    \date	$Date$
*/

#include "Switch.h"
#include "Memory.h"
#include "Math.h"

using namespace monapi2;

/*
高速化メモ。

forの処理を
	for(uint n=0;n<count;n++)
と書くときと
	for(uint n=count;n>0;n--)
と書くときは後者の方がループごとにcountの値を呼び出す必要がないので速そうだが
比較テストした結果は違いはなかった。

コンパイラの最適化処理でどっちも最速な形に直されるのか。
書く方は気にしないでよさそうだ。
*/


///@author junjunn @date update:2005/08/20
void MemoryFn::copy(void* pTo,const void* cpFrom, uint nCount)
{
//スピードを上げるために4バイト転送を使う。
//まず転送バイト数を4で割って商と剰余を計算し、4バイト転送と1バイト転送にわける。
	int i4ByteCount,i1ByteCount;
	MathFn::divide(nCount,4,&i4ByteCount,&i1ByteCount);

#ifdef _COMPILER_VISUALC

//4バイト転送
	__asm
	{
		mov edi,pTo			//レジスタセット
		mov esi,cpFrom
		mov ecx,i4ByteCount
		cld					//順方向転送指示
		rep movsd			//4バイト転送命令
	}

//1バイト転送
	__asm
	{
		mov ecx,i1ByteCount	//他のレジスタ値は4バイト転送の時にセットした時から流用
		rep movsb			//1バイト転送
	}
#else
//4バイト転送部分
    asm volatile("movl %0, %%edi \n"
                 "movl %1, %%esi \n"
                 "movl %2, %%ecx \n"
                 "cld            \n"
                 "rep movsd      \n"
                 :
                 : "m"(pTo), "m"(cpFrom), "m"(i4ByteCount)
                 : "edi", "esi", "ecx");

//1バイト転送部分
    asm volatile("movl %0, %%ecx \n"
                 "rep movsb      \n"
                 :
                 : "m"(i1ByteCount)
                 : "ecx");
#endif
}

///@author junjunn @date update:2005/08/20
void MemoryFn::swap(void* pBuffer1,void* pBuffer2,uint nCount)
{
//スピードを上げるために4バイト転送を使う。
	int i4ByteCount,i1ByteCount;
	MathFn::divide(nCount,4,&i4ByteCount,&i1ByteCount);

//4バイト転送部分
	uint32* pdwBuffer1=(uint32*)pBuffer1;
	uint32* pdwBuffer2=(uint32*)pBuffer2;
	for(uint n=0;n<(uint)i4ByteCount;n++)
	{
		int iTemp = *pdwBuffer1;
		*pdwBuffer1++ = *pdwBuffer2;
		*pdwBuffer2++ = iTemp;
	}

//1バイト転送部分
	uint8* pcBuffer1=(uint8*)pdwBuffer1;
	uint8* pcBuffer2=(uint8*)pdwBuffer2;
	for(n=0;n<(uint)i1ByteCount;n++)
	{
		char cTemp = *pcBuffer1;
		*pcBuffer1++ = *pcBuffer2;
		*pcBuffer2++ = cTemp;
	}
}

///@author junjunn @date update:2005/08/20
void MemoryFn::copySafe(void* pTo, const void* cpFrom,uint nCount)
{
//普通のcopyでは困る条件
	if (pTo>cpFrom && pTo<(uint8*)cpFrom+nCount)
	{
//スピードを上げるために4バイト転送を使う。
		int i4ByteCount,i1ByteCount;
		MathFn::divide(nCount,4,&i4ByteCount,&i1ByteCount);

//反対方向からコピー
//1バイト転送部分
		uint8* pcTo		=(uint8*)pTo	+ nCount;
		uint8* cpcFrom	=(uint8*)cpFrom	+ nCount;
		for(uint n=0;n<(uint)i1ByteCount;n++)
		{
			*--pcTo = *--cpcFrom;
		}

//4バイト転送部分
		uint32* pdwTo		=(uint32*)pcTo;
		uint32* cpdwFrom	=(uint32*)cpcFrom;
		for(n=0;n<(uint)i4ByteCount;n++)
		{
			*--pdwTo = *--cpdwFrom;
		}
	}
	else
	{
		copy(pTo,cpFrom,nCount);
	}
}

///@author junjunn @date update:2005/08/20
int MemoryFn::compare(const void* cpBuffer1,const void* cpBuffer2,uint nCount)
{
//スピードを上げるために4バイト比較を使う。
	int i4ByteCount,i1ByteCount;
	MathFn::divide(nCount,4,&i4ByteCount,&i1ByteCount);

	uint32* cpdwBuffer1=(uint32*)cpBuffer1;
	uint32* cpdwBuffer2=(uint32*)cpBuffer2;
	for (uint n=0;n<(uint)i4ByteCount;n++)
	{
		if (*cpdwBuffer1!=*cpdwBuffer2)
		{
			uint8* cpcBuffer1=(uint8*)cpdwBuffer1;
			uint8* cpcBuffer2=(uint8*)cpdwBuffer2;
			for(n=0;n<4;n++)
			{
				if (*cpcBuffer1!=*cpcBuffer2)	return (cpcBuffer1-(uint8*)cpBuffer1);

				cpcBuffer1++;
				cpcBuffer2++;
			}
		}

		cpdwBuffer1++;
		cpdwBuffer2++;
	}

	uint8* cpcBuffer1=(uint8*)cpdwBuffer1;
	uint8* cpcBuffer2=(uint8*)cpdwBuffer2;
	for(n=0;n<(uint)i1ByteCount;n++)
	{
		if (*cpcBuffer1!=*cpcBuffer2)	return (cpcBuffer1-(uint8*)cpBuffer1);

		cpcBuffer1++;
		cpcBuffer2++;
	}

	return 0;
}

///@author junjunn @date update:2005/08/20
void MemoryFn::set(void* pTo,int iValue,uint nCount)
{
//スピードを上げるために4バイト転送を使う。
	int i4ByteCount,i1ByteCount;
	MathFn::divide(nCount,4,&i4ByteCount,&i1ByteCount);

//4バイト転送部分
	uint32* pdwTo=(uint32*)pTo;
	int iValue4 = iValue + (iValue<<8) + (iValue<<16) + (iValue<<24);
	for(uint n=0;n<(uint)i4ByteCount;n++)
	{
		*pdwTo++ = iValue4;
	}

//1バイト転送部分
	uint8* pcTo=(uint8*)pdwTo;
	for(n=0;n<(uint)i1ByteCount;n++)
	{
		*pcTo++ = (uint8)iValue;
	}
}

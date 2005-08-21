/**
	@file	Memory.h
	@brief	メモリ読み込み・メモリ書き込み・メモリ操作

	メモリ読み込み・メモリ書き込み・メモリ操作

	License=Mona License
    \version $Revision$
    \date	$Date$
*/
#ifndef _MONAPI2_BASIC_MEMORY_H
#define _MONAPI2_BASIC_MEMORY_H

#include "Type.h"

namespace monapi2	{

class MemoryFn
{
public:
	static void copy(void* pTo,const void* cpFrom,uint nCount);
	static void swap(void* pBuffer1,void* pBuffer2,uint nCount);
	static void copySafe(void* pTo, const void* cpFrom,uint nCount);
	static int compare(const void* cpBuffer1,const void* cpBuffer2,uint nCount);
	static void set(void* pto,int iValue,uint nCount);
};

}	//namespace monapi2


#endif

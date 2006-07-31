/**
	@file	Memory.h
	@brief	メモリ読み込み・メモリ書き込み・メモリ操作

	メモリ読み込み・メモリ書き込み・メモリ操作

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_MEMORY_H
#define _MONAPI2_BASIC_MEMORY_H

#include "switch.h"
#include "Type.h"

namespace monapi2	{

#ifndef SAFE_DELETE
//deleteした後に間違って使われないよう消去。
	#define SAFE_DELETE(p)	{delete p;p=NULL;}
#endif

/**
	@brief	メモリ操作。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
class MemoryFn
{
public:
	static uint copy(void* pTo,const void* cpFrom,uint nCount);
	static void swap(void* pBuffer1,void* pBuffer2,uint nCount);
	static void copySafe(void* pTo, const void* cpFrom,uint nCount);
	static int findDiff(const void* cpBuffer1,const void* cpBuffer2,uint nCount);
	static void set(void* pto,int iValue,uint nCount);
	static bool isEqual(const void* cpBuffer1,const void* cpBuffer2,uint nCount)
		{return (findDiff(cpBuffer1,cpBuffer2,nCount)==-1);}
};

}	//namespace monapi2


#endif

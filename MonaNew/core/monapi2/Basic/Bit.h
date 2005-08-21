/**
	@file	Memory.h
	@brief	メモリ読み込み・メモリ書き込み・メモリ操作

	メモリ読み込み・メモリ書き込み・メモリ操作

	License=Mona License
    \version $Revision$
    \date	$Date$
*/
#ifndef _MONAPI2_BASIC_BIT_H
#define _MONAPI2_BASIC_BIT_H

namespace monapi2	{

///int型の変数にビット単位で入出力する。staticクラス
class BitFn
{
public:
	static bool get(int iTarget,int iBitIndex);				///<iTargetのiBit番目のビットのboolを返す。
	static void set(int *piTarget,int iBitIndex,bool bOn);	///<*piTargetのiBit番目のビットにbOnをセットする。

	static const uint m_acnBitMask[32];
};

}	//namespace monapi2

#endif

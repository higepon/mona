/**
	@file	Bit.h
	@brief	ビット操作

	ビット操作

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_BIT_H
#define _MONAPI2_BASIC_BIT_H

#include "switch.h"
#include "Type.h"

namespace monapi2	{

/**
	@brief int型の変数にビット単位で入出力する。staticクラス
	@date	2005/08/20	junjunn 作成
*/
class BitFn
{
public:
	static bool get(int iTarget,int iBitIndex);				///<iTargetのiBit番目のビットのboolを返す。
	static void set(int *piTarget,int iBitIndex,bool bOn);	///<*piTargetのiBit番目のビットにbOnをセットする。

	static const uint m_acnBitMask[32];
};

}	//namespace monapi2

#endif

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

inline byte getByte0(dword dw)	{return (byte)((dw & 0x000000FF));}
inline byte getByte1(dword dw)	{return (byte)((dw & 0x0000FF00) >> 8);}
inline byte getByte2(dword dw)	{return (byte)((dw & 0x00FF0000) >> 16);}
inline byte getByte3(dword dw)	{return (byte)((dw & 0xFF000000) >> 24);}
inline byte getByte0(int i)		{return (byte)((i & 0x000000FF));}
inline byte getByte1(int i)		{return (byte)((i & 0x0000FF00) >> 8);}
inline byte getByte2(int i)		{return (byte)((i & 0x00FF0000) >> 16);}
inline byte getByte3(int i)		{return (byte)((i & 0xFF000000) >> 24);}
inline byte getByte0(word w)	{return (byte)(w & 0x00FF);}
inline byte getByte1(word w)	{return (byte)(w >> 8);}
inline word getWord0(int w)		{return (word)(w & 0xFFFF);}
inline word getWord1(int w)		{return (word)(w >> 16);}

inline uint packByte(int i,int i2)	{return ((word)i) | (((word)i2)<<8);}
inline uint packWord(int i,int i2)	{return ((word)i) | (((word)i2)<<16);}

}	//namespace monapi2

#endif

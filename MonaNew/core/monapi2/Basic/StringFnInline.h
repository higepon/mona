/**
	@file	StringInline.h
	@brief	String.hのインライン関数の展開

	String.hのインライン関数の展開

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_STRINGFNINLINE_H
#define _MONAPI2_BASIC_STRINGFNINLINE_H

#include "StringFn.h"

namespace monapi2	{

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool StringFn::isEqual(pcchar1 cszP,pcchar1 cszQ,int iCompareCount)
{
//最初に1文字だけ調べてそこが違えばcompare関数を呼び出す前に終わらせれる。
	if (cszP[0]!=cszQ[0])	return false;

	return (findDiff(cszP,cszQ,iCompareCount)==-1);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool StringFn::isEqualNoCase(pcchar1 cszP,pcchar1 cszQ,int iCompareCount)
{
	return (findDiffNoCase(cszP,cszQ,iCompareCount)==-1);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool StringFn::isEmpty(pcchar1 cszSource)
{
	return isEqual(cszSource,"");
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
pchar1 StringFn::findEnd(pcchar1 pAt)
{
	return (pchar1)(pAt+getLength(pAt));
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void StringFn::empty(pchar1 szSource)
{
	szSource[0]='\0';
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
int	StringFn::join(pchar1 szOut,pcchar1 cszIn,int iCopyCount)
{
	szOut = findEnd(szOut);
	return copy(szOut,cszIn,iCopyCount);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
int	StringFn::remove(pchar1 szSrc,pcchar1 cszRemove)
{
	return replace(szSrc,cszRemove,"");
}

}	//namespace monapi2

#endif

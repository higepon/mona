/**
	@file	StringFn.cpp
	@brief	文字列関連

	文字列関連

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "StringFn.h"
#include "StringOther.h"

namespace monapi2
{

//StringDivide////////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
cpchar1	StringDivide::getAt(uint iIndex)	const
{
	if (iIndex>=m_nCount)	return NULL;

	return m_apData[iIndex].getString();
}

//StringDivide////////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/11/20	junjunn 作成
*/
String*	StringDivide::getStringAt(uint iIndex)	const
{
	if (iIndex>=m_nCount)	return NULL;

	return &m_apData[iIndex];
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void StringDivide::set(cpchar1 cszStart,cpchar1 cszToken)
{
	getStringArray()->removeAll();

	int iLengthToken = StringFn::getLength(cszToken);

//最後の区切り
	cpchar1 pLastAppear	= cszStart;
//次の区切り
	cpchar1 p			= cszStart;

//分解して見つかった文字列を置く。
	String strWord;
	while (p = StringFn::find(p,cszToken))
	{
		strWord.copy(pLastAppear,p-pLastAppear);

		add(strWord);

		p+=iLengthToken;
		pLastAppear=p;
	}

//最後の項目
	add(pLastAppear);
}

}		//namespace monapi2

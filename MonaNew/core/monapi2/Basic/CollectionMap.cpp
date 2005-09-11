/**
	@file	CollectionMap.cpp
	@brief	コレクション マップ

	コレクション マップ

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "CollectionMap.h"

namespace monapi2
{

//テンプレートを使っているとなぜか.cppだとリンクエラーにな物があるので
//そいつらはTemplateInline.hに入っている。

//MapBase///////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
uint getHash1(cpchar1 cszKey,uint nHashTableSize)
{
	uint nHash=0;

	for (cpchar1 p = cszKey;*p;p++)
	{
		nHash = (nHash * 256 + *p) % nHashTableSize;
	}

	return nHash;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
uint getHash1(int iKey,uint nHashTableSize)
{
	return iKey % nHashTableSize;
}


//mapposition//////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
mapposition::mapposition()
{
	m_iListArrayIndex = 0;
	m_iListIndex = -1;
}

}

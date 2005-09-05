/**
	@file	CollectionList.cpp
	@brief	コレクション リスト

	コレクション リスト

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "CollectionList.h"
#include "StringFn.h"

namespace monapi2
{

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
int compareByPString(const void *p, const void *q)
{
	pcchar1 p2=((String*)p)->getString();
	pcchar1 q2=((String*)q)->getString();
	return StringFn::compare(p2,q2);
}

}

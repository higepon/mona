/**
	@file	StringFn.h
	@brief	文字列関連

	文字列関連

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_STRINGOTHER_H
#define _MONAPI2_BASIC_STRINGOTHER_H

#include "Switch.h"
#include "Type.h"
#include "CollectionArray.h"

namespace monapi2	{

/**
	@brief	文字列をトークン文字列で区切って分解し取り出す。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
class StringDivide : protected StringArray
{
//メソッド
public:
///@name 生成
//@{
	StringDivide()										{}							///<コンストラクタ
	StringDivide(cpchar1 cszSource,cpchar1 cszToken)	{set(cszSource,cszToken);};	///<コンストラクタ。Set(cszStart,cszToken)になる。
	void set(cpchar1 cszSource,cpchar1 cszToken);			///<cszStartをcszTokenで区切る
//@}


	cpchar1	getAt(int iIndex)	const {return getStringArray()->getAt(iIndex);}	///<iIndexの文字列を返す
	int		getCount()			const {return getStringArray()->getCount();}	///<全体の要素数を返す

	StringArray* getStringArray()	const {return ((StringArray*)this);}
};

}	//namespace monapi2

#endif

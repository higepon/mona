/**
	@file	VariableArgument.h
	@brief	可変引数の取り扱い

	可変引数の取り扱い

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_VARIABLEARGUMENT_H
#define _MONAPI2_BASIC_VARIABLEARGUMENT_H

namespace monapi2
{

/**
	@brief	可変引数を楽に扱うためのマクロ。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
typedef	char*	vapointer;
#define VAPOINTER_SET_START(vap,lastArgument)	(void)((vap)=(vapointer)&(lastArgument)+sizeof(lastArgument))
#define VAPOINTER_ADVANCE(vap,type)				((type*)(vap+=sizeof(type)))[-1]

}		//namespace monapi2

#endif

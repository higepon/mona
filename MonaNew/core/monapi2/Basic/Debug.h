/**
	@file	Debug.h
	@brief	デバッグ

	デバッグ

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_DEBUG_H
#define _MONAPI2_BASIC_DEBUG_H

#include "switch.h"
#include "Type.h"

namespace monapi2	{

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
#define STOP {int _z=0;_z++;}

#ifndef ASSERT
	#define ASSERT(X)
#endif

#ifndef TRACE
	#define TRACE(X)
#endif

}	//namespace monapi2

#endif

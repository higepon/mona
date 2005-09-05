/**
	@file	switch.h
	@brief	コンパイルのオプションスイッチ

	コンパイルのオプションスイッチ

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_SWITCH_H
#define _MONAPI2_BASIC_SWITCH_H

namespace monapi2	{

/**
	@date	2005/08/20	junjunn 作成
*/

//コンパイラの選択。
//アセンブラの記述法が違うため場合分けしてる。
//選択：_COMPILER_VISUALC、又は#else

#ifdef _COMPILER_VISUALC
	#pragma warning(disable : 4127)	///<条件式が定数です。
	#pragma warning(disable : 4514)	///<参照されていないｲﾝﾗｲﾝ関数は削除されました。
	#pragma warning(disable : 4706)	///<条件式の比較値は、代入の結果になっています。 
	#pragma warning(disable : 4710)	///<not inlined
	#pragma warning(disable : 4725) ///<Pentium のﾓﾃﾞﾙによっては、命令が不正確になります。
	#pragma warning(disable : 4786) ///<ﾃﾞﾊﾞｯｸﾞ情報で識別子が 255 文字に切り捨てられました。
#endif	//#ifdef _COMPILER_VISUALC

}	//namespace monapi2

#endif

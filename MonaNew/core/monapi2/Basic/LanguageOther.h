/**
	@file	LanguageOther.h
	@brief	言語・文字コードツール

	言語・文字コードツール

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_LANGUAGEOTHER_H
#define _MONAPI2_BASIC_LANGUAGEOTHER_H

#include "switch.h"
#include "Type.h"
#include "Language.h"

namespace monapi2	{

#if 1

#define GENERATE_CONVERSION_CODE

/**
文字コード変換データ半自動ジェネレータ

Unicodeコンソーシアムで配られているタブ区切り形式の.txt変換表を読み
変換コードをファイルに吐き出す。
書式はCソースコード形式なのでそのまま#includeしちゃえばコードが完成する。
・・・しかし実際はデータ量が多くソースコードとしてコンパイルすると時間がかかり面倒なので
コンパイルはしない。
代わりにLanguageFn::readTableでファイルを外部データファイルとして読み込み、
必要な時にセットするのがいいと思う。


使い方：
・文字コード変換ソースコード作成

	//変換領域を初期化しておく。
		LanguageFn::initRule();
	//とりあえずオブジェクトを作る。
		CGenerateConversionRule GCC;
	//変換領域設定。
		GCC.getConversionRule2Way()->setShiftJISUnicode();
	//入力＆出力ファイル設定　→　実行
		GCC.generate("CP932.txt","Language2.cpp");


・文字コード変換ソースコードを読み込む

	//パラメーター設定＆実行
		LanguageFn::readTable("Language2.cpp");


変換に指示する領域については人間が調べて手動でセットしておく必要がある。
やり方はdumpBMPを実行するとビットマップで変換に使われる領域がわかるので
それを見て人間が四角形に切り分け、使われている領域をvoid LanguageFn::initRule()の
中に書いてセットする。

	@date	2005/08/20	junjunn 作成
*/
class CGenerateConversionCode
{
public:
///変換領域の呼び出し。
	CConversionRule2Way* getConversionRule2Way()	{return &m_ConversionRule2Way;}
///作業メイン
	bool generate(pcchar1 cszConversionTableFilePath,pcchar1 cszPathOut);


protected:
///現在のm_aIntPairの内容を解釈しCコードを吐き出す
	void generateCCode(int i1to2,String* pstrOut);
///CIntPair::m_i1で比較する。
	static int compareByCode1(const void* p,const void* q);
//変換表をBMPイメージに変換。変換表を視覚的に捕らえてどこに有効なデータがあるのか探す。
//設定したデータはsetConversionRuleの中に手動で設定。
	void dumpBMP();


public:


protected:
//メンバ
///intペア構造
	class CIntPair	{public:int m_i1,m_i2;};
///文字コード1と文字コード2の全ペアを保持する。
	CIntPair m_aIntPair[0xFFFF];
///変換領域ルール。
	CConversionRule2Way m_ConversionRule2Way;
};

#endif //#ifdef GENERATECONVIRSIONCODE

}	//namespace monapi2

#endif

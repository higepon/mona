/**
	@file	Language.h
	@brief	言語・文字コード

	言語・文字コード

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_LANGUAGE_H
#define _MONAPI2_BASIC_LANGUAGE_H

#include "switch.h"
#include "Type.h"
#include "String.h"
#include "CollectionArray.h"
#include "../MultiMedia/Rect.h"

namespace monapi2	{

/**
	@brief	文字関数を集めたstaticクラス。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
class CharFn
{
public:
//取得
	static bool isLower(char1 c);		///<cは'a'～'z'
	static bool isUpper(char1 c);		///<cは'A'～'Z'
	static bool isAlpha(char1 c);		///<cは'a'～'z'、'A'～'Z'
	static bool isAlphaDigit(char1 c);	///<cは'a'～'z'、'A'～'Z'、'0'～'9'
	static bool isCSymbol(char1 c);		///<cは'a'～'z'、'A'～'Z'、'0'～'9'、'_'
	static bool isDigit(char1 c);		///<cは'0'～'9'
	static bool isHex(char1 c);			///<cは'0'～'9'、'A'～'F'
	static bool isSpace(char1 c);		///<cは0x09-0x0D、0x20
	static bool isASCII(char1 c);		///<cは7ビットの範囲(< 0x80)か


//操作
	static byte toLower(char1 c);		///<cを小文字に変換。
	static byte toUpper(char1 c);		///<cを大文字に変換。
};


/*
ワイド⇔マルチ文字の変換
設計アイデア：

ShiftJIS←→Unicodeの変換は0xFFFFの大きさの対応表を作って全部書けば一発で終わるが
実際はほとんどの領域は使用されてないのでそのままでは無駄が大きい。
使用されている領域はおおまかに分けて四角形の形で散らばっているので
それぞれを列挙してそこだけの変換を書けばOK。
その為に以下の構造になっている。

	Rect
		1つの変換領域

	Rect g_arectConversionXXXtoYYY[]	など
		XXX→YYYにある全ての変換領域。

	class ConversionRule
		g_arectConversionXXXtoYYY + 変換の名前

	CConversionRule2Way
		XXXからYYYへのConversionRule　＋　YYYからXXXへのConversionRule。


	@date	2005/08/20	junjunn 作成
*/
class CConversionRule
{
public:
	void set(pcchar1 cszName,Rect* parectConversion)
	{
		m_strName= cszName;
		m_parectConversion = parectConversion;
	}

public:
	Rect* m_parectConversion;
	String m_strName;
};

/**
1→2、2→1の変換領域をパック
	@date	2005/08/20	junjunn 作成
*/
class CConversionRule2Way
{
public:
	void setShiftJISUnicode();

public:
	CConversionRule m_aConversionRule[2];
};


/**
ワイド⇔マルチ文字の変換。

こっちはAPIとしての存在。
実際の変換表その物はCLanguageCodeConverterがやってる。


	@date	2005/08/20	junjunn 作成
*/
class LanguageFn
{
public:
//文字列変換
	static int convertShiftJIStoUnicode(pchar2 wszOut	,pcchar1 cszIn	,int iMaxInLen=-1);	///<char → wchar
	static int convertUnicodetoShiftJIS(pchar1 szOut	,pcchar2 cwszIn	,int iMaxInLen=-1);	///<wchap → char
//class Bufferを使った変換
	static int convertShiftJIStoUnicode(class Buffer* pbufOut,pcchar1 cszIn);		///<char → wchar
	static int convertUnicodetoShiftJIS(Buffer* pbufOut,pcchar2 cwszIn);			///<wchar → char

//単文字コード変換
///SJIS文字コードからUnicode(2バイト幅形式)文字コードに変換
	static int convertShiftJIStoUnicode(int iSJISCode);
	static int convertShiftJIStoUnicode(byte x,byte y);
///Unicode(2バイト幅形式)文字コードからSJIS文字コードに変換
	static int convertUnicodetoShiftJIS(int iSJISCode);
	static int convertUnicodetoShiftJIS(byte x,byte y);

///変換コードテーブルを読み出す。
	static void init(pcchar1 cszPathShiftJIStoUnicode);
	static void initRule();
};


/**
ShiftJIS←→Unicodeなどの単文字についての変換表。
変換表を作るためにファイルからデータを一回読み出す必要がありこれがオブジェクトっぽいので
クラスにした。
LanguageFnの関数はこれに問い合わせをしている。

	@date	2005/08/20	junjunn 作成
*/
class CLanguageCodeConverter
{
public:
///コンストラクタ
	CLanguageCodeConverter()	{m_bReady=false;}
///準備が出来ているか。
	bool isReady()	{return m_bReady;}
///変換コードテーブルを読み出す。
	void readTable(pcchar1 cszPath);
///SJIS文字コードからUnicode(2バイト幅形式)文字コードに変換
	int convert1to2(int iCode1);
	int convert1to2(byte x,byte y);
///Unicode(2バイト幅形式)文字コードからSJIS文字コードに変換
	int convert2to1(int iCode2);
	int convert2to1(byte x,byte y);

//1→2、または2→1の共通化
	int convert(byte x,byte y,int i1to2);

public:
///変換領域。どこの領域に有効なデータがあるのかRectを設定する。
	CConversionRule2Way m_ConversionRule2Way;

protected:
///変換データ
	ArrayAD<word*> m_arrayPWordConversionData[2];
///準備ができているか
	bool m_bReady;
};


//スクリプトの書式に関する関数。
void getCGenerateConversionCodeTableName(pchar1 szOut,CConversionRule* pConversionRule,int iIndex);

}	//namespace monapi2

#endif

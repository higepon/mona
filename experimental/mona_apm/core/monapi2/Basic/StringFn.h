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
#ifndef _MONAPI2_BASIC_STRINGFN_H
#define _MONAPI2_BASIC_STRINGFN_H

#include "Switch.h"
#include "Type.h"
#include "VariableArgument.h"

namespace monapi2	{

///文字列関数を集めたstaticクラス
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
class StringFn
{
public:
//取得
	static inline	bool	isEmpty(cpchar1 cszSource);										///<空かどうか。
	static inline	bool	isEqual(cpchar1 cszP,cpchar1 cszQ,int iSearchCount=-1);			///<文字同一判定。
	static inline	bool	isEqualNoCase(cpchar1 cszP,cpchar1 cszQ,int iSearchCount=-1);	///IsEqualの大文字小文字関係なしバージョン。
	static			int		findDiff(cpchar1 cszP,cpchar1 cszQ,int iSearchCount=-1);			///<文字列比較。iSearchCount==-1の場合は全文比較、それ以外の場合は指定した長さまで。
	static			int		findDiffUpto(cpchar1 cszP,cpchar1 cszQ,cpchar1 cszPUpto);		///<文字列比較。cszP<cszPUptoまで。
	static			int		findDiffNoCase(cpchar1 cszP,cpchar1 cszQ,int iSearchCount=-1);	///<compareの大文字小文字関係なしバージョン。
	static			int		compare(cpchar1 cszP,cpchar1 cszQ,int iSearchCount=-1);			///<アルファベット順にどちらが前かを比較。
	static			uint	getLength(cpchar1 cszSource);									///<文字列の長さ。
	static			uint	getLength(cpcharv cszSource);									///<文字列の長さ。
	static 			pchar1	find(cpchar1 cszSource,char1 cFind,int iSearchCount=-1);		///<cszSourceのなかからcFindを見つける。
	static			pchar1	find(cpchar1 cszSource,cpchar1 cszFind,int iSearchCount=-1);	///<cszSourceのなかからcszFindを見つける。
	static			pchar1	findUpto(cpchar1 cszSource,char1 cFind,cpchar1 cszSourceUpto);	///<cszSourceのなかからcFindを見つける。
	static			pchar1	findUpto(cpchar1 cszSource,cpchar1 cszFind,cpchar1 cszSourceUpto);	///<cszSourceのなかからcszFindを見つける。
	static			pchar1	findReverse(cpchar1 cszSource,char1 cFind,int iSearchCount=-1);
	static			pchar1	findReverse(cpchar1 cszSource,cpchar1 cszFind,int iSearchCount=-1);
	static			cpchar1	getEnd(cpchar1 cszSource);
	static			int		getCountAppear(cpchar1 cszSource,char1 cFind,int iSearchCount=-1);		///<cszSourceにcFindが何回出てきたか。
	static			int		getCountAppear(cpchar1 cszSource,cpchar1 cszFind,int iSearchCount=-1);	///<cszSourceにcszFindが何回出てきたか。
	static			int		toInt(cpchar1 cszSource,int iBase=10,cpchar1* pcszEnd=NULL);			///<cszSourceから始まる文字列を数値に解釈する。cszEndは末尾を入れて貰うアドレス。
	static			float	toFloat(cpchar1 csz,cpchar1* pcszEnd=NULL);						///<cszSourceから始まる文字列を数値に解釈する。cszEndは末尾を入れて貰うアドレス。
	static inline	pchar1	findEnd(cpchar1 pAt);											///<{return pAt+strlen(pAt);}
	static			int		duplicate(pchar1* pszOut,cpchar1 cszIn);						///<ヒープにコピーを作って返す。
	static			int		getReplaceLength(cpchar1 cszIn,cpchar1 cszFrom,cpchar1 cszTo);	///<置き換え後の長さを求める。

	static inline	void	empty(pchar1 szOut);										///<空にする。
	static			int		copy(pchar1 szOut,cpchar1 cszIn,int iCopyCount=-1);			///<文字列コピー。戻り値はコピーした文字数。
	static			int		copy(pchar2 szOut,cpchar2 cszIn,int iCopyCount=-1);			///<文字列コピー。戻り値はコピーした文字数。
	static inline	int		join(pchar1 szOut,cpchar1 cszIn,int iCopyCount=-1);			///<文字列連結。戻り値はコピーした文字数。
	static			int		format(pchar1 szBuffer,cpchar1 cszFormat,...);				///<sprintf形式で文字列を作る。
	static			int		formatV(pchar1 szBuffer,cpchar1 cszFormat,vapointer vap);	///<sprintf形式で文字列を作る。
	static			int		toString(pchar1 szBuffer,int iTarget,int iBase=10,bool bCapital=false,int iWidth=0);
	static			int		replace(pchar1 szOut,char1 cFrom,char1 cTo);				///<文字置き換えchr版。szSrcには置き換え後の文字列の長さを格納できるだけのバッファがなければならない。
	static			int		replace(pchar1 szOut,cpchar1 cszIn,cpchar1 cszFrom,cpchar1 cszTo,int iFirstAppear=-1);		///<文字置き換え。
	static			int		replace(pchar1* pszOut,cpchar1 cszIn,cpchar1 cszFrom,cpchar1 cszTo);			///<文字置き換え。
	static			int		replace(class String* pstrOut,cpchar1 cszIn,cpchar1 cszFrom,cpchar1 cszTo);		///<文字置き換え。
	static			int		replace(pchar1 szInOut,cpchar1 cszFrom,cpchar1 cszTo);		///<文字置き換えchr版。szSrcには置き換え後の文字列の長さを格納できるだけのバッファがなければならない。
	static			int		remove(pchar1 szOut,char1 cRemove);							///<cRemoveをszSrcから取り除く。
	static	inline	int		remove(pchar1 szOut,cpchar1 cszRemove);						///<cszRemoveをszSrcから取り除く。
	static			void	toLower(pchar1 szSource);									///<szSource全部を小文字に変換。
	static			void	toUpper(pchar1 szSource);									///<szSource全部を大文字に変換。


//APIとしては公開していない物。
public:
//ToIntとToFloatのサブルーチン。cszSourceから始まる文字列をintに直し数字文字列の末尾をpstrEndに入れる。
	static int getIntAt(cpchar1 cszSource,int iBase,cpchar1* pcszEnd);
	static int estimateVALength(cpchar1 cszFormat,vapointer vap);		///<sprintf形式で文字列を作る。
	static char toString(int iTarget,bool bCapital=false);				///<0～15を'0'～'9'、'A'～'F'にする。
	static void toStringFloatPlain(pchar1 szBuffer,double d);			////<dを文字列にする。1.234e+45とかの形式ではなく1.234とかのシンプルな形だけ。大きい桁は表示無理・・・
};


}	//namespace monapi2

#include "StringFnInline.h"

#endif

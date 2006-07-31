/**
	@file	LanguageOther.cpp
	@brief	言語・文字コードツール

	言語・文字コードツール

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "LanguageOther.h"
#include "StringFn.h"
#include "FileFn.h"
#include "StringOther.h"

#ifdef GENERATE_CONVERSION_CODE
	#include "../MultiMedia/Bitmap.h"
#endif

namespace monapi2
{

#ifdef GENERATE_CONVERSION_CODE

/**
	@brief	.h参照。
	@date	2005/08/20	junjunn 作成
*/
bool CGenerateConversionCode::generate(cpchar1 cszConversionTableFilePath,cpchar1 cszPathOut)
{
//ファイルを読む
	String strFileContent;
	if (! FileFn::read(cszConversionTableFilePath,&strFileContent)) return false;

//行に分割
	StringDivide SDLine(strFileContent,"\n");

	int i;
//変換テーブルの用意
	for (i=0;i<0xFFFF;i++)	
	{
		m_aIntPair[i].m_i1=i;
		m_aIntPair[i].m_i2=0;
	}

//全行査定
	for (uint iLine=0;iLine<SDLine.getCount();iLine++)
	{
//タブで分割
		StringDivide Sdword(SDLine.getAt(iLine),"	");

//最初はコード1
		cpchar1 cszCode1= Sdword.getAt(0);
		int iCode1	=StringFn::toInt(cszCode1+2,16);

//有効ではない行。
		if (cszCode1[0] != '0') continue;		//行頭は0xで始まるので

//次はコード2
		cpchar1 cszCode2 = Sdword.getAt(1);
		int iCode2;

//定義されていない場所には空白が書かれてある。それは無効。		
		if (cszCode2[0] != ' ')		iCode2=StringFn::toInt(cszCode2+2,16);
		else						iCode2=0;

//変換テーブルに入れる。
//		ASSERT(iCode1<0xFFFF);
		m_aIntPair[iCode1].m_i2 = iCode2;
	}

//変換表を色に直してBitmapにする。これを見て大幅な戦略を練る。
//	dumpBMP();return false;

//最終的に吐き出す全コード
	String strCodeWhole;

//ヘッダー
	strCodeWhole=
		"/**\n"
		"	@file	Language2.cpp\n"
		"	@brief	class CGenerateConversionCodeで自動作成されたShiftJIS←→Unicode変換のC++ファイル\n"
		"\n"
		"	class CGenerateConversionCodeで自動作成されたShiftJIS←→Unicode変換のC++ファイル\n"
		"\n"
		"	License=Mona License\n"
		"	@version $Revision$\n"
		"	@date	$Date$\n"
		"*/\n"
		"\n"
		"#include \"Language.h\"\n"
		"\n"
		"namespace monapi2	{\n"
		"\n"
		"//このファイルはコンパイルに時間かかるので実際はしない。代わりにデータファイルとして読み出す。\n"
		"#if 0\n\n";


//1→2の変換テーブル
	generateCCode(0,&strCodeWhole);

//コード1と2を交換
	for (i=0;i<0xFFFF;i++)
	{
		int iI2Copy=m_aIntPair[i].m_i2;
		m_aIntPair[i].m_i2=m_aIntPair[i].m_i1;
		m_aIntPair[i].m_i1=iI2Copy;
	}
//(元)コード2で並び替え
	quicksort(m_aIntPair,0xFFFF,sizeof(CIntPair),compareByCode1);

//2→1の変換テーブル
	generateCCode(1,&strCodeWhole);

//フッター
	strCodeWhole+="\n\n#endif	//#if 0\n\n}	//namespace monapi2\n";

//出力
	FileFn::write(cszPathOut,&strCodeWhole);

	return true;
}

/**
	@brief	.h参照。
	@date	2005/08/20	junjunn 作成
*/
void CGenerateConversionCode::dumpBMP()
{
	Bitmap bitmap;
	bitmap.createCanvas(0x100,0x100);

//真っ黒に塗りつぶす。
	bitmap.fill(0);

	for (int i=0x0;i<0xFFFF;i++)
	{
		int iCode1=m_aIntPair[i].m_i1;
		int iCode2=m_aIntPair[i].m_i2;

		if (iCode2>0)
		{
			colort color=(0x00FF0000+(iCode2));
			bitmap.setPixel(((iCode1&0xFF00)>>8),iCode1&0xFF,color);
		}
	}

	bitmap.write("test.bmp");
}

/**
	@brief	.h参照。
	@date	2005/08/20	junjunn 作成
*/
void CGenerateConversionCode::generateCCode(int i1to2,String* pstrOut)
{
	CConversionRule* pConversionRule = &m_ConversionRule2Way.m_aConversionRule[i1to2];

//複数のテーブルに分けて処理
	{
		pstrOut->formatJoinSelf(
			"int LanguageFn::convert%s(byte x,byte y)\n"
			"{\n",
			pConversionRule->m_strName.getString());


		char szTemp[256];
//全ての領域を巡回し
		for (int i=0;;i++)
		{
			Rect* pRect = &pConversionRule->m_parectConversion[i];
			if (pRect->getLeft() < 0)	break;	//番兵

			StringFn::format(szTemp,
				"	if (0x%2X<=x&&x<0x%2X && 0x%2X<=y&&y<0x%2X)\n	{\n",
				pRect->getLeft(),pRect->getRight(),pRect->getTop(),pRect->getBottom());
			*pstrOut +=	szTemp;

			char szTableName[100];
			getCGenerateConversionCodeTableName(szTableName,pConversionRule,i);

			StringFn::format(szTemp,"		vchar %s[]={\n			",szTableName);
			*pstrOut +=	szTemp;

			int iElementOutCount=0;	//コード2文字列をいくつ吐き出したか。
			for (int x=pRect->getLeft();x<pRect->getRight();x++)
			{
				for (int y=pRect->getTop();y<pRect->getBottom();y++)
				{
					int iCode2=m_aIntPair[y + 0x100*x].m_i2;
					StringFn::format(szTemp,"0x%4X,",iCode2);
					(*pstrOut) +=	szTemp;

//一定数で改行までする
					if (++iElementOutCount==8)
					{
						(*pstrOut)+="\n";
						if (x!=pRect->getRight()-1 || y!=pRect->getBottom()-1)	//最後じゃないなら
						{
							StringFn::format(szTemp,"			");
							(*pstrOut)+=szTemp;
							iElementOutCount=0;
						}
					}
				}
			}

			StringFn::format(szTemp,
				"		};\n"
				"		return %s[(y-0x%2X) + 0x%2X*(x-0x%2X)];\n	}\n\n",
				szTableName,pRect->getLeft(),pRect->getHeight(),pRect->getTop());
			(*pstrOut) +=	szTemp;
		}
	}

	*pstrOut +=
		"	return 0;\n"
		"}\n";
}

/**
	@brief	.h参照。
	@date	2005/08/20	junjunn 作成
*/
int CGenerateConversionCode::compareByCode1(const void* p,const void* q)
{
	CIntPair* pIntPairP=(CIntPair*)p;
	CIntPair* pIntPairQ=(CIntPair*)q;

	return pIntPairP->m_i1-pIntPairQ->m_i1;
}

#endif //#ifdef GENERATE_CONVERSION_CODE

}	//namespace monapi2

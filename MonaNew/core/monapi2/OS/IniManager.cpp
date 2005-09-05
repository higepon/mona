/**
	@file	IniManager.h
	@brief	.ini形式のデータのファイルへの入出力

	.ini形式のデータのファイルへの入出力

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "IniManager.h"
#include "../Basic/StringFn.h"
#include "../Basic/StringOther.h"
#include "../Basic/FileFn.h"

namespace monapi2	{

/**
IniManager::Write()の時のソートに使う

	@date	2005/08/20	junjunn 作成
*/
class CDirKeyValue
{
public:
	String m_strDir;
	String m_strKey;
	String m_strValue;
};

/**
	@date	2005/08/20	junjunn 作成
*/
int compareDirKeyValue(const void *p, const void *q)
{
	CDirKeyValue* pDKV1=(CDirKeyValue*)p;
	CDirKeyValue* pDKV2=(CDirKeyValue*)q;
	return (StringFn::compare(pDKV1->m_strDir,pDKV2->m_strDir));
}


//IniManager/////////////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool IniManager::isLineComment(pcchar1 p)
{
	if (StringFn::isEmpty(p))		return true;

	char1 c0 = p[0];
	char1 c1 = p[1];

	if (c0=='/'  && c1=='/')		return true;
	if (c0=='#')					return true;
	if (c0=='\n')					return true;

	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void IniManager::read(pcchar1 cszPath)
{
//ファイルから読み出し。
	String str;
	if (! FileFn::read(cszPath,&str))		return;

//整形。
	str.remove('\r');

//解析
	parse(str);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void IniManager::parse(pcchar1 cszContent)
{
	m_strCurDir.empty();

//行に分割
	StringDivide SDLine(cszContent,"\n");
//行を巡回	
	for (int i=0;i<SDLine.getCount();i++)
	{
//現在の行。
		pcchar1 pLine = SDLine.getAt(i);
//コメント行ならスキップ
		if (isLineComment(pLine))		continue;

//ディレクトリ指定の行らしい
		if (pLine[0]=='[')
		{
//ディレクトリ名の初めと終わり。
			pcchar1 pStart = pLine+1;
			pcchar1 pEnd = StringFn::find(pLine,']');
			if (pEnd==NULL)	pEnd = StringFn::getEnd(pStart);	//閉じカッコがないのはおかしいがなんとか修正する。
			m_strCurDir.copy(pStart,pEnd-pStart);
		}
//値
		else
		{
//Key=Value形式を区切る。
			StringDivide SDKeyValue(pLine,"=");
//キー名
			String strKey=SDKeyValue.getAt(0);
//先頭の空白を取り除く。
			strKey.remove(" ");		//空白
			strKey.remove("	");		//タブ

//値
			String strValue;
			if (SDKeyValue.getCount()>=2)
			{
//	strTest= "a=b";
//など値にも=が入ってしまっている困ったケースを考慮して残りを一つにくっつける。
				String strValueUnified=SDKeyValue.getAt(1);
				for (int i=2;i<SDKeyValue.getCount();i++)
				{
					strValueUnified+="=";
					strValueUnified+=SDKeyValue.getAt(i);
				}

//先頭を見つける。
				pcchar1 pStart = strValueUnified;
//先頭の空白をスキップ。
				while (*pStart==' ' || *pStart=='	')	pStart++;

//最後を見つける。
				pcchar1 pEnd=pStart;
//="～"形式だったら対応する"を見つける。
				if (*pStart=='"')
				{
					pStart++;
					pEnd=StringFn::find(pStart,'"');
					if (pEnd==NULL)	pEnd = StringFn::getEnd(pStart+1);	//閉じカッコがないのはおかしいがなんとか修正する。
				}
//=～形式だったら;か行の終わりを見つける。
				else
				{
					while (! (*pEnd==';' || *pEnd=='\0'))	pEnd++;

				}

//値を取り出す。
				int iLen=pEnd-pStart;
				pchar1 pBuffer=(pchar1)strValue.extendBuffer(iLen+1);
				StringFn::copy(pBuffer,pStart,iLen);
			}

//ディレクトリ＋キー名で作るフルキー名。
			String strKeyWhole;
			strKeyWhole.format("%s/%s",m_strCurDir.getString(),strKey.getString());
			m_strstrmap.setAt(strKeyWhole,strValue);
		}
	}
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void IniManager::toString(String* pstrOut)
{
	pstrOut->empty();
	int iCount=m_strstrmap.getCount();
	if (iCount==0)		return;

//ソートするため配列に置き換える
	CDirKeyValue* paDKV=new CDirKeyValue[iCount];

	int i=0;
	mapposition pos = m_strstrmap.getStartPosition();
	String strKey,strValue;
	while(pos)
	{
		m_strstrmap.getNext(&pos,&strKey,&strValue);
		StringDivide SD(strKey,"/");
		paDKV[i].m_strDir=SD.getAt(0);
		paDKV[i].m_strKey=SD.getAt(1);
		paDKV[i].m_strValue=strValue;
		i++;
	}

//ソート
	quicksort(paDKV,iCount,sizeof(CDirKeyValue),compareDirKeyValue);

//文字列に出力
	String strLine;
	pcchar1 cszLastDir="";
	for (i=0;i<iCount;i++)
	{
//新しいディレクトリになったらディレクトリセクションを挿入。
		if (! StringFn::isEqual(cszLastDir,paDKV[i].m_strDir))
		{
			strLine.format("[%s]\n",paDKV[i].m_strDir.getString());
			*pstrOut+=strLine;
			cszLastDir = paDKV[i].m_strDir;
		}
		strLine.format("%s=%s\n",paDKV[i].m_strKey.getString(),paDKV[i].m_strValue.getString());
		*pstrOut+=strLine;
	}

	delete[] paDKV;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void IniManager::write(pcchar1 cszPath)
{
	String str;
	toString(&str);
	FileFn::write(cszPath,&str);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool IniManager::lookup(pcchar1 cszDir,pcchar1 cszKey,bool bDefault)
{
	pcchar1 p=lookup(cszDir,cszKey,"");
	return (StringFn::isEmpty(p))?bDefault:(StringFn::toInt(p)!=0);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
int	IniManager::lookup(pcchar1 cszDir,pcchar1 cszKey,int iDefault)
{
	pcchar1 p=lookup(cszDir,cszKey,"");
	return (StringFn::isEmpty(p))?iDefault:StringFn::toInt(p);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
float IniManager::lookup(pcchar1 cszDir,pcchar1 cszKey,float fDefault)
{
	pcchar1 p=lookup(cszDir,cszKey,"");
	return (StringFn::isEmpty(p))?fDefault:(float)StringFn::toFloat(p);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
pcchar1	IniManager::lookup(pcchar1 cszDir,pcchar1 cszKey,pcchar1 cszDefault)
{
	String	strFullKey;
	strFullKey.format("%s/%s",cszDir,cszKey);

	if (m_strstrmap.lookup(strFullKey,&m_strValueBuffer))
	{
		return m_strValueBuffer;
	}

	return cszDefault;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void IniManager::setAt(pcchar1 cszDir,pcchar1 cszKey,bool bValue)
{
	setAt(cszDir,cszKey,bValue);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void IniManager::setAt(pcchar1 cszDir,pcchar1 cszKey,int iValue)
{
	char1 szValue[32];
	StringFn::toString(szValue,iValue);

	setAt(cszDir,cszKey,szValue);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void IniManager::setAt(pcchar1 cszDir,pcchar1 cszKey,float fValue)
{
	char1 szValue[32];
	StringFn::format(szValue,"%f",fValue);

	setAt(cszDir,cszKey,szValue);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void IniManager::setAt(pcchar1 cszDir,pcchar1 cszKey,pcchar1 cszValue)
{
	String strFullKey;
	strFullKey.format("%s/%s",cszDir,cszKey);

	m_strstrmap.setAt(strFullKey,cszValue);
}

}	//namespace monapi2

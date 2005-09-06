/**
	@file	File.cpp
	@brief	ファイル詳細操作

	ファイル詳細操作

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "File.h"

namespace monapi2	{

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
const char* File::getName() const
{
	return NULL;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
const char* File::getPath() const
{
	return NULL;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
int File::getSize() const
{
	return 0;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
int File::getLastModifiedTime() const
{
	return 0;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool File::open(const char* cszPath,bool bAllowWrite)
{
#ifndef MONA
	const char* pOpenFlag = (bAllowWrite)?"wb":"rb";
	m_pFile = fopen(cszPath,pOpenFlag);
	return (m_pFile!=NULL);
#endif

	cszPath=NULL, bAllowWrite=false;
	return 0;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
uint File::read(byte* pOut,uint nIndex,uint nCount)
{
#ifndef MONA
	fseek(m_pFile,nIndex,SEEK_SET);
	return fread(pOut,sizeof(byte),nCount,m_pFile);
#endif

	pOut=NULL,nIndex=0,nCount=0;
	return 0;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
uint File::write(const byte* cpIn,uint nIndex,uint nCount) const
{
#ifndef MONA
	fseek(m_pFile,nIndex,SEEK_SET);
	return fwrite(cpIn,sizeof(byte),nCount,m_pFile);
#endif

	cpIn=NULL,nCount=0;nIndex=0;
	return 0;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void File::close()
{
#ifndef MONA
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile=NULL;
	}
#endif
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void File::setPath(const char )
{
}


}		//namespace monapi2

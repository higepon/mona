/**
	@file	FileFn.cpp
	@brief	ファイル一発操作

	ファイル一発操作

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "FileFn.h"
#include "Buffer.h"
#include "String.h"

//デバッグのための応急措置
#ifdef _WIN32
	#include <afxwin.h>
	#include <io.h>
#endif

namespace monapi2	{

//FileFn//////////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::isExist(pcchar1 cszPath)
{
	cszPath;
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::isFile(pcchar1 cszPath)
{
	cszPath;
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
int FileFn::getSize(pcchar1 cszPath)
{
#ifdef _WIN32
	struct _finddata_t FindData;
	if (_findfirst(cszPath,&FindData)!=-1)
		return FindData.size;
	else
		return -1;
#endif

	return -1;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
uint FileFn::getLastModifiedTime(pcchar1 cszPath)
{
	cszPath;
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::read(pcchar1 cszPath,byte* pBufferOut,int iBufferSize)
{
//デバッグのための応急措置
#ifdef _WIN32
	FILE* p=fopen(cszPath,"r");
	fread(pBufferOut,sizeof(byte),iBufferSize,p);
	fclose(p);
	return true;
#endif

	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::read(pcchar1 cszPath,Buffer* pbufOut)
{
	int iFileSize = getSize(cszPath);
	if (iFileSize!=-1)
	{
		byte* pBuffer = pbufOut->extendBuffer(iFileSize);
		read(cszPath,pBuffer,iFileSize);
		return true;
	}

	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::read(pcchar1 cszPath,String* pstrOut)
{
	int iFileSize = getSize(cszPath);
	if (iFileSize!=-1)
	{
		pchar1 pBuffer = pstrOut->extendBuffer(iFileSize+1);
		read(cszPath,(byte*)pBuffer,iFileSize);
		pBuffer[iFileSize] = '\0';
		return true;
	}

	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::write(pcchar1 cszPath,const byte* cpIn,int iCount)
{
//デバッグのための応急措置
#ifdef _WIN32
	FILE* p=fopen(cszPath,"wb");
	fwrite(cpIn,sizeof(byte),iCount,p);
	fclose(p);
	return true;
#endif

	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::write(pcchar1 cszPath,const Buffer* cpbufIn)
{
	return write(cszPath,cpbufIn->getData(),cpbufIn->getSize());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::write(pcchar1 cszPath,const String* cpstrIn)
{
	return write(cszPath,(byte*)cpstrIn->getString(),cpstrIn->getLength());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::createDirectory(pcchar1 cszPath,pcchar1 cszDirName)
{
	cszDirName;
	cszPath;
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::deleteFile(pcchar1 cszPath)
{
	cszPath;
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::deleteDirectory(pcchar1 cszPath)
{
	cszPath;
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::move(pcchar1 cszPathFrom,pcchar1 cszPathTo)
{
	cszPathFrom;
	cszPathTo;
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::copy(pcchar1 cszPathFrom,pcchar1 cszPathTo)
{
	cszPathFrom;
	cszPathTo;
	return false;
}


}		//namespace monapi2

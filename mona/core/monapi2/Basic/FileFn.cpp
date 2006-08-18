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
#ifdef MONA
	#include <monapi.h>
	#include <monapi/messages.h>
#endif

#ifdef _WIN32
	#include <afxwin.h>
	#include <io.h>
	#include <sys/types.h>
	#include <sys/stat.h>
#endif

#include "FileFn.h"
#include "Buffer.h"
#include "String.h"
#include "Memory.h"

//デバッグのための応急措置

namespace monapi2	{

//FileFn//////////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::isExist(cpchar1 /*cszPath*/)
{
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::isFile(cpchar1 /*cszPath*/)
{
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
int FileFn::getSize(cpchar1 cszPath)
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
uint FileFn::getLastModifiedTime(cpchar1 /*cszPath*/)
{
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
	@date	2005/09/20	junjunn 実装
*/
bool FileFn::read(cpchar1 cszPath,byte* pBufferOut,int iBufferSize)
{
#ifdef MONA
	monapi_cmemoryinfo* pMemoryInfo = monapi_file_read_all(cszPath);
	if (! pMemoryInfo)	return false;

	MemoryFn::copy(pBufferOut,pMemoryInfo->Data,pMemoryInfo->Size);
	return true;
#else
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
	@date	2005/09/20	junjunn 実装
*/
bool FileFn::read(cpchar1 cszPath,Buffer* pbufOut)
{
#ifdef MONA
	monapi_cmemoryinfo* pMemoryInfo = monapi_call_file_read_data(cszPath,MONAPI_FALSE);
	if (! pMemoryInfo)	return false;

	pbufOut->copy(pMemoryInfo->Data,pMemoryInfo->Size);
	return true;
#else
	pbufOut;cszPath;
	return false;
#endif
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
	@date	2005/09/20	junjunn 実装
*/
bool FileFn::read(cpchar1 cszPath,String* pstrOut)
{
#ifdef MONA
	monapi_cmemoryinfo* pMemoryInfo = monapi_call_file_read_data(cszPath,MONAPI_FALSE);
	if (! pMemoryInfo)	return false;

	int iFileSize = pMemoryInfo->Size;
//String系はサービスで'\0'まで確保してつける。
	pchar1 pBuffer = pstrOut->extendBuffer(iFileSize+1);
	MemoryFn::copy(pBuffer,pMemoryInfo->Data,pMemoryInfo->Size);
	pBuffer[iFileSize] = '\0';
	return true;
#else
	FILE* pFile = fopen(cszPath,"rb");
	if (pFile)
	{
		struct _stat oStat;
		_stat(cszPath,&oStat);
		int iFileSize=oStat.st_size;
		pchar1 pBuffer = pstrOut->extendBuffer(iFileSize+1);
		fread(pBuffer,1,iFileSize,pFile);
		pBuffer[iFileSize] = '\0';
		fclose(pFile);
		return true;
	}

	return false;
#endif
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::write(cpchar1 cszPath,const byte* cpIn,int iCount)
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
bool FileFn::write(cpchar1 cszPath,const Buffer* cpbufIn)
{
	return write(cszPath,cpbufIn->getData(),cpbufIn->getSize());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::write(cpchar1 cszPath,const String* cpstrIn)
{
	return write(cszPath,(byte*)cpstrIn->getString(),cpstrIn->getLength());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::createDirectory(cpchar1 /*cszPath*/,cpchar1 /*cszDirName*/)
{
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::deleteFile(cpchar1 /*cszPath*/)
{
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::deleteDirectory(cpchar1 /*cszPath*/)
{
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::move(cpchar1 /*cszPathFrom*/,cpchar1 /*cszPathTo*/)
{
	return false;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool FileFn::copy(cpchar1 /*cszPathFrom*/,cpchar1 /*cszPathTo*/)
{
	return false;
}


}		//namespace monapi2
